/*
 * platform_dragonfly_version.h - DragonFly BSD version discovery.
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

#ifndef VICE_PLATFORM_FREEBSD_VERSION_H
#define VICE_PLATFORM_FREEBSD_VERSION_H

#include <sys/param.h>

#if (__DragonFly_version==100000)
#define PLATFORM_OS "DragonFly BSD 1.0"
#endif

#if (__DragonFly_version==120000)
#define PLATFORM_OS "DragonFly BSD 1.2.0"
#endif

#if (__DragonFly_version==140000)
#define PLATFORM_OS "DragonFly BSD 1.4.x"
#endif

#if (__DragonFly_version==160000)
#define PLATFORM_OS "DragonFly BSD 1.6.0"
#endif

#if (__DragonFly_version==180000)
#  include <magic.h>
#  ifdef MAGIC_NO_CHECK_COMPRESS
#    define PLATFORM_OS "DragonFly BSD 1.8.1"
#  else
#    define PLATFORM_OS "DragonFly BSD 1.8.0"
#  endif
#endif

#if (__DragonFly_version==196000)
#define PLATFORM_OS "DragonFly BSD 1.10.x"
#endif

#if (__DragonFly_version==197500)
#define PLATFORM_OS "DragonFly BSD 1.12.x"
#endif

#if (__DragonFly_version==200000)
#  include <sys/unistd.h>
#  ifdef _SC_PAGE_SIZE
#    define PLATFORM_OS "DragonFly BSD 2.0.1"
#  else
#    define PLATFORM_OS "DragonFly BSD 2.0.0"
#  endif
#endif

#if (__DragonFly_version==200200)
#define PLATFORM_OS "DragonFly BSD 2.2.x"
#endif

#if (__DragonFly_version==200400)
#  include <sys/diskmbr.h>
#  ifdef DOSPTYP_OPENBSD
#    define PLATFORM_OS "DragonFly BSD 2.4.1"
#  else
#    define PLATFORM_OS "DragonFly BSD 2.4.0"
#  endif
#endif

#ifndef PLATFORM_OS
#define PLATFORM_OS "DragonFly BSD"
#endif

#endif // VICE_PLATFORM_FREEBSD_VERSION_H
