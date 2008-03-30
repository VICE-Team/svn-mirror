/*
 * diskimage.h - Common low-level disk image access.
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

#ifndef _DISKIMAGE_H
#define _DISKIMAGE_H

#include "types.h"

#define D64_FILE_SIZE_35  174848        /* D64 image, 35 tracks */
#define D64_FILE_SIZE_35E 175531        /* D64 image, 35 tracks with errors */
#define D67_FILE_SIZE     176640        /* D67 image, 35 tracks DOS1 */
#define D71_FILE_SIZE     349696        /* D71 image, 70 tracks */
#define D81_FILE_SIZE     819200        /* D81 image, 80 tracks */
#define D80_FILE_SIZE     533248        /* D80 image, 77 tracks */
#define D82_FILE_SIZE    1066496        /* D82 image, 77 tracks */

#define DISK_IMAGE_DEVICE_FS   0
#define DISK_IMAGE_DEVICE_REAL 1
#define DISK_IMAGE_DEVICE_RAW  2

#define DISK_IMAGE_TYPE_X64 0
#define DISK_IMAGE_TYPE_G64 100
#define DISK_IMAGE_TYPE_D64 1541
#define DISK_IMAGE_TYPE_D67 2040
#define DISK_IMAGE_TYPE_D71 1571
#define DISK_IMAGE_TYPE_D81 1581
#define DISK_IMAGE_TYPE_D80 8050
#define DISK_IMAGE_TYPE_D82 8250
#define DISK_IMAGE_TYPE_TAP 1531


struct gcr_s;

struct disk_image_s {
    void *media;
    unsigned int read_only;
    unsigned int device;
    unsigned int type;
    unsigned int tracks;
    struct gcr_s *gcr;
    BYTE *error_info;
};
typedef struct disk_image_s disk_image_t;

extern void disk_image_init(void);
extern int disk_image_resources_init(void);
extern int disk_image_cmdline_options_init(void);
extern void disk_image_resources_shutdown(void);

extern void disk_image_fsimage_name_set(disk_image_t *image, char *name);
extern char *disk_image_fsimage_name_get(disk_image_t *image);
extern void *disk_image_fsimage_fd_get(disk_image_t *image);
extern int disk_image_fsimage_create(const char *name, unsigned int type);

extern void disk_image_rawimage_name_set(disk_image_t *image, char *name);
extern void disk_image_rawimage_driver_name_set(disk_image_t *image);

extern void disk_image_name_set(disk_image_t *image, char *name);
extern char *disk_image_name_get(disk_image_t *image);

extern void disk_image_media_create(disk_image_t *image);
extern void disk_image_media_destroy(disk_image_t *image);

extern int disk_image_open(disk_image_t *image);
extern int disk_image_close(disk_image_t *image);

extern int disk_image_read_sector(disk_image_t *image, BYTE *buf,
                                  unsigned int track, unsigned int sector);
extern int disk_image_write_sector(disk_image_t *image, BYTE *buf,
                                   unsigned int track, unsigned int sector);
extern int disk_image_check_sector(disk_image_t *image, unsigned int track,
                                   unsigned int sector);
extern unsigned int disk_image_sector_per_track(unsigned int format,
                                                unsigned int track);
extern int disk_image_read_gcr_image(disk_image_t *image);
extern int disk_image_read_track(disk_image_t *image, unsigned int track,
                                 BYTE *gcr_data, int *gcr_track_size);
extern int disk_image_write_track(disk_image_t *image, unsigned int track,
                                  int gcr_track_size,
                                  BYTE *gcr_speed_zone,
                                  BYTE *gcr_track_start_ptr);

extern unsigned int disk_image_speed_map_1541(unsigned int track);
extern unsigned int disk_image_speed_map_1571(unsigned int track);

extern void disk_image_attach_log(disk_image_t *image, signed int lognum,
                                  unsigned int unit, const char *type);
extern void disk_image_detach_log(disk_image_t *image, signed int lognum,
                                  unsigned int unit, const char *type);

#endif

