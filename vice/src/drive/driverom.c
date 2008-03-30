/*
 * driverom.c
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

#include "drive.h"
#include "driverom.h"
#include "log.h"
#include "machine-drive.h"
#include "resources.h"
#include "sysfile.h"
#include "types.h"


/* Logging goes here.  */
static log_t driverom_log;

/* If nonzero, we are far enough in init that we can load ROMs.  */
int drive_rom_load_ok = 0;


int drive_rom_load_images(void)
{
    drive_rom_load_ok = 1;

    machine_drive_rom_load();

    if (machine_drive_rom_check_loaded(DRIVE_TYPE_ANY) < 0) {
        log_error(driverom_log,
                  "No ROM image found at all!  "
                  "Hardware-level emulation is not available.");
        return -1;
    }

    return 0;
}

void drive_rom_initialize_traps(drive_t *drive)
{
    if ((drive->type == DRIVE_TYPE_1541) ||
        (drive->type == DRIVE_TYPE_1541II)) {
        /* Save the ROM check.  */
        drive->rom_checksum[0] = drive->rom[0xeae4 - 0x8000];
        drive->rom_checksum[1] = drive->rom[0xeae5 - 0x8000];
        drive->rom_checksum[2] = drive->rom[0xeae8 - 0x8000];
        drive->rom_checksum[3] = drive->rom[0xeae9 - 0x8000];
        /* Save the idle trap.  */
        drive->rom_idle_trap[0] = drive->rom[0xec9b - 0x8000];

        if (drive->idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive->rom[0xeae4 - 0x8000] = 0xea;
            drive->rom[0xeae5 - 0x8000] = 0xea;
            drive->rom[0xeae8 - 0x8000] = 0xea;
            drive->rom[0xeae9 - 0x8000] = 0xea;
            drive->rom[0xec9b - 0x8000] = 0x00;
        }
    }

    if (drive->type == DRIVE_TYPE_1551) {
        /* Save the ROM check. */
        drive->rom_checksum[0] = drive->rom[0xe9f4 - 0x8000];
        drive->rom_checksum[1] = drive->rom[0xe9f5 - 0x8000];
        /* Save the idle trap changes. */
        drive->rom_idle_trap[0] = drive->rom[0xeabf - 0x8000];
        drive->rom_idle_trap[1] = drive->rom[0xeac0 - 0x8000];
        drive->rom_idle_trap[2] = drive->rom[0xead0 - 0x8000];
        drive->rom_idle_trap[3] = drive->rom[0xead9 - 0x8000];

        if (drive->idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive->rom[0xe9f4 - 0x8000] = 0xea;
            drive->rom[0xe9f5 - 0x8000] = 0xea;
            drive->rom[0xeabf - 0x8000] = 0xea;
            drive->rom[0xeac0 - 0x8000] = 0xea;
            drive->rom[0xead0 - 0x8000] = 0x08;
            drive->rom[0xead9 - 0x8000] = 0x00;
        }
    }

    if (drive->type == DRIVE_TYPE_1581) {
        drive->rom[0xaf6f - 0x8000] = 0x4c;
        drive->rom[0xaf70 - 0x8000] = 0xca;
        drive->rom[0xaf71 - 0x8000] = 0xaf;
        drive->rom[0xdaee - 0x8000] = 0x00;
    }
}

void drive_rom_init(void)
{
    driverom_log = log_open("DriveROM"); 
}

