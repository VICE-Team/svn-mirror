/*
 * c128fastiec.c - Fast IEC bus handling for the C128.
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

#include "c128fastiec.h"
#include "c64.h"
#include "cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "maincpu.h"
#include "types.h"


static int fast_cpu_direction, fast_drive_direction[2];


void c128fastiec_init(void)
{
    fast_cpu_direction = 0;
    fast_drive_direction[0] = 1;
    fast_drive_direction[1] = 1;
}

void iec_fast_cpu_write(BYTE data)
{
    drive_t *drive0, *drive1;

    drive0 = drive_context[0]->drive;
    drive1 = drive_context[1]->drive;

    /*log_debug("CW %02x %i", data, maincpu_clk);*/

    if (fast_cpu_direction) {
        if (drive0->enable) {
            drivecpu_execute(drive_context[0], maincpu_clk);
            if (drive0->type == DRIVE_TYPE_1570
                || drive0->type == DRIVE_TYPE_1571
                || drive0->type == DRIVE_TYPE_1571CR)
                ciacore_set_sdr(drive_context[0]->cia1571, data);
            if (drive0->type == DRIVE_TYPE_1581)
                ciacore_set_sdr(drive_context[0]->cia1581, data);
        }
        if (drive1->enable) {
            drivecpu_execute(drive_context[1], maincpu_clk);
            if (drive1->type == DRIVE_TYPE_1570
                || drive1->type == DRIVE_TYPE_1571
                || drive1->type == DRIVE_TYPE_1571CR)
                ciacore_set_sdr(drive_context[1]->cia1571, data);
            if (drive1->type == DRIVE_TYPE_1581)
                ciacore_set_sdr(drive_context[1]->cia1581, data);
        }
    }
}

void iec_fast_drive_write(BYTE data, unsigned int dnr)
{
    /*log_debug("DW %02x %i", data, maincpu_clk);*/
    if (fast_drive_direction[dnr])
        ciacore_set_sdr(machine_context.cia1, data);
}

void iec_fast_cpu_direction(int direction)
{
    /* 0: input */
    fast_cpu_direction = direction;
}

void iec_fast_drive_direction(int direction, unsigned int dnr)
{
    /* 0: input */
    fast_drive_direction[dnr] = direction;
}

