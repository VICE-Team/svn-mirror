/*
 * traps.h - Allow VICE to replace ROM code with C function calls.
 *
 * Written by
 *   Teemu Rantanen (tvr@cs.hut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
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

#include "types.h"

typedef struct trap_s
{
    char        *name;
    ADDRESS      address;
    ADDRESS      resume_address;
    BYTE         check[3];
#ifdef __STDC__
    void       (*func)(void);
#else
    void       (*func)();
#endif
} trap_t;

extern void traps_init(void);
extern int traps_init_resources(void);
extern int traps_init_cmdline_options(void);
extern int traps_add(const trap_t *t);
extern int traps_remove(const trap_t *t);
extern int traps_handler(void);

#endif
