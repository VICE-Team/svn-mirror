/*
 * iecbus.c - IEC bus handling.
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

#include "cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecbus.h"
#include "iecdrive.h"
#include "via.h"
#include "types.h"


void (*iecbus_callback_write)(BYTE, CLOCK) = NULL;

iecbus_t iecbus;

static BYTE iec_old_atn = 0x10;


void iecbus_init(void)
{
    memset(&iecbus, 0xff, sizeof(iecbus_t));
    iecbus.drv_port = 0x85;
}

/* No drive is enabled.  */
static void iec_cpu_write_conf0(BYTE data, CLOCK clock)
{
    iecbus.iec_fast_1541 = data;
}

/* Only the first drive is enabled.  */
static void iec_cpu_write_conf1(BYTE data, CLOCK clock)
{
    drive_t *drive;

    drive = drive_context[0]->drive;
    drivecpu_execute(drive_context[0], clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iecbus.cpu_bus & 0x10)) {
        iec_old_atn = iecbus.cpu_bus & 0x10;
        if (drive->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[0]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[0]->cia1581);
    }

    if (drive->type != DRIVE_TYPE_1581)
        iecbus.drv_bus[8] = (((iecbus.drv_data[8] << 3) & 0x40)
                            | ((iecbus.drv_data[8] << 6)
                            & ((~iecbus.drv_data[8] ^ iecbus.cpu_bus) << 3)
                            & 0x80));
    else
        iecbus.drv_bus[8] = (((iecbus.drv_data[8] << 3) & 0x40)
                            | ((iecbus.drv_data[8] << 6)
                            & ((iecbus.drv_data[8] | iecbus.cpu_bus) << 3)
                            & 0x80));

    iec_update_ports();
}

/* Only the second drive is enabled.  */
static void iec_cpu_write_conf2(BYTE data, CLOCK clock)
{
    drive_t *drive;

    drive = drive_context[1]->drive;
    drivecpu_execute(drive_context[1], clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iecbus.cpu_bus & 0x10)) {
        iec_old_atn = iecbus.cpu_bus & 0x10;
        if (drive->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[1]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[1]->cia1581);
    }

    if (drive->type != DRIVE_TYPE_1581)
        iecbus.drv_bus[9] = (((iecbus.drv_data[9] << 3) & 0x40)
                            | ((iecbus.drv_data[9] << 6)
                            & ((~iecbus.drv_data[9] ^ iecbus.cpu_bus) << 3)
                            & 0x80));
    else
        iecbus.drv_bus[9] = (((iecbus.drv_data[9] << 3) & 0x40)
                            | ((iecbus.drv_data[9] << 6)
                            & ((iecbus.drv_data[9] | iecbus.cpu_bus) << 3)
                            & 0x80));

    iec_update_ports();
}
#if 0
/* Both drive are enabled.  */
static void iec_cpu_write_conf3(BYTE data, CLOCK clock)
{
    drive_t *drive0, *drive1;

    drive0 = drive_context[0]->drive;
    drive1 = drive_context[1]->drive;
    drivecpu_execute(drive_context[0], clock);
    drivecpu_execute(drive_context[1], clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iecbus.cpu_bus & 0x10)) {
        iec_old_atn = iecbus.cpu_bus & 0x10;

        if (drive0->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[0]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[0]->cia1581);

        if (drive1->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[1]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[1]->cia1581);
    }

    if (drive0->type != DRIVE_TYPE_1581)
        iecbus.drv_bus[8] = (((iecbus.drv_data[8] << 3) & 0x40)
                            | ((iecbus.drv_data[8] << 6)
                            & ((~iecbus.drv_data[8] ^ iecbus.cpu_bus) << 3)
                            & 0x80));
    else
        iecbus.drv_bus[8] = (((iecbus.drv_data[8] << 3) & 0x40)
                            | ((iecbus.drv_data[8] << 6)
                            & ((iecbus.drv_data[8] | iecbus.cpu_bus) << 3)
                            & 0x80));

    if (drive1->type != DRIVE_TYPE_1581)
        iecbus.drv_bus[9] = (((iecbus.drv_data[9] << 3) & 0x40)
                            | ((iecbus.drv_data[9] << 6)
                            & ((~iecbus.drv_data[9] ^ iecbus.cpu_bus) << 3)
                            & 0x80));
    else
        iecbus.drv_bus[9] = (((iecbus.drv_data[9] << 3) & 0x40)
                            | ((iecbus.drv_data[9] << 6)
                            & ((iecbus.drv_data[9] | iecbus.cpu_bus) << 3)
                            & 0x80));

    iec_update_ports();
}
#endif
static void iec_cpu_write_conf3(BYTE data, CLOCK clock)
{
/*    drive_t *drive0, *drive1;*/
    unsigned int dnr;

/*
    drive0 = drive_context[0]->drive;
    drive1 = drive_context[1]->drive;
    drivecpu_execute(drive_context[0], clock);
    drivecpu_execute(drive_context[1], clock);
*/
    drivecpu_execute_all(clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iecbus.cpu_bus & 0x10)) {
        iec_old_atn = iecbus.cpu_bus & 0x10;

        for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
            if (drive_context[dnr]->drive->type == DRIVE_TYPE_NONE)
                continue;

            if (drive_context[dnr]->drive->type != DRIVE_TYPE_1581)
                viacore_signal(drive_context[dnr]->via1d1541, VIA_SIG_CA1,
                               iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    ciacore_set_flag(drive_context[dnr]->cia1581);
        }
    }

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        unsigned int unit;

        if (drive_context[dnr]->drive->type == DRIVE_TYPE_NONE)
            continue;

        unit = dnr + 8;

        if (drive_context[dnr]->drive->type != DRIVE_TYPE_1581)
            iecbus.drv_bus[unit] = (((iecbus.drv_data[unit] << 3) & 0x40)
                                   | ((iecbus.drv_data[unit] << 6)
                                   & ((~iecbus.drv_data[unit]
                                   ^ iecbus.cpu_bus) << 3) & 0x80));
        else
            iecbus.drv_bus[unit] = (((iecbus.drv_data[unit] << 3) & 0x40)
                                   | ((iecbus.drv_data[unit] << 6)
                                   & ((iecbus.drv_data[unit]
                                   | iecbus.cpu_bus) << 3) & 0x80));
    }

    iec_update_ports();
}

void iecbus_calculate_callback_index(void)
{
    int iecbus_callback_index;

    iecbus_callback_index = (drive_context[0]->drive->enable ? 1 : 0)
                            | (drive_context[1]->drive->enable ? 2 : 0)
                            | (drive_context[2]->drive->enable ? 4 : 0)
                            | (drive_context[3]->drive->enable ? 8 : 0);

    switch (iecbus_callback_index) {
      case 0:
        iecbus_callback_write = iec_cpu_write_conf0;
        break;
      case 1:
        iecbus_callback_write = iec_cpu_write_conf1;
        break;
      case 2:
        iecbus_callback_write = iec_cpu_write_conf2;
        break;
      default:
        iecbus_callback_write = iec_cpu_write_conf3;
        break;
    }
}

void iec_cpu_undump(BYTE data)
{
    iec_update_cpu_bus(data);
    iec_old_atn = iecbus.cpu_bus & 0x10;
}

