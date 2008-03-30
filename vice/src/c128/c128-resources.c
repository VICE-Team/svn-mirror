/*
 * c128-resources.c
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

#include "c128-resources.h"
#include "c128.h"
#include "c128mem.h"
#include "c128rom.h"
#include "c64export.h"
#include "drive.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "reu.h"
#include "resources.h"
#include "sid-resources.h"
#include "util.h"


#define KBD_INDEX_C128_SYM  0
#define KBD_INDEX_C128_POS  1


static int romset_firmware[15];

/* What sync factor between the CPU and the drive?  If equal to
   `MACHINE_SYNC_PAL', the same as PAL machines.  If equal to
   `MACHINE_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as 65536 * drive_clk / clk_[main machine] */
static int sync_factor;

/* Type of machine.  */
static int machine_type;

/* Name of the international character ROM.  */
static char *chargen_int_rom_name = NULL;

/* Name of the German character ROM.  */
static char *chargen_de_rom_name = NULL;

/* Name of the French character ROM.  */
static char *chargen_fr_rom_name = NULL;

/* Name of the Swedish character ROM.  */
static char *chargen_se_rom_name = NULL;

/* Name of the BASIC LO ROM.  */
static char *basiclo_rom_name = NULL;

/* Name of the BASIC HI ROM.  */
static char *basichi_rom_name = NULL;

/* Name of the international Kernal ROM.  */
static char *kernal_int_rom_name = NULL;

/* Name of the German Kernal ROM.  */
static char *kernal_de_rom_name = NULL;

/* Name of the Finnish Kernal ROM.  */
static char *kernal_fi_rom_name = NULL;

/* Name of the French Kernal ROM.  */
static char *kernal_fr_rom_name = NULL;

/* Name of the Italian Kernal ROM.  */
static char *kernal_it_rom_name = NULL;

/* Name of the Norwegian Kernal ROM.  */
static char *kernal_no_rom_name = NULL;

/* Name of the Swedish Kernal ROM.  */
static char *kernal_se_rom_name = NULL;

/* Name of the BASIC ROM.  */
static char *basic64_rom_name = NULL;

/* Name of the Kernal ROM.  */
static char *kernal64_rom_name = NULL;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

/* Flag: Do we enable the IEEE488 interface emulation?  */
int ieee488_enabled;

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
int acia_de_enabled;

#if 0
/* Flag: Do we enable the $D7** ACIA RS232 interface emulation?  */
int acia_d7_enabled;
#endif
#endif


static int set_machine_type(resource_value_t v, void *param)
{
    unsigned int type = (unsigned int)v;

    if (type != C128_MACHINE_INT && type != C128_MACHINE_FINNISH
        && type != C128_MACHINE_FRENCH && type != C128_MACHINE_GERMAN
        && type != C128_MACHINE_ITALIAN && type != C128_MACHINE_NORWEGIAN
        && type != C128_MACHINE_SWEDISH)
        return -1;

    machine_type = type;

    mem_set_machine_type(type);

    if (c128rom_kernal_setup() < 0)
        return -1;

    if (c128rom_chargen_setup() < 0)
        return -1;

    return 0;
}

static int set_chargen_int_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_int_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_chargen_int(chargen_int_rom_name) < 0)
        return -1;

    if (c128rom_chargen_setup() < 0)
        return -1;

    return 0;
}

static int set_chargen_de_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_de_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_chargen_de(chargen_de_rom_name) < 0)
        return -1;

    if (c128rom_chargen_setup() < 0)
        return -1;

    return 0;
}

static int set_chargen_fr_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_fr_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_chargen_fr(chargen_fr_rom_name) < 0)
        return -1;

    if (c128rom_chargen_setup() < 0)
        return -1;

    return 0;
}

static int set_chargen_se_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_se_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_chargen_se(chargen_se_rom_name) < 0)
        return -1;

    if (c128rom_chargen_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_int_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_int_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_int(kernal_int_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_de_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_de_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_de(kernal_de_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_fi_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_fi_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_fi(kernal_fi_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_fr_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_fr_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_fr(kernal_fr_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_it_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_it_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_it(kernal_it_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_no_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_no_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_no(kernal_no_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_kernal_se_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_se_rom_name, (const char *)v))
        return 0;

    if (c128rom_load_kernal_se(kernal_se_rom_name) < 0)
        return -1;

    if (c128rom_kernal_setup() < 0)
        return -1;

    return 0;
}

static int set_basiclo_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basiclo_rom_name, (const char *)v))
        return 0;

    return c128rom_load_basiclo(basiclo_rom_name);
}

static int set_basichi_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basichi_rom_name, (const char *)v))
        return 0;

    return c128rom_load_basichi(basichi_rom_name);
}

static int set_kernal64_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal64_rom_name, (const char *)v))
        return 0;

    return c128rom_load_kernal64(kernal64_rom_name);
}

static int set_basic64_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basic64_rom_name, (const char *)v))
        return 0;

    return c128rom_load_basic64(basic64_rom_name);
}

static int set_emu_id_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
        emu_id_enabled = 0;
        return 0;
    } else {
        emu_id_enabled = 1;
        return 0;
    }
}

static int set_ieee488_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
        ieee488_enabled = 0;
        return 0;
    } else {
        int reu_enabled;

        resources_get_value("REU", (void *)&reu_enabled);

        if (!reu_enabled) {
            ieee488_enabled = 1;
            return 0;
        } else {
            /* The REU and the IEEE488 interface share the same address
               space, so they cannot be enabled at the same time.  */
            return -1;
        }
    }
}

#ifdef HAVE_RS232
#if 0
static int set_acia_d7_enabled(resource_value_t v, void *param)
{
    acia_d7_enabled = (int)v;
    return 0;
}
#endif

static int set_acia_de_enabled(resource_value_t v, void *param)
{
  static const c64export_resource_t export_res = {
      "ACIA", 1, 0, 0, 0
  };

  if ((int)v==1)
  {
    if (c64export_query(&export_res) < 0)
      return -1;
    if (c64export_add(&export_res) < 0)
      return -1;
  }
  else
    c64export_remove(&export_res);
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

static const resource_t resources[] =
{
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      RES_EVENT_SAME, NULL,
      (void *)&sync_factor, set_sync_factor, NULL },
    { "MachineType", RES_INTEGER, (resource_value_t)C128_MACHINE_INT,
      RES_EVENT_SAME, NULL,
      (void *)&machine_type, set_machine_type, NULL },
    { "ChargenIntName", RES_STRING, (resource_value_t)"chargen",
      RES_EVENT_NO, NULL,
      (void *)&chargen_int_rom_name, set_chargen_int_rom_name, NULL },
    { "RomsetChargenIntName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[0], set_romset_firmware, (void *)0 },
    { "ChargenDEName", RES_STRING, (resource_value_t)"chargde",
      RES_EVENT_NO, NULL,
      (void *)&chargen_de_rom_name, set_chargen_de_rom_name, NULL },
    { "RomseChargenDENamet", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[1], set_romset_firmware, (void *)1 },
    { "ChargenFRName", RES_STRING, (resource_value_t)"chargfr",
      RES_EVENT_NO, NULL,
      (void *)&chargen_fr_rom_name, set_chargen_fr_rom_name, NULL },
    { "RomsetChargenFRName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[2], set_romset_firmware, (void *)2 },
    { "ChargenSEName", RES_STRING, (resource_value_t)"chargse",
      RES_EVENT_NO, NULL,
      (void *)&chargen_se_rom_name, set_chargen_se_rom_name, NULL },
    { "RomsetChargenSEName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[3], set_romset_firmware, (void *)3 },
    { "KernalIntName", RES_STRING, (resource_value_t)"kernal",
      RES_EVENT_NO, NULL,
      (void *)&kernal_int_rom_name, set_kernal_int_rom_name, NULL },
    { "RomsetKernalIntName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[4], set_romset_firmware, (void *)4 },
    { "KernalDEName", RES_STRING, (resource_value_t)"kernalde",
      RES_EVENT_NO, NULL,
      (void *)&kernal_de_rom_name, set_kernal_de_rom_name, NULL },
    { "RomsetKernalDEName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[5], set_romset_firmware, (void *)5 },
    { "KernalFIName", RES_STRING, (resource_value_t)"kernalfi",
      RES_EVENT_NO, NULL,
      (void *)&kernal_fi_rom_name, set_kernal_fi_rom_name, NULL },
    { "RomsetKernalFIName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[6], set_romset_firmware, (void *)6 },
    { "KernalFRName", RES_STRING, (resource_value_t)"kernalfr",
      RES_EVENT_NO, NULL,
      (void *)&kernal_fr_rom_name, set_kernal_fr_rom_name, NULL },
    { "RomsetKernalFRName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[7], set_romset_firmware, (void *)7 },
    { "KernalITName", RES_STRING, (resource_value_t)"kernalit",
      RES_EVENT_NO, NULL,
      (void *)&kernal_it_rom_name, set_kernal_it_rom_name, NULL },
    { "RomsetKernalITName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[8], set_romset_firmware, (void *)8 },
    { "KernalNOName", RES_STRING, (resource_value_t)"kernalno",
      RES_EVENT_NO, NULL,
      (void *)&kernal_no_rom_name, set_kernal_no_rom_name, NULL },
    { "RomsetKernalNOName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[9], set_romset_firmware, (void *)9 },
    { "KernalSEName", RES_STRING, (resource_value_t)"kernalse",
      RES_EVENT_NO, NULL,
      (void *)&kernal_se_rom_name, set_kernal_se_rom_name, NULL },
    { "RomsetKernalSEName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[10], set_romset_firmware, (void *)10 },
    { "BasicLoName", RES_STRING, (resource_value_t)"basiclo",
      RES_EVENT_NO, NULL,
      (void *)&basiclo_rom_name, set_basiclo_rom_name, NULL },
    { "RomsetBasicLoName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[11], set_romset_firmware, (void *)11 },
    { "BasicHiName", RES_STRING, (resource_value_t)"basichi",
      RES_EVENT_NO, NULL,
      (void *)&basichi_rom_name, set_basichi_rom_name, NULL },
    { "RomsetBasicHiName", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[12], set_romset_firmware, (void *)12 },
    { "Kernal64Name", RES_STRING, (resource_value_t)"kernal64",
      RES_EVENT_NO, NULL,
      (void *)&kernal64_rom_name, set_kernal64_rom_name, NULL },
    { "RomsetKernal64Name", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[13], set_romset_firmware, (void *)13 },
    { "Basic64Name", RES_STRING, (resource_value_t)"basic64",
      RES_EVENT_NO, NULL,
      (void *)&basic64_rom_name, set_basic64_rom_name, NULL },
    { "RomsetBasic64Name", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[14], set_romset_firmware, (void *)14 },
    { "IEEE488", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      (void *)&ieee488_enabled, set_ieee488_enabled, NULL },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      (void *)&emu_id_enabled, set_emu_id_enabled, NULL },
#ifdef HAVE_RS232
    { "Acia1Enable", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_STRICT, (resource_value_t)0,
      (void *)&acia_de_enabled, set_acia_de_enabled, NULL },
#endif
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)KBD_INDEX_C128_SYM,
      RES_EVENT_NO, NULL,
      (void *)&machine_keymap_index, keyboard_set_keymap_index, NULL },
    { "KeymapSymFile", RES_STRING, (resource_value_t)KBD_C128_SYM,
      RES_EVENT_NO, NULL,
      (void *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapPosFile", RES_STRING, (resource_value_t)KBD_C128_POS,
      RES_EVENT_NO, NULL,
      (void *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
#endif
    { "SidStereoAddressStart", RES_INTEGER, (resource_value_t)0xde00,
      RES_EVENT_SAME, NULL,
      (void *)&sid_stereo_address_start, sid_set_sid_stereo_address, NULL },
    { NULL }
};

int c128_resources_init(void)
{
    return resources_register(resources);
}

void c128_resources_shutdown(void)
{
    lib_free(chargen_int_rom_name);
    lib_free(chargen_de_rom_name);
    lib_free(chargen_fr_rom_name);
    lib_free(chargen_se_rom_name);
    lib_free(basiclo_rom_name);
    lib_free(basichi_rom_name);
    lib_free(kernal_int_rom_name);
    lib_free(kernal_de_rom_name);
    lib_free(kernal_fi_rom_name);
    lib_free(kernal_fr_rom_name);
    lib_free(kernal_it_rom_name);
    lib_free(kernal_no_rom_name);
    lib_free(kernal_se_rom_name);
    lib_free(basic64_rom_name);
    lib_free(kernal64_rom_name);
    lib_free(machine_keymap_file_list[0]);
    lib_free(machine_keymap_file_list[1]);
}

