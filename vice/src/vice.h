/*
 * vice.h - Main header file for VICE.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andreas Boose <viceteam@t-online.de>
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
#define _VICE_H

/* We use <config.h> instead of "config.h" so that a compilation using
   -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because vice.h was found in $srcdir).  Well,
   actually automake barfs if the source directory was already
   configured, so this should not be an issue anymore.  */

#include <config.h> /* Automagically created by the `configure' script.  */

/* ------------------------------------------------------------------------- */

/* Portability... */

#if defined(__hpux) || defined(__IBMC__)
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#ifndef _INCLUDE_POSIX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#endif
#endif  /* __hpux */

#if defined HAVE_LIBXPM && defined HAVE_X11_XPM_H
#define XPM
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
#ifdef UNDER_CE
#include "ce32.h"
#endif

/* RISC OS specific stuff */
#ifdef __riscos
#include "ROlib.h"
#endif

/* BeOS specific stuff */
#ifdef __BEOS__
#include "beos.h"
#endif

/* ------------------------------------------------------------------------- */
/* Which OS is using the common keyboard routines?  */
#if !defined(__riscos) && !defined(__OS2__)
#define COMMON_KBD
#endif

/* Which OS is using those ugly scale source coordinates.  */
#if defined(__MSDOS__) || defined(WIN32) || defined(__riscos) || defined(__OS2__)
#define VIDEO_SCALE_SOURCE
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

/* For some reason, our DJGPP setup does not like to have both `limits.h' and
   `sys/param.h' #included.  */
#if !defined(__OS2__) && !defined(DJGPP) && defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif

#ifndef PATH_MAX
#ifndef POSIX
#define PATH_MAX        MAXPATHLEN
#endif  /* Not POSIX.  */
#endif  /* No PATH_MAX.  */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif  /* No MAXPATHLEN.  */

/* As `GET_PATH_MAX' might call a function, a `PATH_VAR' macro is provided to
   declare local strings for storing paths.  WARNING: This needs `alloca()'
   to be available!  */
#ifdef  PATH_MAX
#define GET_PATH_MAX    PATH_MAX
#else
#define NEED_GET_PATH_MAX
extern unsigned int get_path_max();
#define GET_PATH_MAX    (get_path_max())
#endif

/* ------------------------------------------------------------------------- */

/* Some platforms (most notably BeOS and Windows), do not call `main()' as
   the first function.  If this has not been decided so far, fall back to the
   standard way.  */
#ifndef MAIN_PROGRAM
#define MAIN_PROGRAM(argc, argv)        main(argc, argv)
#endif

/* Internationalization stuff */
#if defined(ENABLE_NLS) && defined(HAVE_LIBINTL_H)
#    include <libintl.h>
#    define _(String) gettext (String)
#    ifdef gettext_noop
#        define N_(String) gettext_noop (String)
#    else
#        define N_(String) (String)
#    endif
#else
/* Stubs that do something close enough.  */
#    define _(String) (String)
#    define N_(String) (String)
#endif /* ENABLE_NLS */

#endif 

