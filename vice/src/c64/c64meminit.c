/*
 * c64meminit.c -- Initialize C64 memory.
 *
 * Written by
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

#include "vice.h"

#include "c64mem.h"
#include "c64meminit.h"


void c64meminit(unsigned int base)
{
    unsigned int i;

    /* Setup BASIC ROM at $A000-$BFFF (memory configs 3, 7, 11, 15).  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab_set(base+3, i, basic64_read);
        mem_read_tab_set(base+7, i, basic64_read);
        mem_read_tab_set(base+11, i, basic64_read);
        mem_read_tab_set(base+15, i, basic64_read);
        mem_read_base_set(base+3, i, mem_basic64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+7, i, mem_basic64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+11, i, mem_basic64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+15, i, mem_basic64_rom + ((i & 0x1f) << 8));
    }

    /* Setup Kernal ROM at $E000-$FFFF (memory configs 2, 3, 6, 7, 10,
       11, 14, 15, 26, 27, 30, 31).  */
    for (i = 0xe0; i <= 0xff; i++) {
        mem_read_tab_set(base+2, i, kernal64_read);
        mem_read_tab_set(base+3, i, kernal64_read);
        mem_read_tab_set(base+6, i, kernal64_read);
        mem_read_tab_set(base+7, i, kernal64_read);
        mem_read_tab_set(base+10, i, kernal64_read);
        mem_read_tab_set(base+11, i, kernal64_read);
        mem_read_tab_set(base+14, i, kernal64_read);
        mem_read_tab_set(base+15, i, kernal64_read);
        mem_read_tab_set(base+26, i, kernal64_read);
        mem_read_tab_set(base+27, i, kernal64_read);
        mem_read_tab_set(base+30, i, kernal64_read);
        mem_read_tab_set(base+31, i, kernal64_read);
        mem_read_base_set(base+2, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+3, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+6, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+7, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+10, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+11, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+14, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+15, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+26, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+27, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+30, i, mem_kernal64_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base+31, i, mem_kernal64_rom + ((i & 0x1f) << 8));
    }
}

