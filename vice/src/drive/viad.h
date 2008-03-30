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
struct snapshot_s;

extern void via1d_setup_context(struct drive_context_s *ctxptr);
extern void via2d_setup_context(struct drive_context_s *ctxptr);

extern void drive_via_set_atn(struct via_context_s *via_context, int state);
extern void via1_set_atn(struct drive_context_s *ctxptr, BYTE state);

extern void via1d_init(struct drive_context_s *ctxptr);
extern void via1d_reset(struct via_context_s *via_context);
extern void via1d_signal(struct via_context_s *via_context, int line, int edge);
extern void REGPARM3 via1d_store(struct drive_context_s *ctxptr, WORD addr,
                                 BYTE byte);
extern BYTE REGPARM2 via1d_read(struct drive_context_s *ctxptr, WORD addr);
extern BYTE REGPARM2 via1d_peek(struct drive_context_s *ctxptr, WORD addr);
extern int via1d_snapshot_write_module(struct via_context_s *via_context,
                                       struct snapshot_s *p);
extern int via1d_snapshot_read_module(struct via_context_s *via_context,
                                      struct snapshot_s *p);

extern void via2d_init(struct drive_context_s *ctxptr);
extern void via2d_reset(struct via_context_s *via_context);
extern void via2d_signal(struct via_context_s *via_context, int line, int edge);
extern void REGPARM3 via2d_store(struct drive_context_s *ctxptr, WORD addr,
                                 BYTE byte);
extern BYTE REGPARM2 via2d_read(struct drive_context_s *ctxptr, WORD addr);
extern BYTE REGPARM2 via2d_peek(struct drive_context_s *ctxptr, WORD addr);
extern int via2d_snapshot_write_module(struct via_context_s *via_context,
                                       struct snapshot_s *p);
extern int via2d_snapshot_read_module(struct via_context_s *via_context,
                                      struct snapshot_s *p);

typedef struct via_initdesc_s {
    struct via_context_s *via_ptr;
    void (*clk)(CLOCK, void*);
    void (*int_t1)(CLOCK);
    void (*int_t2)(CLOCK);
} via_initdesc_t;

/* init callbacks, shared by both vias; defined in via1d. */
extern void via_drive_init(struct drive_context_s *ctxptr,
                           const via_initdesc_t *via_desc);

#define drive0_via_set_atn(state) drive_via_set_atn(&(drive0_context.via1), \
                                  state)
#define drive1_via_set_atn(state) drive_via_set_atn(&(drive1_context.via1), \
                                  state)
#define via1d0_signal(l, e)       via1d_signal(&(drive0_context.via1), l, e)
#define via1d1_signal(l, e)       via1d_signal(&(drive1_context.via1), l, e)

extern void viad2_update_pcr(int pcrval, struct drive_s *dptr);

#endif

