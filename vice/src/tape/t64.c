/*
 * t64.c - T64 file support.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <string.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "t64.h"
#include "types.h"
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
    DWORD retval;
    unsigned int weight, i;

    weight = 1;
    retval = 0;

    for (i = 0; i < n; i++, p++) {
        retval |= (DWORD)(*p * weight);
        weight <<= 8;
    }

    return retval;
}

static int check_magic(t64_header_t *hdr)
{
    const char **p;

    for (p = magic_headers; *p != NULL; p++) {
        if (memcmp(*p, hdr->magic, strlen(*p)) == 0) {
            return 1;
        }
    }

    return 0;
}

/* compar argument to qsort() call in t64_open to sort records on content
 * member */
static int comp_content(const void *p1, const void *p2)
{
    const t64_file_record_t *r1 = p1;
    const t64_file_record_t *r2 = p2;

    if (r1->contents < r2->contents) {
        return -1;
    } else if (r1->contents > r2->contents) {
        return 1;
    } else {
        return 0;
    }
}


/* compar argument to qsort() call in t64_open to sort records on their
 * original position in the container */
static int comp_index(const void *p1, const void *p2)
{
    const t64_file_record_t *r1 = p1;
    const t64_file_record_t *r2 = p2;

    if (r1->index < r2->index) {
        return -1;
    } else if (r1->index > r2->index) {
        return 1;
    } else {
        return 0;
    }
}



/* ------------------------------------------------------------------------- */

int t64_header_read(t64_header_t *hdr, FILE *fd)
{
    BYTE buf[T64_HDR_SIZE];

    if (fread(buf, T64_HDR_SIZE, 1, fd) != 1) {
        return -1;
    }

    memcpy(hdr->magic, buf + T64_HDR_MAGIC_OFFSET, T64_HDR_MAGIC_LEN);
    if (!check_magic(hdr)) {
        return -1;
    }

    hdr->version = (WORD)get_number(buf + T64_HDR_VERSION_OFFSET,
                                    (unsigned int)T64_HDR_VERSION_LEN);

    /* We could make a version check, but there are way too many images with
       broken version number out there for us to trust it.  */
#if 0
    if (hdr->version != 0x100) {
        return -1;
    }
#endif

    hdr->num_entries = (WORD)get_number(buf + T64_HDR_NUMENTRIES_OFFSET,
                                        (unsigned int)T64_HDR_NUMENTRIES_LEN);
    if (hdr->num_entries == 0) {
        /* XXX: The correct behavior here would be to reject it, but there
           are so many broken T64 images out there, that it's better if we
           silently suffer.  */
        log_warning(LOG_DEFAULT,
                "t64 image reports 0 max entries, adjusting to 1");
        hdr->num_entries = 1;
    }

    hdr->num_used = (WORD)get_number(buf + T64_HDR_NUMUSED_OFFSET,
                                     (unsigned int)T64_HDR_NUMUSED_LEN);
    if (hdr->num_used == 0) {
        /* corrupt tape image */
        log_warning(LOG_DEFAULT,
                "t64 image reports 0 used entries, adjusting to 1");
        hdr->num_used = 1;
    }

    if (hdr->num_used > hdr->num_entries) {
        return -1;
    }

    memcpy(hdr->description, buf + T64_HDR_DESCRIPTION_OFFSET,
           T64_HDR_DESCRIPTION_LEN);

    return 0;
}

int t64_file_record_read(t64_file_record_t *rec, FILE *fd)
{
    BYTE buf[T64_REC_SIZE];

    if (fread(buf, T64_REC_SIZE, 1, fd) != 1) {
        return -1;
    }

    rec->entry_type = buf[T64_REC_ENTRYTYPE_OFFSET];
    memcpy(rec->cbm_name, buf + T64_REC_CBMNAME_OFFSET, T64_REC_CBMNAME_LEN);
    rec->cbm_type = buf[T64_REC_CBMTYPE_OFFSET];
    rec->start_addr = (WORD)get_number(buf + T64_REC_STARTADDR_OFFSET, (unsigned int)T64_REC_STARTADDR_LEN);
    rec->end_addr = (WORD)get_number(buf + T64_REC_ENDADDR_OFFSET, (unsigned int)T64_REC_ENDADDR_LEN);
    rec->contents = get_number(buf + T64_REC_CONTENTS_OFFSET, T64_REC_CONTENTS_LEN);

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
    t64_t *new64;

    new64 = lib_calloc(1, sizeof(t64_t));

    new64->file_name = NULL;
    new64->fd = NULL;
    new64->file_records = NULL;
    new64->current_file_number = -1;
    new64->current_file_seek_position = 0;

    return new64;
}

void t64_destroy(t64_t *t64)
{
    if (t64->fd != NULL) {
        zfile_fclose(t64->fd);
    }
    lib_free(t64->file_name);
    lib_free(t64->file_records);
    lib_free(t64);
}

t64_t *t64_open(const char *name, unsigned int *read_only)
{
    FILE *fd;
    t64_t *new;
    int i;
    long tapesize;   /* file size in bytes */

    WORD actual_size;
    WORD reported_size;

    fd = zfile_fopen(name, MODE_READ);
    if (fd == NULL) {
        return NULL;
    }

    *read_only = 1;

    new = t64_new();
    new->fd = fd;

    if (t64_header_read(&new->header, fd) < 0) {
        t64_destroy(new);
        return NULL;
    }

    new->file_records = lib_malloc(sizeof(t64_file_record_t)
                                   * new->header.num_entries);

    /* FIXME: why does this read all entries and not just the entries
     * actually used?*/
    for (i = 0; i < new->header.num_entries; i++) {
        if (t64_file_record_read(new->file_records + i, fd) < 0) {
            t64_destroy(new);
            return NULL;
        }
        /* keep track of the original index in the container */
        new->file_records[i].index = i;
    }

    /* Attempt to fix bugged end addresses by sorting the records on their
     * content member and using that information (and the size of the container)
     * to determine the actual end address */

    /* get file size */
    if (fseek(fd, 0L, SEEK_END) != 0) {
        t64_destroy(new);
        return NULL;
    }
    tapesize = ftell(fd);
    if (tapesize < 0) {
        t64_destroy(new);
        return NULL;
    }

    /* sort records on content member */
    qsort(new->file_records, (size_t)(new->header.num_used),
            sizeof *(new->file_records), comp_content);

    /* set end addresses based on the content member of the next entry */
    for (i = 0; i < new->header.num_used - 1; i++) {
        actual_size = (WORD)(new->file_records[i + 1].contents - new->file_records[i].contents);
        reported_size = new->file_records[i].end_addr -
            new->file_records[i].start_addr;

        if (reported_size != actual_size) {
            log_warning(LOG_DEFAULT,
                    "invalid file size for record %d in t64 image: $%04x, "
                    "should be $%04x, fixing",
                    new->file_records[i].index, reported_size, actual_size);

            new->file_records[i].end_addr = new->file_records[i].start_addr +
                actual_size;
        }
    }
    /* use the file size of the container to set the correct end address for
     * the last file record */
    reported_size = new->file_records[i].end_addr -
        new->file_records[i].start_addr;
    actual_size = (WORD)(tapesize - new->file_records[i].contents);
    /* warn and fix if sizes mismatch */
    if (reported_size != actual_size) {
            log_warning(LOG_DEFAULT,
                    "invalid file size for record %d in t64 image: $%04x, "
                    "should be $%04x, fixing",
                    new->file_records[i].index, reported_size, actual_size);


        new->file_records[i].end_addr = new->file_records[i].start_addr +
            actual_size;
    }
    /* restore original order of records */
    qsort(new->file_records, (size_t)(new->header.num_used),
            sizeof *(new->file_records), comp_index);


    new->file_name = lib_stralloc(name);

    return new;
}

int t64_close(t64_t *t64)
{
    int retval;

    if (t64->fd != NULL) {
        retval = zfile_fclose(t64->fd);
        t64->fd = NULL;
    } else {
        retval = 0;
    }

    t64_destroy(t64);
    return retval;
}

/* ------------------------------------------------------------------------- */

int t64_seek_start(t64_t *t64)
{
    return t64_seek_to_file(t64, 0);
}

int t64_seek_to_file(t64_t *t64, int file_number)
{
    if (t64 == NULL || file_number < 0 || file_number >= t64->header.num_entries) {
        return -1;
    }

    t64->current_file_number = file_number;
    t64->current_file_seek_position = 0;

    return 0;
}

int t64_seek_to_next_file(t64_t *t64, unsigned int allow_rewind)
{
    int n;

    if (t64 == NULL) {
        return -1;
    }

    if (t64->current_file_number < 0) {
        n = -1;
    } else {
        n = t64->current_file_number;
    }

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
        if (rec->entry_type == T64_FILE_RECORD_NORMAL) {
            t64->current_file_number = n;
            t64->current_file_seek_position = 0;
            return t64->current_file_number;
        }
    }
}

t64_file_record_t *t64_get_file_record(t64_t *t64, unsigned int num)
{
    if (num >= t64->header.num_entries) {
        return NULL;
    }

    return t64->file_records + num;
}

t64_file_record_t *t64_get_current_file_record(t64_t *t64)
{
    if (t64->current_file_number < 0) {
        log_error(LOG_ERR, "T64: Negative file number.");
    }
    return t64_get_file_record(t64, (unsigned int)(t64->current_file_number));
}

int t64_read(t64_t *t64, BYTE *buf, size_t size)
{
    t64_file_record_t *rec;
    int recsize;
    long offset;
    int amount;

    if (t64 == NULL || t64->fd == NULL || t64->current_file_number < 0 || (int)size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    rec = t64->file_records + t64->current_file_number;

    recsize = t64_file_record_get_size(rec);

    offset = rec->contents + t64->current_file_seek_position;

    if (fseek(t64->fd, offset, SEEK_SET) != 0) {
        return -1;
    }

    /* limit size of the block that is read to not exceed the end of the file */
    if (recsize < (int)(t64->current_file_seek_position + size)) {
        if (t64->current_file_seek_position > recsize) {
            return -1;
        }
        size = recsize - t64->current_file_seek_position;
    }

    amount = (int)fread(buf, 1, size, t64->fd);

    t64->current_file_seek_position += amount;

    return amount;
}

void t64_get_header(t64_t *t64, BYTE *name)
{
    memcpy(name, t64->header.description, T64_REC_CBMNAME_LEN);
}
