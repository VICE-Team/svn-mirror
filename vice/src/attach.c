/*
 * attach.c - File system attach management.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdlib.h>
#include <string.h>

#include "attach.h"
#include "diskimage.h"
#include "drive.h"
#include "fdc.h"
#include "fsdevice.h"
#include "fliplist.h"
#include "resources.h"
#include "serial.h"
#include "snapshot.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vdrive.h"
#include "vdrive-iec.h"
#include "wd1770.h"

typedef struct {
    serial_t *serial;
    vdrive_t *vdrive;
} file_system_t;

static file_system_t file_system[4];

static log_t attach_log = LOG_ERR;

static int file_system_device_enabled[4];

static int set_file_system_device8(resource_value_t v);
static int set_file_system_device9(resource_value_t v);
static int set_file_system_device10(resource_value_t v);
static int set_file_system_device11(resource_value_t v);

static resource_t resources[] = {
    { "FileSystemDevice8", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &file_system_device_enabled[0],
      set_file_system_device8 },
    { "FileSystemDevice9", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &file_system_device_enabled[1],
      set_file_system_device9 },
    { "FileSystemDevice10", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &file_system_device_enabled[2],
      set_file_system_device10 },
    { "FileSystemDevice11", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &file_system_device_enabled[3],
      set_file_system_device11 },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static int file_system_set_serial_hooks(unsigned int unit, int fs)
{
    if (!fs) {
        if (vdrive_iec_attach(unit, "CBM Disk Drive")) {
            log_error(attach_log,
                      "Could not initialize vdrive emulation for device #%i",
                      unit);
            return -1;
        }
    } else {
        if (fsdevice_attach(unit, "FS Drive")) {
            log_error(attach_log,
                      "Could not initialize FS drive for device #%i",
                      unit);
            return -1;
        }
    }
    return 0;
}

int file_system_init_resources(void)
{
    return resources_register(resources);
}

void file_system_init(void)
{
    int i;

    if (attach_log == LOG_ERR)
        attach_log = log_open("Attach");

    for (i = 0; i < 4; i++) {
        file_system[i].serial = serial_get_device(i + 8);;
        file_system[i].vdrive = (vdrive_t *)xmalloc(sizeof(vdrive_t));
        file_system[i].vdrive->image = NULL;
        file_system[i].vdrive->side_sector = NULL;
        vdrive_setup_device(file_system[i].vdrive, i + 8);
        file_system_set_serial_hooks(i + 8, file_system_device_enabled[i]);
    }
}


void *file_system_get_vdrive(unsigned int unit)
{
    if (unit < 8 || unit > 11) {
        log_error(attach_log, "Wrong unit for vdrive");
        return NULL;
    }
    return (void *)(file_system[unit - 8].vdrive);
}

char *file_system_get_disk_name(unsigned int unit)
{
    vdrive_t *vdrive;
    vdrive = file_system_get_vdrive(unit);

    if (vdrive == NULL)
        return NULL;
    if (vdrive->image == NULL)
        return NULL;

    return vdrive->image->name;
}

static int set_file_system_device8(resource_value_t v)
{
    vdrive_t *vdrive;

    file_system_device_enabled[0] = (int) v;

    vdrive = (vdrive_t *)file_system_get_vdrive(8);
    if (vdrive != NULL) {
        if (vdrive->image == NULL) {
            vdrive_setup_device(vdrive, 8);
            file_system_set_serial_hooks(8, file_system_device_enabled[0]);
        }
    }
    return 0;
}

static int set_file_system_device9(resource_value_t v)
{
    vdrive_t *vdrive;

    file_system_device_enabled[1] = (int) v;

    vdrive = (vdrive_t *)file_system_get_vdrive(9);
    if (vdrive != NULL) {
        if (vdrive->image == NULL) {
            vdrive_setup_device(vdrive, 9);
            file_system_set_serial_hooks(9, file_system_device_enabled[1]);
         }
    }
    return 0;
}

static int set_file_system_device10(resource_value_t v)
{
    vdrive_t *vdrive;

    file_system_device_enabled[2] = (int) v;

    vdrive = (vdrive_t *)file_system_get_vdrive(10);
    if (vdrive != NULL) {
        if (vdrive->image == NULL) {
            vdrive_setup_device(vdrive, 10);
            file_system_set_serial_hooks(10, file_system_device_enabled[2]);
        }
    }
    return 0;
}

static int set_file_system_device11(resource_value_t v)
{
    vdrive_t *vdrive;

    file_system_device_enabled[3] = (int) v;

    vdrive = (vdrive_t *)file_system_get_vdrive(11);
    if (vdrive != NULL) {
        if (vdrive->image == NULL) {
            vdrive_setup_device(vdrive, 11);
            file_system_set_serial_hooks(11, file_system_device_enabled[3]);
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

static void detach_disk_image(disk_image_t *image, vdrive_t *floppy,
                              unsigned int unit)
{
    if (image != NULL) {
        switch (unit) {
          case 8:
            wd1770_detach_image(image, 8);
            fdc_detach_image(image, 8);
            drive_detach_image(image, 8);
            vdrive_detach_image(image, 8, floppy);
            break;
          case 9:
            wd1770_detach_image(image, 9);
            fdc_detach_image(image, 9);
            drive_detach_image(image, 9);
            vdrive_detach_image(image, 9, floppy);
            break;
          case 10:
            vdrive_detach_image(image, 10, floppy);
            break;
          case 11:
            vdrive_detach_image(image, 11, floppy);
            break;
        }
        disk_image_close(image);
    }
}

static int attach_disk_image(disk_image_t *image, vdrive_t *floppy,
                             const char *filename, unsigned int unit)
{
    disk_image_t new_image;
    int err = -1;

    if (filename == NULL) {
        log_error(attach_log, "No name, cannot attach floppy image.");
        return -1;
    }

    new_image.name = stralloc(filename);
    new_image.gcr = NULL;

    if (disk_image_open(&new_image) < 0) {
        free(new_image.name);
        log_error(attach_log, "Cannot open file `%s'.", filename);
        return -1;
    }

    detach_disk_image(image, floppy, unit);

    if (image != NULL) {
        free(image->name);
        free(image);
    }

    image = (disk_image_t *)xmalloc(sizeof(disk_image_t));

    memcpy(image, &new_image, sizeof(disk_image_t));

    switch (unit) {
      case 8:
        err = drive_attach_image(image, 8);
        err &= vdrive_attach_image(image, 8, floppy);
        err &= fdc_attach_image(image, 8);
        err &= wd1770_attach_image(image, 8);
        break;
      case 9:
        err = drive_attach_image(image, 9);
        err &= vdrive_attach_image(image, 9, floppy);
        err &= fdc_attach_image(image, 9);
        err &= wd1770_attach_image(image, 9);
        break;
      case 10:
        err = vdrive_attach_image(image, 10, floppy);
        break;
      case 11:
        err = vdrive_attach_image(image, 11, floppy);
        break;
    }
    if (err)
        free(image);
    return err;
}

/* ------------------------------------------------------------------------- */

int file_system_attach_disk(unsigned int unit, const char *filename)
{
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(unit);
    vdrive_setup_device(vdrive, unit);

    if (attach_disk_image(vdrive->image, vdrive, filename, unit) < 0) {
        return -1;
    } else {
        file_system_set_serial_hooks(unit, 0);
        flip_set_current(unit, filename);
        ui_display_drive_current_image(unit - 8, filename);
        return 0;
    }
    return 0;
}

void file_system_detach_disk(int unit)
{
    int i;
    vdrive_t *vdrive;
    
    if (unit < 0) {
        /* XXX Fixme: Hardcoded 2 drives here! */
        for (i = 0; i <= 3; i++) {
            vdrive = file_system_get_vdrive((unsigned int)(i + 8));
            if (vdrive != NULL)
                detach_disk_image(vdrive->image, vdrive, i + 8);
            set_file_system_device8((resource_value_t)
                                    file_system_device_enabled[i]);
            ui_display_drive_current_image(i, "");
	    }
    } else {
        vdrive = file_system_get_vdrive(unit);
        if (vdrive != NULL)
            detach_disk_image(vdrive->image, vdrive, (unsigned int)unit);
        switch(unit) {
          case 8:
            set_file_system_device8((resource_value_t)
                                    file_system_device_enabled[0]);
            ui_display_drive_current_image(0, "");
            break;
          case 9:
            set_file_system_device9((resource_value_t)
                                    file_system_device_enabled[1]);
            ui_display_drive_current_image(1, "");
            break;
          case 10:
            set_file_system_device10((resource_value_t)
                                     file_system_device_enabled[2]);
            ui_display_drive_current_image(2, "");
            break;
          case 11:
            set_file_system_device11((resource_value_t)
                                     file_system_device_enabled[3]);
            ui_display_drive_current_image(3, "");
            break;
        }
    }
}

