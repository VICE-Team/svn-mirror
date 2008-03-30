/*
 * ciad.h - Drive CIA definitions.
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

#ifndef _CIAD_H
#define _CIAD_H

#include "types.h"

struct cia_context_s;
struct drive_context_s;

extern void cia1571_setup_context(struct drive_context_s *ctxptr);
extern void cia1581_setup_context(struct drive_context_s *ctxptr);

extern void cia1571_init(struct drive_context_s *ctxptr);
extern void REGPARM3 cia1571_store(struct drive_context_s *ctxptr,
                                   WORD addr, BYTE value);
extern BYTE REGPARM2 cia1571_read(struct drive_context_s *ctxptr,
                                  WORD addr);
extern BYTE REGPARM2 cia1571_peek(struct drive_context_s *ctxptr,
                                  WORD addr);

extern void cia1581_init(struct drive_context_s *ctxptr);
extern void REGPARM3 cia1581_store(struct drive_context_s *ctxptr,
                                   WORD addr, BYTE value);
extern BYTE REGPARM2 cia1581_read(struct drive_context_s *ctxptr,
                                  WORD addr);
extern BYTE REGPARM2 cia1581_peek(struct drive_context_s *ctxptr,
                                  WORD addr);

typedef struct cia_initdesc_s {
    struct cia_context_s *cia_ptr;
    void (*clk)(CLOCK, void*);
    void (*int_ta)(CLOCK);
    void (*int_tb)(CLOCK);
    void (*int_tod)(CLOCK);
} cia_initdesc_t;

/* init callbacks, shared by both cias; defined in cia1571d. */
extern void cia_drive_init(struct drive_context_s *ctxptr,
                           const cia_initdesc_t *cia_desc);

#endif

