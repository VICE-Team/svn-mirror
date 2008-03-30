/*
 * vic20mem.h -- VIC20 memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Memory configuration handling by
 *  Alexander Lehmann (alex@mathematik.th-darmstadt.de)
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

#ifndef _VIC20MEM_H
#define _VIC20MEM_H

#include "types.h"
#include "mem.h"

#define VIC20_RAM_SIZE			0x10000	/* Kludged... */
#define VIC20_BASIC_ROM_SIZE		0x2000
#define VIC20_KERNAL_ROM_SIZE		0x2000
#define VIC20_CHARGEN_ROM_SIZE		0x1000

#define VIC20_BASIC_CHECKSUM		33073
#define VIC20_KERNAL_CHECKSUM		38203

extern BYTE ram[VIC20_RAM_SIZE];
extern BYTE rom[VIC20_BASIC_ROM_SIZE + VIC20_KERNAL_ROM_SIZE];
extern BYTE chargen_rom[0x400 + VIC20_CHARGEN_ROM_SIZE + 0x400];

extern int rom_loaded;

extern read_func_t read_basic, read_kernal, read_chargen, read_via;
extern store_func_t store_via;

extern int vic20_mem_init_resources(void);
extern int vic20_mem_init_cmdline_options(void);
extern int vic20_mem_disable_ram_block(int num);
extern int vic20_mem_enable_ram_block(int num);

#endif
