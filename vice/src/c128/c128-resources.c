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

#include "c128.h"
#include "c128rom.h"
#include "drive.h"
#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "resources.h"
#include "reu.h"
#include "sid-resources.h"
#include "utils.h"


#define KBD_INDEX_C128_SYM  0
#define KBD_INDEX_C128_POS  1


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
    int type = (int)v;

    if (type != C128_MACHINE_INT && type != C128_MACHINE_FINNISH
        && type != C128_MACHINE_FRENCH && type != C128_MACHINE_GERMAN
        && type != C128_MACHINE_ITALIAN && type != C128_MACHINE_NORWEGIAN
        && type != C128_MACHINE_SWEDISH)
        return -1;

    machine_type = type;

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

        resources_get_value("REU", (resource_value_t)&reu_enabled);

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

#endif

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
      default:
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_t resources[] =
{
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      (resource_value_t *)&sync_factor,
      set_sync_factor, NULL },
    { "MachineType", RES_INTEGER, (resource_value_t)C128_MACHINE_INT,
      (resource_value_t *)&machine_type,
      set_machine_type, NULL },
    { "ChargenIntName", RES_STRING, (resource_value_t)"chargen",
      (resource_value_t *)&chargen_int_rom_name,
      set_chargen_int_rom_name, NULL },
    { "ChargenDEName", RES_STRING, (resource_value_t)"chargde",
      (resource_value_t *)&chargen_de_rom_name,
      set_chargen_de_rom_name, NULL },
    { "ChargenFRName", RES_STRING, (resource_value_t)"chargfr",
      (resource_value_t *)&chargen_fr_rom_name,
      set_chargen_fr_rom_name, NULL },
    { "ChargenSEName", RES_STRING, (resource_value_t)"chargse",
      (resource_value_t *)&chargen_se_rom_name,
      set_chargen_se_rom_name, NULL },
    { "KernalIntName", RES_STRING, (resource_value_t)"kernal",
      (resource_value_t *)&kernal_int_rom_name,
      set_kernal_int_rom_name, NULL },
    { "KernalDEName", RES_STRING, (resource_value_t)"kernalde",
      (resource_value_t *)&kernal_de_rom_name,
      set_kernal_de_rom_name, NULL },
    { "KernalFIName", RES_STRING, (resource_value_t)"kernalfi",
      (resource_value_t *)&kernal_fi_rom_name,
      set_kernal_fi_rom_name, NULL },
    { "KernalFRName", RES_STRING, (resource_value_t)"kernalfr",
      (resource_value_t *)&kernal_fr_rom_name,
      set_kernal_fr_rom_name, NULL },
    { "KernalITName", RES_STRING, (resource_value_t)"kernalit",
      (resource_value_t *)&kernal_it_rom_name,
      set_kernal_it_rom_name, NULL },
    { "KernalNOName", RES_STRING, (resource_value_t)"kernalno",
      (resource_value_t *)&kernal_no_rom_name,
      set_kernal_no_rom_name, NULL },
    { "KernalSEName", RES_STRING, (resource_value_t)"kernalse",
      (resource_value_t *)&kernal_se_rom_name,
      set_kernal_se_rom_name, NULL },
    { "BasicLoName", RES_STRING, (resource_value_t)"basiclo",
      (resource_value_t *)&basiclo_rom_name,
      set_basiclo_rom_name, NULL },
    { "BasicHiName", RES_STRING, (resource_value_t)"basichi",
      (resource_value_t *)&basichi_rom_name,
      set_basichi_rom_name, NULL },
    { "Kernal64Name", RES_STRING, (resource_value_t)"kernal64",
      (resource_value_t *)&kernal64_rom_name,
      set_kernal64_rom_name, NULL },
    { "Basic64Name", RES_STRING, (resource_value_t)"basic64",
      (resource_value_t *)&basic64_rom_name,
      set_basic64_rom_name, NULL },
    { "IEEE488", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&ieee488_enabled,
      set_ieee488_enabled, NULL },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&emu_id_enabled,
      set_emu_id_enabled, NULL },
#ifdef HAVE_RS232
    { "AciaDE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&acia_de_enabled,
      set_acia_de_enabled, NULL },
#if 0
    { "AciaD7", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&acia_d7_enabled,
      set_acia_d7_enabled, NULL },
#endif
#endif
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)KBD_INDEX_C128_SYM,
      (resource_value_t *)&machine_keymap_index,
      keyboard_set_keymap_index, NULL },
    { "KeymapSymFile", RES_STRING,
      (resource_value_t)KBD_C128_SYM,
      (resource_value_t *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapPosFile", RES_STRING,
      (resource_value_t)KBD_C128_POS,
      (resource_value_t *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
#endif
    { "SidStereoAddressStart", RES_INTEGER, (resource_value_t)0xde00,
      (resource_value_t *)&sid_stereo_address_start,
      sid_set_sid_stereo_address, NULL },
    { NULL }
};

int c128_resources_init(void)
{
    return resources_register(resources);
}

