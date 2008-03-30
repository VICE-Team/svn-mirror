/*
 * acia-tmpl.c - Template file for ACIA 6551 emulation.
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

/* ACIA 6551 register number declarations.
 * Interrupt signals are defined in vmachine.h. */

#ifndef _C610ACIA_H
#define _C610ACIA_H

#include "types.h"

#include "snapshot.h"

extern BYTE REGPARM1 read_acia1(ADDRESS a);
extern BYTE REGPARM1 peek_acia1(ADDRESS a);
extern void REGPARM2 store_acia1(ADDRESS a,BYTE b);
extern void reset_acia1(void);
extern int int_acia1(long offset);

extern int acia1_init_cmdline_options(void);
extern int acia1_init_resources(void);

extern int acia1_write_snapshot_module(snapshot_t *p);
extern int acia1_read_snapshot_module(snapshot_t *p);

#endif
