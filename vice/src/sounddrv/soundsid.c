/*
 * soundsid.c - Sound driver for /dev/sid
 *
 * Written by
 *  Dirk Jagdmann <doj@cubic.org>
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#include "sound.h"
#include "types.h"


static int sidfh = -1;
static BYTE buf[0x20];


static int sid_flush(char *state)
{
    if (sidfh >= 0) {
        lseek(sidfh, 0, SEEK_SET);
        write(sidfh, buf, 0x19);
    }

    return 0;
}

static int sid_init(const char *param, int *speed, int *fragsize, int *fragnr,
                    int *channels)
{
    /* No stereo capability. */
    *channels = 1;

    sidfh = open("/dev/sid", O_WRONLY);

    if (sidfh < 0)
        sidfh = open("/dev/misc/sid", O_WRONLY);

    if (sidfh < 0) {
        log_error(LOG_DEFAULT,
                  "could not open sid device /dev/sid or /dev/misc/sid\n");
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    sid_flush(0);

    return 0;
}

static void sid_close(void)
{
    if (sidfh >= 0) {
        memset(buf, 0, sizeof(buf));
        sid_flush(0);
        close(sidfh);
        sidfh = -1;
    }
}

static int sid_write(SWORD *pbuf, size_t nr)
{
    return 0;
}

static int sid_dump(ADDRESS addr, BYTE byte, CLOCK clks)
{
    if (addr < sizeof(buf))
        buf[addr] = byte;

    return 0;
}

static sound_device_t sid_device =
{
    "sid",
    sid_init,
    sid_write,
    sid_dump,
    sid_flush,
    NULL,                       /* bufferspace */
    sid_close,
    NULL,                       /* suspend */
    NULL,                       /* resume */
    0                           /* need_attenuation */
};

int sound_init_sid_device(void)
{
    atexit(sid_close);

    return sound_register_device(&sid_device);
}

