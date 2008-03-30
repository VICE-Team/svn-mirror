/*
 * drive-overflow.c
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

#include "alarm.h"
#include "clkguard.h"
#include "drive-overflow.h"
#include "drive.h"
#include "drivetypes.h"
#include "interrupt.h"
#include "log.h"
#include "rotation.h"


static void drive_clk_overflow_callback(CLOCK sub, void *data)
{
    unsigned int dnr;
    drive_t *d;

    dnr = (unsigned int)data;
    d = &drive[dnr];

    if (d->byte_ready_active == 0x06)
        rotation_rotate_disk(&drive[dnr]);

    rotation_overflow_callback(sub, dnr);

    if (d->attach_clk > (CLOCK) 0)
        d->attach_clk -= sub;
    if (d->detach_clk > (CLOCK) 0)
        d->detach_clk -= sub;
    if (d->attach_detach_clk > (CLOCK) 0)
        d->attach_detach_clk -= sub;

    /* FIXME: Having to do this by hand sucks *big time*!  These should be in
       `drive_t'.  */
    switch (dnr) {
      case 0:
        alarm_context_time_warp(drive0_context.cpu->alarm_context, sub, -1);
        interrupt_cpu_status_time_warp(drive0_context.cpu->int_status, sub, -1);
        break;
      case 1:
        alarm_context_time_warp(drive1_context.cpu->alarm_context, sub, -1);
        interrupt_cpu_status_time_warp(drive1_context.cpu->int_status, sub, -1);
        break;
      default:
        log_error(LOG_DEFAULT,
                  "Unexpected drive number %d in drive_clk_overflow_callback",
                  dnr);
    }
}

void drive_overflow_init(void)
{
    clk_guard_add_callback(drive0_context.cpu->clk_guard,
                           drive_clk_overflow_callback, (void *)0);
    clk_guard_add_callback(drive1_context.cpu->clk_guard,
                           drive_clk_overflow_callback, (void *)1);
}

