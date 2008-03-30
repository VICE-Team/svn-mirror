/*
 * interrupt.h - Implementation of 6510 interrupts and alarms.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#define _INTERRUPT_C

#include "vice.h"

#include <stdio.h>

#include "interrupt.h"

/* ------------------------------------------------------------------------- */

/* Initialization.  */
void cpu_int_status_init(cpu_int_status_t *cs, int num_ints, int num_alarms,
                         opcode_info_t *last_opcode_info_ptr)
{
    int i;

    memset(cs, 0, sizeof(cpu_int_status_t));
    cs->num_ints = num_ints;
    cs->num_alarms = num_alarms;
    for (i = 0; i < 0x100; i++)
	cs->alarm_clk[i] = CLOCK_MAX;
    cs->next_alarm_clk = CLOCK_MAX;
    cs->last_opcode_info_ptr = last_opcode_info_ptr;
    cs->num_last_stolen_cycles = 0;
    cs->last_stolen_cycles_clk = (CLOCK)0;
    cs->global_pending_int = IK_NONE;
}

/* This is used to avoid clock counter overflows.  Return the number of
   cycles subtracted, which is always a multiple of `baseval'.  */
CLOCK prevent_clk_overflow(cpu_int_status_t *cs, CLOCK *clk, CLOCK baseval)
{
    if (*clk > PREVENT_CLK_OVERFLOW_TICK) {
	int i;
        CLOCK prevent_clk_overflow_sub =
            (((PREVENT_CLK_OVERFLOW_TICK & ~((CLOCK)0xffff))
              / baseval - 1) * baseval);

	*clk -= prevent_clk_overflow_sub;
	cs->next_alarm_clk -= prevent_clk_overflow_sub;
	cs->irq_clk -= prevent_clk_overflow_sub;
	cs->nmi_clk -= prevent_clk_overflow_sub;
	if (cs->last_stolen_cycles_clk > prevent_clk_overflow_sub)
	    cs->last_stolen_cycles_clk -= prevent_clk_overflow_sub;
	else
	    cs->last_stolen_cycles_clk = (CLOCK) 0;
	for (i = 0; i < cs->num_alarms; i++) {
	    if (cs->alarm_clk[i] != CLOCK_MAX)
		cs->alarm_clk[i] -= prevent_clk_overflow_sub;
	}
	return prevent_clk_overflow_sub;
    } else
	return 0;
}
