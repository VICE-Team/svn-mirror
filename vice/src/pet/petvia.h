/*
 * petvia.h - PET VIA emulation.
 *
 * Written by
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

#ifndef _PET_VIA_H
#define _PET_VIA_H

#include "types.h"

void reset_via(void);
BYTE REGPARM1 read_via(ADDRESS addr);
BYTE REGPARM1 peek_via(ADDRESS addr);
void REGPARM2 store_via(ADDRESS addr, BYTE value);
int int_viat1(long offset);
int int_viat2(long offset);
void via_prevent_clk_overflow(CLOCK sub);

extern int via_read_snapshot_module(snapshot_t *);
extern int via_write_snapshot_module(snapshot_t *);

#endif
