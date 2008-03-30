/*
 * vdrive.c - Virtual disk-drive implementation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifdef __riscos
#include "ui.h"
#endif

#include "archdep.h"
#include "charsets.h"
#include "fsdevice.h"
#include "log.h"
#include "serial.h"
#include "types.h"
#include "utils.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive-snapshot.h"
#include "vdrive.h"

/* ------------------------------------------------------------------------- */

static log_t vdrive_log = LOG_ERR;

int speed_map_1541[42] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0 };

int speed_map_1571[70] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0,
                           3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0 };

/* PC64 files need this too */
char const *slot_type[] = {
    "DEL", "SEQ", "PRG", "USR", "REL", "CBM", "DJJ", "FAB"
};

static void vdrive_set_disk_geometry(vdrive_t *vdrive);

/* ------------------------------------------------------------------------- */

void vdrive_init(void)
{
    vdrive_log = log_open("VDrive");

    vdrive_command_init();
    vdrive_dir_init();
    vdrive_iec_init();
    vdrive_snapshot_init();
}

int vdrive_setup_device(vdrive_t *vdrive, unsigned int unit)
{
    int i;

    vdrive->unit = unit;

    for (i = 0; i < 15; i++)
        vdrive->buffers[i].mode = BUFFER_NOT_IN_USE;

    vdrive->buffers[15].mode = BUFFER_COMMAND_CHANNEL;
    vdrive->buffers[15].buffer = (BYTE *)xmalloc(256);

    vdrive_command_set_error(vdrive, IPE_DOS_VERSION, 0, 0);
    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Parse name realname, type and read/write mode. '@' on write must
 * be checked elsewhere
 */

int vdrive_parse_name(const char *name, unsigned int length, char *ptr,
                      unsigned int *reallength, unsigned int *readmode,
                      unsigned int *filetype, unsigned int *rl)
{
    const char *p;
    char *c;
    int t;

    if (!name || length == 0)
        return FLOPPY_ERROR;

    p = (char *)memchr(name, ':', length);
    if (p)
        p++;
    else {      /* no colon found */
        if (*name != '$')
            p = name;
        else
            p = name + length;    /* set to null byte */
    }
#ifdef DEBUG_DRIVE
    log_debug("Name (%d): '%s'.", length, p);
#endif

#if 0
    if (*name == '@' && p == name)
        p++;
#endif

    t = length - (p - name);
    *reallength = 0;

    while (*p != ',' && t-- > 0) {
        (*reallength)++;
        *(ptr++) = *(p++);      /* realname pointer */
#ifdef DEBUG_DRIVE
        log_debug("parsing... [%d] %02x  t=%d.", *reallength, *(ptr-1), t);
#endif
    }  /* while */


    *filetype = 0;              /* EP */

    /*
     * Change modes ?
     */
    while (t > 0) {
        t--;
        p++;

        if (t == 0) {
#ifdef DEBUG_DRIVE
            log_debug("done. [%d] %02x  t=%d.", *reallength, *p, t);
            log_debug("No type.");
#endif
            return FLOPPY_ERROR;
        }

        switch (*p) {
          case 'S':
            *filetype = FT_SEQ;
            break;
          case 'P':
            *filetype = FT_PRG;
            break;
          case 'U':
            *filetype = FT_USR;
            break;
          case 'L':                     /* L,(#record length)  max 254 */
            if (rl && p[1] == ',') {
                *rl = p[2];             /* Changing RL causes error */

                if (*rl > 254)
                    return FLOPPY_ERROR;
            }
            *filetype = FT_REL;
            break;
          case 'R':
            *readmode = FAM_READ;
            break;
          case 'W':
            *readmode = FAM_WRITE;
            break;
          default:
#ifdef DEBUG_DRIVE
            log_debug("Invalid extension. p='%s'.", p);
#endif
            if (*readmode != FAM_READ && *readmode != FAM_WRITE)
                return FLOPPY_ERROR;
        }

        c = (char *)memchr(p, ',', t);

        if (c) {
            t -= (c - p);
            p = c;
        } else
            t = 0;
    }  /* while (t) */

#ifdef DEBUG_DRIVE
    log_debug("Type = %s  %s.", slot_type[*filetype],
            (*readmode == FAM_READ ?  "read" : "write"));
#endif

    return FLOPPY_COMMAND_OK;
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
            vdrive_close(vdrive, i);
    }
}

/* ------------------------------------------------------------------------- */

int vdrive_calculate_disk_half(unsigned int type)
{
    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
        return 17;
      case VDRIVE_IMAGE_FORMAT_1571:
        return 17;
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
        log_message(vdrive_log, "Unit %d: D64 disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D71:
        log_message(vdrive_log, "Unit %d: D71 disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D81:
        log_message(vdrive_log, "Unit %d: D81 disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D80:
        log_message(vdrive_log, "Unit %d: D80 disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_D82:
        log_message(vdrive_log, "Unit %d: D82 disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_GCR:
        log_message(vdrive_log, "Unit %d: GCR disk image detached: %s.",
                    unit, image->name);
        break;
      case DISK_IMAGE_TYPE_X64:
        log_message(vdrive_log, "Unit %d: X64 disk image detached: %s.",
                    unit, image->name);
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
        log_message(vdrive_log, "Unit %d: D64 disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D71:
        log_message(vdrive_log, "Unit %d: D71 disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1571;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D81:
        log_message(vdrive_log, "Unit %d: D81 disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1581;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D80:
        log_message(vdrive_log, "Unit %d: D80 disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_8050;
        vdrive->num_tracks  = image->tracks;
        break;
      case DISK_IMAGE_TYPE_D82:
        log_message(vdrive_log, "Unit %d: D82 disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_8250;
        vdrive->num_tracks = image->tracks;
        break;
      case DISK_IMAGE_TYPE_GCR:
        log_message(vdrive_log, "Unit %d: GCR disk image attached: %s.",
                    vdrive->unit, image->name);
        vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
        vdrive->num_tracks = 35;
        break;
      case DISK_IMAGE_TYPE_X64:
        log_message(vdrive_log, "Unit %d: X64 disk image attached: %s.",
                    vdrive->unit, image->name);
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

int vdrive_calc_num_blocks(int format, int tracks)
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

vdrive_t *vdrive_internal_open_disk_image(const char *name)
{
    vdrive_t *vdrive;
    disk_image_t *image;

    image = (disk_image_t *)xmalloc(sizeof(disk_image_t));
    image->name = stralloc(name);
    image->gcr = NULL;

    if (disk_image_open(image) < 0) {
        free(image->name);
        log_error(LOG_ERR, "Cannot open file `%s'", name);
        return NULL;
    }

    vdrive = (vdrive_t *)xmalloc(sizeof(vdrive_t));
    memset(vdrive, 0, sizeof(vdrive_t));

    vdrive_setup_device(vdrive, 100);
    vdrive->image = image;
    vdrive_attach_image(image, 100, vdrive);
    return vdrive;
}

void vdrive_internal_close_disk_image(vdrive_t *vdrive)
{
    disk_image_t *image;

    image = vdrive->image;

    vdrive_detach_image(image, 100, vdrive);
    disk_image_close(image);

    free(image);
    free(vdrive);
}

int vdrive_internal_format_disk_image(const char *filename,
                                      const char *disk_name)
{
    vdrive_t *vdrive;
    const char *format_name;

    format_name = (disk_name == NULL) ? " " : disk_name;

    vdrive = vdrive_internal_open_disk_image(filename);

    if (vdrive == NULL)
        return -1;

    vdrive_command_format(vdrive, format_name);
    vdrive_internal_close_disk_image(vdrive);

    return 0;
}

int vdrive_internal_create_format_disk_image(const char *filename,
                                             const char *diskname,
                                             int type)
{
    disk_image_create(filename, type);
    return vdrive_internal_format_disk_image(filename, diskname);
}

