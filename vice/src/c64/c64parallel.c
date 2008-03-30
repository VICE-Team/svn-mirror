/*
 * c64parallel.c - Parallel cable handling for the C64.
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

#include "c64.h"
#include "c64parallel.h"
#include "cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "maincpu.h"
#include "mc6821.h"
#include "types.h"
#include "via.h"


static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive_value[DRIVE_NUM] = { 0xff, 0xff, 0xff, 0xff };


static BYTE parallel_cable_value(void)
{
    unsigned int dnr;
    BYTE val;

    val = parallel_cable_cpu_value;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        if (drive_context[dnr]->drive->enable
            && drive_context[dnr]->drive->parallel_cable)
            val &= parallel_cable_drive_value[dnr];
    }

    return val;
}

void parallel_cable_drive_write(BYTE data, int handshake, unsigned int dnr)
{
#if 0
    log_debug("DW DATA %02x HS %02x", data, handshake);
#endif

    if (handshake == PARALLEL_WRITE_HS || handshake == PARALLEL_HS)
        ciacore_set_flag(machine_context.cia2);

    if (handshake == PARALLEL_WRITE_HS || handshake == PARALLEL_WRITE)
        parallel_cable_drive_value[dnr] = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    BYTE rc;

    if (handshake)
        ciacore_set_flag(machine_context.cia2);

    rc = parallel_cable_value();

#if 0
    log_debug("DR DATA %02x HS %02x", rc, handshake);
#endif

    return rc;
}

void parallel_cable_cpu_execute(void)
{
    unsigned int dnr;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        if (drive_context[dnr]->drive->enable
            && drive_context[dnr]->drive->parallel_cable)
            drivecpu_execute(drive_context[dnr], maincpu_clk);
    }
}

void parallel_cable_cpu_write(BYTE data)
{
    parallel_cable_cpu_execute();

    parallel_cable_cpu_value = data;

#if 0
    log_debug("CW DATA %02x", data);
#endif
}

BYTE parallel_cable_cpu_read(void)
{
    BYTE rc;

    parallel_cable_cpu_execute();

    rc = parallel_cable_value();

#if 0
    log_debug("CR %02x", rc);
#endif

    return rc;
}

void parallel_cable_cpu_pulse(void)
{
    unsigned int dnr;

    parallel_cable_cpu_execute();

#if 0
    log_debug("CP");
#endif

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive_t *drive;

        drive = drive_context[dnr]->drive;

        if (drive->enable && drive->parallel_cable) {
            if (drive->parallel_cable == DRIVE_PC_DD3)
                mc6821_set_signal(drive_context[dnr], MC6821_SIG_CA1);
            else if (drive->type == DRIVE_TYPE_1570
                || drive->type == DRIVE_TYPE_1571
                || drive->type == DRIVE_TYPE_1571CR)
                ciacore_set_flag(drive_context[dnr]->cia1571);
            else
                viacore_signal(drive_context[dnr]->via1d1541, VIA_SIG_CB1,
                               VIA_SIG_FALL);
        }
    }
}

void parallel_cable_cpu_undump(BYTE data)
{
    parallel_cable_cpu_value = data;
}

