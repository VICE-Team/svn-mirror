/*
 * petmem.c - PET memory handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "crtc-mem.h"
#include "emuid.h"
#include "log.h"
#include "machine.h"

#ifdef WATCOM_COMPILE
#include "../mem.h"
#else
#include "mem.h"
#endif

#include "monitor.h"
#include "pet.h"
#include "petacia.h"
#include "petmem.h"
#include "petpia.h"
#include "petreu.h"
#include "pets.h"
#include "petvia.h"
#include "ram.h"
#include "resources.h"
#include "sid.h"
#include "sid-resources.h"
#include "types.h"
#include "vsync.h"


static BYTE REGPARM1 mem_read_patchbuf(WORD addr);

BYTE petmem_2001_buf_ef[256];

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

/* The PET memory. */

#define RAM_ARRAY 0x20000 /* this includes 8x96 expansion RAM */

BYTE mem_ram[RAM_ARRAY]; /* 128K to make things easier. Real size is 4-128K. */
BYTE mem_rom[PET_ROM_SIZE];
BYTE mem_chargen_rom[PET_CHARGEN_ROM_SIZE];

static int ram_size = RAM_ARRAY;       /* FIXME? */

/* Memory read and write tables. */
static read_func_ptr_t _mem_read_tab[0x101];
static store_func_ptr_t _mem_write_tab[0x101];
static read_func_ptr_t _mem_read_tab_watch[0x101];
static store_func_ptr_t _mem_write_tab_watch[0x101];
static BYTE *_mem_read_base_tab[0x101];
static int mem_read_limit_tab[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* 8x96 mapping register */
BYTE petmem_map_reg = 0;
static int bank8offset = 0;
static int bankCoffset = 0;

static log_t pet_mem_log = LOG_ERR;

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

BYTE REGPARM1 zero_read(WORD addr)
{
    return mem_ram[addr & 0xff];
}

void REGPARM2 zero_store(WORD addr, BYTE value)
{
    mem_ram[addr & 0xff] = value;
}

static BYTE REGPARM1 ram_read(WORD addr)
{
    return mem_ram[addr];
}

static void REGPARM2 ram_store(WORD addr, BYTE value)
{
/*
if (addr == 0x8000) printf("charline=%d, ycount=%d, char=%d\n",
        crtc.current_charline, crtc.raster.ycounter, clk - crtc.rl_start);
*/
    mem_ram[addr] = value;
}

static BYTE REGPARM1 read_ext8(WORD addr)
{
    return mem_ram[addr + bank8offset];
}

static void REGPARM2 store_ext8(WORD addr, BYTE value)
{
    mem_ram[addr + bank8offset] = value;
}

static BYTE REGPARM1 read_extC(WORD addr)
{
    return mem_ram[addr + bankCoffset];
}

static void REGPARM2 store_extC(WORD addr, BYTE value)
{
    mem_ram[addr + bankCoffset] = value;
}

static BYTE REGPARM1 read_vmirror(WORD addr)
{
    return mem_ram[0x8000 + (addr & (petres.videoSize - 1))];
}

static void REGPARM2 store_vmirror(WORD addr, BYTE value)
{
    mem_ram[0x8000 + (addr & (petres.videoSize - 1))] = value;
}

BYTE REGPARM1 rom_read(WORD addr)
{
    return mem_rom[addr & 0x7fff];
}

void REGPARM2 rom_store(WORD addr, BYTE value)
{
    mem_rom[addr & 0x7fff] = value;
}

static BYTE REGPARM1 read_unused(WORD addr)
{
    if (petreu_enabled && addr>=0x8800 && addr<0x8900)
      return read_petreu_reg(addr);

    if (petreu_enabled && addr>=0x8900 && addr<0x8a00)
      return read_petreu_ram(addr);

    if (petreu_enabled && addr>=0x8a00 && addr<0x8b00)
      return read_petreu2_reg(addr);

    if (sidcart_enabled && sidcart_address==1 && addr>=0xe900 && addr<=0xe91f)
      return sid_read(addr);

    if (sidcart_enabled && sidcart_address==0 && addr>=0x8f00 && addr<=0x8f1f)
      return sid_read(addr);

    return (addr >> 8) & 0xff;
}

static BYTE REGPARM1 mem_read_patchbuf(WORD addr)
{
    return petmem_2001_buf_ef[addr & 0xff];
}

/* ------------------------------------------------------------------------- */

/* Functions for watchpoint memory access.  */

static BYTE REGPARM1 read_watch(WORD addr)
{
    monitor_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[addr >> 8](addr);
}

static void REGPARM2 store_watch(WORD addr, BYTE value)
{
    monitor_watch_push_store_addr(addr, e_comp_space);
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

    petmem_map_reg = 0;
}

static void superpet_powerup(void)
{
/* Those two are not reset by a soft reset (/RES), only by power down */
    spet_diag = 0;
    spet_ramwp = 1;
    petmem_reset();
}

int petmem_superpet_diag(void)
{
    return petres.superpet && spet_diag;
}

static BYTE REGPARM1 read_super_io(WORD addr)
{
    if (addr >= 0xeff4) {       /* unused / readonly */
        return read_unused(addr);
    } else
    if (addr >= 0xeff0) {       /* ACIA */
        return acia1_read((WORD)(addr & 0x03));
    } else
    if (addr >= 0xefe4) {       /* unused */
        return read_unused(addr);
    } else
    if (addr >= 0xefe0) {       /* dongle */
    }
    return read_unused(addr);   /* fallback */
}

static void REGPARM2 store_super_io(WORD addr, BYTE value)
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
                    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
                }
                spet_ramwp = !(value & 0x2);
                spet_diag = (value & 0x8);
            }
        } else
        if (addr >= 0xeff4) {   /* unused */
        } else
        if (addr >= 0xeff0) {   /* ACIA */
            acia1_store((WORD)(addr & 0x03), value);
        } else
        if (addr >= 0xefe4) {   /* unused */
        } else
        if (addr >= 0xefe0) {   /* dongle? */
        }
    }
}

static BYTE REGPARM1 read_super_9(WORD addr)
{
    if (spet_ramen) {
        return (mem_ram + 0x10000)[(spet_bank << 12) | (addr & 0x0fff)];
    }
    return rom_read(addr);
}

static void REGPARM2 store_super_9(WORD addr, BYTE value)
{
    if (spet_ramen && !spet_ramwp) {
        (mem_ram + 0x10000)[(spet_bank << 12) | (addr & 0x0fff)] = value;
    }
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

/* The PET have all I/O chips connected to the same select lines.  Only one
   address lines is used as separate (high-active) select input.  I.e. PIA1
   always reacts when address & 0x10 is high, for example $e810, $e830,
   $e850, etc.  PIA2 reacts when address & 0x20 is high, for example $e820,
   $e830 $e860,...  The next two functions try to reflect this behaviour.  */

/* When we write, we write all involved chips.  */

static void REGPARM2 store_io(WORD addr, BYTE value)
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
static BYTE REGPARM1 read_io(WORD addr)
{
    BYTE v1, v2, v3, v4;

    if (emu_id_enabled && addr >= 0xe8a0)
        return emuid_read((WORD)(addr - 0xe8a0));

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

static void REGPARM2 store_dummy(WORD addr, BYTE value)
{
    if (petreu_enabled && addr>=0x8800 && addr<0x8900)
      store_petreu_reg(addr,value);

    if (petreu_enabled && addr>=0x8900 && addr<0x8a00)
      store_petreu_ram(addr,value);

    if (petreu_enabled && addr>=0x8a00 && addr<0x8b00)
      store_petreu2_reg(addr,value);

    if (sidcart_enabled && sidcart_address==1 && addr>=0xe900 && addr<0xe91f)
      sid_store(addr,value);

    if (sidcart_enabled && sidcart_address==0 && addr>=0x8f00 && addr<0x8f1f)
      sid_store(addr,value);

    return;
}

/*
 * This sets the standard PET memory configuration from $9000-$10000.
 * It is used in store_8x96() and mem_initialize_memory().
 */
static void set_std_9tof(void)
{
    int i, l;
    static void REGPARM2 (*store)(WORD, BYTE);
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
                                    : mem_rom + ((i & 0x7f) << 8);
            mem_read_limit_tab[i] = 0x9ffd;
        }
    }

    /* Setup RAM/ROM at $A000 - $AFFF. */
    for (i = 0xa0; i < 0xb0; i++) {
        _mem_read_tab[i] = rama ? ram_read : rom_read;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = rama ? mem_ram + (i << 8)
                                     : mem_rom + ((i & 0x7f) << 8);
        mem_read_limit_tab[i] = 0x9ffd;
    }

    /* Setup ROM at $B000 - $E7FF. */
    for (i = 0xb0; i <= 0xe7; i++) {
        _mem_read_tab[i] = rom_read;
        _mem_write_tab[i] = store;
        _mem_read_base_tab[i] = mem_rom + ((i & 0x7f) << 8);
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
        _mem_read_base_tab[i] = mem_rom + ((i & 0x7f) << 8);
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
void mem_toggle_watchpoints(int flag, void *context)
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
static void REGPARM2 (*store_ff)(WORD addr, BYTE value) = NULL;

/* Write to last page of memory in 8x96.  */
static void REGPARM2 store_8x96(WORD addr, BYTE value)
{
    BYTE changed;
    int l, protected;

    if (store_ff)
        store_ff(addr, value);

    changed = petmem_map_reg ^ value;

    if (addr == 0xfff0 && changed && ((petmem_map_reg | changed) & 0x80)) {
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
        petmem_map_reg = value;

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

    if (petres.map && petmem_map_reg) {
        BYTE old_map_reg;

        old_map_reg = petmem_map_reg;
        petmem_map_reg = 0;
        store_8x96(0xfff0, old_map_reg);
    } else {
        petmem_map_reg = 0;
    }
}

void mem_powerup(void)
{
    ram_init(mem_ram, RAM_ARRAY);

    superpet_powerup();
}

/* ------------------------------------------------------------------------- */

/* FIXME: this does not work for PET 2001.  */

void mem_get_basic_text(WORD *start, WORD *end)
{
    if (start != NULL)
        *start = mem_ram[0x28] | (mem_ram[0x29] << 8);
    if (end != NULL)
        *end = mem_ram[0x2a] | (mem_ram[0x2b] << 8);
}

void mem_set_basic_text(WORD start, WORD end)
{
    mem_ram[0x28] = mem_ram[0xc7] = start & 0xff;
    mem_ram[0x29] = mem_ram[0xc8] = start >> 8;
    mem_ram[0x2a] = mem_ram[0x2c] = mem_ram[0x2e] = mem_ram[0xc9] = end & 0xff;
    mem_ram[0x2b] = mem_ram[0x2d] = mem_ram[0x2f] = mem_ram[0xca] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(WORD addr)
{
    return (addr >= 0xf000) && !(petmem_map_reg & 0x80);
}


/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

static BYTE peek_bank_io(WORD addr)
{
    BYTE v1, v2, v3, v4;

    if (emu_id_enabled && addr >= 0xe8a0)
        return emuid_read((WORD)(addr - 0xe8a0));

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
        break;
    }

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

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "io", "extram", NULL
};

static const int banknums[] = {
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

BYTE mem_bank_read(int bank, WORD addr, void *context)
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
        if (addr >= 0x9000) {
            return mem_rom[addr & 0x7fff];
        }
      case 1:                   /* ram */
        break;
    }
    return mem_ram[addr];
}

BYTE mem_bank_peek(int bank, WORD addr, void *context)
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
    return mem_bank_read(bank, addr, context);
}

void mem_bank_write(int bank, WORD addr, BYTE byte, void *context)
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
        if (addr >= 0x9000) {
            return;
        }
      case 1:                   /* ram */
        break;
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void *context)
{
    mem_ioreg_list_t *mem_ioreg_list = NULL;

    mon_ioreg_add_list(&mem_ioreg_list, "PIA1", 0x0010, 0x001f);
    mon_ioreg_add_list(&mem_ioreg_list, "PIA2", 0x0020, 0x002f);
    mon_ioreg_add_list(&mem_ioreg_list, "VIA", 0x0040, 0x004f);
    if (petres.crtc)
        mon_ioreg_add_list(&mem_ioreg_list, "CRTC", 0x0080, 0x0093);

    return mem_ioreg_list;
}

void mem_get_screen_parameter(WORD *base, BYTE *rows, BYTE *columns)
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
struct pet_table_s { 
    const char *model;
    petinfo_t info;
};
typedef struct pet_table_s pet_table_t;

static pet_table_t pet_table[] = {
    { "2001",
      { 8, 0x0800, 0, 40, 0, 0, 1, 1, 1, 1, 0,
        PET_CHARGEN_NAME, PET_KERNAL1NAME, PET_EDITOR1G40NAME, PET_BASIC1NAME,
        NULL, NULL, NULL } },
    { "3008",
      { 8, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL } },
    { "3016",
      { 16, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL } },
    { "3032",
      { 32, 0x0800, 0, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2G40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL } },
    { "3032B",
      { 32, 0x0800, 0, 40, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL2NAME, PET_EDITOR2B40NAME, PET_BASIC2NAME,
        NULL, NULL, NULL } },
    { "4016",
      { 16, 0x0800, 1, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4G40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "4032",
      { 32, 0x0800, 1, 40, 0, 0, 1, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4G40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "4032B",
      { 32, 0x0800, 1, 40, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B40NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "8032",
      { 32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "8096",
      { 96, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "8296",
      { 128, 0x0100, 1, 80, 0, 0, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { "SuperPET",
      { 32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 1,
        PET_CHARGEN_NAME, PET_KERNAL4NAME, PET_EDITOR4B80NAME, PET_BASIC4NAME,
        NULL, NULL, NULL } },
    { NULL }
};

int petmem_set_conf_info(petinfo_t *pi)
{
    int kindex;

    resources_set_int("RamSize", pi->ramSize);
    resources_set_int("IOSize", pi->IOSize);
    resources_set_int("Crtc", pi->crtc);
    resources_set_int("VideoSize", pi->video);
    resources_set_int("Ram9", pi->mem9);
    resources_set_int("RamA", pi->memA);
    resources_set_int("EoiBlank", pi->eoiblank);
    resources_set_int("SuperPET", pi->superpet);

    resources_get_int("KeymapIndex", &kindex);
    resources_set_int("KeymapIndex", (kindex & 1) + 2 * pi->kbd_type);
    return 0;
}

int pet_set_model_info(petinfo_t *pi)
{
    /* set hardware config */
    petmem_set_conf_info(pi);

    if (pi->pet2k) {    /* set resource only when necessary */
        resources_set_int("Basic1", pi->pet2k);
    }
    resources_set_int("Basic1Chars", pi->pet2kchar);

    resources_set_string("ChargenName", pi->chargenName);
    resources_set_string("KernalName", pi->kernalName);
    resources_set_string("BasicName", pi->basicName);
    resources_set_string("EditorName", pi->editorName);

    /* allow additional ROMs to survive a model switch. */
    if (pi->mem9name)
        resources_set_string("RomModule9Name", pi->mem9name);
    if (pi->memAname)
        resources_set_string("RomModuleAName", pi->memAname);
    if (pi->memBname)
        resources_set_string("RomModuleBName", pi->memBname);
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
                machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
                pet_model = i;
            }
            return 0;
        }
        i++;
    }

    return -1;
}

const char *get_pet_model(void)
{
    return pet_table[pet_model].model;
}

/************************** PET resource handling ************************/

/* check PetInfo struct for consistency after change? */

void petmem_check_info(petres_t *pi)
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

