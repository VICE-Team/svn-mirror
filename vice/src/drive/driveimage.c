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
#include "drivetypes.h"
#include "gcr.h"
#include "log.h"
#include "types.h"


#define GCR_OFFSET(track) ((track - 1) * NUM_MAX_BYTES_TRACK)


/* Logging goes here.  */
static log_t driveimage_log = LOG_DEFAULT;

/* Number of bytes per track size.  */
static const unsigned int raw_track_size[4] = { 6250, 6666, 7142, 7692 };


inline static unsigned int sector_offset(unsigned int track,
                                         unsigned int sector,
                                         unsigned int max_sector,
                                         drive_t *drive)
{
    unsigned int offset;

    offset = GCR_OFFSET(track)
        + (drive->gcr->track_size[track - 1] * sector / max_sector);

    return offset;
}

void drive_image_init_track_size_d64(drive_t *drive)
{
    unsigned int track;

    for (track = 0; track < MAX_TRACKS_1541; track++) {
        drive->gcr->track_size[track] =
            raw_track_size[disk_image_speed_map_1541(track)];
        memset(drive->gcr->speed_zone, disk_image_speed_map_1541(track),
               NUM_MAX_BYTES_TRACK);
    }
}

static void drive_image_init_track_size_d71(drive_t *drive)
{
    unsigned int track;

    for (track = 0; track < MAX_TRACKS_1571; track++) {
        drive->gcr->track_size[track] =
            raw_track_size[disk_image_speed_map_1571(track)];
        memset(drive->gcr->speed_zone, disk_image_speed_map_1571(track),
               NUM_MAX_BYTES_TRACK);
    }
}

static void drive_image_read_d64_d71(drive_t *drive)
{
    BYTE buffer[260], chksum;
    int i;
    unsigned int track, sector;

    if (!(drive->image))
        return;

    buffer[258] = buffer[259] = 0;

    /* Since the D64/D71 format does not provide the actual track sizes or
       speed zones, we set them to standard values.  */
    if ((drive->image->type == DISK_IMAGE_TYPE_D64
        || drive->image->type == DISK_IMAGE_TYPE_D67
        || drive->image->type == DISK_IMAGE_TYPE_X64)
        && (drive->type == DRIVE_TYPE_1541
        || drive->type == DRIVE_TYPE_1541II
        || drive->type == DRIVE_TYPE_1551
        || drive->type == DRIVE_TYPE_1570
        || drive->type == DRIVE_TYPE_2031)) {
        drive_image_init_track_size_d64(drive);
    }
    if (drive->image->type == DISK_IMAGE_TYPE_D71
        || drive->type == DRIVE_TYPE_1571
        || drive->type == DRIVE_TYPE_1571CR
        || drive->type == DRIVE_TYPE_2031) {
        drive_image_init_track_size_d71(drive);
    }

    drive_set_half_track(drive->current_half_track, drive);

    for (track = 1; track <= drive->image->tracks; track++) {
        BYTE *ptr;
        unsigned int max_sector = 0;

        ptr = drive->gcr->data + GCR_OFFSET(track);
        max_sector = disk_image_sector_per_track(drive->image->type,
                                                 track);
        /* Clear track to avoid read errors.  */
        memset(ptr, 0xff, NUM_MAX_BYTES_TRACK);

        for (sector = 0; sector < max_sector; sector++) {
            int rc;
            ptr = drive->gcr->data + sector_offset(track, sector,
                                                   max_sector, drive);

            rc = disk_image_read_sector(drive->image, buffer + 1, track,
                                        sector);
            if (rc < 0) {
                log_error(drive->log,
                          "Cannot read T:%d S:%d from disk image.",
                          track, sector);
                          continue;
            }

            if (rc == 21) {
                ptr = drive->gcr->data + GCR_OFFSET(track);
                memset(ptr, 0x00, NUM_MAX_BYTES_TRACK);
                break;
            }

            buffer[0] = (rc == 22) ? 0xff : 0x07;

            chksum = buffer[1];
            for (i = 2; i < 257; i++)
                chksum ^= buffer[i];
            buffer[257] = (rc == 23) ? chksum ^ 0xff : chksum;
            gcr_convert_sector_to_GCR(buffer, ptr, track, sector,
                                      drive->diskID1, drive->diskID2,
                                      (BYTE)(rc));
        }
    }
}

static int setID(unsigned int dnr)
{
    BYTE buffer[256];
    int rc;
    drive_t *drive;

    drive = drive_context[dnr]->drive;

    if (!(drive->image))
        return -1;

    rc = disk_image_read_sector(drive->image, buffer, 18, 0);
    if (rc >= 0) {
        drive->diskID1 = buffer[0xa2];
        drive->diskID2 = buffer[0xa3];
    }

    return rc;
}

static int drive_check_image_format(unsigned int format, unsigned int dnr)
{
    drive_t *drive;

    drive = drive_context[dnr]->drive;

    switch (format) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_G64:
      case DISK_IMAGE_TYPE_X64:
        if (drive->type != DRIVE_TYPE_1541
            && drive->type != DRIVE_TYPE_1541II
            && drive->type != DRIVE_TYPE_1551
            && drive->type != DRIVE_TYPE_1570
            && drive->type != DRIVE_TYPE_1571
            && drive->type != DRIVE_TYPE_1571CR
            && drive->type != DRIVE_TYPE_2031
            && drive->type != DRIVE_TYPE_2040 /* FIXME: only read compat */
            && drive->type != DRIVE_TYPE_3040
            && drive->type != DRIVE_TYPE_4040)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D67:
        /* New drives and 2031, 3040 and 4040 are only read compatible.  */
        if (drive->type != DRIVE_TYPE_1541
            && drive->type != DRIVE_TYPE_1541II
            && drive->type != DRIVE_TYPE_1551
            && drive->type != DRIVE_TYPE_1570
            && drive->type != DRIVE_TYPE_1571
            && drive->type != DRIVE_TYPE_1571CR
            && drive->type != DRIVE_TYPE_2031
            && drive->type != DRIVE_TYPE_2040
            && drive->type != DRIVE_TYPE_3040
            && drive->type != DRIVE_TYPE_4040)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D71:
        if (drive->type != DRIVE_TYPE_1571
            && drive->type != DRIVE_TYPE_1571CR)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D81:
        if (drive->type != DRIVE_TYPE_1581)
            return -1;
        break;
      case DISK_IMAGE_TYPE_D80:
      case DISK_IMAGE_TYPE_D82:
        if ((drive[dnr].type != DRIVE_TYPE_1001)
            && (drive->type != DRIVE_TYPE_8050)
            && (drive->type != DRIVE_TYPE_8250))
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
    drive_t *drive;

    if (unit != 8 && unit != 9)
        return -1;

    dnr = unit - 8;
    drive = drive_context[dnr]->drive;

    if (drive_check_image_format(image->type, dnr) < 0)
        return -1;

    drive->read_only = image->read_only;
    drive->have_new_disk = 1;
    drive->attach_clk = drive_clk[dnr];
    if (drive->detach_clk > (CLOCK)0)
        drive->attach_detach_clk = drive_clk[dnr];
    drive->ask_extend_disk_image = 1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D64:
        disk_image_attach_log(image, driveimage_log, unit, "D64");
        break;
      case DISK_IMAGE_TYPE_D67:
        disk_image_attach_log(image, driveimage_log, unit, "D67");
        break;
      case DISK_IMAGE_TYPE_D71:
        disk_image_attach_log(image, driveimage_log, unit, "D71");
        break;
      case DISK_IMAGE_TYPE_G64:
        disk_image_attach_log(image, driveimage_log, unit, "G64");
        break;
      case DISK_IMAGE_TYPE_X64:
        disk_image_attach_log(image, driveimage_log, unit, "X64");
        break;
      default:
        return -1;
    }

    drive->image = image;
    drive->image->gcr = drive->gcr;

    if (drive->image->type == DISK_IMAGE_TYPE_G64) {
        if (disk_image_read_gcr_image(drive->image) < 0) {
            return -1;
        }
    } else {
        if (setID(dnr) >= 0) {
            drive_image_read_d64_d71(drive);
            drive->GCR_image_loaded = 1;
            return 0;
        } else {
            return -1;
        }
    }
    drive->GCR_image_loaded = 1;

    return 0;
}

/* Detach a disk image from the true drive emulation. */
int drive_image_detach(disk_image_t *image, unsigned int unit)
{
    unsigned int dnr;
    drive_t *drive;

    if (unit != 8 && unit != 9)
        return -1;

    dnr = unit - 8;
    drive = drive_context[dnr]->drive;

    if (drive->image != NULL) {
        switch(image->type) {
          case DISK_IMAGE_TYPE_D64:
            disk_image_detach_log(image, driveimage_log, unit, "D64");
            break;
          case DISK_IMAGE_TYPE_D67:
            disk_image_detach_log(image, driveimage_log, unit, "D67");
            break;
          case DISK_IMAGE_TYPE_D71:
            disk_image_detach_log(image, driveimage_log, unit, "D71");
            break;
          case DISK_IMAGE_TYPE_G64:
            disk_image_detach_log(image, driveimage_log, unit, "G64");
            break;
          case DISK_IMAGE_TYPE_X64:
            disk_image_detach_log(image, driveimage_log, unit, "X64");
            break;
          default:
            return -1;
        }

        drive_gcr_data_writeback(&drive[dnr]);
        memset(drive->gcr->data, 0, MAX_GCR_TRACKS * NUM_MAX_BYTES_TRACK);
        drive->detach_clk = drive_clk[dnr];
        drive->GCR_image_loaded = 0;
        drive->read_only = 0;
        drive->image = NULL;
    }
    return 0;
}

void drive_image_init(void)
{
    driveimage_log = log_open("DriveImage");
}

