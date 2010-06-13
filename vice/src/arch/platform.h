/*
 * platform.h - port/platform specific discovery macros.
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

#ifndef VICE_PLATFORM_H
#define VICE_PLATFORM_H

#include "vice.h"

/* win32/64 discovery */
#ifdef WIN32_COMPILE
#  ifdef _WIN64
#    ifdef WINIA64
#      define PLATFORM_CPU "IA64"
#    else
#      define PLATFORM_CPU "X64"
#    endif
#    define PLATFORM_OS "WIN64"
#    define PLATFORM_COMPILER "MSVC"
#  else
#    ifdef MSVC_RC
#      ifdef WATCOM_COMPILE
#        define PLATFORM_COMPILER "WATCOM"
#      else
#        define PLATFORM_COMPILER "MSVC"
#      endif
#    else
#      define PLATFORM_COMPILER "GCC"
#    endif
#    define PLATFORM_OS "WIN32"
#    define FIND_X86_CPU
#  endif
#endif

#if !defined(WIN32_COMPILE) && defined(__CYGWIN32__)
#define PLATFORM_OS "Cygwin"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif

/* MacOS X discovery */
#ifdef __APPLE__
#   define PLATFORM_OS "Mac OS X"
#   ifdef __llvm__
#       define PLATFORM_COMPILER  "llvm"
#   else
#       define PLATFORM_COMPILER  "gcc"
#   endif
#   ifdef __POWERPC__
#       define PLATFORM_CPU "ppc"
#   else
#       ifdef __x86_64
#           define PLATFORM_CPU "x86_64"
#       else
#           define PLATFORM_CPU "i386"
#       endif
#   endif
#endif

/* AIX discovery */

#ifdef _AIX

/* find out which compiler is being used */
#ifdef __TOS_AIX__
#  define PLATFORM_COMPILER "xLC"
#else
#  define PLATFORM COMPILER "GCC"
#endif

/* Get AIX version */
#include "platform_aix_version.h"

#endif /* AIX */


/* AmigaOS 4.x discovery */
#ifdef AMIGA_OS4
#define PLATFORM_OS "AmigaOS 4.x"
#define PLATFORM_COMPILER "GCC"
#define PLATFORM_CPU "PPC"
#endif


/* AROS discovery */
#ifdef AMIGA_AROS
#define PLATFORM_OS "AROS"
#define PLATFORM_COMPILER "GCC"
#endif


/* MorphOS discovery */
#ifdef AMIGA_MORPHOS
#define PLATFORM_OS "MorphOS"
#define PLATFORM_COMPILER "GCC"
#endif


/* BeOS discovery */
#ifdef __BEOS__

#ifdef WORDS_BIGENDIAN
#define PLATFORM_CPU "PPC"
#define PLATFORM_COMPILER "MetroWerks"
#else
#define FIND_X86_CPU
#define PLATFORM_COMPILER "GCC"
#endif

#define PLATFORM_OS "BeOS"

#endif /* __BEOS__ */


/* BSDI discovery */
#ifdef __bsdi__
#define PLATFORM_OS "BSDi"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* DragonFly BSD discovery */
#ifdef __DragonFly__
#define PLATFORM_OS "DragonFly BSD"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* FreeBSD discovery */
#ifdef __FreeBSD__

/* Get FreeBSD version */
#include "platform_freebsd_version.h"

#endif


/* NetBSD discovery */
#ifdef __NetBSD__

/* Get NetBSD version */
#include "platform_netbsd_version.h"

#endif

/* OpenBSD discovery */
#ifdef __OpenBSD__

/* Get OpenBSD version */
#include "platform_openbsd_version.h"

#endif

/* QNX 4.x discovery */
#if defined(__QNX__) && !defined(__QNXNTO__)
#define PLATFORM_OS "QNX 4.x"
#define PLATFORM_COMPILER "Watcom"
#define FIND_X86_CPU
#endif


/* QNX 6.x discovery */
#ifdef __QNXNTO__

#define PLATFORM_COMPILER "GCC"

/* Get QNX version */
#include "platform_qnx6_version.h"

#ifdef __arm__
#define PLATFORM_CPU "ARMLE"
#endif

#ifdef __mips__
#define PLATFORM_CPU "MIPSLE"
#endif

#ifdef __sh__
#define PLATFORM_CPU "SHLE"
#endif

#if defined(__powerpc__) || defined(__ppc__)
#define PLATFORM_CPU "PPCBE"
#endif

#ifndef PLATFORM_CPU
#define FIND_X86_CPU
#endif

#endif


/* HPUX discovery */
#ifdef _hpux
#define PLATFORM_OS "HPUX"
#define PLATFORM_COMPILER "HP UPC"
#endif

#if defined(__hpux) && !defined(_hpux)
#define PLATFORM_OS "HPUX"
#define PLATFORM_COMPILER "GCC"
#endif


/* IRIX discovery */
#ifdef __sgi
#define PLATFORM_OS "IRIX"
#define PLATFORM_COMPILER "GCC"
#endif


/* OpenServer 5.x discovery */
#ifdef OPENSERVER5_COMPILE
#define PLATFORM_OS "OpenServer 5.x"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* OpenServer 6.x discovery */
#ifdef OPENSERVER6_COMPILE
#define PLATFORM_OS "OpenServer 6.x"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* UnixWare 7.x discovery */
#ifdef _UNIXWARE7
#define PLATFORM_OS "UnixWare 7.x"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* SunOS and Solaris discovery */
#if defined(sun) || defined(__sun)
#  if defined(__SVR4) || defined(__svr4__)
#    define PLATFORM_OS "Solaris"
# else
#    define PLATFORM_OS "SunOS"
#  endif
#endif

/* UWIN discovery */
#ifdef _UWIN
#define PLATFORM_OS "UWIN"
#define PLATFORM_COMPILER "GCC"
#define FIND_X86_CPU
#endif


/* Generic cpu discovery */
#include "platform_cpu_type.h"


/* Generic m68k cpu discovery */
#ifdef FIND_M68K_CPU

#ifdef __mc68060__
#define PLATFORM_CPU "68060"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68040__)
#define PLATFORM_CPU "68040"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68030__)
#define PLATFORM_CPU "68030"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68020__)
#define PLATFORM_CPU "68020"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68010__)
#define PLATFORM_CPU "68010"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68000__)
#define PLATFORM_CPU "68000"
#endif

#ifndef PLATFORM_CPU
#define PLATFORM_CPU "M68K"
#endif

#endif /* FIND_M68K_CPU */


/* Generic alpha cpu discovery */
#ifdef FIND_ALPHA_CPU

#ifdef __alpha_ev6__
#define PLATFORM_CPU "Alpha EV6"
#endif

#if !defined(PLATFORM_CPU) && defined(__alpha_ev5__)
#define PLATFORM_CPU "Alpha EV5"
#endif

#if !defined(PLATFORM_CPU) && defined(__alpha_ev4__)
#define PLATFORM_CPU "Alpha EV4"
#endif

#ifndef PLATFORM_CPU
#define PLATFORM_CPU "Alpha"
#endif

#endif /* FIND_ALPHA_CPU */


/* Generic x86 cpu discovery */
#ifdef FIND_X86_CPU

#ifdef __i686__
#define PLATFORM_CPU "Pentium Pro"
#endif

#if !defined(PLATFORM_CPU) && defined(__i586__)
#define PLATFORM_CPU "Pentium"
#endif

#if !defined(PLATFORM_CPU) && defined(__i486__)
#define PLATFORM_CPU "80486"
#endif

#if !defined(PLATFORM_CPU) && defined(__i386__)
#define PLATFORM_CPU "80386"
#endif

#endif /* FIND_X86_CPU */


/* Fallbacks for unidentified systems */
#ifndef PLATFORM_CPU
#define PLATFORM_CPU "CPU?"
#endif
#ifndef PLATFORM_OS
#define PLATFORM_OS "OS?"
#endif
#ifndef PLATFORM_COMPILER
#define PLATFORM_COMPILER "CC?"
#endif

#endif
