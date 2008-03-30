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

#include "imagecontents.h"

#include "tape.h"
#include "utils.h"
#include "vdrive.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */

void image_contents_free(image_contents_t *contents)
{
    image_contents_file_list_t *p;

    for (p = contents->file_list; p != NULL; p = p->next)
        free(p);

    free(contents);
}

/* ------------------------------------------------------------------------- */

/* This code is used to check whether the directory is circular.  It should
   be replaced by a more simple check that just stops if the number of
   entries is bigger than expected, but this needs some support in `vdrive.c'
   which we currently do not have (yet).  */

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

/* Argh!  Really ugly!  FIXME!  */
extern char *slot_type[];

/* FIXME: This is duplicated code from `read_disk_image_contents()' in
   `vdrive.c', which should be removed some day.  */
static DRIVE *open_image(const char *name)
{
    static BYTE fake_command_buffer[256];
    DRIVE *floppy;
    char *buf;
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
	fprintf(logfile, "Disk image file %s (V %d.%02d) version higher than emulator (V %d.%02d)\n",
	       name, hdr.v_major, hdr.v_minor,
	       HEADER_VERSION_MAJOR, HEADER_VERSION_MINOR);

	zclose(fd);
	return 0;
    }

    image_format = get_diskformat(hdr.devtype);
    if (image_format < 0) {
	fprintf(errfile, "Error: unknown image format.\n");
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

/* FIXME: some day this should completely replace
   `read_disk_image_contents()' in `vdrive.c'.  */
/* FIXME: loop detection.  */
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

    retval = floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                               floppy->bam, BAM_TRACK_1541, BAM_SECTOR_1541,
                               floppy->D64_Header);
    if (retval < 0) {
        zclose(floppy->ActiveFd);
        free(floppy);
        return NULL;
    }

    new = xmalloc(sizeof(image_contents_t));

    memcpy(new->name, floppy->bam + BAM_NAME_1541, IMAGE_CONTENTS_NAME_LEN);
    new->name[IMAGE_CONTENTS_NAME_LEN] = 0;

    memcpy(new->id, floppy->bam + BAM_ID_1541, IMAGE_CONTENTS_ID_LEN);
    new->id[IMAGE_CONTENTS_ID_LEN] = 0;

    new->blocks_free = floppy_free_block_count(floppy);

    floppy->Curr_track = floppy->Dir_Track;
    floppy->Curr_sector = floppy->Dir_Sector;

    lp = NULL;
    new->file_list = NULL;

    circular_check_init();

    while (1) {
        BYTE *p;
        int i;

        retval = floppy_read_block(floppy->ActiveFd,
                                   floppy->ImageFormat,
                                   buffer,
                                   floppy->Curr_track,
                                   floppy->Curr_sector,
                                   floppy->D64_Header);

        if (retval < 0
            || circular_check(floppy->Curr_track, floppy->Curr_sector)) {
            image_contents_free(new);
            zclose(floppy->ActiveFd);
            free(floppy);
            return NULL;
        }

        for (p = buffer, i = 0; i < 8; i++, p += 32)
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
                
                sprintf (new_list->type, "%c%s%c",
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

/* FIXME: this should completely replace `read_tape_image_contents()' from
   `tape.c'.  */
image_contents_t *image_contents_read_tape(const char *file_name)
{
    BYTE inbuf[TAPE_HDR_SIZE + 1];
    FILE *fd;
    image_contents_t *new;
    image_contents_file_list_t *lp;
    int i;
    int num_entries, max_entries;

    fd = zfopen(file_name, "r");
    if (fd == NULL)
        return NULL;

    /* Check whether this really looks like a tape image. */
    if (fread(inbuf, TAPE_HDR_SIZE, 1, fd) != 1 || !check_t64_header(fd)) {
	zfclose(fd);
	return NULL;
    }

    max_entries = (unsigned int)inbuf[34] + (unsigned int)inbuf[35] * 256;
    num_entries = (unsigned int)inbuf[36] + (unsigned int)inbuf[37] * 256;

    /* Check a little bit...  */
    if (num_entries > max_entries) {
	fprintf(errfile, "Tape inconsistency, giving up!\n");
	zfclose(fd);
	return NULL;
    }

    /* Many T64 images are broken...  */
    if (!num_entries)
	num_entries = 1;

    /* Seek to start of file records.  */
    if (fseek (fd, 64, SEEK_SET) < 0) {
	perror("lseek to file records failed");
	zfclose(fd);
	return NULL;
    }

    new = xmalloc(sizeof(image_contents_t));

    memcpy(new->name, inbuf + 40, 24);
    new->name[24] = 0;
    *new->id = 0;
    new->blocks_free = -1;

    lp = NULL;
    new->file_list = NULL;

    for (i = 0; i < num_entries; i++) {
        image_contents_file_list_t *new_list;
        int start_addr, end_addr;
        int blocks;
        int res;

        res = fread(inbuf, 32, 1, fd);
        if (res != 1) {
            zfclose(fd);
            image_contents_free(new);
            return NULL;
        }

	if (inbuf[0] == 0)
	    /* Free slot.  */
	    continue;

        start_addr = (unsigned int)inbuf[2] + (unsigned int)inbuf[3] * 256;
        end_addr = (unsigned int)inbuf[4] + (unsigned int)inbuf[5] * 256;

        if (end_addr > start_addr)
            blocks = (end_addr - start_addr) / 256 + 1;
        else
            blocks = 0;

        new_list = xmalloc(sizeof(image_contents_file_list_t));

        memcpy(new_list->name, inbuf + 16, 16);
        new_list->name[16] = 0;
        if (inbuf[0] == 1)
            strcpy(new_list->type, " PRG ");
        else
            strcpy(new_list->type, " ??? ");
        new_list->size = blocks;

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

    zfclose(fd);
    return new;
}
