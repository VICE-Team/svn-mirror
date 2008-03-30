/*
 * rawimage.c
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

#include "blockdev.h"
#include "diskimage.h"
#include "log.h"
#include "rawimage.h"
#include "types.h"
#include "utils.h"


static log_t rawimage_log = LOG_ERR;


void rawimage_media_create(disk_image_t *image)
{
    rawimage_t *rawimage;

    rawimage = (rawimage_t *)xmalloc(sizeof(rawimage_t));

    rawimage->name = stralloc("DUMMY");

    image->media = (void *)rawimage;
}

void rawimage_media_destroy(disk_image_t *image)
{
    rawimage_t *rawimage;

    rawimage = (rawimage_t *)(image->media);

    free(rawimage->name);

    free(rawimage);
}

/*-----------------------------------------------------------------------*/

int rawimage_open(disk_image_t *image)
{
    image->type = DISK_IMAGE_TYPE_D81;
    image->tracks = 80;

    blockdev_open(image);

    return 0;
}

int rawimage_close(disk_image_t *image)
{
    blockdev_close(image);

    return 0;
}

/*-----------------------------------------------------------------------*/

int rawimage_read_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                         unsigned int sector)
{
    return blockdev_read_sector(image, buf, track, sector);
}

int rawimage_write_sector(disk_image_t *image, BYTE *buf, unsigned int track,
                          unsigned int sector)
{
    return 0;
}

/*-----------------------------------------------------------------------*/

void rawimage_init(void)
{
    rawimage_log = log_open("Raw Image");

    blockdev_init();
}

int rawimage_resources_init()
{
    return blockdev_resources_init();
}

int rawimage_cmdline_options_init()
{
    return blockdev_cmdline_options_init();
}

