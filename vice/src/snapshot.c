/*
 * snapshot.c - Implementation of machine snapshot files.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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
#include <unistd.h>

#include "snapshot.h"

/* ------------------------------------------------------------------------- */

char snapshot_magic_string[] = "VICE Snapshot File\032";

#define SNAPSHOT_MAGIC_LEN              19

/* ------------------------------------------------------------------------- */

int snapshot_write_byte(FILE *f, BYTE b)
{
    if (fputc(b, f) == EOF)
        return -1;

    return 0;
}

int snapshot_write_word(FILE *f, WORD w)
{
    if (snapshot_write_byte(f, w & 0xff) < 0
        || snapshot_write_byte(f, w >> 8) < 0)
        return -1;

    return 0;
}

int snapshot_write_dword(FILE *f, DWORD w)
{
    if (snapshot_write_word(f, w & 0xffff) < 0
        || snapshot_write_word(f, w >> 16) < 0)
        return -1;

    return 0;
}

int snapshot_write_padded_string(FILE *f, const char *s, BYTE pad_char,
                                 int len)
{
    int i, found_zero;
    BYTE c;

    for (i = found_zero = 0; i < len; i++) {
        if (!found_zero && s[i] == 0)
            found_zero = 1;
        c = found_zero ? (BYTE) pad_char : (BYTE) s[i];
        if (snapshot_write_byte(f, c) < 0)
            return -1;
    }

    return 0;
}

int snapshot_write_byte_array(FILE *f, BYTE *b, int len)
{
    int i;

    for (i = 0; i < len; i++)
        if (snapshot_write_byte(f, b[i]) < 0)
            return -1;

    return 0;
}

int snapshot_read_byte(FILE *f, BYTE *b_return)
{
    int c;

    c = fgetc(f);
    if (c == EOF)
        return -1;
    *b_return = (BYTE) c;
    return 0;
}

int snapshot_read_word(FILE *f, WORD *w_return)
{
    BYTE lo, hi;

    if (snapshot_read_byte(f, &lo) < 0 || snapshot_read_byte(f, &hi) < 0)
        return -1;

    *w_return = lo | (hi << 8);
    return 0;
}

int snapshot_read_dword(FILE *f, DWORD *dw_return)
{
    WORD lo, hi;

    if (snapshot_read_word(f, &lo) < 0 || snapshot_read_word(f, &hi) < 0)
        return -1;

    *dw_return = lo | (hi << 16);
    return 0;
}

int snapshot_read_byte_array(FILE *f, BYTE *b_return, int size)
{
    int i;

    for (i = 0; i < size; i++)
        if (snapshot_read_byte(f, b_return + i) < 0)
            return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */

FILE *snapshot_create(const char *filename,
                      BYTE major_version, BYTE minor_version,
                      const char *machine_name)
{
    FILE *f;

    f = fopen(filename, "wb");
    if (f == NULL)
        return NULL;

    /* Magic string.  */
    if (snapshot_write_padded_string(f, snapshot_magic_string,
                                     0, SNAPSHOT_MAGIC_LEN) == EOF)
        goto fail;

    /* Version number.  */
    if (snapshot_write_byte(f, major_version) < 0
        || snapshot_write_byte(f, minor_version) < 0)
        goto fail;

    /* Machine.  */
    if (snapshot_write_padded_string(f, machine_name, 0,
                                     SNAPSHOT_MACHINE_NAME_LEN) < 0)
        goto fail;

    return f;

fail:
    fclose(f);
    unlink(filename);
    return NULL;
}

FILE *snapshot_open(const char *filename,
                    BYTE *major_version_return, BYTE *minor_version_return,
                    char *machine_name_return)
{
    FILE *f;
    char magic[SNAPSHOT_MAGIC_LEN];

    f = fopen(filename, "rb");
    if (f == NULL)
        goto fail;

    /* Magic string.  */
    if (snapshot_read_byte_array(f, magic, SNAPSHOT_MAGIC_LEN) < 0
        || memcmp(magic, snapshot_magic_string, SNAPSHOT_MAGIC_LEN) != 0)
        goto fail;

    /* Version number.  */
    if (snapshot_read_byte(f, major_version_return) < 0
        || snapshot_read_byte(f, minor_version_return) < 0)
        goto fail;

    /* Machine.  */
    if (snapshot_read_byte_array(f, machine_name_return,
                                 SNAPSHOT_MACHINE_NAME_LEN) < 0)
        goto fail;

    return f;

fail:
    fclose(f);
    return NULL;
}

int snapshot_write_module_header(FILE *f,
                                 const char *name,
                                 BYTE major_version,
                                 BYTE minor_version)
{
    if (snapshot_write_padded_string(f, name, 0, SNAPSHOT_MODULE_NAME_LEN) < 0
        || snapshot_write_byte(f, major_version) < 0
        || snapshot_write_byte(f, minor_version) < 0)
        return -1;

    return 0;
}

int snapshot_read_module_header(FILE *f,
                                char *name_return,
                                BYTE *major_version_return,
                                BYTE *minor_version_return)
{
    if (snapshot_read_byte_array(f, name_return, SNAPSHOT_MODULE_NAME_LEN) < 0
        || snapshot_read_byte(f, major_version_return) < 0
        || snapshot_read_byte(f, minor_version_return) < 0)
        return -1;

    return 0;
}

int snapshot_close(FILE *f)
{
    if (fclose(f) == EOF)
        return -1;
    else
        return 0;
}
