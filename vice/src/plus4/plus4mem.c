/*
 * plus4mem.c -- Plus4 memory handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "log.h"
#include "mon.h"
#include "plus4mem.h"
#include "plus4memlimit.h"
#include "resources.h"
#include "snapshot.h"
#include "sysfile.h"
#include "types.h"
#include "utils.h"


/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int old_reg_pc;

const char *mem_romset_resources_list[] = {
    NULL
};

/* ------------------------------------------------------------------------- */

/* Number of possible memory configurations.  */
#define NUM_CONFIGS     2
/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      1

/* The Plus4 memory.  */
BYTE ram[PLUS4_RAM_SIZE];
BYTE basic_rom[PLUS4_BASIC_ROM_SIZE];
BYTE kernal_rom[PLUS4_KERNAL_ROM_SIZE];

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
static int rom_loaded = 0;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[NUM_VBANKS][NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];

static store_func_ptr_t mem_write_tab_orig[NUM_VBANKS][NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab_orig[NUM_CONFIGS][0x101];

/* Processor port.  */
static struct {
    BYTE dir, data, data_out;
} pport;

/* Current memory configuration.  */
static int mem_config;

/* Logging goes here.  */
static log_t plus4_mem_log = LOG_ERR;

inline void pla_config_changed(void)
{
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}


void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    mem_write_tab[0][mem_config][addr >> 8](addr, value);
}

void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[0][mem_config];
    }
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    return ram[addr & 0xff];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    addr &= 0xff;

    switch ((BYTE) addr) {
      case 0:
        if (pport.dir != value) {
            pport.dir = value;
            pla_config_changed();
        }
        break;
      case 1:
        if (pport.data != value) {
            pport.data = value;
            pla_config_changed();
        }
        break;
      default:
        ram[addr] = value;
    }
}

BYTE REGPARM1 basic_read(ADDRESS addr)
{
    return basic_rom[addr & 0x3fff];
}

BYTE REGPARM1 kernal_read(ADDRESS addr)
{
    return kernal_rom[addr & 0x3fff];
}

BYTE REGPARM1 ram_read(ADDRESS addr)
{
    return ram[addr];
}

void REGPARM2 ram_store(ADDRESS addr, BYTE value)
{
    ram[addr] = value;
}

BYTE REGPARM1 rom_read(ADDRESS addr)
{
    switch (addr & 0xc000) {
      case 0x8000:
        return basic_read(addr);
      case 0xc000:
        return kernal_read(addr);
    }

    return 0;
}

void REGPARM2 rom_store(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0x8000:
        basic_rom[addr & 0x3fff] = value;
        break;
      case 0xc000:
        kernal_rom[addr & 0x3fff] = value;
        break;
    }
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

static void set_write_hook(int config, int page, store_func_t * f)
{
    int i;

    for (i = 0; i < NUM_VBANKS; i++) {
        mem_write_tab[i][config][page] = f;
    }
}

void mem_initialize_memory(void)
{
    int i, j, k;

    mem_limit_init(mem_read_limit_tab);

    /* Default is RAM.  */
    for (i = 0; i <= 0x100; i++) {
        mem_read_tab_watch[i] = read_watch;
        mem_write_tab_watch[i] = store_watch;
    }

    for (i = 0; i < NUM_CONFIGS; i++) {
        set_write_hook(i, 0, store_zero);
        mem_read_tab[i][0] = read_zero;
        mem_read_base_tab[i][0] = ram;
        for (j = 1; j <= 0xfe; j++) {
            mem_read_tab[i][j] = ram_read;
            mem_read_base_tab[i][j] = ram + (j << 8);
            for (k = 0; k < NUM_VBANKS; k++) {
#if 0
                if ((j & 0xc0) == (k << 6)) {
                    switch (j & 0x3f) {
                      case 0x39:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_39xx_store;
                        break;
                      case 0x3f:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_3fxx_store;
                        break;
                      default:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_store;
                    }
                } else {
#endif
                    mem_write_tab[k][i][j] = ram_store;
#if 0
                }
#endif
            }
        }
        mem_read_tab[i][0xff] = ram_read;
        mem_read_base_tab[i][0xff] = ram + 0xff00;
        set_write_hook(i, 0xff, ram_store);
    }

    /* Setup BASIC ROM at $8000-$BFFF (memory config 1).  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab[1][i] = basic_read;
        mem_read_base_tab[1][i] = basic_rom + ((i & 0x3f) << 8);
    }

    /* Setup Kernal ROM at $E000-$FFFF (memory config 1).  */
    for (i = 0xc0; i <= 0xff; i++) {
        mem_read_tab[1][i] = kernal_read;
        mem_read_base_tab[1][i] = kernal_rom + ((i & 0x3f) << 8);
    }

    _mem_read_tab_ptr = mem_read_tab[1];
    _mem_write_tab_ptr = mem_write_tab[0][1];
    _mem_read_base_tab_ptr = mem_read_base_tab[1];
    mem_read_limit_tab_ptr = mem_read_limit_tab[1];
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    int i;

    for (i = 0; i < 0x10000; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }
}


int mem_load_kernal(const char *rom_name)
{
    int trapfl;

    if (!rom_loaded)
        return 0;

    /* Make sure serial code assumes there are no traps installed.  */
    /* serial_remove_traps(); */
    /* we also need the TAPE traps!!! therefore -> */
    /* disable traps before saving the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*)&trapfl);
    resources_set_value("VirtualDevices", (resource_value_t)1);

    /* Load Kernal ROM.  */
    if (sysfile_load(rom_name,
        kernal_rom, PLUS4_KERNAL_ROM_SIZE, PLUS4_KERNAL_ROM_SIZE) < 0) {
        log_error(plus4_mem_log, "Couldn't load kernal ROM `%s'.",
                  rom_name);
        resources_set_value("VirtualDevices", (resource_value_t)trapfl);
        return -1;
    }

    resources_set_value("VirtualDevices", (resource_value_t)trapfl);

    return 0;
}

int mem_load_basic(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    /* Load Basic ROM.  */
    if (sysfile_load(rom_name,
        basic_rom, PLUS4_BASIC_ROM_SIZE, PLUS4_BASIC_ROM_SIZE) < 0) {
        log_error(plus4_mem_log,
                  "Couldn't load basic ROM `%s'.",
                  rom_name);
        return -1;
    }
    return 0;
}

int mem_load(void)
{

    char *rom_name = NULL;
/*
    mem_powerup();
*/
    if (plus4_mem_log == LOG_ERR)
        plus4_mem_log = log_open("PLUS4MEM");

    rom_loaded = 1;

    if (resources_get_value("KernalName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_kernal(rom_name) < 0)
        return -1;

    if (resources_get_value("BasicName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_basic(rom_name) < 0)
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this part needs to be checked.  */

void mem_get_basic_text(ADDRESS * start, ADDRESS * end)
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
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] =
{
    "default", "cpu", "ram", "rom", "io", NULL
};

static int banknums[] =
{
    1, 0, 1, 2, 3
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
    return ram[addr];
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    return mem_bank_read(bank, addr);
}

void mem_bank_write(int bank, ADDRESS addr, BYTE byte)
{
    ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void)
{
    mem_ioreg_list_t *mem_ioreg_list;

    mem_ioreg_list = (mem_ioreg_list_t *)xmalloc(sizeof(mem_ioreg_list_t) * 1);

    mem_ioreg_list[0].name = "TED";
    mem_ioreg_list[0].start = 0xfd00;
    mem_ioreg_list[0].end = 0xfd3f;
    mem_ioreg_list[0].next = NULL;

    return mem_ioreg_list;
}

void mem_get_screen_parameter(ADDRESS *base, BYTE *rows, BYTE *columns)
{
    *base = 0x0c00;
    *rows = 25;
    *columns = 40;
}

/* ------------------------------------------------------------------------- */

/* Snapshot.  */

#define SNAP_ROM_MAJOR 0
#define SNAP_ROM_MINOR 0

#define SNAP_MAJOR 0
#define SNAP_MINOR 0
static const char snap_mem_module_name[] = "C64MEM";

int mem_write_snapshot_module(snapshot_t *s, int save_roms)
{
    return -1;
}

int mem_read_snapshot_module(snapshot_t *s)
{
    return -1;
}

