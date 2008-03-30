/*
 * c128mem.c -- Memory handling for the C128 emulator.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta <jopi@stekt.oulu.fi>
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

#include "c128-resources.h"
#include "c128io.h"
#include "c128mem.h"
#include "c128memlimit.h"
#include "c128mmu.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64pla.h"
#include "c64tpi.h"
#include "functionrom.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mon.h"
#include "parallel.h"
#include "reu.h"
#include "sid.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vdc-mem.h"
#include "vdc.h"
#include "vicii-mem.h"
#include "vicii-phi1.h"
#include "vicii.h"
#include "z80mem.h"

#ifdef HAVE_RS232
#include "c64acia.h"
#endif

/* #define DEBUG_MMU */

#ifdef DEBUG_MMU
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif

/* ------------------------------------------------------------------------- */

const char *mem_romset_resources_list[] = {
    "KernalName", "ChargenName", "BasicName",
    "DosName2031", "DosName1001",
    "DosName1541", "DosName1571", "DosName1581", "DosName1541ii",
    NULL
};

/* ------------------------------------------------------------------------- */

/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      4

/* The C128 memory.  */
BYTE mem_ram[C128_RAM_SIZE];
BYTE mem_basic_rom[C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE];
BYTE mem_kernal_rom[C128_KERNAL_ROM_SIZE];
BYTE mem_chargen_rom[C128_CHARGEN_ROM_SIZE];

BYTE basic64_rom[C128_BASIC64_ROM_SIZE];
BYTE kernal64_rom[C128_KERNAL64_ROM_SIZE];
BYTE chargen64_rom[C128_CHARGEN64_ROM_SIZE];

/* Internal color memory.  */
static BYTE mem_color_ram[0x800];
BYTE *mem_color_ram_ptr;

/* Pointer to the chargen ROM.  */
BYTE *mem_chargen_rom_ptr;

/* Size of RAM...  */
int ram_size = C128_RAM_SIZE;

/* Currently selected RAM bank.  */
BYTE *ram_bank;

/* Shared memory.  */
static ADDRESS top_shared_limit, bottom_shared_limit;

/* Pointers to pages 0 and 1 (which can be physically placed anywhere).  */
BYTE *mem_page_zero, *mem_page_one;

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int mem_old_reg_pc;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

#define NUM_CONFIGS 256

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed.  */
static int tape_sense = 0;

/* Current memory configuration.  */
static int mem_config;

/* Logging goes here.  */
static log_t c128_mem_log = LOG_ERR;

/* Status of the CAPS key (ASCII/DIN).  */
static int caps_sense = 1;

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}


void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    mem_write_tab[mem_config][addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

void mem_update_config(int config)
{
    mem_config = config;

    if (any_watchpoints(e_comp_space)) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[mem_config];
    }

    _mem_read_base_tab_ptr = mem_read_base_tab[config];
    mem_read_limit_tab_ptr = mem_read_limit_tab[config];

    if (bank_limit != NULL) {
        *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8];
        if (*bank_base != 0)
            *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8]
                         - (mem_old_reg_pc & 0xff00);
        *bank_limit = mem_read_limit_tab_ptr[mem_old_reg_pc >> 8];
    }

    if (config >= 0x80) {
        mem_color_ram_ptr = mem_color_ram;
    } else {
        if (pport.data_read & 1)
            mem_color_ram_ptr = mem_color_ram;
        else
            mem_color_ram_ptr = &mem_color_ram[0x400];
    }
}

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

void mem_set_ram_config(BYTE value)
{
    int shared_size;

    /* XXX: We only support 128K here.  */
    vic_ii_set_ram_base(mem_ram + ((value & 0x40) << 10));

    DEBUG_PRINT(("MMU: Store RCR = $%02x\n", value));
    DEBUG_PRINT(("MMU: VIC-II base at $%05X\n", ((value & 0xc0) << 2)));

    if ((value & 0x3) == 0)
        shared_size = 1024;
    else
        shared_size = 0x1000 << ((value & 0x3) - 1);

    /* Share high memory?  */
    if (value & 0x8) {
        top_shared_limit = 0xffff - shared_size;
        DEBUG_PRINT(("MMU: Sharing high RAM from $%04X\n",
              top_shared_limit + 1));
    } else {
        top_shared_limit = 0xffff;
        DEBUG_PRINT(("MMU: No high shared RAM\n"));
    }

    /* Share low memory?  */
    if (value & 0x4) {
        bottom_shared_limit = shared_size;
        DEBUG_PRINT(("MMU: Sharing low RAM up to $%04X\n",
              bottom_shared_limit - 1));
    } else {
        bottom_shared_limit = 0;
        DEBUG_PRINT(("MMU: No low shared RAM\n"));
    }
}

/* ------------------------------------------------------------------------- */

void mem_pla_config_changed(void)
{
    BYTE *old_chargen_rom_ptr;

    c64pla_config_changed(tape_sense, caps_sense, 0x57);

    mmu_set_config64(((~pport.dir | pport.data) & 0x7)
                     | (export.exrom << 3) | (export.game << 4));

    old_chargen_rom_ptr = mem_chargen_rom_ptr;

    if (pport.data_read & 0x40)
        mem_chargen_rom_ptr = mem_chargen_rom;
    else
        mem_chargen_rom_ptr = &mem_chargen_rom[0x1000];

    if (old_chargen_rom_ptr != mem_chargen_rom_ptr)
        machine_update_memory_ptrs();
}

static void mem_toggle_caps_key(void)
{
    caps_sense = (caps_sense) ? 0 : 1;
    mem_pla_config_changed();
    log_message(c128_mem_log, "CAPS key (ASCII/DIN) %s.",
                (caps_sense) ? "released" : "pressed");
}

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        return pport.dir_read;
      case 1:
        return pport.data_read;
    }

    return mem_page_zero[addr];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    addr &= 0xff;

    switch ((BYTE) addr) {
      case 0:
        if (0 /*vbank == 0*/) {
            vicii_mem_vbank_store((ADDRESS)0, vicii_read_phi1());
        } else {
            mem_page_zero[0] = vicii_read_phi1();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.dir != value) {
            pport.dir = value;
            mem_pla_config_changed();
        }
        break;
      case 1:
        if (0 /*vbank == 0*/) {
            vicii_mem_vbank_store((ADDRESS)1, vicii_read_phi1());
        } else {
            mem_page_zero[1] = vicii_read_phi1();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.data != value) {
            pport.data = value;
            mem_pla_config_changed();
        }
        break;
      default:
        mem_page_zero[addr] = value;
    }
}

/* ------------------------------------------------------------------------- */

/* External memory access functions.  */

BYTE REGPARM1 basic_read(ADDRESS addr)
{
    return mem_basic_rom[addr - 0x4000];
}

void REGPARM2 basic_store(ADDRESS addr, BYTE value)
{
    mem_basic_rom[addr - 0x4000] = value;
}

BYTE REGPARM1 kernal_read(ADDRESS addr)
{
    return mem_kernal_rom[addr & 0x1fff];
}

void REGPARM2 kernal_store(ADDRESS addr, BYTE value)
{
    mem_kernal_rom[addr & 0x1fff] = value;
}

BYTE REGPARM1 read_chargen(ADDRESS addr)
{
    return mem_chargen_rom_ptr[addr & 0x0fff];
}

BYTE REGPARM1 basic64_read(ADDRESS addr)
{
    return basic64_rom[addr & 0x1fff];
}

void REGPARM2 basic64_store(ADDRESS addr, BYTE value)
{
    basic64_rom[addr & 0x1fff] = value;
}

BYTE REGPARM1 kernal64_read(ADDRESS addr)
{
    return kernal64_rom[addr & 0x1fff];
}

void REGPARM2 kernal64_store(ADDRESS addr, BYTE value)
{
    kernal64_rom[addr & 0x1fff] = value;
}

BYTE REGPARM1 chargen64_read(ADDRESS addr)
{
    return mem_chargen_rom_ptr[addr & 0x0fff];
    /*return chargen64_rom[addr & 0xfff];*/
}

void REGPARM2 chargen64_store(ADDRESS addr, BYTE value)
{
    chargen64_rom[addr & 0xfff] = value;
}

BYTE REGPARM1 rom_read(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0x0000:
        return bios_read(addr);
      case 0x4000:
      case 0x5000:
      case 0x6000:
      case 0x7000:
      case 0x8000:
      case 0x9000:
      case 0xa000:
      case 0xb000:
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
      case 0x0000:
        bios_store(addr, value);
        break;
      case 0x4000:
      case 0x5000:
      case 0x6000:
      case 0x7000:
      case 0x8000:
      case 0x9000:
      case 0xa000:
      case 0xb000:
        basic_store(addr, value);
        break;
      case 0xe000:
      case 0xf000:
        kernal_store(addr, value);
        break;
    }
}

BYTE REGPARM1 rom64_read(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0xa000:
      case 0xb000:
        return basic64_read(addr);
      case 0xd000:
        return chargen64_read(addr);
      case 0xe000:
      case 0xf000:
        return kernal64_read(addr);
    }

    return 0;
}

void REGPARM2 rom64_store(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0xa000:
      case 0xb000:
        basic64_store(addr, value);
        break;
      case 0xd000:
        chargen64_store(addr, value);
        break;
      case 0xe000:
      case 0xf000:
        kernal64_store(addr, value);
        break;
    }
}

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

/* CPU Memory interface.  */

/* The MMU can basically do the following:

   - select one of the two (four) memory banks as the standard
   (non-shared) memory;

   - turn ROM and I/O on and off;

   - enable/disable top/bottom shared RAM (from 1K to 16K, so bottom
   shared RAM cannot go beyond $3FFF and top shared RAM cannot go
   under $C000);

   - move pages 0 and 1 to any physical address.  */

#define READ_TOP_SHARED(addr)                     \
    ((addr) > top_shared_limit ? mem_ram[(addr)]  \
                               : ram_bank[(addr)])

#define STORE_TOP_SHARED(addr, value)                        \
    ((addr) > top_shared_limit ? (mem_ram[(addr)] = (value)) \
                               : (ram_bank[(addr)] = (value)))

#define READ_BOTTOM_SHARED(addr)                     \
    ((addr) < bottom_shared_limit ? mem_ram[(addr)]  \
                                  : ram_bank[(addr)])

#define STORE_BOTTOM_SHARED(addr, value)                         \
    ((addr) < bottom_shared_limit ? (mem_ram[(addr)] = (value))  \
                                  : (ram_bank[(addr)] = (value)))


static BYTE REGPARM1 read_one(ADDRESS addr)
{
    return mem_page_one[addr - 0x100];
}

static void REGPARM2 store_one(ADDRESS addr, BYTE value)
{
    mem_page_one[addr - 0x100] = value;
}


/* $0200 - $3FFF: RAM (normal or shared).  */

BYTE REGPARM1 lo_read(ADDRESS addr)
{
    return READ_BOTTOM_SHARED(addr);
}

void REGPARM2 lo_store(ADDRESS addr, BYTE value)
{
    STORE_BOTTOM_SHARED(addr, value);
}

static BYTE REGPARM1 ram_read(ADDRESS addr)
{
    return ram_bank[addr];
}

static void REGPARM2 ram_store(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $4000 - $7FFF: RAM or low BASIC ROM.  */
static BYTE REGPARM1 basic_lo_read(ADDRESS addr)
{
    return mem_basic_rom[addr - 0x4000];
}

static void REGPARM2 basic_lo_store(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $8000 - $BFFF: RAM or high BASIC ROM.  */
static BYTE REGPARM1 basic_hi_read(ADDRESS addr)
{
    return mem_basic_rom[addr - 0x4000];
}

static void REGPARM2 basic_hi_store(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $C000 - $CFFF: RAM (normal or shared) or Editor ROM.  */
static BYTE REGPARM1 editor_read(ADDRESS addr)
{
    return mem_basic_rom[addr - 0x4000];
}

static void REGPARM2 editor_store(ADDRESS addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

BYTE REGPARM1 d7xx_read(ADDRESS addr)
{
#if 0                           /*def HAVE_RS232 */
    if (acia_d7_enabled)
        return acia2_read(addr);
#endif
    return 0xff;
}

void REGPARM2 d7xx_store(ADDRESS addr, BYTE value)
{
#if 0                           /*def HAVE_RS232 */
        if (acia_d7_enabled)
            acia2_read(addr, value);
#endif
}

/* $E000 - $FFFF: RAM or Kernal.  */
static BYTE REGPARM1 hi_read(ADDRESS addr)
{
    return mem_kernal_rom[addr & 0x1fff];
}

static void REGPARM2 hi_store(ADDRESS addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

BYTE REGPARM1 top_shared_read(ADDRESS addr)
{
    return READ_TOP_SHARED(addr);
}

void REGPARM2 top_shared_store(ADDRESS addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

/* ------------------------------------------------------------------------- */

void REGPARM2 colorram_store(ADDRESS addr, BYTE value)
{
    mem_color_ram[addr & 0x3ff] = value & 0xf;
}

BYTE REGPARM1 colorram_read(ADDRESS addr)
{
    return mem_color_ram[addr & 0x3ff] | (vicii_read_phi1() & 0xf0);
}

/* ------------------------------------------------------------------------- */

#ifdef _MSC_VER
#pragma optimize("",off)
#endif

void mem_initialize_memory(void)
{
    int i, j;

    mem_chargen_rom_ptr = mem_chargen_rom;
    mem_color_ram_ptr = mem_color_ram;

    mem_limit_init(mem_read_limit_tab);

    for (i = 0; i <= 0x100; i++) {
        mem_read_tab_watch[i] = read_watch;
        mem_write_tab_watch[i] = store_watch;
    }

    for (j = 0; j < NUM_CONFIGS; j++) {

        for (i = 0; i <= 0x100; i++) {
            mem_read_base_tab[j][i] = NULL;
        }

        mem_read_tab[j][0] = read_zero;
        mem_write_tab[j][0] = store_zero;
        mem_read_tab[j][1] = read_one;
        mem_write_tab[j][1] = store_one;
    }


    for (j = 0; j < 128; j += 64) {
        for (i = 0x02; i <= 0x3f; i++) {
            mem_read_tab[0+j][i] = ram_read;
            mem_read_tab[1+j][i] = ram_read;
            mem_read_tab[2+j][i] = ram_read;
            mem_read_tab[3+j][i] = ram_read;
            mem_read_tab[4+j][i] = ram_read;
            mem_read_tab[5+j][i] = ram_read;
            mem_read_tab[6+j][i] = ram_read;
            mem_read_tab[7+j][i] = ram_read;
            mem_read_tab[8+j][i] = ram_read;
            mem_read_tab[9+j][i] = ram_read;
            mem_read_tab[10+j][i] = ram_read;
            mem_read_tab[11+j][i] = ram_read;
            mem_read_tab[12+j][i] = ram_read;
            mem_read_tab[13+j][i] = ram_read;
            mem_read_tab[14+j][i] = ram_read;
            mem_read_tab[15+j][i] = ram_read;
            mem_read_tab[16+j][i] = ram_read;
            mem_read_tab[17+j][i] = ram_read;
            mem_read_tab[18+j][i] = ram_read;
            mem_read_tab[19+j][i] = ram_read;
            mem_read_tab[20+j][i] = ram_read;
            mem_read_tab[21+j][i] = ram_read;
            mem_read_tab[22+j][i] = ram_read;
            mem_read_tab[23+j][i] = ram_read;
            mem_read_tab[24+j][i] = ram_read;
            mem_read_tab[25+j][i] = ram_read;
            mem_read_tab[26+j][i] = ram_read;
            mem_read_tab[27+j][i] = ram_read;
            mem_read_tab[28+j][i] = ram_read;
            mem_read_tab[29+j][i] = ram_read;
            mem_read_tab[30+j][i] = ram_read;
            mem_read_tab[31+j][i] = ram_read;
            mem_read_tab[32+j][i] = lo_read;
            mem_read_tab[33+j][i] = lo_read;
            mem_read_tab[34+j][i] = lo_read;
            mem_read_tab[35+j][i] = lo_read;
            mem_read_tab[36+j][i] = lo_read;
            mem_read_tab[37+j][i] = lo_read;
            mem_read_tab[38+j][i] = lo_read;
            mem_read_tab[39+j][i] = lo_read;
            mem_read_tab[40+j][i] = lo_read;
            mem_read_tab[41+j][i] = lo_read;
            mem_read_tab[42+j][i] = lo_read;
            mem_read_tab[43+j][i] = lo_read;
            mem_read_tab[44+j][i] = lo_read;
            mem_read_tab[45+j][i] = lo_read;
            mem_read_tab[46+j][i] = lo_read;
            mem_read_tab[47+j][i] = lo_read;
            mem_read_tab[48+j][i] = lo_read;
            mem_read_tab[49+j][i] = lo_read;
            mem_read_tab[50+j][i] = lo_read;
            mem_read_tab[51+j][i] = lo_read;
            mem_read_tab[52+j][i] = lo_read;
            mem_read_tab[53+j][i] = lo_read;
            mem_read_tab[54+j][i] = lo_read;
            mem_read_tab[55+j][i] = lo_read;
            mem_read_tab[56+j][i] = lo_read;
            mem_read_tab[57+j][i] = lo_read;
            mem_read_tab[58+j][i] = lo_read;
            mem_read_tab[59+j][i] = lo_read;
            mem_read_tab[60+j][i] = lo_read;
            mem_read_tab[61+j][i] = lo_read;
            mem_read_tab[62+j][i] = lo_read;
            mem_read_tab[63+j][i] = lo_read;
            mem_write_tab[0+j][i] = ram_store;
            mem_write_tab[1+j][i] = ram_store;
            mem_write_tab[2+j][i] = ram_store;
            mem_write_tab[3+j][i] = ram_store;
            mem_write_tab[4+j][i] = ram_store;
            mem_write_tab[5+j][i] = ram_store;
            mem_write_tab[6+j][i] = ram_store;
            mem_write_tab[7+j][i] = ram_store;
            mem_write_tab[8+j][i] = ram_store;
            mem_write_tab[9+j][i] = ram_store;
            mem_write_tab[10+j][i] = ram_store;
            mem_write_tab[11+j][i] = ram_store;
            mem_write_tab[12+j][i] = ram_store;
            mem_write_tab[13+j][i] = ram_store;
            mem_write_tab[14+j][i] = ram_store;
            mem_write_tab[15+j][i] = ram_store;
            mem_write_tab[16+j][i] = ram_store;
            mem_write_tab[17+j][i] = ram_store;
            mem_write_tab[18+j][i] = ram_store;
            mem_write_tab[19+j][i] = ram_store;
            mem_write_tab[20+j][i] = ram_store;
            mem_write_tab[21+j][i] = ram_store;
            mem_write_tab[22+j][i] = ram_store;
            mem_write_tab[23+j][i] = ram_store;
            mem_write_tab[24+j][i] = ram_store;
            mem_write_tab[25+j][i] = ram_store;
            mem_write_tab[26+j][i] = ram_store;
            mem_write_tab[27+j][i] = ram_store;
            mem_write_tab[28+j][i] = ram_store;
            mem_write_tab[29+j][i] = ram_store;
            mem_write_tab[30+j][i] = ram_store;
            mem_write_tab[31+j][i] = ram_store;
            mem_write_tab[32+j][i] = lo_store;
            mem_write_tab[33+j][i] = lo_store;
            mem_write_tab[34+j][i] = lo_store;
            mem_write_tab[35+j][i] = lo_store;
            mem_write_tab[36+j][i] = lo_store;
            mem_write_tab[37+j][i] = lo_store;
            mem_write_tab[38+j][i] = lo_store;
            mem_write_tab[39+j][i] = lo_store;
            mem_write_tab[40+j][i] = lo_store;
            mem_write_tab[41+j][i] = lo_store;
            mem_write_tab[42+j][i] = lo_store;
            mem_write_tab[43+j][i] = lo_store;
            mem_write_tab[44+j][i] = lo_store;
            mem_write_tab[45+j][i] = lo_store;
            mem_write_tab[46+j][i] = lo_store;
            mem_write_tab[47+j][i] = lo_store;
            mem_write_tab[48+j][i] = lo_store;
            mem_write_tab[49+j][i] = lo_store;
            mem_write_tab[50+j][i] = lo_store;
            mem_write_tab[51+j][i] = lo_store;
            mem_write_tab[52+j][i] = lo_store;
            mem_write_tab[53+j][i] = lo_store;
            mem_write_tab[54+j][i] = lo_store;
            mem_write_tab[55+j][i] = lo_store;
            mem_write_tab[56+j][i] = lo_store;
            mem_write_tab[57+j][i] = lo_store;
            mem_write_tab[58+j][i] = lo_store;
            mem_write_tab[59+j][i] = lo_store;
            mem_write_tab[60+j][i] = lo_store;
            mem_write_tab[61+j][i] = lo_store;
            mem_write_tab[62+j][i] = lo_store;
            mem_write_tab[63+j][i] = lo_store;
            mem_read_base_tab[0+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[1+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[2+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[3+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[4+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[5+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[6+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[7+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[8+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[9+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[10+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[11+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[12+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[13+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[14+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[15+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[16+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[17+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[18+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[19+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[20+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[21+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[22+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[23+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[24+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[25+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[26+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[27+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[28+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[29+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[30+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[31+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[32+j][i] = NULL;
            mem_read_base_tab[33+j][i] = NULL;
            mem_read_base_tab[34+j][i] = NULL;
            mem_read_base_tab[35+j][i] = NULL;
            mem_read_base_tab[36+j][i] = NULL;
            mem_read_base_tab[37+j][i] = NULL;
            mem_read_base_tab[38+j][i] = NULL;
            mem_read_base_tab[39+j][i] = NULL;
            mem_read_base_tab[40+j][i] = NULL;
            mem_read_base_tab[41+j][i] = NULL;
            mem_read_base_tab[42+j][i] = NULL;
            mem_read_base_tab[43+j][i] = NULL;
            mem_read_base_tab[44+j][i] = NULL;
            mem_read_base_tab[45+j][i] = NULL;
            mem_read_base_tab[46+j][i] = NULL;
            mem_read_base_tab[47+j][i] = NULL;
            mem_read_base_tab[48+j][i] = NULL;
            mem_read_base_tab[49+j][i] = NULL;
            mem_read_base_tab[50+j][i] = NULL;
            mem_read_base_tab[51+j][i] = NULL;
            mem_read_base_tab[52+j][i] = NULL;
            mem_read_base_tab[53+j][i] = NULL;
            mem_read_base_tab[54+j][i] = NULL;
            mem_read_base_tab[55+j][i] = NULL;
            mem_read_base_tab[56+j][i] = NULL;
            mem_read_base_tab[57+j][i] = NULL;
            mem_read_base_tab[58+j][i] = NULL;
            mem_read_base_tab[59+j][i] = NULL;
            mem_read_base_tab[60+j][i] = NULL;
            mem_read_base_tab[61+j][i] = NULL;
            mem_read_base_tab[62+j][i] = NULL;
            mem_read_base_tab[63+j][i] = NULL;
        }

        for (i = 0x40; i <= 0x7f; i++) {
            mem_read_tab[0+j][i] = ram_read;
            mem_read_tab[1+j][i] = basic_lo_read;
            mem_read_tab[2+j][i] = ram_read;
            mem_read_tab[3+j][i] = basic_lo_read;
            mem_read_tab[4+j][i] = ram_read;
            mem_read_tab[5+j][i] = basic_lo_read;
            mem_read_tab[6+j][i] = ram_read;
            mem_read_tab[7+j][i] = basic_lo_read;
            mem_read_tab[8+j][i] = ram_read;
            mem_read_tab[9+j][i] = basic_lo_read;
            mem_read_tab[10+j][i] = ram_read;
            mem_read_tab[11+j][i] = basic_lo_read;
            mem_read_tab[12+j][i] = ram_read;
            mem_read_tab[13+j][i] = basic_lo_read;
            mem_read_tab[14+j][i] = ram_read;
            mem_read_tab[15+j][i] = basic_lo_read;
            mem_read_tab[16+j][i] = ram_read;
            mem_read_tab[17+j][i] = basic_lo_read;
            mem_read_tab[18+j][i] = ram_read;
            mem_read_tab[19+j][i] = basic_lo_read;
            mem_read_tab[20+j][i] = ram_read;
            mem_read_tab[21+j][i] = basic_lo_read;
            mem_read_tab[22+j][i] = ram_read;
            mem_read_tab[23+j][i] = basic_lo_read;
            mem_read_tab[24+j][i] = ram_read;
            mem_read_tab[25+j][i] = basic_lo_read;
            mem_read_tab[26+j][i] = ram_read;
            mem_read_tab[27+j][i] = basic_lo_read;
            mem_read_tab[28+j][i] = ram_read;
            mem_read_tab[29+j][i] = basic_lo_read;
            mem_read_tab[30+j][i] = ram_read;
            mem_read_tab[31+j][i] = basic_lo_read;
            mem_read_tab[32+j][i] = ram_read;
            mem_read_tab[33+j][i] = basic_lo_read;
            mem_read_tab[34+j][i] = ram_read;
            mem_read_tab[35+j][i] = basic_lo_read;
            mem_read_tab[36+j][i] = ram_read;
            mem_read_tab[37+j][i] = basic_lo_read;
            mem_read_tab[38+j][i] = ram_read;
            mem_read_tab[39+j][i] = basic_lo_read;
            mem_read_tab[40+j][i] = ram_read;
            mem_read_tab[41+j][i] = basic_lo_read;
            mem_read_tab[42+j][i] = ram_read;
            mem_read_tab[43+j][i] = basic_lo_read;
            mem_read_tab[44+j][i] = ram_read;
            mem_read_tab[45+j][i] = basic_lo_read;
            mem_read_tab[46+j][i] = ram_read;
            mem_read_tab[47+j][i] = basic_lo_read;
            mem_read_tab[48+j][i] = ram_read;
            mem_read_tab[49+j][i] = basic_lo_read;
            mem_read_tab[50+j][i] = ram_read;
            mem_read_tab[51+j][i] = basic_lo_read;
            mem_read_tab[52+j][i] = ram_read;
            mem_read_tab[53+j][i] = basic_lo_read;
            mem_read_tab[54+j][i] = ram_read;
            mem_read_tab[55+j][i] = basic_lo_read;
            mem_read_tab[56+j][i] = ram_read;
            mem_read_tab[57+j][i] = basic_lo_read;
            mem_read_tab[58+j][i] = ram_read;
            mem_read_tab[59+j][i] = basic_lo_read;
            mem_read_tab[60+j][i] = ram_read;
            mem_read_tab[61+j][i] = basic_lo_read;
            mem_read_tab[62+j][i] = ram_read;
            mem_read_tab[63+j][i] = basic_lo_read;
            mem_write_tab[0+j][i] = ram_store;
            mem_write_tab[1+j][i] = basic_lo_store;
            mem_write_tab[2+j][i] = ram_store;
            mem_write_tab[3+j][i] = basic_lo_store;
            mem_write_tab[4+j][i] = ram_store;
            mem_write_tab[5+j][i] = basic_lo_store;
            mem_write_tab[6+j][i] = ram_store;
            mem_write_tab[7+j][i] = basic_lo_store;
            mem_write_tab[8+j][i] = ram_store;
            mem_write_tab[9+j][i] = basic_lo_store;
            mem_write_tab[10+j][i] = ram_store;
            mem_write_tab[11+j][i] = basic_lo_store;
            mem_write_tab[12+j][i] = ram_store;
            mem_write_tab[13+j][i] = basic_lo_store;
            mem_write_tab[14+j][i] = ram_store;
            mem_write_tab[15+j][i] = basic_lo_store;
            mem_write_tab[16+j][i] = ram_store;
            mem_write_tab[17+j][i] = basic_lo_store;
            mem_write_tab[18+j][i] = ram_store;
            mem_write_tab[19+j][i] = basic_lo_store;
            mem_write_tab[20+j][i] = ram_store;
            mem_write_tab[21+j][i] = basic_lo_store;
            mem_write_tab[22+j][i] = ram_store;
            mem_write_tab[23+j][i] = basic_lo_store;
            mem_write_tab[24+j][i] = ram_store;
            mem_write_tab[25+j][i] = basic_lo_store;
            mem_write_tab[26+j][i] = ram_store;
            mem_write_tab[27+j][i] = basic_lo_store;
            mem_write_tab[28+j][i] = ram_store;
            mem_write_tab[29+j][i] = basic_lo_store;
            mem_write_tab[30+j][i] = ram_store;
            mem_write_tab[31+j][i] = basic_lo_store;
            mem_write_tab[32+j][i] = ram_store;
            mem_write_tab[33+j][i] = basic_lo_store;
            mem_write_tab[34+j][i] = ram_store;
            mem_write_tab[35+j][i] = basic_lo_store;
            mem_write_tab[36+j][i] = ram_store;
            mem_write_tab[37+j][i] = basic_lo_store;
            mem_write_tab[38+j][i] = ram_store;
            mem_write_tab[39+j][i] = basic_lo_store;
            mem_write_tab[40+j][i] = ram_store;
            mem_write_tab[41+j][i] = basic_lo_store;
            mem_write_tab[42+j][i] = ram_store;
            mem_write_tab[43+j][i] = basic_lo_store;
            mem_write_tab[44+j][i] = ram_store;
            mem_write_tab[45+j][i] = basic_lo_store;
            mem_write_tab[46+j][i] = ram_store;
            mem_write_tab[47+j][i] = basic_lo_store;
            mem_write_tab[48+j][i] = ram_store;
            mem_write_tab[49+j][i] = basic_lo_store;
            mem_write_tab[50+j][i] = ram_store;
            mem_write_tab[51+j][i] = basic_lo_store;
            mem_write_tab[52+j][i] = ram_store;
            mem_write_tab[53+j][i] = basic_lo_store;
            mem_write_tab[54+j][i] = ram_store;
            mem_write_tab[55+j][i] = basic_lo_store;
            mem_write_tab[56+j][i] = ram_store;
            mem_write_tab[57+j][i] = basic_lo_store;
            mem_write_tab[58+j][i] = ram_store;
            mem_write_tab[59+j][i] = basic_lo_store;
            mem_write_tab[60+j][i] = ram_store;
            mem_write_tab[61+j][i] = basic_lo_store;
            mem_write_tab[62+j][i] = ram_store;
            mem_write_tab[63+j][i] = basic_lo_store;
            mem_read_base_tab[0+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[1+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[2+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[3+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[4+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[5+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[6+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[7+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[8+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[9+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[10+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[11+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[12+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[13+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[14+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[15+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[16+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[17+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[18+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[19+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[20+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[21+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[22+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[23+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[24+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[25+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[26+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[27+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[28+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[29+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[30+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[31+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[32+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[33+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[34+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[35+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[36+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[37+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[38+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[39+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[40+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[41+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[42+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[43+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[44+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[45+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[46+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[47+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[48+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[49+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[50+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[51+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[52+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[53+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[54+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[55+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[56+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[57+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[58+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[59+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[60+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[61+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[62+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[63+j][i] = mem_basic_rom + ((i & 0x3f) << 8);
        }

        for (i = 0x80; i <= 0xbf; i++) {
            mem_read_tab[0+j][i] = basic_hi_read;
            mem_read_tab[1+j][i] = basic_hi_read;
            mem_read_tab[2+j][i] = internal_function_rom_read;
            mem_read_tab[3+j][i] = internal_function_rom_read;
            mem_read_tab[4+j][i] = external_function_rom_read;
            mem_read_tab[5+j][i] = external_function_rom_read;
            mem_read_tab[6+j][i] = ram_read;
            mem_read_tab[7+j][i] = ram_read;
            mem_read_tab[8+j][i] = basic_hi_read;
            mem_read_tab[9+j][i] = basic_hi_read;
            mem_read_tab[10+j][i] = internal_function_rom_read;
            mem_read_tab[11+j][i] = internal_function_rom_read;
            mem_read_tab[12+j][i] = external_function_rom_read;
            mem_read_tab[13+j][i] = external_function_rom_read;
            mem_read_tab[14+j][i] = ram_read;
            mem_read_tab[15+j][i] = ram_read;
            mem_read_tab[16+j][i] = basic_hi_read;
            mem_read_tab[17+j][i] = basic_hi_read;
            mem_read_tab[18+j][i] = internal_function_rom_read;
            mem_read_tab[19+j][i] = internal_function_rom_read;
            mem_read_tab[20+j][i] = external_function_rom_read;
            mem_read_tab[21+j][i] = external_function_rom_read;
            mem_read_tab[22+j][i] = ram_read;
            mem_read_tab[23+j][i] = ram_read;
            mem_read_tab[24+j][i] = basic_hi_read;
            mem_read_tab[25+j][i] = basic_hi_read;
            mem_read_tab[26+j][i] = internal_function_rom_read;
            mem_read_tab[27+j][i] = internal_function_rom_read;
            mem_read_tab[28+j][i] = external_function_rom_read;
            mem_read_tab[29+j][i] = external_function_rom_read;
            mem_read_tab[30+j][i] = ram_read;
            mem_read_tab[31+j][i] = ram_read;
            mem_read_tab[32+j][i] = basic_hi_read;
            mem_read_tab[33+j][i] = basic_hi_read;
            mem_read_tab[34+j][i] = internal_function_rom_read;
            mem_read_tab[35+j][i] = internal_function_rom_read;
            mem_read_tab[36+j][i] = external_function_rom_read;
            mem_read_tab[37+j][i] = external_function_rom_read;
            mem_read_tab[38+j][i] = ram_read;
            mem_read_tab[39+j][i] = ram_read;
            mem_read_tab[40+j][i] = basic_hi_read;
            mem_read_tab[41+j][i] = basic_hi_read;
            mem_read_tab[42+j][i] = internal_function_rom_read;
            mem_read_tab[43+j][i] = internal_function_rom_read;
            mem_read_tab[44+j][i] = external_function_rom_read;
            mem_read_tab[45+j][i] = external_function_rom_read;
            mem_read_tab[46+j][i] = ram_read;
            mem_read_tab[47+j][i] = ram_read;
            mem_read_tab[48+j][i] = basic_hi_read;
            mem_read_tab[49+j][i] = basic_hi_read;
            mem_read_tab[50+j][i] = internal_function_rom_read;
            mem_read_tab[51+j][i] = internal_function_rom_read;
            mem_read_tab[52+j][i] = external_function_rom_read;
            mem_read_tab[53+j][i] = external_function_rom_read;
            mem_read_tab[54+j][i] = ram_read;
            mem_read_tab[55+j][i] = ram_read;
            mem_read_tab[56+j][i] = basic_hi_read;
            mem_read_tab[57+j][i] = basic_hi_read;
            mem_read_tab[58+j][i] = internal_function_rom_read;
            mem_read_tab[59+j][i] = internal_function_rom_read;
            mem_read_tab[60+j][i] = external_function_rom_read;
            mem_read_tab[61+j][i] = external_function_rom_read;
            mem_read_tab[62+j][i] = ram_read;
            mem_read_tab[63+j][i] = ram_read;
            mem_write_tab[0+j][i] = basic_hi_store;
            mem_write_tab[1+j][i] = basic_hi_store;
            mem_write_tab[2+j][i] = ram_store;
            mem_write_tab[3+j][i] = ram_store;
            mem_write_tab[4+j][i] = ram_store;
            mem_write_tab[5+j][i] = ram_store;
            mem_write_tab[6+j][i] = ram_store;
            mem_write_tab[7+j][i] = ram_store;
            mem_write_tab[8+j][i] = basic_hi_store;
            mem_write_tab[9+j][i] = basic_hi_store;
            mem_write_tab[10+j][i] = ram_store;
            mem_write_tab[11+j][i] = ram_store;
            mem_write_tab[12+j][i] = ram_store;
            mem_write_tab[13+j][i] = ram_store;
            mem_write_tab[14+j][i] = ram_store;
            mem_write_tab[15+j][i] = ram_store;
            mem_write_tab[16+j][i] = basic_hi_store;
            mem_write_tab[17+j][i] = basic_hi_store;
            mem_write_tab[18+j][i] = ram_store;
            mem_write_tab[19+j][i] = ram_store;
            mem_write_tab[20+j][i] = ram_store;
            mem_write_tab[21+j][i] = ram_store;
            mem_write_tab[22+j][i] = ram_store;
            mem_write_tab[23+j][i] = ram_store;
            mem_write_tab[24+j][i] = basic_hi_store;
            mem_write_tab[25+j][i] = basic_hi_store;
            mem_write_tab[26+j][i] = ram_store;
            mem_write_tab[27+j][i] = ram_store;
            mem_write_tab[28+j][i] = ram_store;
            mem_write_tab[29+j][i] = ram_store;
            mem_write_tab[30+j][i] = ram_store;
            mem_write_tab[31+j][i] = ram_store;
            mem_write_tab[32+j][i] = basic_hi_store;
            mem_write_tab[33+j][i] = basic_hi_store;
            mem_write_tab[34+j][i] = ram_store;
            mem_write_tab[35+j][i] = ram_store;
            mem_write_tab[36+j][i] = ram_store;
            mem_write_tab[37+j][i] = ram_store;
            mem_write_tab[38+j][i] = ram_store;
            mem_write_tab[39+j][i] = ram_store;
            mem_write_tab[40+j][i] = basic_hi_store;
            mem_write_tab[41+j][i] = basic_hi_store;
            mem_write_tab[42+j][i] = ram_store;
            mem_write_tab[43+j][i] = ram_store;
            mem_write_tab[44+j][i] = ram_store;
            mem_write_tab[45+j][i] = ram_store;
            mem_write_tab[46+j][i] = ram_store;
            mem_write_tab[47+j][i] = ram_store;
            mem_write_tab[48+j][i] = basic_hi_store;
            mem_write_tab[49+j][i] = basic_hi_store;
            mem_write_tab[50+j][i] = ram_store;
            mem_write_tab[51+j][i] = ram_store;
            mem_write_tab[52+j][i] = ram_store;
            mem_write_tab[53+j][i] = ram_store;
            mem_write_tab[54+j][i] = ram_store;
            mem_write_tab[55+j][i] = ram_store;
            mem_write_tab[56+j][i] = basic_hi_store;
            mem_write_tab[57+j][i] = basic_hi_store;
            mem_write_tab[58+j][i] = ram_store;
            mem_write_tab[59+j][i] = ram_store;
            mem_write_tab[60+j][i] = ram_store;
            mem_write_tab[61+j][i] = ram_store;
            mem_write_tab[62+j][i] = ram_store;
            mem_write_tab[63+j][i] = ram_store;
            mem_read_base_tab[0+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[1+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[2+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[3+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[4+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[5+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[6+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[7+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[8+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[9+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[10+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[11+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[12+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[13+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[14+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[15+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[16+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[17+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[18+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[19+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[20+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[21+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[22+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[23+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[24+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[25+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[26+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[27+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[28+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[29+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[30+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[31+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[32+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[33+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[34+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[35+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[36+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[37+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[38+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[39+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[40+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[41+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[42+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[43+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[44+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[45+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[46+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[47+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[48+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[49+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[50+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[51+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[52+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[53+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[54+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[55+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[56+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[57+j][i] = mem_basic_rom + 0x4000 + ((i & 0x3f) << 8);
            mem_read_base_tab[58+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[59+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[60+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[61+j][i] = ext_function_rom + ((i & 0x3f) << 8);
            mem_read_base_tab[62+j][i] = mem_ram + 0x10000 + (i << 8);
            mem_read_base_tab[63+j][i] = mem_ram + 0x10000 + (i << 8);
        }

        for (i = 0xc0; i <= 0xcf; i++) {
            mem_read_tab[0+j][i] = editor_read;
            mem_read_tab[1+j][i] = editor_read;
            mem_read_tab[2+j][i] = editor_read;
            mem_read_tab[3+j][i] = editor_read;
            mem_read_tab[4+j][i] = editor_read;
            mem_read_tab[5+j][i] = editor_read;
            mem_read_tab[6+j][i] = editor_read;
            mem_read_tab[7+j][i] = editor_read;
            mem_read_tab[8+j][i] = internal_function_rom_read;
            mem_read_tab[9+j][i] = internal_function_rom_read;
            mem_read_tab[10+j][i] = internal_function_rom_read;
            mem_read_tab[11+j][i] = internal_function_rom_read;
            mem_read_tab[12+j][i] = internal_function_rom_read;
            mem_read_tab[13+j][i] = internal_function_rom_read;
            mem_read_tab[14+j][i] = internal_function_rom_read;
            mem_read_tab[15+j][i] = internal_function_rom_read;
            mem_read_tab[16+j][i] = ram_read;
            mem_read_tab[17+j][i] = ram_read;
            mem_read_tab[18+j][i] = ram_read;
            mem_read_tab[19+j][i] = ram_read;
            mem_read_tab[20+j][i] = ram_read;
            mem_read_tab[21+j][i] = ram_read;
            mem_read_tab[22+j][i] = ram_read;
            mem_read_tab[23+j][i] = ram_read;
            mem_read_tab[24+j][i] = ram_read;
            mem_read_tab[25+j][i] = ram_read;
            mem_read_tab[26+j][i] = ram_read;
            mem_read_tab[27+j][i] = ram_read;
            mem_read_tab[28+j][i] = ram_read;
            mem_read_tab[29+j][i] = ram_read;
            mem_read_tab[30+j][i] = ram_read;
            mem_read_tab[31+j][i] = ram_read;
            mem_read_tab[32+j][i] = editor_read;
            mem_read_tab[33+j][i] = editor_read;
            mem_read_tab[34+j][i] = editor_read;
            mem_read_tab[35+j][i] = editor_read;
            mem_read_tab[36+j][i] = editor_read;
            mem_read_tab[37+j][i] = editor_read;
            mem_read_tab[38+j][i] = editor_read;
            mem_read_tab[39+j][i] = editor_read;
            mem_read_tab[40+j][i] = internal_function_rom_read;
            mem_read_tab[41+j][i] = internal_function_rom_read;
            mem_read_tab[42+j][i] = internal_function_rom_read;
            mem_read_tab[43+j][i] = internal_function_rom_read;
            mem_read_tab[44+j][i] = internal_function_rom_read;
            mem_read_tab[45+j][i] = internal_function_rom_read;
            mem_read_tab[46+j][i] = internal_function_rom_read;
            mem_read_tab[47+j][i] = internal_function_rom_read;
            mem_read_tab[48+j][i] = top_shared_read;
            mem_read_tab[49+j][i] = top_shared_read;
            mem_read_tab[50+j][i] = top_shared_read;
            mem_read_tab[51+j][i] = top_shared_read;
            mem_read_tab[52+j][i] = top_shared_read;
            mem_read_tab[53+j][i] = top_shared_read;
            mem_read_tab[54+j][i] = top_shared_read;
            mem_read_tab[55+j][i] = top_shared_read;
            mem_read_tab[56+j][i] = top_shared_read;
            mem_read_tab[57+j][i] = top_shared_read;
            mem_read_tab[58+j][i] = top_shared_read;
            mem_read_tab[59+j][i] = top_shared_read;
            mem_read_tab[60+j][i] = top_shared_read;
            mem_read_tab[61+j][i] = top_shared_read;
            mem_read_tab[62+j][i] = top_shared_read;
            mem_read_tab[63+j][i] = top_shared_read;
            mem_write_tab[0+j][i] = editor_store;
            mem_write_tab[1+j][i] = editor_store;
            mem_write_tab[2+j][i] = editor_store;
            mem_write_tab[3+j][i] = editor_store;
            mem_write_tab[4+j][i] = editor_store;
            mem_write_tab[5+j][i] = editor_store;
            mem_write_tab[6+j][i] = editor_store;
            mem_write_tab[7+j][i] = editor_store;
            mem_write_tab[8+j][i] = ram_store;
            mem_write_tab[9+j][i] = ram_store;
            mem_write_tab[10+j][i] = ram_store;
            mem_write_tab[11+j][i] = ram_store;
            mem_write_tab[12+j][i] = ram_store;
            mem_write_tab[13+j][i] = ram_store;
            mem_write_tab[14+j][i] = ram_store;
            mem_write_tab[15+j][i] = ram_store;
            mem_write_tab[16+j][i] = ram_store;
            mem_write_tab[17+j][i] = ram_store;
            mem_write_tab[18+j][i] = ram_store;
            mem_write_tab[19+j][i] = ram_store;
            mem_write_tab[20+j][i] = ram_store;
            mem_write_tab[21+j][i] = ram_store;
            mem_write_tab[22+j][i] = ram_store;
            mem_write_tab[23+j][i] = ram_store;
            mem_write_tab[24+j][i] = ram_store;
            mem_write_tab[25+j][i] = ram_store;
            mem_write_tab[26+j][i] = ram_store;
            mem_write_tab[27+j][i] = ram_store;
            mem_write_tab[28+j][i] = ram_store;
            mem_write_tab[29+j][i] = ram_store;
            mem_write_tab[30+j][i] = ram_store;
            mem_write_tab[31+j][i] = ram_store;
            mem_write_tab[32+j][i] = editor_store;
            mem_write_tab[33+j][i] = editor_store;
            mem_write_tab[34+j][i] = editor_store;
            mem_write_tab[35+j][i] = editor_store;
            mem_write_tab[36+j][i] = editor_store;
            mem_write_tab[37+j][i] = editor_store;
            mem_write_tab[38+j][i] = editor_store;
            mem_write_tab[39+j][i] = editor_store;
            mem_write_tab[40+j][i] = top_shared_store;
            mem_write_tab[41+j][i] = top_shared_store;
            mem_write_tab[42+j][i] = top_shared_store;
            mem_write_tab[43+j][i] = top_shared_store;
            mem_write_tab[44+j][i] = top_shared_store;
            mem_write_tab[45+j][i] = top_shared_store;
            mem_write_tab[46+j][i] = top_shared_store;
            mem_write_tab[47+j][i] = top_shared_store;
            mem_write_tab[48+j][i] = top_shared_store;
            mem_write_tab[49+j][i] = top_shared_store;
            mem_write_tab[50+j][i] = top_shared_store;
            mem_write_tab[51+j][i] = top_shared_store;
            mem_write_tab[52+j][i] = top_shared_store;
            mem_write_tab[53+j][i] = top_shared_store;
            mem_write_tab[54+j][i] = top_shared_store;
            mem_write_tab[55+j][i] = top_shared_store;
            mem_write_tab[56+j][i] = top_shared_store;
            mem_write_tab[57+j][i] = top_shared_store;
            mem_write_tab[58+j][i] = top_shared_store;
            mem_write_tab[59+j][i] = top_shared_store;
            mem_write_tab[60+j][i] = top_shared_store;
            mem_write_tab[61+j][i] = top_shared_store;
            mem_write_tab[62+j][i] = top_shared_store;
            mem_write_tab[63+j][i] = top_shared_store;
            mem_read_base_tab[0+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[1+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[2+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[3+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[4+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[5+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[6+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[7+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[8+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[9+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[10+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[11+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[12+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[13+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[14+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[15+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[16+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[17+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[18+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[19+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[20+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[21+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[22+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[23+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[24+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[25+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[26+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[27+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[28+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[29+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[30+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[31+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[32+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[33+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[34+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[35+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[36+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[37+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[38+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[39+j][i] = mem_basic_rom + 0x8000 + ((i & 0xf) << 8);
            mem_read_base_tab[40+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[41+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[42+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[43+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[44+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[45+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[46+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[47+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[48+j][i] = NULL;
            mem_read_base_tab[49+j][i] = NULL;
            mem_read_base_tab[50+j][i] = NULL;
            mem_read_base_tab[51+j][i] = NULL;
            mem_read_base_tab[52+j][i] = NULL;
            mem_read_base_tab[53+j][i] = NULL;
            mem_read_base_tab[54+j][i] = NULL;
            mem_read_base_tab[55+j][i] = NULL;
            mem_read_base_tab[56+j][i] = NULL;
            mem_read_base_tab[57+j][i] = NULL;
            mem_read_base_tab[58+j][i] = NULL;
            mem_read_base_tab[59+j][i] = NULL;
            mem_read_base_tab[60+j][i] = NULL;
            mem_read_base_tab[61+j][i] = NULL;
            mem_read_base_tab[62+j][i] = NULL;
            mem_read_base_tab[63+j][i] = NULL;
        }
    }

    for (i = 0xd0; i <= 0xdf; i++) {
        mem_read_tab[0][i] = read_chargen;
        mem_read_tab[1][i] = read_chargen;
        mem_read_tab[2][i] = read_chargen;
        mem_read_tab[3][i] = read_chargen;
        mem_read_tab[4][i] = read_chargen;
        mem_read_tab[5][i] = read_chargen;
        mem_read_tab[6][i] = read_chargen;
        mem_read_tab[7][i] = read_chargen;
        mem_read_tab[8][i] = internal_function_rom_read;
        mem_read_tab[9][i] = internal_function_rom_read;
        mem_read_tab[10][i] = internal_function_rom_read;
        mem_read_tab[11][i] = internal_function_rom_read;
        mem_read_tab[12][i] = internal_function_rom_read;
        mem_read_tab[13][i] = internal_function_rom_read;
        mem_read_tab[14][i] = internal_function_rom_read;
        mem_read_tab[15][i] = internal_function_rom_read;
        mem_read_tab[16][i] = ram_read;
        mem_read_tab[17][i] = ram_read;
        mem_read_tab[18][i] = ram_read;
        mem_read_tab[19][i] = ram_read;
        mem_read_tab[20][i] = ram_read;
        mem_read_tab[21][i] = ram_read;
        mem_read_tab[22][i] = ram_read;
        mem_read_tab[23][i] = ram_read;
        mem_read_tab[24][i] = ram_read;
        mem_read_tab[25][i] = ram_read;
        mem_read_tab[26][i] = ram_read;
        mem_read_tab[27][i] = ram_read;
        mem_read_tab[28][i] = ram_read;
        mem_read_tab[29][i] = ram_read;
        mem_read_tab[30][i] = ram_read;
        mem_read_tab[31][i] = ram_read;
        mem_read_tab[32][i] = read_chargen;
        mem_read_tab[33][i] = read_chargen;
        mem_read_tab[34][i] = read_chargen;
        mem_read_tab[35][i] = read_chargen;
        mem_read_tab[36][i] = read_chargen;
        mem_read_tab[37][i] = read_chargen;
        mem_read_tab[38][i] = read_chargen;
        mem_read_tab[39][i] = read_chargen;
        mem_read_tab[40][i] = internal_function_rom_read;
        mem_read_tab[41][i] = internal_function_rom_read;
        mem_read_tab[42][i] = internal_function_rom_read;
        mem_read_tab[43][i] = internal_function_rom_read;
        mem_read_tab[44][i] = internal_function_rom_read;
        mem_read_tab[45][i] = internal_function_rom_read;
        mem_read_tab[46][i] = internal_function_rom_read;
        mem_read_tab[47][i] = internal_function_rom_read;
        mem_read_tab[48][i] = top_shared_read;
        mem_read_tab[49][i] = top_shared_read;
        mem_read_tab[50][i] = top_shared_read;
        mem_read_tab[51][i] = top_shared_read;
        mem_read_tab[52][i] = top_shared_read;
        mem_read_tab[53][i] = top_shared_read;
        mem_read_tab[54][i] = top_shared_read;
        mem_read_tab[55][i] = top_shared_read;
        mem_read_tab[56][i] = top_shared_read;
        mem_read_tab[57][i] = top_shared_read;
        mem_read_tab[58][i] = top_shared_read;
        mem_read_tab[59][i] = top_shared_read;
        mem_read_tab[60][i] = top_shared_read;
        mem_read_tab[61][i] = top_shared_read;
        mem_read_tab[62][i] = top_shared_read;
        mem_read_tab[63][i] = top_shared_read;
        mem_write_tab[0][i] = hi_store;
        mem_write_tab[1][i] = hi_store;
        mem_write_tab[2][i] = hi_store;
        mem_write_tab[3][i] = hi_store;
        mem_write_tab[4][i] = hi_store;
        mem_write_tab[5][i] = hi_store;
        mem_write_tab[6][i] = hi_store;
        mem_write_tab[7][i] = hi_store;
        mem_write_tab[8][i] = ram_store;
        mem_write_tab[9][i] = ram_store;
        mem_write_tab[10][i] = ram_store;
        mem_write_tab[11][i] = ram_store;
        mem_write_tab[12][i] = ram_store;
        mem_write_tab[13][i] = ram_store;
        mem_write_tab[14][i] = ram_store;
        mem_write_tab[15][i] = ram_store;
        mem_write_tab[16][i] = ram_store;
        mem_write_tab[17][i] = ram_store;
        mem_write_tab[18][i] = ram_store;
        mem_write_tab[19][i] = ram_store;
        mem_write_tab[20][i] = ram_store;
        mem_write_tab[21][i] = ram_store;
        mem_write_tab[22][i] = ram_store;
        mem_write_tab[23][i] = ram_store;
        mem_write_tab[24][i] = ram_store;
        mem_write_tab[25][i] = ram_store;
        mem_write_tab[26][i] = ram_store;
        mem_write_tab[27][i] = ram_store;
        mem_write_tab[28][i] = ram_store;
        mem_write_tab[29][i] = ram_store;
        mem_write_tab[30][i] = ram_store;
        mem_write_tab[31][i] = ram_store;
        mem_write_tab[32][i] = hi_store;
        mem_write_tab[33][i] = hi_store;
        mem_write_tab[34][i] = hi_store;
        mem_write_tab[35][i] = hi_store;
        mem_write_tab[36][i] = hi_store;
        mem_write_tab[37][i] = hi_store;
        mem_write_tab[38][i] = hi_store;
        mem_write_tab[39][i] = hi_store;
        mem_write_tab[40][i] = top_shared_store;
        mem_write_tab[41][i] = top_shared_store;
        mem_write_tab[42][i] = top_shared_store;
        mem_write_tab[43][i] = top_shared_store;
        mem_write_tab[44][i] = top_shared_store;
        mem_write_tab[45][i] = top_shared_store;
        mem_write_tab[46][i] = top_shared_store;
        mem_write_tab[47][i] = top_shared_store;
        mem_write_tab[48][i] = top_shared_store;
        mem_write_tab[49][i] = top_shared_store;
        mem_write_tab[50][i] = top_shared_store;
        mem_write_tab[51][i] = top_shared_store;
        mem_write_tab[52][i] = top_shared_store;
        mem_write_tab[53][i] = top_shared_store;
        mem_write_tab[54][i] = top_shared_store;
        mem_write_tab[55][i] = top_shared_store;
        mem_write_tab[56][i] = top_shared_store;
        mem_write_tab[57][i] = top_shared_store;
        mem_write_tab[58][i] = top_shared_store;
        mem_write_tab[59][i] = top_shared_store;
        mem_write_tab[60][i] = top_shared_store;
        mem_write_tab[61][i] = top_shared_store;
        mem_write_tab[62][i] = top_shared_store;
        mem_write_tab[63][i] = top_shared_store;
        mem_read_base_tab[0][i] = NULL;
        mem_read_base_tab[1][i] = NULL;
        mem_read_base_tab[2][i] = NULL;
        mem_read_base_tab[3][i] = NULL;
        mem_read_base_tab[4][i] = NULL;
        mem_read_base_tab[5][i] = NULL;
        mem_read_base_tab[6][i] = NULL;
        mem_read_base_tab[7][i] = NULL;
        mem_read_base_tab[8][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[9][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[10][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[11][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[12][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[13][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[14][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[15][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[16][i] = mem_ram + (i << 8);
        mem_read_base_tab[17][i] = mem_ram + (i << 8);
        mem_read_base_tab[18][i] = mem_ram + (i << 8);
        mem_read_base_tab[19][i] = mem_ram + (i << 8);
        mem_read_base_tab[20][i] = mem_ram + (i << 8);
        mem_read_base_tab[21][i] = mem_ram + (i << 8);
        mem_read_base_tab[22][i] = mem_ram + (i << 8);
        mem_read_base_tab[23][i] = mem_ram + (i << 8);
        mem_read_base_tab[24][i] = mem_ram + (i << 8);
        mem_read_base_tab[25][i] = mem_ram + (i << 8);
        mem_read_base_tab[26][i] = mem_ram + (i << 8);
        mem_read_base_tab[27][i] = mem_ram + (i << 8);
        mem_read_base_tab[28][i] = mem_ram + (i << 8);
        mem_read_base_tab[29][i] = mem_ram + (i << 8);
        mem_read_base_tab[30][i] = mem_ram + (i << 8);
        mem_read_base_tab[31][i] = mem_ram + (i << 8);
        mem_read_base_tab[32][i] = NULL;
        mem_read_base_tab[33][i] = NULL;
        mem_read_base_tab[34][i] = NULL;
        mem_read_base_tab[35][i] = NULL;
        mem_read_base_tab[36][i] = NULL;
        mem_read_base_tab[37][i] = NULL;
        mem_read_base_tab[38][i] = NULL;
        mem_read_base_tab[39][i] = NULL;
        mem_read_base_tab[40][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[41][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[42][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[43][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[44][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[45][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[46][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[47][i] = int_function_rom + ((i & 0x7f) << 8);
        mem_read_base_tab[48][i] = NULL;
        mem_read_base_tab[49][i] = NULL;
        mem_read_base_tab[50][i] = NULL;
        mem_read_base_tab[51][i] = NULL;
        mem_read_base_tab[52][i] = NULL;
        mem_read_base_tab[53][i] = NULL;
        mem_read_base_tab[54][i] = NULL;
        mem_read_base_tab[55][i] = NULL;
        mem_read_base_tab[56][i] = NULL;
        mem_read_base_tab[57][i] = NULL;
        mem_read_base_tab[58][i] = NULL;
        mem_read_base_tab[59][i] = NULL;
        mem_read_base_tab[60][i] = NULL;
        mem_read_base_tab[61][i] = NULL;
        mem_read_base_tab[62][i] = NULL;
        mem_read_base_tab[63][i] = NULL;
    }

    for (j = 64; j < 128; j++) {
        for (i = 0xd0; i <= 0xd3; i++) {
            mem_read_tab[j][i] = vic_read;
            mem_write_tab[j][i] = vic_store;
        }

        mem_read_tab[j][0xd4] = sid_read;
        mem_write_tab[j][0xd4] = sid_store;
        mem_read_tab[j][0xd5] = mmu_read;
        mem_write_tab[j][0xd5] = mmu_store;
        mem_read_tab[j][0xd6] = vdc_read;
        mem_write_tab[j][0xd6] = vdc_store;

        mem_read_tab[j][0xd7] = d7xx_read;    /* read_empty_io; */
        mem_write_tab[j][0xd7] = d7xx_store;  /* store_empty_io; */

        mem_read_tab[j][0xd8] = mem_read_tab[j][0xd9] = colorram_read;
        mem_read_tab[j][0xda] = mem_read_tab[j][0xdb] = colorram_read;
        mem_write_tab[j][0xd8] = mem_write_tab[j][0xd9] = colorram_store;
        mem_write_tab[j][0xda] = mem_write_tab[j][0xdb] = colorram_store;

        mem_read_tab[j][0xdc] = cia1_read;
        mem_write_tab[j][0xdc] = cia1_store;
        mem_read_tab[j][0xdd] = cia2_read;
        mem_write_tab[j][0xdd] = cia2_store;

        mem_read_tab[j][0xde] = io1_read;
        mem_write_tab[j][0xde] = io1_store;

        mem_read_tab[j][0xdf] = io2_read;
        mem_write_tab[j][0xdf] = io2_store;
    }

    for (j = 0; j < 128; j += 64) {
        for (i = 0xe0; i <= 0xfe; i++) {
            mem_read_tab[0+j][i] = hi_read;
            mem_read_tab[1+j][i] = hi_read;
            mem_read_tab[2+j][i] = hi_read;
            mem_read_tab[3+j][i] = hi_read;
            mem_read_tab[4+j][i] = hi_read;
            mem_read_tab[5+j][i] = hi_read;
            mem_read_tab[6+j][i] = hi_read;
            mem_read_tab[7+j][i] = hi_read;
            mem_read_tab[8+j][i] = internal_function_rom_read;
            mem_read_tab[9+j][i] = internal_function_rom_read;
            mem_read_tab[10+j][i] = internal_function_rom_read;
            mem_read_tab[11+j][i] = internal_function_rom_read;
            mem_read_tab[12+j][i] = internal_function_rom_read;
            mem_read_tab[13+j][i] = internal_function_rom_read;
            mem_read_tab[14+j][i] = internal_function_rom_read;
            mem_read_tab[15+j][i] = internal_function_rom_read;
            mem_read_tab[16+j][i] = ram_read;
            mem_read_tab[17+j][i] = ram_read;
            mem_read_tab[18+j][i] = ram_read;
            mem_read_tab[19+j][i] = ram_read;
            mem_read_tab[20+j][i] = ram_read;
            mem_read_tab[21+j][i] = ram_read;
            mem_read_tab[22+j][i] = ram_read;
            mem_read_tab[23+j][i] = ram_read;
            mem_read_tab[24+j][i] = ram_read;
            mem_read_tab[25+j][i] = ram_read;
            mem_read_tab[26+j][i] = ram_read;
            mem_read_tab[27+j][i] = ram_read;
            mem_read_tab[28+j][i] = ram_read;
            mem_read_tab[29+j][i] = ram_read;
            mem_read_tab[30+j][i] = ram_read;
            mem_read_tab[31+j][i] = ram_read;
            mem_read_tab[32+j][i] = hi_read;
            mem_read_tab[33+j][i] = hi_read;
            mem_read_tab[34+j][i] = hi_read;
            mem_read_tab[35+j][i] = hi_read;
            mem_read_tab[36+j][i] = hi_read;
            mem_read_tab[37+j][i] = hi_read;
            mem_read_tab[38+j][i] = hi_read;
            mem_read_tab[39+j][i] = hi_read;
            mem_read_tab[40+j][i] = internal_function_rom_read;
            mem_read_tab[41+j][i] = internal_function_rom_read;
            mem_read_tab[42+j][i] = internal_function_rom_read;
            mem_read_tab[43+j][i] = internal_function_rom_read;
            mem_read_tab[44+j][i] = internal_function_rom_read;
            mem_read_tab[45+j][i] = internal_function_rom_read;
            mem_read_tab[46+j][i] = internal_function_rom_read;
            mem_read_tab[47+j][i] = internal_function_rom_read;
            mem_read_tab[48+j][i] = top_shared_read;
            mem_read_tab[49+j][i] = top_shared_read;
            mem_read_tab[50+j][i] = top_shared_read;
            mem_read_tab[51+j][i] = top_shared_read;
            mem_read_tab[52+j][i] = top_shared_read;
            mem_read_tab[53+j][i] = top_shared_read;
            mem_read_tab[54+j][i] = top_shared_read;
            mem_read_tab[55+j][i] = top_shared_read;
            mem_read_tab[56+j][i] = top_shared_read;
            mem_read_tab[57+j][i] = top_shared_read;
            mem_read_tab[58+j][i] = top_shared_read;
            mem_read_tab[59+j][i] = top_shared_read;
            mem_read_tab[60+j][i] = top_shared_read;
            mem_read_tab[61+j][i] = top_shared_read;
            mem_read_tab[62+j][i] = top_shared_read;
            mem_read_tab[63+j][i] = top_shared_read;
            mem_write_tab[0+j][i] = hi_store;
            mem_write_tab[1+j][i] = hi_store;
            mem_write_tab[2+j][i] = hi_store;
            mem_write_tab[3+j][i] = hi_store;
            mem_write_tab[4+j][i] = hi_store;
            mem_write_tab[5+j][i] = hi_store;
            mem_write_tab[6+j][i] = hi_store;
            mem_write_tab[7+j][i] = hi_store;
            mem_write_tab[8+j][i] = ram_store;
            mem_write_tab[9+j][i] = ram_store;
            mem_write_tab[10+j][i] = ram_store;
            mem_write_tab[11+j][i] = ram_store;
            mem_write_tab[12+j][i] = ram_store;
            mem_write_tab[13+j][i] = ram_store;
            mem_write_tab[14+j][i] = ram_store;
            mem_write_tab[15+j][i] = ram_store;
            mem_write_tab[16+j][i] = ram_store;
            mem_write_tab[17+j][i] = ram_store;
            mem_write_tab[18+j][i] = ram_store;
            mem_write_tab[19+j][i] = ram_store;
            mem_write_tab[20+j][i] = ram_store;
            mem_write_tab[21+j][i] = ram_store;
            mem_write_tab[22+j][i] = ram_store;
            mem_write_tab[23+j][i] = ram_store;
            mem_write_tab[24+j][i] = ram_store;
            mem_write_tab[25+j][i] = ram_store;
            mem_write_tab[26+j][i] = ram_store;
            mem_write_tab[27+j][i] = ram_store;
            mem_write_tab[28+j][i] = ram_store;
            mem_write_tab[29+j][i] = ram_store;
            mem_write_tab[30+j][i] = ram_store;
            mem_write_tab[31+j][i] = ram_store;
            mem_write_tab[32+j][i] = hi_store;
            mem_write_tab[33+j][i] = hi_store;
            mem_write_tab[34+j][i] = hi_store;
            mem_write_tab[35+j][i] = hi_store;
            mem_write_tab[36+j][i] = hi_store;
            mem_write_tab[37+j][i] = hi_store;
            mem_write_tab[38+j][i] = hi_store;
            mem_write_tab[39+j][i] = hi_store;
            mem_write_tab[40+j][i] = top_shared_store;
            mem_write_tab[41+j][i] = top_shared_store;
            mem_write_tab[42+j][i] = top_shared_store;
            mem_write_tab[43+j][i] = top_shared_store;
            mem_write_tab[44+j][i] = top_shared_store;
            mem_write_tab[45+j][i] = top_shared_store;
            mem_write_tab[46+j][i] = top_shared_store;
            mem_write_tab[47+j][i] = top_shared_store;
            mem_write_tab[48+j][i] = top_shared_store;
            mem_write_tab[49+j][i] = top_shared_store;
            mem_write_tab[50+j][i] = top_shared_store;
            mem_write_tab[51+j][i] = top_shared_store;
            mem_write_tab[52+j][i] = top_shared_store;
            mem_write_tab[53+j][i] = top_shared_store;
            mem_write_tab[54+j][i] = top_shared_store;
            mem_write_tab[55+j][i] = top_shared_store;
            mem_write_tab[56+j][i] = top_shared_store;
            mem_write_tab[57+j][i] = top_shared_store;
            mem_write_tab[58+j][i] = top_shared_store;
            mem_write_tab[59+j][i] = top_shared_store;
            mem_write_tab[60+j][i] = top_shared_store;
            mem_write_tab[61+j][i] = top_shared_store;
            mem_write_tab[62+j][i] = top_shared_store;
            mem_write_tab[63+j][i] = top_shared_store;
            mem_read_base_tab[0+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[1+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[2+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[3+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[4+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[5+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[6+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[7+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[8+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[9+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[10+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[11+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[12+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[13+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[14+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[15+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[16+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[17+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[18+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[19+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[20+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[21+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[22+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[23+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[24+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[25+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[26+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[27+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[28+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[29+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[30+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[31+j][i] = mem_ram + (i << 8);
            mem_read_base_tab[32+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[33+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[34+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[35+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[36+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[37+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[38+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[39+j][i] = mem_kernal_rom + ((i & 0x1f) << 8);
            mem_read_base_tab[40+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[41+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[42+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[43+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[44+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[45+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[46+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[47+j][i] = int_function_rom + ((i & 0x7f) << 8);
            mem_read_base_tab[48+j][i] = NULL;
            mem_read_base_tab[49+j][i] = NULL;
            mem_read_base_tab[50+j][i] = NULL;
            mem_read_base_tab[51+j][i] = NULL;
            mem_read_base_tab[52+j][i] = NULL;
            mem_read_base_tab[53+j][i] = NULL;
            mem_read_base_tab[54+j][i] = NULL;
            mem_read_base_tab[55+j][i] = NULL;
            mem_read_base_tab[56+j][i] = NULL;
            mem_read_base_tab[57+j][i] = NULL;
            mem_read_base_tab[58+j][i] = NULL;
            mem_read_base_tab[59+j][i] = NULL;
            mem_read_base_tab[60+j][i] = NULL;
            mem_read_base_tab[61+j][i] = NULL;
            mem_read_base_tab[62+j][i] = NULL;
            mem_read_base_tab[63+j][i] = NULL;
        }
    }

    for (j = 0; j < 128; j++) {
        mem_read_tab[j][0xff] = mmu_ffxx_read;
        mem_write_tab[j][0xff] = mmu_ffxx_store;

        mem_read_tab[j][0x100] = mem_read_tab[j][0x0];
        mem_write_tab[j][0x100] = mem_write_tab[j][0x0];

        mem_read_base_tab[j][0x100] = NULL;
    }


    /* C64 mode configuration.  */
    for (i = 128; i < 256; i++) {
        for (j = 2; j <= 0x100; j++) {
            mem_read_tab[i][j] = ram_read;
            mem_write_tab[i][j] = ram_store;
            mem_read_base_tab[i][j] = mem_ram + (j << 8);
        }
    }

    /* Setup BASIC ROM at $A000-$BFFF (memory configs 3, 7, 11, 15).  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab[128+3][i] = basic64_read;
        mem_read_tab[128+7][i] = basic64_read;
        mem_read_tab[128+11][i] = basic64_read;
        mem_read_tab[128+15][i] = basic64_read;
        mem_read_base_tab[128+3][i] = basic64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+7][i] = basic64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+11][i] = basic64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+15][i] = basic64_rom + ((i & 0x1f) << 8);
    }

    /* Setup character generator ROM at $D000-$DFFF (memory configs 1, 2,
       3, 9, 10, 11, 25, 26, 27).  */
    for (i = 0xd0; i <= 0xdf; i++) {
        mem_read_tab[128+1][i] = chargen64_read;
        mem_read_tab[128+2][i] = chargen64_read;
        mem_read_tab[128+3][i] = chargen64_read;
        mem_read_tab[128+9][i] = chargen64_read;
        mem_read_tab[128+10][i] = chargen64_read;
        mem_read_tab[128+11][i] = chargen64_read;
        mem_read_tab[128+25][i] = chargen64_read;
        mem_read_tab[128+26][i] = chargen64_read;
        mem_read_tab[128+27][i] = chargen64_read;
        mem_read_base_tab[128+1][i] = NULL;
        mem_read_base_tab[128+2][i] = NULL;
        mem_read_base_tab[128+3][i] = NULL;
        mem_read_base_tab[128+9][i] = NULL;
        mem_read_base_tab[128+10][i] = NULL;
        mem_read_base_tab[128+11][i] = NULL;
        mem_read_base_tab[128+25][i] = NULL;
        mem_read_base_tab[128+26][i] = NULL;
        mem_read_base_tab[128+27][i] = NULL;
    }

    /* Setup I/O at $D000-$DFFF (memory configs 5, 6, 7).  */
    for (j = 0; j < 32; j++) {
        /* IO is enabled at memory configs 5, 6, 7 and Ultimax.  */
        int io_config[32] = { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
                              1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1 };

        if (io_config[j]) {
            for (i = 0xd0; i <= 0xd3; i++) {
                mem_read_tab[128+j][i] = vic_read;
                mem_write_tab[128+j][i] = vic_store;
            }

                mem_read_tab[128+j][0xd4] = sid_read;
                mem_write_tab[128+j][0xd4] = sid_store;
                mem_read_tab[128+j][0xd5] = mmu_read;
                mem_write_tab[128+j][0xd5] = mmu_store;
                mem_read_tab[128+j][0xd6] = vdc_read;
                mem_write_tab[128+j][0xd6] = vdc_store;
                mem_read_tab[128+j][0xd7] = d7xx_read;
                mem_write_tab[128+j][0xd7] = d7xx_store;

            for (i = 0xd8; i <= 0xdb; i++) {
                mem_read_tab[128+j][i] = colorram_read;
                mem_write_tab[128+j][i] = colorram_store;
            }

            mem_read_tab[128+j][0xdc] = cia1_read;
            mem_write_tab[128+j][0xdc] = cia1_store;
            mem_read_tab[128+j][0xdd] = cia2_read;
            mem_write_tab[128+j][0xdd] = cia2_store;

            mem_read_tab[128+j][0xde] = io1_read;
            mem_write_tab[128+j][0xde] = io1_store;
            mem_read_tab[128+j][0xdf] = io2_read;
            mem_write_tab[128+j][0xdf] = io2_store;

            for (i = 0xd0; i <= 0xdf; i++)
                mem_read_base_tab[128+j][i] = NULL;
        }
    }

    /* Setup Kernal ROM at $E000-$FFFF (memory configs 2, 3, 6, 7, 10,
       11, 14, 15, 26, 27, 30, 31).  */
    for (i = 0xe0; i <= 0xff; i++) {
        mem_read_tab[128+2][i] = kernal64_read;
        mem_read_tab[128+3][i] = kernal64_read;
        mem_read_tab[128+6][i] = kernal64_read;
        mem_read_tab[128+7][i] = kernal64_read;
        mem_read_tab[128+10][i] = kernal64_read;
        mem_read_tab[128+11][i] = kernal64_read;
        mem_read_tab[128+14][i] = kernal64_read;
        mem_read_tab[128+15][i] = kernal64_read;
        mem_read_tab[128+26][i] = kernal64_read;
        mem_read_tab[128+27][i] = kernal64_read;
        mem_read_tab[128+30][i] = kernal64_read;
        mem_read_tab[128+31][i] = kernal64_read;
        mem_read_base_tab[128+2][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+3][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+6][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+7][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+10][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+11][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+14][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+15][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+26][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+27][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+30][i] = kernal64_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[128+31][i] = kernal64_rom + ((i & 0x1f) << 8);
    }

    /* Setup ROML at $8000-$9FFF.  */
    for (i = 0x80; i <= 0x9f; i++) {
        mem_read_tab[128+11][i] = roml_read;
        mem_read_base_tab[128+11][i] = NULL;
        mem_read_tab[128+15][i] = roml_read;
        mem_read_base_tab[128+15][i] = NULL;

        mem_read_tab[128+16][i] = roml_read;
        mem_write_tab[128+16][i] = roml_store;
        mem_read_base_tab[128+16][i] = NULL;
        mem_read_tab[128+17][i] = roml_read;
        mem_write_tab[128+17][i] = roml_store;
        mem_read_base_tab[128+17][i] = NULL;
        mem_read_tab[128+18][i] = roml_read;
        mem_write_tab[128+18][i] = roml_store;
        mem_read_base_tab[128+18][i] = NULL;
        mem_read_tab[128+19][i] = roml_read;
        mem_write_tab[128+19][i] = roml_store;
        mem_read_base_tab[128+19][i] = NULL;
        mem_read_tab[128+20][i] = roml_read;
        mem_write_tab[128+20][i] = roml_store;
        mem_read_base_tab[128+20][i] = NULL;
        mem_read_tab[128+21][i] = roml_read;
        mem_write_tab[128+21][i] = roml_store;
        mem_read_base_tab[128+21][i] = NULL;
        mem_read_tab[128+22][i] = roml_read;
        mem_write_tab[128+22][i] = roml_store;
        mem_read_base_tab[128+22][i] = NULL;
        mem_read_tab[128+23][i] = roml_read;
        mem_write_tab[128+23][i] = roml_store;
        mem_read_base_tab[128+23][i] = NULL;

        mem_read_tab[128+27][i] = roml_read;
        mem_read_base_tab[128+27][i] = NULL;
        mem_read_tab[128+31][i] = roml_read;
        mem_read_base_tab[128+31][i] = NULL;
    }

    /* Setup ROMH at $A000-$BFFF and $E000-$FFFF.  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab[128+16][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+16][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+16][i] = NULL;
        mem_read_tab[128+17][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+17][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+17][i] = NULL;
        mem_read_tab[128+18][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+18][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+18][i] = NULL;
        mem_read_tab[128+19][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+19][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+19][i] = NULL;
        mem_read_tab[128+20][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+20][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+20][i] = NULL;
        mem_read_tab[128+21][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+21][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+21][i] = NULL;
        mem_read_tab[128+22][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+22][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+22][i] = NULL;
        mem_read_tab[128+23][i] = ultimax_a000_bfff_read;
        mem_write_tab[128+23][i] = ultimax_a000_bfff_store;
        mem_read_base_tab[128+23][i] = NULL;

        mem_read_tab[128+26][i] = romh_read;
        mem_read_base_tab[128+26][i] = NULL;
        mem_read_tab[128+27][i] = romh_read;
        mem_read_base_tab[128+27][i] = NULL;
        mem_read_tab[128+30][i] = romh_read;
        mem_read_base_tab[128+30][i] = NULL;
        mem_read_tab[128+31][i] = romh_read;
        mem_read_base_tab[128+31][i] = NULL;
    }
    for (i = 0xe0; i <= 0xff; i++) {
        mem_read_tab[128+16][i] = romh_read;
        mem_read_base_tab[128+16][i] = NULL;
        mem_read_tab[128+17][i] = romh_read;
        mem_read_base_tab[128+17][i] = NULL;
        mem_read_tab[128+18][i] = romh_read;
        mem_read_base_tab[128+18][i] = NULL;
        mem_read_tab[128+19][i] = romh_read;
        mem_read_base_tab[128+19][i] = NULL;
        mem_read_tab[128+20][i] = romh_read;
        mem_read_base_tab[128+20][i] = NULL;
        mem_read_tab[128+21][i] = romh_read;
        mem_read_base_tab[128+21][i] = NULL;
        mem_read_tab[128+22][i] = romh_read;
        mem_read_base_tab[128+22][i] = NULL;
        mem_read_tab[128+23][i] = romh_read;
        mem_read_base_tab[128+23][i] = NULL;
    }

    for (i = 128; i < 256; i++) {
        mem_read_tab[i][0x100] = mem_read_tab[i][0];
        mem_write_tab[i][0x100] = mem_write_tab[i][0];
        mem_read_base_tab[i][0x100] = mem_read_base_tab[i][0];
    }

    mmu_reset();

    keyboard_register_caps_key(mem_toggle_caps_key);

    top_shared_limit = 0xffff;
    bottom_shared_limit = 0x0000;
    ram_bank = mem_ram;
    mem_page_zero = mem_ram;
    mem_page_one = mem_ram + 0x100;

    _mem_read_tab_ptr = mem_read_tab[3];
    _mem_write_tab_ptr = mem_write_tab[3];
    _mem_read_base_tab_ptr = mem_read_base_tab[3];
    mem_read_limit_tab_ptr = mem_read_limit_tab[3];

    pport.data = 0x37;
    pport.dir = 0x2f;
    cartridge_init_config();
}

#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    int i;

    for (i = 0; i < 0x20000; i += 0x80) {
        memset(mem_ram + i, 0x0, 0x40);
        memset(mem_ram + i + 0x40, 0xff, 0x40);
    }
}

/* ------------------------------------------------------------------------- */

/* Change the current video bank.  Call this routine only when the vbank
   has really changed.  */
void mem_set_vbank(int new_vbank)
{
    vbank = new_vbank;
    vic_ii_set_vbank(new_vbank);
}

void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[mem_config];
    }
}

/* Set the tape sense status.  */
void mem_set_tape_sense(int sense)
{
    tape_sense = sense;
    mem_pla_config_changed();
}

/* Enable/disable the REU.  FIXME: should initialize the REU if necessary?  */
void mem_toggle_reu(int flag)
{
    reu_enabled = flag;
}

/* Enable/disable the IEEE488 interface.  */
void mem_toggle_ieee488(int flag)
{
    ieee488_enabled = flag;
}

/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

/* ------------------------------------------------------------------------- */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
    if (start != NULL)
        *start = mem_ram[0x2b] | (mem_ram[0x2c] << 8);
    if (end != NULL)
        *end = mem_ram[0x1210] | (mem_ram[0x1211] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    mem_ram[0x2b] = mem_ram[0xac] = start & 0xff;
    mem_ram[0x2c] = mem_ram[0xad] = start >> 8;
    mem_ram[0x1210] = end & 0xff;
    mem_ram[0x1211] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(ADDRESS addr)
{
    return 1;
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor */

/* FIXME: peek, cartridge support */

static void store_bank_io(ADDRESS addr, BYTE byte)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        vic_store(addr, byte);
        break;
      case 0xd400:
        sid_store(addr, byte);
        break;
      case 0xd500:
        mmu_store(addr, byte);
        break;
      case 0xd600:
        vdc_store(addr, byte);
        break;
      case 0xd700:
        d7xx_store(addr, byte);
        break;
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        colorram_store(addr, byte);
        break;
      case 0xdc00:
        cia1_store(addr, byte);
        break;
      case 0xdd00:
        cia2_store(addr, byte);
        break;
      case 0xde00:
        io1_store(addr, byte);
        break;
      case 0xdf00:
        io2_store(addr, byte);
        break;
    }
    return;
}

static BYTE read_bank_io(ADDRESS addr)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        return vic_read(addr);
      case 0xd400:
        return sid_read(addr);
      case 0xd500:
        return mmu_read(addr);
      case 0xd600:
        return vdc_read(addr);
      case 0xd700:
        return d7xx_read(addr);
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return colorram_read(addr);
      case 0xdc00:
        return cia1_read(addr);
      case 0xdd00:
        return cia2_read(addr);
      case 0xde00:
        return io1_read(addr);
      case 0xdf00:
        return io2_read(addr);
    }
    return 0xff;
}

static BYTE peek_bank_io(ADDRESS addr)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        return vic_peek(addr);
      case 0xd400:
        return sid_read(addr); /* FIXME */
      case 0xd500:
        return mmu_read(addr);
      case 0xd600:
        return vdc_read(addr); /* FIXME */
      case 0xd700:
        return d7xx_read(addr); /* FIXME */
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return colorram_read(addr);
      case 0xdc00:
        return cia1_peek(addr);
      case 0xdd00:
        return cia2_peek(addr);
      case 0xde00:
        return io1_read(addr);  /* FIXME */
      case 0xdf00:
        return io2_read(addr);  /* FIXME */
    }
    return 0xff;
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "io", "ram1", "intfunc", "extfunc", "cart",
    "c64rom", NULL
};

static int banknums[] = {
    1, 0, 1, 2, 3, 4, 5, 6, 7, 8
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
      case 4:                   /* ram1 */
        return mem_ram[addr + 0x10000];
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return read_bank_io(addr);
        }
      case 2:                   /* rom */
        if (addr <= 0x0fff) {
            return bios_read(addr);
        }
        if (addr >= 0x4000 && addr <= 0xcfff) {
            return mem_basic_rom[addr - 0x4000];
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return mem_chargen_rom[addr & 0x0fff];
        }
        if (addr >= 0xe000 && addr <= 0xffff) {
            return mem_kernal_rom[addr & 0x1fff];
        }
      case 1:                   /* ram */
        break;
      case 5:
        if (addr >= 0x8000) {
            return int_function_rom[addr & 0x7fff];
        }
        break;
      case 6:
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return ext_function_rom[addr & 0x3fff];
        }
        break;
      case 7:
        if (addr >= 0x8000 && addr <= 0x9fff) {
            return roml_banks[addr & 0x1fff];
        }
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return romh_banks[addr & 0x1fff];
        }
      case 8:
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return basic64_rom[addr & 0x1fff];
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return chargen64_rom[addr & 0x0fff];
        }
        if (addr >= 0xe000 && addr <= 0xffff) {
            return kernal64_rom[addr & 0x1fff];
        }
    }
    return mem_ram[addr];
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);  /* FIXME */
        break;
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return peek_bank_io(addr);
        }
    }
    return mem_bank_read(bank, addr);
}

void mem_bank_write(int bank, ADDRESS addr, BYTE byte)
{
    switch (bank) {
      case 0:                   /* current */
        mem_store(addr, byte);
        return;
      case 4:                   /* ram1 */
        mem_ram[addr + 0x10000] = byte;
        return;
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            store_bank_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0x4000 && addr <= 0xcfff) {
            return;
        }
        if (addr >= 0xe000 && addr <= 0xffff) {
            return;
        }
      case 1:                   /* ram */
        break;
      case 5:
        if (addr >= 0x8000) {
            return;
        }
        break;
      case 6:
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        break;
      case 7:
        if (addr >= 0x8000 && addr <= 0x9fff) {
            return;
        }
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return;
        }
      case 8:
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return;
        }
        if (addr >= 0xe000 && addr <= 0xffff) {
            return;
        }
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void)
{
    mem_ioreg_list_t *mem_ioreg_list;

    mem_ioreg_list = (mem_ioreg_list_t *)xmalloc(sizeof(mem_ioreg_list_t) * 5);

    mem_ioreg_list[0].name = "VIC-IIe";
    mem_ioreg_list[0].start = 0xd000;
    mem_ioreg_list[0].end = 0xd030;
    mem_ioreg_list[0].next = &mem_ioreg_list[1];

    mem_ioreg_list[1].name = "SID";
    mem_ioreg_list[1].start = 0xd400;
    mem_ioreg_list[1].end = 0xd41f;
    mem_ioreg_list[1].next = &mem_ioreg_list[2];

    mem_ioreg_list[2].name = "MMU";
    mem_ioreg_list[2].start = 0xd500;
    mem_ioreg_list[2].end = 0xd50b;
    mem_ioreg_list[2].next = &mem_ioreg_list[3];

    mem_ioreg_list[3].name = "CIA1";
    mem_ioreg_list[3].start = 0xdc00;
    mem_ioreg_list[3].end = 0xdc0f;
    mem_ioreg_list[3].next = &mem_ioreg_list[4];

    mem_ioreg_list[4].name = "CIA2";
    mem_ioreg_list[4].start = 0xdd00;
    mem_ioreg_list[4].end = 0xdd0f;
    mem_ioreg_list[4].next = NULL;

    return mem_ioreg_list;
}

void mem_get_screen_parameter(ADDRESS *base, BYTE *rows, BYTE *columns)
{
    *base = ((vic_peek(0xd018) & 0xf0) << 6)
            | ((~cia2_peek(0xdd00) & 0x03) << 14);
    *rows = 25;
    *columns = 40;
}

/* ------------------------------------------------------------------------- */

/* FIXME: is called from c64tpi.c to enable/disable C64 IEEE488 ROM module */
void mem_set_exrom(int active)
{
}

void mem_color_ram_to_snapshot(BYTE *color_ram)
{
    unsigned int i;

    for (i = 0; i < 0x400; i++)
        color_ram[i] = (mem_color_ram[i] & 15)
                       | (mem_color_ram[i + 0x400] << 4);
}

void mem_color_ram_from_snapshot(BYTE *color_ram)
{
    unsigned int i;

    for (i = 0; i < 0x400; i++) {
        mem_color_ram[i] = color_ram[i] & 15;
        mem_color_ram[i + 0x400] = color_ram[i] >> 4;
    }
}

