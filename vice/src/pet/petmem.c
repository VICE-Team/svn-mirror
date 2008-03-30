/*
 * petmem.c - PET memory handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

/*
 * FIXME: the rom_*_loaded flag stuff is not clear enough.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "autostart.h"
#include "cmdline.h"
#include "crtc-mem.h"
#include "crtc.h"
#include "emuid.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mon.h"
#include "pet.h"
#include "petacia.h"
#include "petmem.h"
#include "petpia.h"
#include "pets.h"
#include "petvia.h"
#include "resources.h"
#include "sysfile.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"

static BYTE REGPARM1 mem_read_patchbuf(ADDRESS addr);

static BYTE petmem_2001_buf_ef[ 256 ];

/* Old program counter.  Not used without MMU support.  */
unsigned int mem_old_reg_pc;

/* ------------------------------------------------------------------------- */

/* we keep the current system config in here. */

petres_t petres = { 32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
                    NULL, NULL, NULL,
                    NULL, NULL, NULL,
                    0, 0, 0, 0, 0, 0, 0, 0
};

/* ------------------------------------------------------------------------- */

const char *mem_romset_resources_list[] = {
    "KernalName", "EditorName", "ChargenName", "BasicName",
    "RomModule9Name", "RomModuleAName",
    "DosName2031", "DosName1001",       /* cannot use 15*1 disk drives */
    NULL
};


/* ------------------------------------------------------------------------- */

/* The PET memory. */

#define RAM_ARRAY 0x20000 /* this includes 8x96 expansion RAM */

BYTE mem_ram[RAM_ARRAY]; /* 128K to make things easier. Real size is 4-128K. */
BYTE rom[PET_ROM_SIZE];
BYTE chargen_rom[PET_CHARGEN_ROM_SIZE];

int ram_size = RAM_ARRAY;       /* FIXME? */

/* Memory read and write tables. */
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t _mem_write_tab[0x101];
read_func_ptr_t _mem_read_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
BYTE *_mem_read_base_tab[0x101];
int mem_read_limit_tab[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Flag: nonzero if the ROM has been loaded. */
static int rom_loaded = 0;
int rom_9_loaded = 0;    /* 1 = $9*** ROM is loaded */
int rom_A_loaded = 0;    /* 1 = $A*** ROM is loaded */
int rom_B_loaded = 0;    /* 1 = $B*** ROM or Basic 4 is loaded */

/* 8x96 mapping register */
BYTE map_reg = 0;
static int bank8offset = 0;
static int bankCoffset = 0;

static log_t pet_mem_log = LOG_ERR;

#define IS_NULL(s)  (s == NULL || *s == '\0')

/* ------------------------------------------------------------------------- */

/* Tape traps.  */
static trap_t pet4_tape_traps[] =
{
    {
        "TapeFindHeader",
        0xF5E8,
        0xF5EB,
        { 0x20, 0x9A, 0xF8 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xF8E0,
        0xFCC0,
        { 0x20, 0xE0, 0xFC },
        tape_receive_trap,
        rom_read,
        rom_store
    },
    {
        NULL,
        0,
        0,
        { 0, 0, 0 },
        NULL,
        NULL,
        NULL
    }
};

static trap_t pet3_tape_traps[] =
{
    {
        "TapeFindHeader",
        0xF5A9,
        0xF5AC,
        { 0x20, 0x55, 0xF8 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xF89B,
        0xFC7B,
        { 0x20, 0x9B, 0xFC },
        tape_receive_trap,
        rom_read,
        rom_store
    },
    {
        NULL,
        0,
        0,
        { 0, 0, 0 },
        NULL,
        NULL,
        NULL
    }
};

static trap_t pet2_tape_traps[] =
{
    {
        "TapeFindHeader",
        0xF5B2,
        0xF5B5,
        { 0x20, 0x7F, 0xF8 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xF8A5,
        0xFCFB,
        { 0x20, 0x1B, 0xFD },
        tape_receive_trap,
        rom_read,
        rom_store
    },
    {
        NULL,
        0,
        0,
        { 0, 0, 0 },
        NULL,
        NULL,
        NULL
    }
};

static tape_init_t tapeinit1 = {
    243,
    0x20c,
    0x20b,
    0x219,
    0xe685,
    247,
    229,
    0x20f,
    0x20d,
    pet2_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static tape_init_t tapeinit2 = {
    214,
    150,
    157,
    144,
    0xe62e,
    251,
    201,
    0x26f,
    0x9e,
    pet3_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static tape_init_t tapeinit4 = {
    214,
    150,
    157,
    144,
    0xe455,
    251,
    201,
    0x26f,
    0x9e,
    pet4_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

/* ------------------------------------------------------------------------- */

/* PET-specific resources.  */

/* Flag: Do we enable the Emulator ID?  */
static int emu_id_enabled;

/* hardware config */

void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    return mem_ram[addr & 0xff];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    mem_ram[addr & 0xff] = value;
}

BYTE REGPARM1 ram_read(ADDRESS addr)
{
    return mem_ram[addr];
}

void REGPARM2 ram_store(ADDRESS addr, BYTE value)
{
/*
if (addr == 0x8000) printf("charline=%d, ycount=%d, char=%d\n",
        crtc.current_charline, crtc.raster.ycounter, clk - crtc.rl_start);
*/
    mem_ram[addr] = value;
}

static BYTE REGPARM1 read_ext8(ADDRESS addr)
{
    return mem_ram[addr + bank8offset];
}

static void REGPARM2 store_ext8(ADDRESS addr, BYTE value)
{
    mem_ram[addr + bank8offset] = value;
}

static BYTE REGPARM1 read_extC(ADDRESS addr)
{
    return mem_ram[addr + bankCoffset];
}

static void REGPARM2 store_extC(ADDRESS addr, BYTE value)
{
    mem_ram[addr + bankCoffset] = value;
}

static BYTE REGPARM1 read_vmirror(ADDRESS addr)
{
    return mem_ram[0x8000 + (addr & (petres.videoSize - 1))];
}

static void REGPARM2 store_vmirror(ADDRESS addr, BYTE value)
{
    mem_ram[0x8000 + (addr & (petres.videoSize - 1))] = value;
}

BYTE REGPARM1 rom_read(ADDRESS addr)
{
    return rom[addr & 0x7fff];
}

void REGPARM2 rom_store(ADDRESS addr, BYTE value)
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

int spet_ramen  = 1;
int spet_bank   = 0;
int spet_ctrlwp = 1;
int spet_diag   = 0;
int spet_ramwp  = 1;

void petmem_reset(void)
{
    spet_ramen = 1;
    spet_bank = 0;
    spet_ctrlwp = 1;

    map_reg = 0;
}

void superpet_powerup(void)
{
/* Those two are not reset by a soft reset (/RES), only by power down */
    spet_diag = 0;
    spet_ramwp = 1;
    petmem_reset();
}

int superpet_diag(void)
{
    return petres.superpet && spet_diag;
}

BYTE REGPARM1 read_super_io(ADDRESS addr)
{
    if (addr >= 0xeff4) {       /* unused / readonly */
        return read_unused(addr);
    } else
    if (addr >= 0xeff0) {       /* ACIA */
        return acia1_read((ADDRESS)(addr & 0x03));
    } else
    if (addr >= 0xefe4) {       /* unused */
        return read_unused(addr);
    } else
    if (addr >= 0xefe0) {       /* dongle */
    }
    return read_unused(addr);   /* fallback */
}

void REGPARM2 store_super_io(ADDRESS addr, BYTE value)
{
    if (addr >= 0xeffe) {       /* RAM/ROM switch */
        spet_ramen = !(value & 1);
    } else
    if (addr >= 0xeffc) {       /* Bank select */
        spet_bank = value & 0x0f;
        spet_ctrlwp = !(value & 0x80);
    } else {
        if (addr >= 0xeff8) {
            if (!spet_ctrlwp) {
                if (!(value & 1)) {
                    log_error(pet_mem_log, "SuperPET: 6809 not emulated!");
                    maincpu_trigger_reset();
                }
                spet_ramwp = !(value & 0x2);
                spet_diag = (value & 0x8);
            }
        } else
        if (addr >= 0xeff4) {   /* unused */
        } else
        if (addr >= 0xeff0) {   /* ACIA */
            acia1_store((ADDRESS)(addr & 0x03), value);
        } else
        if (addr >= 0xefe4) {   /* unused */
        } else
        if (addr >= 0xefe0) {   /* dongle? */
        }
    }
}

BYTE REGPARM1 read_super_9(ADDRESS addr)
{
    if (spet_ramen) {
        return (mem_ram + 0x10000)[(spet_bank << 12) | (addr & 0x0fff)];
    }
    return rom_read(addr);
}

void REGPARM2 store_super_9(ADDRESS addr, BYTE value)
{
    if (spet_ramen && !spet_ramwp) {
        (mem_ram + 0x10000)[(spet_bank << 12) | (addr & 0x0fff)] = value;
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
        pia1_store(addr, value);

    if (addr & 0x20)
        pia2_store(addr, value);

    if (addr & 0x40)
        via_store(addr, value);

    if ((addr & 0x80) && petres.crtc) {
        crtc_store(addr, value);
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
        return pia1_read(addr);
      case 0x20:                /* PIA2 */
        return pia2_read(addr);
      case 0x40:
        return via_read(addr);  /* VIA */
      case 0x80:                /* CRTC */
        if (petres.crtc) {
            return crtc_read(addr);
        }
      case 0x00:
        return addr >> 8;
      default:                  /* 0x30, 0x50, 0x60, 0x70, 0x90-0xf0 */
        if (addr & 0x10)
            v1 = pia1_read(addr);
        else
            v1 = 0xff;
        if (addr & 0x20)
            v2 = pia2_read(addr);
        else
            v2 = 0xff;
        if (addr & 0x40)
            v3 = via_read(addr);
        else
            v3 = 0xff;
        v4 = 0xff;
        if ((addr & 0x80) && petres.crtc) {
            v4 = crtc_read(addr);
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
 * It is used in store_8x96() and mem_initialize_memory().
 */
static void set_std_9tof(void)
{
    int i, l;
    static void (*store)(ADDRESS, BYTE);
    int ram9, rama;

    store = (petres.map == 2) ? ram_store : store_dummy;
    ram9 = (petres.map == 2 && petres.mem9) ? 1 : 0;
    rama = (petres.map == 2 && petres.memA) ? 1 : 0;

    /* Setup RAM/ROM at $9000 - $9FFF. */
    if (petres.superpet) {
        for (i = 0x90; i < 0xa0; i++) {
            _mem_read_tab[i] = read_super_9;
            _mem_write_tab[i] = store_super_9;
            _mem_read_base_tab[i] = NULL;
            mem_read_limit_tab[i] = -1;
        }
    } else {
        for (i = 0x90; i < 0xa0; i++) {
            _mem_read_tab[i] = ram9 ? ram_read : rom_read;
            _mem_write_tab[i] = store;
            _mem_read_base_tab[i] = ram9 ? mem_ram + (i << 8)
                                    : rom + ((i & 0x7f) << 8);
            mem_read_limit_tab[i] = 0x9ffd;
        }
    }

    /* Setup RAM/ROM at $A000 - $AFFF. */
    for (i = 0xa0; i < 0xb0; i++) {
        _mem_read_tab[i] = rama ? ram_read : rom_read;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = rama ? mem_ram + (i << 8)
                                     : rom + ((i & 0x7f) << 8);
        mem_read_limit_tab[i] = 0x9ffd;
    }

    /* Setup ROM at $B000 - $E7FF. */
    for (i = 0xb0; i <= 0xe7; i++) {
        _mem_read_tab[i] = rom_read;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = rom + ((i & 0x7f) << 8);
        mem_read_limit_tab[i] = 0xe7fd;
    }

    l = ((0xe800 + petres.IOSize) >> 8) & 0xff;

    /* Setup I/O at $e800 - $e800 + petres.IOSize. */
    /* i.e. IO at $e800... */
    _mem_read_tab[0xe8] = read_io;
    _mem_write_tab[0xe8] = store_io;
    _mem_read_base_tab[0xe8] = NULL;
    mem_read_limit_tab[0xe8] = -1;

    /* ... and unused address space behind it */
    for (i = 0xe9; i < l; i++) {
        _mem_read_tab[i] = read_unused;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = NULL;
        mem_read_limit_tab[i] = -1;
    }

    if (petres.superpet) {
        _mem_read_tab[0xef] = read_super_io;
        _mem_write_tab[0xef] = store_super_io;
        _mem_read_base_tab[0xef] = NULL;
        mem_read_limit_tab[0xef] = -1;
    } else
    if (petres.rompatch) {
        _mem_read_tab[0xef] = mem_read_patchbuf;
        _mem_write_tab[0xef] = store_dummy;
        _mem_read_base_tab[0xef] = petmem_2001_buf_ef;
        mem_read_limit_tab[0xef] = 0xeffd;
    }

    /* Setup ROM at $e800 + petres.IOSize - $ffff */
    for (i = l; i <= 0xff; i++) {
        _mem_read_tab[i] = rom_read;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = rom + ((i & 0x7f) << 8);
        mem_read_limit_tab[i] = 0xfffd;
    }

    _mem_read_base_tab_ptr = _mem_read_base_tab;
    mem_read_limit_tab_ptr = mem_read_limit_tab;
}

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    /* We do not need MMU support.  */
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
        if (value & 0x80) {     /* ext. RAM enabled */
            if (changed & 0xa5) {       /* $8000-$bfff */
                protected = value & 0x01;
                l = 0x80;
                if (value & 0x20) {     /* screen memory mapped through */
                    for (; l < 0x90; l++) {
                        _mem_read_tab[l] = ram_read;
                        _mem_write_tab[l] = ram_store;
                        _mem_read_base_tab[l] = mem_ram + (l << 8);
                        mem_read_limit_tab[l] = 0x8ffd;
                    }
                }
                bank8offset = 0x8000 + ((value & 0x04) ? 0x8000 : 0);
                for (; l < 0xc0; l++) {
                    _mem_read_tab[l] = read_ext8;
                    if (protected)
                        _mem_write_tab[l] = store_dummy;
                    else
                        _mem_write_tab[l] = store_ext8;
                    _mem_read_base_tab[l] = mem_ram + bank8offset + (l << 8);
                    mem_read_limit_tab[l] = 0xbffd;
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
                        mem_read_limit_tab[l] = -1;
                    } else {
                        _mem_read_tab[l] = read_extC;
                        if (protected)
                            _mem_write_tab[l] = store_dummy;
                        else
                            _mem_write_tab[l] = store_extC;
                        _mem_read_base_tab[l] = mem_ram
                                                + bankCoffset + (l << 8);
                        if (l < 0xe8) {
                            mem_read_limit_tab[l] = 0xe7fd;
                        } else {
                            mem_read_limit_tab[l] = 0xfffd;
                        }
                    }
                }
                store_ff = _mem_write_tab[0xff];
                _mem_write_tab[0xff] = store_8x96;
            }
        } else {                /* disable ext. RAM */
            for (l = 0x80; l < 0x90; l++) {
                _mem_read_tab[l] = ram_read;
                _mem_write_tab[l] = ram_store;
                _mem_read_base_tab[l] = mem_ram + (l << 8);
                mem_read_limit_tab[l] = 0x8ffd;
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

static void set_vidmem(void) {
    int i, l;

    l = ((0x8000 + petres.videoSize) >> 8) & 0xff;
/*
    log_message(pet_mem_log, "set_vidmem(videoSize=%04x, l=%d)",
                petres.videoSize,l);
*/
    /* Setup RAM from $8000 to $8000 + petres.videoSize */
    for (i = 0x80; i < l; i++) {
        _mem_read_tab[i] = ram_read;
        _mem_write_tab[i] = ram_store;
        _mem_read_base_tab[i] = mem_ram + (i << 8);
        mem_read_limit_tab[i] = (l<<8)-3;
    }

    /* Setup video mirror from $8000 + petres.videoSize to $87ff */
    /* falls through if videoSize >= 0x800 */
    for (; i < 0x88; i++) {
        _mem_read_tab[i] = read_vmirror;
        _mem_write_tab[i] = store_vmirror;
        _mem_read_base_tab[i] = mem_ram + 0x8000 + ((i << 8)
                                & (petres.videoSize - 1));
        mem_read_limit_tab[i] = 0x87fd;
    }

    /* Setup unused from $8800 to $8fff */
    /* falls through if videoSize >= 0x1000 */
    for (; i < 0x90; i++) {
        _mem_read_tab[i] = read_unused;
        _mem_write_tab[i] = store_dummy;
        _mem_read_base_tab[i] = NULL;
        mem_read_limit_tab[i] = -1;
    }
}

/* ------------------------------------------------------------------------- */

/* This does the plain 8032 configuration, as 8096 stuff only comes up when
   writing to $fff0.  */
void mem_initialize_memory(void)
{
    int i, l;

    l = petres.ramSize << 2;       /* ramSize in kB, l in 256 Byte */
    if (l > 128)
        l = 128;                /* fix 8096 / 8296 */

    /* Setup RAM from $0000 to petres.ramSize */
    for (i = 0x00; i < l; i++) {
        _mem_read_tab[i] = ram_read;
        _mem_write_tab[i] = ram_store;
        _mem_read_base_tab[i] = mem_ram + (i << 8);
        mem_read_limit_tab[i] = (l << 8) - 3;
    }

    /* Setup unused from petres.ramSize to $7fff */
    for (i = l; i < 0x80; i++) {
        _mem_read_tab[i] = read_unused;
        _mem_write_tab[i] = store_dummy;
        _mem_read_base_tab[i] = NULL;
        mem_read_limit_tab[i] = -1;
    }

    set_vidmem();

    set_std_9tof();

    if (petres.map) {              /* catch writes to $fff0 register */
        store_ff = _mem_write_tab[0xff];
        _mem_write_tab[0xff] = store_8x96;
    }
    _mem_read_tab[0x100] = _mem_read_tab[0];
    _mem_write_tab[0x100] = _mem_write_tab[0];
    _mem_read_base_tab[0x100] = _mem_read_base_tab[0];
    mem_read_limit_tab[0x100] = -1;

    ram_size = petres.ramSize * 1024;
    _mem_read_tab_ptr = _mem_read_tab;
    _mem_write_tab_ptr = _mem_write_tab;

    for (i = 0; i < 0x101; i++) {
        _mem_read_tab_watch[i] = read_watch;
        _mem_write_tab_watch[i] = store_watch;
    }

    if (petres.map && map_reg) {
        BYTE old_map_reg;

        old_map_reg = map_reg;
        map_reg = 0;
        store_8x96(0xfff0, old_map_reg);
    } else {
        map_reg = 0;
    }
}

void mem_powerup(void)
{
    int i;

    for (i = 0; i < RAM_ARRAY; i += 0x80) {
        memset(mem_ram + i, 0, 0x40);
        memset(mem_ram + i + 0x40, 0xff, 0x40);
    }

    superpet_powerup();
}

/* ------------------------------------------------------------------------- */

/* where we save the unchanged PET kernal 1 areas before patching */
static BYTE petmem_2001_patchbuf_f1[ 256 ];
static BYTE petmem_2001_patchbuf_f3[ 256 ];
static BYTE petmem_2001_patchbuf_f4[ 256 ];
static BYTE petmem_2001_patchbuf_f6[ 256 ];


static BYTE REGPARM1 mem_read_patchbuf(ADDRESS addr)
{
    return petmem_2001_buf_ef[ addr & 0xff ];
}

void petmem_unpatch_2001(void)
{
    /* if not patched return */
    if (!petres.rompatch) return;

    log_warning(pet_mem_log,
                        "PET2001 ROM loaded, but patches disabled! "
                        "IEEE488 will not work.");

    memcpy(rom+0x7100, petmem_2001_patchbuf_f1, 0x100);
    memcpy(rom+0x7300, petmem_2001_patchbuf_f3, 0x100);
    memcpy(rom+0x7400, petmem_2001_patchbuf_f4, 0x100);
    memcpy(rom+0x7600, petmem_2001_patchbuf_f6, 0x100);

    petres.rompatch = 0;

    mem_initialize_memory();
}

void petmem_patch_2001(void)
{
    int i;
    int rp;
    BYTE dat0[] = {0xa9, 0x60, 0x85, 0xf0, 0x60};
    BYTE dat1[] = {0x20, 0xb6, 0xf0, 0xa5, 0xf0, 0x20, 0x5b, 0xf1,
                   0x20, 0x87, 0xf1, 0x85, 0xf7,
                   0x20, 0x87, 0xf1, 0x85, 0xf8, 0x60};
    BYTE dat2[] = {0x20, 0x7a, 0xf1, 0x20, 0xe6, 0xf6,
                   0xad, 0x0b, 0x02, 0x60};
    BYTE dat3[] = {0xa9, 0x61, 0x85, 0xf0, 0x60};
    BYTE dat4[] = {0x20, 0xba, 0xf0, 0xa5, 0xf0, 0x20, 0x2c, 0xf1,
                   0xa5, 0xf7, 0x20, 0x67, 0xf1,
                   0xa5, 0xf8, 0x4c, 0x67, 0xf1};
    BYTE dat5[] = {0xae, 0x0c, 0x02, 0x70, 0x46, 0x20, 0x87, 0xf1};
    BYTE dat6[] = {0x20, 0x2c, 0xf1, 0x4c, 0x7e, 0xf1};

    /* check if already patched */
    if (petres.rompatch)
        return;

    /* check for ROM version */
    if (petres.kernal_checksum != PET_KERNAL1_CHECKSUM)
        return;

    /* check whether patch enabled */
    if (!petres.pet2k) {
        log_warning(pet_mem_log,
                        "PET2001 ROM loaded, but patches not enabled! "
                        "IEEE488 will not work.");
        return;
    }

    log_warning(pet_mem_log, "patching 2001 ROM to make IEEE488 work!");

    memcpy(petmem_2001_patchbuf_f1, rom+0x7100, 0x100);
    memcpy(petmem_2001_patchbuf_f3, rom+0x7300, 0x100);
    memcpy(petmem_2001_patchbuf_f4, rom+0x7400, 0x100);
    memcpy(petmem_2001_patchbuf_f6, rom+0x7600, 0x100);

    /* Patch PET2001 IEEE488 routines to make them work */
    rom[0x7471] = rom[0x7472] = 0xea;   /* NOP */
    rom[0x7180] = rom[0x7181] = 0xea;   /* NOP */
    rom[0x73ef] = 0xf8;
    rom[0x73f3] = 0xf7;
    rp = 0xef00;                /* $ef00 */
    rom[0x7370] = rp & 0xff;
    rom[0x7371] = ((rp >> 8) & 0xff);
    for (i = 0; i < 5; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat0[i];
    rom[0x7379] = rp & 0xff;
    rom[0x737a] = ((rp >> 8) & 0xff);
    for (i = 0; i < 19; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat1[i];
    rom[0x73cc] = 0x20;
    rom[0x73cd] = rp & 0xff;
    rom[0x73ce] = ((rp >> 8) & 0xff);
    for (i = 0; i < 10; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat2[i];
    for (i = 0; i < 8; i++)
        rom[0x7381 + i] = dat5[i];

    rom[0x76c1] = rp & 0xff;
    rom[0x76c2] = ((rp >> 8) & 0xff);
    for (i = 0; i < 5; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat3[i];
    rom[0x76c7] = rp & 0xff;
    rom[0x76c8] = ((rp >> 8) & 0xff);
    for (i = 0; i < 18; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat4[i];
    rom[0x76f4] = rp & 0xff;
    rom[0x76f5] = ((rp >> 8) & 0xff);
    for (i = 0; i < 6; i++)
        petmem_2001_buf_ef[(rp++) & 0xff] = dat6[i];

    strcpy((char*)(petmem_2001_buf_ef + (rp & 0xff)),
                        "vice pet2001 rom patch $ef00-$efff");

    petres.rompatch = 1;

    mem_initialize_memory();
}

void petmem_get_kernal_checksum(void)
{
   int i;

   /* Checksum over top 4 kByte PET kernal.  */
    petres.kernal_checksum = 0;
    for (i = 0x7000; i < 0x8000; i++)
        petres.kernal_checksum += rom[i];
}

void petmem_get_editor_checksum(void)
{
    int i;

    /* 4032 and 8032 have the same kernals, so we have to test more, here
       $E000 - $E800.  */
    petres.editor_checksum = 0;
    for (i = 0x6000; i < 0x6800; i++)
        petres.editor_checksum += rom[i];
}

void petmem_checksum(void)
{
    static WORD last_kernal = 0;
    static WORD last_editor = 0;

    /* log_message(pet_mem_log, "editor checksum=%d, kernal checksum=%d",
                   (int) petres.editor_checksum,
                   (int) petres.kernal_checksum); */

    /* ignore the same message popping up more than once - but only
       check when printing, because we need the tape traps etc */

    petres.rom_video = 0;

    /* The length of the keyboard buffer might actually differ from 10 - in
       the 4032 and 8032 50Hz editor ROMs it is checked against different
       memory locations (0xe3 and 0x3eb) but by default (power-up) it's 10
       anyway.  AF 30jun1998 */
    if (petres.kernal_checksum == PET_KERNAL4_CHECKSUM) {
        if (petres.kernal_checksum != last_kernal)
            log_message(pet_mem_log, "Identified Kernal 4 ROM by checksum.");
        kbd_buf_init(0x26f, 0x9e, 10,
                     (CLOCK)(PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC));
        tape_init(&tapeinit4);
        if (petres.editor_checksum == PET_EDIT4B80_CHECKSUM) {
            if (petres.editor_checksum != last_editor)
                log_message(pet_mem_log,
                            "Identified 80 columns editor by checksum.");
            petres.rom_video = 80;
            autostart_init((CLOCK)(3 * PET_PAL_RFSH_PER_SEC
                           * PET_PAL_CYCLES_PER_RFSH),
                           0, 0xa7, 0xc4, 0xc6, -80);
        } else
        if (petres.editor_checksum == PET_EDIT4B40_CHECKSUM
            || petres.editor_checksum == PET_EDIT4G40_CHECKSUM) {
            if (petres.editor_checksum != last_editor)
                log_message(pet_mem_log,
                            "Identified 80 columns editor by checksum.");
            petres.rom_video = 40;
            autostart_init((CLOCK)(3 * PET_PAL_RFSH_PER_SEC
                           * PET_PAL_CYCLES_PER_RFSH),
                           0, 0xa7, 0xc4, 0xc6, -40);
        }
    } else if (petres.kernal_checksum == PET_KERNAL2_CHECKSUM) {
        if (petres.kernal_checksum != last_kernal)
            log_message(pet_mem_log, "Identified Kernal 2 ROM by checksum.");
        petres.rom_video = 40;
        kbd_buf_init(0x26f, 0x9e, 10,
                     (CLOCK)(PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC));
        autostart_init((CLOCK)(3 * PET_PAL_RFSH_PER_SEC
                       * PET_PAL_CYCLES_PER_RFSH), 0,
                       0xa7, 0xc4, 0xc6, -40);
        tape_init(&tapeinit2);
    } else if (petres.kernal_checksum == PET_KERNAL1_CHECKSUM) {
        if (petres.kernal_checksum != last_kernal)
            log_message(pet_mem_log, "Identified Kernal 1 ROM by checksum.");
        petres.rom_video = 40;
        kbd_buf_init(0x20f, 0x20d, 10,
                     (CLOCK)(PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC));
        autostart_init((CLOCK)(3 * PET_PAL_RFSH_PER_SEC
                       * PET_PAL_CYCLES_PER_RFSH), 0,
                       0x224, 0xe0, 0xe2, -40);
        tape_init(&tapeinit1);
    } else {
        log_warning(pet_mem_log, "Unknown PET ROM.");
    }
    last_kernal = petres.kernal_checksum;
    last_editor = petres.editor_checksum;
}

void petmem_convert_chargen(BYTE *charrom)
{
    int i, j;

    /* Copy graphics charom to second part.  */
    memmove(charrom + 0x800, charrom + 0x400, 0x400);

    /* Inverted chargen into second half. This is a PET hardware feature.  */
    for (i = 0; i < 0x400; i++) {
        charrom[i + 0x400] = charrom[i] ^ 0xff;
        charrom[i + 0xc00] = charrom[i + 0x800] ^ 0xff;
    }

    /* now expand 8 byte/char to 16 byte/char charrom for the CRTC */
    for (i = 511; i>=0; i--) {
        for (j=7; j>=0; j--) {
            charrom[i*16+j] = charrom[i*8+j];
        }
        for (j=7; j>=0; j--) {
            charrom[i*16+8+j] = 0;
        }
    }
}

void petmem_convert_chargen_2k(void)
{
    int i,j;

#if 0
    /* This only works right after loading! */
    /* If pet2001 then exchange upper and lower case letters.  */
    for (i = 8; i < (0x1b * 8); i++) {
        j = chargen_rom[0x400 + i];
        chargen_rom[i + 0x400] = chargen_rom[i + 0x600];
        chargen_rom[i + 0x600] = j;
    }
#endif
    /* If pet2001 then exchange upper and lower case letters.  */
    for (i = 16; i < 0x1b0; i++) {
        /* first the not inverted chars */
        j = chargen_rom[0x1000 + i];
        chargen_rom[0x1000 + i] = chargen_rom[0x1400 + i];
        chargen_rom[0x1400 + i] = j;
    }
    /* If pet2001 then exchange upper and lower case letters.  */
    for (i = 16; i < 0x1b0; i++) {
        /* then the inverted chars */
        j = chargen_rom[0x1800 + i];
        chargen_rom[0x1800 + i] = chargen_rom[0x1c00 + i];
        chargen_rom[0x1c00 + i] = j;
    }
}
 
int mem_load_chargen(void)
{
    if (!rom_loaded)
        return 0;

    if (IS_NULL(petres.chargenName))
        return 0;

    /* Load chargen ROM - we load 2k with 8 bytes/char, and generate
       the inverted 2k. Then we expand the chars to 16 bytes/char
       for the CRTC, filling the rest with zeros */

    if (sysfile_load(petres.chargenName, chargen_rom, 0x800, 0x800) < 0) {
        log_error(pet_mem_log,
                  "Couldn't load character ROM (%s).", petres.chargenName);
        return -1;
    }

    if (petres.pet2kchar) {
        petmem_convert_chargen_2k();
    }

    petmem_convert_chargen(chargen_rom);

    crtc_set_chargen_addr(chargen_rom, 512);

    return 0;
}

int mem_load_basic(void)
{
    int krsize;
    ADDRESS old_start, new_start;

    if (!rom_loaded)
        return 0;

    /* Load Kernal ROM.  */
    if (!IS_NULL(petres.basicName)) {
        const char *name = petres.basicName;

        if ((krsize = sysfile_load(name, rom + 0x3000, 0x2000, 0x3000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.", name);
            return -1;
        }

        old_start = petres.basic_start;
        new_start = 0xe000 - krsize;

        petres.basic_start = new_start;

        if (old_start && (new_start > old_start)) {
            if (old_start <= 0xB000 && new_start >= 0xC000) {
                resources_set_value("RomModuleBName", (resource_value_t) NULL);
            }
        }

        /* setting the _loaded flag to 0 before setting the resource
           does not overwrite ROM! */
        /* if kernal long enough, "unload" expansion ROMs */
        if (petres.basic_start <= 0xb000) {
            rom_B_loaded = 0;
            resources_set_value("RomModuleBName", (resource_value_t) NULL);
            rom_B_loaded = 1;
        }
    }
    return 0;
}

int mem_load_kernal(void)
{
    int krsize;

    if (!rom_loaded)
        return 0;

    /* De-initialize kbd-buf, autostart and tape stuff here before
       reloading the ROM the traps are installed in.  */
    /* log_warning(pet_mem_log, "Deinstalling Traps"); */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_deinstall();

    /* Load Kernal ROM.  */
    if (!IS_NULL(petres.kernalName)) {
        const char *name = petres.kernalName;

        if ((krsize = sysfile_load(name, rom + 0x7000, 0x1000, 0x1000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.", name);
            return -1;
        }
        petmem_get_kernal_checksum();
        petres.rompatch = 0;
        petmem_patch_2001();
    }

    petmem_checksum();

    return 0;
}

int mem_load_editor(void)
{
    int rsize, i;

    if (!rom_loaded)
        return 0;

    /* De-initialize kbd-buf, autostart and tape stuff here before
       reloading the ROM the traps are installed in.  */
    /* log_warning(pet_mem_log, "Deinstalling Traps"); */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_deinstall();

    if (!IS_NULL(petres.editorName)) {
        const char *name = petres.editorName;

        if ((rsize = sysfile_load(name, rom + 0x6000, 0x0800, 0x1000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.", name);
            return -1;
        }
        if (rsize == 0x800) {
            memcpy(rom+0x6000, rom+0x6800, 0x800);
            for (i = 0x800; i < 0x1000; i++)
                *(rom + 0x6000 + i) = 0xe0 | (i>>8);
        }
        petmem_get_editor_checksum();
    }

    petmem_checksum();

    return 0;
}

int mem_load_rom9(void)
{
    int rsize, i;

    if (!rom_loaded)
        return 0;

    if (!IS_NULL(petres.mem9name)) {
        if ((rsize = sysfile_load(petres.mem9name,
            rom + 0x1000, 0x0800, 0x1000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.", petres.mem9name);
            return -1;
        }
        if (rsize == 0x800) {
            memcpy(rom+0x1000, rom+0x1800, 0x800);
            for (i = 0x800; i < 0x1000; i++)
                *(rom + 0x1000 + i) = 0x90 | (i>>8);
        }
        rom_9_loaded = 1;
    } else {
        if (petres.basic_start >= 0xA000) {
            for (i = 0; i < 16; i++)
                memset(rom + 0x1000 + (i<<8), 0x90 + i, 256);
        }
        rom_9_loaded = 0;
    }
    return 0;
}

int mem_load_romA(void)
{
    int rsize, i;

    if (!rom_loaded)
        return 0;

    if (!IS_NULL(petres.memAname)) {
        if ((rsize = sysfile_load(petres.memAname,
            rom + 0x2000, 0x0800, 0x1000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.", petres.memAname);
            return -1;
        }
        if (rsize == 0x800) {
            memcpy(rom + 0x2000, rom + 0x2800, 0x800);
            for (i = 0x800; i < 0x1000; i++)
                *(rom + 0x2000 + i) = 0xA0 | (i>>8);
        }
        rom_A_loaded = 1;
    } else {
        if (petres.basic_start >= 0xB000) {
            for (i = 0; i < 16; i++)
            memset(rom + 0x2000 + (i<<8), 0xA0 + i, 256);
       }
       rom_A_loaded = 0;
    }
    return 0;
}

int mem_load_romB(void)
{
    int rsize, i;

    if (!rom_loaded)
        return 0;

    if (!IS_NULL(petres.memBname)) {
        if ((rsize = sysfile_load(petres.memBname, rom + 0x3000,
            0x0800, 0x1000)) < 0) {
            log_error(pet_mem_log, "Couldn't load ROM `%s'.",
                      petres.memBname);
            return -1;
        }
        if (rsize == 0x800) {
            memcpy(rom + 0x3000, rom + 0x3800, 0x800);
            for (i = 0x800; i < 0x1000; i++)
                *(rom + 0x3000 + i) = 0xB0 | (i>>8);
        }
        rom_B_loaded = 1;
    } else {
        if (petres.basic_start >= 0xC000) {
            for (i = 0; i < 16; i++)
                memset(rom + 0x3000 + (i<<8), 0xB0 + i, 256);
            rom_B_loaded = 0;
        }
    }
    return 0;
}

/* Load memory image files.  This also selects the PET model.  */
int mem_load(void)
{
    int i;

    if (pet_mem_log == LOG_ERR)
        pet_mem_log = log_open("PETMEM");

    rom_loaded = 1;

    tape_deinstall();

    /* Init ROM with 'unused address' values.  */
    for (i = 0; i < PET_ROM_SIZE; i++) {
        rom[i] = 0x80 + ((i >> 8) & 0xff);
    }

    if (mem_load_chargen() < 0)
        return -1;

    if (mem_load_basic() < 0)
        return -1;

    if (mem_load_kernal() < 0)
        return -1;

    if (mem_load_editor() < 0)
        return -1;

    if (mem_load_rom9() < 0)
        return -1;

    if (mem_load_romA() < 0)
        return -1;

    if (mem_load_romB() < 0)
        return -1;

    if (petres.rom_video) {
        log_message(pet_mem_log, "ROM screen width is %d.",
                petres.rom_video);
    } else {
        log_message(pet_mem_log, "ROM screen width is unknown.");
    }

    mem_initialize_memory();

    return 0;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this does not work for PET 2001.  */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
    if (start != NULL)
        *start = mem_ram[0x28] | (mem_ram[0x29] << 8);
    if (end != NULL)
        *end = mem_ram[0x2a] | (mem_ram[0x2b] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    mem_ram[0x28] = mem_ram[0xc7] = start & 0xff;
    mem_ram[0x29] = mem_ram[0xc8] = start >> 8;
    mem_ram[0x2a] = mem_ram[0x2c] = mem_ram[0x2e] = mem_ram[0xc9] = end & 0xff;
    mem_ram[0x2b] = mem_ram[0x2d] = mem_ram[0x2f] = mem_ram[0xca] = end >> 8;
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

    if (emu_id_enabled && addr >= 0xe8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }
    switch (addr & 0xf0) {
      case 0x10:                /* PIA1 */
        return pia1_peek(addr);
      case 0x20:                /* PIA2 */
        return pia2_peek(addr);
      case 0x40:
        return via_peek(addr);  /* VIA */
      case 0x80:                /* CRTC */
        if (petres.crtc) {
            return crtc_read(addr);
        }
      case 0x00:
        return addr >> 8;
      default:                  /* 0x30, 0x50, 0x60, 0x70, 0x90-0xf0 */
        if (addr & 0x10)
            v1 = pia1_peek(addr);
        else
            v1 = 0xff;
        if (addr & 0x20)
            v2 = pia2_peek(addr);
        else
            v2 = 0xff;
        if (addr & 0x40)
            v3 = via_peek(addr);
        else
            v3 = 0xff;
        v4 = 0xff;
        if ((addr & 0x80) && petres.crtc) {
            v4 = crtc_read(addr);
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
        return mem_ram[addr + 0x10000];
        break;
      case 3:                   /* io */
        if (addr >= 0xe000 && addr <= 0xe0ff) {
            return read_io(addr);
        }
      case 2:                   /* rom */
        if (addr >= 0x9000 && addr <= 0xffff) {
            return rom[addr & 0x7fff];
        }
      case 1:                   /* ram */
        break;
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
        if (addr >= 0xe000 && addr <= 0xe0ff) {
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
        mem_ram[addr + 0x10000] = byte;
        return;
      case 3:                   /* io */
        if (addr >= 0xe000 && addr <= 0xe0ff) {
            store_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0x9000 && addr <= 0xffff) {
            return;
        }
      case 1:                   /* ram */
        break;
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void)
{
    mem_ioreg_list_t *mem_ioreg_list;

    mem_ioreg_list = (mem_ioreg_list_t *)xmalloc(sizeof(mem_ioreg_list_t) * 4);

    mem_ioreg_list[0].name = "PIA1";
    mem_ioreg_list[0].start = 0x0010;
    mem_ioreg_list[0].end = 0x001f;
    mem_ioreg_list[0].next = &mem_ioreg_list[1];

    mem_ioreg_list[1].name = "PIA2";
    mem_ioreg_list[1].start = 0x0020;
    mem_ioreg_list[1].end = 0x002f;
    mem_ioreg_list[1].next = &mem_ioreg_list[2];

    mem_ioreg_list[2].name = "VIA";
    mem_ioreg_list[2].start = 0x0040;
    mem_ioreg_list[2].end = 0x004f;
    mem_ioreg_list[2].next = NULL;

    if (petres.crtc) {
        mem_ioreg_list[2].next = &mem_ioreg_list[3];

        mem_ioreg_list[3].name = "CRTC";
        mem_ioreg_list[3].start = 0x0080;
        mem_ioreg_list[3].end = 0x0093;
        mem_ioreg_list[3].next = NULL;
    }

    return mem_ioreg_list;
}

void mem_get_screen_parameter(ADDRESS *base, BYTE *rows, BYTE *columns)
{
    /* FIXME */
    *base = 0;
    *rows = 25;
    *columns = 80;
}

/*-----------------------------------------------------------------------*/

int pet_init_ok = 0;

/* ------------------------------------------------------------------------- */

#define PET_CHARGEN_NAME        "chargen"

/*
 * table with Model information
 */
static struct {
    const char *model;
    petinfo_t info;
} pet_table[] = {
    {"2001",
      {8, 0x0800, 0, 40, 0, 0, 1, 1, 1, 1, 0,
        PET_CHARGEN_NAME, PET_KERNAL1NAME, PET_EDITOR1G40NAME, PET_BASIC1NAME,
        NULL, NULL, NULL}},
    {"3008",
      {8, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL}},
    {"3016",
      {16, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL}},
    {"3032",
      {32, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL}},
    {"3032B",
      {32, 0x0800, 0, 40, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2B40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL}},
    {"4016",
      {16, 0x0800, 1, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4G40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"4032",
      {32, 0x0800, 1, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4G40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"4032B",
      {32, 0x0800, 1, 40, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"8032",
      {32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"8096",
      {96, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"8296",
      {128, 0x0100, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {"SuperPET",
      {32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 1,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL}},
    {NULL}
};

int pet_set_conf_info(petinfo_t *pi)
{
    int kindex;

    resources_set_value("RamSize",
                        (resource_value_t) pi->ramSize);
    resources_set_value("IOSize",
                        (resource_value_t) pi->IOSize);
    resources_set_value("Crtc",
                        (resource_value_t) pi->crtc);
    resources_set_value("VideoSize",
                        (resource_value_t) pi->video);
    resources_set_value("Ram9",
                        (resource_value_t) pi->mem9);
    resources_set_value("RamA",
                        (resource_value_t) pi->memA);
    resources_set_value("EoiBlank",
                        (resource_value_t) pi->eoiblank);
    resources_set_value("SuperPET",
                        (resource_value_t) pi->superpet);

    resources_get_value("KeymapIndex",
                        (resource_value_t *) &kindex);
    resources_set_value("KeymapIndex",
                        (resource_value_t) ((kindex & 1) + 2 * pi->kbd_type));
    return 0;
}

int pet_set_model_info(petinfo_t *pi)
{
    /* set hardware config */
    pet_set_conf_info(pi);

    if (pi->pet2k) {    /* set resource only when necessary */
        resources_set_value("Basic1",
                            (resource_value_t) pi->pet2k);
    }
    resources_set_value("Basic1Chars",
                        (resource_value_t) pi->pet2kchar);

    resources_set_value("ChargenName",
                        (resource_value_t) pi->chargenName);
    resources_set_value("KernalName",
                        (resource_value_t) pi->kernalName);
    resources_set_value("BasicName",
                        (resource_value_t) pi->basicName);
    resources_set_value("EditorName",
                        (resource_value_t) pi->editorName);

    /* allow additional ROMs to survive a model switch. */
    if (pi->mem9name)
        resources_set_value("RomModule9Name",
                            (resource_value_t) pi->mem9name);
    if (pi->memAname)
        resources_set_value("RomModuleAName",
                            (resource_value_t) pi->memAname);
    if (pi->memBname)
        resources_set_value("RomModuleBName",
                            (resource_value_t) pi->memBname);
    return 0;
}

static int pet_model = 8;

int pet_set_model(const char *model_name, void *extra)
{
    int i;

    i = 0;
    while (pet_table[i].model) {
        if (!strcmp(pet_table[i].model, model_name)) {

            pet_set_model_info(&pet_table[i].info);

            /* we have to wait until we have done enough initialization */
            if (pet_init_ok) {
                /* mem_load(); - not needed as resources now load */
                vsync_suspend_speed_eval();
                maincpu_trigger_reset();
                pet_model = i;
            }
            return 0;
        }
        i++;
    }

    return -1;
}

const char *get_pet_model()
{
    return pet_table[pet_model].model;
}

/************************** PET resource handling ************************/

/* check PetInfo struct for consistency after change? */

void pet_check_info(petres_t *pi)
{
    if (pi->superpet) {
        pi->ramSize = 32;       /* 128 */
        pi->map = 0;
    }

    if (pi->video == 40 || (pi->video == 0 && pi->rom_video == 40)) {
        pi->vmask = 0x3ff;
        pi->videoSize = 0x400;
    } else {
        pi->vmask = 0x7ff;
        pi->videoSize = 0x800;
    }

    if (pi->ramSize == 128) {
        pi->vmask = 0x1fff;
        pi->videoSize = 0x1000;
    }
}

