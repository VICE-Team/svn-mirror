/*
 * vdrive-internal.c - Virtual disk-drive implementation.
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

#include "attach.h"
#include "diskimage.h"
#include "log.h"
#include "types.h"
#include "utils.h"
#include "vdrive-command.h"
#include "vdrive.h"


static log_t vdrive_internal_log = LOG_DEFAULT;


static vdrive_t *open_fsimage(const char *name, unsigned int read_only)
{
    vdrive_t *vdrive;
    disk_image_t *image;

    image = (disk_image_t *)xmalloc(sizeof(disk_image_t));

    image->gcr = NULL;
    image->read_only = read_only;

    image->device = DISK_IMAGE_DEVICE_FS;

    disk_image_media_create(image);

    disk_image_fsimage_name_set(image, stralloc(name));

    if (disk_image_open(image) < 0) {
        disk_image_media_destroy(image);
        free(image);
        log_error(vdrive_internal_log, "Cannot open file `%s'", name);
        return NULL;
    }

    vdrive = (vdrive_t *)xcalloc(1, sizeof(vdrive_t));

    vdrive_setup_device(vdrive, 100);
    vdrive->image = image;
    vdrive_attach_image(image, 100, vdrive);
    return vdrive;
}

static vdrive_t *open_rawimage(unsigned int unit, unsigned int read_only)
{
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(unit);

    vdrive->image->read_only = read_only;

    return vdrive;
}

vdrive_t *vdrive_internal_open_disk_image(const char *name,
                                          unsigned int unit,
                                          unsigned int read_only)
{
    switch (unit) {
      case 0:
        return open_fsimage(name, read_only);
      case 8:
      case 9:
      case 10:
      case 11:
        return open_rawimage(unit, read_only);
        break;
    }

    return NULL;
}

static int close_fsimage(vdrive_t *vdrive)
{
    disk_image_t *image;

    image = vdrive->image;

    vdrive_detach_image(image, 100, vdrive);

    if (disk_image_close(image) < 0)
        return -1;

    disk_image_media_destroy(image);
    free(image);
    free(vdrive);

    return 0;
}

static int close_rawimage(vdrive_t *vdrive)
{
    return 0;
}

int vdrive_internal_close_disk_image(vdrive_t *vdrive)
{
    switch (vdrive->image->device) {
      case DISK_IMAGE_DEVICE_FS:
        return close_fsimage(vdrive);
      case DISK_IMAGE_DEVICE_RAW:
        return close_rawimage(vdrive);
    }

    return 0;
}

int vdrive_internal_format_disk_image(const char *filename,
                                      const char *disk_name)
{
    vdrive_t *vdrive;
    const char *format_name;
    int status = 0;

    format_name = (disk_name == NULL) ? " " : disk_name;

    /* FIXME: Pass unit here.  */
    vdrive = vdrive_internal_open_disk_image(filename, 0, 0);

    if (vdrive == NULL)
        return -1;

    if (vdrive_command_format(vdrive, format_name) != IPE_OK)
        status = -1;

    if (vdrive_internal_close_disk_image(vdrive) < 0)
        return -1;

    return status;
}

int vdrive_internal_create_format_disk_image(const char *filename,
                                             const char *diskname,
                                             unsigned int type)
{
    if (disk_image_create(filename, type) < 0)
        return -1;
    if (vdrive_internal_format_disk_image(filename, diskname) < 0)
        return -1;

    return 0;
}

void vdrive_internal_init(void)
{
    vdrive_internal_log = log_open("VDrive Internal");
}

