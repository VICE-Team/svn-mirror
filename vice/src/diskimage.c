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
#include "log.h"
#include "utils.h"

#include "vdrive.h"

#include "zfile.h"

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
            log_error(LOG_ERR, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1541) {
        log_error(LOG_ERR, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 683:
        image->tracks = NUM_TRACKS_1541;
        break;
      case 685:
        if (len != 171) {
            log_message(vdrive_log, "Cannot read block %d", blk);
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
            log_error(LOG_ERR, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1571) {
        log_error(LOG_ERR, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 1366:
        image->tracks = NUM_TRACKS_1571;
        break;
      default:
        return 0;
    }
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
            log_error(LOG_ERR, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_1581) {
        log_error(LOG_ERR, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case 3200:
        image->tracks = NUM_TRACKS_1581;
        break;
      default:
        return 0;
    }
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
            log_error(LOG_ERR, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_8050) {
        log_error(LOG_ERR, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case NUM_BLOCKS_8050:
        image->tracks = NUM_TRACKS_8050;
        break;
      default:
        return 0;
    }
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
            log_error(LOG_ERR, "Disk image too large");
            break;
        }
    }

    if (blk < NUM_BLOCKS_8250) {
        log_error(LOG_ERR, "Cannot read block %d", blk);
        return 0;
    }

    switch (blk) {
      case NUM_BLOCKS_8250:
        image->tracks = NUM_TRACKS_8250;
        break;
      default:
        return 0;
    }
    return 1;
}

static int disk_image_check_for_gcr(disk_image_t *image)
{
    int trackfield;
    BYTE header[32];

    fseek(image->fd, 0, SEEK_SET);
    if (fread((BYTE *)header, sizeof (header), 1, image->fd) < 1) {
        log_error(LOG_ERR, "Cannot read image header.");
        return 0;
    }

    if (strncmp("GCR-1541", (char*)header,8))
        return 0;

    if (header[8] != 0) {
        log_error(LOG_ERR, "Import GCR: Wrong GCR image version.");
        return 0;
    }

    if (header[9] < NUM_TRACKS_1541 * 2 || header[9] > MAX_TRACKS_1541 * 2) {
        log_error(LOG_ERR, "Import GCR: Invalid number of tracks.");
        return 0;
    }

    trackfield = header[10] + header[11] * 256;
    if (trackfield != 7928) {
        log_error(LOG_ERR, "Import GCR: Invalid track field number.");
        return 0;
    }

    image->type = DISK_IMAGE_TYPE_GCR;
    image->tracks = header[9] / 2;
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
        log_error(LOG_ERR, "Cannot open file `%s'.", image->name);
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

int disk_image_close(disk_image_t *image)
{
    return 0;
}

int disk_image_read_sector(disk_image_t *image, BYTE *buf, int track,
                           int sector)
{
    return 0;
}

int disk_image_write_sector(disk_image_t *image, BYTE *buf, int track,
                            int sector)
{
    return 0;
}

