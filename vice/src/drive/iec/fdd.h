/*
 * fdd.h - (M)FM floppy disk drive emulation
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

#ifndef VICE_FDD_H
#define VICE_FDD_H

#include "types.h"

struct disk_image_s;

typedef struct fdd_sector_header_s fdd_sector_header_t;
typedef struct fd_drive_s {
    int number;
    int disk_change; /* out signal */
    int write_protect; /* out signal */
    int track0; /* out signal */
    int index; /* out signal */
    int track;
    int tracks;
    int head;
    int sector;
    int onheader;
    int sectors;
    int motor; /* in signal */
    int rate; /* in signal */
    int sector_size;
    int head_invert;
    int disk_rate;
    int image_sectors;
    struct disk_image_s *image;
    fdd_sector_header_t *headers;
} fd_drive_t;

extern fd_drive_t *fdd_init(int num);
extern void fdd_shutdown(fd_drive_t *drv);

extern void fdd_image_attach(fd_drive_t *drv, struct disk_image_s *image);
extern void fdd_image_detach(fd_drive_t *drv);
extern int fdd_image_read(fd_drive_t *drv, BYTE *buffer);
extern int fdd_image_write(fd_drive_t *drv, BYTE *buffer);
extern int fdd_image_read_header(fd_drive_t *drv, BYTE *track, BYTE *head, BYTE *sector, BYTE *bytes);
extern void fdd_seek_pulse(fd_drive_t *drv, int dir);
extern void fdd_select_head(fd_drive_t *drv, int head);
extern void fdd_set_motor(fd_drive_t *drv, int motor);
extern void fdd_set_rate(fd_drive_t *drv, int rate);

#endif 

