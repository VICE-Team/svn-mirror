/*
 * soundfs.c - Implementation of the filesystem dump sound device
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "sound.h"

static FILE *fs_fd = NULL;

static int fs_init(warn_t *w, char *param, int *speed,
		   int *fragsize, int *fragnr, double bufsize)
{
    if (!param)
	param = "vicesnd.raw";
    fs_fd = fopen(param, "w");
    if (!fs_fd)
	return 1;
    return 0;
}

static int fs_write(warn_t *w, SWORD *pbuf, int nr)
{
    int			i;
    i = fwrite(pbuf, sizeof(SWORD), nr, fs_fd);
    if (i != nr)
	return 1;
    return 0;
}

static void fs_close(warn_t *w)
{
    fclose(fs_fd);
    fs_fd = NULL;
}

static sound_device_t fs_device =
{
    "fs",
    fs_init,
    fs_write,
    NULL,
    NULL,
    NULL,
    fs_close,
    NULL,
    NULL
};

int sound_init_fs_device(void)
{
    return sound_register_device(&fs_device);
}
