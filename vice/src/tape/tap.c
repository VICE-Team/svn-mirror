/*
 * tap.c - TAP file support.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <string.h>

#include "archdep.h"
#include "datasette.h"
#include "tap.h"
#include "types.h"
#include "utils.h"
#include "zfile.h"

int tap_header_read(BYTE *version, BYTE *system, FILE *fd)
{
    BYTE buf[TAP_HDR_SIZE];

    if (fread(buf, TAP_HDR_SIZE, 1, fd) != 1)
        return -1;

    if (strncmp("C64-TAPE-RAW", (char*)&buf[TAP_HDR_MAGIC_OFFSET], 12)
        && strncmp("C16-TAPE-RAW", (char*)&buf[TAP_HDR_MAGIC_OFFSET], 12))
        return -1;

    *version = buf[TAP_HDR_VERSION];
    *system = buf[TAP_HDR_SYSTEM];

    return 0;
}

tap_t *tap_open(const char *name)
{
    FILE *fd;
    int read_only;
    tap_t *new;

    fd = zfopen(name, MODE_READ_WRITE);
    if (fd == NULL) {
        fd = zfopen(name, MODE_READ);
        if (fd == NULL)
            return NULL;
        read_only = 1;
    } else {
        read_only = 0;
    }

    new = xmalloc(sizeof(tap_t));

    new->file_name = NULL;

    if (tap_header_read(&new->version, &new->system, fd) < 0) {
        zfclose(fd);
        free(new);
        return NULL;
    }

    new->fd = fd;
    new->read_only = read_only;
    new->counter = 0;
    new->current_file_seek_position = 0;
    new->mode = DATASETTE_CONTROL_STOP;
    new->offset = TAP_HDR_SIZE;
    new->has_changed = 0;

    fseek(fd, 0, SEEK_END);
    new->size = ftell(fd) - TAP_HDR_SIZE;

    if (new->size < 3) {
        zfclose(new->fd);
        free(new);
        return NULL;
    }

    fseek(fd, new->offset, SEEK_SET);

    new->file_name = stralloc(name);

    return new;
}

int tap_close(tap_t *tap)
{
    int retval;

    if (tap->fd != NULL) {
        if (tap->has_changed)
            if (!fseek(tap->fd,TAP_HDR_LEN,SEEK_SET))
                write_dword(tap->fd, (DWORD *)&tap->size, 4);
        retval = zfclose(tap->fd);
        tap->fd = NULL;
    } else {
        retval = 0;
    }

    if (tap->file_name != NULL)
        free(tap->file_name);
    free(tap);

    return retval;
}

