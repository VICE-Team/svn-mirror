/*
 * fsimage-create.c - Create a disk image.
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

#include "archdep.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "fsimage-create.h"
#include "fsimage.h"
#include "gcr.h"
#include "log.h"
#include "types.h"
#include "utils.h"
#include "x64.h"

/* Remove this!!! */
#include "tap.h"


static log_t createdisk_log = LOG_ERR;


static int disk_image_create_gcr(disk_image_t *image)
{
    BYTE gcr_header[12], gcr_track[7930], *gcrptr;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    unsigned int track, sector;
    fsimage_t *fsimage;

    fsimage = (fsimage_t *)(image->media);

    strcpy((char *)gcr_header, "GCR-1541");

    gcr_header[8] = 0;
    gcr_header[9] = MAX_TRACKS_1541 * 2;
    gcr_header[10] = 7928 % 256;
    gcr_header[11] = 7928 / 256;

    if (fwrite((char *)gcr_header, sizeof(gcr_header), 1, fsimage->fd) < 1) {
        log_error(createdisk_log, "Cannot write GCR header.");
        return -1;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {
        gcr_track_p[track * 2] = 12 + MAX_TRACKS_1541 * 16 + track * 7930;
        gcr_track_p[track * 2 + 1] = 0;
        gcr_speed_p[track * 2] = disk_image_speed_map_1541(track);
        gcr_speed_p[track * 2 + 1] = 0;
    }

    if (util_dword_write(fsimage->fd, gcr_track_p, sizeof(gcr_track_p)) < 0) {
        log_error(createdisk_log, "Cannot write track header.");
        return -1;
    }
    if (util_dword_write(fsimage->fd, gcr_speed_p, sizeof(gcr_speed_p)) < 0) {
        log_error(createdisk_log, "Cannot write speed header.");
        return -1;
    }
    for (track = 0; track < MAX_TRACKS_1541; track++) {
        int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

        memset(&gcr_track[2], 0xff, 7928);
        gcr_track[0] = raw_track_size[disk_image_speed_map_1541(track)] % 256;
        gcr_track[1] = raw_track_size[disk_image_speed_map_1541(track)] / 256;
        gcrptr = &gcr_track[2];

        for (sector = 0;
        sector < disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track + 1);
        sector++) {
            BYTE chksum, id[2], rawdata[260];
            int i;

            id[0] = id[1] = 0xa0;
            memset(rawdata, 0, 260);
            rawdata[0] = 7;
            chksum = rawdata[1];
            for (i = 1; i < 256; i++)
                chksum ^= rawdata[i + 1];
            rawdata[257] = chksum;

            gcr_convert_sector_to_GCR(rawdata, gcrptr, track + 1, sector,
                                      id[0], id[1], 0);
            gcrptr += 360;
        }
        if (fwrite((char *)gcr_track, sizeof(gcr_track), 1, fsimage->fd) < 1 ) {
            log_error(createdisk_log, "Cannot write track data.");
            return -1;
        }
    }
    return 0;
}


/*-----------------------------------------------------------------------*/
/* Create a disk image.  */

int fsimage_create(const char *name, unsigned int type)
{
    disk_image_t *image;
    unsigned int size, i;
    BYTE block[256];
    fsimage_t *fsimage;

    size = 0;

    switch(type) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_X64:
        size = D64_FILE_SIZE_35;
        break;
      case DISK_IMAGE_TYPE_D67:
        size = D67_FILE_SIZE;
        break;
      case DISK_IMAGE_TYPE_D71:
        size = D71_FILE_SIZE;
        break;
      case DISK_IMAGE_TYPE_D81:
        size = D81_FILE_SIZE;
        break;
      case DISK_IMAGE_TYPE_D80:
        size = D80_FILE_SIZE;
        break;
      case DISK_IMAGE_TYPE_D82:
        size = D82_FILE_SIZE;
        break;
      case DISK_IMAGE_TYPE_G64:
        break;
      case DISK_IMAGE_TYPE_TAP:
        break;
      default:
        log_error(createdisk_log,
                  "Wrong image type.  Cannot create disk image.");
        return -1;
    }

    image = (disk_image_t *)xmalloc(sizeof(disk_image_t));
    fsimage = (fsimage_t *)xmalloc(sizeof(fsimage_t));


    image->media = fsimage;
    image->device = DISK_IMAGE_DEVICE_FS;

    fsimage->name = stralloc(name);
    fsimage->fd = fopen(name, MODE_WRITE);

    if (fsimage->fd == NULL) {
        log_error(createdisk_log, "Cannot create disk image `%s'.",
                  fsimage->name);
        free(fsimage->name);
        free(fsimage);
        free(image);
        return -1;
    }

    memset(block, 0, 256);

    switch (type) {
      case DISK_IMAGE_TYPE_X64:
        {
            BYTE header[X64_HEADER_LENGTH];

            memset(header, 0, X64_HEADER_LENGTH);

            header[X64_HEADER_MAGIC_OFFSET + 0] = X64_HEADER_MAGIC_1;
            header[X64_HEADER_MAGIC_OFFSET + 1] = X64_HEADER_MAGIC_2;
            header[X64_HEADER_MAGIC_OFFSET + 2] = X64_HEADER_MAGIC_3;
            header[X64_HEADER_MAGIC_OFFSET + 3] = X64_HEADER_MAGIC_4;
            header[X64_HEADER_VERSION_OFFSET + 0] = X64_HEADER_VERSION_MAJOR;
            header[X64_HEADER_VERSION_OFFSET + 1] = X64_HEADER_VERSION_MINOR;
            header[X64_HEADER_FLAGS_OFFSET + 0] = 1;
            header[X64_HEADER_FLAGS_OFFSET + 1] = NUM_TRACKS_1541;
            header[X64_HEADER_FLAGS_OFFSET + 2] = 1;
            header[X64_HEADER_FLAGS_OFFSET + 3] = 0;
            if (fwrite(header, X64_HEADER_LENGTH, 1, fsimage->fd) < 1) {
                log_error(createdisk_log,
                          "Cannot write X64 header to disk image `%s'.",
                          fsimage->name);
            }

        }
        /* Fall through.  */
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_D67:
      case DISK_IMAGE_TYPE_D71:
      case DISK_IMAGE_TYPE_D81:
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        for (i = 0; i < (size / 256); i++) {
            if (fwrite(block, 256, 1, fsimage->fd) < 1) {
                log_error(createdisk_log,
                          "Cannot seek to end of disk image `%s'.",
                          fsimage->name);
                fclose(fsimage->fd);
                free(fsimage->name);
                free(fsimage);
                free(image);
                return -1;
            }
        }
        break;
      case DISK_IMAGE_TYPE_G64:
        if (disk_image_create_gcr(image) < 0) {
            fclose(fsimage->fd);
            free(fsimage->name);
            free(fsimage);
            free(image);
            return -1;
        }
        break;
      case DISK_IMAGE_TYPE_TAP:
        /* create an empty tap */
        strcpy((char *)&block[TAP_HDR_MAGIC_OFFSET], "C64-TAPE-RAW");
        block[TAP_HDR_VERSION] = 1;
        util_dword_to_le_buf(&block[TAP_HDR_LEN], 4);
        if (fwrite(block, 24, 1, fsimage->fd) < 1) {
            fclose(fsimage->fd);
            free(fsimage->name);
            free(fsimage);
            free(image);
            return -1;
        }

    }

    fclose(fsimage->fd);
    free(fsimage->name);
    free(fsimage);
    free(image);
    return 0;
}

void fsimage_create_init(void)
{
    createdisk_log = log_open("Disk Create");
}

