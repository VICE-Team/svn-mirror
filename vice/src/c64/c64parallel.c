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
#include "maincpu.h"
#include "types.h"
#include "via.h"


static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive0_value = 0xff;
static BYTE parallel_cable_drive1_value = 0xff;


void parallel_cable_drive0_write(BYTE data, int handshake)
{
    if (handshake)
        ciacore_set_flag(machine_context.cia2);
    parallel_cable_drive0_value = data;
}

void parallel_cable_drive1_write(BYTE data, int handshake)
{
    if (handshake)
        ciacore_set_flag(machine_context.cia2);
    parallel_cable_drive1_value = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    if (handshake)
        ciacore_set_flag(machine_context.cia2);
    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_execute(void)
{
    unsigned int dnr;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        if (drive_context[dnr]->drive->enable
            && drive_context[dnr]->drive->parallel_cable_enabled)
            drivecpu_execute(drive_context[dnr], maincpu_clk);
    }
}

void parallel_cable_cpu_write(BYTE data)
{
    if (!(drive_context[0]->drive->enable)
        && !(drive_context[1]->drive->enable))
        return;

    parallel_cable_cpu_execute();

    parallel_cable_cpu_value = data;
}

BYTE parallel_cable_cpu_read(void)
{
    if (!(drive_context[0]->drive->enable)
        && !(drive_context[1]->drive->enable))
        return 0xff;

    parallel_cable_cpu_execute();

    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_pulse(void)
{
    unsigned int dnr;

    if (!(drive_context[0]->drive->enable)
        && !(drive_context[1]->drive->enable))
        return;

    parallel_cable_cpu_execute();

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        if (drive_context[dnr]->drive->enable
            && drive_context[dnr]->drive->parallel_cable_enabled)
            viacore_signal(drive_context[dnr]->via1d1541, VIA_SIG_CB1,
                           VIA_SIG_FALL);
    }
}

void parallel_cable_cpu_undump(BYTE data)
{
    parallel_cable_cpu_value = data;
}

