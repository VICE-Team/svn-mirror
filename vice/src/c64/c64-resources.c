/*
 * c64-resources.c
 *
 * Written by
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "c64cart.h"
#include "c64mem.h"
#include "cartridge.h"
#include "drive.h"
#include "keyboard.h"
#include "machine.h"
#include "resources.h"
#include "reu.h"
#include "sid-resources.h"
#include "utils.h"


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
char *kernal_revision;

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

    return mem_load_chargen(chargen_rom_name);
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_rom_name, (const char *)v))
        return 0;

    return mem_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basic_rom_name, (const char *)v))
        return 0;

    return mem_load_basic(basic_rom_name);
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
        sync_factor = (int) v;
        drive_set_pal_sync_factor();
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_PAL);
        break;
      case MACHINE_SYNC_NTSC:
        sync_factor = (int)v;
        drive_set_ntsc_sync_factor();
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSC);
        break;
      case MACHINE_SYNC_NTSCOLD:
        sync_factor = (int)v;
        drive_set_ntsc_sync_factor();
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSCOLD);
        break;
      default:
        if ((int)v > 0) {
            sync_factor = (int)v;
            drive_set_sync_factor((unsigned int)v);
        } else {
            return -1;
        }
    }
    return 0;
}

static resource_t resources[] = {
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      (resource_value_t *)&sync_factor,
      set_sync_factor, NULL },
    { "ChargenName", RES_STRING, (resource_value_t)"chargen",
      (resource_value_t *)&chargen_rom_name,
      set_chargen_rom_name, NULL },
    { "KernalName", RES_STRING, (resource_value_t)"kernal",
      (resource_value_t *)&kernal_rom_name,
      set_kernal_rom_name, NULL },
    { "BasicName", RES_STRING, (resource_value_t)"basic",
      (resource_value_t *)&basic_rom_name,
      set_basic_rom_name, NULL },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&emu_id_enabled,
      set_emu_id_enabled, NULL },
    { "KernalRev", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&kernal_revision,
      set_kernal_revision, NULL },
#ifdef HAVE_RS232
    { "AciaDE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&acia_de_enabled,
      set_acia_de_enabled, NULL },
#endif
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&machine_keymap_index,
      keyboard_set_keymap_index, NULL },
    { "KeymapSymFile", RES_STRING,
      (resource_value_t)"default.vkm",
      (resource_value_t *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapPosFile", RES_STRING,
      (resource_value_t)"position.vkm",
      (resource_value_t *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
#endif
    { "SidStereoAddressStart", RES_INTEGER, (resource_value_t)0xde00,
      (resource_value_t *)&sid_stereo_address_start,
      sid_set_sid_stereo_address, NULL },
    { NULL }
};

int c64_resources_init(void)
{
    return resources_register(resources);
}

