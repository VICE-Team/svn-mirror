/*
 * mc6821.h - MC6821 emulation for the 1571 disk drives with DD3.
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

#ifndef VICE_MC6821_H
#define VICE_MC6821_H

#include "types.h"

/* Signal values (for signaling edges on the control lines)  */
#define MC6821_SIG_CA1 0
#define MC6821_SIG_CA2 1
#define MC6821_SIG_CB1 2
#define MC6821_SIG_CB2 3

struct drive_s;

struct mc6821_s {
    /* MC6821 register.  */
    BYTE pra;
    BYTE ddra;
    BYTE cra;
    BYTE prb;
    BYTE ddrb;
    BYTE crb;

    /* Drive structure */
    struct drive_s *drive;
};
typedef struct mc6821_s mc6821_t;

struct drive_context_s;
extern void mc6821_init(struct drive_context_s *drv);
extern void mc6821_reset(struct drive_context_s *drv);
extern void mc6821_mem_init(struct drive_context_s *drv, unsigned int type);

extern void mc6821_set_signal(struct drive_context_s *drive_context, int line);

#endif 

