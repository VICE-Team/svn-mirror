/*
 * vicii-timing.c - Timing related settings for the MOS 6569 (VIC-II) emulation.
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

#include "c64.h"
#include "machine.h"
#include "resources.h"
#include "vicii-sprites.h"
#include "vicii-timing.h"
#include "viciitypes.h"


/* Screen constants.  */
#define VICII_PAL_SCREEN_HEIGHT     312
#define VICII_NTSC_SCREEN_HEIGHT    263
#define VICII_NTSCOLD_SCREEN_HEIGHT 262

#define VICII_PAL_OFFSET                   0
#define VICII_NTSC_OFFSET                  0
#define VICII_NTSCOLD_OFFSET               0

#define VICII_SCREEN_PAL_BORDERWIDTH       32
#define VICII_SCREEN_PAL_BORDERHEIGHT      51
#define VICII_SCREEN_NTSC_BORDERWIDTH      32
#define VICII_SCREEN_NTSC_BORDERHEIGHT     27
#define VICII_SCREEN_NTSCOLD_BORDERWIDTH   32
#define VICII_SCREEN_NTSCOLD_BORDERHEIGHT  27

#define VICII_PAL_FIRST_DISPLAYED_LINE     0x10
#define VICII_PAL_LAST_DISPLAYED_LINE      0x11f
#define VICII_PAL_25ROW_START_LINE         0x33
#define VICII_PAL_25ROW_STOP_LINE          0xfb
#define VICII_PAL_24ROW_START_LINE         0x37
#define VICII_PAL_24ROW_STOP_LINE          0xf7

#define VICII_NTSC_FIRST_DISPLAYED_LINE    (0x20 - VICII_NTSC_OFFSET)
#define VICII_NTSC_LAST_DISPLAYED_LINE     0x102
#define VICII_NTSC_25ROW_START_LINE        (0x33 - VICII_NTSC_OFFSET)
#define VICII_NTSC_25ROW_STOP_LINE         (0xfb - VICII_NTSC_OFFSET)
#define VICII_NTSC_24ROW_START_LINE        (0x37 - VICII_NTSC_OFFSET)
#define VICII_NTSC_24ROW_STOP_LINE         (0xf7 - VICII_NTSC_OFFSET)

#define VICII_NTSCOLD_FIRST_DISPLAYED_LINE (0x20 - VICII_NTSCOLD_OFFSET)
#define VICII_NTSCOLD_LAST_DISPLAYED_LINE  0x102
#define VICII_NTSCOLD_25ROW_START_LINE     (0x33 - VICII_NTSCOLD_OFFSET)
#define VICII_NTSCOLD_25ROW_STOP_LINE      (0xfb - VICII_NTSCOLD_OFFSET)
#define VICII_NTSCOLD_24ROW_START_LINE     (0x37 - VICII_NTSCOLD_OFFSET)
#define VICII_NTSCOLD_24ROW_STOP_LINE      (0xf7 - VICII_NTSCOLD_OFFSET)

/* Number of cycles per line.  */
#define VICII_PAL_CYCLES_PER_LINE      C64_PAL_CYCLES_PER_LINE
#define VICII_NTSC_CYCLES_PER_LINE     C64_NTSC_CYCLES_PER_LINE
#define VICII_NTSCOLD_CYCLES_PER_LINE  C64_NTSCOLD_CYCLES_PER_LINE

/* Cycle # at which sprite DMA is set.  */
#define VICII_PAL_SPRITE_FETCH_CYCLE       54
#define VICII_NTSC_SPRITE_FETCH_CYCLE      55
#define VICII_NTSCOLD_SPRITE_FETCH_CYCLE   54

#define VICII_PAL_SPRITE_WRAP_X     504
#define VICII_NTSC_SPRITE_WRAP_X    520
#define VICII_NTSCOLD_SPRITE_WRAP_X 512

/* Cycle # at which the current raster line is re-drawn.  It is set to
   `VICII_CYCLES_PER_LINE', so this actually happens at the very beginning
   (i.e. cycle 0) of the next line.  */
#define VICII_PAL_DRAW_CYCLE       VICII_PAL_CYCLES_PER_LINE
#define VICII_NTSC_DRAW_CYCLE      VICII_NTSC_CYCLES_PER_LINE
#define VICII_NTSCOLD_DRAW_CYCLE   VICII_NTSCOLD_CYCLES_PER_LINE

/* Bad line range.  */
#define VICII_PAL_FIRST_DMA_LINE      0x30
#define VICII_PAL_LAST_DMA_LINE       0xf7
#define VICII_NTSC_FIRST_DMA_LINE     (0x30 - VICII_NTSC_OFFSET)
#define VICII_NTSC_LAST_DMA_LINE      0xf7
#define VICII_NTSCOLD_FIRST_DMA_LINE  (0x30 - VICII_NTSCOLD_OFFSET)
#define VICII_NTSCOLD_LAST_DMA_LINE   0xf7


void vicii_timing_set(machine_timing_t *machine_timing)
{
    resource_value_t mode;

    resources_get_value("MachineVideoStandard", (void *)&mode);

    switch ((int)mode) {
      case MACHINE_SYNC_NTSC:
        vicii.screen_height = VICII_NTSC_SCREEN_HEIGHT;
        vicii.first_displayed_line = VICII_NTSC_FIRST_DISPLAYED_LINE;
        vicii.last_displayed_line = VICII_NTSC_LAST_DISPLAYED_LINE;
        vicii.row_25_start_line = VICII_NTSC_25ROW_START_LINE;
        vicii.row_25_stop_line = VICII_NTSC_25ROW_STOP_LINE;
        vicii.row_24_start_line = VICII_NTSC_24ROW_START_LINE;
        vicii.row_24_stop_line = VICII_NTSC_24ROW_STOP_LINE;
        vicii.screen_borderwidth = VICII_SCREEN_NTSC_BORDERWIDTH;
        vicii.screen_borderheight = VICII_SCREEN_NTSC_BORDERHEIGHT;
        vicii.cycles_per_line = VICII_NTSC_CYCLES_PER_LINE;
        vicii.draw_cycle = VICII_NTSC_DRAW_CYCLE;
        vicii.sprite_fetch_cycle = VICII_NTSC_SPRITE_FETCH_CYCLE;
        vicii.sprite_wrap_x = VICII_NTSC_SPRITE_WRAP_X;
        vicii.first_dma_line = VICII_NTSC_FIRST_DMA_LINE;
        vicii.last_dma_line = VICII_NTSC_LAST_DMA_LINE;
        vicii.offset = VICII_NTSC_OFFSET;
        break;
      case MACHINE_SYNC_NTSCOLD:
        vicii.screen_height = VICII_NTSCOLD_SCREEN_HEIGHT;
        vicii.first_displayed_line = VICII_NTSCOLD_FIRST_DISPLAYED_LINE;
        vicii.last_displayed_line = VICII_NTSCOLD_LAST_DISPLAYED_LINE;
        vicii.row_25_start_line = VICII_NTSCOLD_25ROW_START_LINE;
        vicii.row_25_stop_line = VICII_NTSCOLD_25ROW_STOP_LINE;
        vicii.row_24_start_line = VICII_NTSCOLD_24ROW_START_LINE;
        vicii.row_24_stop_line = VICII_NTSCOLD_24ROW_STOP_LINE;
        vicii.screen_borderwidth = VICII_SCREEN_NTSCOLD_BORDERWIDTH;
        vicii.screen_borderheight = VICII_SCREEN_NTSCOLD_BORDERHEIGHT;
        vicii.cycles_per_line = VICII_NTSCOLD_CYCLES_PER_LINE;
        vicii.draw_cycle = VICII_NTSCOLD_DRAW_CYCLE;
        vicii.sprite_fetch_cycle = VICII_NTSCOLD_SPRITE_FETCH_CYCLE;
        vicii.sprite_wrap_x = VICII_NTSCOLD_SPRITE_WRAP_X;
        vicii.first_dma_line = VICII_NTSCOLD_FIRST_DMA_LINE;
        vicii.last_dma_line = VICII_NTSCOLD_LAST_DMA_LINE;
        vicii.offset = VICII_NTSCOLD_OFFSET;
        break;
      case MACHINE_SYNC_PAL:
      default:
        vicii.screen_height = VICII_PAL_SCREEN_HEIGHT;
        vicii.first_displayed_line = VICII_PAL_FIRST_DISPLAYED_LINE;
        vicii.last_displayed_line = VICII_PAL_LAST_DISPLAYED_LINE;
        vicii.row_25_start_line = VICII_PAL_25ROW_START_LINE;
        vicii.row_25_stop_line = VICII_PAL_25ROW_STOP_LINE;
        vicii.row_24_start_line = VICII_PAL_24ROW_START_LINE;
        vicii.row_24_stop_line = VICII_PAL_24ROW_STOP_LINE;
        vicii.screen_borderwidth = VICII_SCREEN_PAL_BORDERWIDTH;
        vicii.screen_borderheight = VICII_SCREEN_PAL_BORDERHEIGHT;
        vicii.cycles_per_line = VICII_PAL_CYCLES_PER_LINE;
        vicii.draw_cycle = VICII_PAL_DRAW_CYCLE;
        vicii.sprite_fetch_cycle = VICII_PAL_SPRITE_FETCH_CYCLE;
        vicii.sprite_wrap_x = VICII_PAL_SPRITE_WRAP_X;
        vicii.first_dma_line = VICII_PAL_FIRST_DMA_LINE;
        vicii.last_dma_line = VICII_PAL_LAST_DMA_LINE;
        vicii.offset = VICII_PAL_OFFSET;
        break;
    }

    vicii_sprites_init_sprline();
}

