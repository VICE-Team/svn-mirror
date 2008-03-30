/*
 * debug.h - Various debugging options.
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

#ifndef _DEBUG_H
#define _DEBUG_H

#include "types.h"

/* This enables debugging.  Attention: It makes things a bit slower.  */
#define DEBUG

typedef struct debug_s {
#ifdef DEBUG
    int maincpu_traceflg;
    int drivecpu_traceflg[2];
#endif
    int do_core_dumps;
} debug_t;

extern debug_t debug;

extern int debug_resources_init(void);
extern int debug_cmdline_options_init(void);

extern void debug_set_machine_parameter(unsigned int cycles,
                                        unsigned int lines);
extern void debug_maincpu(DWORD reg_pc, CLOCK mclk, const char *dis,
                          BYTE reg_a, BYTE reg_x, BYTE reg_y, BYTE reg_sp);

#endif

