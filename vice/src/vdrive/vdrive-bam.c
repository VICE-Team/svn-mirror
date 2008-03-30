/*
 * vdrive-bam.c - Virtual disk-drive implementation. BAM specific functions.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
 *
 * Based on old code by
 *  Teemu Rantanen      <tvr@cs.hut.fi>
 *  Jarkko Sonninen     <sonninen@lut.fi>
 *  Jouko Valta         <jopi@stekt.oulu.fi>
 *  Olaf Seibert        <rhialto@mbfys.kun.nl>
 *  André Fachat        <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli    <ettore@comm2000.it>
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

#include <string.h>

#include "attach.h"
#include "log.h"
#include "vdrive-bam.h"
#include "vdrive.h"

int vdrive_bam_alloc_first_free_sector(vdrive_t *vdrive, BYTE *bam, int *track,
                                       int *sector)
{
    int t, s, d, max_tracks;

    max_tracks = vdrive_calculate_disk_half(vdrive->image_format);

    for (d = 1; d <= max_tracks; d++) {
        int max_sector;
        t = vdrive->Bam_Track - d;
#ifdef DEBUG_DRIVE
        fprintf(logfile, "Allocate first free sector on track %d?.\n", t);
#endif
        if (t < 1)
            continue;
        max_sector = vdrive_get_max_sectors(vdrive->image_format, t);
        for (s = 0; s < max_sector; s++) {
            if (vdrive_bam_allocate_sector(vdrive->image_format, bam, t, s)) {
                *track = t;
                *sector = s;
#ifdef DEBUG_DRIVE
                fprintf(logfile, "Allocate first free sector: %d,%d.\n", t, s);
#endif
                return 0;
            }
        }
        t = vdrive->Bam_Track + d;
#ifdef DEBUG_DRIVE
        fprintf(logfile, "Allocate first free sector on track %d?.\n", t);
#endif
        if (t > vdrive->NumTracks)
            continue;
        max_sector = vdrive_get_max_sectors(vdrive->image_format, t);
        for (s = 0; s < max_sector; s++) {
            if (vdrive_bam_allocate_sector(vdrive->image_format, bam, t, s)) {
                *track = t;
                *sector = s;
#ifdef DEBUG_DRIVE
                fprintf(logfile, "Allocate first free sector: %d,%d.\n", t, s);
#endif
                return 0;
            }
        }
    }
    return -1;
}

/*
 * This algorithm is used to select a continuation sector.
 * track and sector must be given.
 * XXX the interleave is not taken into account yet.
 * FIXME: does this handle double-sided formats?
 */
int vdrive_bam_alloc_next_free_sector(vdrive_t *vdrive, BYTE *bam, int *track,
                                      int *sector)
{
    int t, s, d;
    int dir, diskhalf;

    if (*track < vdrive->Dir_Track) {
        dir = -1;
        d = vdrive->Bam_Track - *track;
    } else {
        dir = 1;
        d = *track - vdrive->Bam_Track;
    }

    for (diskhalf = 0; diskhalf < 2; diskhalf++) {
        int max_track = vdrive_calculate_disk_half(vdrive->image_format);
        for (; d <= max_track; d++) {
            int max_sector;
            t = vdrive->Bam_Track + dir * d;
#ifdef DEBUG_DRIVE
            fprintf(logfile, "Allocate next free sector on track %d?.\n", t);
#endif
            if (t < 1 || t > vdrive->NumTracks) {
                dir = -dir;
                d = 1;
                break;
            }
            max_sector = vdrive_get_max_sectors(vdrive->image_format, t);
            for (s = 0; s < max_sector; s++) {
                if (vdrive_bam_allocate_sector(vdrive->image_format, bam,
                    t, s)) {
                    *track = t;
                    *sector = s;
#ifdef DEBUG_DRIVE
                    fprintf(logfile, "Allocate next free sector: %d,%d\n", t, s);
#endif
                    return 0;
                }
            }
        }
    }
    return -1;
}

void vdrive_bam_set(int type, BYTE *bamp, int sector)
{
    bamp[1 + sector / 8] |= (1 << (sector % 8));
    return;
}

void vdrive_bam_clr(int type, BYTE *bamp, int sector)
{
    bamp[1 + sector / 8] &= ~(1 << (sector % 8));
    return;
}

int vdrive_bam_isset(int type, BYTE *bamp, int sector)
{
    return bamp[1 + sector / 8] & (1 << (sector % 8));
}

int vdrive_bam_allocate_chain(vdrive_t *vdrive, int t, int s)
{
    BYTE tmp[256];
    int disk_type = -1;

    switch (vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        disk_type = DISK_IMAGE_TYPE_D64;
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        disk_type = DISK_IMAGE_TYPE_D71;
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        disk_type = DISK_IMAGE_TYPE_D81;
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        disk_type = DISK_IMAGE_TYPE_D80;
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        disk_type = DISK_IMAGE_TYPE_D82;
        break;
    }

    while (t) {
        /* Check for illegal track or sector.  */
        if (disk_image_check_sector(disk_type, t, s) < 0) {
            vdrive_command_set_error(&vdrive->buffers[15],
                                     IPE_ILLEGAL_TRACK_OR_SECTOR, s, t);
            return IPE_ILLEGAL_TRACK_OR_SECTOR;
        }
        if (!vdrive_bam_allocate_sector(vdrive->image_format, vdrive->bam,
            t, s)) {
            /* The real drive does not seem to catch this error.  */
            vdrive_command_set_error(&vdrive->buffers[15], IPE_NO_BLOCK, s, t);
            return IPE_NO_BLOCK;
        }
        disk_image_read_sector(vdrive->image, tmp, t, s);
        t = (int)tmp[0];
        s = (int)tmp[1];
    }
    return IPE_OK;
}

BYTE *vdrive_bam_calculate_track(int type, BYTE *bam, int track)
{
    BYTE *bamp = NULL;

    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
        bamp = (track <= NUM_TRACKS_1541) ?
               &bam[BAM_BIT_MAP + 4 * (track - 1)] :
               &bam[BAM_EXT_BIT_MAP_1541 + 4 * (track - NUM_TRACKS_1541 - 1)];
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        bamp = (track <= NUM_TRACKS_1571 / 2) ?
               &bam[BAM_BIT_MAP + 4 * (track - 1)] :
               &bam[0x100 + 3 * (track - 1)];
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        bamp = (track <= BAM_TRACK_1581) ?
               &bam[0x100 + BAM_BIT_MAP_1581 + 6 * (track - 1)] :
               &bam[0x200 + BAM_BIT_MAP_1581 + 6 *
               (track - BAM_TRACK_1581 - 1)];
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        {
            int i;
            for (i = 1; i < 3; i++) {
                if (track >= bam[(i * 0x100) + 4]
                        && track < bam[(i * 0x100) + 5]) {
                    bamp = &bam[(i * 0x100) + BAM_BIT_MAP_8050
                                + 5 * (track - bam[(i * 0x100) + 4]) ];
                    break;
                }
            }
        }
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        {
            int i;
            for (i = 1; i < 5; i++) {
                if (track >= bam[(i * 0x100) + 4]
                        && track < bam[(i * 0x100) + 5]) {
                    bamp = &bam[(i * 0x100) + BAM_BIT_MAP_8050
                                + 5 * (track - bam[(i * 0x100) + 4]) ];
                    break;
                }
            }
        }
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot calculate BAM track.", type);
    }
    return bamp;
}

static void vdrive_bam_sector_free(int type, BYTE *bamp, BYTE *bam, int track,
                                   int add){
    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
      case VDRIVE_IMAGE_FORMAT_1581:
      case VDRIVE_IMAGE_FORMAT_8050:
      case VDRIVE_IMAGE_FORMAT_8250:
        *bamp += add;
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        if (track <= NUM_TRACKS_1571 / 2)
            *bamp += add;
        else
            bam[BAM_EXT_BIT_MAP_1571 + track - NUM_TRACKS_1571 / 2 - 1] += add;
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot find free sector.", type);
    }
}

int vdrive_bam_allocate_sector(int type, BYTE *bam, int track, int sector)
{
    BYTE *bamp;

    bamp = vdrive_bam_calculate_track(type, bam, track);
    if (vdrive_bam_isset(type, bamp, sector)) {
        vdrive_bam_sector_free(type, bamp, bam, track, -1);
        vdrive_bam_clr(type, bamp, sector);
        return 1;
    }
    return 0;
}

int vdrive_bam_free_sector(int type, BYTE *bam, int track, int sector)
{
    BYTE *bamp;

    bamp = vdrive_bam_calculate_track(type, bam, track);
    if (!(vdrive_bam_isset(type, bamp, sector))) {
        vdrive_bam_set(type, bamp, sector);
        vdrive_bam_sector_free(type, bamp, bam, track, 1);
        return 1;
    }
    return 0;
}

void vdrive_bam_clear_all(int type, BYTE *bam)
{
    switch (type) {
      case VDRIVE_IMAGE_FORMAT_1541:
        memset(bam + BAM_BIT_MAP, 0, 4 * NUM_TRACKS_1541);
        memset(bam + BAM_EXT_BIT_MAP_1541, 0, 4 * 5);
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        memset(bam + BAM_BIT_MAP, 0, 4 * NUM_TRACKS_1571 / 2);
        memset(bam + BAM_EXT_BIT_MAP_1571, 0, NUM_TRACKS_1571 / 2);
        memset(bam + 0x100, 0, 3 * NUM_TRACKS_1571 / 2);
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        memset(bam + 0x100 + BAM_BIT_MAP_1581, 0, 6 * NUM_TRACKS_1581 / 2);
        memset(bam + 0x200 + BAM_BIT_MAP_1581, 0, 6 * NUM_TRACKS_1581 / 2);
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        memset(bam + 0x100 + BAM_BIT_MAP_8050, 0, 0x100 - BAM_BIT_MAP_8050);
        memset(bam + 0x200 + BAM_BIT_MAP_8050, 0, 0x100 - BAM_BIT_MAP_8050);
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        memset(bam + 0x100 + BAM_BIT_MAP_8250, 0, 0x100 - BAM_BIT_MAP_8250);
        memset(bam + 0x200 + BAM_BIT_MAP_8250, 0, 0x100 - BAM_BIT_MAP_8250);
        memset(bam + 0x300 + BAM_BIT_MAP_8250, 0, 0x100 - BAM_BIT_MAP_8250);
        memset(bam + 0x400 + BAM_BIT_MAP_8250, 0, 0x100 - BAM_BIT_MAP_8250);
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot clear BAM.", type);
    }
}

/* Should be removed some day.  */
static int mystrncpy(BYTE *d, BYTE *s, int n)
{
    while (n-- && *s)
        *d++ = *s++;
    return (n);
}

void vdrive_bam_create_empty_bam(vdrive_t *vdrive, const char *name, BYTE *id)
{
    /* Create Disk Format for 1541/1571/1581 disks.  */
    /* FIXME: use a define for the length? */
    memset(vdrive->bam, 0, 5 * 256);
    if (vdrive->image_format != VDRIVE_IMAGE_FORMAT_8050
        && vdrive->image_format != VDRIVE_IMAGE_FORMAT_8250) {
        vdrive->bam[0] = vdrive->Dir_Track;
        vdrive->bam[1] = vdrive->Dir_Sector;
        vdrive->bam[2] = (vdrive->image_format == VDRIVE_IMAGE_FORMAT_1581)
                         ? 68 : 65;
        if (vdrive->image_format == VDRIVE_IMAGE_FORMAT_1571)
            vdrive->bam[3] = 0x80;

        memset(vdrive->bam + vdrive->bam_name, 0xa0,
               (vdrive->image_format == VDRIVE_IMAGE_FORMAT_1581) ? 25 : 27);
        mystrncpy(vdrive->bam + vdrive->bam_name, (BYTE *)name + 1, 16);
        mystrncpy(vdrive->bam + vdrive->bam_id, id, 2);
    }

    switch (vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
      case VDRIVE_IMAGE_FORMAT_1571:
        vdrive->bam[BAM_VERSION_1541] = 50;
        vdrive->bam[BAM_VERSION_1541 + 1] = 65;
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        /* Setup BAM linker.  */
        vdrive->bam[0x100] = vdrive->Bam_Track;
        vdrive->bam[0x100 + 1] = 2;
        vdrive->bam[0x200] = 0;
        vdrive->bam[0x200 + 1] = 0xff;
        /* Setup BAM version.  */
        vdrive->bam[BAM_VERSION_1581] = 51;
        vdrive->bam[BAM_VERSION_1581 + 1] = 68;
        vdrive->bam[0x100 + 2] = 68;
        vdrive->bam[0x100 + 3] = 0xbb;
        vdrive->bam[0x100 + 4] = id[0];
        vdrive->bam[0x100 + 5] = id[1];
        vdrive->bam[0x100 + 6] = 0xc0;
        vdrive->bam[0x200 + 2] = 68;
        vdrive->bam[0x200 + 3] = 0xbb;
        vdrive->bam[0x200 + 4] = id[0];
        vdrive->bam[0x200 + 5] = id[1];
        vdrive->bam[0x200 + 6] = 0xc0;
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
      case VDRIVE_IMAGE_FORMAT_8250:
        /* the first BAM block with the disk name is at 39/0, but it
           points to the first bitmap BAM block at 38/0 ...
           Only the last BAM block at 38/3 resp. 38/9 points to the
           first dir block at 39/1 */
        vdrive->bam[0] = 38;
        vdrive->bam[1] = 0;
        vdrive->bam[2] = 67;
        /* byte 3-5 unused */
        /* bytes 6- disk name + id + version */
        memset(vdrive->bam + vdrive->bam_name, 0xa0, 27);
        mystrncpy(vdrive->bam + vdrive->bam_name, (BYTE *)name + 1, 16);
        mystrncpy(vdrive->bam + vdrive->bam_id, id, 2);
        vdrive->bam[BAM_VERSION_8050] = 50;
        vdrive->bam[BAM_VERSION_8050 + 1] = 67;
        /* rest of first block unused */

        /* first bitmap block at 38/0 */
        vdrive->bam[0x100]     = 38;
        vdrive->bam[0x100 + 1] = 3;
        vdrive->bam[0x100 + 2] = 67;
        vdrive->bam[0x100 + 4] = 1; /* In this block from track ... */
        vdrive->bam[0x100 + 5] = 51;    /* till excluding track ... */

        if (vdrive->image_format == VDRIVE_IMAGE_FORMAT_8050) {
            /* second bitmap block at 38/3 */
            vdrive->bam[0x200]     = 39;
            vdrive->bam[0x200 + 1] = 1;
            vdrive->bam[0x200 + 2] = 67;
            vdrive->bam[0x200 + 4] = 51;    /* In this block from track ... */
            vdrive->bam[0x200 + 5] = 79;    /* till excluding track ... */
        } else
        if (vdrive->image_format == VDRIVE_IMAGE_FORMAT_8250) {
            /* second bitmap block at 38/3 */
            vdrive->bam[0x200]     = 38;
            vdrive->bam[0x200 + 1] = 6;
            vdrive->bam[0x200 + 2] = 67;
            vdrive->bam[0x200 + 4] = 51;    /* In this block from track ... */
            vdrive->bam[0x200 + 5] = 101;   /* till excluding track ... */
            /* third bitmap block at 38/6 */
            vdrive->bam[0x300]     = 38;
            vdrive->bam[0x300 + 1] = 9;
            vdrive->bam[0x300 + 2] = 67;
            vdrive->bam[0x300 + 4] = 101;   /* In this block from track ... */
            vdrive->bam[0x300 + 5] = 151;   /* till excluding track ... */
            /* fourth bitmap block at 38/9 */
            vdrive->bam[0x400]     = 39;
            vdrive->bam[0x400 + 1] = 1;
            vdrive->bam[0x400 + 2] = 67;
            vdrive->bam[0x400 + 4] = 151;   /* In this block from track ... */
            vdrive->bam[0x400 + 5] = 155;   /* till excluding track ... */
        }
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot create BAM.",
                  vdrive->image_format);
    }
    return;
}

int vdrive_bam_get_disk_id(int unit, BYTE *id)
{
    vdrive_t *vdrive;

    vdrive = file_system_get_vdrive(unit);

    if (vdrive == NULL || id == NULL)
        return -1;

    memcpy(id, vdrive->bam + vdrive->bam_id, 2);

    return 0;
}

int vdrive_bam_set_disk_id(int unit, BYTE *id)
{
    vdrive_t *vdrive;

    vdrive = file_system_get_vdrive(unit);

    if (vdrive == NULL || id == NULL)
        return -1;

    memcpy(vdrive->bam + vdrive->bam_id, id, 2);

    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Load/Store BAM Image.
 */

/* probably we should make a list with BAM blocks for each drive type... (AF)*/
int vdrive_bam_read_bam(vdrive_t *vdrive)
{
    int err = -1;

    switch(vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        err = disk_image_read_sector(vdrive->image, vdrive->bam,
                                     BAM_TRACK_1541, BAM_SECTOR_1541);
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        err = disk_image_read_sector(vdrive->image, vdrive->bam,
                                BAM_TRACK_1571, BAM_SECTOR_1571);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam+256,
                                 BAM_TRACK_1571+35, BAM_SECTOR_1571);
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        err = disk_image_read_sector(vdrive->image, vdrive->bam,
                                BAM_TRACK_1581, BAM_SECTOR_1581);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 256,
                                 BAM_TRACK_1581, BAM_SECTOR_1581 + 1);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 512,
                                 BAM_TRACK_1581, BAM_SECTOR_1581 + 2);
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
      case VDRIVE_IMAGE_FORMAT_8250:
        err = disk_image_read_sector(vdrive->image, vdrive->bam,
                                 BAM_TRACK_8050, BAM_SECTOR_8050);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 256,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 512,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 3);

        if (vdrive->image_format == VDRIVE_IMAGE_FORMAT_8050)
            break;

        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 768,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 6);
        err |= disk_image_read_sector(vdrive->image, vdrive->bam + 1024,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 9);
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot read BAM.",
                  vdrive->image_format);
    }
    return err;
}

/* Temporary hack.  */
int vdrive_bam_reread_bam(int unit)
{
    return vdrive_bam_read_bam((vdrive_t *)file_system_get_vdrive(unit));
}

int vdrive_bam_write_bam(vdrive_t *vdrive)
{
    int err = -1;

    switch(vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        err = disk_image_write_sector(vdrive->image, vdrive->bam,
                                 BAM_TRACK_1541, BAM_SECTOR_1541);
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        err = disk_image_write_sector(vdrive->image, vdrive->bam,
                                 BAM_TRACK_1571, BAM_SECTOR_1571);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 256,
                                  BAM_TRACK_1571 + 35, BAM_SECTOR_1571);
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        err = disk_image_write_sector(vdrive->image, vdrive->bam,
                                 BAM_TRACK_1581, BAM_SECTOR_1581);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 256, 
                                  BAM_TRACK_1581, BAM_SECTOR_1581 + 1);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 512,
                                  BAM_TRACK_1581, BAM_SECTOR_1581 + 2);
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
      case VDRIVE_IMAGE_FORMAT_8250:
        err = disk_image_write_sector(vdrive->image, vdrive->bam,
                                 BAM_TRACK_8050, BAM_SECTOR_8050);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 256,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 512,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 3);

        if (vdrive->image_format == 8050)
            break;

        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 768,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 6);
        err |= disk_image_write_sector(vdrive->image, vdrive->bam + 1024,
                                  BAM_TRACK_8050 - 1, BAM_SECTOR_8050 + 9);
        break;
      default:
        log_error(LOG_ERR,
                  "Unknown disk type %i.  Cannot read BAM.",
                  vdrive->image_format);
    }
    return err;
}

/* ------------------------------------------------------------------------- */

/*
 * Return the number of free blocks on disk.
 */

int vdrive_bam_free_block_count(vdrive_t *vdrive)
{
    int blocks, i;

    for (blocks = 0, i = 1; i <= vdrive->NumTracks; i++) {
        switch(vdrive->image_format) {
          case VDRIVE_IMAGE_FORMAT_1541:
            if (i != vdrive->Dir_Track)
                blocks += (i <= NUM_TRACKS_1541) ?
                    vdrive->bam[BAM_BIT_MAP + 4 * (i - 1)] :
                    vdrive->bam[BAM_EXT_BIT_MAP_1541 + 4 *
                                (i - NUM_TRACKS_1541 - 1)];
            break;
          case VDRIVE_IMAGE_FORMAT_1571:
            if (i != vdrive->Dir_Track && i != vdrive->Dir_Track + 35)
                blocks += (i <= NUM_TRACKS_1571 / 2) ?
                    vdrive->bam[BAM_BIT_MAP + 4 * (i - 1)] :
                    vdrive->bam[BAM_EXT_BIT_MAP_1571 + i - 1
                                - NUM_TRACKS_1571 / 2];
            break;
          case VDRIVE_IMAGE_FORMAT_1581:
            if (i != vdrive->Dir_Track)
                blocks += (i <= NUM_TRACKS_1581 / 2) ?
                    vdrive->bam[BAM_BIT_MAP_1581 + 256 + 6 * (i - 1)] :
                    vdrive->bam[BAM_BIT_MAP_1581 + 512 + 6 * (i - 1
                    - NUM_TRACKS_1581 / 2)];
            break;
          case VDRIVE_IMAGE_FORMAT_8050:
            if (i != vdrive->Dir_Track) {
                int j;
                for (j = 1; j < 3; j++) {
                    if (i >= vdrive->bam[(j * 0x100) + 4]
                            && i < vdrive->bam[(j * 0x100) + 5]) {
                        blocks += vdrive->bam[(j * 0x100) + BAM_BIT_MAP_8050
                                + 5 * (i - vdrive->bam[(j * 0x100) + 4]) ];
                        break;
                    }
                }
            }
            break;
          case VDRIVE_IMAGE_FORMAT_8250:
            if (i != vdrive->Dir_Track) {
                int j;
                for (j = 1; j < 5; j++) {
                    if (i >= vdrive->bam[(j * 0x100) + 4]
                            && i < vdrive->bam[(j * 0x100) + 5]) {
                        blocks += vdrive->bam[(j * 0x100) + BAM_BIT_MAP_8050
                                + 5 * (i - vdrive->bam[(j * 0x100) + 4]) ];
                        break;
                    }
                }
            }
            break;
          default:
            log_error(LOG_ERR,
                      "Unknown disk type %i.  Cannot calulate free sectors.",
                      vdrive->image_format);
        }
    }
    return blocks;
}

