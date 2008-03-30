/*
 * c64mem.c -- C64 memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#ifndef CBM64
#define CBM64                   /* for mkdep */
#endif

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __riscos
#include "ROlib.h"
#else
#include <sys/stat.h>
#include <errno.h>
#endif
#endif

#include "memutils.h"

#include "file.h"
#include "log.h"
#include "resources.h"
#include "sysfile.h"
#include "utils.h"

#include "c64cart.h"
#include "c64cia.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "cartridge.h"
#include "cmdline.h"
#include "drive.h"
#include "emuid.h"
#include "interrupt.h"
#include "log.h"
#include "maincpu.h"
#include "memutils.h"
#include "mon.h"
#include "parallel.h"
#include "patchrom.h"
#include "resources.h"
#include "reu.h"
#include "serial.h"
#include "sid.h"
#include "snapshot.h"
#include "tape.h"
#include "ui.h"
#include "utils.h"
#include "vicii.h"
#include "vmachine.h"

#ifdef HAVE_RS232
#include "c64acia.h"
#endif

#if !defined __MSDOS && ! defined WIN32
#include "c64romset.h"
#endif

#ifdef __riscos
#include "ROlib.h"
#endif

/* ------------------------------------------------------------------------- */

/* C64 memory-related resources.  */

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

/* Type of the cartridge attached.  */
static int mem_cartridge_type = CARTRIDGE_NONE;

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
static int acia_de_enabled;
#endif

static void cartridge_config_changed(BYTE mode);

static char **romsets; 

static int number_romsets; 

static char *romset_name = 0;

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

    if (basic_rom_name != NULL
        && name != NULL
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
    } else if (mem_cartridge_type == CARTRIDGE_NONE) {
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
    if (!(int) v) {
        ieee488_enabled = 0;
        return 0;
    } else if (!reu_enabled && (mem_cartridge_type == CARTRIDGE_NONE)) {
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
    } else if (!ieee488_enabled && (mem_cartridge_type == CARTRIDGE_NONE)) {
        reu_enabled = 1;
        return 0;
    } else {
        /* The REU and the IEEE488 interface share the same address space, so
           they cannot be enabled at the same time.  */
        return -1;
    }
}

/* FIXME: Should patch the ROM on-the-fly.  */
static int set_kernal_revision(resource_value_t v)
{
    const char *rev = (const char *) v;

    string_set(&kernal_revision, rev);
    return 0;
}

#ifdef HAVE_RS232

static int set_acia_de_enabled(resource_value_t v)
{
    acia_de_enabled = (int) v;
    return 0;
}

#endif

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
    { "KernalRev", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) &kernal_revision, set_kernal_revision },
#ifdef HAVE_RS232
    { "AciaDE", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) &acia_de_enabled, set_acia_de_enabled },
#endif
    { NULL }
};

int c64_mem_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* C64 memory-related command-line options.  */
/* FIXME: Maybe the `-kernal', `-basic' and `-chargen' options should not
   really affect resources.  */

static cmdline_option_t cmdline_options[] =
{
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
#endif
    { NULL }
};

int c64_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Number of possible memory configurations.  */
#define NUM_CONFIGS     32
/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      4

/* The C64 memory.  */
#ifdef AVOID_STATIC_ARRAYS
BYTE *ram;
BYTE *basic_rom;
BYTE *kernal_rom;
BYTE *chargen_rom;
#else
BYTE ram[C64_RAM_SIZE];
BYTE basic_rom[C64_BASIC_ROM_SIZE];
BYTE kernal_rom[C64_KERNAL_ROM_SIZE];
BYTE chargen_rom[C64_CHARGEN_ROM_SIZE];
#endif

/* Size of RAM...  */
int ram_size = C64_RAM_SIZE;

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
static int rom_loaded = 0;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;

/* Memory read and write tables.  */
#ifdef AVOID_STATIC_ARRAYS
static store_func_ptr_t (*mem_write_tab)[NUM_CONFIGS][0x101];
static read_func_ptr_t (*mem_read_tab)[0x101];
static BYTE *(*mem_read_base_tab)[0x101];

static store_func_ptr_t *mem_write_tab_watch;
static read_func_ptr_t *mem_read_tab_watch;
#else
static store_func_ptr_t mem_write_tab[NUM_VBANKS][NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];
#endif

/* Processor port.  */
static struct {
    BYTE dir, data;
} pport;

/* Expansion port signals.  */
static struct {
    BYTE exrom;
    BYTE game;
} export;

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

/* Current memory configuration.  */
static int mem_config;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed.  */
static int tape_sense = 0;

/* Exansion port ROML/ROMH images.  */
#ifdef AVOID_STATIC_ARRAYS
BYTE *roml_banks, *romh_banks;
#else
BYTE roml_banks[0x20000], romh_banks[0x20000];
#endif

/* Exansion port RAM images.  */
#ifdef AVOID_STATIC_ARRAYS
BYTE *export_ram0;
#else
BYTE export_ram0[0x2000];
#endif

/* Expansion port ROML/ROMH/RAM banking.  */
int roml_bank = 0, romh_bank = 0, export_ram = 0;

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
int ultimax = 0;

/* Super Snapshot configuration flags.  */
static BYTE ramconfig = 0xff, romconfig = 9;

/* Logging goes here.  */
static log_t c64_mem_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}


void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    mem_write_tab[vbank][mem_config][addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

static inline void pla_config_changed(void)
{
    mem_config = (((~pport.dir | pport.data) & 0x7) | (export.exrom << 3)
                  | (export.game << 4));

    /* Bit 4: tape sense.  0 = some button pressed, 1 = no buttons pressed.  */
    if (tape_sense)
        /* FIXME: When some button is pressed and bit #4 is output and `1'
           is written to this bit, what do we read?  The `1' from the output
           or the `0' from the cassette logic.  This is no CIA, so the
           external `0' may not win!  Only a test with a real C64 can tell.  */
        ram[1] = ((pport.data | ~pport.dir) & 0x2f)
                 | (pport.data & pport.dir & 0xc0);
    else
        ram[1] = ((pport.data | ~pport.dir) & 0x3f)
                 | (pport.data & pport.dir & 0xc0);

    ram[0] = pport.dir;

    if (any_watchpoints(e_comp_space)) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[vbank][mem_config];
    }

    _mem_read_base_tab_ptr = mem_read_base_tab[mem_config];
}

void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[vbank][mem_config];
    }
}

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

BYTE REGPARM1 read_ram(ADDRESS addr)
{
    return ram[addr];
}

void REGPARM2 store_ram(ADDRESS addr, BYTE value)
{
    ram[addr] = value;
}

void REGPARM2 store_ram_hi(ADDRESS addr, BYTE value)
{
    ram[addr] = value;
    if (reu_enabled && addr == 0xff00)
        reu_dma(-1);
}

void REGPARM2 store_io2(ADDRESS addr, BYTE value)
{
    if ((addr & 0xff00) == 0xdf00) {
        if (reu_enabled)
            store_reu(addr & 0x0f, value);
        if (ieee488_enabled)
            store_tpi(addr & 0x07, value);
    }
    if ((mem_cartridge_type == CARTRIDGE_ACTION_REPLAY) && export_ram)
        export_ram0[0x1f00 + (addr & 0xff)] = value;
    if (mem_cartridge_type == CARTRIDGE_KCS_POWER)
        export_ram0[0x1f00 + (addr & 0xff)] = value;
    if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT && (addr & 0xff) == 0) {
        romconfig = (value == 2) ? 1 : 9;
        romconfig = (romconfig & 0xdf) | ((ramconfig == 0) ? 0x20 : 0);
	if ((value & 0x7f) == 0)
	    romconfig = 35;
	if ((value & 0x7f) == 1 || (value & 0x7f) == 3)
	    romconfig = 0;
	if ((value & 0x7f) == 6) {
	    romconfig = 9;
	    cartridge_release_freeze();
	}
	if ((value & 0x7f) == 9)
	    romconfig = 6;
        cartridge_config_changed(romconfig);
    }
    if (mem_cartridge_type == CARTRIDGE_FINAL_III) {
        if ((addr & 0xff) == 0xff)  {
            /* FIXME: Change this to call `cartridge_config_changed'.  */
            romh_bank = roml_bank = value & 3;
            export.game = ((value >> 5) & 1) ^ 1;
            export.exrom = ((value >> 4) & 1) ^ 1;
            pla_config_changed();
            ultimax = export.game & (export.exrom ^ 1);
            if ((value & 0x30) == 0x10)
                maincpu_set_nmi(I_FREEZE, IK_NMI);
            if (value & 0x40)
                cartridge_release_freeze();    
        }
    }
    if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT && (addr & 0xff) == 1) {
        if(((ramconfig - 1) & 0xff) == value) {
            ramconfig = value;
            romconfig |= 35;
        }
        if(((ramconfig + 1) & 0xff) == value) {
            ramconfig = value;
            romconfig &= 0xdd;
        }
        cartridge_config_changed(romconfig);
    }
    if (mem_cartridge_type == CARTRIDGE_SUPER_GAMES) {
printf("VAL %x\n",value);
        romh_bank = roml_bank = value & 3;
        if (value & 0x4) {
            export.game = 0;
            export.exrom = 1;
        } else {
            export.game = export.exrom = 1;
        }
        if (value == 0xc)
            export.game = export.exrom = 0;
        pla_config_changed();
    }
    return;
}

BYTE REGPARM1 read_io2(ADDRESS addr)
{
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

	if (mem_cartridge_type == CARTRIDGE_ACTION_REPLAY
	    || mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT
	    || mem_cartridge_type == CARTRIDGE_FINAL_III) {
	    if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT
	                           && (addr & 0xff) == 1)
		return ramconfig;
            if (export_ram && mem_cartridge_type == CARTRIDGE_ACTION_REPLAY)
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
        if (mem_cartridge_type == CARTRIDGE_KCS_POWER)
            return export_ram0[0x1f00 + (addr & 0xff)];
    }
    return rand();
}

void REGPARM2 store_io1(ADDRESS addr, BYTE value)
{
    if ((addr & 0xff00) == 0xde00) {
        if (mem_cartridge_type == CARTRIDGE_ACTION_REPLAY)
            cartridge_config_changed(value);
        if (mem_cartridge_type == CARTRIDGE_KCS_POWER)
            cartridge_config_changed(1);
        if (mem_cartridge_type == CARTRIDGE_SIMONS_BASIC)
            cartridge_config_changed(1);
        if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT)
            export_ram0[0x1e00 + (addr & 0xff)] = value;
        if (mem_cartridge_type == CARTRIDGE_OCEAN ||
            mem_cartridge_type == CARTRIDGE_FUNPLAY) {
            switch (mem_cartridge_type) {
              case CARTRIDGE_OCEAN:
                romh_bank = roml_bank = value & 15;
                break;
              case CARTRIDGE_FUNPLAY:
                romh_bank = roml_bank = ((value >> 2) | (value & 1)) & 15;
                break;
            }
            if (value & 0x80) {
                export.game = (value >> 4) & 1;
                export.exrom = 1;
            } else {
                export.game = export.exrom = 1;
            }
            pla_config_changed();
            ultimax = 0;
        }            
    }
#ifdef HAVE_RS232
    if (acia_de_enabled)
        store_acia1(addr & 0x03, value);
#endif
    return;
}

BYTE REGPARM1 read_io1(ADDRESS addr)
{
    if ((addr & 0xff00) == 0xde00) {
	if (mem_cartridge_type == CARTRIDGE_ACTION_REPLAY)
	    return rand();
	if (mem_cartridge_type == CARTRIDGE_KCS_POWER) {
	    cartridge_config_changed(0);
	    return roml_banks[0x1e00 + (addr & 0xff)];
	}
        if (mem_cartridge_type == CARTRIDGE_FINAL_III)
            return roml_banks[0x1e00 + (roml_bank << 13) + (addr & 0xff)];
	if (mem_cartridge_type == CARTRIDGE_SIMONS_BASIC)
	    cartridge_config_changed(0);
	if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT)
	    return export_ram0[0x1e00 + (addr & 0xff)];
    }
#ifdef HAVE_RS232
    if (acia_de_enabled)
        return read_acia1(addr & 0x03);
#endif
    return rand();
}

BYTE REGPARM1 read_rom(ADDRESS addr)
{
    switch (addr & 0xf000) {
      case 0xa000:
      case 0xb000:
        return read_basic(addr);
      case 0xd000:
        return read_chargen(addr);
      case 0xe000:
      case 0xf000:
        return read_kernal(addr);
    }

    return 0;
}

void REGPARM2 store_rom(ADDRESS addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0xa000:
      case 0xb000:
        basic_rom[addr & 0x1fff] = value;
        break;
      case 0xd000:
        chargen_rom[addr & 0x0fff] = value;
        break;
      case 0xe000:
      case 0xf000:
        kernal_rom[addr & 0x1fff] = value;
        break;
    }
}

BYTE REGPARM1 read_roml(ADDRESS addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];
#ifdef DEBUG
    printf("RL: ADDR: %i\tVAL: %i\n",(addr & 0x1fff) + (roml_bank << 13),
           roml_banks[(addr & 0x1fff) + (roml_bank << 13)]);
#endif
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

BYTE REGPARM1 read_romh(ADDRESS addr)
{
#ifdef DEBUG
    printf("RH: ADDR: %i\tVAL: %i\n",(addr & 0x1fff) + (romh_bank << 13),
           romh_banks[(addr & 0x1fff) + (romh_bank << 13)]);
#endif
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

void REGPARM2 store_roml(ADDRESS addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
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

static void set_write_hook(int config, int page, store_func_t * f)
{
    int i;

    for (i = 0; i < NUM_VBANKS; i++) {
        mem_write_tab[i][config][page] = f;
    }
}

void initialize_memory(void)
{
    int i, j, k;
    /* IO is enabled at memory configs 5, 6, 7 and Ultimax.  */
    int io_config[32] = { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1 };
    /* ROML is enabled at memory configs 11, 15, 27, 31 and Ultimax.  */
    int roml_config[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
    /* ROMH is enabled at memory configs 10, 11, 14, 15, 26, 27, 30, 31
       and Ultimax.  */
    int romh_config[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
    /* ROMH is mapped to $A000-$BFFF at memory configs 10, 11, 14, 15, 26,
       27, 30, 31.  If Ultimax is enabled it is mapped to $E000-$FFFF.  */
    int romh_mapping[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
                             0x00, 0x00, 0xa0, 0xa0, 0x00, 0x00, 0xa0, 0xa0 };

    if (c64_mem_log != LOG_ERR)
        c64_mem_log = log_open("C64MEM");
        
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
            mem_read_tab[i][j] = read_ram;
            mem_read_base_tab[i][j] = ram + (j << 8);
            for (k = 0; k < NUM_VBANKS; k++) {
                if ((j & 0xc0) == (k << 6)) {
                    switch (j & 0x3fff) {
                      case 0x39:
                        mem_write_tab[k][i][j] = store_vbank_39xx;
                        break;
                      case 0x3f:
                        mem_write_tab[k][i][j] = store_vbank_3fxx;
                        break;
                      default:
                        mem_write_tab[k][i][j] = store_vbank;
                    }
                } else {
                    mem_write_tab[k][i][j] = store_ram;
                }
            }
        }
        mem_read_tab[i][0xff] = read_ram;
        mem_read_base_tab[i][0xff] = ram + 0xff00;

        /* FIXME: we do not care about vbank writes here, but we probably
           should.  Anyway, the $FFxx addresses are not so likely to contain
           sprites or other stuff that really needs the special handling, and
           it's much easier this way.  */
        set_write_hook(i, 0xff, store_ram_hi);
    }

    /* Setup BASIC ROM at $A000-$BFFF (memory configs 3, 7, 11, 15).  */
    for (i = 0xa0; i <= 0xbf; i++) {
        mem_read_tab[3][i] = read_basic;
        mem_read_tab[7][i] = read_basic;
        mem_read_tab[11][i] = read_basic;
        mem_read_tab[15][i] = read_basic;
        mem_read_base_tab[3][i] = basic_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[7][i] = basic_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[11][i] = basic_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[15][i] = basic_rom + ((i & 0x1f) << 8);
    }

    /* Setup character generator ROM at $D000-$DFFF (memory configs 1, 2,
       3, 9, 10, 11, 25, 26, 27).  */
    for (i = 0xd0; i <= 0xdf; i++) {
        mem_read_tab[1][i] = read_chargen;
        mem_read_tab[2][i] = read_chargen;
        mem_read_tab[3][i] = read_chargen;
        mem_read_tab[9][i] = read_chargen;
        mem_read_tab[10][i] = read_chargen;
        mem_read_tab[11][i] = read_chargen;
        mem_read_tab[25][i] = read_chargen;
        mem_read_tab[26][i] = read_chargen;
        mem_read_tab[27][i] = read_chargen;
        mem_read_base_tab[1][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[2][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[3][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[9][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[10][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[11][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[25][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[26][i] = chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[27][i] = chargen_rom + ((i & 0x0f) << 8);
    }

    /* Setup I/O at $D000-$DFFF (memory configs 5, 6, 7).  */
    for (j = 0; j < NUM_CONFIGS; j++) {
        if (io_config[j]) {
            for (i = 0xd0; i <= 0xd3; i++) {
                mem_read_tab[j][i] = read_vic;
                set_write_hook(j, i, store_vic);
            }
            for (i = 0xd4; i <= 0xd5; i++) {
                mem_read_tab[j][i] = read_sid;
                set_write_hook(j, i, store_sid);
            }
            for (i = 0xd6; i <= 0xd7; i++) {
                mem_read_tab[j][i] = read_sid;
                set_write_hook(j, i, store_sid);
            }
            for (i = 0xd8; i <= 0xdb; i++) {
                mem_read_tab[j][i] = read_colorram;
                set_write_hook(j, i, store_colorram);
            }

            mem_read_tab[j][0xdc] = read_cia1;
            set_write_hook(j, 0xdc, store_cia1);
            mem_read_tab[j][0xdd] = read_cia2;
            set_write_hook(j, 0xdd, store_cia2);

            mem_read_tab[j][0xde] = read_io1;
            set_write_hook(j, 0xde, store_io1);
            mem_read_tab[j][0xdf] = read_io2;
            set_write_hook(j, 0xdf, store_io2);

            for (i = 0xd0; i <= 0xdf; i++)
                mem_read_base_tab[j][i] = NULL;
        }
    }

    /* Setup Kernal ROM at $E000-$FFFF (memory configs 2, 3, 6, 7, 10,
       11, 14, 15, 26, 27, 30, 31).  */
    for (i = 0xe0; i <= 0xff; i++) {
        mem_read_tab[2][i] = read_kernal;
        mem_read_tab[3][i] = read_kernal;
        mem_read_tab[6][i] = read_kernal;
        mem_read_tab[7][i] = read_kernal;
        mem_read_tab[10][i] = read_kernal;
        mem_read_tab[11][i] = read_kernal;
        mem_read_tab[14][i] = read_kernal;
        mem_read_tab[15][i] = read_kernal;
        mem_read_tab[26][i] = read_kernal;
        mem_read_tab[27][i] = read_kernal;
        mem_read_tab[30][i] = read_kernal;
        mem_read_tab[31][i] = read_kernal;
        mem_read_base_tab[2][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[3][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[6][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[7][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[10][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[11][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[14][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[15][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[26][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[27][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[30][i] = kernal_rom + ((i & 0x1f) << 8);
        mem_read_base_tab[31][i] = kernal_rom + ((i & 0x1f) << 8);
    }

    /* Setup ROML at $8000-$9FFF.  */
    for (j = 0; j < NUM_CONFIGS; j++) {
        if (roml_config[j]) {
            for (i = 0x80; i <= 0x9f; i++) {
                mem_read_tab[j][i] = read_roml;
                mem_read_base_tab[j][i] = NULL;
            }
        }
    }
    for (j = 16; j < 24; j++)
        for (i = 0x80; i <= 0x9f; i++)
            set_write_hook(j, i, store_roml);

    /* Setup ROMH at $A000-$BFFF and $E000-$FFFF.  */
    for (j = 0; j < NUM_CONFIGS; j++) {
        if (romh_config[j]) {
            for (i = romh_mapping[j]; i <= (romh_mapping[j] + 0x1f); i++) {
                mem_read_tab[j][i] = read_romh;
                mem_read_base_tab[j][i] = NULL;
            }
        }
    }

    for (i = 0; i < NUM_CONFIGS; i++) {
        mem_read_tab[i][0x100] = mem_read_tab[i][0];
        for (j = 0; j < NUM_VBANKS; j++) {
            mem_write_tab[j][i][0x100] = mem_write_tab[j][i][0];
        }
        mem_read_base_tab[i][0x100] = mem_read_base_tab[i][0];
    }

    _mem_read_tab_ptr = mem_read_tab[7];
    _mem_write_tab_ptr = mem_write_tab[vbank][7];
    _mem_read_base_tab_ptr = mem_read_base_tab[7];

    pport.data = 0xff;
    pport.dir = 0x0;
    export.exrom = 0;
    export.game = 0;

    /* Setup initial memory configuration.  */
    pla_config_changed();

    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
      case CARTRIDGE_KCS_POWER:
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_SUPER_GAMES:
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_FINAL_III:
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GENERIC_16KB:
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_ULTIMAX:
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        cartridge_config_changed(9);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        cartridge_config_changed(1);
        store_io1((ADDRESS) 0xde00, 0);
        break;
    }
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    int i;

#ifdef AVOID_STATIC_ARRAYS
    if (!ram)
    {
	ram = xmalloc(C64_RAM_SIZE);
	basic_rom = xmalloc(C64_BASIC_ROM_SIZE);
	kernal_rom = xmalloc(C64_KERNAL_ROM_SIZE);
	chargen_rom = xmalloc(C64_CHARGEN_ROM_SIZE);

	mem_write_tab = xmalloc(NUM_VBANKS*sizeof(*mem_write_tab));
	mem_read_tab = xmalloc(NUM_CONFIGS*sizeof(*mem_read_tab));
	mem_read_base_tab = xmalloc(NUM_CONFIGS*sizeof(*mem_read_base_tab));

	mem_write_tab_watch = xmalloc(0x101*sizeof(*mem_write_tab_watch));
	mem_read_tab_watch = xmalloc(0x101*sizeof(*mem_read_tab_watch));

	roml_banks = xmalloc(0x20000);
	romh_banks = xmalloc(0x20000);
	export_ram0 = xmalloc(0x2000);
    }
#endif

    for (i = 0; i < 0x10000; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }
}

/* Load ROMs at startup.  This is half-stolen from the old `load_mem()' in
   `memory.c'.  */

char* mem_get_romset_name() {
    return(romset_name);
}

char** mem_get_romsets()
{
    return romsets;
} 

int mem_get_numromsets()
{
    return number_romsets;
} 

int mem_add_romset(char *name)
{
    int i;
    if(number_romsets > 20) return 0;
    for(i = 1 ; i < number_romsets ; i ++) {
      if(!strcmp(name,romsets[i])) {
	  return 0;
      }
    }
    romsets[number_romsets++] = stralloc(name);
    log_message(LOG_DEFAULT, "Adding RomSet `%s'.", name);
    return 1;
}

int mem_load_romset_file(const char *name)
{
    FILE *fp = NULL;
    char *complete_path;
    char romsetname[101];
    char *c;
    
    /* Fixme: Only 20 different RomSets at the moment */
    romsets = xcalloc(sizeof(char*), 20);

    romsets[0] = stralloc("Default");
    log_message(LOG_DEFAULT, "Adding Default RomSet");

    number_romsets = 1;

#if !defined __MSDOS && ! defined WIN32
    c64romset_load();
#endif
    return number_romsets;  /* return ok */
}

int mem_romset_loader()
{
    WORD sum;                   /* ROM checksum */
    int id;                     /* ROM identification number */
    int i;

    /* Make sure serial code assumes there are no traps installed.  */
    serial_remove_traps();

    /* Load Kernal ROM.  */
    if (mem_load_sys_file(kernal_rom_name,
                          kernal_rom, C64_KERNAL_ROM_SIZE,
                          C64_KERNAL_ROM_SIZE) < 0) {
        log_error(c64_mem_log, "Couldn't load kernal ROM `%s'.",
                  kernal_rom_name);
        return -1;
    }
    /* Check Kernal ROM.  */
    for (i = 0, sum = 0; i < C64_KERNAL_ROM_SIZE; i++)
        sum += kernal_rom[i];

    id = read_rom(0xff80);

    log_message(c64_mem_log, "Kernal rev #%d.", id);

    if ((id == 0
         && sum != C64_KERNAL_CHECKSUM_R00)
        || (id == 3
            && sum != C64_KERNAL_CHECKSUM_R03
            && sum != C64_KERNAL_CHECKSUM_R03swe)
        || (id == 0x43
            && sum != C64_KERNAL_CHECKSUM_R43)
        || (id == 0x64
            && sum != C64_KERNAL_CHECKSUM_R64)) {
        log_warning(c64_mem_log,
                    "Warning: Unknown Kernal image `%s'.  Sum: %d ($%04X).",
                    kernal_rom_name, sum, sum);
    } else if (kernal_revision != NULL) {
        if (patch_rom(kernal_revision) < 0)
            return -1;
    }

    {
        int drive_true_emulation;
        resources_get_value("DriveTrueEmulation",
                            (resource_value_t) &drive_true_emulation);
        if (!drive_true_emulation)
            serial_install_traps();
    }

    /* Load Basic ROM.  */
    if (mem_load_sys_file(basic_rom_name,
                          basic_rom, C64_BASIC_ROM_SIZE,
                          C64_BASIC_ROM_SIZE) < 0) {
        log_error(c64_mem_log,
                  "Couldn't load basic ROM `%s'.",
                  basic_rom_name);
        return -1;
    }
    /* Check Basic ROM.  */

    for (i = 0, sum = 0; i < C64_BASIC_ROM_SIZE; i++)
        sum += basic_rom[i];

    if (sum != C64_BASIC_CHECKSUM)
        log_warning(c64_mem_log,
                    "Warning: Unknown Basic image `%s'.  Sum: %d ($%04X).",
                    basic_rom_name, sum, sum);

    /* Load chargen ROM.  */

    if (mem_load_sys_file(chargen_rom_name,
                          chargen_rom, C64_CHARGEN_ROM_SIZE,
                          C64_CHARGEN_ROM_SIZE) < 0) {
        log_error(c64_mem_log, "Couldn't load character ROM `%s'.",
                  chargen_rom_name);
        return -1;
    }
    rom_loaded = 1;

    return 0;
}

int mem_set_romset(char *name) {
  int num_romsets;
  char romsetnamebuffer[MAXPATHLEN];
  char *tmppath;

  if(strcmp(name,"Default")) {
    for(num_romsets = 0; num_romsets < number_romsets ; num_romsets++) {
      if (!strcmp(romsets[num_romsets],name)) {
	romset_name = romsets[num_romsets];
	strcpy(romsetnamebuffer,"chargen-");
	strncat(romsetnamebuffer,romset_name,MAXPATHLEN - strlen(romsetnamebuffer) - 1);
	if ( sysfile_locate(romsetnamebuffer, &tmppath) ) {
	  set_chargen_rom_name((resource_value_t) "chargen");
	} else {
	  set_chargen_rom_name((resource_value_t) romsetnamebuffer);
	}

	strcpy(romsetnamebuffer,"kernal-");
	strncat(romsetnamebuffer,romset_name,MAXPATHLEN - strlen(romsetnamebuffer) - 1);
	if ( sysfile_locate(romsetnamebuffer, &tmppath) ) {
	  set_kernal_rom_name((resource_value_t) "kernal");
	} else {
	  set_kernal_rom_name((resource_value_t) romsetnamebuffer);
	}

	strcpy(romsetnamebuffer,"basic-");
	strncat(romsetnamebuffer,romset_name,MAXPATHLEN - strlen(romsetnamebuffer) - 1);
	if ( sysfile_locate(romsetnamebuffer, &tmppath) ) {
	  set_basic_rom_name((resource_value_t) "basic");
	} else {
	  set_basic_rom_name((resource_value_t) romsetnamebuffer);
	}

	strcpy(romsetnamebuffer,"dos1541-");
	strncat(romsetnamebuffer,romset_name,MAXPATHLEN - strlen(romsetnamebuffer) - 1);
	if ( sysfile_locate(romsetnamebuffer, &tmppath) ) {
	  reload_rom_1541("dos1541");
	} else {
	  reload_rom_1541(romsetnamebuffer);
	}

	log_message(LOG_DEFAULT, "Changing to RomSet %s",name);
      }      
    }
  } else {
    romset_name = romsets[0];
    set_chargen_rom_name((resource_value_t) "chargen");
    set_kernal_rom_name((resource_value_t) "kernal");
    set_basic_rom_name((resource_value_t) "basic");    
    reload_rom_1541("dos1541");
    log_message(LOG_DEFAULT, "Changing to Default RomSet");
  }
  return(0);
}

int isRomInRomSet(char *rom, char *romset) 
{
    if(!romset || !rom || strlen(romset) > strlen(rom)) return 0;
    return !strcmp(rom + strlen(rom) - strlen(romset), romset)?1:0;
}

int mem_load(void)
{
    int back,num_romsets;
    char *temp;
    mem_powerup();

    mem_load_romset_file("rom.cfg");
    back = mem_romset_loader();
    for(num_romsets = 1; num_romsets < number_romsets ; num_romsets++) {
        temp = romsets[num_romsets];
	if( isRomInRomSet(chargen_rom_name,temp) ||
	    isRomInRomSet(basic_rom_name,temp) ||
	    isRomInRomSet(kernal_rom_name,temp) ) {
            romset_name = romsets[num_romsets];
	    break;
      }
    }
    if(num_romsets == number_romsets) {
      romset_name = romsets[0];
    }
    return back;
}

void mem_attach_cartridge(int type, BYTE * rawcart)
{
    mem_cartridge_type = type;
    roml_bank = romh_bank = 0;
    switch (type) {
      case CARTRIDGE_GENERIC_8KB:
        memcpy(roml_banks, rawcart, 0x2000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GENERIC_16KB:
        memcpy(roml_banks, rawcart, 0x2000);
        memcpy(romh_banks, &rawcart[0x2000], 0x2000);
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_ACTION_REPLAY:
        memcpy(roml_banks, rawcart, 0x8000);
        memcpy(romh_banks, rawcart, 0x8000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_KCS_POWER:
        memcpy(roml_banks, rawcart, 0x2000);
        memcpy(romh_banks, &rawcart[0x2000], 0x2000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_FINAL_III:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
        memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
        memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
        memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        cartridge_config_changed(9);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        memcpy(roml_banks, rawcart, 0x2000 * 16);
        memcpy(romh_banks, &rawcart[0x2000 * 16], 0x2000 * 16);
        break;
      case CARTRIDGE_ULTIMAX:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x0000], 0x2000);
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_GAMES:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
        memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
        memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
        memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
        cartridge_config_changed(0);
        break;
      default:
        mem_cartridge_type = CARTRIDGE_NONE;
    }
    return;
}

void mem_detach_cartridge(int type)
{
    cartridge_config_changed(6);
    mem_cartridge_type = CARTRIDGE_NONE;
    return;
}

void mem_freeze_cartridge(int type)
{
    if (type == CARTRIDGE_ACTION_REPLAY || type == CARTRIDGE_SUPER_SNAPSHOT)
	cartridge_config_changed(35);
    if (type == CARTRIDGE_KCS_POWER || type == CARTRIDGE_FINAL_III)
	cartridge_config_changed(3);
}

/* ------------------------------------------------------------------------- */

/* Change the current video bank.  Call this routine only when the vbank
   has really changed.  */
void mem_set_vbank(int new_vbank)
{
    vbank = new_vbank;
    _mem_write_tab_ptr = mem_write_tab[new_vbank][mem_config];
    vic_ii_set_vbank(new_vbank);
}

/* Set the tape sense status.  */
void mem_set_tape_sense(int sense)
{
    tape_sense = sense;
    pla_config_changed();
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

static void cartridge_config_changed(BYTE mode)
{
    export.game = mode & 1;
    export.exrom = ((mode >> 1) & 1) ^ 1;
    romh_bank = roml_bank = (mode >> 3) & 3;
    export_ram = (mode >> 5) & 1;
    pla_config_changed();
    if (mode & 0x40)
        cartridge_release_freeze();
    ultimax = export.game & (export.exrom ^ 1);
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
    return addr >= 0xe000 && (mem_config & 0x2);
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

/* FIXME: peek, cartridge support */

static void store_bank_io(ADDRESS addr, BYTE byte)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        store_vic(addr, byte);
        break;
      case 0xd400:
      case 0xd500:
        store_sid(addr, byte);
        break;
      case 0xd600:
      case 0xd700:
        store_sid(addr, byte);
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
        return read_vic(addr);
      case 0xd400:
      case 0xd500:
      case 0xd600:
      case 0xd700:
        return read_sid(addr);
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
        return peek_vic(addr);
      case 0xd400:
      case 0xd500:
      case 0xd600:
      case 0xd700:
        return read_sid(addr);
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

/* ------------------------------------------------------------------------- */

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] =
{
    "default", "cpu", "ram", "rom", "io", "cart", NULL
};

static int banknums[] =
{
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
      case 3:                   /* io */
        if ((addr >= 0xd000) && (addr < 0xe000)) {
            return read_bank_io(addr);
        }
      case 4:                   /* cart */
        if ((addr >= 0x8000) && (addr <= 0x9FFF)) {
            return roml_banks[addr & 0x1fff];
        }
        if ((addr >= 0xA000) && (addr <= 0xBFFF)) {
            return romh_banks[addr & 0x1fff];
        }
      case 2:                   /* rom */
        if ((addr >= 0xA000) && (addr <= 0xBFFF)) {
            return basic_rom[addr & 0x1fff];
        }
        if ((addr >= 0xD000) && (addr <= 0xDfff)) {
            return chargen_rom[addr & 0x0fff];
        }
        if ((addr >= 0xE000) && (addr <= 0xffff)) {
            return kernal_rom[addr & 0x1fff];
        }
      case 1:                   /* ram */
        break;
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
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            store_bank_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0xA000 && addr <= 0xBFFF) {
            return;
        }
        if (addr >= 0xD000 && addr <= 0xDfff) {
            return;
        }
        if (addr >= 0xE000 && addr <= 0xffff) {
            return;
        }
      case 1:                   /* ram */
        break;
    }
    ram[addr] = byte;
}

/* ------------------------------------------------------------------------- */

/* Snapshot.  */

#define SNAP_ROM_MAJOR 0
#define SNAP_ROM_MINOR 0
static const char snap_rom_module_name[] = "C64ROM";

static int mem_write_rom_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    /* Main memory module.  */

    m = snapshot_module_create(s, snap_rom_module_name, 
					SNAP_ROM_MAJOR, SNAP_ROM_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_byte_array(m, kernal_rom, 
						C64_KERNAL_ROM_SIZE) < 0
        || snapshot_module_write_byte_array(m, basic_rom, 
						C64_BASIC_ROM_SIZE) < 0
        || snapshot_module_write_byte_array(m, chargen_rom, 
						C64_CHARGEN_ROM_SIZE) < 0
        )
	goto fail;

    /* FIXME: save cartridge ROM (& RAM?) areas:
       first write out the configuration, i.e. 
       - type of cartridge (banking scheme type)
       - state of cartridge (active/which bank, ...)
       then the ROM/RAM arrays:
       - cartridge ROM areas
       - cartridge RAM areas  */

    ui_update_menus();

    if (snapshot_module_close(m) < 0)
        goto fail;

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}
 
int mem_read_rom_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;

    /* Main memory module.  */

    m = snapshot_module_open(s, snap_rom_module_name,
                             &major_version, &minor_version);
    if (m == NULL) {
	/* this module is optional */
	/* FIXME: reset all cartridge stuff to standard C64 behaviour */
        return 0;
    }

    if (major_version > SNAP_ROM_MAJOR || minor_version > SNAP_ROM_MINOR) {
        log_error(c64_mem_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_ROM_MAJOR, SNAP_ROM_MINOR);
        goto fail;
    }

    if (snapshot_module_read_byte_array(m, kernal_rom, 
						C64_KERNAL_ROM_SIZE) < 0
        || snapshot_module_read_byte_array(m, basic_rom, 
						C64_BASIC_ROM_SIZE) < 0
        || snapshot_module_read_byte_array(m, chargen_rom, 
						C64_CHARGEN_ROM_SIZE) < 0
        )
	goto fail;

    /* FIXME: read cartridge ROM (& RAM?) areas:
       first read out the configuration, i.e. 
       - type of cartridge (banking scheme type)
       - state of cartridge (active/which bank, ...)
       then the ROM/RAM arrays:
       - cartridge ROM areas
       - cartridge RAM areas
    */

    if (snapshot_module_close(m) < 0)
        goto fail;

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}


#define SNAP_MAJOR 0
#define SNAP_MINOR 0
static const char snap_mem_module_name[] = "C64MEM";

int mem_write_snapshot_module(snapshot_t *s, int save_roms)
{
    snapshot_module_t *m;

    /* Main memory module.  */

    m = snapshot_module_create(s, snap_mem_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_byte(m, pport.data) < 0
        || snapshot_module_write_byte(m, pport.dir) < 0
        || snapshot_module_write_byte(m, export.exrom) < 0
        || snapshot_module_write_byte(m, export.game) < 0
        || snapshot_module_write_byte_array(m, ram, C64_RAM_SIZE) < 0)
        goto fail;

    if (snapshot_module_close(m) < 0)
        goto fail;
    m = NULL;

    if (save_roms && mem_write_rom_snapshot_module(s) < 0)
	goto fail;
	
    /* REU module.  */
    if (reu_enabled && reu_write_snapshot_module(s) < 0)
        goto fail;

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

    /* Main memory module.  */

    m = snapshot_module_open(s, snap_mem_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        log_error(c64_mem_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (snapshot_module_read_byte(m, &pport.data) < 0
        || snapshot_module_read_byte(m, &pport.dir) < 0
        || snapshot_module_read_byte(m, &export.exrom) < 0
        || snapshot_module_read_byte(m, &export.game) < 0
        || snapshot_module_read_byte_array(m, ram, C64_RAM_SIZE) < 0)
        goto fail;

    pla_config_changed();

    if (snapshot_module_close(m) < 0)
        goto fail;
    m = NULL;

    if (mem_read_rom_snapshot_module(s) < 0)
	goto fail;

    /* REU module.  */
    if (reu_read_snapshot_module(s) < 0) {
        reu_enabled = 0;
    } else {
        reu_enabled = 1;
    }

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
