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

#include "drive.h"
#include "drivetypes.h"
#include "driverom.h"
#include "ieee-resources.h"
#include "ieeerom.h"
#include "lib.h"
#include "resources.h"
#include "traps.h"
#include "util.h"


static int romset_firmware[5];

static char *dos_rom_name_2031 = NULL;
static char *dos_rom_name_1001 = NULL;
static char *dos_rom_name_2040 = NULL;
static char *dos_rom_name_3040 = NULL;
static char *dos_rom_name_4040 = NULL;

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

    if (!rom_loaded) {
        return 0;
    }

    driverom_initialize_traps(drive, 0);
    return 0;
}

static int set_dos_rom_name_2040(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_2040, val))
        return 0;

    return ieeerom_load_2040();
}

static int set_dos_rom_name_3040(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_3040, val))
        return 0;

    return ieeerom_load_3040();
}

static int set_dos_rom_name_4040(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_4040, val))
        return 0;

    return ieeerom_load_4040();
}

static int set_dos_rom_name_1001(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_1001, val))
        return 0;

    return ieeerom_load_1001();
}

static int set_dos_rom_name_2031(const char *val, void *param)
{
    if (util_string_set(&dos_rom_name_2031, val))
        return 0;

    return ieeerom_load_2031();
}

static int set_romset_firmware(int val, void *param)
{
    unsigned int num = vice_ptr_to_uint(param);

    romset_firmware[num] = val;

    return 0;
}

static const resource_string_t resources_string[] = {
    { "DosName2031", "dos2031", RES_EVENT_NO, NULL,
      /* FIXME: should be same but names may differ */
      &dos_rom_name_2031, set_dos_rom_name_2031, NULL },
    { "DosName2040", "dos2040", RES_EVENT_NO, NULL,
      &dos_rom_name_2040, set_dos_rom_name_2040, NULL },
    { "DosName3040", "dos3040", RES_EVENT_NO, NULL,
      &dos_rom_name_3040, set_dos_rom_name_3040, NULL },
    { "DosName4040", "dos4040", RES_EVENT_NO, NULL,
      &dos_rom_name_4040, set_dos_rom_name_4040, NULL },
    { "DosName1001", "dos1001", RES_EVENT_NO, NULL,
      &dos_rom_name_1001, set_dos_rom_name_1001, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "RomsetDosName2031", 0, RES_EVENT_NO, NULL,
      &romset_firmware[0], set_romset_firmware, (void *)0 },
    { "RomsetDosName2040", 0, RES_EVENT_NO, NULL,
      &romset_firmware[1], set_romset_firmware, (void *)1 },
    { "RomsetDosName3040", 0, RES_EVENT_NO, NULL,
      &romset_firmware[2], set_romset_firmware, (void *)2 },
    { "RomsetDosName4040", 0, RES_EVENT_NO, NULL,
      &romset_firmware[3], set_romset_firmware, (void *)3 },
    { "RomsetDosName1001", 0, RES_EVENT_NO, NULL,
      &romset_firmware[4], set_romset_firmware, (void *)4 },
    { NULL }
};

static resource_int_t res_drive[] = {
    { NULL, DRIVE_IDLE_TRAP_IDLE, RES_EVENT_SAME, NULL,
      NULL, set_drive_idling_method, NULL },
    { NULL }
};

int ieee_resources_init(void)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        res_drive[0].name = lib_msprintf("Drive%iIdleMethod", dnr + 8);
        res_drive[0].value_ptr = &(drive->idling_method);
        res_drive[0].param = uint_to_void_ptr(dnr);

        if (resources_register_int(res_drive) < 0)
            return -1;

        lib_free((char *)(res_drive[0].name));
    }

    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void ieee_resources_shutdown(void)
{
    lib_free(dos_rom_name_2031);
    lib_free(dos_rom_name_1001);
    lib_free(dos_rom_name_2040);
    lib_free(dos_rom_name_3040);
    lib_free(dos_rom_name_4040);
}

