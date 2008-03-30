/*
 * tpid.h
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _TPID_H
#define _TPID_H

#include "types.h"

struct drive_context_s;
struct drivetpi_context_s;
struct snapshot_s;

extern void tpid_setup_context(struct drive_context_s *ctxptr);

extern void tpid_init(struct drive_context_s *ctxptr);
extern void tpid_reset(struct drive_context_s *ctxptr);
extern void REGPARM3 tpid_store(struct drive_context_s *ctxptr,
                                WORD addr, BYTE byte);
extern BYTE REGPARM2 tpi_read(struct drive_context_s *ctxptr, WORD addr);
extern int tpid_snapshot_write_module(struct drive_context_s *ctxptr,
                                      struct snapshot_s *p);
extern int tpid_snapshot_read_module(struct drive_context_s *ctxptr,
                                     struct snapshot_s *p);
#endif

