/*
 * petmem.c - PET memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef PET
#define	PET			/* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>

#include "types.h"
#include "memutils.h"
#include "pia.h"
#include "petvia.h"
#include "crtc.h"
#include "kbd.h"
#include "resources.h"
#include "cmdline.h"
#include "pets.h"
#include "interrupt.h"
#include "vmachine.h"
#include "maincpu.h"
#include "petmem.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

/* PET-specific memory resources.  */

/* Flag: Is RAM at $9xxx enabled?  */
static int ram_9_enabled;

/* Flag: Is RAM at $Axxx enabled?  */
static int ram_a_enabled;

/* Name of character generator ROM.  */
static char *chargen_rom_name;

/* Name of Kernal ROM.  */
static char *kernal_rom_name;

/* Name of BASIC ROM.  */
static char *basic_rom_name;

/* Name of Editor ROM.  */
static char *editor_rom_name;

/* Name of ROM module at $9xxx.  */
static char *rom_module_9_name;

/* Name of ROM module at $Axxx.  */
static char *rom_module_a_name;

/* Name of ROM module at $Bxxx.  */
static char *rom_module_b_name;

static int set_ram_9_enabled(resource_value_t v)
{
    ram_9_enabled = (int) v;
    return 0;
}

static int set_ram_a_enabled(resource_value_t v)
{
    ram_a_enabled = (int) v;
    return 0;
}

static int set_chargen_rom_name(resource_value_t v)
{
    string_set(&chargen_rom_name, (char *)v);
    return 0;
}

static int set_kernal_rom_name(resource_value_t v)
{
    string_set(&kernal_rom_name, (char *)v);
    return 0;
}

static int set_basic_rom_name(resource_value_t v)
{
    string_set(&basic_rom_name, (char *)v);
    return 0;
}

static int set_editor_rom_name(resource_value_t v)
{
    string_set(&editor_rom_name, (char *)v);
    return 0;
}

static int set_rom_module_9_name(resource_value_t v)
{
    string_set(&rom_module_9_name, (char *)v);
    return 0;
}

static int set_rom_module_a_name(resource_value_t v)
{
    string_set(&rom_module_a_name, (char *)v);
    return 0;
}

static int set_rom_module_b_name(resource_value_t v)
{
    string_set(&rom_module_b_name, (char *)v);
    return 0;
}

static resource_t resources[] = {
    { "Ram9", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &ram_9_enabled, set_ram_9_enabled },
    { "RamA", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &ram_a_enabled, set_ram_a_enabled },
    { "ChargenName", RES_STRING, (resource_value_t) "chargen",
      (resource_value_t *) &chargen_rom_name, set_chargen_rom_name },
    { "KernalName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &kernal_rom_name, set_kernal_rom_name },
    { "BasicName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &basic_rom_name, set_basic_rom_name },
    { "EditorName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &editor_rom_name, set_editor_rom_name },
    { "RomModule9Name", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &rom_module_9_name, set_rom_module_9_name },
    { "RomModuleAName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &rom_module_a_name, set_rom_module_a_name },
    { "RomModuleBName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &rom_module_b_name, set_rom_module_b_name },
    { NULL }
};

int pet_mem_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* PET-specific command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      "<name>", "Specify name of Kernal ROM image" },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
    { "-editor", SET_RESOURCE, 1, NULL, NULL, "EditorName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      "<name>", "Specify name of character generator ROM image" },
    { "-rom9", SET_RESOURCE, 1, NULL, NULL, "RomModule9Name", NULL,
      "<name>", "Specify 4K extension ROM name at $9***" },
    { "-romA", SET_RESOURCE, 1, NULL, NULL, "RomModuleAName", NULL,
      "<name>", "Specify 4K extension ROM name at $A***" },
    { "-romB", SET_RESOURCE, 1, NULL, NULL, "RomModuleBName", NULL,
      "<name>", "Specify 4K extension ROM name at $B***" },
    { "-petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (resource_value_t) 1,
      NULL, "Enable PET8296 4K RAM mapping at $9***" },
    { "+petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (resource_value_t) 0,
      NULL, "Disable PET8296 4K RAM mapping at $9***" },
    { "-petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (resource_value_t) 1,
      NULL, "Enable PET8296 4K RAM mapping at $A***" },
    { "+petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (resource_value_t) 0,
      NULL, "Disable PET8296 4K RAM mapping at $A***" },
    { NULL }
};

int pet_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* The PET memory. */

#define	RAM_ARRAY 0x20000 /* this includes 8x96 expansion RAM */

BYTE ram[RAM_ARRAY]; /* 64K just to make things easier.  Real size is 32K. */
BYTE rom[PET_ROM_SIZE];
BYTE chargen_rom[PET_CHARGEN_ROM_SIZE];

int ram_size = RAM_ARRAY;	/* FIXME? */

/* Memory read and write tables. */
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t _mem_write_tab[0x101];
BYTE *_mem_read_base_tab[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;

/* Flag: nonzero if the ROM has been loaded. */
int rom_loaded = 0;

/* CRTC register pointer. */
static BYTE crtc_ptr = 0;

/* 8x96 mapping register */
static BYTE map_reg = 0;
static int bank8offset = 0;
static int bankCoffset = 0;

#define IS_NULL(s)  (s == NULL || *s == '\0')

/* ------------------------------------------------------------------------- */

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
    ram[addr] = value;
}

static BYTE REGPARM1 read_ext8(ADDRESS addr)
{
    return ram[addr + bank8offset];
}

static void REGPARM2 store_ext8(ADDRESS addr, BYTE value)
{
    ram[addr + bank8offset] = value;
}

static BYTE REGPARM1 read_extC(ADDRESS addr)
{
    return ram[addr + bankCoffset];
}

static void REGPARM2 store_extC(ADDRESS addr, BYTE value)
{
    ram[addr + bankCoffset] = value;
}

static BYTE REGPARM1 read_vmirror(ADDRESS addr)
{
    return ram[0x8000 + (addr & (pet.videoSize - 1))];
}

static void REGPARM2 store_vmirror(ADDRESS addr, BYTE value)
{
    ram[0x8000 + (addr & (pet.videoSize - 1))] = value;
}

BYTE REGPARM1 read_rom(ADDRESS addr)
{
    return rom[addr & 0x7fff];
}

void REGPARM2 store_rom(ADDRESS addr, BYTE value)
{
    rom[addr & 0x7fff] = value;
}

static BYTE REGPARM1 read_unused(ADDRESS addr)
{
    return (addr >> 8) & 0xff;
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

/*
 * The PET have all I/O chips connected to the same select lines.
 * Only one address lines is used as separate (high-active) select
 * input. I.e. PIA1 always reacts when address & 0x10 is high,
 * for example $e810, $e830, $e850, etc. PIA2 reacts when address & 0x20 is
 * high, for example $e820, $e830 $e860, ...
 * The next two functions try to reflect this behaviour.
 */
/*
 * When we write, we write all involved chips
 */
void REGPARM2 store_io(ADDRESS addr, BYTE value)
{
    if (addr & 0x10)
	store_pia1(addr, value);

    if (addr & 0x20)
	store_pia2(addr, value);

    if (addr & 0x40)
	store_via(addr, value);

    if ((addr & 0x80) && pet.crtc) {
	if (addr & 1)
	    store_crtc(crtc_ptr, value);
	else
	    crtc_ptr = value;
    }
}


/*
 * When we read, we only read sensible values. In real operation,
 * the bus drivers of all involved chips interact and you get strange
 * results...
 */
BYTE REGPARM1 read_io(ADDRESS addr)
{
    BYTE v1, v2, v3, v4;

    switch (addr & 0xf0) {
      case 0x10:		/* PIA1 */
        return read_pia1(addr);
      case 0x20:		/* PIA2 */
        return read_pia2(addr);
      case 0x40:
        return read_via(addr);	/* VIA */
      case 0x80:		/* CRTC */
        if (pet.crtc) {
            if (addr & 1)
                return read_crtc(crtc_ptr);
            else
                return 0x9f;	/* Status. */
        }
      case 0x00:
        return addr >> 8;
      default:			/* 0x30, 0x50, 0x60, 0x70, 0x90-0xf0 */
        if (addr & 0x10)
            v1 = read_pia1(addr);
        else
            v1 = 0xff;
        if (addr & 0x20)
            v2 = read_pia2(addr);
        else
            v2 = 0xff;
        if (addr & 0x40)
            v3 = read_via(addr);
        else
            v3 = 0xff;
        v4 = 0xff;
        if ((addr & 0x80) && pet.crtc) {
            if (addr & 1)
                v4 = read_crtc(crtc_ptr);
            else
                v4 = 0x9f;	/* Status. */
        }
        return v1 & v2 & v3 & v4;
    }
}

static void REGPARM2 store_dummy(ADDRESS addr, BYTE value)
{
    return;
}

/*
 * This sets the standard PET memory configuration from $9000-$10000.
 * It is used in store_8x96() and initialize_memory().
 */
static void set_std_9tof(void) {
    int i, l;
    static void (*store)(ADDRESS, BYTE);
    int ram9, rama;

    store = (pet.map == 2) ? store_ram : store_dummy;
    ram9 = (pet.map == 2 && ram_9_enabled) ? 1 : 0;
    rama = (pet.map == 2 && ram_a_enabled) ? 1 : 0;

    /* Setup RAM/ROM at $9000 - $9FFF. */
    for (i=0x90; i < 0xa0; i++) {
	_mem_read_tab[i] = ram9 ? read_ram : read_rom;
	_mem_write_tab[i] = store;
	_mem_read_base_tab[i] = ram9 ? ram + (i << 8) : rom + ((i & 0x7f) << 8);
    }

    /* Setup RAM/ROM at $9000 - $9FFF. */
    for (i=0xa0; i < 0xb0; i++) {
	_mem_read_tab[i] = rama ? read_ram : read_rom;
	_mem_write_tab[i] = store;
	_mem_read_base_tab[i] = rama ? ram + (i << 8) : rom + ((i & 0x7f) << 8);
    }

    /* Setup ROM at $B000 - $E7FF. */
    for (i = 0xb0; i <= 0xe7; i++) {
	_mem_read_tab[i] = read_rom;
	_mem_write_tab[i] = store;
	_mem_read_base_tab[i] = rom + ((i & 0x7f) << 8);
    }

    l = ((0xe800 + pet.IOSize) >> 8) & 0xff;

    /* Setup I/O at $e800 - $e800 + pet.IOSize. */
    /* i.e. IO at $e800... */
    _mem_read_tab[0xe8] = read_io;
    _mem_write_tab[0xe8] = store_io;
    _mem_read_base_tab[0xe8] = NULL;
    /* ... and unused address space behind it */
    for (i = 0xe9; i < l; i++) {
	_mem_read_tab[i] = read_unused;
	_mem_write_tab[i] = store;
	_mem_read_base_tab[i] = NULL;;
    }

    /* Setup ROM at $e800 + pet.IOSize - $ffff */
    for (i = l; i <= 0xff; i++) {
	_mem_read_tab[i] = read_rom;
	_mem_write_tab[i] = store;
	_mem_read_base_tab[i] = rom + ((i & 0x7f) << 8);
    }

    _mem_read_base_tab_ptr = _mem_read_base_tab;
}

/* FIXME: TODO! */
void mem_toggle_watchpoints(int flag)
{
}

/*
 * From the PETio.doc (ftp.funet.fi/firmware/pet/)
 *
 * $fff0 register in PET 8x96
 * 8096 exp-mem (64K):
 * The control register is at $FFF0/65520
 * You have 4 16K-banks, 0...3
 *
 * $8000     $9000               $C000           $E800    $F000     $FFFF
 * !----------------------------!!--------------------------------------!
 *         Bank 0 or 2                     Bank 1 or 3
 * !--------!                                    !-------!
 *   screen                                        io
 *
 * Control Register $FFF0:
 * bit 7:  0 normal 8032 configuration (screen, ROMs, IO, ROMs)
 *        80 expansion memory
 * bit 6:  0 RAM $E800-$EFFF (only when bit7=1)
 *        40 IO peek through
 * bit 5:  0 exp-mem $8000-$8FFF (-"-)
 *        20 screen peek through
 * bit 4: 10 not used
 * bit 3:  0 bank 1 $C000-$FFFF
 *        08 bank 3
 * bit 2:  0 bank 0 $8000-$BFFF
 *        04 bank 2
 * bit 1: 02 write protect bank 1/3
 * bit 0: 01 write protect bank 0/2
 * when bit7=0, all other bits are ignored
 *
 * The missing 32K can't be accessed witout hardware modifications.
 * You can only use the 2K "behind" the screen $8800-$8FFF (exact: 34768-
 * 36863), available in the normal configuration.
 * The register is write-only, and the value is written through to the
 * previously selected ram bank.
 */

/* save old store function for last byte */
static void REGPARM2(*store_ff) (ADDRESS addr, BYTE value) = NULL;

/* write to last page of memory in 8x96 */
static void REGPARM2 store_8x96(ADDRESS addr, BYTE value)
{
    BYTE changed;
    int l, protected;

    if (store_ff)
	store_ff(addr, value);

    changed = map_reg ^ value;

    if (addr == 0xfff0 && changed && ((map_reg | changed) & 0x80)) {
#if 0
        printf("Change $fff0 to %02x\n", value);
#endif
	if (value & 0x80) {	/* ext. RAM enabled */
	    if (changed & 0xa5) {	/* $8000-$bfff */
		protected = value & 0x01;
		l = 0x80;
		if (value & 0x20) {	/* screen memory mapped through */
		    for (; l < 0x90; l++) {
			_mem_read_tab[l] = read_ram;
			_mem_write_tab[l] = store_ram;
			_mem_read_base_tab[l] = ram + (l << 8);
		    }
		}
		bank8offset = 0x8000 + ((value & 0x04) ? 0x8000 : 0);
		for (; l < 0xc0; l++) {
		    _mem_read_tab[l] = read_ext8;
		    if (protected)
			_mem_write_tab[l] = store_dummy;
		    else
			_mem_write_tab[l] = store_ext8;
		    _mem_read_base_tab[l] = ram + bank8offset + (l << 8);
		}
	    }
	    if (changed & 0xca) {	/* $c000-$ffff */
		protected = value & 0x02;
		bankCoffset = 0x8000 + ((value & 0x08) ? 0x8000 : 0);
		for (l = 0xc0; l < 0x100; l++) {
		    if ((l == 0xe8) && (value & 0x40)) {
			_mem_read_tab[l] = read_io;
			_mem_write_tab[l] = store_io;
			_mem_read_base_tab[l] = NULL;
		    } else {
			_mem_read_tab[l] = read_extC;
			if (protected)
			    _mem_write_tab[l] = store_dummy;
			else
			    _mem_write_tab[l] = store_extC;
			_mem_read_base_tab[l] = ram + bankCoffset + (l << 8);
		    }
		}
		store_ff = _mem_write_tab[0xff];
		_mem_write_tab[0xff] = store_8x96;
	    }
	} else {		/* disable ext. RAM */
	    for (l = 0x80; l < 0x90; l++) {
		_mem_read_tab[l] = read_ram;
		_mem_write_tab[l] = store_ram;
		_mem_read_base_tab[l] = ram + (l << 8);
	    }
	    set_std_9tof();
	    store_ff = _mem_write_tab[0xff];
	    _mem_write_tab[0xff] = store_8x96;
	}
	map_reg = value;

    }
    return;
}

/* ------------------------------------------------------------------------- */
/* This does the plain 8032 configuration, as 8096 stuff only comes up
 * when writing to $fff0
 */
void initialize_memory(void)
{
    int i, l;

    l = (pet.ramSize >> 8) & 0xff;

    /* Setup RAM from $0000 to pet.ramSize */
    for (i = 0x00; i < l; i++) {
	_mem_read_tab[i] = read_ram;
	_mem_write_tab[i] = store_ram;
	_mem_read_base_tab[i] = ram + (i << 8);
    }

    /* Setup unused from pet.ramSize to $7fff */
    for (i = l; i < 0x80; i++) {
	_mem_read_tab[i] = read_unused;
	_mem_write_tab[i] = store_dummy;
	_mem_read_base_tab[i] = NULL;
    }

    l = ((0x8000 + pet.videoSize) >> 8) & 0xff;

    /* Setup RAM from $8000 to $8000 + pet.videoSize */
    for (i = 0x80; i < l; i++) {
	_mem_read_tab[i] = read_ram;
	_mem_write_tab[i] = store_ram;
	_mem_read_base_tab[i] = ram + (i << 8);
    }

    /* Setup unused from $8000 + pet.videoSize to $87ff */
    /* falls through if videoSize >= 0x800 */
    for (; i < 0x88; i++) {
	_mem_read_tab[i] = read_vmirror;
	_mem_write_tab[i] = store_vmirror;
	_mem_read_base_tab[i] = ram + 0x8000 + ((i << 8) & (pet.videoSize - 1));
    }

    /* Setup unused from $8800 to $8fff */
    /* falls through if videoSize >= 0x1000 */
    for (; i < 0x90; i++) {
	_mem_read_tab[i] = read_unused;
	_mem_write_tab[i] = store_dummy;
	_mem_read_base_tab[i] = NULL;
    }

    set_std_9tof();

    if (pet.map) {		/* catch writes to $fff0 register */
	store_ff = _mem_write_tab[0xff];
	_mem_write_tab[0xff] = store_8x96;
    }
    if (pet.pet2k) {		/* map in IEEE488 bug fixes */
	_mem_read_tab[0xef] = read_rom;
	_mem_read_base_tab[0xef] = rom + ((0xef & 0x7f) << 8);
    }
    _mem_read_tab[0x100] = _mem_read_tab[0];
    _mem_write_tab[0x100] = _mem_write_tab[0];
    _mem_read_base_tab[0x100] = _mem_read_base_tab[0];

    map_reg = 0;

    ram_size = pet.ramSize;
    _mem_read_tab_ptr = _mem_read_tab;
    _mem_write_tab_ptr = _mem_write_tab;
}

/* ------------------------------------------------------------------------- */

void patch_2001(void)
{
    int i;
    int rp;
    char dat0[] = {0xa9, 0x60, 0x85, 0xf0, 0x60};
    char dat1[] = {0x20, 0xb6, 0xf0, 0xa5, 0xf0, 0x20, 0x5b, 0xf1,
                   0x20, 0x87, 0xf1, 0x85, 0xf7,
                   0x20, 0x87, 0xf1, 0x85, 0xf8, 0x60};
    char dat2[] = {0x20, 0x7a, 0xf1, 0x20, 0xe6, 0xf6,
                   0xad, 0x0b, 0x02, 0x60};
    char dat3[] = {0xa9, 0x61, 0x85, 0xf0, 0x60};
    char dat4[] = {0x20, 0xba, 0xf0, 0xa5, 0xf0, 0x20, 0x2c, 0xf1,
                   0xa5, 0xf7, 0x20, 0x67, 0xf1,
                   0xa5, 0xf8, 0x4c, 0x67, 0xf1};
    char dat5[] = {0xae, 0x0c, 0x02, 0x70, 0x46, 0x20, 0x87, 0xf1};
    char dat6[] = {0x20, 0x2c, 0xf1, 0x4c, 0x7e, 0xf1};

    printf("PET: patching 2001 ROM to make IEEE488 work!\n");

    /* Patch PET2001 IEEE488 routines to make them work */
    rom[0x7471] = rom[0x7472] = 0xea;	/* NOP */
    rom[0x7180] = rom[0x7181] = 0xea;	/* NOP */
    rom[0x73ef] = 0xf8;
    rom[0x73f3] = 0xf7;
    rp = 0x6f00;		/* $ef00 */
    rom[0x7370] = rp & 0xff;
    rom[0x7371] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 5; i++)
	rom[rp++] = dat0[i];
    rom[0x7379] = rp & 0xff;
    rom[0x737a] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 19; i++)
	rom[rp++] = dat1[i];
    rom[0x73cc] = 0x20;
    rom[0x73cd] = rp & 0xff;
    rom[0x73ce] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 10; i++)
	rom[rp++] = dat2[i];
    for (i = 0; i < 8; i++)
	rom[0x7381 + i] = dat5[i];

    rom[0x76c1] = rp & 0xff;
    rom[0x76c2] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 5; i++)
	rom[rp++] = dat3[i];
    rom[0x76c7] = rp & 0xff;
    rom[0x76c8] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 18; i++)
	rom[rp++] = dat4[i];
    rom[0x76f4] = rp & 0xff;
    rom[0x76f5] = ((rp >> 8) & 0xff) | 0x80;
    for (i = 0; i < 6; i++)
	rom[rp++] = dat6[i];

    strcpy(rom+rp,"vice pet2001 rom patch $ef00-$efff");
}

void mem_powerup(void)
{
    int i;

#ifndef __MSDOS__
    printf("Initializing RAM for power-up...\n");
#endif
/*
    for (i = 0; i < pet.ramSize; i += 0x80) {
	memset(ram + i, 0, 0x40);
	memset(ram + i + 0x40, 0xff, 0x40);
    }
    for (i = 0x10000; i < RAM_ARRAY; i += 0x80) {
*/
    for (i = 0; i < RAM_ARRAY; i += 0x80) {
	memset(ram + i, 0, 0x40);
	memset(ram + i + 0x40, 0xff, 0x40);
    }
}

/* Load memory image files.  This also selects the PET model. */
int mem_load(void)
{
    WORD sum;			/* ROM checksum */
    int screen_width;
    int i;
    int rsize;

#if 0	/* should be done by read_* now! */
    /* init unused ram[] area to "unused address" value */
    for (i = pet.ramSize; i < 0x10000; i++) {
	ram[i] = ((i >> 8) & 0xff);
    }
#endif

    /* Load chargen ROM - we load 2k, and generate the inverted 2k. */

    if (mem_load_sys_file(chargen_rom_name, chargen_rom, 2048, 2048) < 0) {
	fprintf(stderr, "Couldn't load character ROM.\n");
	return -1;
    }

    /* copy graphics charom to second part */
    memmove(chargen_rom + 2048, chargen_rom + 1024, 1024);

    if (pet.pet2k) {
	int j;

	/* if pet2001 then exchange upper and lower case letters */
	for (i = 8; i < (0x1b * 8); i++) {
	    j = chargen_rom[0x800 + i];
	    chargen_rom[i + 0x800] = chargen_rom[i + 0xa00];
	    chargen_rom[i + 0xa00] = j;
	}
    }

    /* inverted chargen into second half. This is a PET hardware feature */
    for (i = 0; i < 1024; i++) {
	chargen_rom[i + 1024] = chargen_rom[i] ^ 0xff;
	chargen_rom[i + 3072] = chargen_rom[i + 2048] ^ 0xff;
    }

    /* Init ROM with 'unused address' values */
    for (i = 0; i < PET_ROM_SIZE; i++) {
	rom[i] = 0x80 + ((i >> 8) & 0xff);
    }

    /* Load Kernal ROM. */
    {
        const char *name = (IS_NULL(kernal_rom_name)
                            ? pet.kernalName : kernal_rom_name);

        if ((rsize = mem_load_sys_file(name,
                                       rom, pet.romSize, PET_ROM_SIZE)) < 0) {
            fprintf(stderr, "Couldn't load ROM `%s'.\n\n", name);
            return -1;
        }
    }

    /* Load extension ROMs. */

    if (!IS_NULL(basic_rom_name)
        && ((rsize = mem_load_sys_file(basic_rom_name,
                                       rom + 0x3000, 0x2000, 0x3000)) < 0)) {
	fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                basic_rom_name);
	return -1;
    }

    {
        const char *name = (IS_NULL(editor_rom_name)
                            ? pet.editorName : editor_rom_name);

        if (!IS_NULL(name)
            && ((rsize = mem_load_sys_file(name, rom + 0x6000,
                                           0x0800, 0x0800)) < 0)) {
            fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                    name);
            return -1;
        }
    }

    if (!IS_NULL(rom_module_9_name)
        && ((rsize = mem_load_sys_file(rom_module_9_name,
                                       rom + 0x1000, 0x1000, 0x1000)) < 0)) {
	fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                rom_module_9_name);
	return -1;
    }
    if (!IS_NULL(rom_module_a_name)
	&& ((rsize = mem_load_sys_file(rom_module_a_name,
                                       rom + 0x2000, 0x1000, 0x1000)) < 0)) {
	fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                rom_module_a_name);
	return -1;
    }
    if (!IS_NULL(rom_module_b_name)) {
	if (pet.romSize <= 0x4000) {
	    if ((rsize = mem_load_sys_file(rom_module_b_name, rom + 0x3000,
                                           0x1000, 0x1000)) < 0) {
		fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                        rom_module_b_name);
		return -1;
	    }
	} else {
	    printf("PET: internal ROM too large for extension ROM at $b000 - "
		   "ignoring `%s'\n", rom_module_b_name);
	}
    }

    /* Checksum over top 4 kByte PET kernal. */
    for (i = 0x7000, sum = 0; i < 0x8000; i++)
	sum += rom[i];

    /* 4032 and 8032 have the same kernals, so we have to test more, here
       $E000 - $E800. */
    for (i = 0x6000; i < 0x6800; i++)
	sum += rom[i];

    printf("PET: Loaded ROM, checksum is %d ($%04X).\n", sum, sum);

    if (pet.pet2k) {
	if (sum != PET2001_CHECKSUM) {
	    printf("PET2001 model chosen, but ROM is unknown.  Cannot patch IEEE488!\n");
	} else {
	    patch_2001();
	}
    }

    /* BIG FIXME: I had to remove `video_width' because it should not stay
       here imo.  We should find a cleaner way to implement this. -- Ettore */
    if (sum == PET8032_CHECKSUM_A || sum == PET8032_CHECKSUM_B) {
        printf("Identified PET 8032 ROM by checksum.\n");
        screen_width = 80;
    } else if (sum == PET3032_CHECKSUM) {
        printf("Identified PET 3032 ROM by checksum.\n");
        screen_width = 40;
    } else if (sum == PET4032_CHECKSUM) {
        printf("Identified PET 4032 ROM by checksum.\n");
        screen_width = 40;
    } else {
        screen_width = PET_COLS;
        printf("Unknown PET ROM.\n");
    }

    printf("Setting screen width to %d columns.\n", screen_width);
    crtc_set_screen_mode(ram + 0x8000, pet.vmask, screen_width);

    rom_loaded = 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this is plain wrong. */

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

/* Dummy... */
void mem_set_tape_sense(int v)
{
}
