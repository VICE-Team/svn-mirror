/*
 * platform_solaris_version.h - Solaris version discovery.
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

#ifndef VICE_PLATFORM_SOLARIS_VERSION_H
#define VICE_PLATFORM_SOLARIS_VERSION_H

/* sys/syscall.h:
   syscall			number	SOL1	SOL2	SOL3	SOL4	SOL5	SOL6	SOL7	SOL8	SOL9	SOL10
   SYS_p_online		198		----	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXXX
   SYS_nanosleep	199		----	----	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXXX
   SYS_facl		200		----	----	----	XXXX	XXXX	XXXX	XXXX	XXXX	XXXX	XXXXX
   SYS_lwp_alarm	212		----	----	----	----	XXXX	XXXX	XXXX	XXXX	XXXX	XXXXX
   SYS_ntp_adjtime	249		----	----	----	----	----	XXXX	XXXX	XXXX	XXXX	XXXXX
   SYS_lwp_mutex_init	252		----	----	----	----	----	----	XXXX	XXXX	XXXX	XXXXX
   SYS_umount2		255		----	----	----	-----	----	----	----	XXXX	XXXX	XXXXX
*/

/* limits.h:
				SOL8	SOL9	SOL10
   ATEXIT_MAX	   	XXXX	----	-----
   _XOPEN_NAME_MAX	----	----	XXXXX
*/

/* arpa/nameser.h:
   SOL10	__NAMESER	19991006
   OSOL+	__NAMESER	20090302
*/

/* fnmatch.h:
				OSOL	SOL11.0	SOL11.1	SOL11.2
   FNM_LEADING_DIR	----	 XXXX		 XXXX		 XXXX
*/

/* glob.h:
				SOL11.0	SOL11.1	SOL11.2
   GLOB_LIMIT		 ----		 XXXX		 XXXX
*/

/* inet/ip.h:
				SOL11.1	SOL11.2
   CONN_PATH_LOOKUP	 ----		 XXXX
*/

#include <sys/syscall.h>
#include <limits.h>
#include <sys/types.h>

#ifdef SYS_umount2
#  ifdef _XOPEN_NAME_MAX
#    include <arpa/nameser.h>
#    if (__NAMESER==19991006)
#      define PLATFORM_OS "Solaris 10"
#    else
#      include <fnmatch.h>
#      ifdef FNM_LEADING_DIR
#        include <glob.h>
#        ifdef GLOB_LIMIT
#          include <inet/ip.h>
#          ifdef CONN_PATH_LOOKUP
#            define PLATFORM_OS "Solaris 11.2"
#          else
#            define PLATFORM_OS "Solaris 11.1"
#          endif
#        else
#          define PLATFORM_OS "Solaris 11"
#        endif
#      else
#        define PLATFORM_OS "OpenSolaris"
#      endif
#    endif
#  else
#    ifdef ATEXIT_MAX
#      define PLATFORM_OS "Solaris 8"
#    else
#      define PLATFORM_OS "Solaris 9"
#    endif
#  endif
#else
#  ifdef SYS_lwp_mutex_init
#    define PLATFORM_OS "Solaris 7"
#  else
#    ifdef SYS_ntp_adjtime
#      define PLATFORM_OS "Solaris 2.6"
#    else
#      ifdef SYS_lwp_alarm
#        define PLATFORM_OS "Solaris 2.5"
#      else
#        ifdef SYS_facl
#          define PLATFORM_OS "Solaris 2.4"
#        else
#          ifdef SYS_nanosleep
#            define PLATFORM_OS "Solaris 2.3"
#          else
#            ifdef SYS_p_online
#              define PLATFORM_OS "Solaris 2.2"
#            else
#              define PLATFORM_OS "Solaris 2.1"
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifndef PLATFORM_OS
#define PLATFORM_OS "Solaris"
#endif

#endif // VICE_PLATFORM_SOLARIS_VERSION_H
