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

#undef VERSION
#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_GPIBITMAPS
#define INCL_WINPALETTE
#define INCL_DOSPROCESS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINRECTANGLES
#include <os2.h>
#define INCL_OS2MM
#include <os2me.h>

typedef signed char SIGNED_CHAR;

// 16bit type
typedef signed short SWORD;

// 32bit type
typedef signed int SDWORD;

#undef ADDRESS
typedef WORD ADDRESS;

typedef DWORD CLOCK;

/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

typedef BYTE  PIXEL;
typedef WORD  PIXEL2;
typedef DWORD PIXEL4;

#if defined(__GNUC__) && defined(__i386__) && !defined(NO_REGPARM)
#define REGPARM1 __attribute__((regparm(1)))
#define REGPARM2 __attribute__((regparm(2)))
#define REGPARM3 __attribute__((regparm(3)))
#else
#define REGPARM1
#define REGPARM2
#define REGPARM3
#endif

typedef int file_desc_t;
#define ILLEGAL_FILE_DESC      -1

#endif  /* _VICE_TYPES_H */
