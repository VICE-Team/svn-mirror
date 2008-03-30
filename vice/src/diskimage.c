/*
 * diskimage.c - Common low-level disk image access.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
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

#include "archdep.h"
#include "diskimage.h"
#include "gcr.h"
#include "log.h"
#include "utils.h"
#include "vdrive.h"
#include "zfile.h"


static log_t disk_image_log = LOG_ERR;

/* Defines for probing the disk image type.  */
#define IS_D64_LEN(x) ((x) == D64_FILE_SIZE_35 || (x) == D64_FILE_SIZE_35E || \
                       (x) == D64_FILE_SIZE_40 || (x) == D64_FILE_SIZE_40E)
#define IS_D71_LEN(x) ((x) == D71_FILE_SIZE)
#define IS_D81_LEN(x) ((x) == D81_FILE_SIZE)
#define IS_D80_LEN(x) ((x) == D80_FILE_SIZE)
#define IS_D82_LEN(x) ((x) == D82_FILE_SIZE)

/*-----------------------------------------------------------------------*/

void disk_image_init(void)
{
    if (disk_image_log == LOG_ERR)
        disk_image_log = log_open("Disk Access");
}

/*-----------------------------------------------------------------------*/

static int disk_image_check_for_d64(disk_image_t *image)
{
    int blk = 0, len;
    BYTE block[256];

    if (!(IS_D64_LEN(file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D64;
    image->tracks = NUM_TRACKS_1541;

    if (fseek(image->fd, 256 * blk, SEEK_SET) < 0)
        return 0;

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > 771) {
            log_error(disk_image_log, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1541) {
        log_error(disk_image_log, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 683:
        image->tracks = NUM_TRACKS_1541;
        break;
      case 685:
        if (len != 171) {
            log_message(disk_image_log, "Cannot read block %d", blk);
            return 0;
        }
        image->tracks = NUM_TRACKS_1541;
        break;
      case 768:
        image->tracks = EXT_TRACKS_1541;
        break;
      case 771:
        image->tracks = EXT_TRACKS_1541;
        break;
      default:
        return 0;
    }
    log_message(disk_image_log, "D64 disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");
    return 1;
}

static int disk_image_check_for_d71(disk_image_t *image)
{
    int blk = 0, len;
    BYTE block[256];

    if (!(IS_D71_LEN(file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D71;
    image->tracks = NUM_TRACKS_1571;

    if (fseek(image->fd, 256 * blk, SEEK_SET) < 0)
        return 0;

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > 1372) {
            log_error(disk_image_log, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1571) {
        log_error(disk_image_log, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 1366:
        image->tracks = NUM_TRACKS_1571;
        break;
      default:
        return 0;
    }
    log_message(disk_image_log, "D71 disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");
    return 1;
}

static int disk_image_check_for_d81(disk_image_t *image)
{
    int blk = 0, len;
    BYTE block[256];

    if (!(IS_D81_LEN(file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D81;
    image->tracks = NUM_TRACKS_1581;

    if (fseek(image->fd, 256 * blk, SEEK_SET) < 0)
        return 0;

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > 3213) {
            log_error(disk_image_log, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1581) {
        log_error(disk_image_log, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 3200:
        image->tracks = NUM_TRACKS_1581;
        break;
      default:
        return 0;
    }
    log_message(disk_image_log, "D81 disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");
    return 1;
}

static int disk_image_check_for_d80(disk_image_t *image)
{
    int blk = 0, len;
    BYTE block[256];

    if (!(IS_D80_LEN(file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D80;
    image->tracks = NUM_TRACKS_8050;

    if (fseek(image->fd, 256 * blk, SEEK_SET) < 0)
        return 0;

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > NUM_BLOCKS_8050 + 6) {
            log_error(disk_image_log, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_8050) {
        log_error(disk_image_log, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case NUM_BLOCKS_8050:
        image->tracks = NUM_TRACKS_8050;
        break;
      default:
        return 0;
    }
    log_message(disk_image_log, "D80 disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");
    return 1;
}

static int disk_image_check_for_d82(disk_image_t *image)
{
    int blk = 0, len;
    BYTE block[256];

    if (!(IS_D82_LEN(file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D82;
    image->tracks = NUM_TRACKS_8250;

    if (fseek(image->fd, 256 * blk, SEEK_SET) < 0)
        return 0;

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > NUM_BLOCKS_8250 + 6) {
            log_error(disk_image_log, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_8250) {
        log_error(disk_image_log, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case NUM_BLOCKS_8250:
        image->tracks = NUM_TRACKS_8250;
        break;
      default:
        return 0;
    }
    log_message(disk_image_log, "D82 disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");
    return 1;
}

/*-----------------------------------------------------------------------*/

int disk_image_read_gcr_image(disk_image_t *image)
{
    int track, track_len, zone_len, i, NumTracks;
    BYTE len[2], comp_speed[NUM_MAX_BYTES_TRACK / 4];
    BYTE *track_data, *zone_data;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    off_t offset;

    NumTracks = image->tracks;

    fseek(image->fd, 12, SEEK_SET);
    if (read_dword(image->fd, gcr_track_p, NumTracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image.");
        return -1;
    }

    fseek(image->fd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(image->fd, gcr_speed_p, NumTracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image.");
        return -1;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {

        track_data = image->gcr->data + track * NUM_MAX_BYTES_TRACK;
        zone_data = image->gcr->speed_zone + track * NUM_MAX_BYTES_TRACK;
        memset(track_data, 0xff, NUM_MAX_BYTES_TRACK);
        memset(zone_data, 0x00, NUM_MAX_BYTES_TRACK / 4);
        image->gcr->track_size[track] = 6250;

        if (track <= NumTracks && gcr_track_p[track * 2] != 0) {

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
                          track_len);
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

                offset = gcr_speed_p[track * 2];

                fseek(image->fd, offset, SEEK_SET);
                if (fread((char *)comp_speed, zone_len, 1, image->fd) < 1) {
                    log_error(disk_image_log,
                              "Could not read GCR disk image.");
                    return -1;
                }
                for (i = 0; i < zone_len; i++) {
                    zone_data[i * 4] = comp_speed[i] & 3;
                    zone_data[i * 4 + 1] = (comp_speed[i] >> 2) & 3;
                    zone_data[i * 4 + 2] = (comp_speed[i] >> 4) & 3;
                    zone_data[i * 4 + 3] = (comp_speed[i] >> 6) & 3;
                }
            } else {
                memset(zone_data, gcr_speed_p[track * 2], NUM_MAX_BYTES_TRACK);
            }
        }
    }
    return 0;
}

static int disk_image_check_for_gcr(disk_image_t *image)
{
    int trackfield;
    BYTE header[32];

    fseek(image->fd, 0, SEEK_SET);
    if (fread((BYTE *)header, sizeof (header), 1, image->fd) < 1) {
        log_error(disk_image_log, "Cannot read image header.");
        return 0;
    }

    if (strncmp("GCR-1541", (char*)header,8))
        return 0;

    if (header[8] != 0) {
        log_error(disk_image_log, "Import GCR: Wrong GCR image version.");
        return 0;
    }

    if (header[9] < NUM_TRACKS_1541 * 2 || header[9] > MAX_TRACKS_1541 * 2) {
        log_error(disk_image_log, "Import GCR: Invalid number of tracks.");
        return 0;
    }

    trackfield = header[10] + header[11] * 256;
    if (trackfield != 7928) {
        log_error(disk_image_log, "Import GCR: Invalid track field number.");
        return 0;
    }

    image->type = DISK_IMAGE_TYPE_GCR;
    image->tracks = header[9] / 2;
    log_message(disk_image_log, "GCR disk image recognised: %s%s.",
                image->name, image->read_only ? " (read only)" : "");

    if (image->gcr != NULL) {
        if (disk_image_read_gcr_image(image) < 0)
            return -1;
    }
    return 1;
}

int disk_image_open(disk_image_t *image)
{
    image->fd = zfopen(image->name, MODE_READ_WRITE);

    /* If we cannot open the image read/write, try to open it read only. */
    if (image->fd == NULL) {
        image->fd = zfopen(image->name, MODE_READ);
        image->read_only = 1;
    } else {
        image->read_only = 0;
    }

    if (image->fd == NULL) {
        log_error(disk_image_log, "Cannot open file `%s'.", image->name);
        return -1;
    } else {
        if (disk_image_check_for_d64(image))
            return 0;        
        if (disk_image_check_for_d71(image))
            return 0;
        if (disk_image_check_for_d81(image))
            return 0;
        if (disk_image_check_for_d80(image))
            return 0;
        if (disk_image_check_for_d82(image))
            return 0;
        if (disk_image_check_for_gcr(image))
            return 0;
    }
    return -1;
}

/*-----------------------------------------------------------------------*/

int disk_image_close(disk_image_t *image)
{
    if (image->fd == NULL)
        return -1;

    zfclose(image->fd);
    free(image->name);
    image->name = NULL;
    return 0;
}

/*-----------------------------------------------------------------------*/

static char sector_map_d64[43] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, /*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19, /* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18, /* 21 - 30 */
    17, 17, 17, 17, 17,                     /* 31 - 35 */
    17, 17, 17, 17, 17, 17, 17              /* 36 - 42 */
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

int disk_image_sector_per_track(int format, int track)
{
    switch (format) {
      case DISK_IMAGE_TYPE_D64:
        return sector_map_d64[track];
        break;
      case DISK_IMAGE_TYPE_D71:
        return sector_map_d71[track];
        break;
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        return sector_map_d80[track];
        break;
      default:
        log_message(disk_image_log,
                    "Unknown disk type %i.  Cannot calculate sectors per track",
                    format);
    }
    return -1;
}

/*-----------------------------------------------------------------------*/

int disk_image_check_sector(int format, int track, int sector)
{
    int sectors = 0, i;

    if (track < 1 || sector < 0)
        return -1;

    switch (format) {
      case DISK_IMAGE_TYPE_D64:
        if (track > MAX_TRACKS_1541 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, i);
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
        sectors = -1;
    }
    return sectors;
}

/*-----------------------------------------------------------------------*/

int disk_image_read_sector(disk_image_t *image, BYTE *buf, int track,
                           int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to write without disk image.");
        return -1;
    }

    switch (image->type) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_D71:
      case DISK_IMAGE_TYPE_D81:
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        sectors = disk_image_check_sector(image->type, track, sector);

        if (sectors < 0) {
            log_error(disk_image_log, "Track %i, Sector %i out of bounds.",
                      track, sector);
            return -1;
        }
        offset = sectors << 8;
        fseek(image->fd, offset, SEEK_SET);

        if (fread((char *)buf, 256, 1, image->fd) < 1) {
            log_error(disk_image_log,
                      "Error reading T:%i S:%i from disk image.",
                      track, sector);
            return -1;
        }
        break;
      case DISK_IMAGE_TYPE_GCR:
        if (track > image->tracks) {
            log_error(disk_image_log,
                      "Track %i out of bounds.  Cannot read GCR track.",
                      track);
            return -1;
        }
        if (image->gcr == NULL) {
            log_error(disk_image_log, "No GCR data available for reading.");
            return -1;
        }
        if (gcr_read_sector(image->gcr, buf, track, sector) < 0) {
            log_error(disk_image_log,
                      "Cannot read track: %i sector: %i from GCR image.",
                      track, sector);
            return -1;
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

void disk_image_write_track(disk_image_t *image, int track,
                            int *gcr_track_size, BYTE *gcr_speed_zone)
{
    int gap, i, NumTracks;
    BYTE len[2], *gcr_track_start_ptr;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    int offset;

    gcr_track_start_ptr = image->gcr->data 
                          + ((track - 1) * NUM_MAX_BYTES_TRACK);

    NumTracks = image->tracks;

    fseek(image->fd, 12, SEEK_SET);
    if (read_dword(image->fd, gcr_track_p, NumTracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image header.");
        return;
    }

    fseek(image->fd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(image->fd, gcr_speed_p, NumTracks * 8) < 0) {
        log_error(disk_image_log, "Could not read GCR disk image header.");
        return;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
        offset = fseek(image->fd, 0, SEEK_END);
        if (offset < 0) {
            log_error(disk_image_log, "Could not extend GCR disk image.");
            return;
        }
        gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = gcr_track_size[track - 1] % 256;
    len[1] = gcr_track_size[track - 1] / 256;

    if (fseek(image->fd, offset, SEEK_SET) < 0
        || fwrite((char *)len, 2, 1, image->fd) < 1) {
        log_error(disk_image_log, "Could not write GCR disk image.");
        return;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - gcr_track_size[track - 1];
    if (gap > 0)
        memset(gcr_track_start_ptr + gcr_track_size[track - 1], 0, gap);

    if (fseek(image->fd, offset + 2, SEEK_SET) < 0
        || fwrite((char *)gcr_track_start_ptr, NUM_MAX_BYTES_TRACK,
        1, image->fd) < 1) {
        log_error(disk_image_log, "Could not write GCR disk image.");
        return;
    }

    for (i = 0; (gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
         == gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
         && i < NUM_MAX_BYTES_TRACK; i++);

    if (i < gcr_track_size[track - 1]) {
        /* This will change soon.  */
        log_error(disk_image_log,
                  "Saving different speed zones is not supported yet.");
        return;
    }

    if (gcr_speed_p[(track - 1) * 2] >= 4) {
        /* This will change soon.  */
        log_error(disk_image_log,
                  "Adding new speed zones is not supported yet.");
        return;
    }

    offset = 12 + NumTracks * 8 + (track - 1) * 8;
    if (fseek(image->fd, offset, SEEK_SET) < 0
        || write_dword(image->fd, &gcr_speed_p[(track - 1) * 2], 4) < 0) {
        log_error(disk_image_log, "Could not write GCR disk image.");
        return;
    }

#if 0  /* We do not support writing different speeds yet.  */
    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
        zone_len = (gcr_track_size[track - 1] + 3) / 4;
    zone_data = gcr_speed_zone + (track - 1) * NUM_MAX_BYTES_TRACK;

    if (gap > 0)
        memset(zone_data + gcr_track_size[track - 1], 0, gap);

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
}

/*-----------------------------------------------------------------------*/

int disk_image_write_sector(disk_image_t *image, BYTE *buf, int track,
                            int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(disk_image_log, "Attempt to write without disk image.");
        return -1;
    }

    sectors = disk_image_check_sector(image->type, track, sector);

    switch (image->type) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_D71:
      case DISK_IMAGE_TYPE_D81:
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        if (sectors < 0) {
            log_error(disk_image_log, "Track: %i, Sector: %i out of bounds.",
                      track, sector);
            return -1;
        }
        offset = sectors << 8;
        fseek(image->fd, offset, SEEK_SET);

        if (fwrite((char *)buf, 256, 1, image->fd) < 1) {
            log_error(disk_image_log, "Error writing T:%i S:%i to disk image.",
                      track, sector);
            return -1;
        }
        break;
      case DISK_IMAGE_TYPE_GCR:
        if (track > image->tracks) {
            log_error(disk_image_log,
                      "Track %i out of bounds.  Cannot write GCR sector",
                      track);
            return -1;
        }
        if (image->gcr == NULL) {
            log_error(disk_image_log, "No GCR data available for writing.");
            return -1;
        }
        if (gcr_write_sector(image->gcr, buf, track, sector) < 0)
            return -1;

        disk_image_write_track(image, track, image->gcr->track_size,
                               image->gcr->speed_zone);
        break;
      default:
        log_error(disk_image_log, "Unknown disk image.  Cannot write sector.");
        return -1;
    }
    return 0;
}

