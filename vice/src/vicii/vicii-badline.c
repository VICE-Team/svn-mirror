/*
 * vicii-badline.c - Bad line handling for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "interrupt.h"
#include "maincpu.h"
#include "raster.h"
#include "types.h"
#include "vicii-badline.h"
#include "vicii-fetch.h"
#include "viciitypes.h"


inline static void line_becomes_good(int cycle)
{
    if (cycle < VIC_II_FETCH_CYCLE)
        vic_ii.bad_line = 0;

    /* By changing the values in the registers, one can make the VIC-II
       switch from idle to display state, but not from display to idle
       state.  So we are always in display state if this happens. This
       is only true if the value changes in some cycle > 0, though;
       otherwise, the line never becomes bad.  */
    if (cycle > 0) {
        raster_add_int_change_foreground(&vic_ii.raster,
            VIC_II_RASTER_CHAR(VIC_II_RASTER_CYCLE(maincpu_clk)),
            &vic_ii.raster.draw_idle_state,
            0);
        vic_ii.idle_state = 0;

        vic_ii.idle_data_location = IDLE_NONE;
        if (cycle > VIC_II_FETCH_CYCLE + 2
            && !vic_ii.ycounter_reset_checked) {
            vic_ii.raster.ycounter = 0;
            vic_ii.ycounter_reset_checked = 1;
        }
    }
}

inline static void line_becomes_bad(int cycle)
{
    if (cycle >= VIC_II_FETCH_CYCLE
        && cycle < VIC_II_FETCH_CYCLE + VIC_II_SCREEN_TEXTCOLS + 3) {
        int pos;            /* Value of line counter when this happens.  */
        int inc;            /* Total increment for the line counter.  */
        int num_chars;      /* Total number of characters to fetch.  */
        int num_0xff_fetches; /* Number of 0xff fetches to do.  */

        vic_ii.bad_line = 1;

        if (cycle <= VIC_II_FETCH_CYCLE + 2)
            vic_ii.raster.ycounter = 0;

        vic_ii.ycounter_reset_checked = 1;

        num_chars = (VIC_II_SCREEN_TEXTCOLS
                     - (cycle - (VIC_II_FETCH_CYCLE + 3)));

        /* Take over the bus until the memory fetch is done.  */
           maincpu_steal_cycles(maincpu_clk, num_chars, 0);

        if (num_chars <= VIC_II_SCREEN_TEXTCOLS) {
            /* Matrix fetches starts immediately, but the VICII needs
               at least 3 cycles to become the bus master.  Before
               this happens, it fetches 0xff.  */
            num_0xff_fetches = 3;

            /* If we were in idle state before creating the bad
               line, the counters have not been incremented.  */
            if (vic_ii.idle_state) {
                pos = 0;
                inc = num_chars;
                if (inc < 0)
                    inc = 0;
            } else {
                pos = cycle - (VIC_II_FETCH_CYCLE + 3);
                if (pos > VIC_II_SCREEN_TEXTCOLS - 1)
                    pos = VIC_II_SCREEN_TEXTCOLS - 1;
                inc = VIC_II_SCREEN_TEXTCOLS;
            }
        } else {
            pos = 0;
            num_chars = inc = VIC_II_SCREEN_TEXTCOLS;
            num_0xff_fetches = cycle - VIC_II_FETCH_CYCLE;
        }

        /* This is normally done at cycle `VIC_II_FETCH_CYCLE + 2'.  */
        vic_ii.mem_counter = vic_ii.memptr;

        /* Force the DMA.  */
        if (num_chars > 0)
            vic_ii_fetch_matrix(pos, num_chars, num_0xff_fetches);

        /* Set the value by which `vic_ii.mem_counter' is incremented on
           this line.  */
        vic_ii.mem_counter_inc = inc;

        /* Remember we have done a DMA.  */
        vic_ii.memory_fetch_done = 2;

        /* As we are on a bad line, switch to display state.  */
        vic_ii.idle_state = 0;

        /* Force screen on even if the store that triggered the DMA has
           set the blank bit.  */
        vic_ii.raster.blank_off = 1;

        /* Try to display things correctly.  This is not exact,
           but should be OK for most cases (FIXME?).  */
        if (inc == VIC_II_SCREEN_TEXTCOLS) {
            vic_ii.raster.draw_idle_state = 0;
            vic_ii.idle_data_location = IDLE_NONE;
        }
    } else if (cycle <= VIC_II_FETCH_CYCLE + VIC_II_SCREEN_TEXTCOLS + 6) {
        /* Bad line has been generated after fetch interval, but
           before `vic_ii.raster.ycounter' is incremented.  */

        vic_ii.bad_line = 1;

        /* If in idle state, counter is not incremented.  */
        if (vic_ii.idle_state)
            vic_ii.mem_counter_inc = 0;

        /* We are not in idle state anymore.  */
        /* This is not 100% correct, but should be OK for most cases.
           (FIXME?)  */
#if 0
        vic_ii.raster.draw_idle_state = vic_ii.idle_state = 0;
#else
        raster_add_int_change_foreground
               (&vic_ii.raster,
                VIC_II_RASTER_CHAR(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.raster.draw_idle_state,
                0);
        vic_ii.idle_state = 0;
#endif

        vic_ii.idle_data_location = IDLE_NONE;
    } else {
        /* Line is now bad, so we must switch to display state.
           Anyway, we cannot do it here as the `ycounter' handling
           must happen in as in idle state.  */
        vic_ii.force_display_state = 1;
    }
}

void vicii_badline_check_state(BYTE value, int cycle, int line,
                               int old_allow_bad_lines)
{
    int was_bad_line, now_bad_line;

    /* Check whether bad line state has changed.  */
    was_bad_line = (old_allow_bad_lines
                    && (vic_ii.raster.ysmooth == (line & 7)));
    now_bad_line = (vic_ii.allow_bad_lines
                    && ((value & 7) == (line & 7)));

    if (was_bad_line && !now_bad_line) {
        line_becomes_good(cycle);
    } else {
        if (!was_bad_line && now_bad_line)
            line_becomes_bad(cycle);
    }
}

