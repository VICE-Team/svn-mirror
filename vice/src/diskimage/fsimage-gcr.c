/*
 * fsimage-gcr.c
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
#include <string.h>

#include "diskconstants.h"
#include "diskimage.h"
#include "fsimage-gcr.h"
#include "gcr.h"
#include "log.h"
#include "types.h"
#include "utils.h"


static log_t fsimage_gcr_log = LOG_ERR;


/*-----------------------------------------------------------------------*/
/* Intial GCR buffer setup.  */

int fsimage_read_gcr_image(disk_image_t *image)
{
    unsigned int track, num_tracks;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    fsimage_t *fsimage;

    fsimage = (fsimage_t *)(image->media);

    num_tracks = image->tracks;

    fseek(fsimage->fd, 12, SEEK_SET);
    if (util_dword_read(fsimage->fd, gcr_track_p, num_tracks * 8) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image.");
        return -1;
    }

    fseek(fsimage->fd, 12 + num_tracks * 8, SEEK_SET);
    if (util_dword_read(fsimage->fd, gcr_speed_p, num_tracks * 8) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image.");
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

            fseek(fsimage->fd, offset, SEEK_SET);
            if (fread((char *)len, 2, 1, fsimage->fd) < 1) {
                log_error(fsimage_gcr_log, "Could not read GCR disk image.");
                return -1;
            }

            track_len = len[0] + len[1] * 256;

            if (track_len < 5000 || track_len > 7928) {
                log_error(fsimage_gcr_log,
                          "Track field length %i is not supported.",
                          (int)track_len);
                return -1;
            }

            image->gcr->track_size[track] = track_len;

            fseek(fsimage->fd, offset + 2, SEEK_SET);
            if (fread((char *)track_data, track_len, 1, fsimage->fd) < 1) {
                log_error(fsimage_gcr_log, "Could not read GCR disk image.");
                return -1;
            }

            zone_len = (track_len + 3) / 4;

            if (gcr_speed_p[track * 2] > 3) {
                unsigned int i;
                BYTE comp_speed[NUM_MAX_BYTES_TRACK / 4];

                offset = gcr_speed_p[track * 2];

                fseek(fsimage->fd, offset, SEEK_SET);
                if (fread((char *)comp_speed, zone_len, 1, fsimage->fd) < 1) {
                    log_error(fsimage_gcr_log,
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
/* Read an entire GCR track from the disk image.  */

int fsimage_gcr_read_track(disk_image_t *image, unsigned int track,
                           BYTE *gcr_data, int *gcr_track_size)
{
    int track_len;
    BYTE len[2];
    DWORD gcr_track_p;
    long offset;
    fsimage_t *fsimage;

    fsimage = (fsimage_t *)(image->media);

    if (fsimage->fd == NULL) {
        log_error(fsimage_gcr_log, "Attempt to read without disk image.");
        return -1;
    }

    fseek(fsimage->fd, 12 + (track - 1) * 8, SEEK_SET);
    if (util_dword_read(fsimage->fd, &gcr_track_p, 4) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image.");
        return -1;
    }

    memset(gcr_data, 0xff, NUM_MAX_BYTES_TRACK);
    *gcr_track_size = 6250;

    if (gcr_track_p != 0) {

        offset = gcr_track_p;

        fseek(fsimage->fd, offset, SEEK_SET);
        if (fread((char *)len, 2, 1, fsimage->fd) < 1) {
            log_error(fsimage_gcr_log, "Could not read GCR disk image.");
            return -1;
        }

        track_len = len[0] + len[1] * 256;

        if (track_len < 5000 || track_len > 7928) {
            log_error(fsimage_gcr_log,
                      "Track field length %i is not supported.",
                      track_len);
            return -1;
        }

        *gcr_track_size = track_len;

        fseek(fsimage->fd, offset + 2, SEEK_SET);
        if (fread((char *)gcr_data, track_len, 1, fsimage->fd) < 1) {
            log_error(fsimage_gcr_log, "Could not read GCR disk image.");
            return -1;
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------*/
/* Write an entire GCR track to the disk image.  */

int fsimage_gcr_write_track(disk_image_t *image, unsigned int track,
                            int gcr_track_size, BYTE *gcr_speed_zone,
                            BYTE *gcr_track_start_ptr)
{
    int gap, i;
    unsigned int num_tracks;
    BYTE len[2];
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    int offset;
    fsimage_t *fsimage;

    fsimage = (fsimage_t *)(image->media);


    if (fsimage->fd == NULL) {
        log_error(fsimage_gcr_log, "Attempt to write without disk image.");
        return -1;
    }

    if (image->read_only != 0) {
        log_error(fsimage_gcr_log,
                  "Attempt to write to read-only disk image.");
        return -1;
    }

    num_tracks = image->tracks;

    fseek(fsimage->fd, 12, SEEK_SET);
    if (util_dword_read(fsimage->fd, gcr_track_p, num_tracks * 8) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image header.");
        return -1;
    }

    fseek(fsimage->fd, 12 + num_tracks * 8, SEEK_SET);
    if (util_dword_read(fsimage->fd, gcr_speed_p, num_tracks * 8) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image header.");
        return -1;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
        offset = fseek(fsimage->fd, 0, SEEK_END);
        if (offset < 0) {
            log_error(fsimage_gcr_log, "Could not extend GCR disk image.");
            return -1;
        }
        gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = gcr_track_size % 256;
    len[1] = gcr_track_size / 256;

    if (fseek(fsimage->fd, offset, SEEK_SET) < 0
        || fwrite((char *)len, 2, 1, fsimage->fd) < 1) {
        log_error(fsimage_gcr_log, "Could not write GCR disk image.");
        return -1;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - gcr_track_size;
    if (gap > 0)
        memset(gcr_track_start_ptr + gcr_track_size, 0, gap);

    if (fseek(fsimage->fd, offset + 2, SEEK_SET) < 0
        || fwrite((char *)gcr_track_start_ptr, NUM_MAX_BYTES_TRACK, 1,
        fsimage->fd) < 1) {
        log_error(fsimage_gcr_log, "Could not write GCR disk image.");
        return -1;
    }

    if (gcr_speed_zone != NULL) {
        for (i = 0; (gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
            == gcr_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
            && i < NUM_MAX_BYTES_TRACK; i++);

        if (i < gcr_track_size) {
            /* This will change soon.  */
            log_error(fsimage_gcr_log,
                      "Saving different speed zones is not supported yet.");
            return -1;
        }

        if (gcr_speed_p[(track - 1) * 2] >= 4) {
            /* This will change soon.  */
            log_error(fsimage_gcr_log,
                      "Adding new speed zones is not supported yet.");
            return -1;
        }

        offset = 12 + num_tracks * 8 + (track - 1) * 8;
        if (fseek(fsimage->fd, offset, SEEK_SET) < 0
            || util_dword_write(fsimage->fd, &gcr_speed_p[(track - 1) * 2], 4)
            < 0) {
            log_error(fsimage_gcr_log, "Could not write GCR disk image.");
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

    if (fseek(fsimage->fd, offset, SEEK_SET) < 0
        || fwrite((char *)comp_speed, NUM_MAX_BYTES_TRACK / 4, 1
        fsimage->fd) < 1) {
        log_error(fsimage_gcr_log, "Could not write GCR disk image");
        return;
    }
#endif
    return 0;
}

/*-----------------------------------------------------------------------*/
/* Read a sector from the GCR disk image.  */

int fsimage_gcr_read_sector(disk_image_t *image, BYTE *buf,
                               unsigned int track, unsigned int sector)
{
    BYTE gcr_data[NUM_MAX_BYTES_TRACK], *gcr_track_start_ptr;
    int gcr_track_size, gcr_current_track_size;

    if (track > image->tracks) {
        log_error(fsimage_gcr_log,
                  "Track %i out of bounds.  Cannot read GCR track.",
                  track);
        return -1;
    }
    if (image->gcr == NULL) {
        if (fsimage_gcr_read_track(image, track, gcr_data,
            &gcr_track_size) < 0) {
            log_error(fsimage_gcr_log,
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
        log_error(fsimage_gcr_log,
                  "Cannot find track: %i sector: %i within GCR image.",
                  track, sector);
        return -1;
    }

    return 0;
}


/*-----------------------------------------------------------------------*/
/* Write a sector to the GCR disk image.  */

int fsimage_gcr_write_sector(disk_image_t *image, BYTE *buf,
                                unsigned int track, unsigned int sector)
{
    BYTE gcr_data[NUM_MAX_BYTES_TRACK];
    BYTE *gcr_track_start_ptr, *speed_zone;
    int gcr_track_size, gcr_current_track_size;

    if (track > image->tracks) {
        log_error(fsimage_gcr_log,
                  "Track %i out of bounds.  Cannot write GCR sector",
                  track);
        return -1;
    }
    if (image->gcr == NULL) {
        if (fsimage_gcr_read_track(image, track, gcr_data,
            &gcr_track_size) < 0) {
            log_error(fsimage_gcr_log,
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
        log_error(fsimage_gcr_log,
                  "Could not find track %i sector %i in disk image",
                  track, sector);
        return -1;
    }
    if (disk_image_write_track(image, track, gcr_current_track_size,
        speed_zone, gcr_track_start_ptr) < 0) {
        log_error(fsimage_gcr_log,
                  "Failed writing track %i to disk image.", track);
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/

void fsimage_gcr_init(void)
{
    fsimage_gcr_log = log_open("Filesystem Image GCR");
}

