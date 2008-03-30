/*
 * tpi.h - IEEE488 interface for the C64.
 *
 * Written by 
 *   Andre' Fachat <a.fachat@physik.tu-chemnitz.de>
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

#ifndef _C64TPI_H
#define _C64TPI_H

#include "types.h"

struct snapshot_s;

extern void tpi_init(void);
extern void tpi_reset(void);
extern void REGPARM2 tpi_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 tpi_read(ADDRESS addr);
extern BYTE tpi_peek(ADDRESS addr);

extern int tpi_write_snapshot_module(struct snapshot_s *p);
extern int tpi_read_snapshot_module(struct snapshot_s *p);

#endif

