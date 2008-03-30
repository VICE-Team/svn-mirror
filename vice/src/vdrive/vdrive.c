/*
 * vdrive.c - Virtual disk-drive implementation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

/* #define DEBUG_DRIVE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "archdep.h"
#include "attach.h"
#include "cbmdos.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "fsdevice.h"
#include "lib.h"
#include "log.h"
#include "types.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive-internal.h"
#include "vdrive-rel.h"
#include "vdrive-snapshot.h"
#include "vdrive.h"


static log_t vdrive_log = LOG_ERR;

static void vdrive_set_disk_geometry(vdrive_t *vdrive);


void vdrive_init(void)
{
    vdrive_log = log_open("VDrive");

    vdrive_command_init();
    vdrive_dir_init();
    vdrive_iec_init();
    vdrive_internal_init();
    vdrive_rel_init();
    vdrive_snapshot_init();
}

/* ------------------------------------------------------------------------- */

int vdrive_device_setup(vdrive_t *vdrive, unsigned int unit)
{
    unsigned int i;

    vdrive->unit = unit;

    for (i = 0; i < 15; i++)
        vdrive->buffers[i].mode = BUFFER_NOT_IN_USE;

    vdrive->buffers[15].mode = BUFFER_COMMAND_CHANNEL;

    if (vdrive->buffers[15].buffer == NULL)
        vdrive->buffers[15].buffer = (BYTE *)lib_malloc(256);
    memset(vdrive->buffers[15].buffer, 0, 256);

    vdrive_command_set_error(vdrive, CBMDOS_IPE_DOS_VERSION, 0, 0);
    return 0;
}

void vdrive_device_shutdown(vdrive_t *vdrive)
{
    unsigned int i;

    if (vdrive != NULL) {
        for (i = 0; i < 16; i++)
            lib_free(vdrive->buffers[i].buffer);
    }
}

/* ------------------------------------------------------------------------- */

/*
 * Close all channels. This happens on 'I' -command and on command-
 * channel close.
 */

void vdrive_close_all_channels(vdrive_t *vdrive)
{
    unsigned int i;
    bufferinfo_t *p;

    for (i = 0; i <= 15; i++) {
        p = &(vdrive->buffers[i]);
        if (p->mode != BUFFER_NOT_IN_USE && p->mode != BUFFER_COMMAND_CHANNEL)
            vdrive_iec_close(vdrive, i);
    }
}

/* ------------------------------------------------------------------------- */

int vdrive_calculate_disk_half(unsigned int type)
{
    /* Maximum distance from dir track to start/end of disk.  */
    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
      case VDRIVE_IMAGE_FORMAT_2040:
        return 17 + 5;
      case VDRIVE_IMAGE_FORMAT_1571:
        return 17 + 35;
      case VDRIVE_IMAGE_FORMAT_1581:
        return 40;
      case VDRIVE_IMAGE_FORMAT_8050:
      case VDRIVE_IMAGE_FORMAT_8250:
        return 39;
      default:
        log_error(vdrive_log,
                  "Unknown disk type %i.  Cannot calculate disk half.", type);
    }
    return -1;
}

int vdrive_get_max_sectors(unsigned int type, unsigned int track)
{
    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
        return disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track);
      case VDRIVE_IMAGE_FORMAT_2040:
        return disk_image_sector_per_track(DISK_IMAGE_TYPE_D67, track);
      case VDRIVE_IMAGE_FORMAT_1571:
        return disk_image_sector_per_track(DISK_IMAGE_TYPE_D71, track);
      case VDRIVE_IMAGE_FORMAT_8050:
        return disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track);
      case VDRIVE_IMAGE_FORMAT_1581:
        return 40;
      case VDRIVE_IMAGE_FORMAT_8250:
        if (track <= NUM_TRACKS_8250 / 2) {
            return disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track);
        } else {
            return disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track
                                               - (NUM_TRACKS_8250 / 2));
        }
      default:
        log_message(vdrive_log,
                    "Unknown disk type %i.  Cannot calculate max sectors",
                    type);

    }
    return -1;
}

/* ------------------------------------------------------------------------- */

/*
 * Functions to attach the disk image files.
 */

void vdrive_detach_image(disk_image_t *image, unsigned int unit,
                         vdrive_t *vdrive)
{
    switch(image->type) {
      case DISK_IMAGE_TYPE_D64:
        disk_image_detach_log(image, vdrive_log, unit, "D64");
        break;
      case DISK_IMAGE_TYPE_D67:
        disk_image_detach_log(image, vdrive_log, unit, "D67");
        break;
      case DISK_IMAGE_TYPE_D71:
        disk_image_detach_log(image, vdrive_log, unit, "D71");
        break;
      case DISK_IMAGE_TYPE_D81:
        disk_image_detach_log(image, vdrive_log, unit, "D81");
        break;
      case DISK_IMAGE_TYPE_D80:
        disk_image_detach_log(image, vdrive_log, unit, "D80");
        break;
      case DISK_IMAGE_TYPE_D82:
        disk_image_detach_log(image, vdrive_log, unit, "D82");
        break;
      case DISK_IMAGE_TYPE_G64:
        disk_image_detach_log(image, vdrive_log, unit, "G64");
        break;
      case DISK_IMAGE_TYPE_X64:
        disk_image_detach_log(image, vdrive_log, unit, "X64");
        break;
      default:
        return;
    }
    vdrive_close_all_channels(vdrive);
    vdrive->image = NULL;
}

int vdrive_attach_image(disk_image_t *image, unsigned int unit,
                        vdrive_t *vdrive)
{
    vdrive->unit = unit;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D64:
        disk_image_attach_log(image, vdrive_log, unit, "D64");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D67:
        disk_image_attach_log(image, vdrive_log, unit, "D67");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_2040;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D71:
        disk_image_attach_log(image, vdrive_log, unit, "D71");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1571;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D81:
        disk_image_attach_log(image, vdrive_log, unit, "D81");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1581;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D80:
        disk_image_attach_log(image, vdrive_log, unit, "D80");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_8050;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D82:
        disk_image_attach_log(image, vdrive_log, unit, "D82");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_8250;
        vdrive->num_tracks = image->tracks;
        break;
      case DISK_IMAGE_TYPE_G64:
        disk_image_attach_log(image, vdrive_log, unit, "G64");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
        vdrive->num_tracks = 35;
        break;
      case DISK_IMAGE_TYPE_X64:
        disk_image_attach_log(image, vdrive_log, unit, "X64");
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
        vdrive->num_tracks = image->tracks;
        break;
      default:
        return -1;
    }

    /* Initialise format constants */
    vdrive_set_disk_geometry(vdrive);

    vdrive->image = image;

    if (vdrive_bam_read_bam(vdrive)) {
        log_error(vdrive_log, "Cannot access BAM.");
        return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Calculate and return the total number of blocks available on a disk.
 */

int vdrive_calc_num_blocks(unsigned int format, unsigned int tracks)
{
    int blocks = -1;

    switch (format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        if (tracks > MAX_TRACKS_1541)
            tracks = MAX_TRACKS_1541;
        blocks = NUM_BLOCKS_1541 + (tracks - 35) * 17;
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        if (tracks > MAX_TRACKS_1571)
            tracks = MAX_TRACKS_1571;
        blocks = NUM_BLOCKS_1571 + (tracks - 70) * 17;
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        blocks = NUM_BLOCKS_1581;
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        blocks = NUM_BLOCKS_8050;
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        blocks = NUM_BLOCKS_8250;
        break;
      default:
        log_error(vdrive_log,
                  "Unknown disk type %i.  Cannot calculate number of blocks.",
                  format);
    }
    return blocks;
}

/* ------------------------------------------------------------------------- */

/*
 * Initialise format constants
 */

static void vdrive_set_disk_geometry(vdrive_t *vdrive)
{
    switch (vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        vdrive->Bam_Track  = BAM_TRACK_1541;
        vdrive->Bam_Sector = BAM_SECTOR_1541;
        vdrive->bam_name   = BAM_NAME_1541;
        vdrive->bam_id     = BAM_ID_1541;
        vdrive->Dir_Track  = DIR_TRACK_1541;
        vdrive->Dir_Sector = DIR_SECTOR_1541;
        break;
      case VDRIVE_IMAGE_FORMAT_2040:
        vdrive->Bam_Track  = BAM_TRACK_2040;
        vdrive->Bam_Sector = BAM_SECTOR_2040;
        vdrive->bam_name   = BAM_NAME_2040;
        vdrive->bam_id     = BAM_ID_2040;
        vdrive->Dir_Track  = DIR_TRACK_2040;
        vdrive->Dir_Sector = DIR_SECTOR_2040;
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        vdrive->Bam_Track  = BAM_TRACK_1571;
        vdrive->Bam_Sector = BAM_SECTOR_1571;
        vdrive->bam_name   = BAM_NAME_1571;
        vdrive->bam_id     = BAM_ID_1571;
        vdrive->Dir_Track  = DIR_TRACK_1571;
        vdrive->Dir_Sector = DIR_SECTOR_1571;
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        vdrive->Bam_Track  = BAM_TRACK_1581;
        vdrive->Bam_Sector = BAM_SECTOR_1581;
        vdrive->bam_name   = BAM_NAME_1581;
        vdrive->bam_id     = BAM_ID_1581;
        vdrive->Dir_Track  = DIR_TRACK_1581;
        vdrive->Dir_Sector = DIR_SECTOR_1581;
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        vdrive->Bam_Track  = BAM_TRACK_8050;
        vdrive->Bam_Sector = BAM_SECTOR_8050;
        vdrive->bam_name   = BAM_NAME_8050;
        vdrive->bam_id     = BAM_ID_8050;
        vdrive->Dir_Track  = DIR_TRACK_8050;
        vdrive->Dir_Sector = DIR_SECTOR_8050;
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        vdrive->Bam_Track  = BAM_TRACK_8250;
        vdrive->Bam_Sector = BAM_SECTOR_8250;
        vdrive->bam_name   = BAM_NAME_8250;
        vdrive->bam_id     = BAM_ID_8250;
        vdrive->Dir_Track  = DIR_TRACK_8250;
        vdrive->Dir_Sector = DIR_SECTOR_8250;
        break;
      default:
        log_error(vdrive_log,
                  "Unknown disk type %i.  Cannot set disk geometry.",
                  vdrive->image_format);
    }
}

/* ------------------------------------------------------------------------- */

static unsigned int last_read_track, last_read_sector;
static BYTE last_read_buffer[256];

void vdrive_get_last_read(unsigned int *track, unsigned int *sector,
                          BYTE **buffer)
{
    *track = last_read_track;
    *sector = last_read_sector;
    *buffer = last_read_buffer;
}

void vdrive_set_last_read(unsigned int track, unsigned int sector,
                          BYTE *buffer)
{
    last_read_track = track;
    last_read_sector = sector;
    memcpy(last_read_buffer, buffer, 256);
}

