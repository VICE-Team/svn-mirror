/*
 * realdevice.c - Real device access.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Michael Klein <nip@c64.org>
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

#include "log.h"
#include "realdevice.h"
#include "serial.h"
#include "types.h"
#include "vsync.h"

#include <opencbm.h>


static log_t realdevice_log = LOG_DEFAULT;

static unsigned int realdevice_enabled = 0;

static CBM_FILE realdevice_fd;


void realdevice_open(unsigned int device, BYTE secondary)
{
    vsync_suspend_speed_eval();

    cbm_open(realdevice_fd, device & 0x0f, secondary & 0x0f, NULL, 0);
}

void realdevice_close(unsigned int device, BYTE secondary)
{
    vsync_suspend_speed_eval();

    cbm_close(realdevice_fd, device & 0x0f, secondary & 0x0f);
}

void realdevice_listentalk(unsigned int device, BYTE secondary)
{
    vsync_suspend_speed_eval();

    switch (device & 0xf0) {
      case 0x20:
        cbm_listen(realdevice_fd, device & 0x0f, secondary & 0x0f);
        break;
      case 0x40:
        cbm_talk(realdevice_fd, device & 0x0f, secondary & 0x0f);
        break;
    }
}

void realdevice_unlisten(void)
{
    vsync_suspend_speed_eval();

    cbm_unlisten(realdevice_fd);
}

void realdevice_untalk(void)
{
    vsync_suspend_speed_eval();

    cbm_untalk(realdevice_fd);
}

void realdevice_write(BYTE data)
{
    BYTE st;

    vsync_suspend_speed_eval();

    st = (cbm_raw_write(realdevice_fd, &data, 1) == 1) ? 0 : 0x83;

    serial_set_st(st);
}

BYTE realdevice_read(void)
{
    BYTE st, data;

    vsync_suspend_speed_eval();

    st = (cbm_raw_read(realdevice_fd, &data, 1) == 1) ? 0 : 2;

    if (cbm_get_eoi(realdevice_fd))
        st |= 0x40;

    if (st)
        serial_set_st(st);

    return data;
}

void realdevice_init(void)
{
    realdevice_log = log_open("Real Device");
}

void realdevice_reset(void)
{
    if (realdevice_enabled)
        cbm_reset(realdevice_fd);
}

int realdevice_enable(void)
{
    if (!realdevice_enabled) {
        if (cbm_driver_open(&realdevice_fd, 0) < 0) {
            log_warning(realdevice_log,
                        "Cannot open %s, realdevice not available",
                        cbm_get_driver_name(0));
            return -1;
        }

        realdevice_enabled = 1;
        log_message(realdevice_log, "%s opened", cbm_get_driver_name(0));
    }

    return 0;
}

void realdevice_disable(void)
{
    if (realdevice_enabled) {
        cbm_driver_close(realdevice_fd);

        realdevice_enabled = 0;
        log_message(realdevice_log, "%s closed", cbm_get_driver_name(0));
    }
}

