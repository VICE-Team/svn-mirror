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

/* We use <autoconf.h> instead of "config.h" so that a compilation using
   -I./include -I$srcdir will use ./include/autoconf.h rather than
   $srcdir/include/autoconf.h (which it would do because vice.h was found in
   $srcdir/include).  */
#include <autoconf.h>	  /* Automagically created by the `configure'
			     script.  */

#define _CONFIG_H
#if !defined C128 && !defined VIC20 && !defined PET && !defined CBM64
#define CBM64
#endif

#if defined VIC20
#define APPLICATION_NAME "xvic"
#define EMULATOR	 "VIC20"
#elif defined PET
#define APPLICATION_NAME "xpet"
#define EMULATOR         "PET"
#elif defined C128
#define APPLICATION_NAME "x128"
#define EMULATOR	 "C128"
#else
#define APPLICATION_NAME "x64"
#define EMULATOR	 "C64"
#endif

#define APPLICATION_CLASS "VICE"

/* ------------------------------------------------------------------------- */

/* Define the default system directory (where the ROMs are). */

#define LIBDIR		PREFIX "/lib/vice"

/* These are relative to LIBDIR. */

#ifdef C128
#define PROJECTDIR	"C128"
#endif

#ifdef CBM64
#define PROJECTDIR	"C64"
#endif

#ifdef VIC20
#define PROJECTDIR	"VIC20"
#endif

#ifdef PET
#define PROJECTDIR	"PET"
#endif

#define DOCDIR		"doc"


/* These are the default image file name, to be searched for under LIBDIR "/"
   PROJECTDIR. */

#ifndef RAMNAME
#define RAMNAME		"ram"
#endif

#ifndef BASICNAME
#define BASICNAME	"basic"
#endif

#ifdef PET
#    define KERNAL2001NAME  	"pet2001"
#    define KERNAL3032NAME	"pet3032"
#    define KERNAL4032NAME	"pet4032"
#    define EDITOR2B40NAME	"edit2.b"
#    define EDITOR4B40NAME	"edit4.b40"
#    define EDITOR4B80NAME      "edit4.b80"
#    ifndef KERNALNAME
#        define KERNALNAME	KERNAL4032NAME /* Default */
#    endif
#    define PET_COLS	80
#else  /* !PET */
#    ifndef KERNALNAME
#        define KERNALNAME	"kernal"
#    endif
#endif /* !PET */

#ifndef REUNAME
#define REUNAME		"reu.c64"
#endif

#ifndef EXROMNAME
#define EXROMNAME	"exrom"
#endif

#ifndef DOSNAME
#define DOSNAME		"dos1541"
#endif

#ifndef BIOSNAME
#define BIOSNAME  	"Z80_BIOS"
#endif

#ifndef CHARNAME
#define CHARNAME	"chargen"
#endif

/* ------------------------------------------------------------------------- */

#define XDEBUGGER
#define PAL

#ifdef CBM64
#define	IEEE488
#endif

/*
 * Which way to copy files ?
 * Since C1541 disk drive copies files to the first filename, whereas
 * they normally go to the last file, one can change the 'c1541' copy
 * syntax via this option.
 *   UNIX:  copy    oldname [oldname2 ...] newname
 *   1541:  copy    newname oldname [oldname2 ...]
 */

/*#define COPY_TO_LEFT*/	/* no */


/* Enable T64 support. */

#if defined CBM64 || defined C128
#define CBMTAPE
#endif

/* Autostart kludge */
#ifdef CBM64
#define AUTOSTART
#endif


/*
 * Shall Virtual RAM Expander be installed ?
 * Sizes available are 128, 256 and 512 KB
 * (C128 internal RAM may be expanded in vmachine.h)
 */

#ifndef VIC20
#ifndef PET

#define REU
#define REUSIZE	       512

#endif
#endif


/* Utility to patch between ROM revisions.
 * Supports 0, 3, 67 and 100 for the C64, but user can define more.  */

#ifndef VIC20
#ifndef PET
#define PATCH_ROM
#endif
#endif


/* Emulator Identification Utility. */

#define EMULATOR_ID


/* Printer. */

/* #define PRINTER */

#define PRN_CS_DEFAULT  0	/* 0 */


#if 1  /* European A4 Papersize */
#define PS_WIDTH	595
#define PS_HEIGHT	842

#else  /* American Letter Papersize */
#define PS_WIDTH	612
#define PS_HEIGHT	792
#endif

#if defined CBM64 || defined C128 || defined PET || defined VIC20
#define SOUND
#endif

/* Sound defaults. */

#define SOUND_SAMPLE_RATE		22050	/* Hz */
#ifndef __MSDOS__
#define SOUND_SAMPLE_BUFFER_SIZE	350	/* ms */
#else
#define SOUND_SAMPLE_BUFFER_SIZE	100
#endif

/* Debugging info. */

/* #define DEBUG */		/* no */


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

#ifdef DEBUG
#undef LED_ARRAY
#endif

#ifdef __MSDOS__
#undef EDIT
#elif !defined EDIT
#define EDIT
#endif

#if defined HAVE_X11_EXTENSIONS_XSHM_H && defined HAVE_LIBXEXT && defined HAVE_XSHMPUTIMAGE
#define MITSHM
#endif

#if defined __MSDOS__ || (defined HAVE_JOYSTICK_H && defined __linux__)
#define JOYSTICK
#endif

#if defined HAVE_LIBXPM && defined HAVE_X11_XPM_H
#define XPM
#endif

#if defined HAVE_SYSTEMINFO_H
#define SYSINFO
#endif

#if defined CBM64 || defined VIC20 || defined C128
#define HAVE_TRUE1541
#endif

/* FIXME: We currently allow unaligned memory accesses on i386 only, as they
   allow for some optimizations.  What other architectures could benefit from
   having this enabled?  (Maybe the PowerPC would?)  */
#if defined __i386__
#define ALLOW_UNALIGNED_ACCESS
#endif

/* ------------------------------------------------------------------------- */

/* This code comes from GNU make:
   Copyright (C) 1988, 89, 90, 91, 92, 93, 94, 95 Free Software Foundation, Inc.
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

/* Generic globals. */
extern const char *progname;

#ifdef __MSDOS__
extern char boot_path[];
#endif

#endif  /* _VICE_H */
