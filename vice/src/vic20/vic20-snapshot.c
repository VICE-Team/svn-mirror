/*
 * vic20-snapshot.c -- VIC20 snapshot handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Multiple memory configuration support originally by
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

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "log.h"
#include "mem.h"
#include "resources.h"
#include "snapshot.h"
#include "types.h"
#include "vic20-resources.h"
#include "vic20mem.h"
#include "vic20rom.h"


static log_t vic20_snapshot_log = LOG_ERR;

/************************************************************************/

/*
 * VIC20 memory dump contains the available memory at the moment
 */
#define VIC20MEM_DUMP_VER_MAJOR   1
#define VIC20MEM_DUMP_VER_MINOR   0
#define SNAP_MEM_MODULE_NAME      "VIC20MEM"

/*
 * UBYTE        CONFIG          Bit 0: 1 = expansion block 0 RAM enabled
 *                                  1: 1 = expansion block 1 RAM enabled
 *                                  2: 1 = expansion block 2 RAM enabled
 *                                  3: 1 = expansion block 3 RAM enabled
 *                                  5: 1 = expansion block 5 RAM enabled
 *
 * ARRAY        RAM0            1k RAM $0000-$03ff
 * ARRAY        RAM1            4k RAM $1000-$1fff
 * ARRAY        COLORRAM        2k Video RAM
 * ARRAY        BLK0            3k RAM $0400-$0fff (if blk 0 RAM enabled)
 * ARRAY        BLK1            8k RAM $2000-$3fff (if blk 1 RAM enabled)
 * ARRAY        BLK2            8k RAM $4000-$5fff (if blk 2 RAM enabled)
 * ARRAY        BLK3            8k RAM $6000-$7fff (if blk 3 RAM enabled)
 * ARRAY        BLK5            8k RAM $A000-$Bfff (if blk 5 RAM enabled)
 *
 */


static int mem_write_ram_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    BYTE config;

    config = (ram_block_0_enabled ? 1 : 0)
                | (ram_block_1_enabled ? 2 : 0)
                | (ram_block_2_enabled ? 4 : 0)
                | (ram_block_3_enabled ? 8 : 0)
                | (ram_block_5_enabled ? 32 : 0) ;

    m = snapshot_module_create(p, SNAP_MEM_MODULE_NAME,
                               VIC20MEM_DUMP_VER_MAJOR, VIC20MEM_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, config);

    snapshot_module_write_byte_array(m, mem_ram, 0x0400);
    snapshot_module_write_byte_array(m, mem_ram + 0x1000, 0x1000);
    snapshot_module_write_byte_array(m, mem_ram + 0x9400, 0x0800);

    if (config & 1) {
        snapshot_module_write_byte_array(m, mem_ram + 0x0400, 0x0c00);
    }
    if (config & 2) {
        snapshot_module_write_byte_array(m, mem_ram + 0x2000, 0x2000);
    }
    if (config & 4) {
        snapshot_module_write_byte_array(m, mem_ram + 0x4000, 0x2000);
    }
    if (config & 8) {
        snapshot_module_write_byte_array(m, mem_ram + 0x6000, 0x2000);
    }
    if (config & 32) {
        snapshot_module_write_byte_array(m, mem_ram + 0xA000, 0x2000);
    }

    snapshot_module_close(m);

    return 0;
}

static int mem_read_ram_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    BYTE config;

    m = snapshot_module_open(p, SNAP_MEM_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != VIC20MEM_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_read_byte(m, &config);

    snapshot_module_read_byte_array(m, mem_ram, 0x0400);
    snapshot_module_read_byte_array(m, mem_ram + 0x1000, 0x1000);
    /* setup wraparound copy of chargen */
    memcpy(mem_chargen_rom, mem_ram + 0x1c00, 0x400);

    snapshot_module_read_byte_array(m, mem_ram + 0x9400, 0x0800);

    resources_set_value("RAMBlock0", (resource_value_t)(config & 1));
    if (config & 1) {
        snapshot_module_read_byte_array(m, mem_ram + 0x0400, 0x0c00);
    }
    resources_set_value("RAMBlock1", (resource_value_t)(config & 2));
    if (config & 2) {
        snapshot_module_read_byte_array(m, mem_ram + 0x2000, 0x2000);
    }
    resources_set_value("RAMBlock2", (resource_value_t)(config & 4));
    if (config & 4) {
        snapshot_module_read_byte_array(m, mem_ram + 0x4000, 0x2000);
    }
    resources_set_value("RAMBlock3", (resource_value_t)(config & 8));
    if (config & 8) {
        snapshot_module_read_byte_array(m, mem_ram + 0x6000, 0x2000);
    }
    resources_set_value("RAMBlock5", (resource_value_t)(config & 32));
    if (config & 32) {
        snapshot_module_read_byte_array(m, mem_ram + 0xA000, 0x2000);
    }

    snapshot_module_close(m);

    mem_initialize_memory();

    return 0;
}

/*
 * VIC20 ROM dump
 */
#define VIC20ROM_DUMP_VER_MAJOR   1
#define VIC20ROM_DUMP_VER_MINOR   1
#define SNAP_ROM_MODULE_NAME      "VIC20ROM"

/*
 * UBYTE        CONFIG          Bit 0: 1 = ROM block $2*** enabled
 *                                  1: 1 = ROM block $3*** enabled
 *                                  2: 1 = ROM block $4*** enabled
 *                                  3: 1 = ROM block $5*** enabled
 *                                  4: 1 = ROM block $6*** enabled
 *                                  5: 1 = ROM block $7*** enabled
 *                                  6: 1 = ROM block $A*** enabled
 *                                  7: 1 = ROM block $B*** enabled
 *
 * ARRAY        KERNAL          8k KERNAL ROM $e000-$ffff
 * ARRAY        BASIC           8k BASIC ROM $c000-$dfff
 * ARRAY        CHARGEN         4k CHARGEN ROM
 * ARRAY        BLK1A           4k ROM $2*** (if CONFIG & 1)
 * ARRAY        BLK1B           4k ROM $3*** (if CONFIG & 2)
 * ARRAY        BLK3A           4k ROM $6*** (if CONFIG & 16)
 * ARRAY        BLK3B           4k ROM $7*** (if CONFIG & 32)
 * ARRAY        BLK5A           4k ROM $A*** (if CONFIG & 64)
 * ARRAY        BLK5B           4k ROM $B*** (if CONFIG & 128)
 *
 * ARRAY        BLK2A           4k ROM $4*** (if CONFIG & 4)
 * ARRAY        BLK2B           4k ROM $5*** (if CONFIG & 8)
 */


static int mem_write_rom_snapshot_module(snapshot_t *p, int save_roms)
{
    snapshot_module_t *m;
    BYTE config;
    int trapfl;

    if (!save_roms)
        return 0;

    m = snapshot_module_create(p, SNAP_ROM_MODULE_NAME,
                          VIC20MEM_DUMP_VER_MAJOR, VIC20MEM_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    /* disable traps before saving the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*) &trapfl);
    resources_set_value("VirtualDevices", (resource_value_t) 1);

    config = mem_rom_blocks;

    snapshot_module_write_byte(m, config);

    /* save kernal */
    snapshot_module_write_byte_array(m, mem_rom + 0x2000, 0x2000);
    /* save basic */
    snapshot_module_write_byte_array(m, mem_rom + 0x0000, 0x2000);

    snapshot_module_write_byte_array(m, mem_chargen_rom + 0x400, 0x1000);

    if (config & 1) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x2000, 0x1000);
    }
    if (config & 2) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x3000, 0x1000);
    }
    if (config & 16) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x6000, 0x1000);
    }
    if (config & 32) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x7000, 0x1000);
    }
    if (config & 64) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0xA000, 0x1000);
    }
    if (config & 128) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0xB000, 0x1000);
    }
    if (config & 4) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x4000, 0x1000);
    }
    if (config & 8) {
        snapshot_module_write_byte_array(m, mem_cartrom + 0x5000, 0x1000);
    }

    /* enable traps again when necessary */
    resources_set_value("VirtualDevices", (resource_value_t) trapfl);

    snapshot_module_close(m);

    return 0;
}

static int mem_read_rom_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    BYTE config;
    int trapfl;

    m = snapshot_module_open(p, SNAP_ROM_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL)
        return 0;       /* optional */

    if (vmajor != VIC20ROM_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    /* disable traps before loading the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*) &trapfl);
    resources_set_value("VirtualDevices", (resource_value_t) 1);

    snapshot_module_read_byte(m, &config);

    /* read kernal */
    snapshot_module_read_byte_array(m, mem_rom + 0x2000, 0x2000);
    /* read basic */
    snapshot_module_read_byte_array(m, mem_rom + 0x0000, 0x2000);

    snapshot_module_read_byte_array(m, mem_chargen_rom + 0x400, 0x1000);

    mem_rom_blocks = 0;

    if (config & 1) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x2000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK1A;
    }
    if (config & 2) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x3000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK1B;
    }
    if (config & 16) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x6000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK3A;
    }
    if (config & 32) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x7000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK3B;
    }
    if (config & 64) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0xA000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5A;
    }
    if (config & 128) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0xB000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5B;
    }
    if (config & 4) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x4000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK2A;
    }
    if (config & 8) {
        snapshot_module_read_byte_array(m, mem_cartrom + 0x5000, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK2B;
    }

    vic20rom_kernal_checksum();
    vic20rom_basic_checksum();

    log_warning(vic20_snapshot_log,
                "Dumped Romset files and saved settings will "
                "represent\nthe state before loading the snapshot!");

    /* enable traps again when necessary */
    resources_set_value("VirtualDevices", (resource_value_t) trapfl);

    snapshot_module_close(m);

    mem_initialize_memory();

    return 0;
}

int vic20_snapshot_write_module(snapshot_t *m, int save_roms)
{
    if (mem_write_ram_snapshot_module(m) < 0
        || mem_write_rom_snapshot_module(m, save_roms) < 0 )
        return -1;
    return 0;
}

int vic20_snapshot_read_module(snapshot_t *m)
{
    if (mem_read_ram_snapshot_module(m) < 0
        || mem_read_rom_snapshot_module(m) < 0 )
        return -1;
    return 0;
}

