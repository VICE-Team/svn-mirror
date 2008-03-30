/*
 * attach.c - File system attach management.
 *
 * Written by
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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
#include "resources.h"
#include "true1541.h"
#include "attach.h"

static int file_system_device8_enabled;
static int file_system_device9_enabled;
static int file_system_device10_enabled;
static int file_system_device11_enabled;

static int set_file_system_device8(resource_value_t v);
static int set_file_system_device9(resource_value_t v);
static int set_file_system_device10(resource_value_t v);
static int set_file_system_device11(resource_value_t v);

static resource_t resources[] = {
    { "FileSystemDevice8", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &file_system_device8_enabled,
      set_file_system_device8 },
    { "FileSystemDevice9", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &file_system_device9_enabled,
      set_file_system_device9 },
    { "FileSystemDevice10", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &file_system_device10_enabled,
      set_file_system_device10 },
    { "FileSystemDevice11", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &file_system_device11_enabled,
      set_file_system_device11 },
    { NULL }
};

int file_system_init_resources(void)
{
    return resources_register(resources);
}

void initialize_drives(void)
{
    /* Initialize drives.  Only drive #8 allows true 1541 emulation.  */
    initialize_1541(8,
                    (file_system_device8_enabled ? DT_FS : DT_DISK) | DT_1541,
                    true1541_attach_floppy, true1541_detach_floppy, NULL);
    initialize_1541(9,
                    (file_system_device9_enabled ? DT_FS : DT_DISK) | DT_1541,
                    NULL, NULL, NULL);
    initialize_1541(10,
                    (file_system_device10_enabled ? DT_FS : DT_DISK) | DT_1541,
                    NULL, NULL, NULL);
    initialize_1541(11,
                    (file_system_device11_enabled ? DT_FS : DT_DISK) | DT_1541,
                    NULL, NULL, NULL);
}

static int set_file_system_device8(resource_value_t v)
{
    serial_t *p;
    DRIVE *floppy;

    file_system_device8_enabled = (int) v;

    p = get_serial_device(8);
    floppy = (DRIVE *)p->info;
    if (floppy != NULL) {
	if (floppy->ActiveFd < 0) {
	    p->inuse = 0;
	    initialize_1541(8, (file_system_device8_enabled
                        ? DT_FS : DT_DISK) | DT_1541,
                        true1541_attach_floppy, true1541_detach_floppy,
                        floppy);
	}
    }
    return 0;
}

static int set_file_system_device9(resource_value_t v)
{
    serial_t *p;
    DRIVE *floppy;

    file_system_device9_enabled = (int) v;

    p = get_serial_device(9);
    floppy = (DRIVE *)p->info;
    if (floppy != NULL) {
	if (floppy->ActiveFd < 0) {
	    p->inuse = 0;
	    initialize_1541(9, (file_system_device9_enabled
                        ? DT_FS : DT_DISK) | DT_1541, NULL, NULL, floppy);
	}
    }
    return 0;
}

static int set_file_system_device10(resource_value_t v)
{
    serial_t *p;
    DRIVE *floppy;

    file_system_device10_enabled = (int) v;

    p = get_serial_device(10);
    floppy = (DRIVE *)p->info;
    if (floppy != NULL) {
	if (floppy->ActiveFd < 0) {
	    p->inuse = 0;
	    initialize_1541(10, (file_system_device10_enabled 
                        ? DT_FS : DT_DISK) | DT_1541, NULL, NULL, floppy);
	}
    }
    return 0;
}

static int set_file_system_device11(resource_value_t v)
{
    serial_t *p;
    DRIVE *floppy;

    file_system_device11_enabled = (int) v;

    p = get_serial_device(11);
    floppy = (DRIVE *)p->info;
    if (floppy != NULL) {
	if (floppy->ActiveFd < 0) {
	    p->inuse = 0;
	    initialize_1541(11, (file_system_device11_enabled
                        ? DT_FS : DT_DISK) | DT_1541, NULL, NULL, floppy);
	}
    }
    return 0;
}

int file_system_attach_disk(int unit, char *filename)
{

    serial_t *p;
    DRIVE *floppy;

    p = get_serial_device(unit);
    floppy = (DRIVE *)p->info;
    if ((floppy == NULL) || ((floppy->type & DT_FS) == DT_FS)) {
	p->inuse = 0;
	if (unit == 8)
	    initialize_1541(unit, DT_DISK | DT_1541,
                            true1541_attach_floppy, true1541_detach_floppy,
                            floppy);
	else
	    initialize_1541(unit, DT_DISK | DT_1541, NULL, NULL, floppy);
    }
    return serial_select_file(DT_DISK | DT_1541, unit, filename);
}

void file_system_detach_disk(int unit)
{
    if (unit < 0) {
	remove_serial(8);
	set_file_system_device8((resource_value_t)
                                 file_system_device8_enabled);
	remove_serial(9);
	set_file_system_device9((resource_value_t)
                                 file_system_device9_enabled);
	remove_serial(10);
	set_file_system_device10((resource_value_t)
                                  file_system_device10_enabled);
	remove_serial(11);
	set_file_system_device11((resource_value_t)
                                  file_system_device11_enabled);
    } else {
	remove_serial(unit);
	switch(unit) {
	  case 8:
	    set_file_system_device8((resource_value_t)
                                     file_system_device8_enabled);
	    break;
	  case 9:
	    set_file_system_device9((resource_value_t)
                                     file_system_device9_enabled);
	    break;
	  case 10:
	    set_file_system_device10((resource_value_t)
                                      file_system_device10_enabled);
	    break;
	  case 11:
	    set_file_system_device11((resource_value_t)
                                      file_system_device11_enabled);
	    break;
	}
    }
}

