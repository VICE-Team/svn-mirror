/*
 * sounddump.c - Implementation of the dump sound device
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

static FILE *dump_fd = NULL;

static int dump_init(warn_t *w, char *param, int *speed,
		     int *fragsize, int *fragnr, double bufsize)
{
    if (!param)
	param = "vicesnd.sid";
    dump_fd = fopen(param, "w");
    if (!dump_fd)
	return 1;
    return 0;
}

static int dump_write(warn_t *w, SWORD *pbuf, int nr)
{
    return 0;
}

static int dump_dump(warn_t *w, ADDRESS addr, BYTE byte, CLOCK clks)
{
    int				i;
    i = fprintf(dump_fd, "%d %d %d\n", (int)clks, addr, byte);
    if (i < 0)
	return 1;
    return 0;
}

static int dump_flush(warn_t *w, char *state)
{
    int				i;
    i = fprintf(dump_fd, "%s", state);
    if (i < 0)
	return 1;
    i = fflush(dump_fd);
    return i;
}

static void dump_close(warn_t *w)
{
    fclose(dump_fd);
    dump_fd = NULL;
}

static sound_device_t dump_device =
{
    "dump",
    dump_init,
    dump_write,
    dump_dump,
    dump_flush,
    NULL,
    dump_close,
    NULL,
    NULL
};

int sound_init_dump_device(void)
{
    return sound_register_device(&dump_device);
}
