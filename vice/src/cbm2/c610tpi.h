/*
 * tpi.h - IEEE488 and keyboard interface for the C610
 *
 * Written by
 *   Andre' Fachat (a.fachat@physik.tu-chemnitz.de)
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

#ifndef _C610TPI_H
#define _C610TPI_H

extern void tpi1_init(void);
extern void reset_tpi1(void);
extern void store_tpi1(ADDRESS addr, BYTE byte);
extern BYTE read_tpi1(ADDRESS addr);
extern BYTE peek_tpi1(ADDRESS addr);
extern void tpi1_set_int(int bit, int state);
extern void tpi1_restore_int(int bit, int state);
extern void tpi1_set_tape_sense(int v);
extern int tpi1_write_snapshot_module(snapshot_t *p);
extern int tpi1_read_snapshot_module(snapshot_t *p);

extern void tpi2_init(void);
extern void reset_tpi2(void);
extern void store_tpi2(ADDRESS addr, BYTE byte);
extern BYTE read_tpi2(ADDRESS addr);
extern BYTE peek_tpi2(ADDRESS addr);
extern void tpi2_set_int(int bit, int state);
extern void tpi2_restore_int(int bit, int state);
extern int tpi2_write_snapshot_module(snapshot_t *p);
extern int tpi2_read_snapshot_module(snapshot_t *p);

extern void set_cbm2_model_port_mask(BYTE);

#endif /* _C610TPI_H */
