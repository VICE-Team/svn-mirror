/*
 * types.h - Type definitions for VICE.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <boose@unixserv.rz.fh-hannover.de>
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

#ifndef _TYPES_H
#define _TYPES_H

#include <limits.h>

#define BYTE unsigned char

typedef signed char SIGNED_CHAR;

#if SIZEOF_UNSIGNED_SHORT == 2
typedef unsigned short WORD;
typedef signed short SWORD;
#elif SIZEOF_UNSIGNED_INT == 2
typedef unsigned int WORD;
typedef signed int SWORD;
#else
#error Cannot find a proper 16-bit type!
#endif

#if SIZEOF_UNSIGNED_INT == 4
typedef unsigned int DWORD;
typedef signed int SDWORD;
#elif SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DWORD;
typedef signed long SWORD;
#else
#error Cannot find a proper 32-bit type!
#endif

typedef WORD ADDRESS;

typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

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

#define REGPARM1
#define REGPARM2
#define REGPARM3

#include <stdio.h>
typedef FILE *file_desc_t;
#define ILLEGAL_FILE_DESC      NULL
extern file_desc_t open(const char *file, int oflag, ...);
extern int lseek(file_desc_t fd, int offset, int whence);
extern int read(file_desc_t fd, void *buf, int nbytes);
extern int write(file_desc_t fd, void *buf, int nbytes);
extern int close(file_desc_t fd);
extern file_desc_t fileno(file_desc_t);

#endif  /* _TYPES_H */

