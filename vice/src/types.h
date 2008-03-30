/*
 * types.h - Generic type definitions for VICE.
 *
 * Written by
 *  Jarkko Sonninen  (sonninen@lut.fi)
 *  Jouko Valta      (jopi@stekt.oulu.fi)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat     (a.fachat@physik.tu-chemnitz.de)
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *
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

#ifndef _VICE_TYPES_H
#define _VICE_TYPES_H

#include <limits.h>

typedef signed char SIGNED_CHAR;
typedef unsigned char BYTE;

#if SIZEOF_UNSIGNED_SHORT == 2
typedef unsigned short WORD;
typedef signed short SWORD;
#elif SIZEOF_UNSIGNED_INT == 2
typedef unsigned int WORD;
typedef signed int SWORD;
#else
#error Cannot find a proper 16-bit type!
#endif

typedef WORD ADDRESS;

#if SIZEOF_UNSIGNED_INT == 4
typedef unsigned int DWORD;
typedef signed int SDWORD;
#elif SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DWORD;
typedef signed long SWORD;
#else
#error Cannot find a proper 32-bit type!
#endif

typedef DWORD CLOCK;

#if X_DISPLAY_DEPTH == 16
typedef WORD		  		PIXEL;
#elif X_DISPLAY_DEPTH == 24
typedef DWORD				PIXEL;
#elif X_DISPLAY_DEPTH == 8 || X_DISPLAY_DEPTH == 0
typedef BYTE		PIXEL;
#else
#error Unsupported display depth!
#endif

#if X_DISPLAY_DEPTH == 8 || X_DISPLAY_DEPTH == 0
typedef WORD		PIXEL2;
typedef DWORD		PIXEL4;
#else
typedef struct { PIXEL a, b; }		PIXEL2;
typedef struct { PIXEL a, b, c, d; }	PIXEL4;
#endif

#if defined(__GNUC__) && defined(__i386__) && !defined(NO_REGPARM)
#define REGPARM1 __attribute__((regparm(1)))
#define REGPARM2 __attribute__((regparm(2)))
#define REGPARM3 __attribute__((regparm(3)))
#else
#define REGPARM1
#define REGPARM2
#define REGPARM3
#endif

#endif  /* _VICE_TYPES_H */
