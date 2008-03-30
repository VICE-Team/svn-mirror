/*
 * vdc-mem.c - Memory interface for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner   <markus@brenner.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "machine.h"
#include "maincpu.h"
#include "types.h"
#include "vdc-mem.h"
#include "vdc.h"
#include "vdctypes.h"

/*#define REG_DEBUG*/

static void vdc_write_data(void)
{
    int ptr;

    /* Update address.  */
    ptr = (vdc.regs[18] << 8) + vdc.regs[19];

    /* Write data byte to update address. */
    vdc.ram[ptr & vdc.vdc_address_mask] = vdc.regs[31];
#ifdef REG_DEBUG
    log_message(vdc.log, "STORE %04x %02x", ptr & vdc.vdc_address_mask,
                vdc.regs[31]);
#endif
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
        for (i = 0; i < blklen; i++) {
            vdc.ram[(ptr + i) & vdc.vdc_address_mask]
                = vdc.ram[(ptr2 + i) & vdc.vdc_address_mask];
        }
        ptr2 += blklen;
        vdc.regs[31] = vdc.ram[(ptr2 - 1) & vdc.vdc_address_mask];
        vdc.regs[32] = (ptr2 >> 8) & 0xff;
        vdc.regs[33] = ptr2 & 0xff;
    } else {
#ifdef REG_DEBUG
        log_message(vdc.log, "Fill mem %04x, len %03x, data %02x",
                    ptr, blklen, vdc.regs[31]);
#endif
        for (i = 0; i < blklen; i++)
            vdc.ram[(ptr + i) & vdc.vdc_address_mask] = vdc.regs[31];
    }

    ptr = ptr + blklen;
    vdc.regs[18] = (ptr >> 8) & 0xff;
    vdc.regs[19] = ptr & 0xff;
}


/* VDC interface functions. */

/* Store a value in a VDC register. */
void REGPARM2 vdc_store(WORD addr, BYTE value)
{
    BYTE oldval;

    machine_handle_pending_alarms(maincpu_num_write_cycles());

    /* $d600 sets the internal vdc address pointer */
    if ((addr & 1) == 0) {
#ifdef REG_DEBUG
        /*log_message(vdc.log, "STORE $D600 %02x", value);*/
#endif
        vdc.update_reg = value & 0x3f;
        return;
    }

    oldval = vdc.regs[vdc.update_reg];

    /* $d601 sets the vdc register indexed by the update register pointer */
    vdc.regs[vdc.update_reg] = value;

#ifdef REG_DEBUG
    log_message(vdc.log, "REG %02i VAL %02x", vdc.update_reg, value);
#endif

    switch (vdc.update_reg) {
      case 0:                   /* R00  Horizontal total (characters + 1) */
        if (vdc.regs[0] != oldval) {
            if (vdc.regs[0] >= 120 && vdc.regs[0] <= 127) {
                vdc.xchars_total = vdc.regs[0] + 1;
                vdc_calculate_xsync();
            }
        }
#ifdef REG_DEBUG
        log_message(vdc.log, "Horizontal Total %i", vdc.xchars_total);
#endif
        break;
      case 1:                   /* R01  Horizontal characters displayed */
        if (vdc.regs[1] != oldval) {
            if (vdc.regs[1] >= 8 && vdc.regs[1] <= VDC_SCREEN_MAX_TEXTCOLS) {
                if ((int)vdc.screen_text_cols != vdc.regs[1]) {
                    vdc.update_geometry = 1;
                }
            }
        }
#ifdef REG_DEBUG
        log_message(vdc.log, "Horizontal Displayed %i", vdc.regs[1]);
#endif
        break;

      case 2:                   /* R02  Horizontal Sync Position */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 2 unsupported!");
#endif
        break;

      case 3:                   /* R03  Horizontal/Vertical Sync widths */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 3 unsupported!");
#endif
        break;

      case 4:                   /* R04  Vertical total (character) rows */
        if (vdc.regs[4] != oldval)
            vdc.update_geometry = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Vertical Total %i.", vdc.regs[4]);
#endif
        break;

      case 5:                   /* R05  Vertical total line adjust */
        if (vdc.regs[5] != oldval)
            vdc.update_geometry = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Vertical Total Fine Adjust %i.", vdc.regs[5]);
#endif
        break;

      case 6:                   /* R06  Number of display lines on screen */
        if (vdc.regs[6] != oldval)
            vdc.update_geometry = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Vertical Displayed %i.", vdc.regs[6]);
#endif
        break;

      case 7:                   /* R07  Vertical sync position */
        if (vdc.regs[7] != oldval)
            vdc.update_geometry = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Vertical Sync Position %i.", vdc.regs[7]);
#endif
        break;

      case 8:                   /* R08  unused: Interlace and Skew */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 8 unsupported!");
#endif
        break;

      case 9:                   /* R09  Rasters between two display lines */
        if (vdc.regs[9] != oldval)
            vdc.update_geometry = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Character Total Vertical %i", vdc.regs[9]);
#endif
        break;

      case 10:                  /* R10  Cursor Mode, Start Scan */
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

      case 11:                  /* R11  Cursor */
        break;

      case 12:                  /* R12  Display Start Address hi */
      case 13:                  /* R13  Display Start Address lo */
        /* Screen address will be taken at first displayed line.  */
#ifdef REG_DEBUG
        log_message(vdc.log, "Update screen_adr: %x.", vdc.screen_adr);
#endif
        break;

      case 14:
      case 15:                  /* R14-5 Cursor location HI/LO */
        vdc.crsrpos = ((vdc.regs[14] << 8) | vdc.regs[15])
                      & vdc.vdc_address_mask;;
        break;

      case 16:                  /* R16/17 Light Pen hi/lo */
      case 17:
        break;

      case 18:                  /* R18/19 Update Address hi/lo */
      case 19:
        vdc.update_adr = ((vdc.regs[18] << 8) | vdc.regs[19])
                         & vdc.vdc_address_mask;
        break;

      case 20:                  /* R20/21 Attribute Start Address hi/lo */
      case 21:
        vdc.attribute_adr = ((vdc.regs[20] << 8) | vdc.regs[21])
                            & vdc.vdc_address_mask;
#ifdef REG_DEBUG
        log_message(vdc.log, "Update attribute_adr: %x.", vdc.attribute_adr);
#endif
        break;

      case 22:                  /* R22 Character Horizontal Size Control */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 22 unsupported!");
#endif
        break;

      case 23:                  /* R23 Vert. Character Pxl Spc */
#ifdef REG_DEBUG
        log_message(vdc.log, "Vert. Character Pxl Spc %i.",
                    vdc.regs[23] & 0x1f);
#endif
        break;

      case 24:
        if (value & 0x20)
          vdc.text_blink_frequency = 32;
        else
          vdc.text_blink_frequency = 16;

        if ((vdc.regs[24] & 0x1f) != (oldval & 0x1f))
            vdc.update_geometry = 1;

#ifdef REG_DEBUG
        log_message(vdc.log, "Vertical Smooth Scroll %i.", vdc.regs[24] & 0x1f);
        log_message(vdc.log, "Blink frequency: %s.",
                    (vdc.regs[24] & 0x20) ? "1/32" : "1/16");
        log_message(vdc.log, "Screen mode: %s.",
                    (vdc.regs[24] & 0x40) ? "reverse" : "normal");
#endif
        break;

      case 25:
        if (7 - (vdc.regs[25] & 7) != vdc.xsmooth) {
#ifdef ALLOW_UNALIGNED_ACCESS
            vdc.xsmooth = 7 - (vdc.regs[25] & 7);
            vdc.raster.xsmooth = vdc.xsmooth;
#else
            vdc.xsmooth = 0;
            vdc.raster.xsmooth = 0;
#endif
        }
#ifdef REG_DEBUG
        log_message(vdc.log, "Video mode: %s.",
                    (vdc.regs[25] & 0x80) ? "bitmap" : "text");
        log_message(vdc.log, "Color source: %s.",
                    (vdc.regs[25] & 0x40) ? "attribute space" : "register 26");
        if (vdc.regs[25] & 0x20)
            log_message(vdc.log, "Semi-Graphics Mode unsupported!");
        if (vdc.regs[25] & 0x10)
            log_message(vdc.log, "Double-Pixel Mode unsupported!");
#endif
        break;

      case 26:
        if ((!(vdc.regs[25] & 0x40)) && (vdc.regs[26] != oldval))
            vdc.force_repaint = 1;
#ifdef REG_DEBUG
        log_message(vdc.log, "Color register %x.", vdc.regs[26]);
#endif
        break;

      case 27:                  /* R27  Row/Adrs. Increment */
#ifdef REG_DEBUG
        log_message(vdc.log, "Row/Adrs. Increment %i.", vdc.regs[27]);
#endif
        break;

      case 28:
        vdc.chargen_adr = ((vdc.regs[28] << 8) & 0xe000)
                          & vdc.vdc_address_mask;
#ifdef REG_DEBUG
        log_message(vdc.log, "Update chargen_adr: %x.", vdc.chargen_adr);
#endif
        break;

      case 30:                  /* Word Count */
        vdc_perform_fillcopy();
        break;

      case 31:                  /* Data */
        vdc_write_data();
        break;

      case 32:                  /* R32/33 Block Start Address hi/lo */
      case 33:
        break;

      case 34:                  /* R34  Display Enable Begin */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 34 unsupported!");
#endif
        break;

      case 35:                  /* R35  Display Enable End */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 35 unsupported!");
#endif
        break;

      case 36:                  /* R36  DRAM Refresh Rate */
#ifdef REG_DEBUG
        log_message(vdc.log, "REG 36 unsupported!");
#endif
        break;
    }
}


BYTE REGPARM1 vdc_read(WORD addr)
{
    machine_handle_pending_alarms(0);

    if (addr & 1) {
        /*log_message(vdc.log, "read: addr = %x", addr);*/

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

        if (vdc.update_reg == 28) {
            if (vdc.vdc_address_mask == 0xffff)
                return vdc.regs[28] | 0x1f;
            else
                return vdc.regs[28] | 0x0f;
        }

        if (vdc.update_reg < 37)
            return vdc.regs[vdc.update_reg];

        return 0xff;
    } else {
        /* Emulate vblank bit.  */
        if (vdc.raster.current_line > vdc.last_displayed_line)
            return 0xb8 | vdc.revision;

        return 0x98 | vdc.revision;
    }
}

BYTE REGPARM1 vdc_ram_read(WORD addr)
{
    return vdc.ram[addr & vdc.vdc_address_mask];
}

void REGPARM2 vdc_ram_store(WORD addr, BYTE value)
{
   vdc.ram[addr & vdc.vdc_address_mask] = value; 
}

