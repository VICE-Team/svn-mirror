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
#include "viciitypes.h"


/* Number of cycles per line.  */
#define VIC_II_PAL_CYCLES_PER_LINE      C64_PAL_CYCLES_PER_LINE
#define VIC_II_NTSC_CYCLES_PER_LINE     C64_NTSC_CYCLES_PER_LINE
#define VIC_II_NTSCOLD_CYCLES_PER_LINE  C64_NTSCOLD_CYCLES_PER_LINE

/* Cycle # at which sprite DMA is set.  */
#define VIC_II_PAL_SPRITE_FETCH_CYCLE       54
#define VIC_II_NTSC_SPRITE_FETCH_CYCLE      55
#define VIC_II_NTSCOLD_SPRITE_FETCH_CYCLE   54

/* Cycle # at which the current raster line is re-drawn.  It is set to
   `VIC_II_CYCLES_PER_LINE', so this actually happens at the very beginning
   (i.e. cycle 0) of the next line.  */
#define VIC_II_PAL_DRAW_CYCLE       VIC_II_PAL_CYCLES_PER_LINE
#define VIC_II_NTSC_DRAW_CYCLE      VIC_II_NTSC_CYCLES_PER_LINE
#define VIC_II_NTSCOLD_DRAW_CYCLE   VIC_II_NTSCOLD_CYCLES_PER_LINE


void vicii_timing_set(machine_timing_t *machine_timing)
{
    resource_value_t mode;

    resources_get_value("MachineVideoStandard", &mode);

    switch ((int)mode) {
      case MACHINE_SYNC_NTSC:
        vic_ii.screen_height = VIC_II_NTSC_SCREEN_HEIGHT;
        vic_ii.first_displayed_line = VIC_II_NTSC_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_NTSC_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_NTSC_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_NTSC_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_NTSC_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_NTSC_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_NTSC_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_NTSC_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_NTSC_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_NTSC_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_NTSC_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_NTSC_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_NTSC_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_NTSC_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_NTSC_OFFSET;
        break;
      case MACHINE_SYNC_NTSCOLD:
        vic_ii.first_displayed_line = VIC_II_NTSCOLD_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_NTSCOLD_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_NTSCOLD_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_NTSCOLD_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_NTSCOLD_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_NTSCOLD_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_NTSCOLD_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_NTSCOLD_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_NTSCOLD_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_NTSCOLD_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_NTSCOLD_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_NTSCOLD_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_NTSCOLD_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_NTSCOLD_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_NTSCOLD_OFFSET;
        break;
      case MACHINE_SYNC_PAL:
      default:
        vic_ii.screen_height = VIC_II_PAL_SCREEN_HEIGHT;
        vic_ii.first_displayed_line = VIC_II_PAL_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_PAL_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_PAL_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_PAL_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_PAL_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_PAL_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_PAL_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_PAL_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_PAL_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_PAL_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_PAL_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_PAL_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_PAL_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_PAL_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_PAL_OFFSET;
        break;
    }
}

