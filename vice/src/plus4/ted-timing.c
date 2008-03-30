/*
 * ted-timing.c - Timing related settings for the TED emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Tibor Biczo <crown@axelero.hu>
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

#include "machine.h"
#include "plus4.h"
#include "resources.h"
#include "ted-timing.h"
#include "tedtypes.h"


/* Number of cycles per line.  */
#define TED_PAL_CYCLES_PER_LINE     PLUS4_PAL_CYCLES_PER_LINE
#define TED_NTSC_CYCLES_PER_LINE    PLUS4_NTSC_CYCLES_PER_LINE

/* Cycle # at which the current raster line is re-drawn.  It is set to
   `TED_CYCLES_PER_LINE', so this actually happens at the very beginning
   (i.e. cycle 0) of the next line.  */
#define TED_PAL_DRAW_CYCLE          TED_PAL_CYCLES_PER_LINE
#define TED_NTSC_DRAW_CYCLE         TED_NTSC_CYCLES_PER_LINE


void ted_timing_set(machine_timing_t *machine_timing)
{
    resource_value_t mode;

    resources_get_value("MachineVideoStandard", (void *)&mode);

    switch ((int)mode) {
      case MACHINE_SYNC_NTSC:
        ted.screen_height = TED_NTSC_SCREEN_HEIGHT;
        ted.first_displayed_line = TED_NTSC_FIRST_DISPLAYED_LINE;
        ted.last_displayed_line = TED_NTSC_LAST_DISPLAYED_LINE;
        ted.row_25_start_line = TED_NTSC_25ROW_START_LINE;
        ted.row_25_stop_line = TED_NTSC_25ROW_STOP_LINE;
        ted.row_24_start_line = TED_NTSC_24ROW_START_LINE;
        ted.row_24_stop_line = TED_NTSC_24ROW_STOP_LINE;
        ted.screen_borderwidth = TED_SCREEN_NTSC_BORDERWIDTH;
        ted.screen_borderheight = TED_SCREEN_NTSC_BORDERHEIGHT;
        ted.cycles_per_line = TED_NTSC_CYCLES_PER_LINE;
        ted.draw_cycle = TED_NTSC_DRAW_CYCLE;
        ted.first_dma_line = TED_NTSC_FIRST_DMA_LINE;
        ted.last_dma_line = TED_NTSC_LAST_DMA_LINE;
        ted.offset = TED_NTSC_OFFSET;
        ted.vsync_line = TED_NTSC_VSYNC_LINE;
        break;
      case MACHINE_SYNC_PAL:
      default:
        ted.screen_height = TED_PAL_SCREEN_HEIGHT;
        ted.first_displayed_line = TED_PAL_FIRST_DISPLAYED_LINE;
        ted.last_displayed_line = TED_PAL_LAST_DISPLAYED_LINE;
        ted.row_25_start_line = TED_PAL_25ROW_START_LINE;
        ted.row_25_stop_line = TED_PAL_25ROW_STOP_LINE;
        ted.row_24_start_line = TED_PAL_24ROW_START_LINE;
        ted.row_24_stop_line = TED_PAL_24ROW_STOP_LINE;
        ted.screen_borderwidth = TED_SCREEN_PAL_BORDERWIDTH;
        ted.screen_borderheight = TED_SCREEN_PAL_BORDERHEIGHT;
        ted.cycles_per_line = TED_PAL_CYCLES_PER_LINE;
        ted.draw_cycle = TED_PAL_DRAW_CYCLE;
        ted.first_dma_line = TED_PAL_FIRST_DMA_LINE;
        ted.last_dma_line = TED_PAL_LAST_DMA_LINE;
        ted.offset = TED_PAL_OFFSET;
        ted.vsync_line = TED_PAL_VSYNC_LINE;
        break;
    }
}

