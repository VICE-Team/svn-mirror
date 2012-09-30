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
#include "fsimage.h"
#include "gcr.h"
#include "cbmdos.h"
#include "log.h"
#include "lib.h"
#include "types.h"
#include "util.h"


static log_t fsimage_gcr_log = LOG_ERR;
static const BYTE gcr_image_header_expected[] =
    { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31, 0x00 };
static const int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

/*-----------------------------------------------------------------------*/
/* Intial GCR buffer setup.  */

int fsimage_read_gcr_image(disk_image_t *image)
{
    unsigned int half_track;

    for (half_track = 0; half_track < image->half_tracks; half_track++) {
        BYTE *track_data;
        int gcr_track_size;

        if (image->gcr->track_data[half_track] == NULL) {
            image->gcr->track_data[half_track] = lib_calloc(1, NUM_MAX_MEM_BYTES_TRACK);
        }
        track_data = image->gcr->track_data[half_track];

        if (fsimage_gcr_read_half_track(image, half_track + 2, track_data, &gcr_track_size) < 0) {
            image->gcr->track_size[half_track] = raw_track_size[disk_image_speed_map(image->type, (half_track / 2) + 1)];
            continue;
        }
        image->gcr->track_size[half_track] = gcr_track_size;
    }
    for (; half_track < MAX_GCR_TRACKS; half_track++) {
        if (image->gcr->track_data[half_track]) {
            lib_free(image->gcr->track_data[half_track]);
            image->gcr->track_data[half_track] = NULL;
        }
    }
    return 0;
}
/*-----------------------------------------------------------------------*/
/* Seek to half track */

static long fsimage_gcr_seek_half_track(fsimage_t *fsimage, unsigned int half_track,
        WORD *max_track_length, BYTE *num_half_tracks)
{
    BYTE buf[12];

    if (fsimage->fd == NULL) {
        log_error(fsimage_gcr_log, "Attempt to read without disk image.");
        return -1;
    }
    if (util_fpread(fsimage->fd, buf, 12, 0) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image.");
        return -1;
    }
    if (memcmp(gcr_image_header_expected, buf, sizeof(gcr_image_header_expected)) != 0) {
        log_error(fsimage_gcr_log, "Unexpected GCR header found." );
        return -1;
    }

    *num_half_tracks = buf[9];
    if (*num_half_tracks > MAX_GCR_TRACKS) {
        log_error(fsimage_gcr_log, "Too many half tracks." );
        return -1;
    }

    *max_track_length = util_le_buf_to_word(&buf[10]);
    if (*max_track_length > NUM_MAX_MEM_BYTES_TRACK) {
        log_error(fsimage_gcr_log, "Too large max track length.");
        return -1;
    }

    if (util_fpread(fsimage->fd, buf, 4, 12 + (half_track - 2) * 4) < 0) {
        log_error(fsimage_gcr_log, "Could not read GCR disk image.");
        return -1;
    }
    return util_le_buf_to_dword(buf);
}

/*-----------------------------------------------------------------------*/
/* Read an entire GCR track from the disk image.  */

int fsimage_gcr_read_half_track(disk_image_t *image, unsigned int half_track,
                                BYTE *gcr_data, int *gcr_track_size)
{
    WORD track_len;
    BYTE buf[4];
    long offset;
    fsimage_t *fsimage;
    WORD max_track_length;
    BYTE num_half_tracks;

    fsimage = image->media.fsimage;

    offset = fsimage_gcr_seek_half_track(fsimage, half_track, &max_track_length, &num_half_tracks);
    if (offset < 0) {
        return -1;
    }

    memset(gcr_data, 0, max_track_length);

    if (offset != 0) {
        if (util_fpread(fsimage->fd, buf, 2, offset) < 0) {
            log_error(fsimage_gcr_log, "Could not read GCR disk image.");
            return -1;
        }

        track_len = util_le_buf_to_word(buf);

        if ((track_len < 1) || (track_len > max_track_length)) {
            log_error(fsimage_gcr_log,
                      "Track field length %u is not supported.",
                      track_len);
            return -1;
        }

        *gcr_track_size = track_len;

        if (fread(gcr_data, track_len, 1, fsimage->fd) < 1) {
            log_error(fsimage_gcr_log, "Could not read GCR disk image.");
            return -1;
        }
    } else {
        *gcr_track_size = raw_track_size[disk_image_speed_map(image->type, half_track / 2)];

        memset(gcr_data, 0x55, *gcr_track_size);
    }
    return 0;
}

static int fsimage_gcr_read_track(disk_image_t *image, unsigned int track,
                           BYTE *gcr_data, int *gcr_track_size)
{
    return fsimage_gcr_read_half_track(image, track << 1, gcr_data, gcr_track_size);
}

/*-----------------------------------------------------------------------*/
/* Write an entire GCR track to the disk image.  */

int fsimage_gcr_write_half_track(disk_image_t *image, unsigned int half_track,
                                 int gcr_track_size, BYTE *gcr_track_start_ptr)
{
    int gap, extend = 0;
    WORD max_track_length;
    BYTE buf[4];
    long offset;
    fsimage_t *fsimage;
    BYTE num_half_tracks;

    fsimage = image->media.fsimage;

    offset = fsimage_gcr_seek_half_track(fsimage, half_track, &max_track_length, &num_half_tracks);
    if (offset < 0) {
        return -1;
    }
    if (image->read_only != 0) {
        log_error(fsimage_gcr_log,
                  "Attempt to write to read-only disk image.");
        return -1;
    }

    if (gcr_track_size > max_track_length) {
        log_error(fsimage_gcr_log,
                  "Track too long for image.");
        return -1;
    }

    if (offset == 0) {
        offset = fseek(fsimage->fd, 0, SEEK_END);
        if (offset == 0) {
            offset = ftell(fsimage->fd);
        }
        if (offset < 0) {
            log_error(fsimage_gcr_log, "Could not extend GCR disk image.");
            return -1;
        }
        extend = 1;
    }

    if (gcr_track_start_ptr != NULL) {
        util_word_to_le_buf(buf, gcr_track_size);

        if (util_fpwrite(fsimage->fd, buf, 2, offset) < 0) {
            log_error(fsimage_gcr_log, "Could not write GCR disk image.");
            return -1;
        }

        /* Clear gap between the end of the actual track and the start of
           the next track.  */
        gap = max_track_length - gcr_track_size;
        if (gap > 0) {
            memset(gcr_track_start_ptr + gcr_track_size, 0, gap);
        }

        if (fwrite(gcr_track_start_ptr, max_track_length, 1, fsimage->fd) < 1) {
            log_error(fsimage_gcr_log, "Could not write GCR disk image.");
            return -1;
        }

        if (extend) {
            util_dword_to_le_buf(buf, offset);
            if (util_fpwrite(fsimage->fd, buf, 4, 12 + (half_track - 2) * 4) < 0) {
                log_error(fsimage_gcr_log, "Could not write GCR disk image.");
                return -1;
            }

            util_dword_to_le_buf(buf, disk_image_speed_map(image->type, half_track / 2));
            if (util_fpwrite(fsimage->fd, buf, 4, 12 + (half_track - 2 + num_half_tracks) * 4) < 0) {
                log_error(fsimage_gcr_log, "Could not write GCR disk image.");
                return -1;
            }
        }
    }

    /* Make sure the stream is visible to other readers.  */
    fflush(fsimage->fd);

    return 0;
}

static int fsimage_gcr_write_track(disk_image_t *image, unsigned int track,
                            int gcr_track_size, BYTE *gcr_track_start_ptr)
{
  return fsimage_gcr_write_half_track(image, track << 1, gcr_track_size, gcr_track_start_ptr);
}

/*-----------------------------------------------------------------------*/
/* Read a sector from the GCR disk image.  */

int fsimage_gcr_read_sector(disk_image_t *image, BYTE *buf,
                               unsigned int track, unsigned int sector)
{
    BYTE gcr_data[NUM_MAX_MEM_BYTES_TRACK];
    int gcr_track_size;
    disk_track_t raw;
    fdc_err_t rf;

    if (track > image->tracks) {
        log_error(fsimage_gcr_log,
                  "Track %i out of bounds.  Cannot read GCR track.",
                  track);
        return CBMDOS_IPE_ILLEGAL_TRACK_OR_SECTOR;
    }

    if (image->gcr == NULL) {
        if (fsimage_gcr_read_track(image, track, gcr_data, &gcr_track_size) < 0) {
            return CBMDOS_IPE_NOT_READY;
        }
        raw.data = gcr_data;
        raw.size = gcr_track_size;
    } else {
        raw.data = image->gcr->track_data[(track * 2) - 2];
        raw.size = image->gcr->track_size[(track * 2) - 2];
    }
    rf = CBMDOS_FDC_ERR_DRIVE;
    if (raw.data != NULL) {
        rf = gcr_read_sector(&raw, buf, sector);
    }
    if (rf != CBMDOS_FDC_ERR_OK) {
        log_error(fsimage_gcr_log,
                  "Cannot find track: %i sector: %i within GCR image.",
                  track, sector);
        switch (rf) {
        case CBMDOS_FDC_ERR_HEADER:
            return CBMDOS_IPE_READ_ERROR_BNF;   /* 20 */
        case CBMDOS_FDC_ERR_SYNC:
            return CBMDOS_IPE_READ_ERROR_SYNC;  /* 21 */
        case CBMDOS_FDC_ERR_NOBLOCK:
            return CBMDOS_IPE_READ_ERROR_DATA;  /* 22 */
        case CBMDOS_FDC_ERR_DCHECK:
            return CBMDOS_IPE_READ_ERROR_CHK;   /* 23 */ 
        case CBMDOS_FDC_ERR_VERIFY:
            return CBMDOS_IPE_WRITE_ERROR_VER;  /* 25 */
        case CBMDOS_FDC_ERR_WPROT:
            return CBMDOS_IPE_WRITE_PROTECT_ON; /* 26 */
        case CBMDOS_FDC_ERR_HCHECK:
            return CBMDOS_IPE_READ_ERROR_BCHK;  /* 27 */
        case CBMDOS_FDC_ERR_BLENGTH:
            return CBMDOS_IPE_WRITE_ERROR_BIG;  /* 28 */
        case CBMDOS_FDC_ERR_ID:
            return CBMDOS_IPE_DISK_ID_MISMATCH; /* 29 */
        case CBMDOS_FDC_ERR_DRIVE:
            return CBMDOS_IPE_NOT_READY;        /* 74 */
        case CBMDOS_FDC_ERR_DECODE:
            return CBMDOS_IPE_READ_ERROR_GCR;   /* 24 */
        default:
            return CBMDOS_IPE_NOT_READY;
        }
    }
    return CBMDOS_IPE_OK;
}


/*-----------------------------------------------------------------------*/
/* Write a sector to the GCR disk image.  */

int fsimage_gcr_write_sector(disk_image_t *image, BYTE *buf,
                                unsigned int track, unsigned int sector)
{
    BYTE *gcr_data;
    int gcr_track_size;
    disk_track_t raw;

    if (track > image->tracks) {
        log_error(fsimage_gcr_log,
                  "Track %i out of bounds.  Cannot write GCR sector",
                  track);
        return -1;
    }

    gcr_data = (BYTE*) lib_malloc(NUM_MAX_MEM_BYTES_TRACK);

    if (image->gcr == NULL) {
        if (fsimage_gcr_read_track(image, track, gcr_data,
            &gcr_track_size) < 0) {
            log_error(fsimage_gcr_log,
                      "Cannot read track %i from GCR image.", track);
            lib_free(gcr_data);
            return -1;
        }
        raw.data = gcr_data;
        raw.size = gcr_track_size;
    } else {
        if (image->gcr->track_data[(track * 2) - 2] == NULL) {
            image->gcr->track_data[(track * 2) - 2] = lib_calloc(1, NUM_MAX_MEM_BYTES_TRACK);
        }
        raw.data = image->gcr->track_data[(track * 2) - 2];
        raw.size = image->gcr->track_size[(track * 2) - 2];
    }
    if (gcr_write_sector(&raw, buf, sector) != CBMDOS_FDC_ERR_OK) {
        log_error(fsimage_gcr_log,
                  "Could not find track %i sector %i in disk image",
                  track, sector);
        lib_free(gcr_data);
        return -1;
    }
    if (fsimage_gcr_write_track(image, track, raw.size, raw.data) < 0) {
        log_error(fsimage_gcr_log,
                  "Failed writing track %i to disk image.", track);
        lib_free(gcr_data);
        return -1;
    }

    lib_free(gcr_data);

    return 0;
}

/*-----------------------------------------------------------------------*/

void fsimage_gcr_init(void)
{
    fsimage_gcr_log = log_open("Filesystem Image GCR");
}
