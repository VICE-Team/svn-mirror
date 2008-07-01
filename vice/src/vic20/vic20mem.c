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
#include "emuid.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"

#ifdef WATCOM_COMPILE
#include "../mem.h"
#else
#include "mem.h"
#endif

#include "monitor.h"
#include "ram.h"
#include "resources.h"
#include "sid-resources.h"
#include "sid.h"
#include "types.h"
#include "ui.h"
#include "vic-mem.h"
#include "vic20-resources.h"
#include "vic20ieeevia.h"
#include "vic20mem.h"
#include "vic20memrom.h"
#include "vic20via.h"


static log_t vic20_mem_log = LOG_ERR;

/*----------------------------------------------------------------------*/

/* Old program counter.  Not used without MMU support.  */
unsigned int mem_old_reg_pc;

/* ------------------------------------------------------------------------- */

/* The VIC20 memory. */
BYTE mem_ram[VIC20_RAM_SIZE];

BYTE mem_cartrom[0x10000];

/* Last data read/write by the cpu, this value lingers on the C(PU)-bus and
   gets used when the CPU reads from unconnected space on the C(PU)-bus */
BYTE vic20_cpu_last_data;

/* Memory read and write tables.  */
/*
static read_func_ptr_t _mem_read_tab[0x101];
static store_func_ptr_t _mem_write_tab[0x101];
*/
static BYTE *_mem_read_base_tab[0x101];
static int mem_read_limit_tab[0x101];

/* These ones are used when watchpoints are turned on.  */
static read_func_ptr_t _mem_read_tab_watch[0x101];
static store_func_ptr_t _mem_write_tab_watch[0x101];
static read_func_ptr_t _mem_read_tab_nowatch[0x101];
static store_func_ptr_t _mem_write_tab_nowatch[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* ------------------------------------------------------------------------- */

static void REGPARM2 store_wrap(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    mem_ram[addr & (VIC20_RAM_SIZE - 1)] = value;
    vic20memrom_chargen_rom[addr & 0x3ff] = value;
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 zero_read(WORD addr)
{
    vic20_cpu_last_data = mem_ram[addr & 0xff];
    return vic20_cpu_last_data;
}

void REGPARM2 zero_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    mem_ram[addr & 0xff] = value;
}

static BYTE REGPARM1 ram_read(WORD addr)
{
    vic20_cpu_last_data = mem_ram[addr];
    return vic20_cpu_last_data;
}

static void REGPARM2 ram_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    mem_ram[addr & (VIC20_RAM_SIZE - 1)] = value;
}

static BYTE REGPARM1 read_cartrom(WORD addr)
{
    vic20_cpu_last_data = mem_cartrom[addr & 0xffff];
    return vic20_cpu_last_data;
}

/* FIXME: Using random values for high nibble instead of VIC fetches */
static BYTE REGPARM1 colorram_read(WORD addr)
{
    vic20_cpu_last_data = mem_ram[addr] | (rand() & 0xf0);
    return vic20_cpu_last_data;
}

static void REGPARM2 colorram_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    mem_ram[addr & (VIC20_RAM_SIZE - 1)] = value & 0xf;
}

static void REGPARM2 via_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    if (addr & 0x10)            /* $911x (VIA2) */
        via2_store(addr, value);
    if (addr & 0x20)            /* $912x (VIA1) */
        via1_store(addr, value);
}

static BYTE REGPARM1 via_read(WORD addr)
{
    vic20_cpu_last_data = 0xff;

    if (addr & 0x10)            /* $911x (VIA2) */
        vic20_cpu_last_data &= via2_read(addr);
    if (addr & 0x20)            /* $912x (VIA1) */
        vic20_cpu_last_data &= via1_read(addr);

    return vic20_cpu_last_data;
}

static BYTE REGPARM1 read_emuid(WORD addr)
{
    addr &= 0xff;

    if (addr >= 0xa0)
        return emuid_read((WORD)(addr - 0xa0));

    return 0xff;
}

static void REGPARM2 store_emuid(WORD addr, BYTE value)
{
#if 0
    addr &= 0xff;
    if (addr == 0xff) {
        emulator_id[addr - 0xa0] ^= 0xff;
    }
#endif
    return;
}

/*-------------------------------------------------------------------*/

static BYTE REGPARM1 io3_read(WORD addr)
{
    if (sidcart_enabled && sidcart_address==1 && addr>=0x9c00 && addr<=0x9c1f)
    {
        vic20_cpu_last_data = sid_read(addr);
        return vic20_cpu_last_data;
    }

    if (emu_id_enabled && (addr & 0xff00) == 0x9f00)
    {
        vic20_cpu_last_data = read_emuid(addr);
        return vic20_cpu_last_data;
    }

    vic20_cpu_last_data = 0xff;
    return 0xff;
}

static void REGPARM2 io3_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;

    if (sidcart_enabled && sidcart_address==1 && addr>=0x9c00 && addr<=0x9c1f) {
        sid_store(addr,value);
        return;
    }

    if (emu_id_enabled && (addr & 0xff00) == 0x9f00)
        store_emuid(addr, value);

    return;
}

static BYTE REGPARM1 io2_read(WORD addr)
{
    if (sidcart_enabled && sidcart_address==0 && addr>=0x9800 && addr<=0x981f)
    {
        vic20_cpu_last_data = sid_read(addr);
        return vic20_cpu_last_data;
    }

    if (ieee488_enabled)
    {
        if (addr & 0x10)
        {
            vic20_cpu_last_data = ieeevia2_read(addr);
            return vic20_cpu_last_data;
        }
        else
        {
            vic20_cpu_last_data = ieeevia1_read(addr);
            return vic20_cpu_last_data;
        }
    }
    vic20_cpu_last_data = 0xff;
    return 0xff;
}

static void REGPARM2 io2_store(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    if (sidcart_enabled && sidcart_address==0 && addr>=0x9800 && addr<=0x981f) {
        sid_store(addr,value);
        return;
    }

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

static BYTE REGPARM1 read_unconnected_c_bus(WORD addr)
{
    return vic20_cpu_last_data;
}

static BYTE REGPARM1 read_dummy(WORD addr)
{
    return (addr >> 8);
}

static void REGPARM2 store_dummy(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
}


/* Watchpoint functions */


static BYTE REGPARM1 read_watch(WORD addr)
{
    monitor_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab_nowatch[addr >> 8](addr);
}

static void REGPARM2 store_watch(WORD addr, BYTE value)
{
    monitor_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab_nowatch[addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

/* Generic memory access.  */

void REGPARM2 mem_store(WORD addr, BYTE value)
{
    _mem_write_tab_ptr[addr >> 8](addr, value);
}

BYTE REGPARM1 mem_read(WORD addr)
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

static int vic20_mem_enable_rom_block(int num)
{
    if (num == 1 || num == 2 || num == 3 || num == 5) {
#if 0  /* changed to have the vic20 cpu always use the function */
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_cartrom, store_dummy,
                mem_cartrom, 0xffff);
#endif
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_cartrom, store_dummy,
                NULL, 0);

        return 0;
    } else
        return -1;
}

int vic20_mem_enable_ram_block(int num)
{
    if (num == 0) {
#if 0  /* changed to have the vic20 cpu always use the function */
        set_mem(0x04, 0x0f,
                ram_read, ram_store,
                mem_ram, 0xffff);
#endif
        set_mem(0x04, 0x0f,
                ram_read, ram_store,
                NULL, 0);
        return 0;
    } else if (num > 0 && num != 4 && num <= 5) {
#if 0  /* changed to have the vic20 cpu always use the function */
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                ram_read, ram_store,
                mem_ram, 0xffff);
#endif
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                ram_read, ram_store,
                NULL, 0);
        return 0;
    } else
        return -1;
}

int vic20_mem_disable_ram_block(int num)
{
    if (num == 0) {
#if 0  /* changed to have the vic20 cpu always use the function */
        set_mem(0x04, 0x0f,
                read_dummy, store_dummy,
                mem_ram, 0xffff);
#endif
        set_mem(0x04, 0x0f,
                read_dummy, store_dummy,
                NULL, 0);
        return 0;
    } else if (num > 0 && num != 4 && num <= 5) {
#if 0  /* changed to have the vic20 cpu always use the function */
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_dummy, store_dummy,
                mem_ram, 0xffff);
#endif
        set_mem(num * 0x20, num * 0x20 + 0x1f,
                read_unconnected_c_bus, store_dummy,
                NULL, 0);
        return 0;
    } else
        return -1;
}

void mem_initialize_memory(void)
{
    int i;

    /* Setup low standard RAM at $0000-$03FF. */
#if 0  /* changed to have the vic20 cpu always use the function */
    set_mem(0x00, 0x03,
            ram_read, ram_store,
            mem_ram, 0xffff);
#endif
    set_mem(0x00, 0x03,
            ram_read, ram_store,
            NULL, 0);

    /* Setup more low RAM at $1000-$1FFF.  */
#if 0  /* changed to have the vic20 cpu always use the function */
    set_mem(0x10, 0x1b,
            ram_read, ram_store,
            mem_ram, 0xffff);
    set_mem(0x1c, 0x1f,
            ram_read, store_wrap,
            mem_ram, 0xffff);
#endif
    set_mem(0x10, 0x1b,
            ram_read, ram_store,
            NULL, 0);
    set_mem(0x1c, 0x1f,
            ram_read, store_wrap,
            NULL, 0);

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
#if 0  /* changed to have the vic20 cpu always use the function */
    set_mem(0x80, 0x8f,
            vic20memrom_chargen_read, store_dummy,
            vic20memrom_chargen_rom + 0x400, 0x0fff);
#endif
    set_mem(0x80, 0x8f,
            vic20memrom_chargen_read, store_dummy,
            NULL, 0);

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
#if 0  /* changed to have the vic20 cpu always use the function */
    set_mem(0x94, 0x97,
            colorram_read, colorram_store,
            mem_ram, 0xffff);
#endif
    set_mem(0x94, 0x97,
            colorram_read, colorram_store,
            NULL, 0);

    /* Setup I/O2 at the expansion port */
    set_mem(0x98, 0x9b,
            io2_read, io2_store,
            NULL, 0);

    /* Setup I/O3 at the expansion port (includes emulator ID) */
    set_mem(0x9c, 0x9f,
            io3_read, io3_store,
            NULL, 0);

    /* Setup BASIC ROM at $C000-$DFFF. */
#if 0  /* changed to have the vic20 cpu always use the function */
    set_mem(0xc0, 0xdf,
            vic20memrom_basic_read, store_dummy,
            vic20memrom_basic_rom, 0x1fff);
#endif
    set_mem(0xc0, 0xdf,
            vic20memrom_basic_read, store_dummy,
            NULL, 0);

    /* Setup Kernal ROM at $E000-$FFFF. */
    set_mem(0xe0, 0xff,
            vic20memrom_kernal_read, store_dummy,
            vic20memrom_kernal_trap_rom, 0x1fff);

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

    mem_toggle_watchpoints(0, NULL);
}

void mem_toggle_watchpoints(int flag, void *context)
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
    ram_init(mem_ram, 0x8000);
    memset(mem_ram + 0x8000, 0, 0x8000);
}

/* ------------------------------------------------------------------------- */

void mem_attach_cartridge(int type, BYTE * rawcart)
{
    switch(type) {
      case CARTRIDGE_VIC20_4KB_2000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $2000.");
        memcpy(mem_cartrom + 0x2000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A;
        resources_set_int("RAMBlock1", 0);
        break;
      case CARTRIDGE_VIC20_8KB_2000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $2000.");
        memcpy(mem_cartrom + 0x2000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A | VIC_ROM_BLK1B;
        resources_set_int("RAMBlock1", 0);
        break;
      case CARTRIDGE_VIC20_16KB_2000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $2000.");
        memcpy(mem_cartrom + 0x2000, rawcart, 0x2000);
        memcpy(mem_cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK1A | VIC_ROM_BLK1B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_int("RAMBlock1", 0);
        resources_set_int("RAMBlock5", 0);
        break;

      case CARTRIDGE_VIC20_4KB_4000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $4000.");
        memcpy(mem_cartrom + 0x4000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A;
        resources_set_int("RAMBlock2", 0);
        break;
      case CARTRIDGE_VIC20_8KB_4000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $4000.");
        memcpy(mem_cartrom + 0x4000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A | VIC_ROM_BLK2B;
        resources_set_int("RAMBlock2", 0);
        break;
      case CARTRIDGE_VIC20_16KB_4000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $4000.");
        memcpy(mem_cartrom + 0x4000, rawcart, 0x2000);
        memcpy(mem_cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK2A | VIC_ROM_BLK2B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_int("RAMBlock2", 0);
        resources_set_int("RAMBlock5", 0);
        break;

      case CARTRIDGE_VIC20_4KB_6000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $6000.");
        memcpy(mem_cartrom + 0x6000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A;
        resources_set_int("RAMBlock3", 0);
        break;
      case CARTRIDGE_VIC20_8KB_6000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $6000.");
        memcpy(mem_cartrom + 0x6000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A | VIC_ROM_BLK3B;
        resources_set_int("RAMBlock3", 0);
        break;
      case CARTRIDGE_VIC20_16KB_6000:
        log_message(vic20_mem_log, "CART: attaching 16KB cartridge at $6000.");
        memcpy(mem_cartrom + 0x6000, rawcart, 0x2000);
        memcpy(mem_cartrom + 0xA000, rawcart + 0x2000, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK3A | VIC_ROM_BLK3B
                        | VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_int("RAMBlock3", 0);
        resources_set_int("RAMBlock5", 0);
        break;

      case CARTRIDGE_VIC20_4KB_A000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $A000.");
        memcpy(mem_cartrom + 0xa000, rawcart, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5A;
        resources_set_int("RAMBlock5", 0);
        break;
      case CARTRIDGE_VIC20_8KB_A000:
        log_message(vic20_mem_log, "CART: attaching 8KB cartridge at $A000.");
        memcpy(mem_cartrom + 0xA000, rawcart, 0x2000);
        mem_rom_blocks |= VIC_ROM_BLK5A | VIC_ROM_BLK5B;
        resources_set_int("RAMBlock5", 0);
        break;

      case CARTRIDGE_VIC20_4KB_B000:
        log_message(vic20_mem_log, "CART: attaching 4KB cartridge at $B000.");
        memcpy(mem_cartrom + 0xB000, rawcart, 0x1000);
        mem_rom_blocks |= VIC_ROM_BLK5B;
        resources_set_int("RAMBlock5", 0);
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

void mem_get_basic_text(WORD *start, WORD *end)
{
    if (start != NULL)
        *start = mem_ram[0x2b] | (mem_ram[0x2c] << 8);
    if (end != NULL)
        *end = mem_ram[0x2d] | (mem_ram[0x2e] << 8);
}

void mem_set_basic_text(WORD start, WORD end)
{
    mem_ram[0x2b] = mem_ram[0xac] = start & 0xff;
    mem_ram[0x2c] = mem_ram[0xad] = start >> 8;
    mem_ram[0x2d] = mem_ram[0x2f] = mem_ram[0x31] = mem_ram[0xae] = end & 0xff;
    mem_ram[0x2e] = mem_ram[0x30] = mem_ram[0x32] = mem_ram[0xaf] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(WORD addr)
{
    return addr >= 0xe000;
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor */

/* Exported banked memory access functions for the monitor */

/* FIXME: peek */

static const char *banknames[] = { "default", "cpu", NULL };

static const int banknums[] = { 0, 0 };

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

BYTE mem_bank_read(int bank, WORD addr, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);
        break;
    }
    return 0xff;
}

BYTE mem_bank_peek(int bank, WORD addr, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);  /* FIXME */
        break;
    }
    return mem_bank_read(bank, addr, context);
}

void mem_bank_write(int bank, WORD addr, BYTE byte, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        mem_store(addr, byte);
        return;
    }
}

mem_ioreg_list_t *mem_ioreg_list_get(void *context)
{
    mem_ioreg_list_t *mem_ioreg_list = NULL;

    mon_ioreg_add_list(&mem_ioreg_list, "VIC", 0x9000, 0x900f);
    mon_ioreg_add_list(&mem_ioreg_list, "VIA1", 0x9120, 0x912f);
    mon_ioreg_add_list(&mem_ioreg_list, "VIA2", 0x9110, 0x911f);

    return mem_ioreg_list;
}

void mem_get_screen_parameter(WORD *base, BYTE *rows, BYTE *columns)
{
    *base = ((vic_peek(0x9005) & 0x80) ? 0 : 0x8000) + ((vic_peek(0x9005) & 0x70) << 6) + ((vic_peek(0x9002) & 0x80) << 2);
    *rows = (vic_peek(0x9003) & 0x7e) >> 1;
    *columns = vic_peek(0x9002) & 0x7f;
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
    static unsigned short const patch_bytes[] = {
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
    WORD a;

    resources_get_int("MachineVideoStandard", &video_mode);

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
        a = (WORD)patch_bytes[i++];

        i += (bytes * rev);	/* select patch */
        for(n = bytes; n--;) {
	        vic20memrom_trap_store(a, (BYTE)patch_bytes[i]);
	        rom_store(a++, (BYTE)patch_bytes[i++]);
        }

	    i += (bytes * (PATCH_VERSIONS - rev));	/* skip patch */
    }

    log_message(LOG_DEFAULT, "VIC20 kernal patched to 901486-0%d.",7-rev);

    return 0;
}

