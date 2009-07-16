/*
 * megacart.c -- VIC20 Mega-Cart emulation.
 *
 * Written by
 *  Daniel Kahlin <daniel@kahlin.net>
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

#include "archdep.h"
#include "cartridge.h"
#include "lib.h"
#include "machine.h"
#include "megacart.h"
#include "mem.h"
#include "resources.h"
#include "types.h"
#include "util.h"
#include "vic20cartmem.h"
#include "vic20mem.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */

/*
 * Cartridge RAM
 *
 * Mapping
 *      RAM                 VIC20
 *   0x0000 - 0x1fff  ->  0xa000 - 0xbfff
 *   0x2000 - 0x7fff  ->  0x2000 - 0x7fff
 *
 * (by reasoning around minimal decoding, may be different on actual HW)
 */
#define CART_RAM_SIZE 0x8000
static BYTE *cart_ram = NULL;

/*
 * Cartridge NvRAM
 *
 * Mapping
 *      NvRAM                 VIC20
 *   0x0400 - 0x0fff  ->  0x0400 - 0x0fff
 *   0x1000 - 0x17ff  ->  0x9800 - 0x9fff
 *
 * (by reasoning around minimal decoding, may be different on actual HW)
 */
#define CART_NVRAM_SIZE 0x2000
static BYTE *cart_nvram = NULL;

/*
 * Cartridge ROM
 *
 * Mapping
 *      ROM
 *   0x000000 - 0x0fffff  ->  Low ROM: banks 0x00-0x7f
 *   0x100000 - 0x1fffff  ->  High ROM: banks 0x00-0x7f
 *
 */
#define CART_ROM_SIZE 0x200000
static BYTE *cart_rom = NULL;

/* Cartridge States */
static enum { BUTTON_RESET, SOFTWARE_RESET } reset_mode = BUTTON_RESET;
static int oe_flop = 0;
static int nvram_en_flop = 0;
static BYTE bank_low_reg = 0;
static BYTE bank_high_reg = 0;

/* ------------------------------------------------------------------------- */

/* helper pointers */
static BYTE *cart_rom_low;
static BYTE *cart_rom_high;

/* ------------------------------------------------------------------------- */

/* 0x9800-0x9bff */
BYTE REGPARM1 megacart_io2_read(WORD addr)
{
    BYTE value;
    if (nvram_en_flop) {
        value = cart_nvram[addr & 0x0fff];
    } else {
        value = addr >> 8;
    }
    return value;
}

void REGPARM2 megacart_io2_store(WORD addr, BYTE value)
{
    if (nvram_en_flop) {
        cart_nvram[addr & 0x0fff] = value;
    }
}

/* 0x9c00-0x9fff */
BYTE REGPARM1 megacart_io3_read(WORD addr)
{
    BYTE value;
    if (nvram_en_flop) {
        value = cart_nvram[addr & 0x0fff];
    } else {
        value = addr >> 8;
    }
    return value;
}

void REGPARM2 megacart_io3_store(WORD addr, BYTE value)
{
    if (nvram_en_flop) {
        cart_nvram[addr & 0x0fff] = value;
    }

    if ((addr & 0x180) == 0x080) { /* $9c80 */
        bank_high_reg = value;
    }

    if ((addr & 0x180) == 0x100) { /* $9d00 */
        bank_low_reg = value;
    }

    if ((addr & 0x180) == 0x180) { /* $9d80 */
        nvram_en_flop = (value & 0x1) ? 0 : 1;
        bank_high_reg = value;
        bank_low_reg = value;
    }

    if ((addr & 0x200) == 0x200) { /* $9e00 */
        /* peform reset */
        reset_mode = SOFTWARE_RESET;
        machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
    }
}


void REGPARM2 megacart_ram123_store(WORD addr, BYTE value)
{
    if (nvram_en_flop) {
        cart_nvram[addr & 0x0fff] = value;
    }
}

BYTE REGPARM1 megacart_ram123_read(WORD addr)
{
    if (nvram_en_flop) {
        return cart_nvram[addr & 0x0fff];
    } else {
        return addr >> 8;
    }
}

/* 
 * 0x2000-0x7fff
 * 0xa000-0xbfff
 */
void REGPARM2 megacart_mem_store(WORD addr, BYTE value)
{
    BYTE bank_low;
    BYTE bank_high;
    int ram_low_en;
    int ram_high_en;
    int ram_wp;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;
    ram_wp = (bank_high & 0x40) ? 0 : 1;

    if (addr >= 0x2000 && addr < 0x8000) {
        if (!ram_wp && (ram_low_en && ram_high_en) ) {
            cart_ram[addr] = value;
        }
    }
    if (addr >= 0xa000 && addr < 0xc000) {
        if (!ram_wp && (ram_low_en && ram_high_en) ) {
            cart_ram[addr & 0x1fff] = value;
        }
    }
}

BYTE REGPARM1 megacart_mem_read(WORD addr)
{
    BYTE bank_low;
    BYTE bank_high;
    int ram_low_en;
    int ram_high_en;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;

    if (addr >= 0x2000 && addr < 0x8000) {
        if (!ram_low_en) {
            return cart_rom_low[(addr & 0x1fff) | (bank_low * 0x2000)];
        } else {
            if (ram_high_en) {
                return cart_ram[addr];
            } else {
                return addr >> 8;
            }
        }
    }
    if (addr >= 0xa000 && addr < 0xc000) {
        if (!ram_high_en) {
            return cart_rom_high[(addr & 0x1fff) | (bank_high * 0x2000)];
        } else {
            if (!ram_low_en) {
                return cart_rom_low[(addr & 0x1fff) | (bank_low * 0x2000)];
            } else {
                return cart_ram[addr & 0x1fff];
            }
        }
    }
    return 0x00; /* should never happen */
}


void megacart_init(void)
{
    reset_mode = BUTTON_RESET;
    oe_flop = 0;
    nvram_en_flop = 0;
}

void megacart_reset(void)
{
    if (reset_mode == SOFTWARE_RESET) {
        oe_flop = !oe_flop;
    } else {
        oe_flop = 0;
    }
    reset_mode = BUTTON_RESET;
}

void megacart_config_setup(BYTE *rawcart)
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
    if ( fread(cart_rom, size, 1, fd) < 1) {
        zfile_fclose(fd);
        return -1;
    }
    zfile_fclose(fd);
    return 0;
}

int megacart_bin_attach(const char *filename)
{
    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_nvram) {
        cart_nvram = lib_malloc(CART_NVRAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    if ( zfile_load(filename, cart_rom, (size_t)CART_ROM_SIZE) < 0 ) {
        megacart_detach();
        return -1;
    }

    cart_rom_low = cart_rom;
    cart_rom_high = cart_rom + 0x100000;

    mem_cart_blocks = VIC_CART_RAM123 |
        VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
        VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();
    return 0;
}

void megacart_detach(void)
{
    mem_cart_blocks = 0;
    mem_initialize_memory();
    lib_free(cart_ram);
    lib_free(cart_nvram);
    lib_free(cart_rom);
    cart_ram = NULL;
    cart_nvram = NULL;
    cart_rom = NULL;
}

/* ------------------------------------------------------------------------- */
