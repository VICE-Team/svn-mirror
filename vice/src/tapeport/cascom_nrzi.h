/*
 * cascom_nrzi.h - NRZI line coding + edge-timing bit recovery for the cascom
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

/* AX.25 uses NRZI on the wire: a data '0' is sent as a level transition, a '1'
 * as no transition. These helpers are shared by cascom.c (network mode) and the
 * unit test; absolute polarity never matters (differential).
 */

#ifndef VICE_CASCOM_NRZI_H
#define VICE_CASCOM_NRZI_H

/* NRZI encoder state (raw bit -> line level), used to play a frame onto the C64
   RX line: '0' toggles the level, '1' holds it. */
typedef struct cascom_nrzi_s {
    int level;
} cascom_nrzi_t;

void cascom_nrzi_init(cascom_nrzi_t *n, int level);
int cascom_nrzi_encode(cascom_nrzi_t *n, int bit);

/* TX recovery: the C64 toggles CASS WRITE only on data '0's (NRZI transitions),
   so the number of bit-cells between two consecutive edges, rounded from the
   CPU-cycle gap, is (run of 1s) + 1. Returns that cell count (>= 1); the bits
   represented are (count - 1) ones followed by one zero. */
int cascom_nrzi_gap_cells(double gap_cycles, double cycles_per_bit);

#endif
