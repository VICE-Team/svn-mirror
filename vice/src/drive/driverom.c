/*
 * driverom.c
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

#include "drive.h"
#include "driverom.h"
#include "log.h"
#include "resources.h"
#include "sysfile.h"
#include "types.h"


/* RAM/ROM.  */
BYTE drive_rom1541[DRIVE_ROM1541_SIZE_EXPANDED];
BYTE drive_rom1541ii[DRIVE_ROM1541II_SIZE];
BYTE drive_rom1551[DRIVE_ROM1551_SIZE];
BYTE drive_rom1571[DRIVE_ROM1571_SIZE];
BYTE drive_rom1581[DRIVE_ROM1581_SIZE];
BYTE drive_rom2031[DRIVE_ROM2031_SIZE];
BYTE drive_rom1001[DRIVE_ROM1001_SIZE];
BYTE drive_rom2040[DRIVE_ROM2040_SIZE];
BYTE drive_rom3040[DRIVE_ROM3040_SIZE];
BYTE drive_rom4040[DRIVE_ROM4040_SIZE];

/* If nonzero, the ROM image has been loaded.  */
unsigned int rom1541_loaded = 0;
unsigned int rom1541ii_loaded = 0;
unsigned int rom1551_loaded = 0;
unsigned int rom1571_loaded = 0;
unsigned int rom1581_loaded = 0;
unsigned int rom2031_loaded = 0;
unsigned int rom2040_loaded = 0;
unsigned int rom3040_loaded = 0;
unsigned int rom4040_loaded = 0;
unsigned int rom1001_loaded = 0;

unsigned int drive_rom1541_size;

/* Logging goes here.  */
static log_t driverom_log;

/* If nonzero, we are far enough in init that we can load ROMs.  */
static int drive_rom_load_ok = 0;

static void drive_new_rom_image_loaded(int dtype)
{
    if (drive[0].type == dtype)
        drive_rom_setup_image(0);
    if (drive[1].type == dtype)
        drive_rom_setup_image(1);
}

int drive_rom_do_1541_checksum(void)
{
    int i;
    unsigned long s;

    /* Calculate ROM checksum.  */
    for (i = DRIVE_ROM1541_SIZE_EXPANDED - drive_rom1541_size, s = 0;
        i < DRIVE_ROM1541_SIZE_EXPANDED; i++)
        s += drive_rom1541[i];

    if (s != DRIVE_ROM1541_CHECKSUM)
        log_warning(driverom_log, "Unknown 1541 ROM image.  Sum: %lu.", s);

    return 0;
}

int drive_rom_load_1541(void)
{
    char *rom_name = NULL;
    int filesize;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1541", (resource_value_t *)&rom_name);

    /* Load the ROMs. */
    filesize = sysfile_load(rom_name, drive_rom1541, DRIVE_ROM1541_SIZE,
                            DRIVE_ROM1541_SIZE_EXPANDED);
    if (filesize < 0) {
        log_error(driverom_log,
                  "1541 ROM image not found.  "
                  "Hardware-level 1541 emulation is not available.");
        drive_rom1541_size = 0;
    } else {
        rom1541_loaded = 1;
        drive_rom1541_size = (unsigned int)filesize;
        drive_rom_do_1541_checksum();
        drive_new_rom_image_loaded(DRIVE_TYPE_1541);
        return 0;
    }
    return -1;
}

int drive_rom_load_1541ii(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1541ii", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom1541ii, DRIVE_ROM1541II_SIZE,
                     DRIVE_ROM1541II_SIZE) < 0) {
        log_error(driverom_log,
                  "1541-II ROM image not found.  "
                  "Hardware-level 1541-II emulation is not available.");
    } else {
        rom1541ii_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_1541II);
        return 0;
    }
    return -1;
}

int drive_rom_load_1551(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1551", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom1551, DRIVE_ROM1551_SIZE,
                     DRIVE_ROM1551_SIZE) < 0) {
        log_error(driverom_log,
                  "1551 ROM image not found.  "
                  "Hardware-level 1551 emulation is not available.");
    } else {
        rom1551_loaded = 1;
        return 0;
    }
    return -1;
}

int drive_rom_load_1571(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1571", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom1571, DRIVE_ROM1571_SIZE,
                     DRIVE_ROM1571_SIZE) < 0) {
        log_error(driverom_log,
                  "1571 ROM image not found.  "
                  "Hardware-level 1571 emulation is not available.");
    } else {
        rom1571_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_1571);
        return 0;
    }
    return -1;
}

int drive_rom_load_1581(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1581", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom1581, DRIVE_ROM1581_SIZE,
                     DRIVE_ROM1581_SIZE) < 0) {
        log_error(driverom_log,
                  "1581 ROM image not found.  "
                  "Hardware-level 1581 emulation is not available.");
    } else {
        rom1581_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_1581);
        return 0;
    }
    return -1;
}

int drive_rom_load_2031(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName2031", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom2031, DRIVE_ROM2031_SIZE,
                     DRIVE_ROM2031_SIZE) < 0) {
        log_error(driverom_log,
                  "2031 ROM image not found.  "
                  "Hardware-level 2031 emulation is not available.");
    } else {
        rom2031_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_2031);
        return 0;
    }
    return -1;
}

int drive_rom_load_2040(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName2040", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom2040, DRIVE_ROM2040_SIZE,
                     DRIVE_ROM2040_SIZE) < 0) {
        log_error(driverom_log,
                  "2040 ROM image not found.  "
                  "Hardware-level 2040 emulation is not available.");
    } else {
        rom2040_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_2040);
        return 0;
    }
    return -1;
}

int drive_rom_load_3040(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName3040", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom3040, DRIVE_ROM3040_SIZE,
                     DRIVE_ROM3040_SIZE) < 0) {
        log_error(driverom_log,
                  "3040 ROM image not found.  "
                  "Hardware-level 3040 emulation is not available.");
    } else {
        rom3040_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_3040);
        return 0;
    }
    return -1;
}

int drive_rom_load_4040(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName4040", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom4040, DRIVE_ROM4040_SIZE,
                     DRIVE_ROM4040_SIZE) < 0) {
        log_error(driverom_log,
                  "4040 ROM image not found.  "
                  "Hardware-level 4040 emulation is not available.");
    } else {
        rom4040_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_4040);
        return 0;
    }
    return -1;
}

int drive_rom_load_1001(void)
{
    char *rom_name = NULL;

    if (!drive_rom_load_ok)
        return 0;

    resources_get_value("DosName1001", (resource_value_t *)&rom_name);

    if (sysfile_load(rom_name, drive_rom1001, DRIVE_ROM1001_SIZE,
                     DRIVE_ROM1001_SIZE) < 0) {
        log_error(driverom_log,
                  "1001 ROM image not found.  "
                  "Hardware-level 1001/8050/8250 emulation is not available.");
    } else {
        rom1001_loaded = 1;
        drive_new_rom_image_loaded(DRIVE_TYPE_1001);
        return 0;
    }
    return -1;
}

int drive_rom_load_images(void)
{
    drive_rom_load_ok = 1;

    drive_rom_load_1541();
    drive_rom_load_1541ii();
    drive_rom_load_1551();
    drive_rom_load_1571();
    drive_rom_load_1581();
    drive_rom_load_2031();
    drive_rom_load_2040();
    drive_rom_load_3040();
    drive_rom_load_4040();
    drive_rom_load_1001();

    if (!rom1541_loaded
        && !rom1541ii_loaded
        && !rom1551_loaded
        && !rom1571_loaded
        && !rom1581_loaded
        && !rom2031_loaded
        && !rom2040_loaded
        && !rom3040_loaded
        && !rom4040_loaded
        && !rom1001_loaded) {
        log_error(driverom_log,
                  "No ROM image found at all!  "
                  "Hardware-level emulation is not available.");
        return -1;
    }

    return 0;
}

void drive_rom_setup_image(unsigned int dnr)
{
    if (rom_loaded) {
        switch (drive[dnr].type) {
          case DRIVE_TYPE_1541:
            memcpy(drive[dnr].rom, drive_rom1541, DRIVE_ROM1541_SIZE_EXPANDED);
            break;
          case DRIVE_TYPE_1541II:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1541ii,
                   DRIVE_ROM1541II_SIZE);
            break;
          case DRIVE_TYPE_1551:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1551,
                   DRIVE_ROM1551_SIZE);
            break;
          case DRIVE_TYPE_1571:
            memcpy(drive[dnr].rom, drive_rom1571, DRIVE_ROM1571_SIZE);
            break;
          case DRIVE_TYPE_1581:
            memcpy(drive[dnr].rom, drive_rom1581, DRIVE_ROM1581_SIZE);
            break;
          case DRIVE_TYPE_2031:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom2031,
                   DRIVE_ROM2031_SIZE);
            break;
          case DRIVE_TYPE_2040:
            memcpy(&(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM2040_SIZE]),
                   drive_rom2040, DRIVE_ROM2040_SIZE);
            break;
          case DRIVE_TYPE_3040:
            memcpy(&(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM3040_SIZE]),
                   drive_rom3040, DRIVE_ROM3040_SIZE);
            break;
          case DRIVE_TYPE_4040:
            memcpy(&(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM4040_SIZE]),
                   drive_rom4040, DRIVE_ROM4040_SIZE);
            break;
          case DRIVE_TYPE_1001:
          case DRIVE_TYPE_8050:
          case DRIVE_TYPE_8250:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1001,
                   DRIVE_ROM1001_SIZE);
            break;
        }
    }
}

void drive_rom_initialize_traps(unsigned int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1541) {
        /* Save the ROM check.  */
        drive[dnr].rom_checksum[0] = drive[dnr].rom[0xeae4 - 0x8000];
        drive[dnr].rom_checksum[1] = drive[dnr].rom[0xeae5 - 0x8000];
        drive[dnr].rom_checksum[2] = drive[dnr].rom[0xeae8 - 0x8000];
        drive[dnr].rom_checksum[3] = drive[dnr].rom[0xeae9 - 0x8000];
        /* Save the idle trap.  */
        drive[dnr].rom_idle_trap = drive[dnr].rom[0xec9b - 0x8000];

        if (drive[dnr].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[dnr].rom[0xeae4 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae5 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae8 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae9 - 0x8000] = 0xea;
            drive[dnr].rom[0xec9b - 0x8000] = 0x00;
        }
    }

    if (drive[dnr].type == DRIVE_TYPE_1581) {
        /* Save the ROM check.  */
        /*drive[dnr].rom_checksum[0] = drive[dnr].rom[0xeae4 - 0x8000];
        drive[dnr].rom_checksum[1] = drive[dnr].rom[0xeae5 - 0x8000];
        drive[dnr].rom_checksum[2] = drive[dnr].rom[0xeae8 - 0x8000];
        drive[dnr].rom_checksum[3] = drive[dnr].rom[0xeae9 - 0x8000];*/
        /* Save the idle trap.  */
        /*drive[dnr].rom_idle_trap = drive[dnr].rom[0xec9b - 0x8000];*/
        drive[dnr].rom[0xaf6f - 0x8000] = 0x4c;
        drive[dnr].rom[0xaf70 - 0x8000] = 0xca;
        drive[dnr].rom[0xaf71 - 0x8000] = 0xaf;
        drive[dnr].rom[0xc0be - 0x8000] = 0x00;
    }
}

int drive_rom_check_loaded(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        if (rom1541_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1541II:
        if (rom1541ii_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1551:
        if (rom1551_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1571:
        if (rom1571_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1581:
        if (rom1581_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_2031:
        if (rom2031_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_2040:
        if (rom2040_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_3040:
        if (rom3040_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_4040:
        if (rom4040_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        if (rom1001_loaded < 1 && rom_loaded)
            return -1;
        break;
      default:
        return -1;
    }

    return 0;
}
void drive_rom_init(void)
{
    driverom_log = log_open("DriveROM"); 
}

