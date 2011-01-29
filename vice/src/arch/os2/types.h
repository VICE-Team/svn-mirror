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
#ifndef VICE_TYPES_H
#define VICE_TYPES_H

#include <limits.h>

#ifdef __IBMC__
#define getcwd _getcwd
#define F_OK 0
#define W_OK 2
#define R_OK 4
#define X_OK 6
typedef int pid_t;

#ifndef __EXTENDED__
typedef long off_t;
#endif

#define STDOUT_FILENO (fileno(stdout) & 0xffff)
#define STDERR_FILENO (fileno(stderr) & 0xffff)
#define _O_BINARY O_BINARY
#define _O_TRUNC O_TRUNC
#define _O_WRONLY O_WRONLY
#define _O_CREAT O_CREAT
#define _P_WAIT P_WAIT
#endif

#ifdef __EMX__
#define vfork fork
#endif

typedef signed char SIGNED_CHAR;

/* Definitions see os2mdef.h */
#ifndef BYTE
typedef char BYTE;
#endif

typedef unsigned short WORD;
typedef signed short SWORD;
typedef signed long SDWORD;
typedef unsigned long DWORD;

typedef DWORD CLOCK;

/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

#define vice_ptr_to_int(x) ((int)(long)(x))
#define vice_ptr_to_uint(x) ((unsigned int)(unsigned long)(x))
#define int_to_void_ptr(x) (x)
#define uint_to_void_ptr(x) (x)

#endif  /* _VICE_TYPES_H */
