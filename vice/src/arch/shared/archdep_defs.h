/** \file   archdep_defs.h
 * \brief   Defines, enums and types used by the archdep functions
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

#ifndef VICE_ARCHDEP_DEFS_H
#define VICE_ARCHDEP_DEFS_H

#include "vice.h"


/** \brief  Various OS-identification macros
 *
 * The question marks indicate ports I have my doubts about they'll even run
 * VICE at all.
 *
 * <pre>
 *  ARCHDEP_OS_UNIX
 *    ARCHDEP_OS_OSX
 *    ARCHDEP_OS_LINUX
 *    ARCHDEP_OS_BSD
 *      ARCHDEP_OS_BSD_FREE
 *      ARCHDEP_OS_BSD_NET
 *      ARCHDEP_OS_BSD_OPEN
 *      ARCHDEP_OS_BSD_DRAGON
 *    ARCHDEP_OS_QNX (?)
 *    ARCHDEP_OS_SOLARIS (?)
 *  ARCHDEP_OS_WINDOWS
 *  ARCHDEP_OS_OS2 (?)
 *  ARCHDEP_OS_BEOS
 *  ARCHDEP_OS_MSDOS (?)
 *  ARCHDEP_OS_AMIGA
 * </pre>
 */
#ifdef UNIX_COMPILE
/* Generic UNIX */
# define ARCHDEP_OS_UNIX
# if defined(MACOSX_SUPPORT)
/* OSX */
#  define ARCHDEP_OS_OSX
# elif defined(__linux__)
/* Linux */
#  define ARCHDEP_OS_LINUX
# elif defined(__FreeBSD__)
/* FreeBSD */
#  define ARCHDEP_OS_BSD
#  define ARCHDEP_OS_BSD_FREE
/* NetBSD */
# elif defined(__NetBSD__)
#  define ARCHDEP_OS_BSD
#  define ARCHDEP_OS_BSD_NET
/* OpenBSD */
# elif defined(__OpenBSD__)
#  define ARCHDEP_OS_BSD
#  define ARCHDEP_OS_BSD_OPEN
/* DragonFly BSD */
# elif defined(__DragonFly__)
#  define ARCHDEP_OS_BSD
#  define ARCHDEP_OS_BSD_DRAGON
# elif defined(__QNX__)
/* QNX (do we even support this anymore?) */
#  define ARCHDEP_OS_QNX
# elif defined(sun) || defined(__sun)
/* Solaris (same question) */
#  define ARCHDEP_OS_SOLARIS
# endif /* ifdef UNIX_COMPILE */
#elif defined(WIN32_COMPILE)
/* Windows */
# define ARCHDEP_OS_WINDOWS
#elif defined(OS2_COMPILE)
/* OS/2 (again: has anyone even tested this?) */
# define ARCHDEP_OS_OS2
#elif defined(__BEOS__)
/* BeOS (maybe Haiku?) */
# define ARCHDEP_OS_BEOS
#elif defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)
/* MS-DOS (really?) */
# define ARCHDEP_OS_DOS
#elif defined(AMIGA_SUPPORT)
/* Amiga (may have to split into Aros etc) */
# define ARCHDEP_OS_AMIGA
#endif


/** \brief  Arch-dependent directory separator used in paths
 */
#if defined(ARCHDEP_OS_WINDOWS) || defined(ARCHDEP_OS_OS2) \
    || defined(ARCHDEP_HAVE_DOS)
# define ARCHDEP_DIR_SEPARATOR  '\\'
#else
# define ARCHDEP_DIR_SEPARATOR  '/'
#endif

/** \brief  Extension used for autostart disks
 */
#define ARCHDEP_AUTOSTART_DICK_EXTENSION    "d64"


#if defined(ARCHEP_OS_AMIGA) || defined(ARCHDEP_OS_MSDOS) \
    || defined(ARCHDEP_OS_OS2) || defined(ARCHDEP_OS_WINDOWS)
# define ARCHDEP_FINDPATH_SEPARATOR_STRING  ";"
#else
# define ARCHDEP_FINDPATH_SEPARATOR_STRING  ":"
#endif


/* set LIBDIR and DOCDIR */
#ifdef ARCHDEP_OS_UNIX
# define LIBDIR VICEDIR
# define DOCDIR LIBDIR "/doc"
#endif

#endif
