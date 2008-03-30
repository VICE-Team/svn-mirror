/*
 * functionrom.c
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

#include "archdep.h"
#include "cmdline.h"
#include "resources.h"
#include "utils.h"
#include "types.h"

#define INTERNAL_FUNCTION_ROM_SIZE 0x8000
#define EXTERNAL_FUNCTION_ROM_SIZE 0x4000

/* Flag: Do we enable the internal function ROM?  */
static int internal_function_rom_enabled;

/* Name of the internal function ROM.  */
static char *internal_function_rom_name;

/* Image of the internal function ROM.  */
BYTE int_function_rom[INTERNAL_FUNCTION_ROM_SIZE];

/* Flag: Do we enable the external function ROM?  */
static int external_function_rom_enabled;

/* Name of the external function ROM.  */
static char *external_function_rom_name;

/* Image of the external function ROM.  */
BYTE ext_function_rom[EXTERNAL_FUNCTION_ROM_SIZE];

static int functionrom_load_internal(void);
static int functionrom_load_external(void);

static int set_internal_function_rom_enabled(resource_value_t v, void *param)
{
    internal_function_rom_enabled = (int)v;
    return functionrom_load_internal();
}

static int set_internal_function_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&internal_function_rom_name, (const char *)v))
        return 0;

    return functionrom_load_internal();
}

static int set_external_function_rom_enabled(resource_value_t v, void *param)
{
    external_function_rom_enabled = (int)v;
    return functionrom_load_external();
}

static int set_external_function_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&external_function_rom_name, (const char *)v))
        return 0;

    return functionrom_load_external();
}

static resource_t resources[] =
{
    { "InternalFunctionROM", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&internal_function_rom_enabled,
      set_internal_function_rom_enabled, NULL },
    { "InternalFunctionName", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&internal_function_rom_name,
      set_internal_function_rom_name, NULL },
    { "ExternalFunctionROM", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&external_function_rom_enabled,
      set_external_function_rom_enabled, NULL },
    { "ExternalFunctionName", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&external_function_rom_name,
      set_external_function_rom_name, NULL },
    { NULL }
};

int functionrom_resources_init(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-intfrom", SET_RESOURCE, 1, NULL, NULL, "InternalFunctionName", NULL,
      "<name>", "Specify name of internal Function ROM image" },
    { "-extfrom", SET_RESOURCE, 1, NULL, NULL, "ExternalFunctionName", NULL,
      "<name>", "Specify name of external Function ROM image" },
    { "-intfunc", SET_RESOURCE, 0, NULL, NULL, "InternalFunctionROM",
      (resource_value_t)1, NULL, "Enable the internal Function ROM" },
    { "+intfunc", SET_RESOURCE, 0, NULL, NULL, "InternalFunctionROM",
      (resource_value_t)0, NULL, "Disable the internal Function ROM" },
    { "-extfunc", SET_RESOURCE, 0, NULL, NULL, "ExternalFunctionROM",
      (resource_value_t)1, NULL, "Enable the external Function ROM" },
    { "+extfunc", SET_RESOURCE, 0, NULL, NULL, "ExternalFunctionROM",
      (resource_value_t)0, NULL, "Disable the external Function ROM" },
    { NULL }
};

int functionrom_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

static int functionrom_load_internal(void)
{
    if (internal_function_rom_enabled) {
        if (util_check_null_string(internal_function_rom_name))
            return 0;

        if (util_file_load(internal_function_rom_name, int_function_rom,
            INTERNAL_FUNCTION_ROM_SIZE,
            UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0)
            return -1;
    } else {
        memset(int_function_rom, 0, sizeof(int_function_rom));
    }

    return 0;
}

static int functionrom_load_external(void)
{
    if (external_function_rom_enabled) {
        if (util_check_null_string(external_function_rom_name))
            return 0;

        if (util_file_load(external_function_rom_name, ext_function_rom,
            EXTERNAL_FUNCTION_ROM_SIZE,
            UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0)
            return -1;
    } else {
        memset(ext_function_rom, 0, sizeof(ext_function_rom));
    }

    return 0;
}

BYTE REGPARM1 internal_function_rom_read(ADDRESS addr)
{
    return int_function_rom[addr & 0x7fff];
}

void REGPARM2 internal_function_rom_store(ADDRESS addr, BYTE value)
{
    int_function_rom[addr & 0x7fff] = value;
}

BYTE REGPARM1 external_function_rom_read(ADDRESS addr)
{
    return ext_function_rom[addr & 0x3fff];
}

void REGPARM2 external_function_rom_store(ADDRESS addr, BYTE value)
{
    ext_function_rom[addr & 0x3fff] = value;
}

