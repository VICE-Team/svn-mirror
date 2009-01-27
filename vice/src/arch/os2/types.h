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

//#undef VERSION
//#include <os2.h>
//#include <os2me.h>
//#undef VERSION

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef __IBMC__
 #define inline _Inline  // This means only a possible inline. See doku.
 #define getcwd _getcwd
 #define F_OK   0
 #define W_OK   2
 #define R_OK   4
 #define X_OK   6
 typedef int pid_t;
 #ifndef __EXTENDED__
 typedef long off_t;
 #endif
 #define STDOUT_FILENO (fileno(stdout) & 0xffff)
 #define STDERR_FILENO (fileno(stderr) & 0xffff)
 #define _O_BINARY O_BINARY
 #define _O_TRUNC  O_TRUNC
 #define _O_WRONLY O_WRONLY
 #define _O_CREAT  O_CREAT
 #define _P_WAIT   P_WAIT
#endif

#ifdef __EMX__
  #define vfork fork
#endif

/* sizeof(char)  = 1 */
/* sizeof(short) = 2 */
/* sizeof(int)   = 4 */
/* sizeof(long)  = 4 */

typedef signed char SIGNED_CHAR;

/* Definitions see os2mdef.h */
#ifndef BYTE
typedef char    BYTE;
#endif
typedef unsigned short   WORD;
typedef   signed short  SWORD;
typedef   signed long  SDWORD;
typedef unsigned long   DWORD;
//#define BYTE  unsigned char
//#define WORD  unsigned short
//#define DWORD unsigned long


typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

/*#if defined(__GNUC__) && defined(__i386__) && !defined(NO_REGPARM) && !defined(OS2)
#define REGPARM1 __attribute__((regparm(1)))
#define REGPARM2 __attribute__((regparm(2)))
#define REGPARM3 __attribute__((regparm(3)))
#else*/
#define REGPARM1
#define REGPARM2
#define REGPARM3
//#endif

#endif  /* _VICE_TYPES_H */
