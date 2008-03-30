/*
 * soundesd.c - Implementation of the EsounD sound device
 *
 * Written by
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

#include "sound.h"

#include <stdio.h>
#include <unistd.h>

#include <esd.h>


static int fd_esd;

static int esd_init(const char *param, int *speed,
		    int *fragsize, int *fragnr, int *channels)
{
    esd_format_t format = ESD_STREAM | ESD_PLAY | ESD_MONO | ESD_BITS16;

    /* No stereo capability. */
    *channels = 1;

    fd_esd = esd_play_stream(format, *speed, param, "VICE");

    return fd_esd < 0;
}

static int esd_write(SWORD *pbuf, size_t nr)
{
    return write(fd_esd, pbuf, nr * sizeof(SWORD)) == 0;
}

static void esd_shutdown(void)
{
    esd_close(fd_esd);
}


static sound_device_t esd_device =
{
    "esd",
    esd_init,
    esd_write,
    NULL,
    NULL,
    NULL,
    esd_shutdown,
    NULL,
    NULL,
    1
};

int sound_init_esd_device(void)
{
    return sound_register_device(&esd_device);
}
