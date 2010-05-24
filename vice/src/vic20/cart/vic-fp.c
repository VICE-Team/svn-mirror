/*
 * vic-fp.c -- Vic Flash Plugin emulation.
 *
 * Written by
 *  Marko Makela <marko.makela@iki.fi>
 * based on megacart.c by Daniel Kahlin <daniel@kahlin.net>
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
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "cartridge.h"
#include "lib.h"
#include "machine.h"
#include "vic-fp.h"
#include "mem.h"
#include "resources.h"
#include "snapshot.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include "vic20cart.h"
#include "vic20cartmem.h"
#include "vic20mem.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */
/*
 * Cartridge RAM (32 KiB)
 *
 * Mapping
 *      RAM                 VIC20
 *   0x0000 - 0x1fff  ->  0xa000 - 0xbfff
 *   0x2400 - 0x2fff  ->  0x0400 - 0x0fff
 *   0x2000 - 0x7fff  ->  0x2000 - 0x7fff
 */
#define CART_RAM_SIZE 0x8000
static BYTE *cart_ram = NULL;

/*
 * Cartridge ROM (4 MiB)
 */
#define CART_ROM_SIZE 0x400000
static BYTE *cart_rom = NULL;

#define CART_CFG_ENABLE (!(cart_cfg_reg & 0x80)) /* cart_cfg_reg enable */
#define CART_CFG_ROM_WP (cart_cfg_reg & 0x40) /* ROM write protect */
#define CART_CFG_BLK5_RAM (cart_cfg_reg & 0x20) /* RAM at BLK5 instead of ROM */
#define CART_CFG_BLK1 ((cart_cfg_reg & 0x18) == 0x18) /* BLK1 enabled */
#define CART_CFG_RAM123 ((cart_cfg_reg & 0x18) == 0x08) /* RAM123 enabled */
#define CART_CFG_A21 (cart_cfg_reg & 0x01) /* ROM address line 21 */
#define CART_CFG_MASK 0xf9 /* write mask for cart_cfg_reg */

#define CART_BANK_DEFAULT 0x00
#define CART_CFG_DEFAULT 0x40

/** ROM bank switching register (A20..A13), mapped at $9800..$9bfe (even) */
static BYTE cart_bank_reg;
/** configuration register, mapped at $9801..$9bff (odd)
 * b7 == 1 => I/O2 disabled until RESET
 * b6 == 1 => ROM write protect (set by default)
 * b5 == 1 => RAM at BLK5 (instead of ROM)
 * b4 => 0=3k (RAM123), 1=8k+ (BLK1)
 * b3 == 1 => BLK1/RAM123 enable
 * b2, b1=unused (always 0)
 * b0 => A21
 */
static BYTE cart_cfg_reg;

/* Cartridge States */
/** configuration register enabled */
static int cfg_en_flop;
/** RAM at RAM123 enabled */
static int ram123_en_flop;
/** RAM at BLK1 enabled */
static int blk1_en_flop;
/** RAM at BLK5 instead of ROM */
static int ram5_flop;

#define CART_CFG_INIT(value) do {               \
    cart_cfg_reg = value & CART_CFG_MASK;       \
    cfg_en_flop = CART_CFG_ENABLE;              \
    ram123_en_flop = CART_CFG_RAM123;           \
    blk1_en_flop = CART_CFG_BLK1;               \
    ram5_flop = CART_CFG_BLK5_RAM;              \
    cart_rom_bank = &cart_rom[CART_CFG_A21<<21];\
} while (0)

/* ------------------------------------------------------------------------- */

/* helper pointers */
static BYTE *cart_rom_bank;

/* ------------------------------------------------------------------------- */

/* read 0x0400-0x0fff */
BYTE REGPARM1 vic_fp_ram123_read(WORD addr)
{
    if (ram123_en_flop) {
        return cart_ram[(addr & 0x1fff) + 0x2000];
    } else {
        return vic20_v_bus_last_data;
    }
}

/* store 0x0400-0x0fff */
void REGPARM2 vic_fp_ram123_store(WORD addr, BYTE value)
{
    if (ram123_en_flop) {
        cart_ram[(addr & 0x1fff) + 0x2000] = value;
    }
}

/* read 0x2000-0x3fff */
BYTE REGPARM1 vic_fp_blk1_read(WORD addr)
{
    if (blk1_en_flop) {
        return cart_ram[addr];
    }

    return vic20_cpu_last_data;
}

/* store 0x2000-0x3fff */
void REGPARM2 vic_fp_blk1_store(WORD addr, BYTE value)
{
    if (blk1_en_flop) {
        cart_ram[addr] = value;
    }
}

/* read 0x4000-0x7fff */
BYTE REGPARM1 vic_fp_blk23_read(WORD addr)
{
    return cart_ram[addr];
}

/* store 0x4000-0x7fff */
void REGPARM2 vic_fp_blk23_store(WORD addr, BYTE value)
{
    cart_ram[addr] = value;
}

/* read 0xa000-0xbfff */
BYTE REGPARM1 vic_fp_blk5_read(WORD addr)
{
    if (ram5_flop) {
        return cart_ram[addr & 0x1fff];
    } else {
        return cart_rom_bank[(addr & 0x1fff) | (cart_bank_reg << 13)];
    }
}

/* store 0xa000-0xbfff */
void REGPARM2 vic_fp_blk5_store(WORD addr, BYTE value)
{
    if (ram5_flop) {
        cart_ram[addr & 0x1fff] = value;
    } else if (!CART_CFG_ROM_WP) {
        cart_rom_bank[(addr & 0x1fff) | (cart_bank_reg << 13)] = value;
    }
}

/* read 0x9800-0x9bff */
BYTE REGPARM1 vic_fp_io2_read(WORD addr)
{
    BYTE value;
    if (!cfg_en_flop) {
        value = vic20_cpu_last_data;
    } else if (addr & 1) {
        value = cart_cfg_reg;
    } else {
        value = cart_bank_reg;
    }

    return value;
}

/* store 0x9800-0x9bff */
void REGPARM2 vic_fp_io2_store(WORD addr, BYTE value)
{
    if (!cfg_en_flop) {
        /* ignore */
    } else if (addr & 1) {
        CART_CFG_INIT(value);
    } else {
        cart_bank_reg = value;
    }
}

/* ------------------------------------------------------------------------- */

void vic_fp_init(void)
{
    vic_fp_reset();
}

void vic_fp_reset(void)
{
    cart_bank_reg = CART_BANK_DEFAULT;
    CART_CFG_INIT(CART_CFG_DEFAULT);
}

void vic_fp_config_setup(BYTE *rawcart)
{
}


static int zfile_load(const char *filename, BYTE *dest, size_t size)
{
    FILE *fd;

    fd = zfile_fopen(filename, MODE_READ);
    if (!fd) {
        return -1;
    }
    if (util_file_length(fd) != size) {
        zfile_fclose(fd);
        return -1;
    }
    if (fread(dest, size, 1, fd) < 1) {
        zfile_fclose(fd);
        return -1;
    }
    zfile_fclose(fd);
    return 0;
}

int vic_fp_bin_attach(const char *filename)
{
    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    if (zfile_load(filename, cart_rom, (size_t)CART_ROM_SIZE) < 0 ) {
        vic_fp_detach();
        return -1;
    }

    cart_rom_bank = CART_CFG_A21 ? cart_rom + 0x200000 : cart_rom;

    mem_cart_blocks = VIC_CART_RAM123 |
        VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
        VIC_CART_IO2;
    mem_initialize_memory();
    return 0;
}

void vic_fp_detach(void)
{
    mem_cart_blocks = 0;
    mem_initialize_memory();
    lib_free(cart_ram);
    lib_free(cart_rom);
    cart_ram = NULL;
    cart_rom = NULL;
    cart_rom_bank = NULL;
}

/* ------------------------------------------------------------------------- */

#define VIC20CART_DUMP_VER_MAJOR   2
#define VIC20CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "VIC-FlashPlugin"

int vic_fp_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                               VIC20CART_DUMP_VER_MAJOR,
                               VIC20CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, cart_bank_reg) < 0)
        || (SMW_B(m, cart_cfg_reg) < 0)
        || (SMW_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMW_BA(m, cart_rom, CART_ROM_SIZE) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int vic_fp_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if (vmajor != VIC20CART_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    if (0
        || (SMR_B(m, &cart_bank_reg) < 0)
        || (SMR_B(m, &cart_cfg_reg) < 0)
        || (SMR_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMR_BA(m, cart_rom, CART_ROM_SIZE) < 0)) {
        snapshot_module_close(m);
        lib_free(cart_ram);
        lib_free(cart_rom);
        cart_ram = NULL;
        cart_rom = NULL;
        return -1;
    }

    snapshot_module_close(m);

    CART_CFG_INIT(cart_cfg_reg);

    mem_cart_blocks = VIC_CART_RAM123 |
        VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
        VIC_CART_IO2;
    mem_initialize_memory();

    return 0;
}
