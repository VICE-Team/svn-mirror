/*
 * c128mem.h
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#ifndef _C128MEM_H
#define _C128MEM_H

#define C128_RAM_SIZE			0x20000
#define C128_KERNAL_ROM_SIZE		0x2000
#define C128_BASIC_ROM_SIZE		0x8000
#define C128_EDITOR_ROM_SIZE		0x1000
#define C128_CHARGEN_ROM_SIZE		0x1000

#define C128_BASIC_CHECKSUM_85		38592
#define C128_BASIC_CHECKSUM_86		2496
#define C128_EDITOR_CHECKSUM_R01	56682
#define C128_EDITOR_CHECKSUM_R01SWE	9364
#define C128_EDITOR_CHECKSUM_R01GER	9619
#define C128_KERNAL_CHECKSUM_R01	22353
#define C128_KERNAL_CHECKSUM_R01SWE	24139
#define C128_KERNAL_CHECKSUM_R01GER	22098

extern int c128_mem_init_resources(void);
extern int c128_mem_init_cmdline_options(void);

#endif
