/*
 * diskimage.h - Common low-level disk image access.
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

#ifndef _DISKIMAGE_H
#define _DISKIMAGE_H

#include <stdio.h>

#include <types.h>

#define D64_FILE_SIZE_35  174848        /* D64 image, 35 tracks */
#define D64_FILE_SIZE_35E 175531        /* D64 image, 35 tracks with errors */
#define D64_FILE_SIZE_40  196608        /* D64 image, 40 tracks */
#define D64_FILE_SIZE_40E 197376        /* D64 image, 40 tracks with errors */
#define D71_FILE_SIZE     349696        /* D71 image, 70 tracks */
#define D81_FILE_SIZE     819200        /* D81 image, 80 tracks */
#define D80_FILE_SIZE     533248        /* D80 image, 77 tracks */
#define D82_FILE_SIZE    1066496        /* D82 image, 77 tracks */

#define DISK_IMAGE_TYPE_X64 0
#define DISK_IMAGE_TYPE_GCR 100
#define DISK_IMAGE_TYPE_D64 1541
#define DISK_IMAGE_TYPE_D71 1571
#define DISK_IMAGE_TYPE_D81 1581
#define DISK_IMAGE_TYPE_D80 8050
#define DISK_IMAGE_TYPE_D82 8250

typedef struct {
    FILE *fd;
    char *name;
    int read_only;
    int type;
    int tracks;
} disk_image_t;

extern void disk_image_init(void);

extern int disk_image_open(disk_image_t *image);
extern int disk_image_close(disk_image_t *image);
extern int disk_image_read_sector(disk_image_t *image, BYTE *buf, int track,
                                  int sector);
extern int disk_image_write_sector(disk_image_t *image, BYTE *buf, int track,
                                   int sector);
extern int disk_image_check_sector(int format, int track, int sector);
extern int disk_image_sector_per_track(int format, int track);

#endif

