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


static void set_drive_ram(unsigned int dnr)
{
    drive_t *drive = drive_context[dnr]->drive;

    if (drive->type == DRIVE_TYPE_NONE)
        return;

    drivemem_init(drive_context[dnr], drive->type);

    return;
}

static int set_drive_idling_method(int val, void *param)
{
    unsigned int dnr;
    drive_t *drive;

    dnr = vice_ptr_to_uint(param);
    drive = drive_context[dnr]->drive;

    /* FIXME: Maybe we should call `drive_cpu_execute()' here?  */
    if (val != DRIVE_IDLE_SKIP_CYCLES
        && val != DRIVE_IDLE_TRAP_IDLE
        && val != DRIVE_IDLE_NO_IDLE)
        return -1;

    drive->idling_method = val;

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

static int set_dos_rom_name_1541(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1541, val))
        return 0;

    return iecrom_load_1541();
}

static int set_dos_rom_name_1541ii(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1541ii, val))
        return 0;

    return iecrom_load_1541ii();
}

static int set_dos_rom_name_1570(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1570, val))
        return 0;

    return iecrom_load_1570();
}

static int set_dos_rom_name_1571(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1571, val))
        return 0;

    return iecrom_load_1571();
}

static int set_dos_rom_name_1581(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1581, val))
        return 0;

    return iecrom_load_1581();
}

static int set_drive_ram2(int val, void *param)
{
    drive_t *drive = drive_context[vice_ptr_to_uint(param)]->drive;

    drive->drive_ram2_enabled = val;
    set_drive_ram(vice_ptr_to_uint(param));
    return 0;
}

static int set_drive_ram4(int val, void *param)
{
    drive_t *drive = drive_context[vice_ptr_to_uint(param)]->drive;

    drive->drive_ram4_enabled = val;
    set_drive_ram(vice_ptr_to_uint(param));
    return 0;
}

static int set_drive_ram6(int val, void *param)
{
    drive_t *drive = drive_context[vice_ptr_to_uint(param)]->drive;

    drive->drive_ram6_enabled = val;
    set_drive_ram(vice_ptr_to_uint(param));
    return 0;
}

static int set_drive_ram8(int val, void *param)
{
    drive_t *drive = drive_context[vice_ptr_to_uint(param)]->drive;;

    drive->drive_ram8_enabled = val;
    set_drive_ram(vice_ptr_to_uint(param));
    return 0;
}

static int set_drive_rama(int val, void *param)
{
    drive_t *drive = drive_context[vice_ptr_to_uint(param)]->drive;;

    drive->drive_rama_enabled = val;
    set_drive_ram(vice_ptr_to_uint(param));
    return 0;
}

static int set_romset_firmware(int val, void *param)
{
    unsigned int num = vice_ptr_to_uint(param);

    romset_firmware[num] = val;

    return 0;
}

static const resource_string_t resources_string[] = {
    { "DosName1541", "dos1541", RES_EVENT_NO, NULL,
      /* FIXME: should be same but names may differ */
      &dos_rom_name_1541, set_dos_rom_name_1541, NULL },
    { "DosName1541ii", "d1541II", RES_EVENT_NO, NULL,
      &dos_rom_name_1541ii, set_dos_rom_name_1541ii, NULL },
    { "DosName1570", "dos1570", RES_EVENT_NO, NULL,
      &dos_rom_name_1570, set_dos_rom_name_1570, NULL },
    { "DosName1571", "dos1571", RES_EVENT_NO, NULL,
      &dos_rom_name_1571, set_dos_rom_name_1571, NULL },
    { "DosName1581", "dos1581", RES_EVENT_NO, NULL,
      &dos_rom_name_1581, set_dos_rom_name_1581, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "RomsetDosName1541", 0, RES_EVENT_NO, NULL,
      &romset_firmware[0], set_romset_firmware, (void *)0 },
    { "RomsetDosName1541ii", 0, RES_EVENT_NO, NULL,
      &romset_firmware[1], set_romset_firmware, (void *)1 },
    { "RomsetDosName1570", 0, RES_EVENT_NO, NULL,
      &romset_firmware[2], set_romset_firmware, (void *)2 },
    { "RomsetDosName1571", 0, RES_EVENT_NO, NULL,
      &romset_firmware[3], set_romset_firmware, (void *)3 },
    { "RomsetDosName1581", 0, RES_EVENT_NO, NULL,
      &romset_firmware[4], set_romset_firmware, (void *)4 },
    { NULL }
};

static resource_int_t res_drive[] = {
    { NULL, DRIVE_IDLE_TRAP_IDLE, RES_EVENT_SAME, NULL,
      NULL, set_drive_idling_method, NULL },
    { NULL, 0, RES_EVENT_SAME, NULL,
      NULL, set_drive_ram2, NULL },
    { NULL, 0, RES_EVENT_SAME, NULL,
      NULL, set_drive_ram4, NULL },
    { NULL, 0, RES_EVENT_SAME, NULL,
      NULL, set_drive_ram6, NULL },
    { NULL, 0, RES_EVENT_SAME, NULL,
      NULL, set_drive_ram8, NULL },
    { NULL, 0, RES_EVENT_SAME, NULL,
      NULL, set_drive_rama, NULL },
    { NULL }
};

int iec_resources_init(void)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        res_drive[0].name = lib_msprintf("Drive%iIdleMethod", dnr + 8);
        res_drive[0].value_ptr = &(drive->idling_method);
        res_drive[0].param = uint_to_void_ptr(dnr);
        res_drive[1].name = lib_msprintf("Drive%iRAM2000", dnr + 8);
        res_drive[1].value_ptr = &(drive->drive_ram2_enabled);
        res_drive[1].param = uint_to_void_ptr(dnr);
        res_drive[2].name = lib_msprintf("Drive%iRAM4000", dnr + 8);
        res_drive[2].value_ptr = &(drive->drive_ram4_enabled);
        res_drive[2].param = uint_to_void_ptr(dnr);
        res_drive[3].name = lib_msprintf("Drive%iRAM6000", dnr + 8);
        res_drive[3].value_ptr = &(drive->drive_ram6_enabled);
        res_drive[3].param = uint_to_void_ptr(dnr);
        res_drive[4].name = lib_msprintf("Drive%iRAM8000", dnr + 8);
        res_drive[4].value_ptr = &(drive->drive_ram8_enabled);
        res_drive[4].param = uint_to_void_ptr(dnr);
        res_drive[5].name = lib_msprintf("Drive%iRAMA000", dnr + 8);
        res_drive[5].value_ptr = &(drive->drive_rama_enabled);
        res_drive[5].param = uint_to_void_ptr(dnr);

        if (resources_register_int(res_drive) < 0)
            return -1;

        lib_free((char *)(res_drive[0].name));
        lib_free((char *)(res_drive[1].name));
        lib_free((char *)(res_drive[2].name));
        lib_free((char *)(res_drive[3].name));
        lib_free((char *)(res_drive[4].name));
        lib_free((char *)(res_drive[5].name));
    }

    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void iec_resources_shutdown(void)
{
    lib_free(dos_rom_name_1541);
    lib_free(dos_rom_name_1541ii);
    lib_free(dos_rom_name_1570);
    lib_free(dos_rom_name_1571);
    lib_free(dos_rom_name_1581);
}

