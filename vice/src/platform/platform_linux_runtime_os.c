/*
 * platform_linux_runtime_os.c - Linux runtime version discovery.
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

/* Tested and confirmed working on:
*/

/* cpu | libc
   -----------
   x86 | libc4
   x86 | libc5
   x86 | glibc1.*
   x86 | glibc2.*
   x86 | dietlibc
   x86 | newlib
 */

#include "vice.h"

#ifdef __linux

#include <stdio.h>
#include <ctype.h>

#if defined(__GLIBC__) && (__GLIBC__==2) && (__GLIBC__MINOR__>0)
#  include <gnu/libc-version.h>
#endif

char *platform_get_linux_runtime_os(void)
{
#ifdef __dietlibc__
#define CLIB_HANDLED
    return "Linux dietlibc";
#endif

#if !defined(CLIB_HANDLED) && defined(_NEWLIB_VERSION)
#define CLIB_HANDLED
    return "Linux newlib " _NEWLIB_VERSION;
#endif

#if !defined(CLIB_HANDLED) && defined(__GLIBC__)
#  define CLIB_HANDLED
#  if (__GLIBC__==2)
#    if (__GLIBC__MINOR__>0)
    char linuxlibc[40];
    sprintf(linuxlibc, "Linux glibc %s", gnu_get_libc_version();
    return linuxlibc;
#    else
       return "Linux glibc 2.x";
#    endif
#  else
    return "Linux glibc 1.x";
#  endif
#endif

#if !defined(CLIB_HANDLED) && defined(_LINUX_C_LIB_VERSION)
#  define CLIB_HANDLED
    return _LINUX_C_LIB_VERSION;
#endif

#if !defined(CLIB_HANDLED) && (VICE_LINUX_CLIB_VERSION_MAJOR==1)
#  define CLIB_HANDLED
    return "Linux glibc 1.x";
#endif

#if !defined(CLIB_HANDLED) && (VICE_LINUX_CLIB_VERSION_MAJOR==6)
#  define CLIB_HANDLED
    return "Linux glibc 2.x";
#endif

#ifndef CLIB_HANDLED
#  include <sys/ucontext.h>
#  ifdef _UCONTEXT_H
#    define CLIB_HANDLED
#    return "Linux musl";
#  endif
#endif

#ifndef CLIB_HANDLED
    return "Unknown libc version";
#endif
}

#endif
