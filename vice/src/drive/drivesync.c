/*
 * drivesync.c
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

#include <math.h>

#include "drive.h"
#include "drivesync.h"
#include "drivetypes.h"


static unsigned int sync_factor;

static void drive_sync_cpu_set_factor(drive_context_t *drv,
                                      unsigned int sync_factor)
{
    unsigned long i;

    for (i = 0; i <= MAX_TICKS; i++) {
        unsigned long tmp;

        tmp = i * (unsigned long)sync_factor;

        drv->cpud.clk_conv_table[i] = tmp / 0x10000;
        drv->cpud.clk_mod_table[i] = tmp % 0x10000;
    }
}

void drive_sync_factor(void)
{
    drive_sync_cpu_set_factor(&drive0_context,
                              drive[0].clock_frequency * sync_factor);
    drive_sync_cpu_set_factor(&drive1_context,
                              drive[1].clock_frequency * sync_factor);
}

void drive_set_machine_parameter(long cycles_per_sec)
{
    sync_factor = (unsigned int)floor(65536.0 * (1000000.0
                  / ((double)cycles_per_sec)));

    drive_sync_factor();
}

void drive_sync_set_1571(int new_sync, unsigned int dnr)
{
    if (rom_loaded) {
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        drive_initialize_rotation(new_sync ? 1 : 0, dnr);
        drive[dnr].clock_frequency = (new_sync) ? 2 : 1;
        drive_sync_factor();
    }
}

void drive_sync_clock_frequency(unsigned int type, unsigned int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1551:
        drive[dnr].clock_frequency = 2;
        break;
      case DRIVE_TYPE_1581:
        drive[dnr].clock_frequency = 2;
        break;
      case DRIVE_TYPE_2031:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        drive[dnr].clock_frequency = 1;
        break;
      default:
        drive[dnr].clock_frequency = 1;
    }
}

