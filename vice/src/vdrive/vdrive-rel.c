/*
 * vdrive-rel.c - Virtual disk-drive implementation.
 *                Relative files specific functions.
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

#include "diskimage.h"
#include "log.h"
#include "serial.h"
#include "utils.h"
#include "vdrive-dir.h"
#include "vdrive.h"

#define SIDE_SECTORS_MAX 6
#define SIDE_INDEX_MAX   120

#define OFFSET_NEXT_TRACK  0
#define OFFSET_NEXT_SECTOR 1
#define OFFSET_SECTOR_NUM  2
#define OFFSET_RECORD_LEN  3
#define OFFSET_SIDE_SECTOR 4
#define OFFSET_POINTER     16



static log_t vdrive_rel_log = LOG_ERR;

void vdrive_rel_init(void)
{
    vdrive_rel_log = log_open("VDriveREL");
}

static int vdrive_rel_open_existing(vdrive_t *vdrive, unsigned int secondary)
{
    unsigned int track, sector, i;

    track = vdrive->buffers[secondary].slot[SLOT_SIDE_TRACK];
    sector = vdrive->buffers[secondary].slot[SLOT_SIDE_SECTOR];

    for (i = 0; i < SIDE_SECTORS_MAX; i++) {
        if (disk_image_read_sector(vdrive->image,
            &vdrive->side_sector[256 * i], track, sector) != 0) {
            log_debug("Cannot read side sector.");
            return -1;
        }
        if (vdrive->side_sector[256 * i + OFFSET_SECTOR_NUM] != i) {
            log_debug("Side sector number do not match.");
            return -1;
        }

        track = vdrive->side_sector[256 * i + OFFSET_NEXT_TRACK];
        sector = vdrive->side_sector[256 * i + OFFSET_NEXT_SECTOR];

        if (track == 0)
            break;
    }

    return 0;
}

int vdrive_rel_open(vdrive_t *vdrive, unsigned int secondary,
                    unsigned int record_length)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    log_debug("VDrive REL open with record length %i.", record_length);

    if (vdrive->side_sector != NULL)    
        free(vdrive->side_sector);

    vdrive->side_sector = (BYTE *)xmalloc(SIDE_SECTORS_MAX * 256);
    memset(vdrive->side_sector, 0, SIDE_SECTORS_MAX * 256);

    if (p->slot) {
        log_debug("Open existing REL file.");
        vdrive_rel_open_existing(vdrive, secondary);
    } else {
        log_debug("Open new REL file.");
    }

    p->mode = BUFFER_RELATIVE;
    p->bufptr = 0;
    p->buffer = (BYTE *)xmalloc(256);
    p->record = 0;

    return SERIAL_OK;
}

static unsigned int vdrive_rel_side_num(unsigned int rec_lo,
                                        unsigned int rec_hi,
                                        unsigned int rec_len)
{
    unsigned int record;

    record = (rec_lo + (rec_hi << 8)) * rec_len;
    return record / (256 * SIDE_INDEX_MAX);
}

static unsigned int vdrive_rel_pos_num(unsigned int rec_lo,
                                       unsigned int rec_hi,
                                       unsigned int rec_len)
{
    unsigned int record, side, offset;

    record = (rec_lo + (rec_hi << 8)) * rec_len;
    side = record / (256 * SIDE_INDEX_MAX);

    offset = (record / 256) - side * SIDE_INDEX_MAX;

    return offset * 2 + OFFSET_POINTER;
}

static unsigned int vdrive_rel_record_max(vdrive_t *vdrive)
{
    unsigned int i, j;

    for (i = 0; i < SIDE_SECTORS_MAX; i++) {
        if (vdrive->side_sector[256 * i + OFFSET_NEXT_TRACK] == 0)
            break;
    }

    for (j = 0; j < SIDE_INDEX_MAX; j++) {
        if (vdrive->side_sector[256 * i + OFFSET_POINTER + 2 * j] == 0)
            break;
    }

    return i * SIDE_INDEX_MAX + j;
}

static BYTE *vdrive_rel_read_buffer(vdrive_t *vdrive, unsigned int track,
                                    unsigned int sector)
{
    BYTE secdata[256];
    BYTE *recbuf;

    if (disk_image_read_sector(vdrive->image, secdata, track, sector) != 0)
        return NULL;

    recbuf = (BYTE *)xmalloc(2 * 254);

    memcpy(recbuf, &secdata[2], 254);

    if (secdata[0] != 0) {
        if (disk_image_read_sector(vdrive->image, secdata, secdata[0],
            secdata[1]) != 0) {
            free(recbuf);
            return NULL;
        }
        memcpy(&recbuf[254], &secdata[2], 254);
    }

    return recbuf;
}


int vdrive_rel_position(vdrive_t *vdrive, unsigned int channel,
                        unsigned int rec_lo, unsigned int rec_hi,
                        unsigned int position)
{
    unsigned int side_num, pos_num, record, record_max, rec_start, rec_len;
    unsigned int copy_start, copy_len;
    unsigned int track, sector;
    bufferinfo_t *p = &(vdrive->buffers[channel]);
    BYTE *recdata;

    rec_len = vdrive->side_sector[OFFSET_RECORD_LEN];

log_debug("rec_len %i", rec_len);

    if (position >= rec_len) {
        log_debug("Position larger than record!?");
        return 66;
    }

    record = rec_lo + (rec_hi << 8);
    record_max = vdrive_rel_record_max(vdrive);

log_debug("record %i", record);
log_debug("record_max %i", record_max);

    if (record > record_max) {
        log_error(vdrive_rel_log, "Enlarging REL files is not supported.");
        return 70;
    }

    rec_start = (record * rec_len) % 254;

log_debug("rec_start %i", rec_start);

    side_num = vdrive_rel_side_num(rec_lo, rec_hi, rec_len);
    pos_num = vdrive_rel_pos_num(rec_lo, rec_hi, rec_len);

log_debug("side_num %i", side_num);
log_debug("pos_num %i", pos_num);

    track = vdrive->side_sector[side_num * 256 + pos_num];
    sector = vdrive->side_sector[side_num * 256 + pos_num + 1]; 

log_debug("track %i", track);
log_debug("sector %i", sector);

    recdata = vdrive_rel_read_buffer(vdrive, track, sector);

    if (recdata == NULL) {
        log_debug("Cannot read track %i sector %i.", track, sector);
        return 66;
    }

    copy_start = rec_start + position;
    copy_len = rec_len - position;

log_debug("copy_start %i", copy_start);
log_debug("copy_len %i", copy_len);

    memcpy(p->buffer, &recdata[copy_start], copy_len);
    free(recdata);

    p->bufptr = 0;

    return 0;
}

