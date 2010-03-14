/*
 * joy-hid.c - Mac OS X joystick support using USB HID devices.
 *
 * Written by
 *   Christian Vogelgsang <chris@vogelgsang.org>
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

#define JOY_INTERNAL

#include "vice.h"
#include "joy.h"
#include "log.h"
#include "lib.h"

#ifdef HAS_JOYSTICK

/* ----- Static Data ----- */

joy_hid_dev_t joy_hid_dev_a;
joy_hid_dev_t joy_hid_dev_b;

joy_hid_axis_info_t joy_hid_axis_infos[] = {
    { "X", kHIDUsage_GD_X },
    { "Y", kHIDUsage_GD_Y },
    { "Z", kHIDUsage_GD_Z },
    { "Rx", kHIDUsage_GD_Rx },
    { "Ry", kHIDUsage_GD_Ry },
    { "Rz", kHIDUsage_GD_Rz },
    { NULL, 0 }
};

/* ----- Tools ----- */

/* count devices with same vid:pid */
int joy_hid_get_device_serial(int size, joy_hid_device_t *devs, int vid, int pid)
{
    int i;
    int serial = 0;
    
    for(i=0;i<size;i++) {
        if((devs->vendor_id == vid) && (devs->product_id == pid)) {
            serial++;
        }
        devs++;
    }
    return serial;
}

const char *joy_hid_find_axis_name(int tag)
{
    joy_hid_axis_info_t *ptr = joy_hid_axis_infos;;
    while(ptr->name != NULL) {
        if (ptr->id == tag) {
            return ptr->name;
        }
        ptr++;
    }
    return NULL;
}

int joy_hid_find_axis_tag(const char *name,int def)
{
    if (name == NULL) {
        return def;
    }
    
    joy_hid_axis_info_t *ptr = joy_hid_axis_infos;;
    while(ptr->name != NULL) {
        if (strcmp(ptr->name, name) == 0) {
            return ptr->id;
        }
        ptr++;
    }
    return def;
}

void joy_hid_free_devices(int num_devices, joy_hid_device_t *devices)
{
    joy_hid_device_t *d = devices;
    int i;
    
    for(i=0;i<num_devices;i++) {
        lib_free(d->product_name);
        d++;
    }
    lib_free(devices);
}

#endif /* HAS_JOYSTICK */
