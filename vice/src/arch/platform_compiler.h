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
 * compiler | support
 * ------------------
 * gcc      | yes
 * hp uc    | yes
 * llvm     | yes
 * xLC      | yes
 */

#ifndef VICE_PLATFORM_COMPILER_H
#define VICE_PLATFORM_COMPILER_H

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

#if !defined(PLATFORM_COMPILER) && defined(__GNUC__)
#define PLATFORM_COMPILER "GCC"
#endif

#endif
