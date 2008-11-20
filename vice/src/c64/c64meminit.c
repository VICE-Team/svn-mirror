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

#include <stdio.h>

#include "c64cart.h"
#include "c64cia.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64meminit.h"
#include "c64memrom.h"
#include "sid.h"
#include "vicii-mem.h"


/* IO is enabled at memory configs 5, 6, 7 and Ultimax.  */
const unsigned int c64meminit_io_config[32] = 
    { 0, 0, 0, 0, 0, 1, 1, 1,
      0, 0, 0, 0, 0, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 1, 1, 1 };

/* ROML is enabled at memory configs 11, 15, 27, 31 and Ultimax.  */
static const unsigned int c64meminit_roml_config[32] = 
    { 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 1,
      1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 1, 0, 0, 0, 1 };

/* ROMH is enabled at memory configs 10, 11, 14, 15, 26, 27, 30, 31
   and Ultimax.  */
static const unsigned int c64meminit_romh_config[32] =
    { 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 1, 1, 0, 0, 1, 1 };

/* ROMH is mapped to $A000-$BFFF at memory configs 10, 11, 14, 15, 26,
   27, 30, 31.  If Ultimax is enabled it is mapped to $E000-$FFFF.  */
static const unsigned int c64meminit_romh_mapping[32] = 
    { 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0xe0, 0xe0, 0xe0, 0xe0,
      0xe0, 0xe0, 0xe0, 0xe0,
      0x00, 0x00, 0xa0, 0xa0,
      0x00, 0x00, 0xa0, 0xa0 };

void c64meminit(unsigned int base)
{
    unsigned int i, j;

    /* Setup BASIC ROM at $A000-$BFFF (memory configs 3, 7, 11, 15).  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab_set(base + 3, i, c64memrom_basic64_read);
        mem_read_tab_set(base + 7, i, c64memrom_basic64_read);
        mem_read_tab_set(base + 11, i, c64memrom_basic64_read);
        mem_read_tab_set(base + 15, i, c64memrom_basic64_read);
        mem_read_base_set(base + 3, i, c64memrom_basic64_rom
                          + ((i & 0x1f) << 8));
        mem_read_base_set(base + 7, i, c64memrom_basic64_rom
                          + ((i & 0x1f) << 8));
        mem_read_base_set(base + 11, i, c64memrom_basic64_rom
                          + ((i & 0x1f) << 8));
        mem_read_base_set(base + 15, i, c64memrom_basic64_rom
                          + ((i & 0x1f) << 8));
    }

    /* Setup I/O at $D000-$DFFF (memory configs 5, 6, 7).  */
    for (j = 0; j < 32; j++) {
        if (c64meminit_io_config[j] == 1) {
            for (i = 0xd0; i <= 0xd3; i++) {
                mem_read_tab_set(base + j, i, vicii_read);
                mem_set_write_hook(base + j, i, vicii_store);
            }
            for (i = 0xd4; i <= 0xd7; i++) {
                mem_read_tab_set(base + j, i, sid_read);
                mem_set_write_hook(base + j, i, sid_store);
            }
            for (i = 0xd8; i <= 0xdb; i++) {
                mem_read_tab_set(base + j, i, colorram_read);
                mem_set_write_hook(base + j, i, colorram_store);
            }

            mem_read_tab_set(base + j, 0xdc, cia1_read);
            mem_set_write_hook(base + j, 0xdc, cia1_store);
            mem_read_tab_set(base + j, 0xdd, cia2_read);
            mem_set_write_hook(base + j, 0xdd, cia2_store);

            mem_read_tab_set(base + j, 0xde, c64io1_read);
            mem_set_write_hook(base + j, 0xde, c64io1_store);
            mem_read_tab_set(base + j, 0xdf, c64io2_read);
            mem_set_write_hook(base + j, 0xdf, c64io2_store);

            for (i = 0xd0; i <= 0xdf; i++)
                mem_read_base_set(base + j, i, NULL);
        }
        if (c64meminit_io_config[j] == 2) {
            for (i = 0xd0; i <= 0xdf; i++) {
                mem_read_tab_set(base + j, i, ultimax_d000_dfff_read);
                mem_set_write_hook(base + j, i, ultimax_d000_dfff_store);
                mem_read_base_set(base + j, i, NULL);
            }
        }
    }

    /* Setup Kernal ROM at $E000-$FFFF (memory configs 2, 3, 6, 7, 10,
       11, 14, 15, 26, 27, 30, 31).  */
    for (i = 0xe0; i <= 0xff; i++) {
        mem_read_tab_set(base + 2, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 3, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 6, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 7, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 10, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 11, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 14, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 15, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 26, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 27, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 30, i, c64memrom_kernal64_read);
        mem_read_tab_set(base + 31, i, c64memrom_kernal64_read);
        mem_read_base_set(base + 2, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 3, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 6, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 7, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 10, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 11, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 14, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 15, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 26, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 27, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 30, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
        mem_read_base_set(base + 31, i,
                          c64memrom_kernal64_trap_rom + ((i & 0x1f) << 8));
    }

    /* Setup ROML at $8000-$9FFF.  */
    for (j = 0; j < 32; j++) {
        if (c64meminit_roml_config[j]) {
            for (i = 0x80; i <= 0x9f; i++) {
                mem_read_tab_set(base + j, i, roml_read);
                mem_read_base_set(base + j, i, NULL);
                mem_set_write_hook(base + j, i, roml_no_ultimax_store);
            }
        }
    }

    /* Setup ROMH at $A000-$BFFF and $E000-$FFFF.  */
    for (j = 0; j < 32; j++) {
        if (c64meminit_romh_config[j]) {
            for (i = c64meminit_romh_mapping[j];
                i <= c64meminit_romh_mapping[j] + 0x1f; i++) {
                mem_read_tab_set(base + j, i, romh_read);
                mem_read_base_set(base + j, i, NULL);
                if (i >= 0xa0 && i <= 0xbf)
                {
                    mem_set_write_hook(base + j, i, romh_no_ultimax_store);
                }
            }
        }
    }

    /* Setup Ultimax configuration.  */
    for (j = 16; j < 24; j++) {
        for (i = 0x10; i <= 0x7f; i++) {
            mem_read_tab_set(base + j, i, ultimax_1000_7fff_read);
            mem_set_write_hook(base + j, i, ultimax_1000_7fff_store);
            mem_read_base_set(base + j, i, NULL);
        }
        for (i = 0x80; i <= 0x9f; i++) {
            mem_set_write_hook(base + j, i, roml_store);
        }
        for (i = 0xa0; i <= 0xbf; i++) {
            mem_read_tab_set(base + j, i, ultimax_a000_bfff_read);
            mem_set_write_hook(base + j, i, ultimax_a000_bfff_store);
            mem_read_base_set(base + j, i, NULL);
        }
        for (i = 0xc0; i <= 0xcf; i++) {
            mem_read_tab_set(base + j, i, ultimax_c000_cfff_read);
            mem_set_write_hook(base + j, i, ultimax_c000_cfff_store);
            mem_read_base_set(base + j, i, NULL);
        }
        for (i = 0xe0; i <= 0xff; i++) {
            mem_set_write_hook(base + j, i, romh_store);
        }
    }
}

