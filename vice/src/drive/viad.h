/*
 * viad.h - Drive VIA definitions.
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#ifndef _VIAD_H
#define _VIAD_H

#include "types.h"
#include "via.h"

extern void via1d0_set_atn(BYTE state);
extern void via1d1_set_atn(BYTE state);

extern void reset_via1d0(void);
extern void via1d0_signal(int line, int edge);
extern void REGPARM2 store_via1d0(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via1d0(ADDRESS addr);
extern int int_via1d0t1(long offset);
extern int int_via1d0t2(long offset);
extern void via1d0_prevent_clk_overflow(CLOCK sub);

extern void reset_via2d0(void);
extern void via2d0_signal(int line, int edge);
extern void REGPARM2 store_via2d0(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via2d0(ADDRESS addr);
extern int int_via2d0t1(long offset);
extern int int_via2d0t2(long offset);
extern void via2d0_prevent_clk_overflow(CLOCK sub);

extern void reset_via1d1(void);
extern void via1d1_signal(int line, int edge);
extern void REGPARM2 store_via1d1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via1d1(ADDRESS addr);
extern int int_via1d1t1(long offset);
extern int int_via1d1t2(long offset);
extern void via1d1_prevent_clk_overflow(CLOCK sub);

extern void reset_via2d1(void);
extern void via2d1_signal(int line, int edge);
extern void REGPARM2 store_via2d1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via2d1(ADDRESS addr);
extern int int_via2d1t1(long offset);
extern int int_via2d1t2(long offset);
extern void via2d1_prevent_clk_overflow(CLOCK sub);

#endif

