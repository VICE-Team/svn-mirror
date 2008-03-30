/*
 * drive-resources.c - Hardware-level Commodore disk drive emulation,
 *                     resource module.
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

#include "drive-resources.h"
#include "drive.h"
#include "drivecpu.h"
#include "driverom.h"
#include "iecdrive.h"
#include "log.h"
#include "machine-drive.h"
#include "resources.h"
#include "vdrive-bam.h"


/* Is true drive emulation switched on?  */
static int drive_true_emulation;

static int set_drive0_type(resource_value_t v, void *param);
static int set_drive1_type(resource_value_t v, void *param);

static int set_drive_true_emulation(resource_value_t v, void *param)
{
    drive_true_emulation = (int)v;

    if ((int)v) {
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

static int set_drive0_type(resource_value_t v, void *param)
{
    unsigned int type;
    int busses;

    type = (unsigned int)v;
    busses = iec_available_busses();

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

        set_drive1_type((resource_value_t)DRIVE_TYPE_NONE, NULL);
    }

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1551:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
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
        drive_rom_initialize_traps(0);
        machine_drive_idling_method(0);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[0].type = type;
        drive_disable(0);
        return 0;
      default:
        return -1;
    }
}

static int set_drive1_type(resource_value_t v, void *param)
{
    unsigned int type;
    int busses;

    type = (unsigned int)v;
    busses = iec_available_busses();

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
      case DRIVE_TYPE_1551:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
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
        drive_rom_initialize_traps(1);
        machine_drive_idling_method(1);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[1].type = type;
        drive_disable(1);
        return 0;
      default:
        return -1;
    }
}

static const resource_t resources[] = {
    { "DriveTrueEmulation", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&drive_true_emulation,
      set_drive_true_emulation, NULL },
    { "Drive8Type", RES_INTEGER, (resource_value_t)DRIVE_TYPE_1541,
      (resource_value_t *)&(drive[0].type),
      set_drive0_type, NULL },
    { "Drive9Type", RES_INTEGER, (resource_value_t)DRIVE_TYPE_NONE,
      (resource_value_t *)&(drive[1].type),
      set_drive1_type, NULL },
    { NULL }
};

int drive_resources_init(void)
{
    return machine_drive_resources_init() | resources_register(resources);
}

