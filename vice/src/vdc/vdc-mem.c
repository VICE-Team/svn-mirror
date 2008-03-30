/*
 * vdc-mem.c - Memory interface for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner   (markus@brenner.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdlib.h>

#include "vdc.h"
#include "vdc-mem.h"



static void vdc_perform_fillcopy(void)
{
    int ptr, ptr2;
    int i;
    int blklen;

    /* Word count, # of bytes to copy */
    blklen = vdc.regs[30] ? vdc.regs[30] : 1; 

    ptr2 = (vdc.regs[32] << 8) + vdc.regs[33]; /* block start address */
    ptr  = (vdc.regs[18] << 8) + vdc.regs[19]; /* update address */

    if (vdc.regs[24] & 0x80) /* COPY flag */
    {
/*
        log_message(vdc.log, "blockcopy: blklen = %x, ptr = %x", blklen, ptr);
*/
        for (i = 0; i < blklen; i++)
            vdc.ram[ptr + i] = vdc.ram[ptr2 + i];
        ptr2 += blklen;
        vdc.regs[32] = (ptr2 >> 8) & 0xff;
        vdc.regs[33] = ptr2 & 0xff;
    }
    else
    {
/*
        log_message(vdc.log, "memset: vdcram = %x, len = %x", ptr, blklen);
*/
        memset (vdc.ram + ptr, vdc.regs[31], blklen);
    }
    ptr += blklen;
    vdc.regs[18] = (ptr >> 8) & 0xff;
    vdc.regs[19] = ptr & 0xff;
    vdc.regs[30] = 0;
}



/* VDC interface functions. */

/* Store a value in a VDC register. */
void REGPARM2 store_vdc(ADDRESS addr, BYTE value)
{
/*
    log_message(vdc.log, "store: addr = %x, byte = %x", addr, value);
*/
    /* $d600 sets the internal vdc address pointer */
    if ((addr & 1) == 0)
    {
        vdc.update_reg = value;
        return;
    }

    /* $d601 sets the vdc register indexed by the update register pointer */
    vdc.regs[vdc.update_reg] = value;

    switch (vdc.update_reg)
    {
      case 0:			/* R00  Horizontal total (characters + 1) */
/*	new_vdc_cycles_per_line = vdc[0] + 1; */
        break;

      case 1:			/* R01  Horizontal characters displayed */
/*
	if (!vdc[1])
	    return;
	new_memptr_inc = vdc[1];
        if (hw_double_cols) {
            new_memptr_inc *= 2;
        }
*/
	/* catch screens to large for our text cache */
/*
	new_memptr_inc = vdc_min( SCREEN_MAX_TEXTCOLS, new_memptr_inc );
*/
        break;

      case 2:			/* R02  Horizontal Sync Position */
	break;

      case 3:			/* R03  Horizontal/Vertical Sync widths */
	break;

      case 4:			/* R04  Vertical total (character) rows */
/*
        new_vdc_vertical_total = vdc[4] + 1;
*/
	break;

      case 5:			/* R05  Vertical total line adjust */
/*        new_vdc_vertical_adjust = vdc[5] & 0x1f; */
	break;

      case 6:			/* R06  Number of display lines on screen */
/*        new_vdc_screen_textlines = vdc[6] & 0x7f; */
        break;

      case 7:			/* R07  Vertical sync position */
	break;

      case 8:			/* R08  unused: Interlace and Skew */
	break;

      case 9:			/* R09  Rasters between two display lines */
/*	new_screen_charheight = vdc_min(8, vdc[9] + 1); */
	break;

      case 10:			/* R10  Cursor Mode, Start Scan */
/*
        crsrstart = value & 0x1f;
        value = ((value >> 5) & 0x03) ^ 0x01;
        if (crsr_enable && (crsrmode != value)) {
          crsrmode = value;
          crsrstate = 1;
          crsrcnt = 16;
	  crsr_set_dirty();
        }
*/
	break;

      case 11:			/* R11  Cursor (not implemented on the PET) */
/*
	crsr_set_dirty();
        crsrend = value & 0x1f;
	crsr_set_dirty();
*/
	break;

      case 12:			/* R12  Display Start Address hi */
      case 13:			/* R13  Display Start Address lo */
        vdc.screen_adr = ((vdc.regs[12] << 8) | vdc.regs[13]) & VDC_ADDRESS_MASK;
        log_message(vdc.log,"update screen_adr: %x", vdc.screen_adr);
	break;

      case 14:
/*
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x00ff) | ((value << 8) & 0x3f00)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
*/
	break;

      case 15:			/* R14-5 Cursor location HI/LO -- unused */
/*
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x3f00) | (value & 0xff)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
*/
	break;

      case 16:			/* R16/17 Light Pen hi/lo */
      case 17:
	break;

      case 18:			/* R18/19 Update Address hi/lo */
      case 19:
        vdc.update_adr = ((vdc.regs[18] << 8) | vdc.regs[19]) & VDC_ADDRESS_MASK;
	break;

      case 20:			/* R20/21 Attribute Start Address hi/lo */
      case 21:
        vdc.attribute_adr = ((vdc.regs[20] << 8) | vdc.regs[21]) & VDC_ADDRESS_MASK;
        log_message(vdc.log,"update attribute_adr: %x", vdc.attribute_adr);
	break;

      case 25:
        vdc.raster.video_mode = (vdc.regs[25] & 0x80) ? VDC_BITMAP_MODE : VDC_TEXT_MODE;
        break;

      case 28:
        vdc.chargen_adr = (vdc.regs[28] << 8) & 0xf000; 
        log_message(vdc.log,"update chargen_adr: %x", vdc.chargen_adr);
        break;

      case 30:			/* Word Count */
        vdc_perform_fillcopy();
        break;
      case 31:			/* Data */
        vdc_perform_fillcopy();
        break;

      case 32:			/* R32/33 Block Start Address hi/lo */
      case 33:
        break;
    }
}



BYTE REGPARM1 read_vdc(ADDRESS addr)
{
    if (addr & 1)
    {
/*
    	log_message(vdc.log, "read: addr = %x", addr);
*/
        if (vdc.update_reg == 31)
/*
            return vdc.ram[vdc.update_adr];
*/
            return vdc.ram[(vdc.regs[18] << 8) + vdc.regs[19]];
        return ((vdc.update_reg < 37) ? vdc.regs[vdc.update_reg] : 0xff);
    }
    else
        return 0x9f;
}
