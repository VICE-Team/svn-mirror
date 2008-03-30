/*
 * c128mem.c -- Memory handling for the C128 emulator.
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

/* TODO:
   - cartridge support;
   - maybe C64 mode?  */

#include "vice.h"

#include <stdio.h>

#include "vmachine.h"
#include "drive.h"
#include "c128mem.h"
#include "resources.h"
#include "cmdline.h"
#include "c64cia.h"
#include "sid.h"
#include "memutils.h"
#include "maincpu.h"
#include "parallel.h"
#include "c64tpi.h"
#include "tapeunit.h"
#include "mon.h"
#include "utils.h"
#include "vdc.h"
#include "emuid.h"
#include "rs232.h"
#include "snapshot.h"
#include "../c64/vicii.h"

#ifdef HAVE_RS232
#include "c64acia.h"
#endif

/* #define DEBUG_MMU */

#ifdef DEBUG_MMU
#define DEBUG(x) printf x
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------- */

/* C128 memory-related resources.  */

/* Name of the character ROM.  */
static char *chargen_rom_name;

/* Name of the BASIC ROM.  */
static char *basic_rom_name;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name;

/* Flag: Do we enable the Emulator ID?  */
static int emu_id_enabled;

/* Flag: Do we enable the IEEE488 interface emulation?  */
static int ieee488_enabled;

/* Flag: Do we enable the external REU?  */
static int reu_enabled;

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
static int acia_de_enabled;

/* Flag: Do we enable the $D7** ACIA RS232 interface emulation?  */
static int acia_d7_enabled;
#endif

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
    if (!(int) v) {
        emu_id_enabled = 0;
        return 0;
    } else {
        emu_id_enabled = 1;
        return 0;
    }
}

static int set_ieee488_enabled(resource_value_t v)
{
    if (!(int) v) {
        ieee488_enabled = 0;
        return 0;
    } else if (!reu_enabled) {
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
    if (!(int) v) {
        reu_enabled = 0;
        return 0;
    } else if (!ieee488_enabled) {
        reu_enabled = 1;
        return 0;
    } else {
        /* The REU and the IEEE488 interface share the same address space, so
           they cannot be enabled at the same time.  */
        return -1;
    }
}

#ifdef HAVE_RS232
static int set_acia_d7_enabled(resource_value_t v)
{
    acia_d7_enabled = (int) v;
    return 0;
}

static int set_acia_de_enabled(resource_value_t v)
{
    acia_de_enabled = (int) v;
    return 0;
}
#endif

static resource_t resources[] =
{
    { "ChargenName", RES_STRING, (resource_value_t) "chargen",
     (resource_value_t *) & chargen_rom_name, set_chargen_rom_name },
    { "KernalName", RES_STRING, (resource_value_t) "kernal",
     (resource_value_t *) & kernal_rom_name, set_kernal_rom_name },
    { "BasicName", RES_STRING, (resource_value_t) "basic",
     (resource_value_t *) & basic_rom_name, set_basic_rom_name },
    { "REU", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & reu_enabled, set_reu_enabled },
    { "IEEE488", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & ieee488_enabled, set_ieee488_enabled },
    { "EmuID", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & emu_id_enabled, set_emu_id_enabled },
#ifdef HAVE_RS232
    { "AciaDE", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & acia_de_enabled, set_acia_de_enabled },
    { "AciaD7", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & acia_d7_enabled, set_acia_d7_enabled },
#endif
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
#ifdef HAVE_RS232
    { "-acia1", SET_RESOURCE, 0, NULL, NULL, "AciaDE", (resource_value_t) 1,
      NULL, "Enable the $DE** ACIA RS232 interface emulation" },
    { "+acia1", SET_RESOURCE, 0, NULL, NULL, "AciaDE", (resource_value_t) 0,
      NULL, "Disable the $DE** ACIA RS232 interface emulation" },
#if 0
    { "-acia2", SET_RESOURCE, 0, NULL, NULL, "AciaD7", (resource_value_t) 1,
      NULL, "Enable the $D7** ACIA RS232 interface emulation" },
    { "+acia2", SET_RESOURCE, 0, NULL, NULL, "AciaD7", (resource_value_t) 0,
      NULL, "Disable the $D7** ACIA RS232 interface emulation" },
#endif
#endif
    { NULL }
};

int c128_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      4

/* The C128 memory.  */
BYTE ram[C128_RAM_SIZE];
BYTE basic_rom[C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE];
BYTE kernal_rom[C128_KERNAL_ROM_SIZE];
BYTE chargen_rom[C128_CHARGEN_ROM_SIZE];

/* Size of RAM...  */
int ram_size = C128_RAM_SIZE;

/* Currently selected RAM bank.  */
static BYTE *ram_bank;

/* Memory configuration.  */
static int chargen_in;
static int basic_lo_in;
static int basic_hi_in;
static int kernal_in;
static int editor_in;
static int io_in;

/* Shared memory.  */
static ADDRESS top_shared_limit, bottom_shared_limit;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed.  */
static int tape_sense = 0;

/* Pointers to pages 0 and 1 (which can be physically placed anywhere).  */
BYTE *page_zero, *page_one;

static BYTE mmu[11];

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
int rom_loaded = 0;

/* Memory read and write tables.  These are non-static to allow the CPU code
   to access them.  */
store_func_ptr_t _mem_write_tab[0x101];
read_func_ptr_t _mem_read_tab[0x101];
store_func_ptr_t *_mem_write_tab_ptr = _mem_write_tab;
read_func_ptr_t *_mem_read_tab_ptr = _mem_read_tab;

/* Fake BIOS initialization.  This is required because the real C128 is
   normally booted by the Z80, which we currently don't emulate at all.  */
static BYTE biostab[] = {
    0x78, 0xa9, 0x3e, 0x8d, 0x00, 0xff, 0xa9, 0xb0, 0x8d, 0x05,
    0xd5, 0xea, 0x58, 0x60, 0x00, 0x00, 0xf3, 0x3e, 0x3e, 0x32,
    0x00, 0xff, 0x01, 0x05, 0xd5, 0x3e, 0xb1, 0xed, 0x79, 0x00,
    0xcf
};

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

/* ------------------------------------------------------------------------- */

/* MMU Implementation.  */

inline BYTE REGPARM1 read_mmu(ADDRESS addr)
{
    addr &= 0xff;

    if (addr < 0xb) {
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

inline void REGPARM2 store_mmu(ADDRESS address, BYTE value)
{
    address &= 0xff;

    if (address < 0xb) {
        mmu[address] = value;

        switch (address) {
          case 0:
            /* Configuration register (CR).  */
            {
                io_in = !(value & 0x1);
                basic_lo_in = !(value & 0x2);
                basic_hi_in = !(value & 0xc);
                kernal_in = chargen_in = editor_in = !(value & 0x30);
                /* (We handle only 128K here.)  */
                ram_bank = ram + (((long) value & 0x40) << 10);
                DEBUG(("MMU: Store CR = $%02x, PC = $%04X\n", value, reg_pc));
                DEBUG(("MMU: RAM bank at $%05X\n", ram_bank - ram));
            }
            break;

          case 6:
            /* RAM configuration register (RCR).  */
            {
                int shared_size;

                /* XXX: We only support 128K here.  */
                vic_ii_set_ram_base(ram + ((value & 0x40) << 10));

                DEBUG(("MMU: Store RCR = $%02x\n", value));
                DEBUG(("MMU: VIC-II base at $%05X\n", ((value & 0xc0) << 2)));

                if ((value & 0x3) == 0)
                    shared_size = 1024;
                else
                    shared_size = 0x1000 << ((value & 0x3) - 1);

                /* Share high memory?  */
                if (value & 0x8) {
                    top_shared_limit = 0xffff - shared_size;
                    DEBUG(("MMU: Sharing high RAM from $%04X\n",
                           top_shared_limit + 1));
                } else {
                    top_shared_limit = 0xffff;
                    DEBUG(("MMU: No high shared RAM\n"));
                }

                /* Share low memory?  */
                if (value & 0x4) {
                    bottom_shared_limit = shared_size;
                    DEBUG(("MMU: Sharing low RAM up to $%04X\n",
                           bottom_shared_limit - 1));
                } else {
                    bottom_shared_limit = 0;
                    DEBUG(("MMU: No low shared RAM\n"));
                }
            }
            break;

          case 5:
            value = (value & 0x7f) | 0x30;
            if ((value & 0x41) != 0x01)
                fprintf(stderr,
                        "MMU: Attempted accessing unimplemented mode: $D505 <- $%02X.\n",
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
            DEBUG(("MMU: Page Zero at $%05X, Page One at $%05X\n",
                   page_zero - ram, page_one - ram));
            break;
        }
    }
}

/* ------------------------------------------------------------------------- */

/* External memory access functions.  */

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
        break;
      case 0x4000:
      case 0x5000:
      case 0x6000:
      case 0x7000:
      case 0x8000:
      case 0x9000:
      case 0xa000:
      case 0xb000:
        store_basic(addr, value);
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
    return _mem_read_tab[addr >> 8](addr);
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

#define READ_TOP_SHARED(addr)                           \
    ((addr) > top_shared_limit ? ram[(addr)]            \
                                : ram_bank[(addr)])

#define STORE_TOP_SHARED(addr, value)                           \
    ((addr) > top_shared_limit ? (ram[(addr)] = (value))        \
                               : (ram_bank[(addr)] = (value)))

#define READ_BOTTOM_SHARED(addr)                        \
    ((addr) < bottom_shared_limit ? ram[(addr)]         \
                                  : ram_bank[(addr)])

#define STORE_BOTTOM_SHARED(addr, value)                                \
    ((addr) < bottom_shared_limit ? (ram[(addr)] = (value))             \
                                  : (ram_bank[(addr)] = (value)))

BYTE REGPARM1 read_zero(ADDRESS addr)
{
    return page_zero[addr];
}

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    page_zero[addr] = value;
}

static BYTE REGPARM1 read_one(ADDRESS addr)
{
    return page_one[addr - 0x100];
}

static void REGPARM2 store_one(ADDRESS addr, BYTE value)
{
    page_one[addr - 0x100] = value;
}


/* $0200 - $3FFF: RAM (normal or shared).  */

static BYTE REGPARM1 read_lo(ADDRESS addr)
{
    return READ_BOTTOM_SHARED(addr);
}

static void REGPARM2 store_lo(ADDRESS addr, BYTE value)
{
    STORE_BOTTOM_SHARED(addr, value);
}


/* $4000 - $7FFF: RAM or low BASIC ROM.  */

static BYTE REGPARM1 read_basic_lo(ADDRESS addr)
{
    if (basic_lo_in)
        return basic_rom[addr - 0x4000];
    else
        return ram_bank[addr];
}

static void REGPARM2 store_basic_lo(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $8000 - $BFFF: RAM or high BASIC ROM.  */

static BYTE REGPARM1 read_basic_hi(ADDRESS addr)
{
    if (basic_hi_in) {
        return basic_rom[addr - 0x4000];
    } else
        return ram_bank[addr];
}

static void REGPARM2 store_basic_hi(ADDRESS addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $C000 - $CFFF: RAM (normal or shared) or Editor ROM.  */

static BYTE REGPARM1 read_editor(ADDRESS addr)
{
    if (editor_in)
        return basic_rom[addr - 0x4000];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_editor(ADDRESS addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d0xx(ADDRESS addr)
{
    if (io_in)
        return read_vic(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d0xx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_vic(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d4xx(ADDRESS addr)
{
    if (io_in)
        return read_sid(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d4xx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_sid(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d5xx(ADDRESS addr)
{
    if (io_in)
        return read_mmu(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d5xx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_mmu(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d6xx(ADDRESS addr)
{
    if (io_in)
        return read_vdc(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d6xx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_vdc(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d7xx(ADDRESS addr)
{
    if (io_in) {
#if 0                           /*def HAVE_RS232 */
        if (acia_d7_enabled)
            return read_acia2(addr);
        else
#endif
            return 0xff;
    } else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d7xx(ADDRESS addr, BYTE value)
{
    if (io_in) {
#if 0                           /*def HAVE_RS232 */
        if (acia_d7_enabled) {
            store_acia2(addr, value);
        }
#endif
    } else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_d8xx(ADDRESS addr)
{
    if (io_in)
        return read_colorram(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_d8xx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_colorram(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_dcxx(ADDRESS addr)
{
    if (io_in)
        return read_cia1(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_dcxx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_cia1(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_ddxx(ADDRESS addr)
{
    if (io_in)
        return read_cia2(addr);
    else if (chargen_in)
        return chargen_rom[addr];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_ddxx(ADDRESS addr, BYTE value)
{
    if (io_in)
        store_cia2(addr, value);
    else
        STORE_TOP_SHARED(addr, value);
}

/* $E000 - $FFFF: RAM or Kernal.  */
static BYTE REGPARM1 read_hi(ADDRESS addr)
{
    if (kernal_in)
        return kernal_rom[addr & 0x1fff];
    else
        return READ_TOP_SHARED(addr);
}

/* $E000 - $FEFF: RAM or Kernal.  */
static void REGPARM2 store_hi(ADDRESS addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}


/* $FF00 - $FFFF: RAM or Kernal, with MMU at $FF00 - $FF04.  */

static BYTE REGPARM1 read_ffxx(ADDRESS addr)
{
    if (addr == 0xff00)
        return mmu[0];
    else if (kernal_in)
        return kernal_rom[addr & 0x1fff];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_ffxx(ADDRESS addr, BYTE value)
{
    if (addr == 0xff00)
        store_mmu(0, value);
    else if (addr <= 0xff04)
        store_mmu(0, mmu[addr & 0xf]);
    else
        STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 read_empty_io(ADDRESS addr)
{
    if (io_in)
        return 0xff;
    else if (chargen_in)
        return chargen_rom[addr - 0xd000];
    else
        return READ_TOP_SHARED(addr);
}

static void REGPARM2 store_empty_io(ADDRESS addr, BYTE value)
{
    if (!io_in)
        STORE_TOP_SHARED(addr, value);
}

/* ------------------------------------------------------------------------- */
/* those are approximate copies from the c64 versions....
 * they leave out the cartridge support
 */

void REGPARM2 store_io1(ADDRESS addr, BYTE value)
{
    if (!io_in) {
        STORE_TOP_SHARED(addr, value);
    } else {
#ifdef HAVE_RS232
        if (acia_de_enabled)
            store_acia1(addr & 0x03, value);
#endif
    }
    return;
}

BYTE REGPARM1 read_io1(ADDRESS addr)
{
    if (io_in) {
#ifdef HAVE_RS232
        if (acia_de_enabled)
            return read_acia1(addr & 0x03);
#endif
        return 0xff;            /* rand(); - C64 has rand(), which is correct? */
    } else if (chargen_in)
        return chargen_rom[addr - 0xd000];
    else
        return READ_TOP_SHARED(addr);
}

void REGPARM2 store_io2(ADDRESS addr, BYTE value)
{
    if (!io_in) {
        STORE_TOP_SHARED(addr, value);
    } else {
        if (ieee488_enabled) {
            store_tpi(addr & 0x07, value);
        }
    }
    return;
}

BYTE REGPARM1 read_io2(ADDRESS addr)
{
    if (io_in) {
        if (emu_id_enabled && addr >= 0xdfa0) {
            addr &= 0xff;
            if (addr == 0xff)
                emulator_id[addr - 0xa0] ^= 0xff;
            return emulator_id[addr - 0xa0];
        }
        if (ieee488_enabled) {
            return read_tpi(addr & 0x07);
        }
        return 0xff;  /* rand(); - C64 has rand(), which is correct? */
    } else if (chargen_in)
        return chargen_rom[addr - 0xd000];
    else
        return READ_TOP_SHARED(addr);
}

/* ------------------------------------------------------------------------- */

void initialize_memory(void)
{
    int i;

    _mem_read_tab[0] = read_zero;
    _mem_write_tab[0] = store_zero;
    _mem_read_tab[1] = read_one;
    _mem_write_tab[1] = store_one;

    for (i = 0x02; i <= 0x3f; i++) {
        _mem_read_tab[i] = read_lo;
        _mem_write_tab[i] = store_lo;
    }

    for (i = 0x40; i <= 0x7f; i++) {
        _mem_read_tab[i] = read_basic_lo;
        _mem_write_tab[i] = store_basic_lo;
    }

    for (i = 0x80; i <= 0xbf; i++) {
        _mem_read_tab[i] = read_basic_hi;
        _mem_write_tab[i] = store_basic_hi;
    }

    for (i = 0xc0; i <= 0xcf; i++) {
        _mem_read_tab[i] = read_editor;
        _mem_write_tab[i] = store_editor;
    }

    for (i = 0xd0; i <= 0xd3; i++) {
        _mem_read_tab[i] = read_d0xx;
        _mem_write_tab[i] = store_d0xx;
    }

    _mem_read_tab[0xd4] = read_d4xx;
    _mem_write_tab[0xd4] = store_d4xx;
    _mem_read_tab[0xd5] = read_d5xx;
    _mem_write_tab[0xd5] = store_d5xx;
    _mem_read_tab[0xd6] = read_d6xx;
    _mem_write_tab[0xd6] = store_d6xx;

    _mem_read_tab[0xd7] = read_d7xx;    /* read_empty_io; */
    _mem_write_tab[0xd7] = store_d7xx;  /* store_empty_io; */

    _mem_read_tab[0xd8] = _mem_read_tab[0xd9] = read_d8xx;
    _mem_read_tab[0xda] = _mem_read_tab[0xdb] = read_d8xx;
    _mem_write_tab[0xd8] = _mem_write_tab[0xd9] = store_d8xx;
    _mem_write_tab[0xda] = _mem_write_tab[0xdb] = store_d8xx;

    _mem_read_tab[0xdc] = read_dcxx;
    _mem_write_tab[0xdc] = store_dcxx;
    _mem_read_tab[0xdd] = read_ddxx;
    _mem_write_tab[0xdd] = store_ddxx;

    _mem_read_tab[0xde] = read_io1;
    _mem_write_tab[0xde] = store_io1;

    _mem_read_tab[0xdf] = read_io2;
    _mem_write_tab[0xdf] = store_io2;

    for (i = 0xe0; i <= 0xfe; i++) {
        _mem_read_tab[i] = read_hi;
        _mem_write_tab[i] = store_hi;
    }

    _mem_read_tab[0xff] = read_ffxx;
    _mem_write_tab[0xff] = store_ffxx;

    _mem_read_tab[0x100] = _mem_read_tab[0x0];
    _mem_write_tab[0x100] = _mem_write_tab[0x0];

    /* FIXME?  Is this the real configuration?  */
    basic_lo_in = basic_hi_in = kernal_in = editor_in = 1;
    io_in = 1;
    chargen_in = 0;
    top_shared_limit = 0xffff;
    bottom_shared_limit = 0x0000;
    ram_bank = ram;
    page_zero = ram;
    page_one = ram + 0x100;
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
    WORD sum;                   /* ROM checksum */
    int id;                     /* ROM identification number */
    int i;

    mem_powerup();

    page_zero = ram;
    page_one = ram + 0x100;

    initialize_memory();

    /* Load Kernal ROM.  */
    if (mem_load_sys_file(kernal_rom_name,
                          kernal_rom, C128_KERNAL_ROM_SIZE,
                          C128_KERNAL_ROM_SIZE) < 0) {
        fprintf(stderr, "Couldn't load kernal ROM `%s'.\n",
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
        fprintf(stderr, "Warning: Kernal image may be corrupted. Sum: %d\n",
                sum);

    /* Load Basic ROM.  */
    if (mem_load_sys_file(basic_rom_name,
                   basic_rom, C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE,
                       C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE) < 0) {
        fprintf(stderr, "Couldn't load basic ROM `%s'.\n",
                basic_rom_name);
        return -1;
    }
    /* Check Basic ROM.  */
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
        fprintf(stderr, "Warning: EDITOR image may be corrupted. Sum: %d\n",
                sum);
        fprintf(stderr, "Check your Basic ROM\n");
    }
    /* Load chargen ROM.  */
    if (mem_load_sys_file(chargen_rom_name,
                          chargen_rom, C128_CHARGEN_ROM_SIZE,
                          C128_CHARGEN_ROM_SIZE) < 0) {
        fprintf(stderr, "Couldn't load character ROM `%s'.\n",
                chargen_rom_name);
        return -1;
    }
    /* Fake BIOS initialization.  This is needed because the real C128 is
       initialized by the Z80, which we currently do not implement.  */
    memcpy(ram + 0xffd0, biostab, sizeof(biostab));

    rom_loaded = 1;
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Change the current video bank.  */
void mem_set_vbank(int new_vbank)
{
    if (new_vbank != vbank) {
        vbank = new_vbank;
        vic_ii_set_vbank(new_vbank);
    }
}

void mem_toggle_watchpoints(int flag)
{
    /* FIXME: Still to do.  */
}

/* Set the tape sense status.  */
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

void mem_get_basic_text(ADDRESS * start, ADDRESS * end)
{
    if (start != NULL)
        *start = ram[0x2b] | (ram[0x2c] << 8);
    if (end != NULL)
        *end = ram[0x1210] | (ram[0x1211] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    ram[0x2b] = ram[0xac] = start & 0xff;
    ram[0x2c] = ram[0xad] = start >> 8;
    ram[0x1210] = end & 0xff;
    ram[0x1211] = end >> 8;
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
        store_d0xx(addr, byte);
        break;
      case 0xd400:
        store_d4xx(addr, byte);
        break;
      case 0xd500:
        store_d5xx(addr, byte);
        break;
      case 0xd600:
        store_d6xx(addr, byte);
        break;
      case 0xd700:
        store_d7xx(addr, byte);
        break;
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        store_colorram(addr, byte);
        break;
      case 0xdc00:
        store_cia1(addr, byte);
        break;
      case 0xdd00:
        store_cia2(addr, byte);
        break;
      case 0xde00:
        store_io1(addr, byte);
        break;
      case 0xdf00:
        store_io2(addr, byte);
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
        return read_d0xx(addr);
      case 0xd400:
        return read_d4xx(addr);
      case 0xd500:
        return read_d5xx(addr);
      case 0xd600:
        return read_d6xx(addr);
      case 0xd700:
        return read_d7xx(addr);
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return read_colorram(addr);
      case 0xdc00:
        return read_cia1(addr);
      case 0xdd00:
        return read_cia2(addr);
      case 0xde00:
        return read_io1(addr);
      case 0xdf00:
        return read_io2(addr);
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
        return read_d0xx(addr); /* FIXME */
      case 0xd400:
        return read_d4xx(addr); /* FIXME */
      case 0xd500:
        return read_d5xx(addr); /* FIXME */
      case 0xd600:
        return read_d6xx(addr); /* FIXME */
      case 0xd700:
        return read_d7xx(addr); /* FIXME */
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return read_colorram(addr);
      case 0xdc00:
        return peek_cia1(addr);
      case 0xdd00:
        return peek_cia2(addr);
      case 0xde00:
        return read_io1(addr);  /* FIXME */
      case 0xdf00:
        return read_io2(addr);  /* FIXME */
    }
    return 0xff;
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "io", "ram1", NULL
};

static int banknums[] = {
    1, 0, 1, 2, 3, 4
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
        return ram[addr + 0x10000];
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return read_bank_io(addr);
        }
      case 2:                   /* rom */
        if (addr >= 0x4000 && addr <= 0xCFFF) {
            return basic_rom[addr - 0x4000];
        }
        if (addr >= 0xD000 && addr <= 0xDFFF) {
            return chargen_rom[addr & 0x0fff];
        }
        if (addr >= 0xE000 && addr <= 0xFFFF) {
            return kernal_rom[addr & 0x1fff];
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
        ram[addr + 0x10000] = byte;
        return;
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            store_bank_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0x4000 && addr <= 0xCFFF) {
            return;
        }
        if (addr >= 0xE000 && addr <= 0xffff) {
            return;
        }
      case 1:                   /* ram */
    }
    ram[addr] = byte;
}


/* ------------------------------------------------------------------------- */

/* Snapshot.  */
/* FIXME: Do we want to make a snapshot of all the ROMs too?  */

static char snap_module_name[] = "C128MEM";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int mem_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;
    int i;

    /* Main memory module.  */

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    /* assuming no side-effects */
    for (i=0; i<11; i++) {
	if ( snapshot_module_write_byte(m, read_mmu(i)) < 0)
	    goto fail;
    }

    if (0
        || snapshot_module_write_byte_array(m, ram, C128_RAM_SIZE) < 0)
        goto fail;

    if (snapshot_module_close(m) < 0)
        goto fail;

    /* REU module.  */
/*
    if (reu_enabled && reu_write_snapshot_module(s) < 0)
        goto fail;
*/
    /* IEEE 488 module.  */
    if (ieee488_enabled && tpi_write_snapshot_module(s) < 0)
        goto fail;

#ifdef HAVE_RS232
    /* ACIA module.  */
    if (acia_de_enabled && acia1_write_snapshot_module(s) < 0)
        goto fail;
#endif

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int mem_read_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    int i;
    BYTE byte;

    /* Main memory module.  */

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        fprintf(stderr,
                "MEM: Snapshot module version (%d.%d) newer than %d.%d.\n",
                major_version, minor_version,
                SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    for (i=0; i<11; i++) {
	if ( snapshot_module_read_byte(m, &byte) < 0)
	    goto fail;
	store_mmu(i, byte);	/* Assuming no side-effects */
    }

    if (0
        || snapshot_module_read_byte_array(m, ram, C128_RAM_SIZE) < 0)
        goto fail;

    /* pla_config_changed(); */

    if (snapshot_module_close(m) < 0)
        goto fail;

    /* REU module.  */
/*
    if (reu_read_snapshot_module(s) < 0) {
        reu_enabled = 0;
    } else {
        reu_enabled = 1;
    }
*/

    /* IEEE488 module.  */
    if (tpi_read_snapshot_module(s) < 0) {
        ieee488_enabled = 0;
    } else {
        /* FIXME: Why do we need to do so???  */
        reset_tpi();
        ieee488_enabled = 1;
    }

#ifdef HAVE_RS232
    /* ACIA module.  */
    if (acia1_read_snapshot_module(s) < 0) {
        acia_de_enabled = 0;
    } else {
        /* FIXME: Why do we need to do so???  */
        reset_acia1();          /* Clear interrupts.  */
        acia_de_enabled = 1;
    }
#endif

    ui_update_menus();

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}
