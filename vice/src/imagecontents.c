/*
 * imagecontents.c - Extract the directory from disk/tape images.
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

#include <string.h>
#include <errno.h>

#include "imagecontents.h"

#include "charsets.h"
#include "log.h"
#include "t64.h"
#include "utils.h"
#include "vdrive.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */

image_contents_t *image_contents_new(void)
{
    image_contents_t *new;

    new = xmalloc(sizeof(image_contents_t));

    memset(new->name, 0, sizeof(new->name));
    memset(new->id, 0, sizeof(new->id));
    new->blocks_free = -1;
    new->file_list = NULL;

    return new;
}

void image_contents_destroy(image_contents_t *contents)
{
    image_contents_file_list_t *p;

    for (p = contents->file_list; p != NULL; p = p->next)
        free(p);

    free(contents);
}

char *image_contents_to_string(image_contents_t *contents)
{
    static char filler[IMAGE_CONTENTS_FILE_NAME_LEN] = "                ";
    image_contents_file_list_t *p;
    char line_buf[256];
    char *buf;
    int buf_size, max_buf_size;
    int len;

#define BUFCAT(s, n) bufcat(buf, &buf_size, &max_buf_size, (s), (n))

    max_buf_size = 4096;
    buf = xmalloc(max_buf_size);
    buf_size = 0;

    BUFCAT("0 \"", 3);
    BUFCAT((char *)contents->name, strlen((char *)contents->name));
    BUFCAT("\" ", 2);
    BUFCAT((char *)contents->id, strlen((char *)contents->id));

    if (contents->file_list == NULL) {
        const char *s = "\n(eMPTY IMAGE.)";

        BUFCAT(s, strlen(s));
    }

    for (p = contents->file_list; p != NULL; p = p->next) {
        int name_len;

        len = sprintf(line_buf, "\n%-5d \"%s\" ", p->size, p->name);
        BUFCAT(line_buf, len);

        name_len = strlen((char *)p->name);
        if (name_len < IMAGE_CONTENTS_FILE_NAME_LEN)
            BUFCAT(filler, IMAGE_CONTENTS_FILE_NAME_LEN 
                           - strlen((char *)p->name));
        BUFCAT((char *)p->type, strlen((char *)p->type));
    }

    if (contents->blocks_free >= 0) {
        len = sprintf(line_buf, "\n%d blocks free.", contents->blocks_free);
        BUFCAT(line_buf, len);
    }

    BUFCAT("\n", 2);              /* With a closing zero.  */

    petconvstring(buf, 1);

    return buf;
}

/* ------------------------------------------------------------------------- */

/* This code is used to check whether the directory is circular.  It should
   be replaced by a more simple check that just stops if the number of
   entries is bigger than expected, but this needs some support in `vdrive.c'
   which we do not have yet.  */

static struct {
    unsigned int track;
    unsigned int sector;
} *block_list = NULL;

unsigned int block_list_nelems;
unsigned int block_list_size;

static void circular_check_init(void)
{
    block_list_nelems = 0;
}

static int circular_check(unsigned int track, unsigned int sector)
{
    int i;

    for (i = 0; i < block_list_nelems; i++)
        if (block_list[i].track == track && block_list[i].sector == sector)
            return 1;

    if (block_list_nelems == block_list_size) {
        if (block_list_size == 0) {
            block_list_size = 512;
            block_list = xmalloc(sizeof(*block_list) * block_list_size);
        } else {
            block_list_size *= 2;
            block_list = xrealloc(block_list,
                                  sizeof(*block_list) * block_list_size);
        }
    }

    block_list[block_list_nelems].track = track;
    block_list[block_list_nelems++].sector = sector;

    return 0;
}

/* ------------------------------------------------------------------------ */

/* Disk contents.  */
/* FIXME: When we will have a module for disk image handling in the style of
   `t64.c', this will be moved into it.  */

/* Argh!  Really ugly!  FIXME!  */
extern char *slot_type[];

static DRIVE *open_image(const char *name)
{
    static BYTE fake_command_buffer[256];
    DRIVE *floppy;
    hdrinfo hdr;
    file_desc_t fd;
    int image_format;

    fd = zopen(name, O_RDONLY, 0);
    if (fd == ILLEGAL_FILE_DESC)
	return NULL;
    if (check_header(fd, &hdr))
	return NULL;

    if (hdr.v_major > HEADER_VERSION_MAJOR
	|| (hdr.v_major == HEADER_VERSION_MAJOR
	    && hdr.v_minor > HEADER_VERSION_MINOR)) {
	zclose(fd);
	return 0;
    }

    image_format = get_diskformat(hdr.devtype);
    if (image_format < 0) {
	zclose(fd);
	return NULL;
    }

    floppy = xmalloc(sizeof(DRIVE));

    floppy->ActiveFd = fd;
    floppy->ImageFormat = image_format;
    floppy->NumTracks = hdr.tracks;
    floppy->NumBlocks = num_blocks(floppy->ImageFormat, hdr.tracks);
    floppy->ErrFlg    = hdr.errblk;
    floppy->D64_Header= hdr.d64 | hdr.d71 | hdr.d81;
    floppy->ReadOnly = 1;	/* Just to be sure... */

    /* This fake is necessary to `open_1541'...  Ugly, but that is the only
       way I know with the existing functions.  */
    floppy->buffers[15].mode = BUFFER_COMMAND_CHANNEL;
    floppy->buffers[15].buffer = fake_command_buffer;
    floppy->buffers[0].mode = BUFFER_NOT_IN_USE;

    /* Initialize format constants.  */
    set_disk_geometry(floppy, floppy->ImageFormat);

    return floppy;
}

image_contents_t *image_contents_read_disk(const char *file_name)
{
    image_contents_t *new;
    DRIVE *floppy;
    BYTE buffer[256];
    int retval;
    image_contents_file_list_t *lp;

    floppy = open_image(file_name);
    if (floppy == NULL)
        return NULL;

    retval = vdrive_bam_read_bam(floppy);

    if (retval < 0) {
        zclose(floppy->ActiveFd);
        free(floppy);
        return NULL;
    }

    new = image_contents_new();

    memcpy(new->name, floppy->bam + floppy->bam_name, IMAGE_CONTENTS_NAME_LEN);
    new->name[IMAGE_CONTENTS_NAME_LEN] = 0;

    memcpy(new->id, floppy->bam + floppy->bam_id, IMAGE_CONTENTS_ID_LEN);
    new->id[IMAGE_CONTENTS_ID_LEN] = 0;

    new->blocks_free = floppy_free_block_count(floppy);

    floppy->Curr_track = floppy->Dir_Track;
    floppy->Curr_sector = floppy->Dir_Sector;

    lp = NULL;
    new->file_list = NULL;

    circular_check_init();

    while (1) {
        BYTE *p;
        int j;

        retval = floppy_read_block(floppy->ActiveFd,
                                   floppy->ImageFormat,
                                   buffer,
                                   floppy->Curr_track,
                                   floppy->Curr_sector,
                                   floppy->D64_Header);

        if (retval < 0
            || circular_check(floppy->Curr_track, floppy->Curr_sector)) {
            image_contents_destroy(new);
            zclose(floppy->ActiveFd);
            free(floppy);
            return NULL;
        }

        for (p = buffer, j = 0; j < 8; j++, p += 32)
            if (p[SLOT_TYPE_OFFSET] != 0) {
                image_contents_file_list_t *new_list;
                int i;

                new_list = xmalloc(sizeof(image_contents_file_list_t));
                new_list->size = ((int) p[SLOT_NR_BLOCKS]
                                  + ((int) p[SLOT_NR_BLOCKS + 1] << 8));

                for (i = 0; i < IMAGE_CONTENTS_FILE_NAME_LEN; i++) {
                    if (p[SLOT_NAME_OFFSET + i] == 0xa0)
                        break;
                    else
                        new_list->name[i] = p[SLOT_NAME_OFFSET + i];
                }
                new_list->name[i] = 0;
                
                sprintf ((char *)new_list->type, "%c%s%c",
                         (p[SLOT_TYPE_OFFSET] & FT_CLOSED ? ' ' : '*'),
                         slot_type[p[SLOT_TYPE_OFFSET] & 0x07],
                         (p[SLOT_TYPE_OFFSET] & FT_LOCKED ? '<' : ' '));

                new_list->next = NULL;

                if (lp == NULL) {
                    new_list->prev = NULL;
                    new->file_list = new_list;
                    lp = new->file_list;
                } else {
                    new_list->prev = lp;
                    lp->next = new_list;
                    lp = new_list;
                }
            }

        if (buffer[0] == 0)
            break;

        floppy->Curr_track = (int) buffer[0];
        floppy->Curr_sector = (int) buffer[1];
    }

    zclose(floppy->ActiveFd);
    free(floppy);
    return new;
}

/* ------------------------------------------------------------------------- */

/* Tape contents.  */
/* FIXME: When we will have a module for disk image handling in the style of
   `t64.c', this will be moved to `t64.c'.  */

image_contents_t *image_contents_read_tape(const char *file_name)
{
    t64_t *t64;
    image_contents_t *new;
    image_contents_file_list_t *lp;

    t64 = t64_open(file_name);
    if (t64 == NULL)
        return NULL;

    new = image_contents_new();

    memcpy(new->name, t64->header.description, T64_REC_CBMNAME_LEN);
    *new->id = 0;
    new->blocks_free = -1;

    lp = NULL;
    new->file_list = NULL;

    while (t64_seek_to_next_file(t64, 0) >= 0) {
        t64_file_record_t *rec;

        rec = t64_get_current_file_record(t64);
        if (rec->entry_type != T64_FILE_RECORD_FREE) {
            image_contents_file_list_t *new_list;

            new_list = xmalloc(sizeof(image_contents_file_list_t));
            memcpy(new_list->name, rec->cbm_name, T64_REC_CBMNAME_LEN);
            new_list->name[IMAGE_CONTENTS_FILE_NAME_LEN] = 0;

            /* XXX: Not quite true, but this is what the tape emulation
               will do anyway.  */
            strcpy((char *)new_list->type, " PRG ");

            new_list->size = (rec->end_addr - rec->start_addr) / 254;
            new_list->next = NULL;

            if (lp == NULL) {
                new_list->prev = NULL;
                new->file_list = new_list;
                lp = new->file_list;
            } else {
                new_list->prev = lp;
                lp->next = new_list;
                lp = new_list;
            }
        }
    }

    t64_close(t64);
    return new;
}
