/*
 * diskimage.c - Common low-level disk image access.
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

#include "archdep.h"
#include "createdisk.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "gcr.h"
#include "log.h"
#include "probe.h"
#include "types.h"
#include "utils.h"
#include "x64.h"
#include "zfile.h"


static log_t disk_image_log = LOG_ERR;

/*-----------------------------------------------------------------------*/
/* Initialization.  */

void disk_image_init(void)
{
    if (disk_image_log == LOG_ERR)
        disk_image_log = log_open("Disk Access");
    disk_image_createdisk_init();
    disk_image_probe_init();
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
/* Intial GCR buffer setup.  */

int disk_image_read_gcr_image(disk_image_t *image)
{
    unsigned int track, num_tracks;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];

    num_tracks = image->tracks;

    fseek(image->fd, 12, SEEK_SET);
    if (read_dword(image->fd, gcr_track_p, num_tracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image.");
        return -1;
    }

    fseek(image->fd, 12 + num_tracks * 8, SEEK_SET);
    if (read_dword(image->fd, gcr_speed_p, num_tracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image.");
        return -1;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {
        BYTE *track_data, *zone_data;

        track_data = image->gcr->data + track * NUM_MAX_BYTES_TRACK;
        zone_data = image->gcr->speed_zone + track * NUM_MAX_BYTES_TRACK;
        memset(track_data, 0xff, NUM_MAX_BYTES_TRACK);
        memset(zone_data, 0x00, NUM_MAX_BYTES_TRACK / 4);
        image->gcr->track_size[track] = 6250;

        if (track <= num_tracks && gcr_track_p[track * 2] != 0) {
            BYTE len[2];
            long offset;
            size_t track_len;
            unsigned int zone_len;

            offset = gcr_track_p[track * 2];

            fseek(image->fd, offset, SEEK_SET);
            if (fread((char *)len, 2, 1, image->fd) < 1) {
                log_error(disk_image_log, "Could not read GCR disk image.");
                return -1;
            }

            track_len = len[0] + len[1] * 256;

            if (track_len < 5000 || track_len > 7928) {
                log_error(disk_image_log,
                          "Track field length %i is not supported.",
                          (int)track_len);
                return -1;
            }

            image->gcr->track_size[track] = track_len;

            fseek(image->fd, offset + 2, SEEK_SET);
            if (fread((char *)track_data, track_len, 1, image->fd) < 1) {
                log_error(disk_image_log, "Could not read GCR disk image.");
                return -1;
            }

            zone_len = (track_len + 3) / 4;

            if (gcr_speed_p[track * 2] > 3) {
                unsigned int i;
                BYTE comp_speed[NUM_MAX_BYTES_TRACK / 4];

                offset = gcr_speed_p[track * 2];

                fseek(image->fd, offset, SEEK_SET);
                if (fread((char *)comp_speed, zone_len, 1, image->fd) < 1) {
                    log_error(disk_image_log,
                              "Could not read GCR disk image.");
                    return -1;
                }
                for (i = 0; i < zone_len; i++) {
                    zone_data[i * 4 + 3] = comp_speed[i] & 3;
                    zone_data[i * 4 + 2] = (comp_speed[i] >> 2) & 3;
                    zone_data[i * 4 + 1] = (comp_speed[i] >> 4) & 3;
                    zone_data[i * 4 ] = (comp_speed[i] >> 6) & 3;
                }
            } else {
                memset(zone_data, gcr_speed_p[track * 2], NUM_MAX_BYTES_TRACK);
            }
        }
    }
    return 0;
}


/*-----------------------------------------------------------------------*/
/* Open a disk image.  */

int disk_image_open(disk_image_t *image)
{
    if (image->read_only) {
        image->fd = zfopen(image->name, MODE_READ);
    } else  {
        image->fd = zfopen(image->name, MODE_READ_WRITE);

        /* If we cannot open the image read/write, try to open it read only. */
        if (image->fd == NULL) {
            image->fd = zfopen(image->name, MODE_READ);
            image->read_only = 1;
        }
    }

    image->error_info = NULL;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Cannot open file `%s'.", image->name);
        return -1;
    }

    if (disk_image_probe(image) == 0)
        return 0;

    zfclose(image->fd);
    log_message(disk_image_log, "Unknown disk image `%s'.", image->name);
    return -1;
}


/*-----------------------------------------------------------------------*/
/* Close a disk image.  */

int disk_image_close(disk_image_t *image)
{
    if (image->fd == NULL)
        return -1;

    zfclose(image->fd);

    free(image->name);
    image->name = NULL;
    if (image->error_info != NULL) {
        free(image->error_info);
        image->error_info = NULL;
    }
    return 0;
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
/* Read an entire GCR track from the disk image.  */

int disk_image_read_track(disk_image_t *image, unsigned int track,
                          BYTE *gcr_data, int *gcr_track_size)
{
    int track_len;
    BYTE len[2];
    DWORD gcr_track_p;
    long offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to write without disk image.");
        return -1;
    }

    fseek(image->fd, 12 + (track - 1) * 8, SEEK_SET);
    if (read_dword(image->fd, &gcr_track_p, 4) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image.");
        return -1;
    }

    memset(gcr_data, 0xff, NUM_MAX_BYTES_TRACK);
    *gcr_track_size = 6250;

    if (gcr_track_p != 0) {

        offset = gcr_track_p;

        fseek(image->fd, offset, SEEK_SET);
        if (fread((char *)len, 2, 1, image->fd) < 1) {
            log_error(disk_image_log, "Could not read GCR disk image.");
            return -1;
        }

        track_len = len[0] + len[1] * 256;

        if (track_len < 5000 || track_len > 7928) {
            log_error(disk_image_log,
                      "Track field length %i is not supported.",
                      track_len);
            return -1;
        }

        *gcr_track_size = track_len;

        fseek(image->fd, offset + 2, SEEK_SET);
        if (fread((char *)gcr_data, track_len, 1, image->fd) < 1) {
            log_error(disk_image_log, "Could not read GCR disk image.");
            return -1;
        }
    }
    return 0;
}


/*-----------------------------------------------------------------------*/
/* Read an sector from the disk image.  */

int disk_image_read_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                           unsigned int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to read without disk image.");
        return 74;
    }

    switch (image->type) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_D67:
      case DISK_IMAGE_TYPE_D71:
      case DISK_IMAGE_TYPE_D81:
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
      case DISK_IMAGE_TYPE_X64:
        sectors = disk_image_check_sector(image->type, track, sector);

        if (sectors < 0) {
            log_error(disk_image_log, "Track %i, Sector %i out of bounds.",
                      track, sector);
            return 66;
        }

        offset = sectors << 8;

        if (image->type == DISK_IMAGE_TYPE_X64)
            offset += X64_HEADER_LENGTH;

        fseek(image->fd, offset, SEEK_SET);

        if (fread((char *)buf, 256, 1, image->fd) < 1) {
            log_error(disk_image_log,
                      "Error reading T:%i S:%i from disk image.",
                      track, sector);
            return -1;
        }

        if (image->error_info != NULL) {
            switch (image->error_info[sectors]) {
              case 0x0:
              case 0x1:
                return 0;
              case 0x2:
                return 20;
              case 0x3:
                return 21;
              case 0x4:
                return 22;
              case 0x5:
                return 23;
              case 0x7:
                return 25;
              case 0x8:
                return 26;
              case 0x9:
                return 27;
              case 0xA:
                return 28;
              case 0xB:
                return 29;
              case 0xF:
                return 74;
              case 0x10:
                return 24;
              default:
                return 0;
            }
        }
        break;
      case DISK_IMAGE_TYPE_G64:
        {
            BYTE gcr_data[NUM_MAX_BYTES_TRACK], *gcr_track_start_ptr;
            int gcr_track_size, gcr_current_track_size;

            if (track > image->tracks) {
                log_error(disk_image_log,
                          "Track %i out of bounds.  Cannot read GCR track.",
                          track);
                return -1;
            }
            if (image->gcr == NULL) {
                if (disk_image_read_track(image, track, gcr_data,
                    &gcr_track_size) < 0) {
                    log_error(disk_image_log,
                              "Cannot read track %i from GCR image.", track);
                    return -1;
                }
                gcr_track_start_ptr = gcr_data;
                gcr_current_track_size = gcr_track_size;
            } else {
                gcr_track_start_ptr = image->gcr->data
                                      + ((track - 1) * NUM_MAX_BYTES_TRACK);
                gcr_current_track_size = image->gcr->track_size[track - 1];
            }
            if (gcr_read_sector(gcr_track_start_ptr, gcr_current_track_size,
                buf, track, sector) < 0) {
                log_error(disk_image_log,
                          "Cannot find track: %i sector: %i within GCR image.",
                          track, sector);
                return -1;
            }
        }
        break;
      default:
        log_error(disk_image_log, 
                  "Unknown disk image type %i.  Cannot read sector.",
                  image->type);
        return -1;
    }
    return 0;
}


/*-----------------------------------------------------------------------*/
/* Write an entire GCR track to the disk image.  */

int disk_image_write_track(disk_image_t *image, unsigned int track,
                           int gcr_track_size, BYTE *gcr_speed_zone,
                           BYTE *gcr_track_start_ptr)
{
    int gap, i;
    unsigned int num_tracks;
    BYTE len[2];
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    int offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to write without disk image.");
        return -1;
    }

    if (image->read_only != 0) {
        log_error(disk_image_log, "Attempt to write to read-only disk image.");
        return -1;
    }

    num_tracks = image->tracks;

    fseek(image->fd, 12, SEEK_SET);
    if (read_dword(image->fd, gcr_track_p, num_tracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image header.");
        return -1;
    }

    fseek(image->fd, 12 + num_tracks * 8, SEEK_SET);
    if (read_dword(image->fd, gcr_speed_p, num_tracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image header.");
        return -1;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
        offset = fseek(image->fd, 0, SEEK_END);
        if (offset < 0) {
            log_error(disk_image_log, "Could not extend GCR disk image.");
            return -1;
        }
        gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = gcr_track_size % 256;
    len[1] = gcr_track_size / 256;

    if (fseek(image->fd, offset, SEEK_SET) < 0
        || fwrite((char *)len, 2, 1, image->fd) < 1) {
        log_error(disk_image_log, "Could not write GCR disk image.");
        return -1;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - gcr_track_size;
    if (gap > 0)
        memset(gcr_track_start_ptr + gcr_track_size, 0, gap);

    if (fseek(image->fd, offset + 2, SEEK_SET) < 0
        || fwrite((char *)gcr_track_start_ptr, NUM_MAX_BYTES_TRACK,
        1, image->fd) < 1) {
        log_error(disk_image_log, "Could not write GCR disk image.");
        return -1;
    }

    if (gcr_speed_zone != NULL) {
        for (i = 0; (gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
            == gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
            && i < NUM_MAX_BYTES_TRACK; i++);

        if (i < gcr_track_size) {
            /* This will change soon.  */
            log_error(disk_image_log,
                      "Saving different speed zones is not supported yet.");
            return -1;
        }

        if (gcr_speed_p[(track - 1) * 2] >= 4) {
            /* This will change soon.  */
            log_error(disk_image_log,
                      "Adding new speed zones is not supported yet.");
            return -1;
        }

        offset = 12 + num_tracks * 8 + (track - 1) * 8;
        if (fseek(image->fd, offset, SEEK_SET) < 0
            || write_dword(image->fd, &gcr_speed_p[(track - 1) * 2], 4) < 0) {
            log_error(disk_image_log, "Could not write GCR disk image.");
            return -1;
        }
    }

#if 0  /* We do not support writing different speeds yet.  */
    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
        zone_len = (gcr_track_size + 3) / 4;
    zone_data = gcr_speed_zone + (track - 1) * NUM_MAX_BYTES_TRACK;

    if (gap > 0)
        memset(zone_data + gcr_track_size, 0, gap);

    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
        comp_speed[i] = (zone_data[i * 4]
                         | (zone_data[i * 4 + 1] << 2)
                         | (zone_data[i * 4 + 2] << 4)
                         | (zone_data[i * 4 + 3] << 6));

    if (fseek(image->fd, offset, SEEK_SET) < 0
        || fwrite((char *)comp_speed, NUM_MAX_BYTES_TRACK / 4, 1
        image->fd) < 1) {
        log_error(disk_image_log, "Could not write GCR disk image");
        return;
    }
#endif
    return 0;
}


/*-----------------------------------------------------------------------*/
/* Write a sector to the disk image.  */

int disk_image_write_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                            unsigned int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to write without disk image.");
        return -1;
    }

    if (image->read_only != 0) {
        log_error(disk_image_log, "Attempt to write to read-only disk image.");
        return -1;
    }

    sectors = disk_image_check_sector(image->type, track, sector);

    switch (image->type) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_D67:
      case DISK_IMAGE_TYPE_D71:
      case DISK_IMAGE_TYPE_D81:
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
      case DISK_IMAGE_TYPE_X64:
        if (sectors < 0) {
            log_error(disk_image_log, "Track: %i, Sector: %i out of bounds.",
                      track, sector);
            return -1;
        }
        offset = sectors << 8;

        if (image->type == DISK_IMAGE_TYPE_X64)
            offset += X64_HEADER_LENGTH;

        fseek(image->fd, offset, SEEK_SET);

        if (fwrite((char *)buf, 256, 1, image->fd) < 1) {
            log_error(disk_image_log, "Error writing T:%i S:%i to disk image.",
                      track, sector);
            return -1;
        }
        break;
      case DISK_IMAGE_TYPE_G64:
        {
            BYTE gcr_data[NUM_MAX_BYTES_TRACK];
            BYTE *gcr_track_start_ptr, *speed_zone;
            int gcr_track_size, gcr_current_track_size;

            if (track > image->tracks) {
                log_error(disk_image_log,
                          "Track %i out of bounds.  Cannot write GCR sector",
                          track);
                return -1;
            }
            if (image->gcr == NULL) {
                if (disk_image_read_track(image, track, gcr_data,
                    &gcr_track_size) < 0) {
                    log_error(disk_image_log,
                              "Cannot read track %i from GCR image.", track);
                    return -1;
                }
                gcr_track_start_ptr = gcr_data;
                gcr_current_track_size = gcr_track_size;
                speed_zone = NULL;
            } else {
                gcr_track_start_ptr = image->gcr->data
                                      + ((track - 1) * NUM_MAX_BYTES_TRACK);
                gcr_current_track_size = image->gcr->track_size[track - 1];
                speed_zone = image->gcr->speed_zone;
            }
            if (gcr_write_sector(gcr_track_start_ptr,
                gcr_current_track_size, buf, track, sector) < 0) {
                log_error(disk_image_log,
                          "Could not find track %i sector %i in disk image",
                          track, sector);
                return -1;
            }
            if (disk_image_write_track(image, track, gcr_current_track_size,
                speed_zone, gcr_track_start_ptr) < 0) {
                log_error(disk_image_log,
                          "Failed writing track %i to disk image.", track);
                return -1;
            }
        }
        break;
      default:
        log_error(disk_image_log, "Unknown disk image.  Cannot write sector.");
        return -1;
    }
    return 0;
}

