/*
 * cascom_nrzi.c - NRZI line coding + edge-timing bit recovery for the cascom
 *                 modem network gateway.
 *
 * Written by
 *  Felix Erckenbrecht <code@eligs.de>
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

#include "cascom_nrzi.h"

void cascom_nrzi_init(cascom_nrzi_t *n, int level)
{
    n->level = level ? 1 : 0;
}

int cascom_nrzi_encode(cascom_nrzi_t *n, int bit)
{
    if (!bit) {
        n->level ^= 1;          /* data 0 -> transition */
    }
    return n->level;
}

int cascom_nrzi_gap_cells(double gap_cycles, double cycles_per_bit)
{
    int n;

    if (cycles_per_bit < 1.0) {
        cycles_per_bit = 1.0;
    }
    n = (int)(gap_cycles / cycles_per_bit + 0.5);
    if (n < 1) {
        n = 1;
    }
    return n;
}
