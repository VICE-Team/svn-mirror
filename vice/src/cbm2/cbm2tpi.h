/*
 * cbm2tpi.h - IEEE488 and keyboard interface for the CBM-II
 *
 * Written by
 *  Andre' Fachat <a.fachat@physik.tu-chemnitz.de>
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

#ifndef _CBM2TPI_H
#define _CBM2TPI_H

#include "types.h"

struct snapshot_s;

extern void tpi1_init(void);
extern void tpi1_reset(void);
extern void REGPARM2 tpi1_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 tpi1_read(ADDRESS addr);
extern BYTE tpi1_peek(ADDRESS addr);
extern void tpi1_set_int(int bit, int state);
extern void tpi1_restore_int(int bit, int state);
extern void tpi1_set_tape_sense(int v);
extern int tpi1_snapshot_write_module(struct snapshot_s *p);
extern int tpi1_snapshot_read_module(struct snapshot_s *p);

extern void tpi2_init(void);
extern void tpi2_reset(void);
extern void REGPARM2 tpi2_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 tpi2_read(ADDRESS addr);
extern BYTE tpi2_peek(ADDRESS addr);
extern void tpi2_set_int(int bit, int state);
extern void tpi2_restore_int(int bit, int state);
extern int tpi2_snapshot_write_module(struct snapshot_s *p);
extern int tpi2_snapshot_read_module(struct snapshot_s *p);

extern void set_cbm2_model_port_mask(BYTE);

#endif

