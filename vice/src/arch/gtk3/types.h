/*
 * types.h - Type definitions for VICE.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_TYPES_H
#define VICE_TYPES_H

#include "vice.h"

typedef signed char SIGNED_CHAR;

#ifdef _WIN32
#include <WinDef.h>
#endif

/* FIXME: we should completely get rid of BYTE,WORD,DWORD etc in VICE code */
#define BYTE unsigned char

#if SIZEOF_UNSIGNED_SHORT == 2
#ifndef _WIN32
typedef unsigned short WORD;
#endif
typedef signed short SWORD;
#else
#error Cannot find a proper 16-bit type!
#endif

#if SIZEOF_UNSIGNED_INT == 4
#ifndef _WIN32
typedef unsigned int DWORD;
#endif
typedef signed int SDWORD;
#elif SIZEOF_UNSIGNED_LONG == 4
#ifndef _WIN32
typedef unsigned long DWORD;
#endif
typedef signed long SDWORD;
#else
#error Cannot find a proper 32-bit type!
#endif

typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

/* FIXME: these can probably be global and the same for all ports (that
          have C99) */
#define vice_ptr_to_int(x) ((int)(intptr_t)(x))
#define vice_ptr_to_uint(x) ((unsigned int)(uintptr_t)(x))
#define int_to_void_ptr(x) ((void *)(intptr_t)(x))
#define uint_to_void_ptr(x) ((void *)(uintptr_t)(x))

#endif
