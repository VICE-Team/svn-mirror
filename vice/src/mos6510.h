/*
 * mos6510.h - Definitions for emulation of the 6510 processor.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _MOS6510_H
#define _MOS6510_H

#include "types.h"

/* Struct for the 6510 registers.  */
typedef struct _mos6510_regs {
    ADDRESS pc;
    BYTE a;
    BYTE x;
    BYTE y;
    BYTE sp;
    struct {
        int z;
        int n;
        int v;
        int b;
        int d;
        int i;
        int c;
    } p;
} mos6510_regs_t;

/* These define the position of the status flags in the P (`status')
   register.  */
#define S_SIGN		0x80
#define S_OVERFLOW	0x40
#define S_UNUSED	0x20
#define S_BREAK		0x10
#define S_DECIMAL	0x08
#define S_INTERRUPT	0x04
#define S_ZERO		0x02
#define S_CARRY		0x01

/* Mmmh...  It would be much nicer to have a union instead.  */
#define MOS6510_GET_P(r)                        \
    ((r.p.z ? S_ZERO : 0)                       \
     | (r.p.n ? S_SIGN : 0)                     \
     | (r.p.v ? S_OVERFLOW : 0)                 \
     | (r.p.b ? S_BREAK : 0)                    \
     | (r.p.d ? S_DECIMAL : 0)                  \
     | (r.p.i ? S_INTERRUPT : 0)                \
     | (r.p.c ? S_CARRY : 0))

#endif
