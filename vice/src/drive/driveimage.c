/*
 * driveimage.c
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
#include "drive.h"
#include "driveimage.h"
#include "gcr.h"


/* Number of bytes in one raw sector.  */
#define NUM_BYTES_SECTOR_GCR 360

#define GCR_OFFSET(track, sector)  ((track - 1) * NUM_MAX_BYTES_TRACK \
                                    + sector * NUM_BYTES_SECTOR_GCR)


/* Logging goes here.  */
static log_t driveimage_log = LOG_ERR;

/* Number of bytes per track size.  */
static unsigned int raw_track_size[4] = { 6250, 6666, 7142, 7692 };


void drive_image_init_track_size_d64(unsigned int dnr)
{
    unsigned int track;

    for (track = 0; track < MAX_TRACKS_1541; track++) {
        drive[dnr].gcr->track_size[track] =
            raw_track_size[disk_image_speed_map_1541(track)];
        memset(drive[dnr].gcr->speed_zone, disk_image_speed_map_1541(track),
               NUM_MAX_BYTES_TRACK);
    }
}

static void drive_image_init_track_size_d71(unsigned int dnr)
{
    unsigned int track;

    for (track = 0; track < MAX_TRACKS_1571; track++) {
        drive[dnr].gcr->track_size[track] =
            raw_track_size[disk_image_speed_map_1571(track)];
        memset(drive[dnr].gcr->speed_zone, disk_image_speed_map_1571(track),
               NUM_MAX_BYTES_TRACK);
    }
}

static void drive_image_read_d64_d71(unsigned int dnr)
{
    BYTE buffer[260], chksum;
    int i;
    unsigned int track, sector;

    if (!drive[dnr].image)
        return;

    buffer[258] = buffer[259] = 0;

    /* Since the D64/D71 format does not provide the actual track sizes or
       speed zones, we set them to standard values.  */
    if ((drive[dnr].image->type == DISK_IMAGE_TYPE_D64
        || drive[dnr].image->type == DISK_IMAGE_TYPE_D67
        || drive[dnr].image->type == DISK_IMAGE_TYPE_X64)
        && (drive[dnr].type == DRIVE_TYPE_1541
        || drive[dnr].type == DRIVE_TYPE_1541II
        || drive[dnr].type == DRIVE_TYPE_1551
        || drive[dnr].type == DRIVE_TYPE_2031)) {
        drive_image_init_track_size_d64(dnr);
    }
    if (drive[dnr].image->type == DISK_IMAGE_TYPE_D71
        || drive[dnr].type == DRIVE_TYPE_1571
        || drive[dnr].type == DRIVE_TYPE_2031) {
        drive_image_init_track_size_d71(dnr);
    }

    drive_set_half_track(drive[dnr].current_half_track, &drive[dnr]);

    for (track = 1; track <= drive[dnr].image->tracks; track++) {
        BYTE *ptr;
        unsigned int max_sector = 0;

        ptr = drive[dnr].gcr->data + GCR_OFFSET(track, 0);
        max_sector = disk_image_sector_per_track(drive[dnr].image->type,
                                                 track);
        /* Clear track to avoid read errors.  */
        memset(ptr, 0xff, NUM_MAX_BYTES_TRACK);

        for (sector = 0; sector < max_sector; sector++) {
            int rc;
            ptr = drive[dnr].gcr->data + GCR_OFFSET(track, sector);

            rc = disk_image_read_sector(drive[dnr].image, buffer + 1, track,
                                        sector);
            if (rc < 0) {
                log_error(drive[dnr].log,
                          "Cannot read T:%d S:%d from disk image.",
                          track, sector);
                          continue;
            }

            if (rc == 21) {
                ptr = drive[dnr].gcr->data + GCR_OFFSET(track, 0);
                memset(ptr, 0x00, NUM_MAX_BYTES_TRACK);
                break;
            }

            buffer[0] = (rc == 22) ? 0xff : 0x07;

            chksum = buffer[1];
            for (i = 2; i < 257; i++)
                chksum ^= buffer[i];
            buffer[257] = (rc == 23) ? chksum ^ 0xff : chksum;
            gcr_convert_sector_to_GCR(buffer, ptr, track, sector,
                                      drive[dnr].diskID1, drive[dnr].diskID2,
                                      (BYTE)(rc));
        }
    }
}

static int setID(unsigned int dnr)
{
    BYTE buffer[256];
    int rc;

    if (!drive[dnr].image)
        return -1;

    rc = disk_image_read_sector(drive[dnr].image, buffer, 18, 0);
    if (rc >= 0) {
        drive[dnr].diskID1 = buffer[0xa2];
        drive[dnr].diskID2 = buffer[0xa3];
    }

    return rc;
}

static int drive_check_image_format(unsigned int format, unsigned int dnr)
{
    switch (format) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_G64:
      case DISK_IMAGE_TYPE_X64:
        if (drive[dnr].type != DRIVE_TYPE_1541
            && drive[dnr].type != DRIVE_TYPE_1541II
            && drive[dnr].type != DRIVE_TYPE_1551
            && drive[dnr].type != DRIVE_TYPE_1571
            && drive[dnr].type != DRIVE_TYPE_2031
            && drive[dnr].type != DRIVE_TYPE_2040 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_3040
            && drive[dnr].type != DRIVE_TYPE_4040)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D67:
        if (drive[dnr].type != DRIVE_TYPE_1541 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_1541II /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_1551 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_1571 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_2031 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_2040
            && drive[dnr].type != DRIVE_TYPE_3040 /* FIXME: only read compat */
            && drive[dnr].type != DRIVE_TYPE_4040) /* FIXME: only read compat */            return -1;
        break;
      case DISK_IMAGE_TYPE_D71:
        if (drive[dnr].type != DRIVE_TYPE_1571)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D81:
        if (drive[dnr].type != DRIVE_TYPE_1581)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        if ((drive[dnr].type != DRIVE_TYPE_1001)
            && (drive[dnr].type != DRIVE_TYPE_8050)
            && (drive[dnr].type != DRIVE_TYPE_8250))
            return -1;
        break;
      default:
        return -1;
    }
    return 0;
}

/* Attach a disk image to the true drive emulation. */
int drive_image_attach(disk_image_t *image, unsigned int unit)
{
    unsigned int dnr;

    if (unit != 8 && unit != 9)
        return -1;

    dnr = unit - 8;

    if (drive_check_image_format(image->type, dnr) < 0)
        return -1;

    drive[dnr].read_only = image->read_only;
    drive[dnr].have_new_disk = 1;
    drive[dnr].attach_clk = drive_clk[dnr];
    if (drive[dnr].detach_clk > (CLOCK)0)
        drive[dnr].attach_detach_clk = drive_clk[dnr];
    drive[dnr].ask_extend_disk_image = 1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D64:
        log_message(driveimage_log, "Unit %d: D64 disk image attached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D67:
        log_message(driveimage_log, "Unit %d: D67 disk image attached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D71:
        log_message(driveimage_log, "Unit %d: D71 disk image attached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_G64:
        log_message(driveimage_log, "Unit %d: G64 disk image attached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_X64:
        log_message(driveimage_log, "Unit %d: X64 disk image attached: %s.",
                    unit, image->name);
        break;
      default:
        return -1;
    }

    drive[dnr].image = image;
    drive[dnr].image->gcr = drive[dnr].gcr;

    if (drive[dnr].image->type == DISK_IMAGE_TYPE_G64) {
        if (disk_image_read_gcr_image(drive[dnr].image) < 0) {
            return -1;
        }
    } else {
        if (setID(dnr) >= 0) {
            drive_image_read_d64_d71(dnr);
            drive[dnr].GCR_image_loaded = 1;
            return 0;
        } else {
            return -1;
        }
    }
    drive[dnr].GCR_image_loaded = 1;

    return 0;
}

/* Detach a disk image from the true drive emulation. */
int drive_image_detach(disk_image_t *image, unsigned int unit)
{
    unsigned int dnr;

    if (unit != 8 && unit != 9)
        return -1;

    dnr = unit - 8;

    if (drive[dnr].image != NULL) {
        switch(image->type) {
          case DISK_IMAGE_TYPE_D64:
            log_message(driveimage_log, "Unit %d: D64 disk image detached: %s.",
                        unit, image->name);
            break;
          case DISK_IMAGE_TYPE_D67:
            log_message(driveimage_log, "Unit %d: D67 disk image detached: %s.",
                        unit, image->name);
            break;
          case DISK_IMAGE_TYPE_D71:
            log_message(driveimage_log, "Unit %d: D71 disk image detached: %s.",
                        unit, image->name);
            break;
          case DISK_IMAGE_TYPE_G64:
            log_message(driveimage_log, "Unit %d: G64 disk image detached: %s.",
                        unit, image->name);
            break;
          case DISK_IMAGE_TYPE_X64:
            log_message(driveimage_log, "Unit %d: X64 disk image detached: %s.",
                        unit, image->name);
            break;
          default:
            return -1;
        }

        drive_gcr_data_writeback(dnr);
        memset(drive[dnr].gcr->data, 0, MAX_GCR_TRACKS * NUM_MAX_BYTES_TRACK);
        drive[dnr].detach_clk = drive_clk[dnr];
        drive[dnr].GCR_image_loaded = 0;
        drive[dnr].read_only = 0;
        drive[dnr].image = NULL;
    }
    return 0;
}

void drive_image_init(void)
{
    driveimage_log = log_open("DriveImage");
}

