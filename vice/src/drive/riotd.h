/*
 * riotd.h - Drive VIA definitions.
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

#ifndef _RIOTD_H
#define _RIOTD_H

#include "types.h"
#include "snapshot.h"

extern void riot1d0_set_atn(BYTE state);
extern void riot1d1_set_atn(BYTE state);

void riot1d0_set_pardata(void);
void riot1d1_set_pardata(void);

extern void riot1d0_init(void);
extern void riot1d0_signal(int sig, int type);
extern void riot1d0_reset(void);
extern void REGPARM2 store_riot1d0(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_riot1d0(ADDRESS addr);
extern int riot1d0_write_snapshot_module(snapshot_t * p);
extern int riot1d0_read_snapshot_module(snapshot_t * p);

extern void riot2d0_init(void);
extern void riot2d0_signal(int sig, int type);
extern void riot2d0_reset(void);
extern void REGPARM2 store_riot2d0(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_riot2d0(ADDRESS addr);
extern int riot2d0_write_snapshot_module(snapshot_t * p);
extern int riot2d0_read_snapshot_module(snapshot_t * p);

extern void riot1d1_init(void);
extern void riot1d1_signal(int sig, int type);
extern void riot1d1_reset(void);
extern void REGPARM2 store_riot1d1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_riot1d1(ADDRESS addr);
extern int riot1d1_write_snapshot_module(snapshot_t * p);
extern int riot1d1_read_snapshot_module(snapshot_t * p);

extern void riot2d1_init(void);
extern void riot2d1_signal(int sig, int type);
extern void riot2d1_reset(void);
extern void REGPARM2 store_riot2d1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_riot2d1(ADDRESS addr);
extern int riot2d1_write_snapshot_module(snapshot_t * p);
extern int riot2d1_read_snapshot_module(snapshot_t * p);

/* debug without RIOT code */
#if 0
#define	riot1d0_init()
#define	reset_riot1d0()
#define	store_riot1d0(a,b)
#define	read_riot1d0(a)				0xff
#define	riot1d0_write_snapshot_module(a)	0
#define	riot1d0_read_snapshot_module(a)		0

#define	riot1d1_init()
#define	riot1d1_reset()
#define	store_riot1d1(a,b)
#define	read_riot1d1(a)				0xff
#define	riot1d1_write_snapshot_module(a)	0
#define	riot1d1_read_snapshot_module(a)		0

#define	riot2d0_init()
#define	reset_riot2d0()
#define	store_riot2d0(a,b)
#define	read_riot2d0(a)				0xff
#define	riot2d0_write_snapshot_module(a)	0
#define	riot2d0_read_snapshot_module(a)		0

#define	riot2d1_init()
#define	riot2d1_reset()
#define	store_riot2d1(a,b)
#define	read_riot2d1(a)				0xff
#define	riot2d1_write_snapshot_module(a)	0
#define	riot2d1_read_snapshot_module(a)		0
#endif

#endif

