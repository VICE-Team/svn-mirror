/*
 * vic20mem.c -- VIC20 memory handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "cmdline.h"
#include "emuid.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mon.h"
#include "resources.h"
#include "sysfile.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vic.h"
#include "vic20-resources.h"
#include "vic20ieeevia.h"
#include "vic20mem.h"
#include "vic20via.h"

#define IS_NULL(s)  (s == NULL || *s == '\0')

/* ------------------------------------------------------------------------- */

const char *mem_romset_resources_list[] = {
    "KernalName", "ChargenName", "BasicName",
    "CartridgeFile2000", "CartridgeFile6000",
    "CartridgeFileA000", "CartridgeFileB000",
    "DosName1541", "DosName1571", "DosName1581",
    "DosName1541ii",
    NULL
};

/*----------------------------------------------------------------------*/

static log_t vic20_mem_log = LOG_ERR;

/*----------------------------------------------------------------------*/

/* Old program counter.  Not used without MMU support.  */
unsigned int old_reg_pc;

/* ------------------------------------------------------------------------- */

/* The VIC20 memory. */
BYTE ram[VIC20_RAM_SIZE];
int ram_size = VIC20_RAM_SIZE;
BYTE rom[VIC20_BASIC_ROM_SIZE + VIC20_KERNAL_ROM_SIZE];
#define kernal_rom (rom + VIC20_BASIC_ROM_SIZE)
#define basic_rom (rom)

BYTE cartrom[0x10000];

/* The second 0x400 handles a possible segfault by a wraparound of the
   chargen by setting it to $8c00.  FIXME: This does not cause the exact
   behavior to be emulated though!  */
BYTE chargen_rom[0x400 + VIC20_CHARGEN_ROM_SIZE + 0x400];

/* Memory read and write tables.  */
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t _mem_write_tab[0x101];
BYTE *_mem_read_base_tab[0x101];
int mem_read_limit_tab[0x101];

/* These ones are used when watchpoints are turned on.  */
read_func_ptr_t _mem_read_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
read_func_ptr_t _mem_read_tab_nowatch[0x101];
store_func_ptr_t _mem_write_tab_nowatch[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
static int vicrom_loaded = 0;

/* ------------------------------------------------------------------------- */

static void REGPARM2 store_wrap(ADDRESS addr, BYTE value)
{
    ram[addr & (VIC20_RAM_SIZE - 1)] = value;
    chargen_rom[addr & 0x3ff] = value;
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 basic_read(ADDRESS addr)
{
    return basic_rom[addr & 0x1fff];
}

BYTE REGPARM1 kernal_read(ADDRESS addr)
{
    return kernal_rom[addr & 0x1fff];
}

BYTE REGPARM1 chargen_read(ADDRESS addr)
{
    return chargen_rom[0x400 + (addr & 0xfff)];
}

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    return ram[addr & 0xff];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    ram[addr & 0xff] = value;
}

static BYTE REGPARM1 ram_read(ADDRESS addr)
{
    return ram[addr];
}

static void REGPARM2 ram_store(ADDRESS addr, BYTE value)
{
    ram[addr & (VIC20_RAM_SIZE - 1)] = value;
}

static BYTE REGPARM1 read_cartrom(ADDRESS addr)
{
    return cartrom[addr & 0xffff];
}

BYTE REGPARM1 rom_read(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0x8000:
        return chargen_read(addr);
      case 0xc000:
      case 0xd000:
        return basic_read(addr);
      case 0xe000:
      case 0xf000:
        return kernal_read(addr);
    }

    return 0;
}

void REGPARM2 rom_store(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0x8000:
        chargen_rom[0x400 + (addr & 0x0fff)] = value;
        break;
      case 0xc000:
      case 0xd000:
        basic_rom[addr & 0x1fff] = value;
        break;
      case 0xe000:
      case 0xf000:
        kernal_rom[addr & 0x1fff] = value;
        break;
    }
}

void REGPARM2 via_store(ADDRESS addr, BYTE value)
{
    if (addr & 0x10)            /* $911x (VIA2) */
        via2_store(addr, value);
    if (addr & 0x20)            /* $912x (VIA1) */
        via1_store(addr, value);
}

BYTE REGPARM1 via_read(ADDRESS addr)
{
    BYTE ret = 0xff;

    if (addr & 0x10)            /* $911x (VIA2) */
        ret &= via2_read(addr);
    if (addr & 0x20)            /* $912x (VIA1) */
        ret &= via1_read(addr);

    return ret;
}

static BYTE REGPARM1 read_emuid(ADDRESS addr)
{
    addr &= 0xff;
    if (addr >= 0xa0) {
        return emulator_id[addr - 0xa0];
    }
    return 0xff;
}

static void REGPARM2 store_emuid(ADDRESS addr, BYTE value)
{
    addr &= 0xff;
    if (addr == 0xff) {
        emulator_id[addr - 0xa0] ^= 0xff;
    }
    return;
}

/*-------------------------------------------------------------------*/

static BYTE REGPARM1 io3_read(ADDRESS addr)
{
    if (emu_id_enabled && (addr & 0xff00) == 0x9f00)
        return read_emuid(addr);
    return 0xff;
}

static void REGPARM2 io3_store(ADDRESS addr, BYTE value)
{
    if (emu_id_enabled && (addr & 0xff00) == 0x9f00)
        store_emuid(addr, value);
    return;
}

static BYTE REGPARM1 io2_read(ADDRESS addr)
{
    if (ieee488_enabled) {
        if (addr & 0x10) {
            return ieeevia2_read(addr);
        } else {
            return ieeevia1_read(addr);
        }
    }
    return 0xff;
}

static void REGPARM2 io2_store(ADDRESS addr, BYTE value)
{
    if (ieee488_enabled) {
        if (addr & 0x10) {
            ieeevia2_store(addr, value);
        } else {
            ieeevia1_store(addr, value);
        }
    }
    return;
}

/*-------------------------------------------------------------------*/

static BYTE REGPARM1 read_dummy(ADDRESS addr)
{
    return (addr >> 8);
}

static void REGPARM2 store_dummy(ADDRESS addr, BYTE value)
{
    return;
}


/* Watchpoint functions */


static BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab_nowatch[addr >> 8](addr);
}

static void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab_nowatch[addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

/* Generic memory access.  */

void REGPARM2 mem_store(ADDRESS addr, BYTE value)
{
    _mem_write_tab_ptr[addr >> 8](addr, value);
}

BYTE REGPARM1 mem_read(ADDRESS addr)
{
    return _mem_read_tab_ptr[addr >> 8](addr);
}

/* ------------------------------------------------------------------------- */

static void set_mem(int start_page, int end_page,
                    read_func_ptr_t read_func,
                    store_func_ptr_t store_func,
                    BYTE *read_base, int base_mask)
{
    int i;

    if (read_base != NULL) {
        for (i = start_page; i <= end_page; i++) {
            _mem_read_tab_nowatch[i] = read_func;
            _mem_write_tab_nowatch[i] = store_func;
            _mem_read_base_tab[i] = read_base + ((i << 8) & base_mask);
            mem_read_limit_tab[i] = (end_page << 8) + 0xfd;
        }
    } else {
        for (i = start_page; i <= end_page; i++) {
            _mem_read_tab_nowatch[i] = read_func;
            _mem_write_tab_nowatch[i] = store_func;
            _mem_read_base_tab[i] = NULL;
            mem_read_limit_tab[i] = -1;
        }
    }
}

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    /* We do not need MMU support.  */
}

int vic20_mem_enable_rom_block(int num)
{
    if (num == 1 || num == 2 || num == 3 || num == 5) {
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_cartrom, store_dummy,
                cartrom, 0xffff);
        return 0;
    } else
        return -1;
}

int vic20_mem_enable_ram_block(int num)
{
    if (num == 0) {
        set_mem(0x04, 0x0f,
                ram_read, ram_store,
                ram, 0xffff);
        return 0;
    } else if (num > 0 && num != 4 && num <= 5) {
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                ram_read, ram_store,
                ram, 0xffff);
        return 0;
    } else
        return -1;
}

int vic20_mem_disable_ram_block(int num)
{
    if (num == 0) {
        set_mem(0x04, 0x0f,
                read_dummy, store_dummy,
                ram, 0xffff);
        return 0;
    } else if (num > 0 && num != 4 && num <= 5) {
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_dummy, store_dummy,
                ram, 0xffff);
        return 0;
    } else
        return -1;
}

void mem_initialize_memory(void)
{
    int i;

    /* Setup low standard RAM at $0000-$0300. */
    set_mem(0x00, 0x03,
            ram_read, ram_store,
            ram, 0xffff);

    /* Setup more low RAM at $1000-$1FFF.  */
    set_mem(0x10, 0x1b,
            ram_read, ram_store,
            ram, 0xffff);
    set_mem(0x1c, 0x1f,
            ram_read, store_wrap,
            ram, 0xffff);

    /* Setup RAM at $0400-$0FFF.  */
    if (ram_block_0_enabled)
        vic20_mem_enable_ram_block(0);
    else
        vic20_mem_disable_ram_block(0);

    /* Setup RAM or cartridge ROM at $2000-$3FFF.  */
    if (mem_rom_blocks & (VIC_ROM_BLK1A | VIC_ROM_BLK1B)) {
        vic20_mem_enable_rom_block(1);
    } else
    if (ram_block_1_enabled) {
        vic20_mem_enable_ram_block(1);
    } else {
        vic20_mem_disable_ram_block(1);
    }

    /* Setup RAM or cartridge ROM at $4000-$5FFF.  */
    if (mem_rom_blocks & (VIC_ROM_BLK2A | VIC_ROM_BLK2B)) {
        vic20_mem_enable_rom_block(2);
    } else
    if (ram_block_2_enabled) {
        vic20_mem_enable_ram_block(2);
    } else {
        vic20_mem_disable_ram_block(2);
    }

    /* Setup RAM or cartridge ROM at $6000-$7FFF.  */
    if (mem_rom_blocks & (VIC_ROM_BLK3A | VIC_ROM_BLK3B)) {
        vic20_mem_enable_rom_block(3);
    } else
    if (ram_block_3_enabled) {
        vic20_mem_enable_ram_block(3);
    } else {
        vic20_mem_disable_ram_block(3);
    }

    /* Setup RAM or cartridge ROM at $A000-$BFFF.  */
    if (mem_rom_blocks & (VIC_ROM_BLK5A | VIC_ROM_BLK5B)) {
        vic20_mem_enable_rom_block(5);
    } else
    if (ram_block_5_enabled) {
        vic20_mem_enable_ram_block(5);
    } else {
        vic20_mem_disable_ram_block(5);
    }

    /* Setup character generator ROM at $8000-$8FFF. */
    set_mem(0x80, 0x8f,
            chargen_read, store_dummy,
            chargen_rom + 0x400, 0x0fff);

    /* Setup VIC-I at $9000-$90FF. */
    set_mem(0x90, 0x90,
            vic_read, vic_store,
            NULL, 0);

    /* Setup VIAs at $9100-$93FF. */
    set_mem(0x91, 0x93,
            via_read, via_store,
            NULL, 0);

    /* Setup color memory at $9400-$97FF.
       Warning: we use a kludge here.  Instead of mapping the color memory
       separately, we map it directly in the corresponding RAM address
       space. */
    set_mem(0x94, 0x97,
            ram_read, ram_store,
            ram, 0xffff);

    /* Setup I/O2 at the expansion port */
    set_mem(0x98, 0x9b,
            io2_read, io2_store,
            NULL, 0);

    /* Setup I/O3 at the expansion port (includes emulator ID) */
    set_mem(0x9c, 0x9f,
            io3_read, io3_store,
            NULL, 0);

    /* Setup BASIC ROM at $C000-$DFFF. */
    set_mem(0xc0, 0xdf,
            basic_read, store_dummy,
            basic_rom, 0x1fff);

    /* Setup Kernal ROM at $E000-$FFFF. */
    set_mem(0xe0, 0xff,
            kernal_read, store_dummy,
            kernal_rom, 0x1fff);

    _mem_read_tab_nowatch[0x100] = _mem_read_tab_nowatch[0];
    _mem_write_tab_nowatch[0x100] = _mem_write_tab_nowatch[0];
    _mem_read_base_tab[0x100] = _mem_read_base_tab[0];
    mem_read_limit_tab[0x100] = -1;

    _mem_read_base_tab_ptr = _mem_read_base_tab;
    mem_read_limit_tab_ptr = mem_read_limit_tab;

    for (i = 0; i <= 0x100; i++) {
        _mem_read_tab_watch[i] = read_watch;
        _mem_write_tab_watch[i] = store_watch;
    }

    mem_toggle_watchpoints(0);
}

void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = _mem_read_tab_watch;
        _mem_write_tab_ptr = _mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = _mem_read_tab_nowatch;
        _mem_write_tab_ptr = _mem_write_tab_nowatch;
    }
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    int i;

    for (i = 0; i < VIC20_RAM_SIZE; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }
}

int mem_kernal_checksum(void)
{
    int i;
    WORD sum;

    /* Check Kernal ROM.  */
    for (i = 0, sum = 0; i < VIC20_KERNAL_ROM_SIZE; i++)
        sum += kernal_rom[i];

    if (sum != VIC20_KERNAL_CHECKSUM) {
        log_error(vic20_mem_log,
                  "Warning: Unknown Kernal image.  Sum: %d ($%04X).",
                  sum, sum);
    }
    return 0;
}

int mem_load_kernal(const char *rom_name)
{
    int trapfl;

    if (!vicrom_loaded)
        return 0;

    /* disable traps before saving the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*) &trapfl);
    resources_set_value("VirtualDevices", (resource_value_t) 1);

    if (!IS_NULL(rom_name)) {
        /* Load Kernal ROM. */
        if (sysfile_load(rom_name,
            kernal_rom, VIC20_KERNAL_ROM_SIZE,
            VIC20_KERNAL_ROM_SIZE) < 0) {
            log_error(vic20_mem_log, "Couldn't load kernal ROM.");
            resources_set_value("VirtualDevices", (resource_value_t) trapfl);
            return -1;
        }
    }

    mem_kernal_checksum();

    resources_set_value("VirtualDevices", (resource_value_t) trapfl);

    return 0;
}

int mem_basic_checksum(void)
{
    int i;
    WORD sum;

    /* Check Basic ROM. */
    for (i = 0, sum = 0; i < VIC20_BASIC_ROM_SIZE; i++)
        sum += basic_rom[i];

    if (sum != VIC20_BASIC_CHECKSUM)
        log_error(vic20_mem_log,
                  "Warning: Unknown Basic image.  Sum: %d ($%04X).",
                  sum, sum);
    return 0;
}

int mem_load_basic(const char *rom_name)
{
    if (!vicrom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load Basic ROM. */
        if (sysfile_load(rom_name,
            basic_rom, VIC20_BASIC_ROM_SIZE,
            VIC20_BASIC_ROM_SIZE) < 0) {
            log_error(vic20_mem_log, "Couldn't load basic ROM.");
            return -1;
        }
    }
    return mem_basic_checksum();
}

int mem_load_chargen(const char *rom_name)
{
    if (!vicrom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load chargen ROM. */
        if (sysfile_load(rom_name,
            chargen_rom + 0x400, VIC20_CHARGEN_ROM_SIZE,
            VIC20_CHARGEN_ROM_SIZE) < 0) {
            log_error(vic20_mem_log, "Couldn't load character ROM.");
            return -1;
        }
    }
    return 0;
}

/* Load ROMs at startup.  This is half-stolen from the old `load_mem()' in
   `memory.c'. */
int mem_load(void)
{
    char *rom_name = NULL;

    if (vic20_mem_log == LOG_ERR)
        vic20_mem_log = log_open("VIC20MEM");

    mem_powerup();

    vicrom_loaded = 1;

    if (resources_get_value("KernalName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_kernal(rom_name) < 0)
        return -1;

    if (resources_get_value("BasicName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_basic(rom_name) < 0)
        return -1;

    if (resources_get_value("ChargenName", (resource_value_t)&rom_name) < 0)
        return -1;
    if( mem_load_chargen(rom_name) < 0)
        return -1;

    /* patch the kernal respecting the video mode */ 
    mem_patch_kernal();

    return 0;
}

/* ------------------------------------------------------------------------- */

void mem_attach_cartridge(int type, BYTE * rawcart)
{
    switch(type) {
      case CARTRIDGE_VIC20_4KB_2000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $2000.");
        memcpy(cartrom + 0x2000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A;
        resources_set_value("RAMBlock1", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_8KB_2000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $2000.");
        memcpy(cartrom + 0x2000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A | VIC_ROM_BLK1B;
        resources_set_value("RAMBlock1", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_16KB_2000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $2000.");
        memcpy(cartrom + 0x2000, rawcart, 0x2000);
        memcpy(cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A | VIC_ROM_BLK1B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_value("RAMBlock1", (resource_value_t)0);
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;

      case CARTRIDGE_VIC20_4KB_4000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $4000.");
        memcpy(cartrom + 0x4000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A;
        resources_set_value("RAMBlock2", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_8KB_4000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $4000.");
        memcpy(cartrom + 0x4000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A | VIC_ROM_BLK2B;
        resources_set_value("RAMBlock2", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_16KB_4000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $4000.");
        memcpy(cartrom + 0x4000, rawcart, 0x2000);
        memcpy(cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A | VIC_ROM_BLK2B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_value("RAMBlock2", (resource_value_t)0);
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;

      case CARTRIDGE_VIC20_4KB_6000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $6000.");
        memcpy(cartrom + 0x6000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A;
        resources_set_value("RAMBlock3", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_8KB_6000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $6000.");
        memcpy(cartrom + 0x6000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A | VIC_ROM_BLK3B;
        resources_set_value("RAMBlock3", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_16KB_6000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $6000.");
        memcpy(cartrom + 0x6000, rawcart, 0x2000);
        memcpy(cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A | VIC_ROM_BLK3B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_value("RAMBlock3", (resource_value_t)0);
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;

      case CARTRIDGE_VIC20_4KB_A000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $A000.");
        memcpy(cartrom + 0xa000, rawcart, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5A;
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;
      case CARTRIDGE_VIC20_8KB_A000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $A000.");
        memcpy(cartrom + 0xA000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;

      case CARTRIDGE_VIC20_4KB_B000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $B000.");
        memcpy(cartrom + 0xB000, rawcart, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5B;
        resources_set_value("RAMBlock5", (resource_value_t)0);
        break;
      default:
        log_error(vic20_mem_log, "Unknown Cartridge Type!");
        return;
    }

    mem_initialize_memory();
    return;
}

void mem_detach_cartridge(int type)
{
    switch(type) {
      case CARTRIDGE_VIC20_16KB_2000:
        mem_rom_blocks &= ~(VIC_ROM_BLK5A | VIC_ROM_BLK5B);
      case CARTRIDGE_VIC20_8KB_2000:
      case CARTRIDGE_VIC20_4KB_2000:
        mem_rom_blocks &= ~(VIC_ROM_BLK1A | VIC_ROM_BLK1B);
        break;
      case CARTRIDGE_VIC20_16KB_4000:
        mem_rom_blocks &= ~(VIC_ROM_BLK5A | VIC_ROM_BLK5B);
      case CARTRIDGE_VIC20_8KB_4000:
      case CARTRIDGE_VIC20_4KB_4000:
        mem_rom_blocks &= ~(VIC_ROM_BLK2A | VIC_ROM_BLK2B);
        break;
      case CARTRIDGE_VIC20_16KB_6000:
        mem_rom_blocks &= ~(VIC_ROM_BLK5A | VIC_ROM_BLK5B);
      case CARTRIDGE_VIC20_8KB_6000:
      case CARTRIDGE_VIC20_4KB_6000:
        mem_rom_blocks &= ~(VIC_ROM_BLK3A | VIC_ROM_BLK3B);
        break;
      case CARTRIDGE_VIC20_8KB_A000:
      case CARTRIDGE_VIC20_4KB_A000:
      case CARTRIDGE_VIC20_4KB_B000:
        mem_rom_blocks &= ~(VIC_ROM_BLK5A | VIC_ROM_BLK5B);
        break;
      default:
        return;
    }

    mem_initialize_memory();
    return;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this part needs to be checked. */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
    if (start != NULL)
        *start = ram[0x2b] | (ram[0x2c] << 8);
    if (end != NULL)
        *end = ram[0x2d] | (ram[0x2e] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    ram[0x2b] = ram[0xac] = start & 0xff;
    ram[0x2c] = ram[0xad] = start >> 8;
    ram[0x2d] = ram[0x2f] = ram[0x31] = ram[0xae] = end & 0xff;
    ram[0x2e] = ram[0x30] = ram[0x32] = ram[0xaf] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(ADDRESS addr)
{
    return addr >= 0xe000;
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor */

/* Exported banked memory access functions for the monitor */

/* FIXME: peek */

static const char *banknames[] = {
    "default", "cpu", NULL
};

const int banknums[] = {
    0, 0
};

const char **mem_bank_list(void)
{
    return banknames;
}

int mem_bank_from_name(const char *name)
{
    int i = 0;

    while (banknames[i]) {
        if (!strcmp(name, banknames[i])) {
            return banknums[i];
        }
        i++;
    }
    return -1;
}

BYTE mem_bank_read(int bank, ADDRESS addr)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);
        break;
    }
    return 0xff;
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);  /* FIXME */
        break;
    }
    return mem_bank_read(bank, addr);
}

void mem_bank_write(int bank, ADDRESS addr, BYTE byte)
{
    switch (bank) {
      case 0:                   /* current */
        mem_store(addr, byte);
        return;
    }
}

mem_ioreg_list_t *mem_ioreg_list_get(void)
{
    mem_ioreg_list_t *mem_ioreg_list;

    mem_ioreg_list = (mem_ioreg_list_t *)xmalloc(sizeof(mem_ioreg_list_t) * 3);

    mem_ioreg_list[0].name = "VIC";
    mem_ioreg_list[0].start = 0x9000;
    mem_ioreg_list[0].end = 0x900f;
    mem_ioreg_list[0].next = &mem_ioreg_list[1];

    mem_ioreg_list[1].name = "VIA1";
    mem_ioreg_list[1].start = 0x9120;
    mem_ioreg_list[1].end = 0x912f;
    mem_ioreg_list[1].next = &mem_ioreg_list[2];

    mem_ioreg_list[2].name = "VIA2";
    mem_ioreg_list[2].start = 0x9110;
    mem_ioreg_list[2].end = 0x911f;
    mem_ioreg_list[2].next = NULL;

    return mem_ioreg_list;
}

void mem_get_screen_parameter(ADDRESS *base, BYTE *rows, BYTE *columns)
{
    /* FIXME */
    *base = 0x1000;
    *rows = 23;
    *columns = 22;
}

/************************************************************************/

/* This is a light version of C64's patchrom to change between PAL and
   NTSC kernal
    0: kernal ROM 901486-07 (VIC20 PAL)
    1: kernal ROM 901486-06 (VIC20 NTSC)
*/
#define PATCH_VERSIONS 1

int mem_patch_kernal(void)
{
    static unsigned short patch_bytes[] = {
        1, 0xE475,
            0xe8,
            0x41,

        2, 0xEDE4,
            0x0c, 0x26,
            0x05, 0x19,

        6, 0xFE3F,
            0x26, 0x8d, 0x24, 0x91, 0xa9, 0x48,
            0x89, 0x8d, 0x24, 0x91, 0xa9, 0x42,

        21, 0xFF5C,
            0xe6, 0x2a, 0x78, 0x1c, 0x49, 0x13, 0xb1, 0x0f,
                0x0a, 0x0e, 0xd3, 0x06, 0x38, 0x03, 0x6a, 0x01,
                0xd0, 0x00, 0x83, 0x00, 0x36,
            0x92, 0x27, 0x40, 0x1a, 0xc6, 0x11, 0x74, 0x0e,
                0xee, 0x0c, 0x45, 0x06, 0xf1, 0x02, 0x46, 0x01,
                0xb8, 0x00, 0x71, 0x00, 0x2a,

        0, 00
    };

    int rev, video_mode;
    short bytes, n, i = 0;
    ADDRESS a;

    resources_get_value("MachineVideoStandard", (resource_value_t*)&video_mode);

    switch (video_mode) {
      case MACHINE_SYNC_PAL:    
        rev = 0;    /* use kernal 901486-07 */
        break;
      case MACHINE_SYNC_NTSC:    
        rev = 1;    /* use kernal 901486-06 */
        break;
      default:
        log_message(LOG_ERR, "VIC20MEM: unknown sync, cannot patch kernal.");
        return -1;
    }

    while ((bytes = patch_bytes[i++]) > 0) {
    	a = (ADDRESS)patch_bytes[i++];

	    i += (bytes * rev);	/* select patch */
	    for(n = bytes; n--;)
	        rom_store(a++, (BYTE)patch_bytes[i++]);

	    i += (bytes * (PATCH_VERSIONS - rev));	/* skip patch */
    }

    log_message(LOG_DEFAULT, "VIC20 kernal patched to 901486-0%d.",7-rev);

    return 0;
}

