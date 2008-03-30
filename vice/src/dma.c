/*
 * dma.c - Utility functions for DMA hosts.
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

#include "dma.h"
#include "interrupt.h"
#include "types.h"


/* Asynchronously steal `num' cycles from the CPU, starting from cycle
   `start_clk'.  */
void dma_maincpu_steal_cycles(CLOCK start_clk, int num, CLOCK sub)
{
    cpu_int_status_t *cs = &maincpu_int_status;
    CLOCK *clk_ptr = &maincpu_clk;

    if (num == 0)
        return;

    if (start_clk == cs->last_stolen_cycles_clk)
        cs->num_last_stolen_cycles += num;
    else
        cs->num_last_stolen_cycles = num;

    *clk_ptr += num;
    cs->last_stolen_cycles_clk = start_clk + num + sub;

    cs->irq_clk += num;
    cs->nmi_clk += num;
}

