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
#include "tape.h"
#include "types.h"
#include "utils.h"
#include "zfile.h"


#define TAP_PULSE_SHORT_MIN 36
#define TAP_PULSE_SHORT_MAX 54
#define TAP_PULSE_MIDDLE_MIN 55
#define TAP_PULSE_MIDDLE_MAX 73
#define TAP_PULSE_LONG_MIN 74
#define TAP_PULSE_LONG_MAX 92

#define TAP_PULSE_SHORT(x) \
    ((x) >= tap_pulse_short_min && (x) <= tap_pulse_short_max)
#define TAP_PULSE_MIDDLE(x) \
    ((x) >= tap_pulse_middle_min && (x) <= tap_pulse_middle_max)
#define TAP_PULSE_LONG(x) \
    ((x) >= tap_pulse_long_min && (x) <= tap_pulse_long_max)

#define TAP_PILOT_HEADER_MIN 10000


static int tap_pulse_short_min;
static int tap_pulse_short_max;
static int tap_pulse_middle_min;
static int tap_pulse_middle_max;
static int tap_pulse_long_min;
static int tap_pulse_long_max;


static int tap_header_read(tap_t *tap, FILE *fd)
{
    BYTE buf[TAP_HDR_SIZE];

    if (fread(buf, TAP_HDR_SIZE, 1, fd) != 1)
        return -1;

    if (strncmp("C64-TAPE-RAW", (char *)&buf[TAP_HDR_MAGIC_OFFSET], 12)
        && strncmp("C16-TAPE-RAW", (char *)&buf[TAP_HDR_MAGIC_OFFSET], 12))
        return -1;

    tap->version = buf[TAP_HDR_VERSION];
    tap->system = buf[TAP_HDR_SYSTEM];

    memcpy(tap->name, &buf[TAP_HDR_MAGIC_OFFSET], 12);

    return 0;
}

static tap_t *tap_new(void)
{
    tap_t *tap;

    tap = xcalloc(1, sizeof(tap_t));

    tap->file_name = NULL;
    tap->counter = 0;
    tap->current_file_seek_position = 0;
    tap->mode = DATASETTE_CONTROL_STOP;
    tap->offset = TAP_HDR_SIZE;
    tap->has_changed = 0;
    tap->current_file_number = -1;

    return tap;
}

tap_t *tap_open(const char *name, unsigned int *read_only)
{
    FILE *fd;
    tap_t *new;

    fd = NULL;

    if (*read_only == 0)
        fd = zfopen(name, MODE_READ_WRITE);

    if (fd == NULL) {
        fd = zfopen(name, MODE_READ);
        if (fd == NULL)
            return NULL;
        *read_only = 1;
    } else {
        *read_only = 0;
    }

    new = tap_new();

    if (tap_header_read(new, fd) < 0) {
        zfclose(fd);
        free(new);
        return NULL;
    }

    new->fd = fd;
    new->read_only = *read_only;

    new->size = (int)util_file_length(fd) - TAP_HDR_SIZE;

    if (new->size < 3) {
        zfclose(new->fd);
        free(new);
        return NULL;
    }

    new->file_name = stralloc(name);
    new->tap_file_record = xcalloc(1, sizeof(tape_file_record_t));

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
    if (tap->tap_file_record)
        free(tap->tap_file_record);
    free(tap);

    return retval;
}

/* ------------------------------------------------------------------------- */

inline static void tap_unget_bit(tap_t *tap)
{
    fseek(tap->fd, -1, SEEK_CUR);
}

inline static int tap_get_bit(tap_t *tap)
{
    BYTE data;
    size_t res;

    res = fread(&data, 1, 1, tap->fd);

    if (res == 0)
        return -1;

    return (int)data;
}

static int tap_search_header_pilot(tap_t *tap)
{
    unsigned int i;
    BYTE data[256];
    int count;
    size_t res;

    while (1) {
        count = 0;

        while (1) {
            res = fread(&data, 256, 1, tap->fd);

            if (res < 1)
                return -1;

            for (i = 0; i < 256; i++) {
                if (!TAP_PULSE_SHORT(data[i]))
                    break;
            }
            if (i != 256)
                break;

            count += 256;

            if (count >= TAP_PILOT_HEADER_MIN)
                return 0;
        }
    }
}

static int tap_skip_pilot(tap_t *tap)
{
    int data;

    while (1) {
        data = tap_get_bit(tap);

        if (data < 0)
           return -1;

        if (TAP_PULSE_SHORT(data))
           continue;

        break;
    }

    tap_unget_bit(tap);
    return 0;
}

static int tap_read_byte(tap_t *tap)
{
    unsigned int i;
    int data, data2;
    BYTE read;

    read = 0;

    data = tap_get_bit(tap);
    if (data < 0)
        return -1;
    if (!TAP_PULSE_LONG(data))
        return -2;

    data = tap_get_bit(tap);
    if (data < 0)
        return -1;
    if (!TAP_PULSE_MIDDLE(data))
        return -2;

    for (i = 0; i < 8; i++) {
        data = tap_get_bit(tap);
        if (data < 0)
            return -1;
        data2 = tap_get_bit(tap);
        if (data2 < 0)
            return -1;
        read >>= 1;
        if (TAP_PULSE_MIDDLE(data) && TAP_PULSE_SHORT(data2))
            read |= 0x80;
    }

    data = tap_get_bit(tap);
    data = tap_get_bit(tap);

    return read;
}

static int tap_read_sync(tap_t *tap, int start)
{
    unsigned int i;
    int read;

    for (i = 0; i < 9; i++) {
        read = tap_read_byte(tap);
        if (read < 0)
            return read;
        if (read != start - i)
            return -2;
    }
    return 0;
}

static int tap_read_header(tap_t *tap)
{
    unsigned int i;
    int read, read2;

    read = tap_read_byte(tap);
    if (read < 0)
        return read;

    tap->tap_file_record->type = (BYTE)read;

    read = tap_read_byte(tap);
    if (read < 0)
        return read;
    read2 = tap_read_byte(tap);
    if (read2 < 0)
        return read2;

    tap->tap_file_record->start_addr = (ADDRESS)(read + read2 * 256);

    read = tap_read_byte(tap);
    if (read < 0)
        return read;
    read2 = tap_read_byte(tap);
    if (read2 < 0)
        return read2;

    tap->tap_file_record->end_addr = (ADDRESS)(read + read2 * 256);

    for (i = 0; i < 16; i++) {
        read = tap_read_byte(tap);
        if (read < 0)
           return read;
        tap->tap_file_record->name[i] = (BYTE)read;
    }

    return 0;
}

static int tap_search_header(tap_t *tap)
{
    int res;

    while (1) {
        if (tap_search_header_pilot(tap) < 0)
            return -1;

        if (tap_skip_pilot(tap) < 0)
            return -1;

        res = tap_read_sync(tap, 0x89);
        if (res == -1)
            return -1;
        if (res == -2)
            continue;

        res = tap_read_header(tap);
        if (res == -1)
            return -1;
        if (res == -2)
            continue;

        break;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

tape_file_record_t *tap_get_current_file_record(tap_t *tap)
{
    return tap->tap_file_record;
}

void tap_seek_start(tap_t *tap)
{
    tap->current_file_seek_position = 0;
    fseek(tap->fd, tap->offset, SEEK_SET);
}

int tap_seek_to_next_file(tap_t *tap, unsigned int allow_rewind)
{
    int res;

    if (tap == NULL)
        return -1;

    while (1) {
        res = tap_search_header(tap);

        if (res < 0) {
            if (allow_rewind) {
                tap->current_file_number = -1;
                allow_rewind = 0; /* Do not let this happen again.  */
                tap_seek_start(tap);
                continue;
            } else {
                return -1;
            }
        }
        tap->current_file_number++;
        break;
    }
    return 0;
}

void tap_get_header(tap_t *tap, BYTE *name)
{
    memcpy(name, tap->name, 12);
}

void tap_init(tape_init_t *init)
{
    tap_pulse_short_min = init->pulse_short_min / 8;
    tap_pulse_short_max = init->pulse_short_max / 8;
    tap_pulse_middle_min = init->pulse_middle_min / 8;
    tap_pulse_middle_max = init->pulse_middle_max / 8;
    tap_pulse_long_min = init->pulse_long_min / 8;
    tap_pulse_long_max = init->pulse_long_max / 8;
}

