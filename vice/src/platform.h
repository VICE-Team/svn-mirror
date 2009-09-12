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

/* AIX discovery */

#ifdef _AIX

/* find out which compiler is being used */
#ifdef __TOS_AIX__
#  define PLATFORM_COMPILER "xLC"
#else
#  define PLATFORM COMPILER "GCC"
#endif

/* find out what version of AIX is being used */
#ifdef _AIX61
#define PLATFORM_OS "AIX 6.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX53)
#define PLATFORM_OS "AIX 5.3"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX52)
#define PLATFORM_OS "AIX 5.2"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX51)
#define PLATFORM_OS "AIX 5.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX433)
#define PLATFORM_OS "AIX 4.3.3"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX432)
#define PLATFORM_OS "AIX 4.3.2"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX431)
#define PLATFORM_OS "AIX 4.3.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX43)
#define PLATFORM_OS "AIX 4.3"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX421)
#define PLATFORM_OS "AIX 4.2.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX42)
#define PLATFORM_OS "AIX 4.2"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX415)
#define PLATFORM_OS "AIX 4.1.5"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX414)
#define PLATFORM_OS "AIX 4.1.4"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX413)
#define PLATFORM_OS "AIX 4.1.3"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX411)
#define PLATFORM_OS "AIX 4.1.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX41)
#define PLATFORM_OS "AIX 4.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX4)
#define PLATFORM_OS "AIX 4.0"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX32)
#define PLATFORM_OS "AIX 3.2"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX31)
#define PLATFORM_OS "AIX 3.1"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX3)
#define PLATFORM_OS "AIX 3.0"
#endif

#ifdef PLATFORM_OS
#define PLATFORM_CPU "RS6000"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX13)
#define PLATFORM_OS "AIX 1.3"
#endif

#if !defined(PLATFORM_OS) && defined(_AIX11)
#define PLATFORM_OS "AIX 1.1"
#endif

#ifndef PLATFORM_OS
#define PLATFORM_OS "AIX"
#endif

/* define FIND_X86_CPU for later generic x86 cpu discovery */
#ifndef PLATFORM_CPU
#define FIND_X86_CPU
#endif

#endif /* AIX */


/* AmigaOS 4.x discovery */
#ifdef AMIGA_OS4
#define PLATFORM_OS "AmigaOS 4.x"
#define PLATFORM_COMPILER "GCC"
#define PLATFORM_CPU "PPC"
#endif


/* AROS discovery */
#ifdef AMIGA_AROS

#if defined(__amd64__) || defined(__x86_64__)
#define PLATFORM_CPU "AMD64/x86_64"
#endif

#if defined(__ppc__) || defined(__powerpc__)
#define PLATFORM_CPU "PPC"
#endif

/* define FIND_X86_CPU for later generic x86 cpu discovery */
#ifndef PLATFORM_CPU
#define FIND_X86_CPU
#endif

#define PLATFORM_OS "AROS"
#define PLATFORM_COMPILER "GCC"

#endif /* AMIGA_AROS */


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

#if defined(__amd64__) || defined(__x86_64__)
#define PLATFORM_CPU "AMD64/x86_64"
#endif

#if defined(__powerpc__) || defined(__ppc__)
#define PLATFORM_CPU "PPC"
#endif

#ifdef __ia64__
#define PLATFORM_CPU "IA64"
#endif

#ifdef __sparc__
#define PLATFORM_CPU "Sparc"
#endif

#ifdef __alpha__
#define FIND_ALPHA_CPU
#endif

/* Find out what version of freebsd is being used. */
#if (__FreeBSD__==1)
#define PLATFORM_OS "FreeBSD 1.x"
#else

#include <sys/param.h>

#if (__FreeBSD_version==119411)
#define PLATFORM_OS "FreeBSD 2.0"
#endif

#if (__FreeBSD_version==199504)
#define PLATFORM_OS "FreeBSD 2.0.5"
#endif

#endif /* __FreeBSD__ == 1 */


#endif /* __FreeBSD__ */


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

#endif
