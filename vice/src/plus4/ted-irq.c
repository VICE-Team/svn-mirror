/*
 * ted-irq.c - IRQ related functions for the TED emulation.
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

#include "alarm.h"
#include "interrupt.h"
#include "ted-irq.h"
#include "tedtypes.h"


static inline void ted_irq_set_line_clk(CLOCK mclk)
{
    if (ted.irq_status & ted.regs[0xa] & 0xfe) {
        ted.irq_status |= 0x80;
        maincpu_set_irq_clk(ted.int_num, 1, mclk);
    } else {
        ted.irq_status &= 0x7f;
        maincpu_set_irq_clk(ted.int_num, 0, mclk);
    }
}

void ted_irq_raster_set(CLOCK mclk)
{
    ted.irq_status |= 0x2;
    ted_irq_set_line_clk(mclk);
}

void ted_irq_raster_clear(CLOCK mclk)
{
    ted.irq_status &= 0xfd;
    ted_irq_set_line_clk(mclk);
}

void ted_irq_set_raster_line(unsigned int line)
{
    if (line == ted.raster_irq_line && ted.raster_irq_clk != CLOCK_MAX)
        return;

    if (line < (unsigned int)ted.screen_height) {
        unsigned int current_line = TED_RASTER_Y(maincpu_clk);

        ted.raster_irq_clk = (TED_LINE_START_CLK(maincpu_clk)
                             + TED_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                             + (ted.cycles_per_line
                             * (line - current_line)));

        /* Raster interrupts on line 0 are delayed by 1 cycle.  */
        if (line == 0)
            ted.raster_irq_clk++;

        if (line <= current_line)
            ted.raster_irq_clk += (ted.screen_height
                                  * ted.cycles_per_line);
        alarm_set(ted.raster_irq_alarm, ted.raster_irq_clk);
    } else {
        TED_DEBUG_RASTER(("TED: update_raster_irq(): "
                         "raster compare out of range ($%04X)!", line));
        ted.raster_irq_clk = CLOCK_MAX;
        alarm_unset(ted.raster_irq_alarm);
    }

    TED_DEBUG_RASTER(("TED: update_raster_irq(): "
                     "ted.raster_irq_clk = %ul, "
                     "line = $%04X, "
                     "ted.regs[0x0a] & 2 = %d\n",
                     ted.raster_irq_clk, line, ted.regs[0x0a] & 2));

    ted.raster_irq_line = line;
}

void ted_irq_next_frame(void)
{
    ted.raster_irq_clk += ted.screen_height * ted.cycles_per_line;
    alarm_set(ted.raster_irq_alarm, ted.raster_irq_clk);
}

void ted_irq_init(void)
{
    ted.int_num = interrupt_cpu_status_int_new(maincpu_int_status);
}

