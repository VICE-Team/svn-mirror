/*
 * c128mem.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

/* This is just a quick hack!  */

#include "vice.h"

#include <stdio.h>

#include "vmachine.h"
#include "true1541.h"
#include "c128mem.h"
#include "resources.h"
#include "cmdline.h"
#include "c64cia.h"
#include "sid.h"
#include "memutils.h"
#include "maincpu.h"
#include "parallel.h"
#include "tpi.h"
#include "tapeunit.h"
#include "mon.h"
#include "utils.h"
#include "vdc.h"
#include "../c64/vicii.h"

/* ------------------------------------------------------------------------- */

/* C128 memory-related resources.  */

/* Name of the character ROM.  */
static char *chargen_rom_name;

/* Name of the BASIC ROM.  */
static char *basic_rom_name;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name;

/* Kernal revision for ROM patcher.  */
static char *kernal_revision;

/* Flag: Do we enable the Emulator ID?  */
static int emu_id_enabled;

/* Flag: Do we enable the IEEE488 interface emulation?  */
static int ieee488_enabled;

/* Flag: Do we enable the external REU?  */
static int reu_enabled;

/* Flag: Do we enable Action Replay Cartridge support?  */
static int action_replay_enabled;

static void action_config_changed(BYTE mode);
static int setup_action_replay(void);

/* FIXME: Should load the new character ROM.  */
static int set_chargen_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (chargen_rom_name != NULL && name != NULL
        && strcmp(name, chargen_rom_name) == 0)
        return 0;

    string_set(&chargen_rom_name, name);
    return 0;
}

/* FIXME: Should load the new Kernal ROM.  */
static int set_kernal_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (kernal_rom_name != NULL && name != NULL
        && strcmp(name, kernal_rom_name) == 0)
        return 0;

    string_set(&kernal_rom_name, name);
    return 0;
}

/* FIXME: Should load the new BASIC ROM.  */
static int set_basic_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (basic_rom_name != NULL && name != NULL
        && strcmp(name, basic_rom_name) == 0)
        return 0;

    string_set(&basic_rom_name, name);
    return 0;
}

static int set_emu_id_enabled(resource_value_t v)
{
    if (!(int)v) {
	emu_id_enabled = 0;
	return 0;
    } else if (!action_replay_enabled) {
	emu_id_enabled = 1;
	return 0;
    } else {
	/* Other extensions share the same address space, so they cannot be
	enabled at the same time.  */
	return -1;
    }
}

static int set_ieee488_enabled(resource_value_t v)
{
    if (!(int)v) {
        ieee488_enabled = 0;
        return 0;
    } else if (!reu_enabled && !action_replay_enabled) {
        ieee488_enabled = 1;
        return 0;
    } else {
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time.  */
        return -1;
    }
}

/* FIXME: Should initialize the REU when turned on.  */
static int set_reu_enabled(resource_value_t v)
{
    if (!(int)v) {
        reu_enabled = 0;
        return 0;
    } else if (!ieee488_enabled && !action_replay_enabled) {
        reu_enabled = 1;
        return 0;
    } else {
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time.  */
        return -1;
    }
}

static resource_t resources[] = {
    { "ChargenName", RES_STRING, (resource_value_t) "chargen",
      (resource_value_t *) &chargen_rom_name, set_chargen_rom_name },
    { "KernalName", RES_STRING, (resource_value_t) "kernal",
      (resource_value_t *) &kernal_rom_name, set_kernal_rom_name },
    { "BasicName", RES_STRING, (resource_value_t) "basic",
      (resource_value_t *) &basic_rom_name, set_basic_rom_name },
    { "REU", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &reu_enabled, set_reu_enabled },
    { "IEEE488", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &ieee488_enabled, set_ieee488_enabled },
    { "EmuID", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &emu_id_enabled, set_emu_id_enabled },
    { NULL }
};

int c128_mem_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* C128 memory-related command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      "<name>", "Specify name of Kernal ROM image" },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      "<name>", "Specify name of character generator ROM image" },
    { "-reu", SET_RESOURCE, 0, NULL, NULL, "REU", (resource_value_t) 1,
      NULL, "Enable the 512K RAM expansion unit" },
    { "+reu", SET_RESOURCE, 0, NULL, NULL, "REU", (resource_value_t) 0,
      NULL, "Disable the 512K RAM expansion unit" },
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 1,
      NULL, "Enable emulator identification" },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 0,
      NULL, "Disable emulator identification" },
    { "-ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (resource_value_t) 1,
      NULL, "Enable the IEEE488 interface emulation" },
    { "+ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (resource_value_t) 0,
      NULL, "Disable the IEEE488 interface emulation" },
    { "-kernalrev", SET_RESOURCE, 1, NULL, NULL, "KernalRev", NULL,
      "<revision>", "Patch the Kernal ROM to the specified <revision>" },
    { NULL }
};

int c128_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Number of possible memory configurations.  */
#define NUM_CONFIGS	32
/* Number of possible video banks (16K each).  */
#define NUM_VBANKS	4

/* The C128 memory.  */
BYTE ram[C128_RAM_SIZE];
BYTE basic_rom[C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE];
BYTE kernal_rom[C128_KERNAL_ROM_SIZE];
BYTE chargen_rom[C128_CHARGEN_ROM_SIZE];

/* Size of RAM...  */
int ram_size = C128_RAM_SIZE;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed. */
static int tape_sense = 0;

static BYTE *page_zero, *page_one;

static int shared_size;
static int shared_lo;
static int shared_hi;

static BYTE mmu[11];

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
int rom_loaded = 0;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[0x101];
static read_func_ptr_t mem_read_tab[0x101];
static BYTE *mem_read_base_tab[0x101];

static BYTE inittab0[] = {
    0x4e, 0x67, 0x7c, 0x64, 0x6e, 0x7a, 0x62, 0x7e, 0x19, 0x2f,
    0x01, 0x4a, 0x09, 0x06, 0x53, 0x65, 0x57, 0x4e, 0x58, 0x15,
    0x3f, 0x74, 0x33, 0x3c, 0x60, 0x5d, 0x5f, 0x5e, 0x5e, 0x58,
    0x50, 0x5a, 0x06, 0x2a, 0x6c, 0x26, 0x2f, 0x7e, 0x48, 0x5e,
    0x42, 0x58, 0x5b, 0x4f, 0x4b, 0x0a
};

static BYTE biostab[] = {
    0x78, 0xa9, 0x3e, 0x8d, 0x00, 0xff, 0xa9, 0xb0, 0x8d, 0x05,
    0xd5, 0xea, 0x58, 0x60, 0x00, 0x00, 0xf3, 0x3e, 0x3e, 0x32,
    0x00, 0xff, 0x01, 0x05, 0xd5, 0x3e, 0xb1, 0xed, 0x79, 0x00,
    0xcf
};

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

/* ------------------------------------------------------------------------- */


BYTE REGPARM1 read_mmu(ADDRESS addr)
{
    addr &= 0xff;

    if ((addr & 0xff) < 0xb) {
        if (addr == 5) {
	    /* 0x80 = 40/80 key released.  */
            return mmu[5] | 0x80;
        } else {
            return mmu[addr];
	}
    } else {
        return 0xf;
    }
}

void REGPARM2 store_mmu(ADDRESS address, BYTE value)
{
    address &= 0xff;

    if (address < 0xb) {
        mmu[address] = value;

        switch (address) {
          case 0:
            kernal_rom[0x1f00] = value;
            ram[0xff00] = ram[0x1ff00] = value;
            initialize_memory();
            break;
          case 6:
            initialize_memory();
            break;
          case 5:
            value = (value & 0x7f) | 0x30;
            if ((value & 0x41) != 0x01)
                fprintf (stderr,
                         "$D505 %02X - Attempted accessing unimplemented mode.\n",
                         value);
            break;
          case 7:
          case 8:
          case 9:
          case 10:
            page_zero = (ram + (mmu[0x8] & 0x1 ? 0x10000 : 0x00000)
                         + (mmu[0x7] << 8));
            page_one = (ram + (mmu[0xa] & 0x1 ? 0x10000 : 0x00000)
                        + (mmu[0x9] << 8));
            printf ("MMU: PAGE ZERO at $%05X, PAGE ONE at $%05X\n",
                    page_zero - ram, page_one - ram);
            break;
        }
    }
    return;
}

void REGPARM1 store_zero(ADDRESS address, BYTE value)
{
    page_zero[address] = value;
}

BYTE REGPARM1 read_zero(ADDRESS address)
{
    return page_zero[address];
}

void REGPARM1 store_one(ADDRESS address, BYTE value)
{
    page_one[address & 0xff] = value;
}

BYTE REGPARM1 read_one(ADDRESS address)
{
    return page_one[address & 0xff];
}

BYTE REGPARM1 read_ram(ADDRESS address)
{
  return (ram[(address > 0xffff - shared_size && shared_hi
               ? address : (address < shared_size && shared_lo
                            ? address
                            : (((long) mmu[0] & 0x40) << 10) + address))]);
}

void REGPARM2 store_ram(ADDRESS address, BYTE value)
{
    ram[(address > 0xffff - shared_size && shared_hi
	 ? address : (address < shared_size && shared_lo
		      ? address
                      : (((long) mmu[0] & 0x40) << 10) + address))] = value;
}

void REGPARM2 store_ram_hi(ADDRESS addr, BYTE value)
{
    ram[((mmu[0] & 0x40) << 10) + addr] = value;

    if (addr > 0xff00 && addr <= 0xff04)
        store_mmu(0, mmu[addr & 15]);
    else if (addr == 0xff00)
        store_mmu(0, value);

#if 0
    if (reu_enabled && addr == 0xff00)
	reu_dma(-1);
#endif
}

BYTE REGPARM1 read_basic(ADDRESS addr)
{
    return basic_rom[addr - 0x4000];
}

void REGPARM2 store_basic(ADDRESS addr, BYTE value)
{
    basic_rom[addr - 0x4000] = value;
}

BYTE REGPARM1 read_kernal(ADDRESS addr)
{
    return kernal_rom[addr & 0x1fff];
}

void REGPARM2 store_kernal(ADDRESS addr, BYTE value)
{
    kernal_rom[addr & 0x1fff] = value;
}

BYTE REGPARM1 read_chargen(ADDRESS addr)
{
    return chargen_rom[addr & 0xffff];
}

BYTE REGPARM1 read_rom(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0xe000:
      case 0xf000:
	return read_kernal(addr);
      case 0x4000:
      case 0x5000:
      case 0x6000:
      case 0x7000:
      case 0x8000:
      case 0x9000:
      case 0xa000:
      case 0xb000:
	return read_basic(addr);
    }

    return 0;
}

void REGPARM2 store_rom(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0xe000:
      case 0xf000:
	store_kernal(addr, value);
      case 0x4000:
      case 0x5000:
      case 0x6000:
      case 0x7000:
      case 0x8000:
      case 0x9000:
      case 0xa000:
      case 0xb000:
	store_basic(addr, value);
    }
}

void REGPARM2 store_io1(ADDRESS addr, BYTE value)
{
    return;
}

BYTE REGPARM1 read_io1(ADDRESS addr)
{
    return rand();
}

void REGPARM2 store_io2(ADDRESS addr, BYTE value)
{
    return;
}

BYTE REGPARM1 read_io2(ADDRESS addr)
{
#if 0
    if (emu_id_enabled && addr >= 0xdfa0) {
	addr &= 0xff;
	if (addr == 0xff)
	    emulator_id[addr - 0xa0] ^= 0xff;
	return emulator_id[addr - 0xa0];
    }
    if ((addr & 0xff00) == 0xdf00) {
	if (reu_enabled)
	    return read_reu(addr & 0x0f);
	if (ieee488_enabled)
	    return read_tpi(addr & 0x07);
	if (action_replay_enabled) {
        if (export_ram)
            return export_ram0[0x1f00 + (addr & 0xff)];
	    switch (roml_bank) {
	      case 0:
		return roml_banks[addr & 0x1fff];
	      case 1:
		return roml_banks[(addr & 0x1fff) + 0x2000];
	      case 2:
		return roml_banks[(addr & 0x1fff) + 0x4000];
	      case 3:
		return roml_banks[(addr & 0x1fff) + 0x6000];
	    }
	}
    }
    return rand();
#else
    return rand();
#endif
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

void initialize_memory(void)
{
    int i, j;

    for (i = 0; i < 0x101; i++)
	mem_read_base_tab[i] = NULL;

    _mem_read_base_tab_ptr = mem_read_base_tab;

    shared_lo = mmu[6] & 4;
    shared_hi = mmu[6] & 8;

    if (!(mmu[6] & 0x3))
        shared_size = 0x400;
    else
        shared_size = 0x1000 << (mmu[6] & 0x3);

    mem_read_tab[0] = mem_read_tab[0x100] = read_zero;
    mem_write_tab[0] = mem_write_tab[0x100] = store_zero;
    mem_read_tab[1] = read_one;
    mem_write_tab[1] = store_one;

    for (i = 0x2; i < 0x10; i++) {
	mem_read_tab[i] = read_ram;
	mem_write_tab[i] = store_ram;
    }

    for (i = 0x10; i <= 0xff; i += 0x10) {
        mem_read_tab[i] = read_ram;
        mem_write_tab[i] = store_ram;
    }

    switch (mmu[0] & 0x30) {
      case 0x00:
        mem_read_tab[0xe0] = mem_read_tab[0xf0] = read_kernal;
        mem_read_tab[0xd0] = read_chargen;
        mem_read_tab[0xc0] = read_basic;
        break;
      case 0x10:
      case 0x20:
        /* Cartridge: FIXME */
        break;
    }

    switch (mmu[0] & 0xc) {
      case 0:
        mem_read_tab[0x80] = mem_read_tab[0x90] = read_basic;
        mem_read_tab[0xa0] = mem_read_tab[0xb0] = read_basic;
        break;
      case 4:
      case 8:
        /* Cartridge: FIXME */
        break;
    }

    if (!(mmu[0] & 2)) {
        mem_read_tab[0x40] = mem_read_tab[0x50] = read_basic;
        mem_read_tab[0x60] = mem_read_tab[0x70] = read_basic;
    }

    for (i = 0x10; i <= 0xf0; i += 0x10) {
        for (j = 1; j <= 0xf; j++) {
            mem_read_tab[i + j] = mem_read_tab[i];
            mem_write_tab[i + j] = mem_write_tab[i];
	}
    }

    if (!(mmu[0] & 1)) {
        mem_read_tab[0xd0] = mem_read_tab[0xd1] = read_vic;
        mem_read_tab[0xd2] = mem_read_tab[0xd3] = read_vic;
        mem_write_tab[0xd0] = mem_write_tab[0xd1] = store_vic;
        mem_write_tab[0xd2] = mem_write_tab[0xd3] = store_vic;
        mem_read_tab[0xd4] = read_sid;
        mem_write_tab[0xd4] = store_sid;
        mem_read_tab[0xd5] = read_mmu;
        mem_write_tab[0xd5] = store_mmu;
        mem_read_tab[0xd6] = read_vdc;
        mem_write_tab[0xd6] = store_vdc;
	mem_read_tab[0xd7] = read_io2;
	mem_write_tab[0xd7] = store_io2;
        mem_read_tab[0xd8] = mem_read_tab[0xd9] = read_colorram;
        mem_read_tab[0xda] = mem_read_tab[0xdb] = read_colorram;
        mem_write_tab[0xd8] = mem_write_tab[0xd9] = store_colorram;
        mem_write_tab[0xda] = mem_write_tab[0xdb] = store_colorram;
        mem_read_tab[0xdc] = read_cia1;
        mem_write_tab[0xdc] = store_cia1;
        mem_read_tab[0xdd] = read_cia2;
        mem_write_tab[0xdd] = store_cia2;
        mem_read_tab[0xde] = read_io1;
        mem_write_tab[0xde] = store_io1;
        mem_read_tab[0xdf] = read_io2;
        mem_write_tab[0xdf] = store_io2;
    }

    _mem_read_tab_ptr = mem_read_tab;
    _mem_write_tab_ptr = mem_write_tab;
    mem_write_tab[0xff] = store_ram_hi;
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    int i;

#ifndef __MSDOS__
    printf("Initializing RAM for power-up...\n");
#endif

    for (i = 0; i < 0x20000; i += 0x80) {
	memset(ram + i, 0, 0x40);
	memset(ram + i + 0x40, 0xff, 0x40);
    }
}

/* Load ROMs at startup.  This is half-stolen from the old `load_mem()' in
   `memory.c'.  */
int mem_load(void)
{
    WORD sum;			/* ROM checksum */
    int	id;			/* ROM identification number */
    int i;

    mem_powerup();

    page_zero = ram;
    page_one = ram + 0x100;

    initialize_memory();

    /* Load Kernal ROM. */
    if (mem_load_sys_file(kernal_rom_name,
			  kernal_rom, C128_KERNAL_ROM_SIZE,
			  C128_KERNAL_ROM_SIZE) < 0) {
	fprintf(stderr,"Couldn't load kernal ROM `%s'.\n",
                kernal_rom_name);
	return -1;
    }

    /* Check Kernal ROM.  */
    for (i = 0, sum = 0; i < C128_KERNAL_ROM_SIZE; i++)
	sum += kernal_rom[i];

    id = read_rom(0xff80);

    printf("Kernal rev #%d.\n", id);

    if (id == 1
        && sum != C128_KERNAL_CHECKSUM_R01
        && sum != C128_KERNAL_CHECKSUM_R01SWE
        && sum != C128_KERNAL_CHECKSUM_R01GER)
        fprintf (stderr, "Warning: Kernal image may be corrupted. Sum: %d\n",
		 sum);

    /* Load Basic ROM. */
    if (mem_load_sys_file(basic_rom_name,
			  basic_rom, C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE,
			  C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE) < 0) {
	fprintf(stderr, "Couldn't load basic ROM `%s'.\n",
                basic_rom_name);
	return -1;
    }

    /* Check Basic ROM. */
    for (i = 0, sum = 0; i < C128_BASIC_ROM_SIZE; i++)
	sum += basic_rom[i];

    if (sum != C128_BASIC_CHECKSUM_85 && sum != C128_BASIC_CHECKSUM_86)
	fprintf(stderr,
                "Warning: Unknown Basic image `%s'.  Sum: %d ($%04X)\n",
		basic_rom_name, sum, sum);

    /* Check Editor ROM.  */
    for (i = C128_BASIC_ROM_SIZE, sum = 0;
         i < C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE;
         i++)
        sum += basic_rom[i];

    if (id == 01
        && sum != C128_EDITOR_CHECKSUM_R01
        && sum != C128_EDITOR_CHECKSUM_R01SWE
        && sum != C128_EDITOR_CHECKSUM_R01GER) {
        fprintf (stderr, "Warning: EDITOR image may be corrupted. Sum: %d\n",
		 sum);
        fprintf (stderr, "Check your Basic ROM\n");
    }

    /* Load chargen ROM. */
    if (mem_load_sys_file(chargen_rom_name,
			  chargen_rom, C128_CHARGEN_ROM_SIZE,
			  C128_CHARGEN_ROM_SIZE) < 0) {
	fprintf(stderr, "Couldn't load character ROM `%s'.\n",
                chargen_rom_name);
	return -1;
    }

    /* Fake BIOS initialization.  This is needed because the real C128 is
       initialized by the Z80, which we currently do not implement.  */
    memcpy (ram + 0xffd0, biostab, sizeof(biostab));
    memcpy (basic_rom + 0x6eb3, inittab0, sizeof(inittab0));

    rom_loaded = 1;
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Change the current video bank. */
void mem_set_vbank(int new_vbank)
{
    if (new_vbank != vbank) {
	vbank = new_vbank;
	_mem_write_tab_ptr = mem_write_tab;
	vic_ii_set_vbank(new_vbank);
    }
}

void mem_toggle_watchpoints(int flag)
{
}

/* Set the tape sense status. */
void mem_set_tape_sense(int sense)
{
    tape_sense = sense;
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

/* FIXME: this part is wrong. */

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
