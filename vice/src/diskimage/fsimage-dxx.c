/*
 * fsimage-dxx.c
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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
#include <string.h>

#include "diskconstants.h"
#include "diskimage.h"
#include "cbmdos.h"
#include "fsimage-dxx.h"
#include "fsimage.h"
#include "gcr.h"
#include "log.h"
#include "lib.h"
#include "types.h"
#include "util.h"
#include "x64.h"

static log_t fsimage_dxx_log = LOG_ERR;
static const int raw_track_size[4] = { 6250, 6666, 7142, 7692 };
static const unsigned int gaps_between_sectors[4] = { 9, 12, 17, 8 };

int fsimage_dxx_write_half_track(disk_image_t *image, unsigned int half_track,
                                 int gcr_track_size, BYTE *gcr_track_start_ptr)
{
    unsigned int track, sector, max_sector = 0;
    BYTE buffer[256];
    disk_track_t raw = {gcr_track_start_ptr, gcr_track_size};

    track = half_track / 2;

    max_sector = disk_image_sector_per_track(image->type, track);

    for (sector = 0; sector < max_sector; sector++) {
        if (gcr_read_sector(&raw, buffer, sector) != CBMDOS_FDC_ERR_OK) {
            log_error(fsimage_dxx_log,
                    "Could not find data sector of T:%d S:%d.",
                    track, sector);
        } else {
            fsimage_dxx_write_sector(image, buffer, track, sector);
        }
    }
    return 0;
}

int fsimage_read_dxx_image(disk_image_t *image)
{
    BYTE buffer[256];
    int gap;
    unsigned int track, sector;
    gcr_header_t header;
    int rc;

    rc = fsimage_dxx_read_sector(image, buffer, 18, 0);
    if (rc < 0) {
        return -1;
    }
    header.id1 = buffer[0xa2];
    header.id2 = buffer[0xa3];

    for (track = 1; track <= image->tracks; track++) {
        BYTE *ptr;
        unsigned int max_sector = 0;

        if (image->gcr->track_data[(track * 2) - 2] == NULL) {
            image->gcr->track_data[(track * 2) - 2] = lib_malloc(NUM_MAX_MEM_BYTES_TRACK);
        }
        ptr = image->gcr->track_data[(track * 2) - 2];
        if (image->type == DISK_IMAGE_TYPE_D71) {
            image->gcr->track_size[(track * 2) - 2] =
                raw_track_size[disk_image_speed_map_1571(track - 1)];
            gap = gaps_between_sectors[disk_image_speed_map_1571(track - 1)];
        } else {
            image->gcr->track_size[(track * 2) - 2] =
                raw_track_size[disk_image_speed_map_1541(track - 1)];
            gap = gaps_between_sectors[disk_image_speed_map_1541(track - 1)];
        }

        max_sector = disk_image_sector_per_track(image->type, track);

        /* Clear track to avoid read errors.  */
        memset(ptr, 0x55, NUM_MAX_BYTES_TRACK);

        header.track = track;
        for (sector = 0; sector < max_sector; sector++) {

            rc = fsimage_dxx_read_sector(image, buffer, track, sector);

            header.sector = sector;
            gcr_convert_sector_to_GCR(buffer, ptr, &header, 9, 5, (BYTE)(rc));

            ptr += SECTOR_GCR_SIZE_WITH_HEADER + 9 + gap + 5;
        }

        /* Clear odd track */
        if (image->gcr->track_data[(track * 2) - 1]) {
            lib_free(image->gcr->track_data[(track * 2) - 1]);
            image->gcr->track_data[(track * 2) - 1] = NULL;
        }
        image->gcr->track_size[(track * 2) - 1] = image->gcr->track_size[(track * 2) - 2];
    }
    return 0;
}

int fsimage_dxx_read_sector(disk_image_t *image, BYTE *buf,
                               unsigned int track, unsigned int sector)
{
    int sectors;
    long offset;
    fsimage_t *fsimage;

    fsimage = image->media.fsimage;

    sectors = disk_image_check_sector(image, track, sector);

    if (sectors < 0) {
        log_error(fsimage_dxx_log, "Track %i, Sector %i out of bounds.",
                track, sector);
        return CBMDOS_IPE_ILLEGAL_TRACK_OR_SECTOR;
    }

    offset = sectors << 8;

    if (image->type == DISK_IMAGE_TYPE_X64)
        offset += X64_HEADER_LENGTH;

    if (util_fpread(fsimage->fd, buf, 256, offset) < 0) {
        log_error(fsimage_dxx_log,
                "Error reading T:%i S:%i from disk image.",
                track, sector);
        return -1;
    }

    if (fsimage->error_info != NULL) {
        switch (fsimage->error_info[sectors]) {
        case 0x0:
        case 0x1:
            return CBMDOS_IPE_OK;               /* 0 */
        case 0x2:
            return CBMDOS_IPE_READ_ERROR_BNF;   /* 20 */
        case 0x3:
            return CBMDOS_IPE_READ_ERROR_SYNC;  /* 21 */
        case 0x4:
            return CBMDOS_IPE_READ_ERROR_DATA;  /* 22 */
        case 0x5:
            return CBMDOS_IPE_READ_ERROR_CHK;   /* 23 */ 
        case 0x7:
            return CBMDOS_IPE_WRITE_ERROR_VER;  /* 25 */
        case 0x8:
            return CBMDOS_IPE_WRITE_PROTECT_ON; /* 26 */
        case 0x9:
            return CBMDOS_IPE_READ_ERROR_BCHK;  /* 27 */
        case 0xA:
            return CBMDOS_IPE_WRITE_ERROR_BIG;  /* 28 */
        case 0xB:
            return CBMDOS_IPE_DISK_ID_MISMATCH; /* 29 */
        case 0xF:
            return CBMDOS_IPE_NOT_READY;        /* 74 */
        case 0x10:
            return CBMDOS_IPE_READ_ERROR_GCR;   /* 24 */
        default:
            return 0;
        }
    }
    return 0;
}

int fsimage_dxx_write_sector(disk_image_t *image, BYTE *buf,
                                unsigned int track, unsigned int sector)
{
    int sectors;
    long offset;
    fsimage_t *fsimage;

    fsimage = image->media.fsimage;

    sectors = disk_image_check_sector(image, track, sector);

    if (sectors < 0) {
        log_error(fsimage_dxx_log, "Track: %i, Sector: %i out of bounds.",
                track, sector);
        return -1;
    }
    offset = sectors << 8;

    if (image->type == DISK_IMAGE_TYPE_X64)
        offset += X64_HEADER_LENGTH;

    if (util_fpwrite(fsimage->fd, buf, 256, offset) < 0) {
        log_error(fsimage_dxx_log, "Error writing T:%i S:%i to disk image.",
                track, sector);
        return -1;
    }

    /* Make sure the stream is visible to other readers.  */
    fflush(fsimage->fd);
    return 0;
}

void fsimage_dxx_init(void)
{
    fsimage_dxx_log = log_open("Filesystem Image DXX");
}
