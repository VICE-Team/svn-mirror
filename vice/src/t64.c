/*
 * t64.c - T64 file support.
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
#include <stdlib.h>

#include "t64.h"

#include "types.h"
#include "utils.h"
#include "zfile.h"

static const char *magic_headers[] = {
    "C64 tape image file",
    "C64S tape file",
    "C64S tape image file",
    NULL
};

/* ------------------------------------------------------------------------- */

static DWORD get_number(BYTE *p, unsigned int n)
{
    int weight, retval, i;

    weight = 1;
    retval = 0;
    for (i = 0; i < n; i++, p++) {
        retval |= *p * weight;
        weight <<= 8;
    }

    return retval;
}

static int check_magic(t64_header_t *hdr)
{
    const char **p;

    for (p = magic_headers; *p != NULL; p++)
        if (memcmp(*p, hdr->magic, strlen(*p)) == 0)
            return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

int t64_header_read(t64_header_t *hdr, FILE *fd)
{
    BYTE buf[T64_HDR_SIZE];

    if (fread(buf, T64_HDR_SIZE, 1, fd) != 1)
        return -1;

    memcpy(hdr->magic, buf + T64_HDR_MAGIC_OFFSET, T64_HDR_MAGIC_LEN);
    if (!check_magic(hdr))
        return -1;

    hdr->version = get_number(buf + T64_HDR_VERSION_OFFSET,
                              T64_HDR_VERSION_LEN);

    /* We could make a version check, but there are way too many images with
       broken version number out there for us to trust it.  */
#if 0
    if (hdr->version != 0x100)
        return -1;
#endif

    hdr->num_entries = get_number(buf + T64_HDR_NUMENTRIES_OFFSET,
                                  T64_HDR_NUMENTRIES_LEN);
    if (hdr->num_entries == 0) {
        /* XXX: The correct behavior here would be to reject it, but there
           are so many broken T64 images out there, that it's better if we
           silently suffer.  */
        hdr->num_entries = 1;
    }

    hdr->num_used = get_number(buf + T64_HDR_NUMUSED_OFFSET,
                               T64_HDR_NUMUSED_LEN);
    if (hdr->num_used > hdr->num_entries)
        return -1;

    memcpy(hdr->description, buf + T64_HDR_DESCRIPTION_OFFSET,
           T64_HDR_DESCRIPTION_LEN);

    return 0;
}

int t64_file_record_read(t64_file_record_t *rec, FILE *fd)
{
    BYTE buf[T64_REC_SIZE];

    if (fread(buf, T64_REC_SIZE, 1, fd) != 1)
        return -1;

    rec->entry_type = buf[T64_REC_ENTRYTYPE_OFFSET];
    memcpy(rec->cbm_name, buf + T64_REC_CBMNAME_OFFSET, T64_REC_CBMNAME_LEN);
    rec->cbm_type = buf[T64_REC_CBMTYPE_OFFSET];
    rec->start_addr = get_number(buf + T64_REC_STARTADDR_OFFSET,
                                 T64_REC_STARTADDR_LEN);
    rec->end_addr = get_number(buf + T64_REC_ENDADDR_OFFSET,
                               T64_REC_ENDADDR_LEN);
    rec->contents = get_number(buf + T64_REC_CONTENTS_OFFSET,
                               T64_REC_CONTENTS_LEN);

    return 0;
}

int t64_file_record_get_size(t64_file_record_t *rec)
{
    WORD size;

    size = rec->end_addr - rec->start_addr;

    return (int) size;
}

t64_t *t64_new(void)
{
    t64_t *new;

    new = xmalloc(sizeof(t64_t));

    new->file_name = NULL;
    new->fd = NULL;
    memset(&new->header, 0, sizeof(new->header));
    new->file_records = NULL;
    new->current_file_number = -1;
    new->current_file_seek_position = 0;

    return new;
}

void t64_destroy(t64_t *t64)
{
    if (t64->fd != NULL)
        zfclose(t64->fd);
    if (t64->file_name != NULL)
        free(t64->file_name);
    if (t64->file_records != NULL)
        free(t64->file_records);
    free(t64);
}

t64_t *t64_open(const char *name)
{
    FILE *fd;
    t64_t *new;
    int i;

    fd = zfopen(name, "rb");
    if (fd == NULL)
        return NULL;

    new = t64_new();
    new->fd = fd;

    if (t64_header_read(&new->header, fd) < 0) {
        t64_destroy(new);
        return NULL;
    }

    new->file_records = xmalloc(sizeof(t64_file_record_t)
                                * new->header.num_entries);

    for (i = 0; i < new->header.num_entries; i++)
        if (t64_file_record_read(new->file_records + i, fd) < 0) {
            t64_destroy(new);
            return NULL;
        }

    new->file_name = stralloc(name);
    return new;
}

int t64_close(t64_t *t64)
{
    int retval;

    if (t64->fd != NULL) {
        retval = zfclose(t64->fd);
        t64->fd = NULL;
    } else {
        retval = 0;
    }

    t64_destroy(t64);
    return retval;
}

int t64_rewind(t64_t *t64)
{
    return t64_seek_to_file(t64, 0);
}

int t64_seek_to_file(t64_t *t64, int file_number)
{
    if (t64 == NULL || file_number < 0
        || file_number >= t64->header.num_entries)
        return -1;

    t64->current_file_number = file_number;
    t64->current_file_seek_position = 0;

    return 0;
}

int t64_seek_to_next_file(t64_t *t64, int allow_rewind)
{
    int n;

    if (t64 == NULL)
        return -1;

    if (t64->current_file_number < 0)
        n = 0;
    else
        n = t64->current_file_number;

    while (1) {
        t64_file_record_t *rec;

        n++;
        if (n >= t64->header.num_entries) {
            if (allow_rewind) {
                n = 0;
                allow_rewind = 0; /* Do not let this happen again.  */
            } else {
                return -1;
            }
        }

        rec = t64->file_records + n;
        if (rec->entry_type != T64_FILE_RECORD_FREE) {
            t64->current_file_number = n;
            t64->current_file_seek_position = 0;
            return t64->current_file_number;
        }
    }
}

t64_file_record_t *t64_get_file_record(t64_t *t64, unsigned int num)
{
    if (num >= t64->header.num_entries)
        return NULL;

    return t64->file_records + num;
}

t64_file_record_t *t64_get_current_file_record(t64_t *t64)
{
    return t64_get_file_record(t64, t64->current_file_number);
}

int t64_read(t64_t *t64, BYTE *buf, int size)
{
    t64_file_record_t *rec;
    int recsize;
    long offset;
    int amount;

    if (t64->fd == NULL || t64->current_file_number < 0 || size < 0)
        return -1;

    if (size == 0)
        return 0;

    rec = t64->file_records + t64->current_file_number;

    recsize = t64_file_record_get_size(rec);
    if (t64->current_file_seek_position >= recsize)
        return -1;

    offset = rec->contents + t64->current_file_seek_position;
    if (fseek(t64->fd, offset, SEEK_SET) != 0)
        return -1;

    if (recsize < t64->current_file_seek_position + size)
        size = recsize - t64->current_file_seek_position;

    amount = fread(buf, 1, size, t64->fd);
    t64->current_file_seek_position += amount;

    return amount;
}

int t64_read_byte(t64_t *t64)
{
    int retval;
    BYTE b;

    retval = t64_read(t64, &b, 1);
    if (retval == 1)
        return b;
    else
        return EOF;
}
