/*
 * via.h - VIA emulation.
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

#ifndef _VIC20_VIA_H
#define _VIC20_VIA_H

#include "types.h"

struct snapshot_s;

extern void via1_init(void);
extern void via1_reset(void);
extern void via1_signal(int line, int edge);
extern void REGPARM2 via1_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 via1_read(WORD addr);
extern BYTE REGPARM1 via1_peek(WORD addr);
extern int via1_snapshot_write_module(struct snapshot_s *p);
extern int via1_snapshot_read_module(struct snapshot_s *p);

extern void via2_init(void);
extern void via2_reset(void);
extern void via2_signal(int line, int edge);
extern void REGPARM2 via2_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 via2_read(WORD addr);
extern BYTE REGPARM1 via2_peek(WORD addr);
extern int via2_snapshot_write_module(struct snapshot_s *p);
extern int via2_snapshot_read_module(struct snapshot_s *p);
extern void via2_set_tape_sense(int v);

#endif

