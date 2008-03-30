/*
 * drive-resources.c - Hardware-level Commodore disk drive emulation,
 *                     resource module.
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

#include "drive-resources.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivemem.h"
#include "iecdrive.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "utils.h"
#include "vdrive-bam.h"

/* Is true drive emulation switched on?  */
static int drive_true_emulation;

/* What sync factor between the CPU and the drive?  If equal to
   `DRIVE_SYNC_PAL', the same as PAL machines.  If equal to
   `DRIVE_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as 65536 * drive_clk / clk_[main machine] */
static int sync_factor;

/* Name of the DOS ROMs.  */
static char *dos_rom_name_1541 = 0;
static char *dos_rom_name_1541ii = 0;
static char *dos_rom_name_1571 = 0;
static char *dos_rom_name_1581 = 0;
static char *dos_rom_name_2031 = 0;
static char *dos_rom_name_1001 = 0;

static int set_drive0_idling_method(resource_value_t v);
static int set_drive1_idling_method(resource_value_t v);
static int set_drive0_type(resource_value_t v);
static int set_drive1_type(resource_value_t v);

static int set_drive_true_emulation(resource_value_t v)
{
    drive_true_emulation = (int) v;
    if ((int) v) {
        if (drive[0].type != DRIVE_TYPE_NONE) {
            drive[0].enable = 1;
            drive_cpu_reset_clk(&drive0_context);
        }
        if (drive[1].type != DRIVE_TYPE_NONE) {
            drive[1].enable = 1;
            drive_cpu_reset_clk(&drive1_context);
        }
        drive_enable(0);
        drive_enable(1);
        iec_calculate_callback_index();
    } else {
        drive_disable(0);
        drive_disable(1);

        /* update BAM after true drive emulation having probably
           changed the BAM on disk (14May1999) */
        if (drive[0].image != NULL) {
            vdrive_bam_reread_bam(8);
        }
        if (drive[1].image != NULL) {
            vdrive_bam_reread_bam(9);
        }
    }
    return 0;
}

static int set_drive0_type(resource_value_t v)
{
    int type = (int) v;
    int busses = iec_available_busses();

    /* if bus for drive type is not allowed, set to default value for bus */
    if (!drive_match_bus(type, 0, busses)) {
	if (busses & IEC_BUS_IEC) {
	    type = DRIVE_TYPE_1541;
	} else
	if (busses & IEC_BUS_IEEE) {
	    type = DRIVE_TYPE_2031;
	} else
	    type = DRIVE_TYPE_NONE;
    }

    if (DRIVE_IS_DUAL(type)) {
	/* dual disk drives disable second emulated unit */

        log_warning(drive[0].log,
			"Dual disk drive disables second emulated drive");

	set_drive1_type((resource_value_t) DRIVE_TYPE_NONE);
    }

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        if (drive[0].type != type) {
	    drive[0].current_half_track = 2 * 18;
	    if ((type == DRIVE_TYPE_1001)
		|| (type == DRIVE_TYPE_8050)
		|| (type == DRIVE_TYPE_8250)) {
	        drive[0].current_half_track = 2 * 38;
	    }
        }
        drive[0].type = type;
        if (drive_true_emulation) {
            drive[0].enable = 1;
            drive_enable(0);
            iec_calculate_callback_index();
        }
        drive_set_disk_drive_type(type, 0);
        drive_initialize_rom_traps(0);
        set_drive0_idling_method((resource_value_t) drive[0].idling_method);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[0].type = type;
        drive_disable(0);
        return 0;
      default:
        return -1;
    }
}

static int set_drive1_type(resource_value_t v)
{
    int type = (int) v;
    int busses = iec_available_busses();

    /* if bus for drive type is not allowed, set to default value for bus */
    if (!drive_match_bus(type, 1, busses)) {
	if (busses & IEC_BUS_IEC) {
	    type = DRIVE_TYPE_1541;
	} else
	if (busses & IEC_BUS_IEEE) {
	    type = DRIVE_TYPE_2031;
	} else
	    type = DRIVE_TYPE_NONE;
    }

    if (drive[0].enable && DRIVE_IS_DUAL(drive[0].type)) {
	/* dual disk drives disable second emulated unit */

        log_warning(drive[1].log,
			"Dual disk drive disables second emulated drive");

	type = DRIVE_TYPE_NONE;
    }

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        if (drive[1].type != type) {
	    drive[1].current_half_track = 2 * 18;
	    if ((type == DRIVE_TYPE_1001)
		|| (type == DRIVE_TYPE_8050)
		|| (type == DRIVE_TYPE_8250)) {
	        drive[1].current_half_track = 2 * 38;
	    }
        }
        drive[1].type = type;
        if (drive_true_emulation) {
            drive[1].enable = 1;
            drive_enable(1);
            iec_calculate_callback_index();
        }
        drive_set_disk_drive_type(type, 1);
        drive_initialize_rom_traps(1);
        set_drive1_idling_method((resource_value_t) drive[1].idling_method);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[1].type = type;
        drive_disable(1);
        return 0;
      default:
        return -1;
    }
}

static int set_drive0_parallel_cable_enabled(resource_value_t v)
{
    drive[0].parallel_cable_enabled = (int) v;
    return 0;
}

static int set_drive1_parallel_cable_enabled(resource_value_t v)
{
    drive[1].parallel_cable_enabled = (int) v;
    return 0;
}

static int set_drive0_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_EXTEND_NEVER:
      case DRIVE_EXTEND_ASK:
      case DRIVE_EXTEND_ACCESS:
        drive[0].extend_image_policy = (int) v;
        return 0;
      default:
        return -1;
    }
}

static int set_drive1_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_EXTEND_NEVER:
      case DRIVE_EXTEND_ASK:
      case DRIVE_EXTEND_ACCESS:
        drive[1].extend_image_policy = (int) v;
        return 0;
      default:
        return -1;
    }
}

static int set_drive0_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `drive[01]_cpu_execute()' here?  */
    if ((int) v != DRIVE_IDLE_SKIP_CYCLES
        && (int) v != DRIVE_IDLE_TRAP_IDLE
        && (int) v != DRIVE_IDLE_NO_IDLE)
        return -1;

    drive[0].idling_method = (int) v;

    if (rom_loaded && drive[0].type == DRIVE_TYPE_1541) {
        if (drive[0].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[0].rom[0xeae4 - 0x8000] = 0xea;
            drive[0].rom[0xeae5 - 0x8000] = 0xea;
            drive[0].rom[0xeae8 - 0x8000] = 0xea;
            drive[0].rom[0xeae9 - 0x8000] = 0xea;
            drive[0].rom[0xec9b - 0x8000] = 0x00;
        } else {
            drive[0].rom[0xeae4 - 0x8000] = drive[0].rom_checksum[0];
            drive[0].rom[0xeae5 - 0x8000] = drive[0].rom_checksum[1];
            drive[0].rom[0xeae8 - 0x8000] = drive[0].rom_checksum[2];
            drive[0].rom[0xeae9 - 0x8000] = drive[0].rom_checksum[3];
            drive[0].rom[0xec9b - 0x8000] = drive[0].rom_idle_trap;
        }
    }
    return 0;
}
static int set_drive1_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `drive[01]_cpu_execute()' here?  */
    if ((int) v != DRIVE_IDLE_SKIP_CYCLES
        && (int) v != DRIVE_IDLE_TRAP_IDLE
        && (int) v != DRIVE_IDLE_NO_IDLE)
        return -1;

    drive[1].idling_method = (int) v;

    if (rom_loaded && drive[1].type == DRIVE_TYPE_1541) {
        if (drive[1].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[1].rom[0xeae4 - 0x8000] = 0xea;
            drive[1].rom[0xeae5 - 0x8000] = 0xea;
            drive[1].rom[0xeae8 - 0x8000] = 0xea;
            drive[1].rom[0xeae9 - 0x8000] = 0xea;
            drive[1].rom[0xec9b - 0x8000] = 0x00;
        } else {
            drive[1].rom[0xeae4 - 0x8000] = drive[1].rom_checksum[0];
            drive[1].rom[0xeae5 - 0x8000] = drive[1].rom_checksum[1];
            drive[1].rom[0xeae8 - 0x8000] = drive[1].rom_checksum[2];
            drive[1].rom[0xeae9 - 0x8000] = drive[1].rom_checksum[3];
            drive[1].rom[0xec9b - 0x8000] = drive[1].rom_idle_trap;
        }
    }
    return 0;
}

static int set_sync_factor(resource_value_t v)
{
    int change_timing = 0;

    if (sync_factor != (int)v)
        change_timing = 1;

    switch ((int) v) {
      case DRIVE_SYNC_PAL:
        sync_factor = (int) v;
        drive_set_pal_sync_factor();
        if (change_timing)
            machine_change_timing(DRIVE_SYNC_PAL);
        break;
      case DRIVE_SYNC_NTSC:
        sync_factor = (int) v;
        drive_set_ntsc_sync_factor();
        if (change_timing)
            machine_change_timing(DRIVE_SYNC_NTSC);
        break;
      case DRIVE_SYNC_NTSCOLD:
        sync_factor = (int) v;
        drive_set_ntsc_sync_factor();
        if (change_timing)
            machine_change_timing(DRIVE_SYNC_NTSCOLD);
        break;
      default:
        if ((int) v > 0) {
            sync_factor = (int) v;
            drive_set_sync_factor((unsigned int) v);
        } else {
            return -1;
        }
    }
    return 0;
}

static int set_dos_rom_name_1001(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1001 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1001) == 0)
        return 0;

    string_set(&dos_rom_name_1001, name);

    return drive_load_1001();
}

static int set_dos_rom_name_2031(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_2031 != NULL && name != NULL
        && strcmp(name, dos_rom_name_2031) == 0)
        return 0;

    string_set(&dos_rom_name_2031, name);

    return drive_load_2031();
}

static int set_dos_rom_name_1541(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1541 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1541) == 0)
        return 0;

    string_set(&dos_rom_name_1541, name);

    return drive_load_1541();
}

static int set_dos_rom_name_1541ii(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1541ii != NULL && name != NULL
        && strcmp(name, dos_rom_name_1541ii) == 0)
        return 0;

    string_set(&dos_rom_name_1541ii, name);

    return drive_load_1541ii();
}

static int set_dos_rom_name_1571(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1571 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1571) == 0)
        return 0;

    string_set(&dos_rom_name_1571, name);

    return drive_load_1571();
}

static int set_dos_rom_name_1581(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1581 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1581) == 0)
        return 0;

    string_set(&dos_rom_name_1581, name);

    return drive_load_1581();
}

static int set_drive0_ram2(resource_value_t v)
{
    drive[0].drive_ram2_enabled = (int) v;
    drive_mem_init(&drive0_context, drive[0].type);
    return 0;
}

static int set_drive1_ram2(resource_value_t v)
{
    drive[1].drive_ram2_enabled = (int) v;
    drive_mem_init(&drive1_context, drive[1].type);
    return 0;
}

static int set_drive0_ram4(resource_value_t v)
{
    drive[0].drive_ram4_enabled = (int) v;
    drive_mem_init(&drive0_context, drive[0].type);
    return 0;
}

static int set_drive1_ram4(resource_value_t v)
{
    drive[1].drive_ram4_enabled = (int) v;
    drive_mem_init(&drive1_context, drive[1].type);
    return 0;
}

static int set_drive0_ram6(resource_value_t v)
{
    drive[0].drive_ram6_enabled = (int) v;
    drive_mem_init(&drive0_context, drive[0].type);
    return 0;
}

static int set_drive1_ram6(resource_value_t v)
{
    drive[1].drive_ram6_enabled = (int) v;
    drive_mem_init(&drive1_context, drive[1].type);
    return 0;
}

static int set_drive0_ram8(resource_value_t v)
{
    drive[0].drive_ram8_enabled = (int) v;
    drive_mem_init(&drive0_context, drive[0].type);
    return 0;
}

static int set_drive1_ram8(resource_value_t v)
{
    drive[1].drive_ram8_enabled = (int) v;
    drive_mem_init(&drive1_context, drive[1].type);
    return 0;
}

static int set_drive0_rama(resource_value_t v)
{
    drive[0].drive_rama_enabled = (int) v;
    drive_mem_init(&drive0_context, drive[0].type);
    return 0;
}

static int set_drive1_rama(resource_value_t v)
{
    drive[1].drive_rama_enabled = (int) v;
    drive_mem_init(&drive1_context, drive[1].type);
    return 0;
}

static resource_t resources[] = {
    { "DriveTrueEmulation", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_true_emulation, set_drive_true_emulation },
    { "Drive8Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_1541,
      (resource_value_t *) &(drive[0].type), set_drive0_type },
    { "Drive9Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_NONE,
      (resource_value_t *) &(drive[1].type), set_drive1_type },
    { "Drive8ParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].parallel_cable_enabled),
       set_drive0_parallel_cable_enabled },
    { "Drive9ParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].parallel_cable_enabled),
       set_drive1_parallel_cable_enabled },
    { "Drive8ExtendImagePolicy", RES_INTEGER,
      (resource_value_t) DRIVE_EXTEND_NEVER, (resource_value_t *)
      &(drive[0].extend_image_policy), set_drive0_extend_image_policy },
    { "Drive9ExtendImagePolicy", RES_INTEGER,
      (resource_value_t) DRIVE_EXTEND_NEVER, (resource_value_t *)
      &(drive[1].extend_image_policy), set_drive1_extend_image_policy },
    { "Drive8IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[0].idling_method), set_drive0_idling_method },
    { "Drive9IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[1].idling_method), set_drive1_idling_method },
    { "VideoStandard", RES_INTEGER, (resource_value_t) DRIVE_SYNC_PAL,
      (resource_value_t *) &sync_factor, set_sync_factor },
    { "DosName1541", RES_STRING, (resource_value_t) "dos1541",
      (resource_value_t *) &dos_rom_name_1541, set_dos_rom_name_1541 },
    { "DosName1541ii", RES_STRING, (resource_value_t) "d1541II",
      (resource_value_t *) &dos_rom_name_1541ii, set_dos_rom_name_1541ii },
    { "DosName1571", RES_STRING, (resource_value_t) "dos1571",
      (resource_value_t *) &dos_rom_name_1571, set_dos_rom_name_1571 },
    { "DosName1581", RES_STRING, (resource_value_t) "dos1581",
      (resource_value_t *) &dos_rom_name_1581, set_dos_rom_name_1581 },
    { "DosName2031", RES_STRING, (resource_value_t) "dos2031",
      (resource_value_t *) &dos_rom_name_2031, set_dos_rom_name_2031 },
    { "DosName1001", RES_STRING, (resource_value_t) "dos1001",
      (resource_value_t *) &dos_rom_name_1001, set_dos_rom_name_1001 },
    { "Drive8RAM2000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].drive_ram2_enabled), set_drive0_ram2 },
    { "Drive9RAM2000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].drive_ram2_enabled), set_drive1_ram2 },
    { "Drive8RAM4000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].drive_ram4_enabled), set_drive0_ram4 },
    { "Drive9RAM4000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].drive_ram4_enabled), set_drive1_ram4 },
    { "Drive8RAM6000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].drive_ram6_enabled), set_drive0_ram6 },
    { "Drive9RAM6000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].drive_ram6_enabled), set_drive1_ram6 },
    { "Drive8RAM8000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].drive_ram8_enabled), set_drive0_ram8 },
    { "Drive9RAM8000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].drive_ram8_enabled), set_drive1_ram8 },
    { "Drive8RAMA000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].drive_rama_enabled), set_drive0_rama },
    { "Drive9RAMA000", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].drive_rama_enabled), set_drive1_rama },
    { NULL }
};

int drive_init_resources(void)
{
    return resources_register(resources);
}

