/*
 * vic20ieeevia.h - VIA emulation.
 *
 * Written by
 *   Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef _VIC20_IEEE_VIA_H
#define _VIC20_IEEE_VIA_H

#include "types.h"

struct snapshot_s;

extern void ieeevia1_init(void);
extern void ieeevia1_reset(void);
extern void ieeevia1_signal(int line, int edge);
extern void REGPARM2 ieeevia1_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 ieeevia1_read(ADDRESS addr);
extern BYTE REGPARM1 ieeevia1_peek(ADDRESS addr);
extern int ieeevia1_write_snapshot_module(struct snapshot_s *p);
extern int ieeevia1_read_snapshot_module(struct snapshot_s *p);

extern void ieeevia2_init(void);
extern void ieeevia2_reset(void);
extern void ieeevia2_signal(int line, int edge);
extern void REGPARM2 ieeevia2_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 ieeevia2_read(ADDRESS addr);
extern BYTE REGPARM1 ieeevia2_peek(ADDRESS addr);
extern int ieeevia2_write_snapshot_module(struct snapshot_s *p);
extern int ieeevia2_read_snapshot_module(struct snapshot_s *p);
extern void ieeevia2_set_tape_sense(int v);

#endif

