/*
 * joy-hid.h - Joystick support for Mac OS X using HID Utility Library.
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
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

#ifndef VICE_JOY_HID_H
#define VICE_JOY_HID_H


/* NOTE: We use the HID Utilites Library provided by Apple for free

   http://developer.apple.com/samplecode/HID_Utilities_Source/index.html

   Make sure to install this (static) library first!
*/
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDUsageTables.h>
#include "HID_Utilities_External.h"

/* HID Mgr Types */
typedef pRecDevice  hid_device_ref_t;
typedef pRecElement hid_element_ref_t;



/* axis map: define names of available axis on a HID device */
struct joy_hid_axis_info {
    const char *name;
    int id;
};
typedef struct joy_hid_axis_info joy_hid_axis_info_t;

/* public list of axis names */
extern joy_hid_axis_info_t joy_hid_axis_infos[];

/* information on the HID device */
struct joy_hid_device {
    int     vendor_id;
    int     product_id;
    int     serial;
    char    *product_name;
    
    hid_device_ref_t device;
};
typedef struct joy_hid_device joy_hid_device_t;

/* describe the HID Util dependent parts of the joystick device */
struct joy_hid_dev  {
    hid_device_ref_t device;

    hid_element_ref_t mapped_axis[HID_NUM_AXIS];
    hid_element_ref_t mapped_buttons[HID_NUM_BUTTONS];
    
    hid_element_ref_t all_buttons[JOYSTICK_DESCRIPTOR_MAX_BUTTONS];
    hid_element_ref_t all_axis[JOYSTICK_DESCRIPTOR_MAX_AXIS];
};
typedef struct joy_hid_dev joy_hid_dev_t;

extern joy_hid_dev_t joy_hid_dev_a;
extern joy_hid_dev_t joy_hid_dev_b;

/* forward declare descriptor */
struct joystick_descriptor;

/* functions */
extern int  joy_hid_load_device_list(void);
extern void joy_hid_unload_device_list(void);
extern int  joy_hid_enumerate_devices(joy_hid_device_t **result);
extern void joy_hid_free_devices(int num,joy_hid_device_t *devices);

extern int  joy_hid_map_device(struct joystick_descriptor *joy, joy_hid_device_t *device);
extern int  joy_hid_map_axis(struct joystick_descriptor *joy, int axis);
extern int  joy_hid_map_button(struct joystick_descriptor *joy, int id);

extern const char *joy_hid_detect_axis(struct joystick_descriptor *joy, int axis);
extern int  joy_hid_detect_button(struct joystick_descriptor *joy);

extern int  joy_hid_read_button(struct joystick_descriptor *jd, int id);
extern int  joy_hid_read_axis(struct joystick_descriptor *jd, int axis);

#endif
