/*
 * probe.c - Probe disk images.
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
#include <string.h>

#include "diskconstants.h"
#include "diskimage.h"
#include "log.h"
#include "types.h"
#include "utils.h"
#include "x64.h"


#define IS_D67_LEN(x) ((x) == D67_FILE_SIZE)
#define IS_D71_LEN(x) ((x) == D71_FILE_SIZE)
#define IS_D81_LEN(x) ((x) == D81_FILE_SIZE)
#define IS_D80_LEN(x) ((x) == D80_FILE_SIZE)
#define IS_D82_LEN(x) ((x) == D82_FILE_SIZE)


static log_t disk_image_probe_log = LOG_ERR;


static void disk_image_check_log(disk_image_t *image, const char *type)
{
    log_message(disk_image_probe_log,
                "%s disk image recognised: %s, %d tracks%s",
                type, image->name, image->tracks,
                image->read_only ? " (read only)." : ".");
}

static int disk_image_check_min_block(unsigned int blk, unsigned int length)
{
    if (blk < length) {
        log_error(disk_image_probe_log, "Cannot read block %d.", blk);
        return -1;
    }
    return 0;
}


static int disk_image_check_for_d64(disk_image_t *image)
{
    /*** detect 35..42 track d64 image, determine image parameters.
         Walk from 35 to 42, calculate expected image file size for each track,
         and compare this with the size of the given image. */

    int checkimage_tracks, checkimage_errorinfo;
    size_t countbytes, checkimage_blocks, checkimage_realsize;

    checkimage_errorinfo = 0;
    checkimage_realsize = util_file_length(image->fd);
    checkimage_tracks = NUM_TRACKS_1541; /* start at track 35 */
    checkimage_blocks = D64_FILE_SIZE_35 / 256;

    while (1) {
        /* check if image matches "checkimage_tracks" */
        if (checkimage_realsize == checkimage_blocks * 256) {
            /* image file matches size-with-no-error-info */
            checkimage_errorinfo = 0;
            break;

        } else if (checkimage_realsize == checkimage_blocks * 256
            + checkimage_blocks) {
            /* image file matches size-with-error-info */
            checkimage_errorinfo = 1;
            break;
        }

        /* try next track (all tracks from 35 to 42 have 17 blocks) */
        checkimage_tracks++;
        checkimage_blocks += 17;

        /* we tried them all up to 42, none worked, image must be corrupt */
        if (checkimage_tracks > MAX_TRACKS_1541)
            return 0;
    }

    /*** test image file: read it (fgetc is pretty fast).
         further size checks are no longer necessary (done during detection) */
    rewind(image->fd);
    for (countbytes = 0; countbytes < checkimage_realsize; countbytes++) {
        if (fgetc(image->fd) == EOF) {
            log_error(disk_image_probe_log, "Cannot read D64 image.");
            return 0;
        }
    }

    /*** set parameters in image structure, read error info */
    image->type = DISK_IMAGE_TYPE_D64;
    image->tracks = checkimage_tracks;
    if (checkimage_errorinfo) {
        image->error_info = (BYTE *)xcalloc(1, MAX_BLOCKS_1541);
        if (fseek(image->fd, (long)(256 * checkimage_blocks), SEEK_SET) < 0)
            return 0;
        if (fread(image->error_info, 1, checkimage_blocks, image->fd)
            < checkimage_blocks)
            return 0;
    }

    /*** log and return successfully */
    disk_image_check_log(image, "D64");
    return 1;
}


static int disk_image_check_for_d67(disk_image_t *image)
{
    unsigned int blk = 0;
    size_t len;
    BYTE block[256];

    if (!(IS_D67_LEN(util_file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D67;
    image->tracks = NUM_TRACKS_2040;

    rewind(image->fd);

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        /* FIXME */
        if (++blk > (NUM_BLOCKS_2040)) {
            log_error(disk_image_probe_log, "Disk image too large");
            break;
        }
    }

    if (disk_image_check_min_block(blk, NUM_BLOCKS_2040) < 0)
        return 0;

    switch (blk) {
      case NUM_BLOCKS_2040:
        image->tracks = NUM_TRACKS_2040;
        break;
      default:
        return 0;
    }
    disk_image_check_log(image, "D67");
    return 1;
}

static int disk_image_check_for_d71(disk_image_t *image)
{
    unsigned int blk = 0;
    size_t len;
    BYTE block[256];

    if (!(IS_D71_LEN(util_file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D71;
    image->tracks = NUM_TRACKS_1571;

    rewind(image->fd);

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > 1372) {
            log_error(disk_image_probe_log, "Disk image too large.");
            break;
        }
    }

    if (disk_image_check_min_block(blk, NUM_BLOCKS_1571) < 0)
        return 0;

    switch (blk) {
      case 1366:
        image->tracks = NUM_TRACKS_1571;
        break;
      default:
        return 0;
    }
    disk_image_check_log(image, "D71");
    return 1;
}

static int disk_image_check_for_d81(disk_image_t *image)
{
    unsigned int blk = 0;
    size_t len;
    BYTE block[256];

    if (!(IS_D81_LEN(util_file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D81;
    image->tracks = NUM_TRACKS_1581;

    rewind(image->fd);

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > 3213) {
            log_error(disk_image_probe_log, "Disk image too large.");
            break;
        }
    }

    if (disk_image_check_min_block(blk, NUM_BLOCKS_1581) < 0)
        return 0;

    switch (blk) {
      case 3200:
        image->tracks = NUM_TRACKS_1581;
        break;
      default:
        return 0;
    }
    disk_image_check_log(image, "D81");
    return 1;
}

static int disk_image_check_for_d80(disk_image_t *image)
{
    unsigned int blk = 0;
    size_t len;
    BYTE block[256];

    if (!(IS_D80_LEN(util_file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D80;
    image->tracks = NUM_TRACKS_8050;

    rewind(image->fd);

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > NUM_BLOCKS_8050 + 6) {
            log_error(disk_image_probe_log, "Disk image too large.");
            break;
        }
    }

    if (disk_image_check_min_block(blk, NUM_BLOCKS_8050) < 0)
        return 0;

    switch (blk) {
      case NUM_BLOCKS_8050:
        image->tracks = NUM_TRACKS_8050;
        break;
      default:
        return 0;
    }
    disk_image_check_log(image, "D80");
    return 1;
}

static int disk_image_check_for_d82(disk_image_t *image)
{
    unsigned int blk = 0;
    size_t len;
    BYTE block[256];

    if (!(IS_D82_LEN(util_file_length(image->fd))))
        return 0;

    image->type = DISK_IMAGE_TYPE_D82;
    image->tracks = NUM_TRACKS_8250;

    rewind(image->fd);

    while ((len = fread(block, 1, 256, image->fd)) == 256) {
        if (++blk > NUM_BLOCKS_8250 + 6) {
            log_error(disk_image_probe_log, "Disk image too large.");
            break;
        }
    }

    if (disk_image_check_min_block(blk, NUM_BLOCKS_8250) < 0)
        return 0;

    switch (blk) {
      case NUM_BLOCKS_8250:
        image->tracks = NUM_TRACKS_8250;
        break;
      default:
        return 0;
    }
    disk_image_check_log(image, "D82");
    return 1;
}

static int disk_image_check_for_x64(disk_image_t *image)
{
    BYTE header[X64_HEADER_LENGTH];

    rewind(image->fd);

    if (fread(header, X64_HEADER_LENGTH, 1, image->fd) < 1)
        return 0;

    if (header[X64_HEADER_MAGIC_OFFSET + 0] != X64_HEADER_MAGIC_1 ||
        header[X64_HEADER_MAGIC_OFFSET + 1] != X64_HEADER_MAGIC_2 ||
        header[X64_HEADER_MAGIC_OFFSET + 2] != X64_HEADER_MAGIC_3 ||
        header[X64_HEADER_MAGIC_OFFSET + 3] != X64_HEADER_MAGIC_4)
        return 0;

    if (header[X64_HEADER_FLAGS_OFFSET + 1] > MAX_TRACKS_1541)
        return 0;

    image->type = DISK_IMAGE_TYPE_X64;
    image->tracks = header[X64_HEADER_FLAGS_OFFSET + 1];

    disk_image_check_log(image, "X64");
    return 1;
}

static int disk_image_check_for_gcr(disk_image_t *image)
{
    int trackfield;
    BYTE header[32];

    fseek(image->fd, 0, SEEK_SET);
    if (fread((BYTE *)header, sizeof (header), 1, image->fd) < 1) {
        log_error(disk_image_probe_log, "Cannot read image header.");
        return 0;
    }

    if (strncmp("GCR-1541", (char*)header, 8))
        return 0;

    if (header[8] != 0) {
        log_error(disk_image_probe_log,
                  "Import GCR: Unknown GCR image version %i.",
                  (int)header[8]);
        return 0;
    }

    if (header[9] < NUM_TRACKS_1541 * 2 || header[9] > MAX_TRACKS_1541 * 2) {
        log_error(disk_image_probe_log,
                  "Import GCR: Invalid number of tracks (%i).",
                  (int)header[9]);
        return 0;
    }

    trackfield = header[10] + header[11] * 256;
    if (trackfield != 7928) {
        log_error(disk_image_probe_log,
                  "Import GCR: Invalid track field number %i.",
                  trackfield);
        return 0;
    }

    image->type = DISK_IMAGE_TYPE_G64;
    image->tracks = header[9] / 2;
    disk_image_check_log(image, "GCR");

    if (image->gcr != NULL) {
        if (disk_image_read_gcr_image(image) < 0)
            return 0;
    }
    return 1;
}


int disk_image_probe(disk_image_t *image)
{
    if (disk_image_check_for_d64(image))
        return 0;
    if (disk_image_check_for_d67(image))
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
    if (disk_image_check_for_x64(image))
        return 0;

    return -1;
}

void disk_image_probe_init(void)
{
    disk_image_probe_log = log_open("Disk Image Probe");
}

