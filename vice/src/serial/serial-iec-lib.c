/*
 * serial-iec-lib.c
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

#include <stdio.h>
#include <stdlib.h>

#include "serial-iec.h"
#include "types.h"
#include "utils.h"


int serial_iec_lib_directory(unsigned int unit, const char *pattern,
                             BYTE **buf)
{
    int length, status;
    BYTE data, *tmpbuf;
    unsigned int maxlen = 0;

    serial_iec_open(unit, 0, pattern, strlen(pattern));

    length = 0;
    tmpbuf = NULL;

    do {
        status = serial_iec_read(unit, 0, &data);
        tmpbuf = util_bufcat(tmpbuf, &length, &maxlen, &data, sizeof(BYTE));
    } while (status == 0);

    serial_iec_close(unit, 0);
    *buf = tmpbuf;

    return length;
}

int serial_iec_lib_read_sector(unsigned int unit, unsigned int track,
                               unsigned int sector, BYTE *buf)
{
    char *command;
    unsigned int i;

    command = xmsprintf("U1 2 0 %i %i", track, sector);

    serial_iec_open(unit, 2, "#", strlen("#"));
    serial_iec_open(unit, 15, command, strlen(command));

    for (i = 0; i < 256; i++)
        serial_iec_read(unit, 2, &buf[i]);

    serial_iec_close(unit, 15);
    serial_iec_close(unit, 2);

    free(command);

    return 0;
}

int serial_iec_lib_write_sector(unsigned int unit, unsigned int track,
                                unsigned int sector, BYTE *buf)
{
    return 0;
}

