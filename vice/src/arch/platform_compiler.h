/*
 * platform_compiler.h - compiler discovery macros.
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

/* Compilers supported:
 *
 * compiler       | support
 * ------------------------------------------------------
 * comeau c++     | yes, but wrong version format for now
 * compaq/dec     | yes, but wrong version format for now
 * dignus systems | yes, but wrong version format for now
 * EKOPath        | yes
 * gcc            | yes
 * green hill     | yes, but wrong version format for now
 * hp uc          | yes
 * intel cc       | yes, but wrong version format for now
 * llvm           | yes
 * xLC            | yes
 */

#ifndef VICE_PLATFORM_COMPILER_H
#define VICE_PLATFORM_COMPILER_H

#undef XQUOTE
#undef QUOTE
#define QUOTE(x) XQUOTE(x)
#define XQUOTE(x) #x

/* GCC discovery first */
#if !defined(PLATFORM_COMPILER) && defined(__GNUC__)
#  if (__GNUC__>2)
#    define PLATFORM_COMPILER "GCC-" QUOTE(__GNUC__) "." QUOTE(__GNUC_MINOR__) "." QUOTE(__GNUC_PATCHLEVEL__)
#  else
#    define PLATFORM_COMPILER "GCC-" QUOTE(__GNUC__) "." QUOTE(__GNUC_MINOR__)
#  endif
#endif

/* llvm discovery */
#if !defined(PLATFORM_COMPILER) && defined(__APPLE__) && defined(llvm)
#define PLATFORM_COMPILER  "llvm"
#endif

/* xLC discovery */
#if !defined(PLATFORM_COMPILER) && defined( _AIX) && defined(__TOS_AIX__)
#define PLATFORM_COMPILER "xLC"
#endif

/* HP UPC discovery */
#if !defined(PLATFORM_COMPILER) && defined(_hpux)
#define PLATFORM_COMPILER "HP UPC"
#endif

/* Comeau compiler discovery */
#if !defined(PLATFORM_COMPILER) && defined(__COMO__)
#define PLATFORM_COMPILE "Comeau c++ " QUOTE(__COMO_VERSION__)
#endif

/* Intel compiler discovery */
#if !defined(PLATFORM_COMPILER) && defined(__INTEL_COMPILER)
#define PLATFORM_COMPILER "Intel Compiler " QUOTE(__INTEL_COMPILER)
#endif

/* compaq/dec compiler discovery */
#if !defined(PLATFORM_COMPILER) && defined(__DECC)
#define PLATFORM_COMPILER "Compaq/DEC compiler " QUOTE(__DECC_VER)
#endif

/* Dignus Systems compiler discovery */
#if !defined(PLATFORM_COMPILER) && defined(__SYSC__)
#define PLATFORM_COMPILER "Dignus Systems compiler " QUOTE(__SYSC_VER__)
#endif

/* EKOPath compiler discovery */
#if !defined(PLATFORM_COMPILER) && defined(__PATHCC__)
#define PLATFORM_COMPILER "EKOPath compiler " QUOTE(__PATHCC__) "." QUOTE(__PATHCC_MINOR__) "." QUOTE(__PATHCC_PATCHLEVEL__)
#endif

#if !defined(PLATFORM_COMPILER) && defined(__ghs__)
#define PLATFORM_COMPILER "Green Hill C/C++ " QUOTE(__GHS_VERSION_NUMBER__)
#endif

#endif
