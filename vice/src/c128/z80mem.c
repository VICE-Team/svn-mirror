/*
 * z80mem.c
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

#include "c128mem.h"
#include "c128mmu.h"
#include "c64cia.h"
#include "cmdline.h"
#include "log.h"
#include "mem.h"
#include "resources.h"
#include "sid.h"
#include "sysfile.h"
#include "utils.h"
#include "vdc.h"
#include "vicii.h"
#include "z80mem.h"

/* Z80 boot BIOS.  */
static BYTE z80bios_rom[0x1000];

/* Name of the character ROM.  */
static char *z80bios_rom_name = NULL;

/* Logging.  */
static log_t z80mem_log = LOG_ERR;

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int old_reg_pc;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_z80mem_read_tab_ptr;
store_func_ptr_t *_z80mem_write_tab_ptr;
BYTE **_z80mem_read_base_tab_ptr;
int *z80mem_read_limit_tab_ptr;

#define NUM_CONFIGS 2

/* Memory read and write tables.  */
#ifdef AVOID_STATIC_ARRAYS
static store_func_ptr_t (*mem_write_tab)[NUM_CONFIGS][0x101];
static read_func_ptr_t (*mem_read_tab)[0x101];
static BYTE *(*mem_read_base_tab)[0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];
#else
static store_func_ptr_t mem_write_tab[NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];
#endif

store_func_ptr_t io_write_tab[0x101];
read_func_ptr_t io_read_tab[0x101];

#define IS_NULL(s)  (s == NULL || *s == '\0')

static int z80mem_load_bios(void)
{
    if (!IS_NULL(z80bios_rom_name)) {
        if (sysfile_load(z80bios_rom_name,
            z80bios_rom, 4096, 4096) < 0) {
            log_error(z80mem_log, "Couldn't load Z80 BIOS ROM `%s'.",
                  z80bios_rom_name);
            return -1;
        }
    }
    return 0;
}

static int set_z80bios_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (z80bios_rom_name != NULL && name != NULL
        && strcmp(name, z80bios_rom_name) == 0)
        return 0;

    string_set(&z80bios_rom_name, name);

    return z80mem_load_bios();
}

static resource_t resources[] = {
    { "Z80BiosName", RES_STRING, (resource_value_t) "z80bios",
     (resource_value_t *) &z80bios_rom_name, set_z80bios_rom_name },
    { NULL }
};

int z80mem_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] =
{
    { "-z80bios", SET_RESOURCE, 1, NULL, NULL, "Z80BiosName", NULL,
      "<name>", "Specify name of Z80 BIOS ROM image" },
    { NULL }
};

int z80mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Generic memory access.  */
static void REGPARM2 z80mem_store(ADDRESS addr, BYTE value)
{
    _z80mem_write_tab_ptr[addr >> 8](addr, value);
}

static BYTE REGPARM1 z80mem_read(ADDRESS addr)
{
    return _z80mem_read_tab_ptr[addr >> 8](addr);
}

static BYTE REGPARM1 read_ram(ADDRESS addr)
{
    return ram_bank[addr];
}

static void REGPARM2 store_ram(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}

static BYTE REGPARM1 read_bios(ADDRESS addr)
{
    return z80bios_rom[addr & 0x0fff];
}


void z80mem_initialize(void)
{
    int i, j;

    /* Memory addess space.  */

    for (j = 0; j < NUM_CONFIGS; j++) {
        for (i = 0; i <= 0x100; i++) {
            mem_read_base_tab[j][i] = NULL;
        }
    }

    for (i = 0; i < 0x10; i++) {
        mem_read_tab[0][i] = read_bios;
        mem_write_tab[0][i] = store_ram;
        mem_read_tab[1][i] = read_ram;
        mem_write_tab[1][i] = store_ram;
    }

    for (j = 0; j < NUM_CONFIGS; j++) {
        for (i = 0x10; i <= 0x100; i++) {
            mem_read_tab[j][i] = read_ram;
            mem_write_tab[j][i] = store_ram;
        }

        mem_read_tab[j][0xff] = mmu_ffxx_read;
        mem_write_tab[j][0xff] = mmu_ffxx_store;

        mem_read_tab[j][0x100] = mem_read_tab[j][0x0];
        mem_write_tab[j][0x100] = mem_write_tab[j][0x0];

        mem_read_base_tab[j][0x100] = NULL;
        mem_read_limit_tab[j][0x100] = -1;
    }

    _z80mem_read_tab_ptr = mem_read_tab[0];
    _z80mem_write_tab_ptr = mem_write_tab[0];
    _z80mem_read_base_tab_ptr = mem_read_base_tab[0];
    z80mem_read_limit_tab_ptr = mem_read_limit_tab[0];

    /* IO address space.  */

    /* At least we know what happens.  */
    for (i = 0; i <= 0x100; i++) {
        io_read_tab[i] = read_ram;
        io_write_tab[i] = store_ram;
    }

    io_read_tab[0x10] = read_colorram;
    io_write_tab[0x10] = store_colorram;
    io_read_tab[0x11] = read_colorram;
    io_write_tab[0x11] = store_colorram;
    io_read_tab[0x12] = read_colorram;
    io_write_tab[0x12] = store_colorram;
    io_read_tab[0x13] = read_colorram;
    io_write_tab[0x13] = store_colorram;

    io_read_tab[0xd0] = read_vic;
    io_write_tab[0xd0] = store_vic;
    io_read_tab[0xd1] = read_vic;
    io_write_tab[0xd1] = store_vic;
    io_read_tab[0xd2] = read_vic;
    io_write_tab[0xd2] = store_vic;
    io_read_tab[0xd3] = read_vic;
    io_write_tab[0xd3] = store_vic;

    io_read_tab[0xd4] = read_sid;
    io_write_tab[0xd4] = store_sid;

    io_read_tab[0xd5] = mmu_read;
    io_write_tab[0xd5] = mmu_store;

    io_read_tab[0xd6] = read_vdc;
    io_write_tab[0xd6] = store_vdc;
    io_read_tab[0xd7] = read_d7xx;
    io_write_tab[0xd7] = store_d7xx;

    io_read_tab[0xdc] = read_cia1;
    io_write_tab[0xdc] = store_cia1;
    io_read_tab[0xdd] = read_cia2;
    io_write_tab[0xdd] = store_cia2;
}

void z80mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

int z80mem_load(void)
{
    if (z80mem_log == LOG_ERR)
        z80mem_log = log_open("Z80MEM");

    z80mem_initialize();

/*
    rom_loaded = 1;
*/

    if (z80mem_load_bios() < 0)
        return -1;

    return 0;
}

