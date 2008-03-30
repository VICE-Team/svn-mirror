/*
 * drive-check.c
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

#include "drive-check.h"
#include "drive.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "machine-drive.h"


static unsigned int drive_check_ieee(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        return 1;
    }

    return 0;
}

static unsigned int drive_check_iec(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1570:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1571CR:
      case DRIVE_TYPE_1581:
        return 1;
    }

    return 0;
}

unsigned int drive_check_old(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        return 1;
    }

    return 0;
}

unsigned int drive_check_dual(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        return 1;
    }

    return 0;
}

static unsigned int drive_check_tcbm(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_1551:
        return 1;
    }

    return 0;
}

unsigned int drive_check_bus(unsigned int drive_type, unsigned int dnr,
                             unsigned int bus_map)
{
    if (drive_type == DRIVE_TYPE_NONE)
        return 1;

    if (drive_check_ieee(drive_type) && (bus_map & IEC_BUS_IEEE))
        return 1;

    if (drive_check_iec(drive_type) && (bus_map & IEC_BUS_IEC))
        return 1;

    if (drive_check_tcbm(drive_type) && (bus_map & IEC_BUS_TCBM))
        return 1;

    return 0;
}

int drive_check_type(unsigned int drive_type, unsigned int dnr)
{
    if (!drive_check_bus(drive_type, dnr, iec_available_busses()))
        return 0;

    if (drive_check_dual(drive_type)) {
        if (dnr > 0) {
            /* A second dual drive is not supported.  */
            return 0;
        } else {
            if (drive_context[1]->drive->type != DRIVE_TYPE_NONE)
                /* Disable dual drive if second drive is enabled.  */
                return 0;
        }
    }

    /* If the first drive is dual no second drive is supported at all.  */
    if (drive_check_dual(drive_context[0]->drive->type) && dnr > 0)
        return 0;

    if (machine_drive_rom_check_loaded(drive_type) < 0)
        return 0;

    return 1;
}

int drive_check_extend_policy(unsigned int drive_type)
{
    if ((drive_type == DRIVE_TYPE_1541) ||
        (drive_type == DRIVE_TYPE_1541II) ||
        (drive_type == DRIVE_TYPE_1551) ||
        (drive_type == DRIVE_TYPE_1570) ||
        (drive_type == DRIVE_TYPE_1571) ||
        (drive_type == DRIVE_TYPE_1571CR) ||
        (drive_type == DRIVE_TYPE_2031))
        return 1;
    return 0;
}

int drive_check_idle_method(unsigned int drive_type)
{
    if ((drive_type == DRIVE_TYPE_1541) ||
        (drive_type == DRIVE_TYPE_1541II) ||
        (drive_type == DRIVE_TYPE_1551))
        return 1;
    return 0;
}

int drive_check_parallel_cable(unsigned int drive_type)
{
    if ((drive_type == DRIVE_TYPE_1541) ||
        (drive_type == DRIVE_TYPE_1541II))
        return 1;
    return 0;
}
