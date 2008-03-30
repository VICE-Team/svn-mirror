/*
 * vicii-irq.c - IRQ related functions for the MOS 6569 (VIC-II) emulation.
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

#include "alarm.h"
#include "interrupt.h"
#include "maincpu.h"
#include "vicii-irq.h"
#include "viciitypes.h"


#define vicii_set_irq(irq, state) maincpu_set_irq(irq, state)


void vicii_irq_set_raster(unsigned int line)
{
    if (line == vic_ii.raster_irq_line && vic_ii.raster_irq_clk != CLOCK_MAX)
        return;

    if (line < vic_ii.screen_height) {
        unsigned int current_line = VIC_II_RASTER_Y(maincpu_clk);

        vic_ii.raster_irq_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                                 + VIC_II_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                                 + (vic_ii.cycles_per_line
                                 * (line - current_line)));

        /* Raster interrupts on line 0 are delayed by 1 cycle.  */
        if (line == 0)
            vic_ii.raster_irq_clk++;

        if (line <= current_line)
            vic_ii.raster_irq_clk += (vic_ii.screen_height
                                     * vic_ii.cycles_per_line);
        alarm_set(vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
    } else {
        VIC_II_DEBUG_RASTER(("update_raster_irq(): "
                            "raster compare out of range ($%04X)!", line));
        alarm_unset(vic_ii.raster_irq_alarm);
    }

    VIC_II_DEBUG_RASTER(("update_raster_irq(): "
                        "vic_ii.raster_irq_clk = %ul, "
                        "line = $%04X, "
                        "vic_ii.regs[0x1a] & 1 = %d",
                        vic_ii.raster_irq_clk, line, vic_ii.regs[0x1a] & 1));

    vic_ii.raster_irq_line = line;
}

void vicii_irq_check_state(unsigned int irq_line)
{
    unsigned int line;
    unsigned int old_raster_irq_line;

    if (irq_line == vic_ii.raster_irq_line)
        return;

    line = VIC_II_RASTER_Y(maincpu_clk);

    old_raster_irq_line = vic_ii.raster_irq_line;
    vicii_irq_set_raster(irq_line);

    if (vic_ii.regs[0x1a] & 0x1) {
        int trigger_irq;

        trigger_irq = 0;

        if (maincpu_rmw_flag) {
            if (VIC_II_RASTER_CYCLE(maincpu_clk) == 0) {
                unsigned int previous_line = VIC_II_PREVIOUS_LINE(line);

                if (previous_line != old_raster_irq_line
                    && ((old_raster_irq_line & 0x100)
                    == (previous_line & 0x100)))
                    trigger_irq = 1;
            } else {
                if (line != old_raster_irq_line
                    && (old_raster_irq_line & 0x100) == (line & 0x100))
                    trigger_irq = 1;
            }
        }

        if (vic_ii.raster_irq_line == line && line != old_raster_irq_line)
            trigger_irq = 1;

        if (trigger_irq) {
            vic_ii.irq_status |= 0x81;
            vicii_set_irq(I_RASTER, 1);
        }
    }
}

void vicii_irq_set_line(void)
{
    if (vic_ii.irq_status & 0x80)
        vicii_set_irq(I_RASTER, 1);
    else
        vicii_set_irq(I_RASTER, 0);
}

