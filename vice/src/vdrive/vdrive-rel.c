/*
 * vdrive-rel.c - Virtual disk-drive implementation.
 *                Relative files specific functions.
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
#include <string.h>

#include "diskimage.h"
#include "lib.h"
#include "log.h"
#include "vdrive-dir.h"
#include "vdrive-rel.h"
#include "vdrive.h"

#define SIDE_SECTORS_MAX 6
#define SIDE_INDEX_MAX   120

#define OFFSET_NEXT_TRACK  0
#define OFFSET_NEXT_SECTOR 1
#define OFFSET_SECTOR_NUM  2
#define OFFSET_RECORD_LEN  3
#define OFFSET_SIDE_SECTOR 4
#define OFFSET_POINTER     16

#define OFFSET_SUPER_254     2
#define OFFSET_SUPER_POINTER 3
#define SIDE_SUPER_MAX       126

static log_t vdrive_rel_log = LOG_ERR;

static unsigned int vdrive_rel_record_max(vdrive_t *vdrive, unsigned int secondary);

void vdrive_rel_init(void)
{
    vdrive_rel_log = log_open("VDriveREL");
}

static int vdrive_rel_open_existing(vdrive_t *vdrive, unsigned int secondary)
{
    unsigned int track, sector, side, i, j, o;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    /* read super side sector, if it exists */

    track = p->slot[SLOT_SIDE_TRACK];
    sector = p->slot[SLOT_SIDE_SECTOR];

    p->super_side_sector = (BYTE *)lib_malloc(256);

    if (disk_image_read_sector(vdrive->image,
        p->super_side_sector, track, sector) != 0) {
        log_debug("Cannot read side sector.");
        lib_free((char *)p->super_side_sector);
        return -1;
    }

    /* check to see if this is a super side sector.  If not, create an
        imaginary one so we don't have to change all our code. */
    if (p->super_side_sector[OFFSET_SUPER_254] != 254) {
        /* clear out block */
        memset(p->super_side_sector, 0, 256);
        /* build valid super side sector block */
        p->super_side_sector[OFFSET_NEXT_TRACK] = track;
        p->super_side_sector[OFFSET_NEXT_SECTOR] = sector;
        p->super_side_sector[OFFSET_SUPER_254] = 254;
        p->super_side_sector[OFFSET_SUPER_POINTER] = track;
        p->super_side_sector[OFFSET_SUPER_POINTER+1] = sector;
        /* set track and sector to 0, i.e. no update */
        p->super_side_sector_track = 0;
        p->super_side_sector_sector = 0;
    }
    else {
        /* set track and sector */
        p->super_side_sector_track = track;
        p->super_side_sector_sector = sector;
    }

    /* find the number of side sector groups */
    for (side = 0; p->super_side_sector[OFFSET_SUPER_POINTER + side * 2] != 0 ; side++ );

    /* allocate memory for side sectors */
    p->side_sector = (BYTE *)lib_malloc(side * SIDE_SECTORS_MAX * 256);
    memset(p->side_sector, 0, side * SIDE_SECTORS_MAX * 256);

    /* Also clear out track and sectors locations of each side sector */
    p->side_sector_track = (BYTE *)lib_malloc(side * SIDE_SECTORS_MAX);
    p->side_sector_sector = (BYTE *)lib_malloc(side * SIDE_SECTORS_MAX);
    memset(p->side_sector_track, 0, side * SIDE_SECTORS_MAX);
    memset(p->side_sector_sector, 0, side * SIDE_SECTORS_MAX);

    for (j = 0; j < side ; j++ ) {
        track = p->super_side_sector[OFFSET_SUPER_POINTER + j * 2];
        sector = p->super_side_sector[OFFSET_SUPER_POINTER + j * 2 + 1];

        for (i = 0; i < SIDE_SECTORS_MAX; i++) {
            o = i + j * SIDE_SECTORS_MAX;
            /* Save the track and sector of each side sector so we can also write
               and update REL files. */
            p->side_sector_track[o] = track;
            p->side_sector_sector[o] = sector;

            o*=256;

            if (disk_image_read_sector(vdrive->image,
                &(p->side_sector[o]), track, sector) != 0) {
                log_debug("Cannot read side sector.");
                return -1;
            }
            if (p->side_sector[o + OFFSET_SECTOR_NUM] != i) {
                log_debug("Side sector number do not match.");
                return -1;
            }

            track = p->side_sector[o + OFFSET_NEXT_TRACK];
            sector = p->side_sector[o + OFFSET_NEXT_SECTOR];

            if (track == 0)
                break;
        }
    }

    /* Determine maximum record */
    p->record_max = vdrive_rel_record_max(vdrive, secondary);

    return 0;
}

int vdrive_rel_open(vdrive_t *vdrive, unsigned int secondary,
                    unsigned int record_length)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    log_debug("VDrive REL open with record length %i.", record_length);

#if 0
    if (p->side_sector != NULL)    
        lib_free(p->side_sector);
#endif

    if (p->slot) {
        log_debug("Open existing REL file.");
        vdrive_rel_open_existing(vdrive, secondary);
    } else {
        log_debug("Open new REL file.");
        /* Can't open a REL file for write yet */
        return SERIAL_ERROR;
    }

    p->mode = BUFFER_RELATIVE;
    p->bufptr = 0;
    p->buffer = (BYTE *)lib_malloc(256);
    p->record = 0;
    p->track = 0;
    p->sector = 0;
    p->buffer_next = (BYTE *)lib_malloc(256);
    p->track_next = 0;
    p->sector_next = 0;

    /* Move to first record, no offset */
    vdrive_rel_position(vdrive, secondary, 1, 0, 1);

    return SERIAL_OK;
}

void vdrive_rel_track_sector(vdrive_t *vdrive, unsigned int secondary,
                             unsigned int record, unsigned int *track,
                             unsigned int *sector, unsigned int *rec_start)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    unsigned int rec_long, side, offset, rec_len, super;

    /* find the record length from the first side sector */
    rec_len = p->side_sector[OFFSET_RECORD_LEN];

    /* compute the total byte offset */
    rec_long = (record * rec_len);

    /* find the sector offset */
    *rec_start = rec_long % 254;

    /* find the super side sector (0 - 125) */
    offset = (254 * SIDE_INDEX_MAX * SIDE_SECTORS_MAX);
    super = rec_long / offset;
    rec_long = rec_long % offset;

    /* find the side sector (0-5) */
    offset = (254 * SIDE_INDEX_MAX);
    side = rec_long / offset;
    rec_long = rec_long % offset;

    /* find the offset into the side sector (0-120) */
    offset = ( rec_long / 254 ) * 2;

    /* add super, side sector and index offsets */
    offset += ( super * SIDE_SECTORS_MAX + side ) * 256 + OFFSET_POINTER;

    *track = p->side_sector[ offset ];
    *sector = p->side_sector[ offset + 1]; 

    return;
}

static unsigned int vdrive_rel_record_max(vdrive_t *vdrive, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    unsigned int i, j, k, l, side;
    unsigned int track, sector;
    BYTE *temp;

    /* The original 1541 documentation states that there can only be 720
        (6*120) records in a REL file - this is incorrect.  It is actually
        blocks.  The side sectors point to each block in the REL file.  It is
        up to the drive to determine the where the buffer pointer should be
        when the record changes. */

    /* To find the maximum records we have to check the last block of the REL
        file.  The last block is referenced in the last side sector group by
        the OFFSET_NEXT_SECTOR byte.  The next sector pointer after this one
        should be zero - we will check anyways to make sure.  Once the last
        block is found, we can move to that sector and check the used bytes
        there. */

    /* The maximum REL file size for a 1541/1571 is 700 blocks, although it
        is documented to be 720.  The 1581 is limited to 3000 blocks,
        although it is documented to be 90720 (126*6*120).  You think they
        would have allowed it to be atleast the maximum amount of disk
        space. */

    /* find the number of side sector groups */
    for (side = 0; p->super_side_sector[OFFSET_SUPER_POINTER + side * 2] != 0 ; side++ );
    side--;

    /* Find last side sector */
    for (i = 0; i < SIDE_SECTORS_MAX; i++) {
        if (p->side_sector[256 * ( i + side * SIDE_SECTORS_MAX) + OFFSET_NEXT_TRACK] == 0)
            break;
    }

    /* Scan last side sector for a null track and sector pointer */
    for (j = 0; j < SIDE_INDEX_MAX; j++) {
        if (p->side_sector[256 * ( i + side * SIDE_SECTORS_MAX) + OFFSET_POINTER + 2 * j] == 0)
            break;
    }

    /* obtain the last byte of the sector according to the index */
    k = p->side_sector[256 * ( i + side * SIDE_SECTORS_MAX) + OFFSET_NEXT_SECTOR ];

    /* generate a last byte index based on the actual sectors used */
    l = OFFSET_POINTER + 2 * j - 1;

    /* compare them */
    if( k!=l ) {
        /* something is wrong with this rel file, it should be repaired */
        log_debug("Relative file ending sector and side sectors don't match up.");
    }

    /* Get the track and sector of the last block */
    j--;
    track = p->side_sector[256 * ( i + side * SIDE_SECTORS_MAX) + OFFSET_POINTER + 2 * j];
    sector = p->side_sector[256 * ( i + side * SIDE_SECTORS_MAX) + OFFSET_POINTER + 2 * j + 1];

    /* read it in */
    temp = (BYTE *)lib_malloc(256);

    if (disk_image_read_sector(vdrive->image,
        temp, track, sector) != 0) {
        log_debug("Cannot relative file data sector.");
        lib_free(temp);
        return -1;
    }

    /* calculate the total bytes based on the number of super side, side
        sectors, and last byte index */
    k = ( side * SIDE_SUPER_MAX * SIDE_INDEX_MAX + i * SIDE_INDEX_MAX + j )
        * 254 + ( temp[1] - 2 + 1 ) + 1;

    /* divide by the record length, and get the maximum records */
    l = k / p->side_sector[OFFSET_RECORD_LEN];

    /* clean up */
    lib_free(temp);

    return l;
}

int vdrive_rel_position(vdrive_t *vdrive, unsigned int channel,
                        unsigned int rec_lo, unsigned int rec_hi,
                        unsigned int position)
{
    unsigned int record, rec_start, rec_len;
    unsigned int track, sector;
    bufferinfo_t *p = &(vdrive->buffers[channel]);

    /* find the record length from the first side sector */
    rec_len = p->side_sector[OFFSET_RECORD_LEN];

    /* position 0 and 1 are the same - the first */
    position = (position == 0) ? 0 : position - 1;

    /* generate error 51 if we point to a position greater than the record
       length */
    if (position >= rec_len) {
        log_debug("Position larger than record!?");
        return 51;
    }

    /* generate a 16 bit value for the record from the two 8-bit values */
    record = rec_lo + (rec_hi << 8);
    /* record 0 and 1 are the same - the first */
    record = (record == 0) ? 0 : record - 1;

    p->record = record;
    /* if the record is greater then the maximum, return error 50, but
       remember the record for growth */
    if (record >= p->record_max) {
        log_error(vdrive_rel_log, "Enlarging REL files is not supported.");
        return 50;
    }

    /* locate the track, sector and sector offset of record */
    vdrive_rel_track_sector(vdrive, channel, record, &track, &sector, &rec_start);

    /* FIXME: check for writes here to commit the buffers */

    /* Check to see if the next record is in the buffered sector */
    if (p->track_next == track && p->sector_next == sector) {
        /* Swap the two buffers */
        BYTE *tmp;
        tmp = p->buffer;
        p->buffer = p->buffer_next;
        p->buffer_next = tmp;
        p->track_next = p->track;
        p->sector_next = p->sector;
        p->track = track;
        p->sector = sector;
    } else if (p->track != track || p->sector != sector ) {
        /* load in the sector to memory */
        if (disk_image_read_sector(vdrive->image, p->buffer, track, sector) != 0) {
            log_debug("Cannot read track %i sector %i.", track, sector);
            return 66;
        }
        p->track=track;
        p->sector=sector;
        /* keep buffered sector where ever it is */
        /* we won't read in the next sector unless we have to */
    }

    /* set the buffer pointer */
    p->bufptr = rec_start + 2 + position;
    /* compute the next pointer record */
    p->record_next = p->bufptr - position + rec_len;

    /* set the maximum record length */
    p->length = p->record_next - 1;

    /* It appears that all Commodore drives have the same problems handling
        REL files when it comes to using the position option of the record
        command.  Normally when a record is selected, the drive firmware
        scan from the end of the record to the beginning for when a non-NULL
        byte is found.  When it finds it, this is considered the end of a
        record (for a read).  The problem is: what happens when you position
        the read pointer past this end point?  An overflow occurs in the
        read of course!  Since the calculated record length is much larger
        than it should be (but less than 256) the drive can read up to a
        whole block of data    from the subsequent records.  Although this is
        a bug (from my point of view), we have to try to emulate it.  Apon
        analyzing the 1541 code (the 1571 and 1581 have the same behavior),
        at $e170 (the subroutine for searching for the non-NULL is at $e1b5)
        the    overflow may not occur under one specific instance.  This is
        when the start of the record is in one block and the selected
        position places it in another block.  In this case, only a length of
        1 byte is set as the record length.  In all other cases, the length
        is set to 255 - position. */

    /* Find the length of the record starting from the end until no zeros
       are found */
    if ( p->length < 256) {
        /* If the maximum length of the record is in this sector, just
           check for where the zeros end */
        for (;p->length >= p->bufptr && p->buffer[p->length] == 0 ; p->length-- );

        /* Compensate for overflow length bug, set maximum length based on
            the position requested */
        if (p->bufptr > p->length && position > 0) {
            p->length = p->bufptr + 254 - position;
        }
    }
    else {
        int status=1;

        /* Get the next sector */
        /* If it doesn't exist, we will use the max length calculated above */
        if (p->buffer[0] != 0) {
            /* Read in the sector if it has not been buffered */
            if (p->buffer[0] != p->track_next || p->buffer[1] != p->sector_next)
            {
                status = disk_image_read_sector(vdrive->image, p->buffer_next, p->buffer[0],
                    p->buffer[1]);
            }
            else {
                status = 0;
            }
        }
        /* If all is good, calculate the length now */
        if (!status) {
            /* update references, if the sector read in */
            p->track_next = p->buffer[0];
            p->sector_next = p->buffer[1];
            /* Start looking in the buffered sector */
            for (;p->length >= 256 && p->length >= p->bufptr && p->buffer_next[p->length-256+2] == 0 ; p->length-- );

            /* If we crossed into the current sector, look there too, but
                only if the position places us in the current sector.
                Otherwise, the length will be 1 at this point. */
            if (p->length < 256 && p->bufptr < 256) {
                for (;p->length >= p->bufptr && p->buffer[p->length] == 0 ; p->length-- );

                /* Compensate for overflow length bug, set maximum length
                    based on the position requested */
                if (p->bufptr > p->length && position > 0 ) {
                    p->length = p->bufptr + 254 - position;
                }
            }
        }
    }

    return 0;

}
