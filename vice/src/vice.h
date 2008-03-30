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

/* FIXME: We currently allow unaligned memory accesses on i386 only, as they
   allow for some optimizations.  What other architectures could benefit from
   having this enabled?  (Maybe the PowerPC would?)  */
#if defined __i386__
#define ALLOW_UNALIGNED_ACCESS
#endif

/* RISC OS specific stuff */
#ifdef __riscos
#include "ROlib.h"
#endif

/* ------------------------------------------------------------------------- */
/* Which OS is using the common keyboard routines?  */
#if !defined(__riscos) && !defined(__OS2__)
#define COMMON_KBD
#endif

/* Which OS is using those ugly scale source coordinates.  */
#if defined(__MSDOS__)
#define VIDEO_SCALE_SOURCE
#endif

/* ------------------------------------------------------------------------- */

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
#    ifdef WIN32
/* using the _(String) definition from unix gettext for win32 intl */
#        include "intl.h"
#        define _(String) intl_translate_text(String)
#        define N_(String) (String)
#    else
/* Stubs that do something close enough.  */
#        define _(String) (String)
#        define N_(String) (String)
#    endif
#endif /* ENABLE_NLS */

#ifdef __OS2__
int yyparse (void);
#undef __GNUC__
#endif

#endif

