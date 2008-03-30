/*
 * diskimage.c - Common low-level disk image access.
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

#include "createdisk.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "fsimage-gcr.h"
#include "fsimage.h"
#include "log.h"
#include "rawimage.h"
#include "realimage.h"
#include "types.h"
#include "utils.h"


static log_t disk_image_log = LOG_ERR;

/*-----------------------------------------------------------------------*/
/* Initialization.  */

void disk_image_init(void)
{
    disk_image_log = log_open("Disk Access");
    disk_image_createdisk_init();
    fsimage_init();
    realimage_init();
    rawimage_init();
}


/*-----------------------------------------------------------------------*/
/* Disk constants.  */

static unsigned int speed_map_1541[42] = {
                           3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0 };

static unsigned int speed_map_1571[70] = {
                           3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0,
                           3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0 };

unsigned int disk_image_speed_map_1541(unsigned int track)
{
    return speed_map_1541[track];
}

unsigned int disk_image_speed_map_1571(unsigned int track)
{
    return speed_map_1571[track];
}

/*-----------------------------------------------------------------------*/
/* Check for track out of bounds.  */

static char sector_map_d64[43] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, /*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19, /* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18, /* 21 - 30 */
    17, 17, 17, 17, 17,                     /* 31 - 35 */
    17, 17, 17, 17, 17, 17, 17              /* 36 - 42 */
};

static char sector_map_d67[36] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, /*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 20, 20, 20, /* 11 - 20 */
    20, 20, 20, 20, 18, 18, 18, 18, 18, 18, /* 21 - 30 */
    17, 17, 17, 17, 17                      /* 31 - 35 */
};

static char sector_map_d71[71] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, /*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19, /* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18, /* 21 - 30 */
    17, 17, 17, 17, 17,                     /* 31 - 35 */
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, /* 36 - 45 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19, /* 46 - 55 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18, /* 56 - 65 */
    17, 17, 17, 17, 17                      /* 66 - 70 */
};

static char sector_map_d80[78] =
{
    0,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, /*  1 - 10 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, /* 11 - 20 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, /* 21 - 30 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 27, /* 31 - 40 */
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, /* 41 - 50 */
    27, 27, 27, 25, 25, 25, 25, 25, 25, 25, /* 51 - 60 */
    25, 25, 25, 25, 23, 23, 23, 23, 23, 23, /* 61 - 70 */
    23, 23, 23, 23, 23, 23, 23          /* 71 - 77 */
};

unsigned int disk_image_sector_per_track(unsigned int format,
                                         unsigned int track)
{
    switch (format) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_X64:
        if (track >= sizeof(sector_map_d64)) {
            log_message(disk_image_log, "Track %i exceeds sector map.", track);
            return 0;
        }
        return sector_map_d64[track];
        break;
      case DISK_IMAGE_TYPE_D67:
        if (track >= sizeof(sector_map_d67)) {
            log_message(disk_image_log, "Track %i exceeds sector map.", track);
            return 0;
        }
        return sector_map_d67[track];
        break;
      case DISK_IMAGE_TYPE_D71:
        if (track >= sizeof(sector_map_d71)) {
            log_message(disk_image_log, "Track %i exceeds sector map.", track);
            return 0;
        }
        return sector_map_d71[track];
        break;
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        if (track >= sizeof(sector_map_d80)) {
            log_message(disk_image_log, "Track %i exceeds sector map.", track);
            return 0;
        }
        return sector_map_d80[track];
        break;
      default:
        log_message(disk_image_log,
                    "Unknown disk type %i.  Cannot calculate sectors per track",                    format);
    }
    return 0;
}

int disk_image_check_sector(unsigned int format, unsigned int track,
                            unsigned int sector)
{
    unsigned int sectors = 0, i;

    if (track < 1 || sector < 0)
        return -1;

    switch (format) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_X64:
        if (track > MAX_TRACKS_1541 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D67:
        if (track > MAX_TRACKS_2040 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D67, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D67, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D71:
        if (track > MAX_TRACKS_1571)
            return -1;
        if (track > NUM_TRACKS_1541) {          /* The second side */
            track -= NUM_TRACKS_1541;
            sectors = NUM_BLOCKS_1541;
        }
        if (sector >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D81:
        if (track > MAX_TRACKS_1581 || sector >=  NUM_SECTORS_1581)
            return -1;
        sectors = (track - 1) * NUM_SECTORS_1581 + sector;
        break;
      case DISK_IMAGE_TYPE_D80:
        if (track > MAX_TRACKS_8050 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D82:
        if (track > MAX_TRACKS_8250)
            return -1;
        if (track > NUM_TRACKS_8050) {          /* The second side */
            track -= NUM_TRACKS_8050;
            sectors = NUM_BLOCKS_8050;
        }
        if (sector >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, i);
        sectors += sector;
        break;
      default:
        return -1;
    }
    return (int)(sectors);
}

/*-----------------------------------------------------------------------*/

int disk_image_open(disk_image_t *image)
{
    int rc = 0;

    switch (image->device) {
      case DISK_IMAGE_DEVICE_FS:
        rc = fsimage_open(image);
        break;
      case DISK_IMAGE_DEVICE_REAL:
        rc = realimage_open(image);
        break;
      case DISK_IMAGE_DEVICE_RAW:
        rc = rawimage_open(image);
        break;
      default:
        log_error(disk_image_log, "Unknow image device %i.", image->device);
    }

    return rc;
}


int disk_image_close(disk_image_t *image)
{
    int rc = 0;

    switch (image->device) {
      case DISK_IMAGE_DEVICE_FS:
        rc = fsimage_close(image);
        break;
      case DISK_IMAGE_DEVICE_REAL:
        rc = realimage_close(image);
        break;
      case DISK_IMAGE_DEVICE_RAW:
        rc = rawimage_close(image);
        break;
      default:
        log_error(disk_image_log, "Unknow image device %i.", image->device);
    }

    return rc;
}

/*-----------------------------------------------------------------------*/

int disk_image_read_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                           unsigned int sector)
{
    int rc = 0;

    switch (image->device) {
      case DISK_IMAGE_DEVICE_FS:
        rc = fsimage_read_sector(image, buf, track, sector);
        break;
      case DISK_IMAGE_DEVICE_REAL:
        rc = realimage_read_sector(image, buf, track, sector);
        break;
      case DISK_IMAGE_DEVICE_RAW:
        rc = rawimage_read_sector(image, buf, track, sector);
        break;
      default:
        log_error(disk_image_log, "Unknow image device %i.", image->device); 
    }

    return rc;
}

int disk_image_write_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                            unsigned int sector)
{
    int rc = 0;

    switch (image->device) {
      case DISK_IMAGE_DEVICE_FS:
        rc = fsimage_write_sector(image, buf, track, sector);
        break;
      case DISK_IMAGE_DEVICE_REAL:
        rc = realimage_write_sector(image, buf, track, sector);
        break;
      case DISK_IMAGE_DEVICE_RAW:
        rc = rawimage_write_sector(image, buf, track, sector);
        break;
      default:
        log_error(disk_image_log, "Unknow image device %i.", image->device);
    }

    return rc;
}

/*-----------------------------------------------------------------------*/

int disk_image_read_track(disk_image_t *image, unsigned int track,
                          BYTE *gcr_data, int *gcr_track_size)
{
    return fsimage_gcr_read_track(image, track, gcr_data, gcr_track_size);
}

int disk_image_write_track(disk_image_t *image, unsigned int track,
                           int gcr_track_size, BYTE *gcr_speed_zone,
                           BYTE *gcr_track_start_ptr)
{
    return fsimage_gcr_write_track(image, track, gcr_track_size, gcr_speed_zone,
                                   gcr_track_start_ptr);
}

int disk_image_read_gcr_image(disk_image_t *image)
{
    return fsimage_read_gcr_image(image);
}

