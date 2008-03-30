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

#include "cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecbus.h"
#include "iecdrive.h"
#include "via.h"
#include "types.h"


void (*iecbus_callback_write)(BYTE, CLOCK) = NULL;

iec_info_t iec_info;

static BYTE iec_old_atn = 0x10;

/* No drive is enabled.  */
static void iec_cpu_write_conf0(BYTE data, CLOCK clock)
{
    iec_info.iec_fast_1541 = data;
}

/* Only the first drive is enabled.  */
static void iec_cpu_write_conf1(BYTE data, CLOCK clock)
{
    drive_t *drive;

    drive = drive_context[0]->drive;
    drivecpu_execute(drive_context[0], clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[0]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[0]->cia1581);
    }

    if (drive->type != DRIVE_TYPE_1581)
        iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                             | ((iec_info.drive_data << 6)
                             & ((~iec_info.drive_data ^ iec_info.cpu_bus) << 3)
                             & 0x80));
    else
        iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                             | ((iec_info.drive_data << 6)
                             & ((iec_info.drive_data | iec_info.cpu_bus) << 3)
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

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive->type != DRIVE_TYPE_1581)
            viacore_signal(drive_context[1]->via1d1541, VIA_SIG_CA1,
                           iec_old_atn ? 0 : VIA_SIG_RISE);
        else
            if (!iec_old_atn)
                ciacore_set_flag(drive_context[1]->cia1581);
    }

    if (drive->type != DRIVE_TYPE_1581)
        iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((~iec_info.drive2_data ^ iec_info.cpu_bus) << 3)
                          & 0x80));
    else
        iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((iec_info.drive2_data | iec_info.cpu_bus) << 3)
                          & 0x80));

    iec_update_ports();
}

/* Both drive are enabled.  */
static void iec_cpu_write_conf3(BYTE data, CLOCK clock)
{
    drive_t *drive0, *drive1;

    drive0 = drive_context[0]->drive;
    drive1 = drive_context[1]->drive;
    drivecpu_execute(drive_context[0], clock);
    drivecpu_execute(drive_context[1], clock);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;

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
        iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                         | ((iec_info.drive_data << 6)
                         & ((~iec_info.drive_data ^ iec_info.cpu_bus) << 3)
                         & 0x80));
    else
        iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                         | ((iec_info.drive_data << 6)
                         & ((iec_info.drive_data | iec_info.cpu_bus) << 3)
                         & 0x80));

    if (drive1->type != DRIVE_TYPE_1581)
        iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                         | ((iec_info.drive2_data << 6)
                         & ((~iec_info.drive2_data ^ iec_info.cpu_bus) << 3)
                         & 0x80));
    else
        iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                         | ((iec_info.drive2_data << 6)
                         & ((iec_info.drive2_data | iec_info.cpu_bus) << 3)
                         & 0x80));

    iec_update_ports();
}

static void iec_cpu_write_conf_any(BYTE data, CLOCK clock)
{
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
      case 3:
        iecbus_callback_write = iec_cpu_write_conf3;
        break;
      default:
        iecbus_callback_write = iec_cpu_write_conf_any;
    }
}

void iec_cpu_undump(BYTE data)
{
    iec_update_cpu_bus(data);
    iec_old_atn = iec_info.cpu_bus & 0x10;
}

