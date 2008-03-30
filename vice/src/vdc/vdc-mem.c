/*
 * vdc-mem.c - Memory interface for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner   <markus@brenner.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <string.h>

#include "types.h"
#include "vdc-mem.h"
#include "vdc.h"
#include "vdctypes.h"



static void vdc_write_data(void)
{
    int ptr;

    /* Update address.  */
    ptr = (vdc.regs[18] << 8) + vdc.regs[19];

    /* Write data byte to update address. */
    vdc.ram[ptr & vdc.vdc_address_mask] = vdc.regs[31];
    ptr += 1;
    vdc.regs[18] = (ptr >> 8) & 0xff;
    vdc.regs[19] = ptr & 0xff;
}


static void vdc_perform_fillcopy(void)
{
    int ptr, ptr2;
    int i;
    int blklen;

    /* Word count, # of bytes to copy */
    blklen = vdc.regs[30] ? vdc.regs[30] : 256;

    /* Update address.  */
    ptr = (vdc.regs[18] << 8) + vdc.regs[19];

    if (vdc.regs[24] & 0x80) { /* COPY flag */
        /* Block start address.  */
        ptr2 = (vdc.regs[32] << 8) + vdc.regs[33];
        /*log_message(vdc.log, "Blockcopy: src = %04x, dest = %04x, len = %02x,"
                    " data = %02x.", ptr2, ptr, blklen,
                    vdc.ram[ptr2 & vdc.vdc_address_mask]);*/
        for (i = 0; i < blklen; i++) {
            vdc.ram[(ptr + i) & vdc.vdc_address_mask]
                = vdc.ram[(ptr2 + i) & vdc.vdc_address_mask];
            /*log_message(vdc.log, "Copy %04x -> %04x %02x", ptr2 + i, ptr + i,
                        vdc.ram[(ptr2 + i) & vdc.vdc_address_mask]);*/
        }
        ptr2 += blklen;
        vdc.regs[31] = vdc.ram[(ptr2 - 1) & vdc.vdc_address_mask];
        vdc.regs[32] = (ptr2 >> 8) & 0xff;
        vdc.regs[33] = ptr2 & 0xff;
    } else {
        /*log_message(vdc.log, "Memset: dest = %04x, len = %02x.",
        ptr, blklen); */
        for (i = 0; i < blklen; i++)
            vdc.ram[(ptr + i) & vdc.vdc_address_mask] = vdc.regs[31];
    }

    ptr = ptr + blklen;
    vdc.regs[18] = (ptr >> 8) & 0xff;
    vdc.regs[19] = ptr & 0xff;
}



/* VDC interface functions. */

/* Store a value in a VDC register. */
void REGPARM2 vdc_store(ADDRESS addr, BYTE value)
{


    /*log_message(vdc.log, "store: addr = %x, byte = %x", addr, value);*/

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
        switch (value & 0x60) {
          case 0x00:
          vdc.cursor_visible = 1;
          vdc.cursor_frequency = 0;
          break;
          case 0x20:
          vdc.cursor_visible = 0;
          vdc.cursor_frequency = 0;
          break;
          case 0x40:
          vdc.cursor_visible = 1;
          vdc.cursor_frequency = 16;
          break;
          case 0x60:
          vdc.cursor_visible = 1;
          vdc.cursor_frequency = 32;
          break;
        }
        break;

      case 11:			/* R11  Cursor */
        break;

      case 12:			/* R12  Display Start Address hi */
      case 13:			/* R13  Display Start Address lo */
        vdc.screen_adr = ((vdc.regs[12] << 8) | vdc.regs[13])
                         & vdc.vdc_address_mask;
        /*log_message(vdc.log,"Update screen_adr: %x.", vdc.screen_adr);*/
        break;

      case 14:
      case 15:                  /* R14-5 Cursor location HI/LO */
        vdc.crsrpos = ((vdc.regs[14] << 8) | vdc.regs[15])
                      & vdc.vdc_address_mask;;
        vdc.regs[24] &= 0x7f;
        break;

      case 16:			/* R16/17 Light Pen hi/lo */
      case 17:
        break;

      case 18:			/* R18/19 Update Address hi/lo */
      case 19:
        vdc.update_adr = ((vdc.regs[18] << 8) | vdc.regs[19])
                         & vdc.vdc_address_mask;
        break;

      case 20:			/* R20/21 Attribute Start Address hi/lo */
      case 21:
        vdc.attribute_adr = ((vdc.regs[20] << 8) | vdc.regs[21])
                            & vdc.vdc_address_mask;
        /*log_message(vdc.log,"Update attribute_adr: %x.", vdc.attribute_adr);*/
        break;

      case 24:
        if (value & 0x20)
          vdc.text_blink_frequency = 32;
        else
          vdc.text_blink_frequency = 16;
        break;

      case 25:
        /*log_message(vdc.log, "Color source: %s.",
                    (vdc.regs[25] & 0x40) ? "attribute space" : "register 26");
        */
        vdc.raster.video_mode = (vdc.regs[25] & 0x80) 
                                ? VDC_BITMAP_MODE : VDC_TEXT_MODE;
        break;

      case 26:
        /*log_message(vdc.log, "Color register %x.", vdc.regs[26]);*/
        break;

      case 28:
        vdc.chargen_adr = ((vdc.regs[28] << 8) & 0xe000)
                          & vdc.vdc_address_mask;
        /*log_message(vdc.log, "Update chargen_adr: %x.", vdc.chargen_adr);*/
        break;

      case 30:			/* Word Count */
        vdc_perform_fillcopy();
        break;

      case 31:			/* Data */
        vdc_write_data();
        break;

      case 32:			/* R32/33 Block Start Address hi/lo */
      case 33:
        break;
    }
}



BYTE REGPARM1 vdc_read(ADDRESS addr)
{
    if (addr & 1) {

    	/*og_message(vdc.log, "read: addr = %x", addr);*/

        if (vdc.update_reg == 31) {
            BYTE retval;
            int ptr;

            retval = vdc.ram[((vdc.regs[18] << 8) + vdc.regs[19])
                     & vdc.vdc_address_mask];
            ptr = (1 + vdc.regs[19] + (vdc.regs[18] << 8))
                  & vdc.vdc_address_mask;
            vdc.regs[18] = (ptr >> 8) & 0xff;
            vdc.regs[19] = ptr & 0xff;
            return retval;
        }
        return ((vdc.update_reg < 37) ? vdc.regs[vdc.update_reg] : 0xff);
    }
    else
        return 0x9f;
}
