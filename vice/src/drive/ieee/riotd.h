/*
 * riotd.h - Drive VIA definitions.
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

#ifndef _RIOTD_H
#define _RIOTD_H

#include "types.h"

struct drive_context_s;
struct driveriot_context_s;
struct snapshot_s;

extern void riot1_setup_context(struct drive_context_s *ctxptr);
extern void riot2_setup_context(struct drive_context_s *ctxptr);

extern void drive_riot_set_atn(struct drive_context_s *ctxptr, int state);
extern void riot1_set_atn(struct drive_context_s *ctxptr, BYTE state);

extern void riot1_set_pardata(struct drive_context_s *ctxptr);

extern void riot1_init(struct drive_context_s *ctxptr);
extern void riot1_signal(struct drive_context_s *ctxptr, int sig, int type);
extern void riot1_reset(struct drive_context_s *ctxptr);
extern void REGPARM3 riot1_store(struct drive_context_s *ctxptr,
                                 ADDRESS addr, BYTE byte);
extern BYTE REGPARM2 riot1_read(struct drive_context_s *ctxptr, ADDRESS addr);
extern int riot1_snapshot_write_module(struct drive_context_s *ctxptr,
                                       struct snapshot_s *p);
extern int riot1_snapshot_read_module(struct drive_context_s *ctxptr,
                                      struct snapshot_s *p);

extern void riot2_init(struct drive_context_s *ctxptr);
extern void riot2_signal(struct drive_context_s *ctxptr, int sig, int type);
extern void riot2_reset(struct drive_context_s *ctxptr);
extern void REGPARM3 riot2_store(struct drive_context_s *ctxptr,
                                 ADDRESS addr, BYTE byte);
extern BYTE REGPARM2 riot2_read(struct drive_context_s *ctxptr, ADDRESS addr);
extern int riot2_snapshot_write_module(struct drive_context_s *ctxptr,
                                       struct snapshot_s *p);
extern int riot2_snapshot_read_module(struct drive_context_s *ctxptr,
                                      struct snapshot_s *p);

typedef struct riot_initdesc_s {
    struct driveriot_context_s *riot_ptr;
    void (*clk)(CLOCK, void*);
    void (*int_t1)(CLOCK);
} riot_initdesc_t;

/* init callbacks, shared by both riots; defined in riot1 */
extern void riot_drive_init(struct drive_context_s *ctxptr,
                            const riot_initdesc_t *riot_desc);

#define drive0_riot_set_atn(s)  drive_riot_set_atn(&drive0_context, s)
#define drive1_riot_set_atn(s)  drive_riot_set_atn(&drive1_context, s)

/* debug without RIOT code */
#if 0
#define riot1_init(c)
#define reset_riot1(c)
#define store_riot1(c,a,b)
#define read_riot1(c,a)                         0xff
#define riot1_write_snapshot_module(c,a)        0
#define riot1_read_snapshot_module(c,a)         0

#define riot2_init(c)
#define reset_riot2(c)
#define store_riot2(c,a,b)
#define read_riot2(c,a)                         0xff
#define riot2_write_snapshot_module(c,a)        0
#define riot2_read_snapshot_module(c,a)         0

#endif

#endif

