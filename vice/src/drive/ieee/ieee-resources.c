/*
 * ieee-resources.c
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

#include "ieee-resources.h"
#include "ieeerom.h"
#include "lib.h"
#include "resources.h"
#include "util.h"


static int romset_firmware[5];

static char *dos_rom_name_2031 = NULL;
static char *dos_rom_name_1001 = NULL;
static char *dos_rom_name_2040 = NULL;
static char *dos_rom_name_3040 = NULL;
static char *dos_rom_name_4040 = NULL;


static int set_dos_rom_name_2040(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_2040, (const char *)v))
        return 0;

    return ieeerom_load_2040();
}

static int set_dos_rom_name_3040(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_3040, (const char *)v))
        return 0;

    return ieeerom_load_3040();
}

static int set_dos_rom_name_4040(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_4040, (const char *)v))
        return 0;

    return ieeerom_load_4040();
}

static int set_dos_rom_name_1001(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1001, (const char *)v))
        return 0;

    return ieeerom_load_1001();
}

static int set_dos_rom_name_2031(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_2031, (const char *)v))
        return 0;

    return ieeerom_load_2031();
}

static int set_romset_firmware(resource_value_t v, void *param)
{
    unsigned int num = (unsigned int)param;

    romset_firmware[num] = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "DosName2031", RES_STRING, (resource_value_t)"dos2031",
      (void *)&dos_rom_name_2031, set_dos_rom_name_2031, NULL },
    { "RomsetDosName2031", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[0], set_romset_firmware, (void *)0 },
    { "DosName2040", RES_STRING, (resource_value_t)"dos2040",
      (void *)&dos_rom_name_2040, set_dos_rom_name_2040, NULL },
    { "RomsetDosName2040", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[1], set_romset_firmware, (void *)1 },
    { "DosName3040", RES_STRING, (resource_value_t)"dos3040",
      (void *)&dos_rom_name_3040, set_dos_rom_name_3040, NULL },
    { "RomsetDosName3040", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[2], set_romset_firmware, (void *)2 },
    { "DosName4040", RES_STRING, (resource_value_t)"dos4040",
      (void *)&dos_rom_name_4040, set_dos_rom_name_4040, NULL },
    { "RomsetDosName4040", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[3], set_romset_firmware, (void *)3 },
    { "DosName1001", RES_STRING, (resource_value_t)"dos1001",
      (void *)&dos_rom_name_1001, set_dos_rom_name_1001, NULL },
    { "RomsetDosName1001", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[4], set_romset_firmware, (void *)4 },
    { NULL }
};

int ieee_resources_init(void)
{
    return resources_register(resources);
}

void ieee_resources_shutdown(void)
{
    lib_free(dos_rom_name_2031);
    lib_free(dos_rom_name_1001);
    lib_free(dos_rom_name_2040);
    lib_free(dos_rom_name_3040);
    lib_free(dos_rom_name_4040);
}

