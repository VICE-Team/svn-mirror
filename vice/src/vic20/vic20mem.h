/*
 * vic20mem.h -- VIC20 memory handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * Memory configuration handling by
 *  Alexander Lehmann <alex@mathematik.th-darmstadt.de>
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

#define VIC20_RAM_SIZE                  0x10000 /* Kludged... */
#define VIC20_BASIC_ROM_SIZE            0x2000
#define VIC20_KERNAL_ROM_SIZE           0x2000
#define VIC20_CHARGEN_ROM_SIZE          0x1000

#define VIC20_BASIC_CHECKSUM            33073
#define VIC20_KERNAL_CHECKSUM           38203

#define VIC_ROM_BLK1A   1
#define VIC_ROM_BLK1B   2
#define VIC_ROM_BLK2A   4
#define VIC_ROM_BLK2B   8
#define VIC_ROM_BLK3A   16
#define VIC_ROM_BLK3B   32
#define VIC_ROM_BLK5A   64
#define VIC_ROM_BLK5B   128

#define VIC_ROM_BLK0A   0
#define VIC_ROM_BLK0B   0

/* VIC20 memory-related resources.  */
#define VIC_BLK0 1
#define VIC_BLK1 2
#define VIC_BLK2 4
#define VIC_BLK3 8
#define VIC_BLK5 16
#define VIC_BLK_ALL (VIC_BLK0 | VIC_BLK1 | VIC_BLK2 | VIC_BLK3 | VIC_BLK5)

extern BYTE mem_rom[VIC20_BASIC_ROM_SIZE + VIC20_KERNAL_ROM_SIZE];
extern BYTE mem_chargen_rom[0x400 + VIC20_CHARGEN_ROM_SIZE + 0x400];
extern BYTE mem_cartrom[0x10000];

#define mem_kernal_rom (mem_rom + VIC20_BASIC_ROM_SIZE)
#define mem_basic_rom (mem_rom)

extern int vic20_mem_init_resources(void);
extern int vic20_mem_init_cmdline_options(void);
extern int vic20_mem_disable_ram_block(int num);
extern int vic20_mem_enable_ram_block(int num);

extern void mem_attach_cartridge(int type, BYTE *rawcart);
extern void mem_detach_cartridge(int type);

extern int mem_patch_kernal(void);

#endif

