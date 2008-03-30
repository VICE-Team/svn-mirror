/*
 * c64mem.h -- C64 memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _C64MEM_H
#define _C64MEM_H

#include "vice.h"

#include "types.h"
#include "mem.h"

#define C64_RAM_SIZE			0x10000
#define C64_KERNAL_ROM_SIZE		0x2000
#define C64_BASIC_ROM_SIZE		0x2000
#define C64_CHARGEN_ROM_SIZE		0x1000

#define C64_BASIC_CHECKSUM		15702
#define C64_KERNAL_CHECKSUM_R00		50955
#define C64_KERNAL_CHECKSUM_R03		50954
#define C64_KERNAL_CHECKSUM_R03swe	50633
#define C64_KERNAL_CHECKSUM_R43		50955
#define C64_KERNAL_CHECKSUM_R64		49680

extern void mem_set_vbank(int new_vbank);
extern read_func_t read_zero, read_basic, read_kernal, read_chargen, read_ram, read_io2, read_rom;
extern store_func_t store_zero, store_ram, store_ram_hi, store_io2, store_rom;

#endif /* _C64MEM_H */
