/*
 * traps.h - Allow VICE to replace ROM code with C function calls.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
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

#ifndef _TRAPS_H
#define _TRAPS_H

#include "mem.h"
#include "types.h"

typedef struct trap_s
{
    const char *name;
    ADDRESS address;
    ADDRESS resume_address;
    BYTE check[3];
#if defined(__STDC__) || defined(__IBMC__)
    void (*func)(void);
#else
    void (*func)();
#endif
    read_func_t *readfunc;
    store_func_t *storefunc;
} trap_t;

extern void traps_init(void);
extern int traps_resources_init(void);
extern int traps_cmdline_options_init(void);
extern int traps_add(const trap_t *t);
extern int traps_remove(const trap_t *t);
extern int traps_handler(void);

#endif

