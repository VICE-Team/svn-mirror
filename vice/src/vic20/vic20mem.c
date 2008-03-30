/*
 * vic20mem.c -- VIC20 memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Alexander Lehmann (alex@mathematik.th-darmstadt.de)
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

#ifndef VIC20
#define	VIC20		/* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "vic20mem.h"
#include "vmachine.h"
#include "interrupt.h"
#include "resources.h"
#include "stdlib.h"
#include "memutils.h"
#include "maincpu.h"
#include "via.h"
#include "vic.h"
#include "mon.h"

/* ------------------------------------------------------------------------- */

/* The VIC20 memory. */
BYTE ram[VIC20_RAM_SIZE];
int ram_size = VIC20_RAM_SIZE;
BYTE rom[VIC20_BASIC_ROM_SIZE + VIC20_KERNAL_ROM_SIZE];
#define kernal_rom (rom + VIC20_BASIC_ROM_SIZE)
#define basic_rom (rom)
BYTE chargen_rom[VIC20_CHARGEN_ROM_SIZE];

/* Memory read and write tables. */
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t _mem_write_tab[0x101];
BYTE *_mem_read_base_tab[0x101];

/* These ones are used when watchpoints are turned on.  */
read_func_ptr_t _mem_read_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
read_func_ptr_t _mem_read_tab_nowatch[0x101];
store_func_ptr_t _mem_write_tab_nowatch[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded. */
int rom_loaded = 0;

/* VIC20 memory configuration. */
static int vic_memconf;

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_basic(ADDRESS addr)
{
    return basic_rom[addr & 0x1fff];
}

BYTE REGPARM1 read_kernal(ADDRESS addr)
{
    return kernal_rom[addr & 0x1fff];
}

BYTE REGPARM1 read_chargen(ADDRESS addr)
{
    return chargen_rom[addr & 0xfff];
}

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    return ram[addr & 0xff];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    ram[addr & 0xff] = value;
}

BYTE REGPARM1 read_ram(ADDRESS addr)
{
    return ram[addr];
}

void REGPARM2 store_ram(ADDRESS addr, BYTE value)
{
    ram[addr & (VIC20_RAM_SIZE - 1)] = value;
}

BYTE REGPARM1 read_rom(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0x8000:
	return read_chargen(addr);
      case 0xc000:
      case 0xd000:
	return read_basic(addr);
      case 0xe000:
      case 0xf000:
	return read_kernal(addr);
    }

    return 0;
}

void REGPARM2 store_rom(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0x8000:
	chargen_rom[addr & 0x0fff] = value;
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

void REGPARM2 store_via(ADDRESS addr, BYTE value)
{
    if (addr & 0x10)		/* $911x (VIA2) */
	store_via2(addr, value);
    if (addr & 0x20)		/* $912x (VIA1) */
	store_via1(addr, value);
}

BYTE REGPARM1 read_via(ADDRESS addr)
{
    BYTE ret = 0xff;

    if (addr & 0x10)		/* $911x (VIA2) */
	ret &= read_via2(addr);
    if (addr & 0x20)		/* $912x (VIA1) */
	ret &= read_via1(addr);

    return ret;
}

static BYTE REGPARM1 read_dummy(ADDRESS addr)
{
    return 0xff;
}

static void REGPARM2 store_dummy(ADDRESS addr, BYTE value)
{
    return;
}


/* Watchpoint functions */


BYTE REGPARM1 read_basic_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_basic(addr);
}

BYTE REGPARM1 read_kernal_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_kernal(addr);
}

BYTE REGPARM1 read_chargen_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_chargen(addr);
}

BYTE REGPARM1 read_ram_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_ram(addr);
}

void REGPARM2 store_ram_watch(ADDRESS addr, BYTE value)
{
    watch_push_store_addr(addr,e_comp_space);
    store_ram(addr, value);
}

void REGPARM2 store_vic_watch(ADDRESS addr, BYTE value)
{
    watch_push_store_addr(addr,e_comp_space);
    store_vic(addr, value);
}

BYTE REGPARM1 read_vic_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_vic(addr);
}

void REGPARM2 store_via_watch(ADDRESS addr, BYTE value)
{
    watch_push_store_addr(addr,e_comp_space);
    store_via(addr, value);
}

BYTE REGPARM1 read_via_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return read_via(addr);
}

static BYTE REGPARM1 read_dummy_watch(ADDRESS addr)
{
    watch_push_load_addr(addr,e_comp_space);
    return 0xff;
}

static void REGPARM2 store_dummy_watch(ADDRESS addr, BYTE value)
{
    watch_push_store_addr(addr,e_comp_space);
    return;
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

/* This function parses the mem config string given as -memory and
 * returns the appropriate values or'ed together.
 *
 * basically we accept a comma separated list of options like the following:
 * ""   - no extension
 * none - same
 * all  - all blocks
 * 3k   - 3k space in block 0 (=3k extension cartridge)
 * 8k   - 1st 8k extension block
 * 16k  - 1st and 2nd 8 extension (= 16k extension cartridge)
 * 24k  - 1-3rd extension block (=8k and 16k extension cartridges)
 *
 * 0,1,2,3,5      - memory in respective block
 * 04,20,40,60,a0 - memory at respective address (same as block #s)
 *
 * example: xvic -memory ""
 *
 *            enables unexpanded computer
 *
 *          xvic -memory 60,a0
 *
 *            enables memory in blocks 3 and 5, which is the usual
 *            configuration for 16k rom modules
 *
 * 12/27/96 Alexander Lehmann (alex@mathematik.th-darmstadt.de)
 */

#define VIC_BLK0 1
#define VIC_BLK1 2
#define VIC_BLK2 4
#define VIC_BLK3 8
#define VIC_BLK5 16
#define VIC_BLK_ALL (VIC_BLK0|VIC_BLK1|VIC_BLK2|VIC_BLK3|VIC_BLK5)

static int get_memconf(void)
{
    int memconf;
    char *memstring = app_resources.memoryExp;
    char *optend;
    char opt[100];

    /* Default is all banks. */
    if (!memstring)
	return VIC_BLK_ALL;

    memconf = 0;

    /* Maybe we should use strtok for this? */

    while (*memstring) {
	for (optend = memstring;*optend && *optend!=','; optend++)
	    ;

	strncpy(opt, memstring, optend-memstring);
	opt[optend-memstring]='\0';

	if (strcmp(opt, "") == 0 || strcmp(opt, "none") == 0) {
	    /* no extension */
	} else if (strcmp(opt, "all") == 0) {
	    memconf = VIC_BLK_ALL;
	} else if (strcmp(opt, "3k") == 0) {
	    memconf |= VIC_BLK0;
	} else if (strcmp(opt, "8k") == 0) {
	    memconf |= VIC_BLK1;
	} else if (strcmp(opt, "16k") == 0) {
	    memconf |= VIC_BLK1|VIC_BLK2;
	} else if (strcmp(opt, "24k") == 0) {
	    memconf |= VIC_BLK1|VIC_BLK2|VIC_BLK3;;
	} else if (strcmp(opt, "0") == 0 || strcmp(opt, "04") == 0) {
	    memconf |= VIC_BLK0;
	} else if (strcmp(opt, "1") == 0 || strcmp(opt, "20") == 0) {
	    memconf |= VIC_BLK1;
	} else if (strcmp(opt, "2") == 0 || strcmp(opt, "40") == 0) {
	    memconf |= VIC_BLK2;
	} else if (strcmp(opt, "3") == 0 || strcmp(opt, "60") == 0) {
	    memconf |= VIC_BLK3;
	} else if (strcmp(opt, "5") == 0 || strcmp(opt, "a0") == 0
		   || strcmp(opt, "A0") == 0) {
	    memconf |= VIC_BLK5;
	} else {
	    fprintf(stderr, "Unsupported memory extension option: \"%s\"\n",
		    opt);
	    exit(1);
	}
	memstring = optend;
	if (*memstring) memstring++; /* skip , */
    }

    if (app_resources.debugFlag) {
	printf("Extension memory enabled: ");
	if (memconf == 0) {
	    printf("none");
	} else {
	    if (memconf & VIC_BLK0) printf("blk0 ");
	    if (memconf & VIC_BLK1) printf("blk1 ");
	    if (memconf & VIC_BLK2) printf("blk2 ");
	    if (memconf & VIC_BLK3) printf("blk3 ");
	    if (memconf & VIC_BLK5) printf("blk5");
	}
	printf("\n");
    }

    return memconf;
}

static void set_mem(int start_page, int end_page,
		    read_func_ptr_t read_func,
		    store_func_ptr_t store_func,
		    read_func_ptr_t read_func_watch,
		    store_func_ptr_t store_func_watch,
		    BYTE *read_base, int base_mask)
{
    int i;

    if (read_base != NULL) {
	for (i = start_page; i <= end_page; i++) {
	    _mem_read_tab_nowatch[i] = read_func;
	    _mem_write_tab_nowatch[i] = store_func;
	    _mem_read_tab_watch[i] = read_func_watch;
	    _mem_write_tab_watch[i] = store_func_watch;
	    _mem_read_base_tab[i] = read_base + ((i << 8) & base_mask);
	}
    } else {
	for (i = start_page; i <= end_page; i++) {
	    _mem_read_tab_nowatch[i] = read_func;
	    _mem_write_tab_nowatch[i] = store_func;
	    _mem_read_tab_watch[i] = read_func_watch;
	    _mem_write_tab_watch[i] = store_func_watch;
	    _mem_read_base_tab[i] = NULL;
	}
    }
}

void initialize_memory(void)
{
    vic_memconf = get_memconf();

    /* Setup low standard RAM at $0000-$7FFF. */

    set_mem(0, 0x03,
	    read_ram, store_ram,
	    read_ram_watch, store_ram_watch,
	    ram, 0xffff);

    if (vic_memconf & VIC_BLK0)
	set_mem(0x04, 0x0f,
		read_ram, store_ram,
		read_ram_watch, store_ram_watch,
		ram, 0xffff);
    else
	set_mem(0x04, 0x0f,
		read_dummy, store_dummy,
		read_dummy_watch, store_dummy_watch,
		NULL, 0);

    set_mem(0x10, 0x1f,
	    read_ram, store_ram,
	    read_ram_watch, store_ram_watch,
	    ram, 0xffff);

    if (vic_memconf & VIC_BLK1)
	set_mem(0x20, 0x3f,
		read_ram, store_ram,
		read_ram_watch, store_ram_watch,
		ram, 0xffff);
    else
	set_mem(0x20, 0x3f,
		read_dummy, store_dummy,
		read_dummy_watch, store_dummy_watch,
		NULL, 0);

    if (vic_memconf & VIC_BLK2)
	set_mem(0x40, 0x5f,
		read_ram, store_ram,
		read_ram_watch, store_ram_watch,
		ram, 0xffff);
    else
	set_mem(0x40, 0x5f,
		read_dummy, store_dummy,
		read_dummy_watch, store_dummy_watch,
		NULL, 0);

    if (vic_memconf & VIC_BLK3)
	set_mem(0x60, 0x7f,
		read_ram, store_ram,
		read_ram_watch, store_ram_watch,
		ram, 0xffff);
    else
	set_mem(0x60, 0x7f,
		read_dummy, store_dummy,
		read_dummy_watch, store_dummy_watch,
		NULL, 0);

    /* Setup character generator ROM at $8000-$8FFF. */
    set_mem(0x80, 0x8f,
	    read_chargen, store_dummy,
	    read_chargen_watch, store_dummy_watch,
	    chargen_rom, 0x0fff);

    /* Setup VIC-I at $9000-$90FF. */
    set_mem(0x90, 0x90,
	    read_vic, store_vic,
	    read_vic_watch, store_vic_watch,
	    NULL, 0);

    /* Setup VIAs at $9100-$93FF. */
    set_mem(0x91, 0x93,
	    read_via, store_via,
	    read_via_watch, store_via_watch,
	    NULL, 0);

    /* Setup color memory at $9400-$9BFF.
       Warning: we use a kludge here.  Instead of mapping the color memory
       separately, we map it directly in the corresponding RAM address
       space. */
    set_mem(0x94, 0x9b,
	    read_ram, store_ram,
	    read_ram_watch, store_ram_watch,
	    ram, 0xffff);
    set_mem(0x9c, 0x9f,
	    read_dummy, store_dummy,
	    read_dummy_watch, store_dummy_watch,
	    NULL, 0);

    /* Setup extra RAM block at $A000-$BFFF. */
    if (vic_memconf & VIC_BLK5)
	set_mem(0xa0, 0xbf,
		read_ram, store_ram,
		read_ram_watch, store_ram_watch,
		ram, 0xffff);
    else
	set_mem(0xa0, 0xbf,
		read_dummy, store_dummy,
		read_dummy_watch, store_dummy_watch,
		NULL, 0);

    /* Setup BASIC ROM at $C000-$DFFF. */
    set_mem(0xc0, 0xdf,
	    read_basic, store_dummy,
	    read_basic_watch, store_dummy_watch,
	    basic_rom, 0x1fff);

    /* Setup Kernal ROM at $E000-$FFFF. */
    set_mem(0xe0, 0xff,
	    read_kernal, store_dummy,
	    read_kernal_watch, store_dummy_watch,
	    kernal_rom, 0x1fff);

    _mem_read_tab_nowatch[0x100] = _mem_read_tab_nowatch[0];
    _mem_write_tab_nowatch[0x100] = _mem_write_tab_nowatch[0];
    _mem_read_tab_watch[0x100] = _mem_read_tab_watch[0];
    _mem_write_tab_watch[0x100] = _mem_write_tab_watch[0];
    _mem_read_base_tab[0x100] = _mem_read_base_tab[0];

    _mem_read_base_tab_ptr = _mem_read_base_tab;

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

/* Initialize RAM for power-up. */

void mem_powerup(void)
{
    int i;

#ifndef __MSDOS__
    printf("Initializing RAM for power-up...\n");
#endif

    for (i = 0; i < VIC20_RAM_SIZE; i += 0x80) {
	memset(ram + i, 0, 0x40);
	memset(ram + i + 0x40, 0xff, 0x40);
    }
}

/* Load ROMs at startup.  This is half-stolen from the old `load_mem()' in
   `memory.c'. */

int mem_load(void)
{
    WORD sum;			/* ROM checksum */
    int i;

    /* Try to load a RAM image if available. */

    if (mem_load_sys_file(app_resources.directory, app_resources.ramName,
			  ram, VIC20_RAM_SIZE, VIC20_RAM_SIZE) < 0) {
	mem_powerup();
    }

    /* Load Kernal ROM. */

    if (mem_load_sys_file(app_resources.directory, app_resources.kernalName,
			  kernal_rom, VIC20_KERNAL_ROM_SIZE,
			  VIC20_KERNAL_ROM_SIZE) < 0) {
	fprintf(stderr,"Couldn't load kernal ROM.\n\n");
	return -1;
    }

    /* Check Kernal ROM.  */

    for (i = 0, sum = 0; i < VIC20_KERNAL_ROM_SIZE; i++)
	sum += kernal_rom[i];

    if (sum != VIC20_KERNAL_CHECKSUM){
	fprintf(stderr, "Warning: Unknown Kernal image.  Sum: %d ($%04X)\n",
		sum, sum);
    }

    /* Load Basic ROM. */

    if (mem_load_sys_file(app_resources.directory, app_resources.basicName,
			  basic_rom, VIC20_BASIC_ROM_SIZE,
			  VIC20_BASIC_ROM_SIZE) < 0) {
	fprintf(stderr, "Couldn't load basic ROM.\n\n");
	return -1;
    }

    /* Check Basic ROM. */

    for (i = 0, sum = 0; i < VIC20_BASIC_ROM_SIZE; i++)
	sum += basic_rom[i];

    if (sum != VIC20_BASIC_CHECKSUM)
	fprintf(stderr, "Warning: Unknown Basic image.  Sum: %d ($%04X)\n",
		sum, sum);

    /* Load chargen ROM. */

    if (mem_load_sys_file(app_resources.directory, app_resources.charName,
			  chargen_rom, VIC20_CHARGEN_ROM_SIZE,
			  VIC20_CHARGEN_ROM_SIZE) < 0) {
	fprintf(stderr, "Couldn't load character ROM.\n");
	return -1;
    }

    rom_loaded = 1;

    return 0;
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

/* Dummy... */
void mem_set_tape_sense(int v)
{
}
