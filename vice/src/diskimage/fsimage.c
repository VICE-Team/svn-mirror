/*
 * fsimage.c
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

#include "archdep.h"
#include "diskimage.h"
#include "fsimage-gcr.h"
#include "fsimage-probe.h"
#include "fsimage.h"
#include "log.h"
#include "types.h"
#include "x64.h"
#include "zfile.h"


static log_t fsimage_log = LOG_ERR;


int fsimage_open(disk_image_t *image)
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
        log_error(fsimage_log, "Cannot open file `%s'.", image->name);
        return -1;
    }

    if (fsimage_probe(image) == 0)
        return 0;

    zfclose(image->fd);
    log_message(fsimage_log, "Unknown disk image `%s'.", image->name);
    return -1;
}

int fsimage_close(disk_image_t *image)
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

int fsimage_read_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                        unsigned int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(fsimage_log, "Attempt to read without disk image.");
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
            log_error(fsimage_log, "Track %i, Sector %i out of bounds.",
                      track, sector);
            return 66;
        }

        offset = sectors << 8;

        if (image->type == DISK_IMAGE_TYPE_X64)
            offset += X64_HEADER_LENGTH;

        fseek(image->fd, offset, SEEK_SET);

        if (fread((char *)buf, 256, 1, image->fd) < 1) {
            log_error(fsimage_log,
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
        if (fsimage_gcr_read_sector(image, buf, track, sector) < 0)
            return -1;
        break;
      default:
        log_error(fsimage_log,
                  "Unknown disk image type %i.  Cannot read sector.",
                  image->type);
        return -1;
    }
    return 0;
}

int fsimage_write_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                         unsigned int sector)
{
    int sectors;
    long offset;

    if (image->fd == NULL) {
        log_error(fsimage_log, "Attempt to write without disk image.");
        return -1;
    }

    if (image->read_only != 0) {
        log_error(fsimage_log, "Attempt to write to read-only disk image.");
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
            log_error(fsimage_log, "Track: %i, Sector: %i out of bounds.",
                      track, sector);
            return -1;
        }
        offset = sectors << 8;

        if (image->type == DISK_IMAGE_TYPE_X64)
            offset += X64_HEADER_LENGTH;

        fseek(image->fd, offset, SEEK_SET);

        if (fwrite((char *)buf, 256, 1, image->fd) < 1) {
            log_error(fsimage_log, "Error writing T:%i S:%i to disk image.",
                      track, sector);
            return -1;
        }
        break;
      case DISK_IMAGE_TYPE_G64:
        if (fsimage_gcr_write_sector(image, buf, track, sector) < 0)
            return -1;
        break;
      default:
        log_error(fsimage_log, "Unknown disk image.  Cannot write sector.");
        return -1;
    }
    return 0;
}

/*-----------------------------------------------------------------------*/

void fsimage_init(void)
{
    fsimage_log = log_open("Filesystem Image");
    fsimage_gcr_init();
    fsimage_probe_init();
}

