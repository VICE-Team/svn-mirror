/*
 * iec-resources.c
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

#include "drive.h"
#include "drivemem.h"
#include "driverom.h"
#include "iec-resources.h"
#include "resources.h"
#include "utils.h"


static char *dos_rom_name_1541 = NULL;
static char *dos_rom_name_1541ii = NULL;
static char *dos_rom_name_1571 = NULL;
static char *dos_rom_name_1581 = NULL;


static int set_dos_rom_name_1541(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1541, (const char *)v))
        return 0;

    return drive_rom_load_1541();
}

static int set_dos_rom_name_1541ii(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1541ii, (const char *)v))
        return 0;

    return drive_rom_load_1541ii();
}

static int set_dos_rom_name_1571(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1571, (const char *)v))
        return 0;

    return drive_rom_load_1571();
}

static int set_dos_rom_name_1581(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1581, (const char *)v))
        return 0;

    return drive_rom_load_1581();
}

static void set_drive_ram(unsigned int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_NONE)
        return;
    if (dnr == 0)
        drive_mem_init(&drive0_context, drive[0].type);
    else
        drive_mem_init(&drive1_context, drive[1].type);
    return;
}

static int set_drive_ram2(resource_value_t v, void *param)
{
    drive[(int)param].drive_ram2_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram4(resource_value_t v, void *param)
{
    drive[(int)param].drive_ram4_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram6(resource_value_t v, void *param)
{
    drive[(int)param].drive_ram6_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram8(resource_value_t v, void *param)
{
    drive[(int)param].drive_ram8_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_rama(resource_value_t v, void *param)
{
    drive[(int)param].drive_rama_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static resource_t resources[] = {
    { "DosName1541", RES_STRING, (resource_value_t)"dos1541",
      (resource_value_t *)&dos_rom_name_1541,
      set_dos_rom_name_1541, NULL },
    { "DosName1541ii", RES_STRING, (resource_value_t)"d1541II",
      (resource_value_t *)&dos_rom_name_1541ii,
      set_dos_rom_name_1541ii, NULL },
    { "DosName1571", RES_STRING, (resource_value_t)"dos1571",
      (resource_value_t *)&dos_rom_name_1571,
      set_dos_rom_name_1571, NULL },
    { "DosName1581", RES_STRING, (resource_value_t)"dos1581",
      (resource_value_t *)&dos_rom_name_1581,
      set_dos_rom_name_1581, NULL },
    { "Drive8RAM2000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[0].drive_ram2_enabled),
      set_drive_ram2, (void *)0 },
    { "Drive9RAM2000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[1].drive_ram2_enabled),
      set_drive_ram2, (void *)1 },
    { "Drive8RAM4000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[0].drive_ram4_enabled),
      set_drive_ram4, (void *)0 },
    { "Drive9RAM4000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[1].drive_ram4_enabled),
      set_drive_ram4, (void *)1 },
    { "Drive8RAM6000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[0].drive_ram6_enabled),
      set_drive_ram6,  (void *)0},
    { "Drive9RAM6000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[1].drive_ram6_enabled),
      set_drive_ram6, (void *)1 },
    { "Drive8RAM8000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[0].drive_ram8_enabled),
      set_drive_ram8, (void *)0 },
    { "Drive9RAM8000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[1].drive_ram8_enabled),
      set_drive_ram8, (void *)1 },
    { "Drive8RAMA000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[0].drive_rama_enabled),
      set_drive_rama, (void *)0 },
    { "Drive9RAMA000", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&(drive[1].drive_rama_enabled),
      set_drive_rama, (void *)1 },
    { NULL }
};

int iec_resources_init(void)
{
    return resources_register(resources);
}

