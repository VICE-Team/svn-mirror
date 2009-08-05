/*
 * finalexpansion.c -- VIC20 Final Expansion v3.1 emulation.
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
#include "finalexpansion.h"
#include "flash040.h"
#include "log.h"
#include "mem.h"
#include "resources.h"
#include "types.h"
#include "util.h"
#include "vic20cartmem.h"
#include "vic20mem.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */

/*#define FE_DEBUG_ENABLED*/

#ifdef FE_DEBUG_ENABLED
#define FE_DEBUG(x) log_debug x
#else
#define FE_DEBUG(x)
#endif

/* base addresses for the blocks */
#define BLK0_BASE 0x0000
#define BLK1_BASE 0x0000
#define BLK2_BASE 0x2000
#define BLK3_BASE 0x4000
#define BLK5_BASE 0x6000

/*
 * Cartridge RAM
 *
 * Mapping in RAM mode:
 *      RAM                    VIC20
 *   0x00400 - 0x00fff  ->  0x0400 - 0x0fff
 *   0x10000 - 0x11fff  ->  0x2000 - 0x3fff
 *   0x12000 - 0x13fff  ->  0x4000 - 0x5fff
 *   0x14000 - 0x15fff  ->  0x6000 - 0x7fff
 *   0x16000 - 0x17fff  ->  0xa000 - 0xbfff
 * (INV_A13/INV_A14 changes this)
 *
 * Mapping in Super RAM mode:
 *      RAM                    VIC20
 *   0x00400 - 0x00fff  ->  0x0400 - 0x0fff
 *   0xN0000 - 0xN1fff  ->  0x2000 - 0x3fff
 *   0xN2000 - 0xN3fff  ->  0x4000 - 0x5fff
 *   0xN4000 - 0xN5fff  ->  0x6000 - 0x7fff
 *   0xN6000 - 0xN7fff  ->  0xa000 - 0xbfff
 * (INV_A13/INV_A14 changes this)
 *
 */
#define CART_RAM_SIZE 0x80000
static BYTE *cart_ram = NULL;

/*
 * Cartridge ROM
 *
 * Mapping
 *      ROM                    VIC20
 *   0xN0000 - 0xN1fff  ->  0x2000 - 0x3fff
 *   0xN2000 - 0xN3fff  ->  0x4000 - 0x5fff
 *   0xN4000 - 0xN5fff  ->  0x6000 - 0x7fff
 *   0xN6000 - 0xN7fff  ->  0xa000 - 0xbfff
 * (INV_A13/INV_A14 changes this)
 *
 */
#define CART_ROM_SIZE 0x80000

/* Cartridge States */
static flash040_context_t flash_state;
static BYTE register_a;
static BYTE register_b;
static BYTE lock_bit;


/* ------------------------------------------------------------------------- */

/* Register A ($9c02) */
/* #define REGA_BANK_MASK   0x1f */
#define REGA_BANK_MASK   0x0f  /* only 512 KB connected */

#define REGA_MODE_MASK   0xe0
#define MODE_START       0x00  /* Start Modus         (000zzzzz) [Mod_ROM_A] */
#define MODE_FLASH_WRITE 0x20  /* Flash-Schreib-Modus (001zzzzz) [Mod_Prog]  */
#define MODE_FLASH_READ  0x40  /* Flash-Lese-Modus    (010zzzzz) [Mod_ROM]   */
#define MODE_RAM         0x80  /* RAM Modus           (100zzzzz) [Mod_RAM]   */
#define MODE_SUPER_RAM   0xa0  /* Super RAM Modus     (101zzzzz) [Mod_RAM2]  */

/* in MODE_RAM */
#define REGA_BLK0_RO     0x01
#define REGA_BLK1_RO     0x02
#define REGA_BLK2_RO     0x04
#define REGA_BLK3_RO     0x08
#define REGA_BLK5_RO     0x10

/* Register B ($9c03) */
#define REGB_BLK0_OFF    0x01
#define REGB_BLK1_OFF    0x02
#define REGB_BLK2_OFF    0x04
#define REGB_BLK3_OFF    0x08
#define REGB_BLK5_OFF    0x10
#define REGB_INV_A13     0x20
#define REGB_INV_A14     0x40
#define REGB_REG_OFF     0x80

/* ------------------------------------------------------------------------- */
static int is_mode_ram(void)
{
    return ( (register_a & REGA_MODE_MASK) == MODE_RAM );
}

/* 0x9c00-0x9fff */
static int is_locked(void)
{
    if (register_b & REGB_REG_OFF) {
        return 1;
    }
    if ((register_a & REGA_MODE_MASK) == MODE_START) {
        return lock_bit;
    }
    return 0;
}

BYTE REGPARM1 finalexpansion_io3_read(WORD addr)
{   
    BYTE value;

    addr &= 0x03;
    FE_DEBUG(("Read reg%02x. (locked=%d)", addr, is_locked()));
    if (!is_locked()) {
        switch (addr) {
        case 0x02:
            value = register_a;
            break;
        case 0x03:
            value = register_b;
            break;
        default:
            value = vic20_cpu_last_data;
            break;
        }
    } else {
        value = vic20_cpu_last_data;
    }
    return value;
}

void REGPARM2 finalexpansion_io3_store(WORD addr, BYTE value)
{
    addr &= 0x03;
    FE_DEBUG(("Wrote reg%02x = %02x. (locked=%d)", addr, value, is_locked()));
    if (!is_locked()) {
        switch (addr) {
        case 0x02:
            register_a = value;
            break;
        case 0x03:
            register_b = value;
            break;
        }
    }
}

/* ------------------------------------------------------------------------- */

static unsigned int calc_addr(WORD addr, int bank, WORD base)
{
    unsigned int faddr;

    faddr = (addr & 0x1fff) | (bank * 0x8000) | base;

    faddr ^= (register_b & REGB_INV_A13) ? 0x2000 : 0;
    faddr ^= (register_b & REGB_INV_A14) ? 0x4000 : 0;

    return faddr;
}

static void internal_store(WORD addr, BYTE value, int blk, WORD base)
{
    BYTE mode;
    int bank;
    unsigned int faddr;

    mode = register_a & REGA_MODE_MASK;

    if (blk == 0) {
        bank = 0;
    } else {
        switch (mode) {
        case MODE_FLASH_WRITE:
        case MODE_SUPER_RAM:
            bank = register_a & REGA_BANK_MASK;
            break;
        default:
            bank = 1;
            break;
        }
    }

    faddr = calc_addr(addr, bank, base);

    switch (mode) {
    case MODE_FLASH_WRITE:
        if (blk != 0) {
            flash040core_store(&flash_state, faddr, value);
        }
        break;
    case MODE_START:
    case MODE_FLASH_READ:
    case MODE_RAM:
    case MODE_SUPER_RAM:
        cart_ram[faddr] = value;
        break;
    default:
        break;
    }
}

static BYTE internal_read(WORD addr, int blk, WORD base)
{
    BYTE mode;
    int bank;
    unsigned int faddr;
    BYTE value;

    mode = register_a & REGA_MODE_MASK;

    if (blk == 0) {
        bank = 0;
    } else {
        switch (mode) {
        case MODE_START:
        case MODE_FLASH_READ:
        case MODE_FLASH_WRITE:
        case MODE_SUPER_RAM:
            bank = register_a & REGA_BANK_MASK;
            break;
        default:
            bank = 1;
            break;
        }
    }

    faddr = calc_addr(addr, bank, base);

    switch (mode) {
    case MODE_START:
        if (blk == 5) {
            value = flash040core_read(&flash_state, faddr);
        } else {
            value = vic20_cpu_last_data;
        }
        break;
    case MODE_FLASH_READ:
    case MODE_FLASH_WRITE:
        value = flash040core_read(&flash_state, faddr);
        break;
    case MODE_RAM:
    case MODE_SUPER_RAM:
        value = cart_ram[faddr];
        break;
    default:
        value = vic20_cpu_last_data;
        break;
    }
    return value;
}

/* ------------------------------------------------------------------------- */

/* 0x0400 - 0x0fff */
void REGPARM2 finalexpansion_ram123_store(WORD addr, BYTE value)
{
    if ( !(register_b & REGB_BLK0_OFF) ) {
        if ( !( is_mode_ram() && (register_a & REGA_BLK0_RO) ) ) {
            internal_store(addr, value, 0, BLK0_BASE);
        }
    }
}

BYTE REGPARM1 finalexpansion_ram123_read(WORD addr)
{
    BYTE value;
    if ( !(register_b & REGB_BLK0_OFF) ) {
        value = internal_read(addr, 0, BLK0_BASE);
    } else {
        value = vic20_v_bus_last_data;
    }
    return value;
}

/* 0x2000-0x3fff */
void REGPARM2 finalexpansion_blk1_store(WORD addr, BYTE value)
{
    if ( !(register_b & REGB_BLK1_OFF) ) {
        if ( !( is_mode_ram() && (register_a & REGA_BLK1_RO) ) ) {
            internal_store(addr, value, 1, BLK1_BASE);
        }
    }
}

BYTE REGPARM1 finalexpansion_blk1_read(WORD addr)
{
    BYTE value;
    if ( !(register_b & REGB_BLK1_OFF) ) {
        value = internal_read(addr, 1, BLK1_BASE);
    } else {
        value = vic20_cpu_last_data;
    }
    return value;
}

/* 0x4000-0x5fff */
void REGPARM2 finalexpansion_blk2_store(WORD addr, BYTE value)
{
    if ( !(register_b & REGB_BLK2_OFF) ) {
        if ( !( is_mode_ram() && (register_a & REGA_BLK2_RO) ) ) {
            internal_store(addr, value, 2, BLK2_BASE);
        }
    }
}

BYTE REGPARM1 finalexpansion_blk2_read(WORD addr)
{
    BYTE value;
    if ( !(register_b & REGB_BLK2_OFF) ) {
        value = internal_read(addr, 2, BLK2_BASE);
    } else {
        value = vic20_cpu_last_data;
    }
    return value;
}

/* 0x6000-0x7fff */
void REGPARM2 finalexpansion_blk3_store(WORD addr, BYTE value)
{
    if ( !(register_b & REGB_BLK3_OFF) ) {
        if ( !( is_mode_ram() && (register_a & REGA_BLK3_RO) ) ) {
            internal_store(addr, value, 3, BLK3_BASE);
        }
    }
}

BYTE REGPARM1 finalexpansion_blk3_read(WORD addr)
{
    BYTE value;
    if ( !(register_b & REGB_BLK3_OFF) ) {
        value = internal_read(addr, 3, BLK3_BASE);
    } else {
        value = vic20_cpu_last_data;
    }
    return value;
}

/* 0xa000-0xbfff */
void REGPARM2 finalexpansion_blk5_store(WORD addr, BYTE value)
{
    lock_bit = 0;

    if ( !(register_b & REGB_BLK5_OFF) ) {
        if ( !( is_mode_ram() && (register_a & REGA_BLK5_RO) ) ) {
            internal_store(addr, value, 5, BLK5_BASE);
        }
    }
}

BYTE REGPARM1 finalexpansion_blk5_read(WORD addr)
{
    BYTE value;

    lock_bit = 1;

    if ( !(register_b & REGB_BLK5_OFF) ) {
        value = internal_read(addr, 5, BLK5_BASE);
    } else {
        value = vic20_cpu_last_data;
    }
    return value;
}



void finalexpansion_init(void)
{
    register_a = 0x00;
    register_b = 0x00;
    lock_bit = 1;
}

void finalexpansion_reset(void)
{
    flash040core_reset(&flash_state);
    register_a = 0x00;
    register_b = 0x00;
    lock_bit = 1;
}

void finalexpansion_config_setup(BYTE *rawcart)
{
}

/* this is a duplicate of the code in megacart.c.  Should be made
   into common code. */
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
    if ( fread(dest, size, 1, fd) < 1) {
        zfile_fclose(fd);
        return -1;
    }
    zfile_fclose(fd);
    return 0;
}

int finalexpansion_bin_attach(const char *filename)
{
    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    /* should probably guard this */
    flash040core_init(&flash_state, FLASH040_TYPE_B);

    if ( zfile_load(filename, flash_state.flash_data, (size_t)CART_ROM_SIZE) < 0 ) {
        finalexpansion_detach();
        return -1;
    }

    mem_cart_blocks = VIC_CART_RAM123 |
        VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
        VIC_CART_IO3;
    mem_initialize_memory();
    return 0;
}

void finalexpansion_detach(void)
{
    mem_cart_blocks = 0;
    mem_initialize_memory();
    flash040core_shutdown(&flash_state);
    lib_free(cart_ram);
    cart_ram = NULL;
}

/* ------------------------------------------------------------------------- */
