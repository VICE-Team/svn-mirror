/*
 * viad.h - 1541 VIA definitions.
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

extern void set_atn(BYTE state);

extern void reset_viaD1(void);
extern void viaD1_signal(int line, int edge);
extern void REGPARM2 store_viaD1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_viaD1(ADDRESS addr);
extern int int_viaD1t1(long offset);
extern int int_viaD1t2(long offset);
extern void viaD1_prevent_clk_overflow(void);

extern void reset_viaD2(void);
extern void viaD2_signal(int line, int edge);
extern void REGPARM2 store_viaD2(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_viaD2(ADDRESS addr);
extern int int_viaD2t1(long offset);
extern int int_viaD2t2(long offset);
extern void viaD2_prevent_clk_overflow(void);

#endif
