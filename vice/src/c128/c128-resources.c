/*
 * c128-resources.c
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

#include "c128mem.h"
#include "resources.h"
#include "reu.h"
#include "utils.h"


/* Name of the character ROM.  */
static char *chargen_rom_name;

/* Name of the BASIC ROM.  */
static char *basic_rom_name;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name;

/* Name of the character ROM.  */
static char *chargen64_rom_name;

/* Name of the BASIC ROM.  */
static char *basic64_rom_name;

/* Name of the Kernal ROM.  */
static char *kernal64_rom_name;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

/* Flag: Do we enable the IEEE488 interface emulation?  */
int ieee488_enabled;

/* Flag: Do we enable the external REU?  */
int reu_enabled;

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
int acia_de_enabled;

#if 0
/* Flag: Do we enable the $D7** ACIA RS232 interface emulation?  */
int acia_d7_enabled;
#endif
#endif


static int set_chargen_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (chargen_rom_name != NULL && name != NULL
        && strcmp(name, chargen_rom_name) == 0)
        return 0;

    util_string_set(&chargen_rom_name, name);

    return mem_load_chargen(chargen_rom_name);
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (kernal_rom_name != NULL && name != NULL
        && strcmp(name, kernal_rom_name) == 0)
        return 0;

    util_string_set(&kernal_rom_name, name);

    return mem_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (basic_rom_name != NULL && name != NULL
        && strcmp(name, basic_rom_name) == 0)
        return 0;

    util_string_set(&basic_rom_name, name);

    return mem_load_basic(basic_rom_name);
}

static int set_chargen64_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (chargen64_rom_name != NULL && name != NULL
        && strcmp(name, chargen64_rom_name) == 0)
        return 0;

    util_string_set(&chargen64_rom_name, name);

    return mem_load_chargen64(chargen64_rom_name);
}

static int set_kernal64_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (kernal64_rom_name != NULL && name != NULL
        && strcmp(name, kernal64_rom_name) == 0)
        return 0;

    util_string_set(&kernal64_rom_name, name);

    return mem_load_kernal64(kernal64_rom_name);
}

static int set_basic64_rom_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (basic64_rom_name != NULL && name != NULL
        && strcmp(name, basic64_rom_name) == 0)
        return 0;

    util_string_set(&basic64_rom_name, name);

    return mem_load_basic64(basic64_rom_name);
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
static int set_reu_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
        reu_enabled = 0;
        reu_deactivate();
        return 0;
    } else if (!ieee488_enabled) {
        reu_enabled = 1;
        reu_activate();
        return 0;
    } else {
        /* The REU and the IEEE488 interface share the same address space, so
           they cannot be enabled at the same time.  */
        return -1;
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

/* ------------------------------------------------------------------------- */

static resource_t resources[] =
{
    { "ChargenName", RES_STRING, (resource_value_t)"chargen",
      (resource_value_t *)&chargen_rom_name,
      set_chargen_rom_name, NULL },
    { "KernalName", RES_STRING, (resource_value_t)"kernal",
      (resource_value_t *)&kernal_rom_name,
      set_kernal_rom_name, NULL },
    { "BasicName", RES_STRING, (resource_value_t)"basic",
      (resource_value_t *)&basic_rom_name,
      set_basic_rom_name, NULL },
    { "Chargen64Name", RES_STRING, (resource_value_t)"charg64",
      (resource_value_t *)&chargen64_rom_name,
      set_chargen64_rom_name, NULL },
    { "Kernal64Name", RES_STRING, (resource_value_t)"kernal64",
      (resource_value_t *)&kernal64_rom_name,
      set_kernal64_rom_name, NULL },
    { "Basic64Name", RES_STRING, (resource_value_t)"basic64",
      (resource_value_t *)&basic64_rom_name,
      set_basic64_rom_name, NULL },
    { "REU", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&reu_enabled,
      set_reu_enabled, NULL },
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
    { NULL }
};

int c128_init_resources(void)
{
    return resources_register(resources);
}

