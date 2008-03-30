/*
 * vice.h - Main header file for VICE.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Jouko Valta      (jopi@stekt.oulu.fi)
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

#ifndef _VICE_H

/* AIX requires this to be the first thing in the file.  */
#if defined (_AIX) && !defined (__GNUC__)
 #pragma alloca
#endif

/* We use <config.h> instead of "config.h" so that a compilation using
   -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because vice.h was found in $srcdir).  Well,
   actually automake barfs if the source directory was already
   configured, so this should not ba an issue anymore.  */

#include <config.h> /* Automagically created by the `configure' script.  */

#define _CONFIG_H

#define APPLICATION_CLASS "VICE"

/* ------------------------------------------------------------------------- */

/* Define the default system directory (where the ROMs are). */

#define LIBDIR		PREFIX "/lib/vice"
#define DOCDIR          LIBDIR "/doc"

/* Sound defaults.  */
#define SOUND_SAMPLE_RATE 22050

#ifndef __MSDOS__
# define SOUND_SAMPLE_BUFFER_SIZE	350	/* ms */
#else
# define SOUND_SAMPLE_BUFFER_SIZE	100
#endif

/* For now, we always handle PAL.  */
#define PAL
#undef NTSC

/* ------------------------------------------------------------------------- */

/* Portability... */

#ifdef __hpux
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#ifndef _INCLUDE_POSIX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#endif
#endif  /* __hpux */

#ifdef __MSDOS__
#undef EDIT
#elif !defined EDIT
#define EDIT
#endif

#if defined HAVE_X11_EXTENSIONS_XSHM_H && defined HAVE_XSHMPUTIMAGE
#define MITSHM
#endif

#if defined __MSDOS__ || (defined HAVE_JOYSTICK_H && defined __linux__)
#define JOYSTICK
#endif

#if defined __MSDOS__
#define HAVE_MOUSE
#endif

#if defined HAVE_LIBXPM && defined HAVE_X11_XPM_H
#define XPM
#endif

#if defined HAVE_SYSTEMINFO_H
#define SYSINFO
#endif

/* FIXME: We currently allow unaligned memory accesses on i386 only, as they
   allow for some optimizations.  What other architectures could benefit from
   having this enabled?  (Maybe the PowerPC would?)  */
#if defined __i386__
#define ALLOW_UNALIGNED_ACCESS
#endif

/* Windows portability cruft.  */
#ifdef WIN32
#include "lose32.h"
#endif

/* ------------------------------------------------------------------------- */

/* This code comes from GNU make:
   Copyright (C) 1988, 89, 90, 91, 92, 93, 94, 95
     Free Software Foundation, Inc.
   It makes sure `GET_PATH_MAX' is set to the maximum length of path strings.
   Notice that `GET_PATH_MAX' might call a function!  */

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifndef	PATH_MAX
#ifndef	POSIX
#define	PATH_MAX	MAXPATHLEN
#endif	/* Not POSIX.  */
#endif	/* No PATH_MAX.  */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif	/* No MAXPATHLEN.  */

/* As `GET_PATH_MAX' might call a function, a `PATH_VAR' macro is provided to
   declare local strings for storing paths.  WARNING: This needs `alloca()'
   to be available!  */
#ifdef	PATH_MAX
#define	GET_PATH_MAX	PATH_MAX
#define	PATH_VAR(var)	char var[PATH_MAX]
#else
#define	NEED_GET_PATH_MAX
extern unsigned int get_path_max();
#define	GET_PATH_MAX	(get_path_max())
#define	PATH_VAR(var)	char *var = (char *) alloca (GET_PATH_MAX)
#endif

/* ------------------------------------------------------------------------- */

/* This `alloca()' portability stuff is from GNU make too.  */

#ifdef	__GNUC__
#undef	alloca
#define	alloca(n)	__builtin_alloca (n)
#else
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#else	/* Not HAVE_ALLOCA_H.  */
#ifndef	_AIX
extern char *alloca ();
#endif	/* Not AIX.  */
#endif	/* HAVE_ALLOCA_H.  */
#endif	/* GCC.  */

/* ------------------------------------------------------------------------- */

/* Some platforms (most notably BeOS and Windows), do not call `main()' as
   the first function.  If this has not been decided so far, fall back to the
   standard way.  */
#ifndef MAIN_PROGRAM
#define MAIN_PROGRAM(argc, argv)        main(argc, argv)
#endif

/* ------------------------------------------------------------------------- */

/* Generic globals. */

extern char *progname;

/* Path to the directory of our executable.  */
extern char *boot_path;

#endif  /* _VICE_H */
