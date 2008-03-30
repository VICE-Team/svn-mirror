/*
 * types.h - Generic type definitions for VICE/2.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_TYPES
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

#undef VERSION

#ifdef __IBMC__
 #define inline _Inline  // This means only a possible inline. See doku.
 #define getcwd _getcwd
 #define R_OK   4
 #define W_OK   2
 typedef int pid_t;
 #ifndef __EXTENDED__
 typedef long off_t;
 #endif
 #define STDOUT_FILENO (0xFFFF & fileno(stdout))
 #define STDERR_FILENO (0xFFFF & fileno(stderr))
 #define _O_BINARY O_BINARY
 #define _O_TRUNC  O_TRUNC
 #define _O_WRONLY O_WRONLY
 #define _O_CREAT  O_CREAT
 #define _P_WAIT   P_WAIT
#endif

#ifdef __EMX__
  #define vfork fork
#endif

typedef signed char SIGNED_CHAR;

typedef unsigned short   WORD;
typedef   signed short  SWORD;
typedef   signed int   SDWORD;
typedef unsigned long   DWORD;

#undef ADDRESS
typedef WORD ADDRESS;

typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

#if X_DISPLAY_DEPTH == 16
typedef WORD		  		PIXEL;
#elif X_DISPLAY_DEPTH == 24
typedef DWORD				PIXEL;
#elif X_DISPLAY_DEPTH == 8 || X_DISPLAY_DEPTH == 0
//#ifndef OS2
#undef PIXEL
typedef BYTE		PIXEL;
//#endif
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

#if defined(__GNUC__) && defined(__i386__) && !defined(NO_REGPARM) && !defined(OS2)
#define REGPARM1 __attribute__((regparm(1)))
#define REGPARM2 __attribute__((regparm(2)))
#define REGPARM3 __attribute__((regparm(3)))
#else
#define REGPARM1
#define REGPARM2
#define REGPARM3
#endif

/* Use ANSI IO on RISC OS */
typedef int file_desc_t;
#define ILLEGAL_FILE_DESC      -1

#endif  /* _VICE_TYPES_H */
