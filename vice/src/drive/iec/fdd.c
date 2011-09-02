/*
 * fdd.c - (M)FM floppy disk drive emulation
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "diskimage.h"
#include "lib.h"
#include "types.h"
#include "fdd.h"
#include "diskimage.h"

struct fdd_sector_header_s {
    BYTE track, head, sector, bytes;
    BYTE itrack, isector;
    BYTE rate;
};

fd_drive_t *fdd_init(int num) {
    fd_drive_t *drv = lib_malloc(sizeof(fd_drive_t));
    drv->image = NULL;
    drv->headers = NULL;
    drv->number = num;
    drv->motor = 0;
    drv->track = 0;
    drv->track0 = 1;
    drv->index = 0;
    drv->sector = 1;
    drv->onheader = 0;
    drv->tracks = 80;
    drv->sectors = 10;
    drv->sector_size = 2;
    drv->head_invert = 1;
    drv->disk_change = 1;
    drv->write_protect = 1;
    drv->rate = 2;
    drv->image_sectors = 40;
    return drv;
}

void fdd_shutdown(fd_drive_t *drv)
{
    if (!drv) {
	return;
    }
    lib_free(drv);
}

void fdd_image_attach(fd_drive_t *drv, struct disk_image_s *image)
{
    int t, h, s, i;
    if (!drv) {
        return;
    }
    drv->image = image;
    switch (image->type) {
    case DISK_IMAGE_TYPE_D1M:
        drv->tracks = 81;
        drv->sectors = 5;
        drv->sector_size = 3;
        drv->head_invert = 1;
        drv->disk_rate = 2;
        drv->image_sectors = 256;
        break;
    case DISK_IMAGE_TYPE_D2M:
        drv->tracks = 81;
        drv->sectors = 10;
        drv->sector_size = 3;
        drv->head_invert = 1;
        drv->disk_rate = 0;
        drv->image_sectors = 256;
        break;
    case DISK_IMAGE_TYPE_D4M:
        drv->tracks = 81;
        drv->sectors = 20;
        drv->sector_size = 3;
        drv->head_invert = 1;
        drv->disk_rate = 3;
        drv->image_sectors = 256;
        break;
    case DISK_IMAGE_TYPE_D81:
    default:
        drv->tracks = 80;
        drv->sectors = 10;
        drv->sector_size = 2;
        drv->head_invert = 1;
        drv->disk_rate = 2;
        drv->image_sectors = 40;
        break;
    }
    drv->disk_change = 1;
    drv->write_protect = image->read_only;
    drv->headers = lib_malloc(drv->tracks * 2 * drv->sectors * sizeof(fdd_sector_header_t));
    for (t = 0; t < drv->tracks; t++)
        for (h = 0; h < 2; h++)
            for (s = 1; s <= drv->sectors; s++) {
                fdd_sector_header_t *header = &drv->headers[(t * 2 + h) * drv->sectors + s - 1];
                header->track = t;
                header->head = h ^ drv->head_invert;
                header->sector = s;
                header->bytes = drv->sector_size;
                i = (t * 2 + (h ^ drv->head_invert)) * drv->sectors + s - 1;
                i <<= drv->sector_size - 1;
                header->itrack = i / drv->image_sectors + 1;
                header->isector = i % drv->image_sectors;
                header->rate = drv->disk_rate;
            }
}

void fdd_image_detach(fd_drive_t *drv)
{
    if (!drv) {
	return;
    }
    drv->image = NULL;
    lib_free(drv->headers);
    drv->headers = NULL;
    drv->disk_change = 1;
}

static void fdd_rotate(fd_drive_t *drv)
{
    if (!drv) {
	return;
    }
    drv->onheader ^= 1;
    if (drv->onheader) {
	drv->sector = (drv->sector % drv->sectors) + 1;
    }
    drv->index = (drv->sector == 1) ? 1 : 0;
}

int fdd_image_read(fd_drive_t *drv, BYTE *buffer)
{
    fdd_sector_header_t *header;
    int i, t, s, res;

    if (!drv) {
	return -1;
    }

    if (drv->onheader) {
	fdd_rotate(drv);
    }

    if (drv->track >= drv->tracks || !drv->image) {
	fdd_rotate(drv);
	return -1;
    }
    header = &drv->headers[(drv->track * 2 + drv->head) * drv->sectors + drv->sector - 1];
    if (header->rate != drv->rate) {
	fdd_rotate(drv);
	return -1;
    }
    t = header -> itrack;
    s = header -> isector;

    for (i = 0; i < (1 << (drv->sector_size - 1)); i++) {
	res = disk_image_read_sector(drv->image, buffer, t, s);
	if (res < 0) {
	    fdd_rotate(drv);
	    return -1;
	}
	buffer += 256;
	s = (s + 1) % drv->image_sectors;
	if (!s) {
	    t++;
	}
    }
    fdd_rotate(drv);
    return 0;
}

int fdd_image_write(fd_drive_t *drv, BYTE *buffer)
{
    fdd_sector_header_t *header;
    int i, t, s, res;

    if (!drv) {
	return -1;
    }
    if (drv->onheader) {
	fdd_rotate(drv);
    }

    if (drv->track >= drv->tracks || drv->write_protect || !drv->image) {
	fdd_rotate(drv);
	return -1;
    }
    header = &drv->headers[(drv->track * 2 + drv->head) * drv->sectors + drv->sector - 1];
    if (header->rate != drv->rate) {
	fdd_rotate(drv);
	return -1;
    }
    t = header -> itrack;
    s = header -> isector;

    for (i = 0; i < (1 << (drv->sector_size - 1)); i++) {
	res = disk_image_write_sector(drv->image, buffer, t, s);
	if (res < 0) {
	    fdd_rotate(drv);
	    return -1;
	}
	buffer += 256;
	s = (s + 1) % drv->image_sectors;
	if (!s) {
	    t++;
	}
    }
    fdd_rotate(drv);
    return 0;
}

void fdd_seek_pulse(fd_drive_t *drv, int dir) {
    if (!drv) {
	return;
    }
    if (drv->motor) {
        drv->track += dir ? 1 : -1;
    }
    if (drv->image) {
        drv->disk_change = 0;
    }
    if (drv->track < 0) drv->track = 0;
    if (drv->track > drv->tracks) drv->track = drv->tracks; /* one extra track */
    drv->track0 = drv->track ? 0 : 1;
}

void fdd_select_head(fd_drive_t *drv, int head)
{
    if (!drv) {
	return;
    }
    drv->head = head & 1;
}

void fdd_set_motor(fd_drive_t *drv, int motor)
{
    if (!drv) {
	return;
    }
    drv->motor = motor & 1;
}

void fdd_set_rate(fd_drive_t *drv, int rate)
{
    if (!drv) {
	return;
    }
    drv->rate = rate & 3;
}

int fdd_image_read_header(fd_drive_t *drv, BYTE *track, BYTE *head, BYTE *sector, BYTE *bytes)
{
    fdd_sector_header_t *header;

    if (!drv) {
	return -1;
    }
    if (!drv->onheader) {
	fdd_rotate(drv);
    }

    if (drv->track >= drv->tracks || !drv->image) {
	fdd_rotate(drv);
        return -1;
    }

    header = &drv->headers[(drv->track * 2 + drv->head) * drv->sectors + drv->sector - 1];
    if (header->rate != drv->rate) {
	fdd_rotate(drv);
	return -1;
    }
    *track = header->track;
    *head = header->head;
    *sector = header->sector;
    *bytes = header->bytes;
    fdd_rotate(drv);
    return 0;
}
