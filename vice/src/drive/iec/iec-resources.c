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
#include "drivetypes.h"
#include "iec-resources.h"
#include "iecrom.h"
#include "lib.h"
#include "resources.h"
#include "util.h"


static int romset_firmware[5];

static char *dos_rom_name_1541 = NULL;
static char *dos_rom_name_1541ii = NULL;
static char *dos_rom_name_1570 = NULL;
static char *dos_rom_name_1571 = NULL;
static char *dos_rom_name_1581 = NULL;


static int set_drive_parallel_cable_enabled(resource_value_t v, void *param)
{
    drive_context[(unsigned int)param]->drive->parallel_cable_enabled = (int)v;

    return 0;
}

static int set_drive_idling_method(resource_value_t v, void *param)
{
    unsigned int dnr;
    drive_t *drive;

    dnr = (unsigned int)param;
    drive = drive_context[dnr]->drive;

    /* FIXME: Maybe we should call `drive_cpu_execute()' here?  */
    if ((int)v != DRIVE_IDLE_SKIP_CYCLES
        && (int)v != DRIVE_IDLE_TRAP_IDLE
        && (int)v != DRIVE_IDLE_NO_IDLE)
        return -1;

    drive->idling_method = (int)v;

    if (rom_loaded &&
        ((drive->type == DRIVE_TYPE_1541) ||
         (drive->type == DRIVE_TYPE_1541II))) {
        if (drive->idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive->rom[0xeae4 - 0x8000] = 0xea;
            drive->rom[0xeae5 - 0x8000] = 0xea;
            drive->rom[0xeae8 - 0x8000] = 0xea;
            drive->rom[0xeae9 - 0x8000] = 0xea;
            drive->rom[0xec9b - 0x8000] = 0x00;
        } else {
            drive->rom[0xeae4 - 0x8000] = drive->rom_checksum[0];
            drive->rom[0xeae5 - 0x8000] = drive->rom_checksum[1];
            drive->rom[0xeae8 - 0x8000] = drive->rom_checksum[2];
            drive->rom[0xeae9 - 0x8000] = drive->rom_checksum[3];
            drive->rom[0xec9b - 0x8000] = drive->rom_idle_trap[0];
        }
    }
    if (rom_loaded && drive->type == DRIVE_TYPE_1551) {
        if (drive->idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive->rom[0xe9f4 - 0x8000] = 0xea;
            drive->rom[0xe9f5 - 0x8000] = 0xea;
            drive->rom[0xeabf - 0x8000] = 0xea;
            drive->rom[0xeac0 - 0x8000] = 0xea;
            drive->rom[0xead0 - 0x8000] = 0x08;
            drive->rom[0xead9 - 0x8000] = 0x00;
        } else {
            drive->rom[0xe9f4 - 0x8000] = drive->rom_checksum[0];
            drive->rom[0xe9f5 - 0x8000] = drive->rom_checksum[1];
            drive->rom[0xeabf - 0x8000] = drive->rom_idle_trap[0];
            drive->rom[0xeac0 - 0x8000] = drive->rom_idle_trap[1];
            drive->rom[0xead0 - 0x8000] = drive->rom_idle_trap[2];
            drive->rom[0xead9 - 0x8000] = drive->rom_idle_trap[3];
        }
    }
    return 0;
}

static int set_dos_rom_name_1541(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1541, (const char *)v))
        return 0;

    return iecrom_load_1541();
}

static int set_dos_rom_name_1541ii(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1541ii, (const char *)v))
        return 0;

    return iecrom_load_1541ii();
}

static int set_dos_rom_name_1570(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1570, (const char *)v))
        return 0;

    return iecrom_load_1570();
}

static int set_dos_rom_name_1571(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1571, (const char *)v))
        return 0;

    return iecrom_load_1571();
}

static int set_dos_rom_name_1581(resource_value_t v, void *param)
{
    if (util_string_set(&dos_rom_name_1581, (const char *)v))
        return 0;

    return iecrom_load_1581();
}

static void set_drive_ram(unsigned int dnr)
{
    drive_t *drive = drive_context[dnr]->drive;

    if (drive->type == DRIVE_TYPE_NONE)
        return;

    drivemem_init(drive_context[dnr], drive->type);

    return;
}

static int set_drive_ram2(resource_value_t v, void *param)
{
    drive_t *drive = drive_context[(unsigned int)param]->drive;

    drive->drive_ram2_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram4(resource_value_t v, void *param)
{
    drive_t *drive = drive_context[(unsigned int)param]->drive;

    drive->drive_ram4_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram6(resource_value_t v, void *param)
{
    drive_t *drive = drive_context[(unsigned int)param]->drive;

    drive->drive_ram6_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_ram8(resource_value_t v, void *param)
{
    drive_t *drive = drive_context[(unsigned int)param]->drive;;

    drive->drive_ram8_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_drive_rama(resource_value_t v, void *param)
{
    drive_t *drive = drive_context[(unsigned int)param]->drive;;

    drive->drive_rama_enabled = (int)v;
    set_drive_ram((unsigned int)param);
    return 0;
}

static int set_romset_firmware(resource_value_t v, void *param)
{
    unsigned int num = (unsigned int)param;

    romset_firmware[num] = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "DosName1541", RES_STRING, (resource_value_t)"dos1541",
      RES_EVENT_NO, NULL, /* FIXME: should be same but names may differ */
      (void *)&dos_rom_name_1541, set_dos_rom_name_1541, NULL },
    { "RomsetDosName1541", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[0], set_romset_firmware, (void *)0 },
    { "DosName1541ii", RES_STRING, (resource_value_t)"d1541II",
      RES_EVENT_NO, NULL,
      (void *)&dos_rom_name_1541ii, set_dos_rom_name_1541ii, NULL },
    { "RomsetDosName1541ii", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[1], set_romset_firmware, (void *)1 },
    { "DosName1570", RES_STRING, (resource_value_t)"dos1570",
      RES_EVENT_NO, NULL,
      (void *)&dos_rom_name_1570, set_dos_rom_name_1570, NULL },
    { "RomsetDosName1570", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[2], set_romset_firmware, (void *)2 },
    { "DosName1571", RES_STRING, (resource_value_t)"dos1571",
      RES_EVENT_NO, NULL,
      (void *)&dos_rom_name_1571, set_dos_rom_name_1571, NULL },
    { "RomsetDosName1571", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[3], set_romset_firmware, (void *)3 },
    { "DosName1581", RES_STRING, (resource_value_t)"dos1581",
      RES_EVENT_NO, NULL,
      (void *)&dos_rom_name_1581, set_dos_rom_name_1581, NULL },
    { "RomsetDosName1581", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&romset_firmware[4], set_romset_firmware, (void *)4 },
    { NULL }
};

static resource_t res_drive[] = {
    { NULL, RES_INTEGER, (resource_value_t)0, 
      RES_EVENT_SAME, NULL,
      NULL, set_drive_parallel_cable_enabled, NULL },
    { NULL, RES_INTEGER, (resource_value_t)DRIVE_IDLE_TRAP_IDLE,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_idling_method, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_ram2, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_ram4, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_ram6, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_ram8, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      NULL, set_drive_rama, NULL },
    { NULL }
};

int iec_resources_init(void)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        res_drive[0].name = lib_msprintf("Drive%iParallelCable", dnr + 8);
        res_drive[0].value_ptr = (void *)&(drive->parallel_cable_enabled);
        res_drive[0].param = (void *)dnr;
        res_drive[1].name = lib_msprintf("Drive%iIdleMethod", dnr + 8);
        res_drive[1].value_ptr = (void *)&(drive->idling_method);
        res_drive[1].param = (void *)dnr;
        res_drive[2].name = lib_msprintf("Drive%iRAM2000", dnr + 8);
        res_drive[2].value_ptr = (void *)&(drive->drive_ram2_enabled);
        res_drive[2].param = (void *)dnr;
        res_drive[3].name = lib_msprintf("Drive%iRAM4000", dnr + 8);
        res_drive[3].value_ptr = (void *)&(drive->drive_ram4_enabled);
        res_drive[3].param = (void *)dnr;
        res_drive[4].name = lib_msprintf("Drive%iRAM6000", dnr + 8);
        res_drive[4].value_ptr = (void *)&(drive->drive_ram6_enabled);
        res_drive[4].param = (void *)dnr;
        res_drive[5].name = lib_msprintf("Drive%iRAM8000", dnr + 8);
        res_drive[5].value_ptr = (void *)&(drive->drive_ram8_enabled);
        res_drive[5].param = (void *)dnr;
        res_drive[6].name = lib_msprintf("Drive%iRAMA000", dnr + 8);
        res_drive[6].value_ptr = (void *)&(drive->drive_rama_enabled);
        res_drive[6].param = (void *)dnr;

        if (resources_register(res_drive) < 0)
            return -1;

        lib_free((char *)(res_drive[0].name));
        lib_free((char *)(res_drive[1].name));
        lib_free((char *)(res_drive[2].name));
        lib_free((char *)(res_drive[3].name));
        lib_free((char *)(res_drive[4].name));
        lib_free((char *)(res_drive[5].name));
        lib_free((char *)(res_drive[6].name));
    }

    return resources_register(resources);
}

void iec_resources_shutdown(void)
{
    lib_free(dos_rom_name_1541);
    lib_free(dos_rom_name_1541ii);
    lib_free(dos_rom_name_1570);
    lib_free(dos_rom_name_1571);
    lib_free(dos_rom_name_1581);
}

