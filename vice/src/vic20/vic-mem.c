/*
 * vic-mem.c - Memory interface for the VIC-I emulation.
 *
 * Written by
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

#include "types.h"

#include "maincpu.h"
#include "vic20mem.h"
#include "vic20sound.h"

#include "vic.h"
#include "vic-mem.h"



/* VIC access functions. */

void REGPARM2 
store_vic (ADDRESS addr, BYTE value)
{
  addr &= 0xf;
  vic.regs[addr] = value;

  VIC_DEBUG_REGISTER (("VIC: write $90%02X, value = $%02X.", addr, value));

  switch (addr)
    {
    case 0:                     /* $9000  Screen X Location. */
      value &= 0x7f;
      if (value > 8)
        value = 8;
      if (value < 1)
        value = 1;
      vic.raster.display_xstart = value * 4;
      vic.raster.display_xstop = vic.raster.display_xstart + vic.text_cols * 8;
      if (vic.raster.display_xstop >= VIC_SCREEN_WIDTH)
        vic.raster.display_xstop = VIC_SCREEN_WIDTH - 1;
      VIC_DEBUG_REGISTER (("Screen X location: $%02X.", value));
      return;
    case 1:                     /* $9001  Screen Y Location. */
      vic.raster.display_ystart = value * 2;
      vic.raster.display_ystop = (vic.raster.display_ystart
                                  + vic.text_lines * vic.char_height);
      VIC_DEBUG_REGISTER (("Screen Y location: $%02X.", value));
      return;

    case 2:                     /* $9002  Columns Displayed. */
      vic.color_ptr = ram + ((value & 0x80) ? 0x9600 : 0x9400);
      vic.text_cols = value & 0x7f;
      if (vic.text_cols > VIC_SCREEN_MAX_TEXT_COLS)
        vic.text_cols = VIC_SCREEN_MAX_TEXT_COLS;
      vic.raster.display_xstop = vic.raster.display_xstart + vic.text_cols * 8;
      if (vic.raster.display_xstop >= VIC_SCREEN_WIDTH)
        vic.raster.display_xstop = VIC_SCREEN_WIDTH - 1;
      vic_update_memory_ptrs ();
      VIC_DEBUG_REGISTER (("Color RAM at $%04X.", vic.color_ptr - ram));
      VIC_DEBUG_REGISTER (("Columns displayed: %d.", vic.text_cols));
      break;

    case 3:                     /* $9003  Rows Displayed, Character size . */
      vic.text_lines = (value & 0x7e) >> 1;
      if (vic.text_lines > VIC_SCREEN_MAX_TEXT_LINES)
        vic.text_lines = VIC_SCREEN_MAX_TEXT_LINES;
      vic.char_height = (value & 0x1) ? 16 : 8;
      vic.raster.display_ystop = (vic.raster.display_ystart
                                  + vic.text_lines * vic.char_height);
      VIC_DEBUG_REGISTER (("Rows displayed: %d.", vic.text_lines));
      VIC_DEBUG_REGISTER (("Character height: %d.", vic.char_height));
      vic_update_memory_ptrs ();
      return;

    case 4:                     /* $9004  Raster line count -- read only. */
      return;

    case 5:                     /* $9005  Video and char matrix base
                                   address. */
      vic_update_memory_ptrs ();
      return;

    case 6:                     /* $9006. */
    case 7:                     /* $9007  Light Pen X,Y. */
      VIC_DEBUG_REGISTER (("(light pen register, read-only)."));
      return;

    case 8:                     /* $9008. */
    case 9:                     /* $9009  Paddle X,Y. */
      return;

    case 10:                    /* $900A  Bass Enable and Frequency. */
    case 11:                    /* $900B  Alto Enable and Frequency. */
    case 12:                    /* $900C  Soprano Enable and Frequency. */
    case 13:                    /* $900D  Noise Enable and Frequency. */
      store_vic_sound (addr, value);
      return;

    case 14:                    /* $900E  Auxiliary Colour, Master Volume. */
      vic.auxiliary_color = value >> 4;
      VIC_DEBUG_REGISTER (("Auxiliary color set to $%02X.", auxiliary_color));
      store_vic_sound (addr, value);
      return;

    case 15:                    /* $900F  Screen and Border Colors,
                                   Reverse Video. */
      vic.raster.border_color = value & 0x7;
      vic.raster.background_color = value >> 4;
      vic.raster.video_mode = ((value & 8)
                               ? VIC_STANDARD_MODE : VIC_REVERSE_MODE);

      VIC_DEBUG_REGISTER (("Border color: $%02X.",
                           vic.raster.border_color));
      VIC_DEBUG_REGISTER (("Background color: $%02X.",
                           vic.raster.background_color));
      return;
    }
}



BYTE REGPARM1 
read_vic (ADDRESS addr)
{
  addr &= 0xf;

  switch (addr)
    {
    case 3:
      return ((VIC_RASTER_Y (clk) & 1) << 7) | (vic.regs[3] & ~0x80);
    case 4:
      return VIC_RASTER_Y (clk) >> 1;
    default:
      return vic.regs[addr];
    }
}

