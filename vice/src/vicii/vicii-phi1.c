/*
 * vicii-phi1.c - Memory interface for the MOS6569 (VIC-II) emulation,
 *                PHI1 support.
 *
 * Written by
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

#include "maincpu.h"
#include "types.h"
#include "vicii-phi1.h"
#include "viciitypes.h"


inline static BYTE gfx_data_illegal_bitmap(unsigned int num)
{
    if (vicii.idle_state)
        return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x39ff];
    else
        return vicii.bitmap_ptr[((vicii.memptr << 3) + vicii.raster.ycounter
                                + num * 8) & 0x19ff];
}

inline static BYTE gfx_data_hires_bitmap(unsigned int num)
{
    if (vicii.idle_state)
        return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3fff];
    else
        return vicii.bitmap_ptr[((vicii.memptr << 3) + vicii.raster.ycounter
                                + num * 8) & 0x1fff];
}

inline static BYTE gfx_data_extended_text(unsigned int num)
{
    if (vicii.idle_state)
        return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x39ff];
    else
        return vicii.chargen_ptr[(vicii.vbuf[num] & 0x3f) * 8
                                 + vicii.raster.ycounter];
}

inline static BYTE gfx_data_normal_text(unsigned int num)
{
    if (vicii.idle_state)
        return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3fff];
    else
        return vicii.chargen_ptr[vicii.vbuf[num] * 8
                                 + vicii.raster.ycounter];
}

static BYTE gfx_data(unsigned int num)
{
    BYTE value = 0;

    switch (vicii.raster.video_mode) {
      case VICII_NORMAL_TEXT_MODE:
      case VICII_MULTICOLOR_TEXT_MODE:
        value = gfx_data_normal_text(num);
        break;
      case VICII_HIRES_BITMAP_MODE:
      case VICII_MULTICOLOR_BITMAP_MODE:
        value = gfx_data_hires_bitmap(num);
        break;
      case VICII_EXTENDED_TEXT_MODE:
      case VICII_ILLEGAL_TEXT_MODE:
        value = gfx_data_extended_text(num);
        break;
      case VICII_ILLEGAL_BITMAP_MODE_1:
      case VICII_ILLEGAL_BITMAP_MODE_2:
        value = gfx_data_illegal_bitmap(num);
        break;
      default:
        value = vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3fff];
    }

    return value;
}

static BYTE idle_gap(void)
{
    return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3fff];
}

static BYTE sprite_data(unsigned int num)
{
    return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3fff];
}

static BYTE sprite_pointer(unsigned int num)
{
    WORD offset;

    offset = ((vicii.regs[0x18] & 0xf0) << 6) + 0x3f8 + num;

    return vicii.ram_base_phi1[vicii.vbank_phi1 + offset];
}

static BYTE refresh_counter(unsigned int num)
{
    BYTE offset;

    offset = 0xff - (VICII_RASTER_Y(maincpu_clk) * 5 + num);

    return vicii.ram_base_phi1[vicii.vbank_phi1 + 0x3f00 + offset];
}

BYTE vicii_read_phi1_lowlevel(void)
{
    BYTE value = 0x40;
    unsigned int cycle;

    cycle = VICII_RASTER_CYCLE(maincpu_clk);

    switch (cycle) {
      case 0:
        value = sprite_pointer(3);
        break;
      case 1:
        value = sprite_data(3);
        break;
      case 2:
        value = sprite_pointer(4);
        break;
      case 3:
        value = sprite_data(4);
        break;
      case 4:
        value = sprite_pointer(5);
        break;
      case 5:
        value = sprite_data(5);
        break;
      case 6:
        value = sprite_pointer(6);
        break;
      case 7:
        value = sprite_data(6);
        break;
      case 8:
        value = sprite_pointer(7);
        break;
      case 9:
        value = sprite_data(7);
        break;
      case 10:
        value = refresh_counter(0);
        break;
      case 11:
        value = refresh_counter(1);
        break;
      case 12:
        value = refresh_counter(2);
        break;
      case 13:
        value = refresh_counter(3);
        break;
      case 14:
        value = refresh_counter(4);
        break;
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
        value = gfx_data(cycle - 15);
        break;
      case 55:
        value = idle_gap();
        break;
      case 56:
        value = idle_gap();
        break;
      case 57:
        value = sprite_pointer(0);
        break;
      case 58:
        value = sprite_data(0);
        break;
      case 59:
        value = sprite_pointer(1);
        break;
      case 60:
        value = sprite_data(1);
        break;
      case 61:
        value = sprite_pointer(2);
        break;
      case 62:
        value = sprite_data(2);
        break;
    }

    return value;
}

BYTE vicii_read_phi1(void)
{
    vicii_handle_pending_alarms(0);

    return vicii_read_phi1_lowlevel();
}

