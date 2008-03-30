/*
 * viad.h - Drive VIA definitions.
 *
 * Written by
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef _VIAD_H
#define _VIAD_H

#include "drivetypes.h"
#include "types.h"

struct drive_context_s;
struct drive_s;
struct via_context_s;

extern void via1d1541_setup_context(struct drive_context_s *ctxptr);
extern void via1d2031_setup_context(struct drive_context_s *ctxptr);
extern void via2d_setup_context(struct drive_context_s *ctxptr);

extern void via1d1541_init(struct drive_context_s *ctxptr);
extern void via1d2031_init(struct drive_context_s *ctxptr);
extern void REGPARM3 via1d1541_store(struct drive_context_s *ctxptr, WORD addr,
                                     BYTE byte);
extern BYTE REGPARM2 via1d1541_read(struct drive_context_s *ctxptr, WORD addr);
extern BYTE REGPARM2 via1d1541_peek(struct drive_context_s *ctxptr, WORD addr);
extern void REGPARM3 via1d2031_store(struct drive_context_s *ctxptr, WORD addr,
                                     BYTE byte);
extern BYTE REGPARM2 via1d2031_read(struct drive_context_s *ctxptr, WORD addr);
extern BYTE REGPARM2 via1d2031_peek(struct drive_context_s *ctxptr, WORD addr);

extern void via2d_init(struct drive_context_s *ctxptr);
extern void REGPARM3 via2d_store(struct drive_context_s *ctxptr, WORD addr,
                                 BYTE byte);
extern BYTE REGPARM2 via2d_read(struct drive_context_s *ctxptr, WORD addr);
extern BYTE REGPARM2 via2d_peek(struct drive_context_s *ctxptr, WORD addr);

extern void via1d2031_set_atn(struct via_context_s *via_context, int state);

extern void viad2_update_pcr(int pcrval, struct drive_s *dptr);

#endif

