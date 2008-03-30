/*
 * via.h - VIA emulation.
 *
 * Written by
 *   Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _VIC20_VIA_H
#define _VIC20_VIA_H

#include "via.h"

extern void reset_via1(void);
extern void via1_signal(int line, int edge);
extern void REGPARM2 store_via1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via1(ADDRESS addr);
extern BYTE REGPARM1 peek_via1(ADDRESS addr);
extern int int_via1t1(long offset);
extern int int_via1t2(long offset);
extern void via1_prevent_clk_overflow(CLOCK sub);

extern void reset_via2(void);
extern void via2_signal(int line, int edge);
extern void REGPARM2 store_via2(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via2(ADDRESS addr);
extern BYTE REGPARM1 peek_via2(ADDRESS addr);
extern int int_via2t1(long offset);
extern int int_via2t2(long offset);
extern void via2_prevent_clk_overflow(CLOCK sub);

#endif  /* _VIA_H */

