/*
 * c610mem.c - CBM-II memory handling.
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "archdep.h"
#include "autostart.h"
#include "c610-resources.h"
#include "c610.h"
#include "c610acia.h"
#include "c610cia.h"
#include "c610mem.h"
#include "c610tpi.h"
#include "cmdline.h"
#include "crtc-mem.h"
#include "crtc.h"
#include "emuid.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mon.h"
#include "resources.h"
#include "sid.h"
#include "sysfile.h"
#include "tape.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "vicii-mem.h"
#include "vicii.h"

/* ------------------------------------------------------------------------- */

/* ultimax = 0 then the others are not needed */
int ultimax = 0;
int romh_bank;
BYTE romh_banks[1];

void cia1_set_extended_keyboard_rows_mask(BYTE foo) {}

/* ------------------------------------------------------------------------- */
/* The CBM-II memory. */

BYTE ram[C610_RAM_SIZE];                /* 1M, banks 0-14 plus extension RAM
                                           in bank 15 */
BYTE rom[C610_ROM_SIZE];                /* complete bank 15 ROM + video RAM */
BYTE chargen_rom[C610_CHARGEN_ROM_SIZE];

BYTE *page_zero;
BYTE *page_one;

/* selected banks for normal access and indirect accesses */
int bank_exec = -1;
int bank_ind = -1;

/* Memory read and write tables - banked. */
static read_func_ptr_t _mem_read_tab[16][0x101];
static store_func_ptr_t _mem_write_tab[16][0x101];
static BYTE *_mem_read_base_tab[16][0x101];
static int mem_read_limit_tab[3][0x101];

/* watch tables are fixed */
read_func_ptr_t _mem_read_tab_watch[0x101];
read_func_ptr_t _mem_read_ind_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
store_func_ptr_t _mem_write_ind_tab_watch[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
read_func_ptr_t *_mem_read_ind_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
store_func_ptr_t *_mem_write_ind_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
BYTE **_mem_read_ind_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int old_reg_pc;

int cbm2_init_ok = 0;

/* Flag: nonzero if the ROM has been loaded. */
static int rom_loaded = 0;

#define IS_NULL(s)  (s == NULL || *s == '\0')

static log_t c610_mem_log = LOG_ERR;

static tape_init_t tapeinit = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    NULL,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

/* ------------------------------------------------------------------------- */

/* state of tpi pc6/7 */
static int c500_vbank = 0;

/* 1= static video matrix RAM (phi2); 0= bank 0 */
static int c500_statvid = 1;

/* 1= character ROM in bank 15 (phi1); 0= bank 0 */
static int c500_vicdotsel = 1;

void c500_set_phi2_bank(int b) {
    if (b == c500_statvid)
        return;

    if (b) {    /* bank 15 */
        /* video memory at $c000/d000 depending on d818 */
        vic_ii_set_phi2_addr_options(0x13ff, 0xc000);
        /* no chargen mapping */
        vic_ii_set_phi2_chargen_addr_options(0, 1);
        /* memory mapping */
        vic_ii_set_phi2_vbank(3);       /* necessary? */
        vic_ii_set_phi2_ram_base(rom);
    } else {
        /* video memory in bank 0 */
        vic_ii_set_phi2_addr_options(0xffff, 0x0000);
        /* no chargen mapping */
        vic_ii_set_phi2_chargen_addr_options(0, 1);
        /* memory mapping */
        vic_ii_set_phi2_vbank(c500_vbank);
        vic_ii_set_phi2_ram_base(ram);
    }

    c500_statvid = b;
}

void c500_set_phi1_bank(int b) {
    if (b == c500_vicdotsel) return;

    if (b) {    /* bank 15 */
        /* video memory at $c000/c800 depending on d818 */
        vic_ii_set_phi1_addr_options(0x0fff, 0xc000);
        /* no chargen mapping */
        vic_ii_set_phi1_chargen_addr_options(0xc000, 0xc000);
        /* memory mapping */
        vic_ii_set_phi1_vbank(3);       /* necessary? */
        vic_ii_set_phi1_ram_base(rom);
    } else {
        /* video memory in bank 0 */
        vic_ii_set_phi1_addr_options(0xffff, 0x0000);
        /* no chargen mapping */
        vic_ii_set_phi1_chargen_addr_options(0, 1);
        /* memory mapping */
        vic_ii_set_phi1_vbank(c500_vbank);
        vic_ii_set_phi1_ram_base(ram);
    }

    c500_vicdotsel = b;
}

void cbm2_set_tpi2pc(BYTE b) {
    int vbank = (b & 0xc0) >> 6;
    c500_vbank = vbank;

    if (isC500) {
        if (!c500_vicdotsel) {
            vic_ii_set_phi1_vbank(vbank);
        }
        if (!c500_statvid) {
            vic_ii_set_phi2_vbank(vbank);
        }
    }
}

void cbm2_set_tpi1ca(int a) {
    if (isC500) {
        c500_set_phi2_bank(a);
    } else {
        crtc_set_chargen_offset((a) ? 256 : 0);
    }
}

void cbm2_set_tpi1cb(int a) {
    if (isC500) {
        c500_set_phi1_bank(a);
    }
}

/* ------------------------------------------------------------------------- */

const char *mem_romset_resources_list[] = {
    "KernalName", "ChargenName", "BasicName",
    "Cart1Name", "Cart2Name", "Cart4Name", "Cart6Name",
    "DosName2031", "DosName1001",
    "DosName1541", "DosName1571", "DosName1581", "DosName1541ii",
    NULL
};

/* ------------------------------------------------------------------------- */

/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

/* ------------------------------------------------------------------------- */

static struct {
    const char *model;
    const int usevicii;
    const int ramsize;
    const char *basic;
    const char *charrom;
    const char *kernal;
    const int line; /* 0=7x0 (50 Hz), 1=6x0 60Hz, 2=6x0 50Hz */
} modtab[] = {
    { "510",  1, 64,   CBM2_BASIC500, CBM2_CHARGEN500, CBM2_KERNAL500, 2  },
    { "610",  0, 128,  CBM2_BASIC128, CBM2_CHARGEN600, CBM2_KERNAL, 2  },
    { "620",  0, 256,  CBM2_BASIC256, CBM2_CHARGEN600, CBM2_KERNAL, 2  },
    { "620+", 0, 1024, CBM2_BASIC256, CBM2_CHARGEN600, CBM2_KERNAL, 2  },
    { "710",  0, 128,  CBM2_BASIC128, CBM2_CHARGEN700, CBM2_KERNAL, 0  },
    { "720",  0, 256,  CBM2_BASIC256, CBM2_CHARGEN700, CBM2_KERNAL, 0  },
    { "720+", 0, 1024, CBM2_BASIC256, CBM2_CHARGEN700, CBM2_KERNAL, 0  },
    { NULL }
};

static int cbm2_model = 1;

int cbm2_set_model(const char *model, void *extra)
{
    int i;

    vsync_suspend_speed_eval();

    for(i = 0; modtab[i].model; i++)
    {
        if (strcmp(modtab[i].model, model))
            continue;

        resources_set_value("UseVicII",
                            (resource_value_t)(modtab[i].usevicii));
        resources_set_value("RamSize",
                            (resource_value_t)(modtab[i].ramsize));
        resources_set_value("BasicName",
                            (resource_value_t)(modtab[i].basic));
        resources_set_value("ChargenName",
                            (resource_value_t)(modtab[i].charrom));
        resources_set_value("KernalName",
                            (resource_value_t)(modtab[i].kernal));
        resources_set_value("ModelLine",
                             (resource_value_t)(modtab[i].line));

        cbm2_model = i;

        /* we have to wait until we did enough initialization */
        if (!cbm2_init_ok)
            return 0; 

        mem_powerup();
        mem_load();
        maincpu_trigger_reset();
        return 0;
    }
    return -1;
}

const char *cbm2_get_model()
{
    return modtab[cbm2_model].model;
}

/* ------------------------------------------------------------------------- */

void set_bank_exec(int val) {
    int i;

    val &= 0x0f;
    if (val != bank_exec) {

        bank_exec = val;

        _mem_read_tab_ptr      = _mem_read_tab[bank_exec];
        _mem_write_tab_ptr     = _mem_write_tab[bank_exec];
        _mem_read_base_tab_ptr = _mem_read_base_tab[bank_exec];
        mem_read_limit_tab_ptr = mem_read_limit_tab[(bank_exec < 15) ? 0 : 1];

        if (bank_limit != NULL) {
            *bank_base = _mem_read_base_tab_ptr[old_reg_pc >> 8];
            if (*bank_base != 0) {
                *bank_base = _mem_read_base_tab_ptr[old_reg_pc >> 8]
                         - (old_reg_pc & 0xff00);
            } else {
                /* disable fast opcode fetch when bank_base is null, i.e.
                   set all limits to 0 when no RAM available.
                   This might also happen when jumping to open mem in
                   bank 15, though. */
                mem_read_limit_tab_ptr = mem_read_limit_tab[2];
            }
            *bank_limit = mem_read_limit_tab_ptr[old_reg_pc >> 8];
        }

        /* set all register mirror locations */
        for (i = 0; i < 16; i++) {
            ram[i << 16] = val;
        }

        page_zero = _mem_read_base_tab_ptr[0];
        page_one = _mem_read_base_tab_ptr[1];

        /* This sets the pointers to otherwise non-mapped memory, to
           avoid that the CPU code uses illegal memory and segfaults. */
        if (!page_zero)
            page_zero = ram + 0xf0000;
        if (!page_one)
            page_one = ram + 0xf0100;
    }
}

void set_bank_ind(int val)
{
    int i;
    val &= 0x0f;

    if (val != bank_ind) {
        bank_ind = val;
        _mem_read_ind_tab_ptr      = _mem_read_tab[bank_ind];
        _mem_write_ind_tab_ptr     = _mem_write_tab[bank_ind];
        _mem_read_ind_base_tab_ptr = _mem_read_base_tab[bank_ind];
        /* set all register mirror locations */
        for (i = 0;i < 16; i++) {
            ram[(i << 16) + 1] = val;
        }
    }
}

/* ------------------------------------------------------------------------- */

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    if (addr == 0)
        set_bank_exec(value);
    else
      if (addr == 1)
          set_bank_ind(value);

    _mem_write_tab_ptr[0]((ADDRESS)(addr & 0xff), value);
}

#define STORE_ZERO(bank)                                                \
    void REGPARM2 store_zero_##bank(ADDRESS addr, BYTE value)           \
    {                                                                   \
        addr &= 0xff;                                                   \
                                                                        \
        if (addr == 0) set_bank_exec(value); else                       \
        if (addr == 1) set_bank_ind(value);                             \
                                                                        \
        ram[(0x##bank << 16) | addr] = value;                           \
    }


#define READ_ZERO(bank)                                                 \
    BYTE REGPARM1 read_zero_##bank(ADDRESS addr)                        \
    {                                                                   \
        return ram[(0x##bank << 16) | (addr & 0xff)];                   \
    }

#define READ_RAM(bank)                                                  \
    BYTE REGPARM1 read_ram_##bank(ADDRESS addr)                         \
    {                                                                   \
        return ram[(0x##bank << 16) | addr];                            \
    }

#define STORE_RAM(bank)                                                 \
    void REGPARM2 store_ram_##bank(ADDRESS addr, BYTE byte)             \
    {                                                                   \
        ram[(0x##bank << 16) | addr] = byte;                            \
    }

STORE_ZERO(0)
STORE_ZERO(1)
STORE_ZERO(2)
STORE_ZERO(3)
STORE_ZERO(4)
STORE_ZERO(5)
STORE_ZERO(6)
STORE_ZERO(7)
STORE_ZERO(8)
STORE_ZERO(9)
STORE_ZERO(A)
STORE_ZERO(B)
STORE_ZERO(C)
STORE_ZERO(D)
STORE_ZERO(E)
STORE_ZERO(F)

READ_ZERO(0)
READ_ZERO(1)
READ_ZERO(2)
READ_ZERO(3)
READ_ZERO(4)
READ_ZERO(5)
READ_ZERO(6)
READ_ZERO(7)
READ_ZERO(8)
READ_ZERO(9)
READ_ZERO(A)
READ_ZERO(B)
READ_ZERO(C)
READ_ZERO(D)
READ_ZERO(E)
READ_ZERO(F)

STORE_RAM(0)
STORE_RAM(1)
STORE_RAM(2)
STORE_RAM(3)
STORE_RAM(4)
STORE_RAM(5)
STORE_RAM(6)
STORE_RAM(7)
STORE_RAM(8)
STORE_RAM(9)
STORE_RAM(A)
STORE_RAM(B)
STORE_RAM(C)
STORE_RAM(D)
STORE_RAM(E)
STORE_RAM(F)

READ_RAM(0)
READ_RAM(1)
READ_RAM(2)
READ_RAM(3)
READ_RAM(4)
READ_RAM(5)
READ_RAM(6)
READ_RAM(7)
READ_RAM(8)
READ_RAM(9)
READ_RAM(A)
READ_RAM(B)
READ_RAM(C)
READ_RAM(D)
READ_RAM(E)
READ_RAM(F)

static store_func_ptr_t store_zero_tab[16] = {
        store_zero_0, store_zero_1, store_zero_2, store_zero_3,
        store_zero_4, store_zero_5, store_zero_6, store_zero_7,
        store_zero_8, store_zero_9, store_zero_A, store_zero_B,
        store_zero_C, store_zero_D, store_zero_E, store_zero_F
};

static store_func_ptr_t store_ram_tab[16] = {
        store_ram_0, store_ram_1, store_ram_2, store_ram_3,
        store_ram_4, store_ram_5, store_ram_6, store_ram_7,
        store_ram_8, store_ram_9, store_ram_A, store_ram_B,
        store_ram_C, store_ram_D, store_ram_E, store_ram_F
};

static read_func_ptr_t read_ram_tab[16] = {
        read_ram_0, read_ram_1, read_ram_2, read_ram_3,
        read_ram_4, read_ram_5, read_ram_6, read_ram_7,
        read_ram_8, read_ram_9, read_ram_A, read_ram_B,
        read_ram_C, read_ram_D, read_ram_E, read_ram_F
};

static read_func_ptr_t read_zero_tab[16] = {
        read_zero_0, read_zero_1, read_zero_2, read_zero_3,
        read_zero_4, read_zero_5, read_zero_6, read_zero_7,
        read_zero_8, read_zero_9, read_zero_A, read_zero_B,
        read_zero_C, read_zero_D, read_zero_E, read_zero_F
};


void REGPARM2 store_zeroX(ADDRESS addr, BYTE value)
{
    if (addr == 0)
        set_bank_exec(value);
    else
        if (addr == 1)
            set_bank_ind(value);
}

BYTE REGPARM1 rom_read(ADDRESS addr)
{
    return rom[addr];
}

BYTE REGPARM1 read_chargen(ADDRESS addr)
{
    return chargen_rom[addr & 0xfff];
}

void REGPARM2 rom_store(ADDRESS addr, BYTE value)
{
    rom[addr] = value;
}

static BYTE REGPARM1 read_unused(ADDRESS addr)
{
    return 0xff; /* (addr >> 8) & 0xff; */
}

static void REGPARM2 store_dummy(ADDRESS addr, BYTE value)
{
    return;
}

/* ------------------------------------------------------------------------- */

/* Functions for watchpoint memory access.  */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[bank_exec][addr >> 8](addr);
}

void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab[bank_exec][addr >> 8](addr, value);
}

BYTE REGPARM1 read_ind_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[bank_ind][addr >> 8](addr);
}

void REGPARM2 store_ind_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab[bank_ind][addr >> 8](addr, value);
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

void REGPARM2 store_io(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf800) {
      case 0xd000:
        rom_store(addr, value);         /* video RAM mapped here... */
        if (isC500 && (addr >= 0xd400)) {
            colorram_store(addr, value);
        }
        return;
      case 0xd800:
        switch(addr & 0xff00) {
          case 0xd800:
            if (isC500) {
                vic_store(addr, value);
            } else {
                crtc_store(addr, value);
            }
            return;
          case 0xd900:
            return;                     /* disk units */
          case 0xda00:
            sid_store((ADDRESS)(addr & 0xff), value);
            return;
          case 0xdb00:
            return;                     /* coprocessor */
          case 0xdc00:
            cia1_store((ADDRESS)(addr & 0x0f), value);
            return;
          case 0xdd00:
            acia1_store((ADDRESS)(addr & 0x03), value);
            return;
          case 0xde00:
            tpi1_store((ADDRESS)(addr & 0x07), value);
            return;
          case 0xdf00:
            tpi2_store((ADDRESS)(addr & 0x07), value);
            return;
        }
    }
}


BYTE REGPARM1 read_io(ADDRESS addr)
{
/*
    if (emu_id_enabled && addr >= 0xE8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }
*/

    switch (addr & 0xf800) {
      case 0xd000:
        return rom_read(addr);
      case 0xd800:
        switch (addr & 0xff00) {
          case 0xd800:
            if (isC500) {
                return vic_read(addr);
            } else {
                return crtc_read(addr);
            }
          case 0xd900:
            return read_unused(addr);
          case 0xda00:
            if (isC500) {
                return sid_read(addr);
            } else {
                return 0xff;            /* 2 MHz too fast for SID */
            }
          case 0xdb00:
            return read_unused(addr);
          case 0xdc00:
            return cia1_read(addr);
          case 0xdd00:
            return acia1_read(addr);
          case 0xde00:
            /* FIXME: VIC-II irq? */
            /* if (isC500 && ((addr & 7) == 2)) {
                   return tpi1_read(addr&7)|1; }   */
            return tpi1_read((ADDRESS)(addr & 0x07));
          case 0xdf00:
            return tpi2_read((ADDRESS)(addr & 0x07));
        }
    }
    return read_unused(addr);
}


/* FIXME: TODO! */
void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = _mem_read_tab_watch;
        _mem_read_ind_tab_ptr = _mem_read_ind_tab_watch;
        _mem_write_tab_ptr = _mem_write_tab_watch;
        _mem_write_ind_tab_ptr = _mem_write_ind_tab_watch;
    } else {
        set_bank_exec(bank_exec);
        set_bank_ind(bank_ind);
    }
}

/* ------------------------------------------------------------------------- */
/* handle CPU reset */

void mem_reset(void) {
    set_bank_exec(15);
    set_bank_ind(15);

    if (isC500) {
        c500_set_phi1_bank(15);
        c500_set_phi2_bank(15);
    }
}

/* ------------------------------------------------------------------------- */

void mem_initialize_memory(void)
{
    int i;

    /* first the tables that hold the predefined bank mappings */
    for (i = 0; i < 16; i++) {          /* 16 banks possible */
        mem_initialize_memory_bank(i);
    }

    /* set bank limit tables for optimized opcode fetch */
    for (i=256;i>=0;i--) {
        mem_read_limit_tab[0][i] = 0xfffd;      /* all RAM banks go here */
        mem_read_limit_tab[2][i] = 0;           /* all empty banks go here */

        if (!_mem_read_base_tab[15][i]) {
            mem_read_limit_tab[1][i] = -1;
        } else
        if (i < 0x08) { /* system RAM */
            mem_read_limit_tab[1][i] = 0x07fd;
        } else
        if (i < 0x10) { /* ROM/RAM 0800-0FFF */
            mem_read_limit_tab[1][i] = 0x0ffd;
        } else
        if (i < 0x20) { /* ROM/RAM 1000-1FFF */
            mem_read_limit_tab[1][i] = 0x1ffd;
        } else
        if (i < 0x40) { /* ROM/RAM 2000-3FFF */
            mem_read_limit_tab[1][i] = 0x3ffd;
        } else
        if (i < 0x60) { /* ROM/RAM 4000-5FFF */
            mem_read_limit_tab[1][i] = 0x5ffd;
        } else
        if (i < 0x80) { /* ROM/RAM 6000-7FFF */
            mem_read_limit_tab[1][i] = 0x7ffd;
        } else
        if (i < 0xc0) { /* ROM 8000-BFFF */
            mem_read_limit_tab[1][i] = 0xbffd;
        } else
        if (i <0xd0) {  /* C000-CFFF */
            if (isC500) { /* charrom */
                mem_read_limit_tab[1][i] = 0xcffd;
            } else {    /* open(?) */
                mem_read_limit_tab[1][i] = 0;
            }
        } else
        if (i < 0xe0) { /* I/O D000-DFFF */
            mem_read_limit_tab[1][i] = 0;
        } else {        /* ROM E000-FFFF */
            mem_read_limit_tab[1][i] = 0xfffd;
        }
    }

    /* set watchpoint tables */
    for (i = 256; i >= 0; i--) {
        _mem_read_tab_watch[i] = read_watch;
        _mem_read_ind_tab_watch[i] = read_ind_watch;
        _mem_write_tab_watch[i] = store_watch;
        _mem_write_ind_tab_watch[i] = store_ind_watch;
    }
}

void mem_initialize_memory_bank(int i)
{
    int j;

    switch (i) {
      case 0:
        if (isC500 || ramsize >= 512) {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_ram_tab[i];
                _mem_write_tab[i][j] = store_ram_tab[i];
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
            _mem_write_tab[i][0] = store_zero_tab[i];
            _mem_read_tab[i][0] = read_zero_tab[i];
        } else {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_unused;
                _mem_write_tab[i][j] = store_dummy;
                _mem_read_base_tab[i][j] = NULL;
            }
           _mem_write_tab[i][0] = store_zeroX;
        }
        break;
      case 1:
        for (j = 255; j >= 0; j--) {
            _mem_read_tab[i][j] = read_ram_tab[i];
            _mem_write_tab[i][j] = store_ram_tab[i];
            _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
        }
        _mem_write_tab[i][0] = store_zero_tab[i];
        _mem_read_tab[i][0] = read_zero_tab[i];
        break;
      case 2:
        if (ramsize >= 128) {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_ram_tab[i];
                _mem_write_tab[i][j] = store_ram_tab[i];
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
            _mem_write_tab[i][0] = store_zero_tab[i];
            _mem_read_tab[i][0] = read_zero_tab[i];
            break;
        }
      case 3:
      case 4:
        if (ramsize >= 256) {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_ram_tab[i];
                _mem_write_tab[i][j] = store_ram_tab[i];
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
            _mem_write_tab[i][0] = store_zero_tab[i];
            _mem_read_tab[i][0] = read_zero_tab[i];
            break;
        }
      case 5:
      case 6:
      case 7:
        if (ramsize >= 512) {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_ram_tab[i];
                _mem_write_tab[i][j] = store_ram_tab[i];
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
            _mem_write_tab[i][0] = store_zero_tab[i];
            _mem_read_tab[i][0] = read_zero_tab[i];
            break;
        }
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
        if (ramsize >= 1024) {
            for (j = 255; j >= 0; j--) {
                _mem_read_tab[i][j] = read_ram_tab[i];
                _mem_write_tab[i][j] = store_ram_tab[i];
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
           _mem_write_tab[i][0] = store_zero_tab[i];
           _mem_read_tab[i][0] = read_zero_tab[i];
           break;
        }
        /* fallback for ramsize < some_value */
        for (j = 255; j >= 0; j--) {
            _mem_read_tab[i][j] = read_unused;
            _mem_write_tab[i][j] = store_dummy;
            _mem_read_base_tab[i][j] = NULL;
        }
        _mem_write_tab[i][0] = store_zeroX;
        break;
      case 15:
        for (j = 0; j < 0x08; j++) {
            _mem_read_tab[i][j] = read_ram_F;
            _mem_write_tab[i][j] = store_ram_F;
            _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
        }
        for (; j < 0xc0; j++) { /* 0800-BFFF */
            _mem_read_tab[i][j] = rom_read;
            _mem_write_tab[i][j] = store_dummy;
            _mem_read_base_tab[i][j] = rom + (j << 8);
        }
        for (; j < 0xd0; j++) { /* C000-CFFF */
            if (!isC500) {
                _mem_read_tab[i][j] = read_unused;
                _mem_write_tab[i][j] = store_dummy;
                _mem_read_base_tab[i][j] = NULL;
            } else {
                _mem_read_tab[i][j] = read_chargen;
                _mem_write_tab[i][j] = store_dummy;
                _mem_read_base_tab[i][j] = chargen_rom + ((j << 8) & 0x0f);
            }
        }
        for (; j < 0xe0; j++) { /* D000-DFFF */
            _mem_read_tab[i][j] = read_io;
            _mem_write_tab[i][j] = store_io;
            _mem_read_base_tab[i][j] = NULL;
        }
        for (; j < 0x100; j++) {
            _mem_read_tab[i][j] = rom_read;
            _mem_write_tab[i][j] = store_dummy;
            _mem_read_base_tab[i][j] = rom + (j << 8);
        }

        if (cart08_ram) {
            for (j = 0x08; j < 0x10; j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }
        if (cart1_ram) {
            for (j=0x10;j<0x20;j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }
        if (cart2_ram) {
            for (j = 0x20; j < 0x40; j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }
        if (cart4_ram) {
            for (j = 0x40; j < 0x60; j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }
        if (cart6_ram) {
            for (j = 0x60; j < 0x80; j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }
        if (cartC_ram) {
            for (j = 0xc0; j < 0xd0; j++) {
                _mem_read_tab[i][j] = read_ram_F;
                _mem_write_tab[i][j] = store_ram_F;
                _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
            }
        }

        _mem_write_tab[i][0] = store_zero_F;
        _mem_read_tab[i][0] = read_zero_F;
        _mem_read_base_tab[i][0] = ram + 0xf0000;
        break;
    }
    _mem_read_tab[i][0x100] = _mem_read_tab[i][0];
    _mem_write_tab[i][0x100] = _mem_write_tab[i][0];
    _mem_read_base_tab[i][0x100] = _mem_read_base_tab[i][0];
}


void mem_powerup(void)
{
    int i;

    for (i = 0; i < C610_RAM_SIZE; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }
    for (i = 0; i < 0x800; i += 0x80) {
        memset(rom + i, 0, 0x40);
        memset(rom + i + 0x40, 0xff, 0x40);
        memset(rom + 0x800 + i, 0, 0x40);
        memset(rom + 0x800 + i + 0x40, 0xff, 0x40);
        memset(rom + 0xd000 + i, 0, 0x40);
        memset(rom + 0xd000 + i + 0x40, 0xff, 0x40);
    }

    bank_exec = 0;
    bank_ind = 0;
    set_bank_exec(15);
    set_bank_ind(15);
}

/*************************************************************************
 * Load all the ROMs.
 * Called from mem_load() and from setting the resources.
 */

int mem_load_chargen(const char *rom_name)
{
    int i;

    if (!rom_loaded)
        return 0;  /* init not far enough */

    /* Load chargen ROM
     * we load 4k of 16-byte-per-char Charrom.
     * Then we generate the inverted chars */

    if (!IS_NULL(rom_name)) {
        memset(chargen_rom, 0, C610_CHARGEN_ROM_SIZE);

        if (sysfile_load(rom_name, chargen_rom, 4096, 4096) < 0) {
            log_error(c610_mem_log, "Couldn't load character ROM '%s'.",
                      rom_name);
            return -1;
        }

        if (!isC500) {
            memmove(chargen_rom + 4096, chargen_rom + 2048, 2048);

            /* Inverted chargen into second half. This is a hardware feature.*/
            for (i = 0; i < 2048; i++) {
                chargen_rom[i + 2048] = chargen_rom[i] ^ 0xff;
                chargen_rom[i + 6144] = chargen_rom[i + 4096] ^ 0xff;
            }
        }
    }

    if (isC500) {
        /* VIC-II config */
    } else {
        crtc_set_chargen_addr(chargen_rom, C610_CHARGEN_ROM_SIZE >> 4);
    }

    return 0;
}

int mem_checksum(void)
{
    int i;
    WORD sum;

    /* Checksum over top 8 kByte kernal.  */
    for (i = 0xe000, sum = 0; i < 0x10000; i++)
        sum += rom[i];

    log_message(c610_mem_log, "Kernal checksum is %d ($%04X).",
                sum, sum);
    return 0;
}

int mem_load_kernal(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    /* De-initialize kbd-buf, autostart and tape stuff here before
       reloading the ROM the traps are installed in.  */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_init(&tapeinit);

    /* Load Kernal ROM.  */
    if (!IS_NULL(rom_name)) {
        if (sysfile_load(rom_name, rom + 0xe000, 0x2000, 0x2000) < 0) {
            log_error(c610_mem_log, "Couldn't load ROM `%s'.", rom_name);
            return -1;
        }
    }

    return mem_checksum();
}


int mem_load_basic(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    /* Load BASIC ROM.  */
    if (!IS_NULL(rom_name)) {
        if ((sysfile_load(rom_name, rom + 0x8000, 0x4000, 0x4000) < 0)) {
            log_error(c610_mem_log, "Couldn't load BASIC ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        log_warning(c610_mem_log, "Disabling BASIC by unloading ROM!");
        memset(rom + 0x8000, 0xff, 0x4000);
    }
    return 0;
}

int mem_load_cart_1(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    if (!IS_NULL(rom_name)) {
        if ((sysfile_load(rom_name, rom + 0x1000, 0x1000, 0x1000) < 0)) {
            log_error(c610_mem_log, "Couldn't load ROM `%s'.",
                      rom_name);
        }
    } else {
        memset(rom + 0x1000, 0xff, 0x1000);
    }
    return 0;
}

int mem_load_cart_2(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    if (!IS_NULL(rom_name)) {
        if ((sysfile_load(rom_name, rom + 0x2000, 0x2000, 0x2000) < 0)) {
            log_error(c610_mem_log, "Couldn't load ROM `%s'.",
                      rom_name);
        }
    } else {
        memset(rom + 0x2000, 0xff, 0x2000);
    }
    return 0;
}

int mem_load_cart_4(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    if (!IS_NULL(rom_name)) {
        if ((sysfile_load(rom_name, rom + 0x4000, 0x2000, 0x2000) < 0)) {
            log_error(c610_mem_log, "Couldn't load ROM `%s'.",
                      rom_name);
        }
    } else {
        memset(rom + 0x4000, 0xff, 0x2000);
    }
    return 0;
}

int mem_load_cart_6(const char *rom_name)
{
    if (!rom_loaded)
        return 0;  /* init not far enough */

    if (!IS_NULL(rom_name)) {
        if ((sysfile_load(rom_name, rom + 0x6000, 0x2000, 0x2000) < 0)) {
            log_error(c610_mem_log, "Couldn't load ROM `%s'.",
                      rom_name);
        }
    } else {
        memset(rom + 0x6000, 0xff, 0x2000);
    }
    return 0;
}

/* Load memory image files. */
int mem_load(void)
{
    int i;
    char *rom_name = NULL;

    if (c610_mem_log == LOG_ERR)
        c610_mem_log = log_open("CBM2MEM");

    rom_loaded = 1;

    if (resources_get_value("ChargenName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_chargen(rom_name) < 0)
        return -1;

    /* Init Disk/Cartridge ROM with 'unused address' values.  */
    for (i = 0x800; i < 0x8000; i++) {
        rom[i] = 0xff;
    }

    if (resources_get_value("KernalName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_kernal(rom_name) < 0)
        return -1;

    if (resources_get_value("BasicName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_basic(rom_name) < 0)
        return -1;

    /* Load extension ROMs.  */

    if (resources_get_value("Cart1Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_cart_1(rom_name) < 0)
        return -1;

    if (resources_get_value("Cart2Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_cart_2(rom_name) < 0)
        return -1;

    if (resources_get_value("Cart4Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_cart_4(rom_name) < 0)
        return -1;

    if (resources_get_value("Cart6Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (mem_load_cart_6(rom_name) < 0)
        return -1;

    if (isC500) {
        /* VIC-II config */
    } else {
        crtc_set_screen_addr(rom + 0xd000);
    }

    return 0;
}

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

/* ------------------------------------------------------------------------- */

/* FIXME: To do!  */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(ADDRESS addr)
{
    return 1;   /* (addr >= 0xf000) && !(map_reg & 0x80); */
}

void mem_set_tape_sense(int value)
{
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

static BYTE peek_bank_io(ADDRESS addr)
{
    switch (addr & 0xf800) {
      case 0xc000:
      case 0xc800:
        return read_unused(addr);
      case 0xd000:
        return rom_read(addr);
      case 0xd800:
        switch (addr & 0xff00) {
          case 0xd800:
            if (isC500) {
                return vic_peek(addr);
            } else {
                return crtc_read(addr);
            }
          case 0xd900:
            return read_unused(addr);
          case 0xda00:
            return sid_read(addr);
          case 0xdb00:
            return read_unused(addr);
          case 0xdc00:
            return cia1_peek(addr);
          case 0xdd00:
            return acia1_peek(addr);
          case 0xde00:
            return tpi1_peek((ADDRESS)(addr & 0x07));
          case 0xdf00:
            return tpi2_peek((ADDRESS)(addr & 0x07));
        }
    }
    return read_unused(addr);
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram0", "ram1", "ram2", "ram3",
    "ram4", "ram5", "ram6", "ram7", "ram8", "ram9",
    "ramA", "ramB", "ramC", "ramD", "ramE", "ramF",
    "romio", NULL
};

static int banknums[] = {
    17, 17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
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
      case 17:                  /* current */
        return mem_read(addr);
      case 16:                   /* romio */
        if (addr >= 0xd000 && addr < 0xe000)
            return read_io(addr);
        return _mem_read_tab[15][addr >> 8](addr);
      default:
        if (bank >=0 && bank <15) {
            return read_ram_tab[bank](addr);
        }
    }
    return read_unused(addr);
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    if (bank == 16) {
        if (addr >= 0xc000 && addr < 0xe000) {
            return peek_bank_io(addr);
        }
    }
    return mem_bank_read(bank, addr);
}

void mem_bank_write(int bank, ADDRESS addr, BYTE byte)
{
    switch (bank) {
      case 17:                   /* current */
        mem_store(addr, byte);
        return;
      case 16:
        if (addr >= 0xd000 && addr <=0xdfff) {
            store_io(addr, byte);
            return;
        }
        _mem_write_tab[15][addr >> 8](addr, byte);
        return;
      default:
        if (bank >=0 && bank < 16) {
            if (addr & 0xff00) {
                store_ram_tab[bank](addr, byte);
            } else {
                store_zero_tab[bank](addr, byte);
            }
            return;
        }
    }
    store_dummy(addr, byte);
}

mem_ioreg_list_t *mem_ioreg_list_get(void)
{
    mem_ioreg_list_t *mem_ioreg_list;

    mem_ioreg_list = (mem_ioreg_list_t *)xmalloc(sizeof(mem_ioreg_list_t) * 6);

    if (isC500) {
        mem_ioreg_list[0].name = "VIC-II";
        mem_ioreg_list[0].start = 0xd800;
        mem_ioreg_list[0].end = 0xd82e;
        mem_ioreg_list[0].next = &mem_ioreg_list[1];
    } else {
        mem_ioreg_list[0].name = "CRTC";
        mem_ioreg_list[0].start = 0xd800;
        mem_ioreg_list[0].end = 0xd80f;
        mem_ioreg_list[0].next = &mem_ioreg_list[1];
    }

    mem_ioreg_list[1].name = "SID";
    mem_ioreg_list[1].start = 0xda00;
    mem_ioreg_list[1].end = 0xda1f;
    mem_ioreg_list[1].next = &mem_ioreg_list[2];

    mem_ioreg_list[2].name = "CIA1";
    mem_ioreg_list[2].start = 0xdc00;
    mem_ioreg_list[2].end = 0xdc0f;
    mem_ioreg_list[2].next = &mem_ioreg_list[3];

    mem_ioreg_list[3].name = "ACIA1";
    mem_ioreg_list[3].start = 0xdd00;
    mem_ioreg_list[3].end = 0xdd03;
    mem_ioreg_list[3].next = &mem_ioreg_list[4];

    mem_ioreg_list[4].name = "TPI1";
    mem_ioreg_list[4].start = 0xde00;
    mem_ioreg_list[4].end = 0xde07;
    mem_ioreg_list[4].next = &mem_ioreg_list[5];

    mem_ioreg_list[5].name = "TPI2";
    mem_ioreg_list[5].start = 0xdf00;
    mem_ioreg_list[5].end = 0xdf07;
    mem_ioreg_list[5].next = NULL;

    return mem_ioreg_list;
}

void mem_get_screen_parameter(ADDRESS *base, BYTE *rows, BYTE *columns)
{
    /* FIXME */
    *base = 0;
    *rows = 25;
    *columns = 80;
}

