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
#define PET                     /* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>

#include "types.h"
#include "memutils.h"
#include "snapshot.h"
#include "pia.h"
#include "pet.h"
#include "petvia.h"
#include "petacia.h"
#include "crtc.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "autostart.h"
#include "resources.h"
#include "cmdline.h"
#include "tapeunit.h"
#include "pets.h"
#include "interrupt.h"
#include "vmachine.h"
#include "maincpu.h"
#include "petmem.h"
#include "emuid.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

/* The PET memory. */

#define RAM_ARRAY 0x20000 /* this includes 8x96 expansion RAM */

BYTE ram[RAM_ARRAY];      /* 128K to make things easier. Real size is 4-128K. */
BYTE rom[PET_ROM_SIZE];
BYTE chargen_rom[PET_CHARGEN_ROM_SIZE];

int ram_size = RAM_ARRAY;       /* FIXME? */

/* Memory read and write tables. */
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t _mem_write_tab[0x101];
read_func_ptr_t _mem_read_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
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

/* prototype */
void set_screen(void);

/* ------------------------------------------------------------------------- */

/* Tape traps.  */
static trap_t pet4_tape_traps[] =
{
    {
        "FindHeader",
        0xF5E8,
        0xF5EB,
        {0x20, 0x9A, 0xF8},
        findheader
    },
    {
        "WriteHeader",
        0xF66B,
        0xF66E,
        {0x20, 0xD5, 0xF8},
        writeheader
    },
    {
        "TapeReceive",
        0xF8E0,
        0xFCC0,
        {0x20, 0xE0, 0xFC},
        tapereceive
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

static trap_t pet3_tape_traps[] =
{
    {
        "FindHeader",
        0xF5A9,
        0xF5AC,
        {0x20, 0x55, 0xF8},
        findheader
    },
    {
        "WriteHeader",
        0xF62C,
        0xF62F,
        {0x20, 0x90, 0xF8},
        writeheader
    },
    {
        "TapeReceive",
        0xF89B,
        0xFC7B,
        {0x20, 0x9B, 0xFC},
        tapereceive
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

static trap_t pet2_tape_traps[] =
{
    {
        "FindHeader",
        0xF5B2,
        0xF5B5,
        {0x20, 0x7F, 0xF8},
        findheader
    },
    {
        "WriteHeader",
        0xF63D,
        0xF640,
        {0x20, 0xC4, 0xF8},
        writeheader
    },
    {
        "TapeReceive",
        0xF8A5,
        0xFCFB,
        {0x20, 0x1B, 0xFD},
        tapereceive
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the Emulator ID?  */
static int emu_id_enabled;

static int set_emu_id_enabled(resource_value_t v)
{
    emu_id_enabled = (int)v;
    return 0;
}

/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

static resource_t resources[] = {
    { "EmuID", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &emu_id_enabled, set_emu_id_enabled },
    { NULL }
};

int pet_mem_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 1,
      NULL, "Enable emulator identification" },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 0,
      NULL, "Disable emulator identification" },
    { NULL }
};

int pet_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

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

/* Functions for watchpoint memory access.  */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[addr >> 8](addr);
}

void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab[addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

/* SuperPET handling
 *
 * This adds some write-only registers at $eff*, an ACIA at $eff0 and
 * 64k RAM that are mapped in 4k pages at $9***
 * Here the 8x96 expansion RAM doubles as the SuperPET banked RAM.
 */

static int spet_ramen  = 1;
static int spet_bank   = 0;
static int spet_ctrlwp = 1;
static int spet_diag   = 0;
static int spet_ramwp  = 1;

void superpet_reset(void)
{
    spet_ramen = 1;
    spet_bank = 0;
    spet_ctrlwp = 1;
}

void superpet_powerup(void)
{
/* Those two are not reset by a soft reset (/RES), only by power down */
    spet_diag = 0;
    spet_ramwp = 1;
    superpet_reset();
}

int superpet_diag(void)
{
    return pet.superpet && spet_diag;
}

BYTE REGPARM1 read_super_io(ADDRESS addr)
{
    if(addr >= 0xeff4) {	/* unused / readonly */
        return read_unused(addr);
    } else
    if(addr >= 0xeff0) {	/* ACIA */
	return read_acia1(addr & 0x03);
    } else
    if(addr >= 0xefe4) {	/* unused */
        return read_unused(addr);
    } else
    if(addr >= 0xefe0) {	/* dongle */
    }
    return read_unused(addr);	/* fallback */
}

void REGPARM2 store_super_io(ADDRESS addr, BYTE value)
{
    if(addr >= 0xeffe) {	/* RAM/ROM switch */
	spet_ramen = !(value & 1);
    } else
    if(addr >= 0xeffc) {	/* Bank select */
	spet_bank = value & 0x0f;
	spet_ctrlwp = !(value & 0x80);
    } else
    if(addr >= 0xeff8) {
	if(!spet_ctrlwp) {
	    if(!(value & 1)) {
		printf("SuperPET: 6809 not emulated!\n");
		maincpu_trigger_reset();
	    }
	    spet_ramwp = !(value & 0x2);
	    spet_diag = (value & 0x8);
	}
    } else
    if(addr >= 0xeff4) {	/* unused */
    } else
    if(addr >= 0xeff0) {	/* ACIA */
	store_acia1(addr & 0x03, value);
    } else
    if(addr >= 0xefe4) {	/* unused */
    } else
    if(addr >= 0xefe0) {	/* dongle? */
    }
}

BYTE REGPARM1 read_super_9(ADDRESS addr)
{
    if (spet_ramen) {
	return (ram+0x10000)[(spet_bank << 12) | (addr & 0x0fff)];
    }
    return read_rom(addr);
}

void REGPARM2 store_super_9(ADDRESS addr, BYTE value)
{
    if (spet_ramen && !spet_ramwp) {
	(ram+0x10000)[(spet_bank << 12) | (addr & 0x0fff)] = value;
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

/* The PET have all I/O chips connected to the same select lines.  Only one
   address lines is used as separate (high-active) select input.  I.e. PIA1
   always reacts when address & 0x10 is high, for example $e810, $e830,
   $e850, etc.  PIA2 reacts when address & 0x20 is high, for example $e820,
   $e830 $e860,...  The next two functions try to reflect this behaviour.  */

/* When we write, we write all involved chips.  */

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

    if (emu_id_enabled && addr >= 0xE8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }

    switch (addr & 0xf0) {
      case 0x10:                /* PIA1 */
        return read_pia1(addr);
      case 0x20:                /* PIA2 */
        return read_pia2(addr);
      case 0x40:
        return read_via(addr);  /* VIA */
      case 0x80:                /* CRTC */
        if (pet.crtc) {
            if (addr & 1)
                return read_crtc(crtc_ptr);
            else
                return 0x9f;    /* Status. */
        }
      case 0x00:
        return addr >> 8;
      default:                  /* 0x30, 0x50, 0x60, 0x70, 0x90-0xf0 */
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
                v4 = 0x9f;      /* Status. */
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
static void set_std_9tof(void)
{
    int i, l;
    static void (*store)(ADDRESS, BYTE);
    int ram9, rama;

    store = (pet.map == 2) ? store_ram : store_dummy;
    ram9 = (pet.map == 2 && pet.mem9) ? 1 : 0;
    rama = (pet.map == 2 && pet.memA) ? 1 : 0;

    /* Setup RAM/ROM at $9000 - $9FFF. */
    if(pet.superpet) {
        for (i = 0x90; i < 0xa0; i++) {
            _mem_read_tab[i] = read_super_9;
            _mem_write_tab[i] = store_super_9;
            _mem_read_base_tab[i] = NULL;
        }
    } else {
        for (i = 0x90; i < 0xa0; i++) {
            _mem_read_tab[i] = ram9 ? read_ram : read_rom;
            _mem_write_tab[i] = store;
            _mem_read_base_tab[i] = ram9 ? ram + (i << 8) : rom + ((i & 0x7f) << 8);
        }
    }

    /* Setup RAM/ROM at $A000 - $AFFF. */
    for (i = 0xa0; i < 0xb0; i++) {
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

    if(pet.superpet) {
        _mem_read_tab[0xef] = read_super_io;
        _mem_write_tab[0xef] = store_super_io;
        _mem_read_base_tab[0xef] = NULL;;
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
    if (flag) {
        _mem_read_tab_ptr = _mem_read_tab_watch;
        _mem_write_tab_ptr = _mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = _mem_read_tab;
        _mem_write_tab_ptr = _mem_write_tab;
    }
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
 *
 */

/* Save old store function for last byte.  */
static void REGPARM2 (*store_ff)(ADDRESS addr, BYTE value) = NULL;

/* Write to last page of memory in 8x96.  */
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
        if (value & 0x80) {     /* ext. RAM enabled */
            if (changed & 0xa5) {       /* $8000-$bfff */
                protected = value & 0x01;
                l = 0x80;
                if (value & 0x20) {     /* screen memory mapped through */
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
            if (changed & 0xca) {       /* $c000-$ffff */
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
        } else {                /* disable ext. RAM */
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

/* This does the plain 8032 configuration, as 8096 stuff only comes up when
   writing to $fff0.  */
void initialize_memory(void)
{
    int i, l;

    l = pet.ramSize << 2;       /* ramSize in kB, l in 256 Byte */
    if (l > 128)
        l = 128;                /* fix 8096 / 8296 */

    /*printf("PET: initialize memory, ramSize=%04x -> l=%d\n", pet.ramSize * 1024,l); */

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

    if (pet.map) {              /* catch writes to $fff0 register */
        store_ff = _mem_write_tab[0xff];
        _mem_write_tab[0xff] = store_8x96;
    }
    if (pet.pet2k) {            /* map in IEEE488 bug fixes */
        _mem_read_tab[0xef] = read_rom;
        _mem_read_base_tab[0xef] = rom + ((0xef & 0x7f) << 8);
    }
    _mem_read_tab[0x100] = _mem_read_tab[0];
    _mem_write_tab[0x100] = _mem_write_tab[0];
    _mem_read_base_tab[0x100] = _mem_read_base_tab[0];

    map_reg = 0;

    ram_size = pet.ramSize * 1024;
    _mem_read_tab_ptr = _mem_read_tab;
    _mem_write_tab_ptr = _mem_write_tab;

    for (i = 0; i < 0x101; i++) {
        _mem_read_tab_watch[i] = read_watch;
        _mem_write_tab_watch[i] = store_watch;
    }
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
    rom[0x7471] = rom[0x7472] = 0xea;   /* NOP */
    rom[0x7180] = rom[0x7181] = 0xea;   /* NOP */
    rom[0x73ef] = 0xf8;
    rom[0x73f3] = 0xf7;
    rp = 0x6f00;                /* $ef00 */
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

    strcpy(rom + rp, "vice pet2001 rom patch $ef00-$efff");
}

void mem_powerup(void)
{
    int i;

#ifndef __MSDOS__
    printf("Initializing RAM for power-up...\n");
#endif
    for (i = 0; i < RAM_ARRAY; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }

    superpet_powerup();
}

/* Load memory image files.  This also selects the PET model.  */
int mem_load(void)
{
    WORD sum;                   /* ROM checksum */
    int i;
    int rsize, krsize;

    /* De-initialize kbd-buf, autostart and tape stuff here before
       reloading the ROM the traps are installed in.  */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_init(0, 0, 0, 0, 0, 0, 0, NULL, 0, 0);

    /* Load chargen ROM - we load 2k, and generate the inverted 2k.  */

    if (mem_load_sys_file(pet.chargenName, chargen_rom, 2048, 2048) < 0) {
        fprintf(stderr, "Couldn't load character ROM.\n");
        return -1;
    }

    /* Copy graphics charom to second part.  */
    memmove(chargen_rom + 2048, chargen_rom + 1024, 1024);

    if (pet.pet2k) {
        int j;

        /* If pet2001 then exchange upper and lower case letters.  */
        for (i = 8; i < (0x1b * 8); i++) {
            j = chargen_rom[0x800 + i];
            chargen_rom[i + 0x800] = chargen_rom[i + 0xa00];
            chargen_rom[i + 0xa00] = j;
        }
    }

    /* Inverted chargen into second half. This is a PET hardware feature.  */
    for (i = 0; i < 1024; i++) {
        chargen_rom[i + 1024] = chargen_rom[i] ^ 0xff;
        chargen_rom[i + 3072] = chargen_rom[i + 2048] ^ 0xff;
    }

    /* Init ROM with 'unused address' values.  */
    for (i = 0; i < PET_ROM_SIZE; i++) {
        rom[i] = 0x80 + ((i >> 8) & 0xff);
    }

    /* Load Kernal ROM.  */
    {
        const char *name =      /* (IS_NULL(kernal_rom_name)
           ? */ pet.kernalName /*: kernal_rom_name) */ ;

        if ((krsize = mem_load_sys_file(name,
                                        rom, 0x2000, PET_ROM_SIZE)) < 0) {
            fprintf(stderr, "Couldn't load ROM `%s'.\n\n", name);
            return -1;
        }
    }

    /* Load extension ROMs.  */
#if 0
    if (!IS_NULL(basic_rom_name)
        && ((rsize = mem_load_sys_file(basic_rom_name,
                                   rom + 0x3000, 0x2000, 0x3000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                basic_rom_name);
        return -1;
    }
#endif

    {
        const char *name = pet.editorName;

        if (!IS_NULL(name)
            && ((rsize = mem_load_sys_file(name, rom + 0x6000,
                                           0x0800, 0x0800)) < 0)) {
            fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                    name);
            return -1;
        }
    }

    if (!IS_NULL(pet.mem9name)
        && ((rsize = mem_load_sys_file(pet.mem9name,
                                   rom + 0x1000, 0x1000, 0x1000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                pet.mem9name);
        return -1;
    }
    if (!IS_NULL(pet.memAname)
        && ((rsize = mem_load_sys_file(pet.memAname,
                                   rom + 0x2000, 0x1000, 0x1000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                pet.memAname);
        return -1;
    }
    if (!IS_NULL(pet.memBname)) {
        if (krsize <= 0x4000) {
            if ((rsize = mem_load_sys_file(pet.memBname, rom + 0x3000,
                                           0x1000, 0x1000)) < 0) {
                fprintf(stderr, "Couldn't load ROM `%s'.\n\n",
                        pet.memBname);
                return -1;
            }
        } else {
            printf("PET: internal ROM too large for extension ROM at $b000 - "
                   "ignoring `%s'\n", pet.memBname);
        }
    }

    /* Checksum over top 4 kByte PET kernal.  */
    for (i = 0x7000, sum = 0; i < 0x8000; i++)
        sum += rom[i];

    /* 4032 and 8032 have the same kernals, so we have to test more, here
       $E000 - $E800.  */
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
    pet.screen_width = 0;
    /* The length of the keyboard buffer might actually differ from 10 - in
       the 4032 and 8032 50Hz editor ROMs it is checked against different
       memory locations (0xe3 and 0x3eb) but by default (power-up) it's 10
       anyway.  AF 30jun1998 */
    if (sum == PET8032_CHECKSUM_A || sum == PET8032_CHECKSUM_B) {
        printf("Identified PET 8032 ROM by checksum.\n");
        pet.screen_width = 80;
        kbd_buf_init(0x26f, 0x9e, 10,
                     PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
        autostart_init(3 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0,
                       0xa7, 0xc4, 0xc6, -80);
        tape_init(214, 150, 157, 144, 0xe455, 251, 201, pet4_tape_traps,
                  0x26f, 0x9e);
    } else if (sum == PET3032_CHECKSUM_A || sum == PET3032_CHECKSUM_B) {
        printf("Identified PET 3032 ROM by checksum.\n");
        pet.screen_width = 40;
        kbd_buf_init(0x26f, 0x9e, 10,
                     PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
        autostart_init(3 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0,
                       0xa7, 0xc4, 0xc6, -40);
        tape_init(214, 150, 157, 144, 0xe62e, 251, 201, pet3_tape_traps,
                  0x26f, 0x9e);
    } else if (sum == PET4032_CHECKSUM_A || sum == PET4032_CHECKSUM_B) {
        printf("Identified PET 4032 ROM by checksum.\n");
        pet.screen_width = 40;
        kbd_buf_init(0x26f, 0x9e, 10,
                     PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
        autostart_init(3 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0,
                       0xa7, 0xc4, 0xc6, -40);
        tape_init(214, 150, 157, 144, 0xe455, 251, 201, pet4_tape_traps,
                  0x26f, 0x9e);
    } else if (sum == PET2001_CHECKSUM) {
        printf("Identified PET 2001 ROM by checksum.\n");
        pet.screen_width = 40;
        kbd_buf_init(0x20f, 0x20d, 10,
                     PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
        autostart_init(3 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0,
                       0x224, 0xe0, 0xe2, -40);
        tape_init(243, 0x20c, 0x20b, 0x219, 0xe685, 247, 229, pet2_tape_traps,
                  0x20f, 0x20d);
    } else {
        printf("Unknown PET ROM.\n");
    }

    if (pet.screen_width) {
        set_screen();
    }
    rom_loaded = 1;

    return 0;
}

void set_screen(void)
{
    int cols, vmask;

    cols = pet.video;
    vmask = pet.vmask;

    if (!cols) {
        cols = pet.screen_width;
        vmask = (cols == 40) ? 0x3ff : 0x7ff;
    }
    if (!cols) {
        cols = PET_COLS;
        vmask = (cols == 40) ? 0x3ff : 0x7ff;
    }

    printf("Setting screen width to %d columns (vmask=%04x).\n", cols, vmask);
    crtc_set_screen_mode(ram + 0x8000, vmask, cols, 0);
}

/* ------------------------------------------------------------------------- */

/* FIXME: this does not work for PET 2001.  */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
    if (start != NULL)
        *start = ram[0x28] | (ram[0x29] << 8);
    if (end != NULL)
        *end = ram[0x2a] | (ram[0x2b] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    ram[0x28] = ram[0xc7] = start & 0xff;
    ram[0x29] = ram[0xc8] = start >> 8;
    ram[0x2a] = ram[0x2c] = ram[0x2e] = ram[0xc9] = end & 0xff;
    ram[0x2b] = ram[0x2d] = ram[0x2f] = ram[0xca] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(ADDRESS addr)
{
    return (addr >= 0xf000) && !(map_reg & 0x80);
}


/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

static BYTE peek_bank_io(ADDRESS addr)
{
    BYTE v1, v2, v3, v4;

    if (emu_id_enabled && addr >= 0xE8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }
    switch (addr & 0xf0) {
      case 0x10:                /* PIA1 */
        return peek_pia1(addr);
      case 0x20:                /* PIA2 */
        return peek_pia2(addr);
      case 0x40:
        return peek_via(addr);  /* VIA */
      case 0x80:                /* CRTC */
        if (pet.crtc) {
            if (addr & 1)
                return read_crtc(crtc_ptr);
            else
                return 0x9f;    /* Status. */
        }
      case 0x00:
        return addr >> 8;
      default:                  /* 0x30, 0x50, 0x60, 0x70, 0x90-0xf0 */
        if (addr & 0x10)
            v1 = peek_pia1(addr);
        else
            v1 = 0xff;
        if (addr & 0x20)
            v2 = peek_pia2(addr);
        else
            v2 = 0xff;
        if (addr & 0x40)
            v3 = peek_via(addr);
        else
            v3 = 0xff;
        v4 = 0xff;
        if ((addr & 0x80) && pet.crtc) {
            if (addr & 1)
                v4 = read_crtc(crtc_ptr);
            else
                v4 = 0x9f;      /* Status. */
        }
        return v1 & v2 & v3 & v4;
    }
    return 0xff;
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "io", "extram", NULL
};

static int banknums[] = {
    0, 0, 1, 2, 3, 4
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
      case 4:                   /* extended RAM area (8x96) */
          return ram[addr + 0x10000];
          break;
      case 3:                   /* io */
          if (addr >= 0xE000 && addr <= 0xE0FF) {
              return read_io(addr);
          }
      case 2:                   /* rom */
          if (addr >= 0x9000 && addr <= 0xFFFF) {
              return rom[addr & 0x7fff];
          }
      case 1:                   /* ram */
    }
    return ram[addr];
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);  /* FIXME */
        break;
      case 3:                   /* io */
        if (addr >= 0xE000 && addr <= 0xE0FF) {
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
      case 4:                   /* extended RAM area (8x96) */
        ram[addr + 0x10000] = byte;
        return;
      case 3:                   /* io */
        if (addr >= 0xE000 && addr <= 0xE0FF) {
            store_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0x9000 && addr <= 0xFFFF) {
            return;
        }
      case 1:                   /* ram */
    }
    ram[addr] = byte;
}

/*-----------------------------------------------------------------------*/

/*
 * PET memory dump should be 4-32k or 128k, depending on the config, as RAM.
 * Plus 64k expansion RAM (8096 or SuperPET) if necessary. Also there
 * is the 1/2k video RAM as "VRAM".
 * In this prototype we save the full ram......
 */
#define	PETMEM_DUMP_VER_MAJOR	0
#define	PETMEM_DUMP_VER_MINOR	0

/*
 * UBYTE 	CONFIG		0 = 40 col normal PET
 *				1 = 80 col normal PET
 * 				2 = SuperPET
 *				3 = 8096
 *				4 = 8296
 * UBYTE	MEMSIZE		memory size of low 32k in k (4,8,16,32)
 *
 * UBYTE	CONF8X96	8x96 configuration register
 * UBYTE	SUPERPET	SuperPET config:
 *				Bit 0: spet_ramen,  1= RAM enabled
 *				    1: spet_ramwp,  1= RAM write protected
 *				    2: spet_ctrlwp, 1= CTRL reg write prot.
 *				    3: spet_diag,   0= diag active
 *				    4-7: spet_bank, RAM block in use
 *
 * ARRAY	RAM		4-32k RAM (not 8296, dep. on MEMSIZE)
 * ARRAY	VRAM		2/4k RAM (not 8296, dep in CONFIG)
 * ARRAY	EXTRAM		64k (SuperPET and 8096 only)
 * ARRAY	RAM		128k RAM (8296 only)
 *
 */

int mem_write_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    BYTE config, memsize, conf8x96, superpet;

    memsize = pet.ramSize >> 10;
    if(memsize > 32) {
	memsize = 32;
    }

    config = pet.videoSize == 0x400 ? 0 : 1;
    if(pet.map) {
	config = pet.map + 2;
    } else
    if(pet.superpet) {
	config = 2;
    }

    conf8x96 = map_reg;

    superpet = (spet_ramen ? 1 : 0)
		| (spet_ramwp ? 2 : 0)
		| (spet_ctrlwp ? 4 : 0)
		| (spet_diag ? 8 : 0)
		| ((spet_bank << 4) & 0xf0) ;

    m = snapshot_module_create(p, "PETMEM",
                               PETMEM_DUMP_VER_MAJOR, PETMEM_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;
    snapshot_module_write_byte(m, config);
    snapshot_module_write_byte(m, memsize);
    snapshot_module_write_byte(m, conf8x96);
    snapshot_module_write_byte(m, superpet);

    if(config != 4) {
        snapshot_module_write_byte_array(m, ram, memsize << 10);

	snapshot_module_write_byte_array(m, ram + 0x8000,
					(config == 0) ? 0x400 : 0x800);

        if(config == 2 || config == 3) {
            snapshot_module_write_byte_array(m, ram + 0x10000, 0x10000);
	}
    } else {	/* 8296 */
        snapshot_module_write_byte_array(m, ram, 0x20000);
    }

    snapshot_module_close(m);

    return 0;
}

int mem_read_snapshot_module(snapshot_t *p)
{
    char name[SNAPSHOT_MODULE_NAME_LEN];
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    BYTE config, memsize, conf8x96, superpet;

    m = snapshot_module_open(p, name, &vmajor, &vminor);
    if (m == NULL)
        return -1;
    if (strcmp(name, "PETMEM") || vmajor != PETMEM_DUMP_VER_MAJOR)
        return -1;

    snapshot_module_read_byte(m, &config);
    snapshot_module_read_byte(m, &memsize);
    snapshot_module_read_byte(m, &conf8x96);
    snapshot_module_read_byte(m, &superpet);

    /* TODO: warning if RAM size does not match */

    /* TODO: warning if config does not match */

    if(pet.map) {
	store_8x96(0xfff0, conf8x96);
    }
    map_reg = conf8x96;

    spet_ramen = superpet & 1;
    spet_ramwp = superpet & 2;
    spet_ctrlwp= superpet & 4;
    spet_diag  = superpet & 8;
    spet_bank  = (superpet >> 4) & 0xf;

    if(config != 4) {
        snapshot_module_read_byte_array(m, ram, memsize << 10);

	snapshot_module_read_byte_array(m, ram + 0x8000,
					(config == 0) ? 0x400 : 0x800);

        if(config == 2 || config == 3) {
            snapshot_module_read_byte_array(m, ram + 0x10000, 0x10000);
	}
    } else {	/* 8296 */
        snapshot_module_read_byte_array(m, ram, 0x20000);
    }

    snapshot_module_close(m);

    return 0;
}

