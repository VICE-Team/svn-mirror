/*
 * plus4-resources.c
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

#include "plus4mem.h"
#include "resources.h"
#include "utils.h"


/* Name of the BASIC ROM.  */
static char *basic_rom_name = NULL;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name = NULL;

/* Name of the 3plus1 ROMs.  */
static char *tplus1lo_rom_name = NULL;
static char *tplus1hi_rom_name = NULL;

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

static int set_3plus1lo_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&tplus1lo_rom_name, (const char *)v))
        return 0;

    return mem_load_3plus1lo(tplus1lo_rom_name);
}

static int set_3plus1hi_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&tplus1hi_rom_name, (const char *)v))
        return 0;

    return mem_load_3plus1hi(tplus1hi_rom_name);
}

static resource_t resources[] = {
    { "KernalName", RES_STRING, (resource_value_t)"kernal",
      (resource_value_t *)&kernal_rom_name,
      set_kernal_rom_name, NULL },
    { "BasicName", RES_STRING, (resource_value_t)"basic",
      (resource_value_t *)&basic_rom_name,
      set_basic_rom_name, NULL },
    { "3plus1loName", RES_STRING, (resource_value_t)"3plus1lo",
      (resource_value_t *)&tplus1lo_rom_name,
      set_3plus1lo_rom_name, NULL },
    { "3plus1hiName", RES_STRING, (resource_value_t)"3plus1hi",
      (resource_value_t *)&tplus1hi_rom_name,
      set_3plus1hi_rom_name, NULL },
    { NULL }
};

int plus4_resources_init(void)
{
    return resources_register(resources);
}

