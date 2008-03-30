/*
 * c64-resources.c
 *
 * Written by
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

#include "c64-resources.h"
#include "c64cart.h"
#include "c64rom.h"
#include "cartridge.h"
#include "drive.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "reu.h"
#include "georam.h"
#include "sid-resources.h"
#include "util.h"


#define KBD_INDEX_C64_SYM   0
#define KBD_INDEX_C64_SYMDE 1
#define KBD_INDEX_C64_POS   2


static int romset_firmware[3];

/* What sync factor between the CPU and the drive?  If equal to
   `MACHINE_SYNC_PAL', the same as PAL machines.  If equal to
   `MACHINE_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as 65536 * drive_clk / clk_[main machine] */
static int sync_factor;

/* Name of the character ROM.  */
static char *chargen_rom_name = NULL;

/* Name of the BASIC ROM.  */
static char *basic_rom_name = NULL;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name = NULL;

/* Kernal revision for ROM patcher.  */
char *kernal_revision = NULL;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
int acia_de_enabled;
#endif

static int set_chargen_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_rom_name, (const char *)v))
        return 0;

    return c64rom_load_chargen(chargen_rom_name);
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_rom_name, (const char *)v))
        return 0;

    return c64rom_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basic_rom_name, (const char *)v))
        return 0;

    return c64rom_load_basic(basic_rom_name);
}

static int set_emu_id_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
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

/* FIXME: Should patch the ROM on-the-fly.  */
static int set_kernal_revision(resource_value_t v, void *param)
{
    util_string_set(&kernal_revision, (const char *)v);
    return 0;
}

#ifdef HAVE_RS232

static int set_acia_de_enabled(resource_value_t v, void *param)
{
    acia_de_enabled = (int)v;
    return 0;
}

#endif

static int set_sync_factor(resource_value_t v, void *param)
{
    int change_timing = 0;

    if (sync_factor != (int)v)
        change_timing = 1;

    switch ((int)v) {
      case MACHINE_SYNC_PAL:
        sync_factor = (int)v;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_PAL);
        break;
      case MACHINE_SYNC_NTSC:
        sync_factor = (int)v;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSC);
        break;
      case MACHINE_SYNC_NTSCOLD:
        sync_factor = (int)v;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSCOLD);
        break;
      default:
        return -1;
    }

    return 0;
}

static int set_romset_firmware(resource_value_t v, void *param)
{
    unsigned int num = (unsigned int)param;

    romset_firmware[num] = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      (void *)&sync_factor, set_sync_factor, NULL },
    { "ChargenName", RES_STRING, (resource_value_t)"chargen",
      (void *)&chargen_rom_name, set_chargen_rom_name, NULL },
    { "RomsetChargenName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[0], set_romset_firmware, (void *)0 },
    { "KernalName", RES_STRING, (resource_value_t)"kernal",
      (void *)&kernal_rom_name, set_kernal_rom_name, NULL },
    { "RomsetKernalName", RES_INTEGER, (resource_value_t)1,
      (void *)&romset_firmware[1], set_romset_firmware, (void *)1 },
    { "BasicName", RES_STRING, (resource_value_t)"basic",
      (void *)&basic_rom_name, set_basic_rom_name, NULL },
    { "RomsetBasicName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[2], set_romset_firmware, (void *)2 },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      (void *)&emu_id_enabled, set_emu_id_enabled, NULL },
    { "KernalRev", RES_STRING, (resource_value_t)"",
      (void *)&kernal_revision, set_kernal_revision, NULL },
#ifdef HAVE_RS232
    { "Acia1Enable", RES_INTEGER, (resource_value_t)0,
      (void *)&acia_de_enabled, set_acia_de_enabled, NULL },
#endif
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)KBD_INDEX_C64_DEFAULT,
      (void *)&machine_keymap_index, keyboard_set_keymap_index, NULL },
    { "KeymapSymFile", RES_STRING, (resource_value_t)KBD_C64_SYM_US,
      (void *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapSymDeFile", RES_STRING, (resource_value_t)KBD_C64_SYM_DE,
      (void *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
    { "KeymapPosFile", RES_STRING, (resource_value_t)KBD_C64_POS,
      (void *)&machine_keymap_file_list[2],
      keyboard_set_keymap_file, (void *)2 },
#endif
    { "SidStereoAddressStart", RES_INTEGER, (resource_value_t)0xde00,
      (void *)&sid_stereo_address_start, sid_set_sid_stereo_address, NULL },
    { NULL }
};

int c64_resources_init(void)
{
    return resources_register(resources);
}

void c64_resources_shutdown(void)
{
    lib_free(chargen_rom_name);
    lib_free(basic_rom_name);
    lib_free(kernal_rom_name);
    lib_free(kernal_revision);
    lib_free(machine_keymap_file_list[0]);
    lib_free(machine_keymap_file_list[1]);
    lib_free(machine_keymap_file_list[2]);
}

