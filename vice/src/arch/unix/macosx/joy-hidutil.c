/*
 * joy-hidutil.c - Mac OS X joystick support using HID Utility Library.
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
#ifndef HAS_HIDMGR

/* ----- Helpers ----- */

static pRecElement find_axis_element(joystick_descriptor_t *joy,const char *name)
{
    int tag = joy_hid_find_axis_tag(name, -1);
    if(tag == -1)
        return NULL;
    
    int i;
    for (i = 0; i < joy->num_hid_axis; i++) {
        pRecElement elem = joy->hid->all_axis[i];
        if (elem->usage == tag) {
            return elem;
        }
    }
    return NULL;
}

static pRecElement find_button_element(joystick_descriptor_t *joy,int id)
{
    int tag = joy->buttons[id].id;
    int i;

    for (i = 0; i < joy->num_hid_buttons; i++) {
      pRecElement elem = joy->hid->all_buttons[i];
      if (elem->usage == tag) {
          return elem;
      }
    }
    return NULL;  
}

/* ----- Setup Joystick Descriptor ----- */

static void build_button_axis_lists(joystick_descriptor_t *joy)
{
    pRecElement element;

    joy->num_hid_buttons = 0;
    joy->num_hid_axis = 0;
    joy->num_hid_hat_switches = 0;

    for (element = HIDGetFirstDeviceElement(joy->hid->device, kHIDElementTypeInput);
         element != NULL;
         element = HIDGetNextDeviceElement(element, kHIDElementTypeInput)) {
        /* axis elements */
        if (element->usagePage == kHIDPage_GenericDesktop) {
            const char *name = joy_hid_find_axis_name(element->usage);

            if (name != NULL) {
                if (joy->num_hid_axis == JOYSTICK_DESCRIPTOR_MAX_AXIS) {
                    log_message(LOG_DEFAULT, "mac_joy: TOO MANY AXIS FOUND!");
                } else {
                    joy->hid->all_axis[joy->num_hid_axis] = element;
                    joy->num_hid_axis++;
                }
            }
        }
        /* button elements */
        else if (element->usagePage == kHIDPage_Button) {
            if (element->usage >= 1) {
                if (joy->num_hid_buttons == JOYSTICK_DESCRIPTOR_MAX_BUTTONS) {
                    log_message(LOG_DEFAULT, "mac_joy: TOO MANY BUTTONS FOUND!");
                } else {
                    joy->hid->all_buttons[joy->num_hid_buttons] = element;
                    joy->num_hid_buttons++;
                }
            }
        }
    }
}

/* ----- API ----- */

int  joy_hid_load_device_list(void)
{
    int num_devices;

    /* build device list */
    HIDBuildDeviceList(kHIDPage_GenericDesktop, 0);

    /* no device list? -> no joysticks! */
    if (!HIDHaveDeviceList()) {
        return 0;
    }

    /* get number of devices */
    num_devices = HIDCountDevices();
    log_message(LOG_DEFAULT, "mac_joy: found %d devices (with HID Utils)", num_devices);
    if (num_devices == 0) {
        return 0;
    }

    return num_devices;    
}

void joy_hid_unload_device_list(void)
{
    /* cleanup device list */
    HIDReleaseDeviceList();
}

int  joy_hid_enumerate_devices(joy_hid_device_t **result)
{
    pRecDevice device;
    int num_devices = 0;

    /* iterate through all devices */
    for (device = HIDGetFirstDevice(); 
         device != NULL;
         device = HIDGetNextDevice(device)) {
        /* check if its a joystick or game pad device */
        if ((device->usage == kHIDUsage_GD_Joystick) || 
            (device->usage == kHIDUsage_GD_GamePad)) {
            num_devices ++;       
        }
    }
    
    /* nothing found */
    if(num_devices == 0) {
        return 0;
    }
    
    /* allocate devices */
    joy_hid_device_t *devs = (joy_hid_device_t *)lib_malloc(sizeof(joy_hid_device_t) * num_devices);
    if(devs == NULL) {
        return 0;
    }
    *result = devs;
    
    /* iterate through all devices */
    joy_hid_device_t *d = devs;
    int num = 0;
    for (device = HIDGetFirstDevice(); 
         device != NULL; 
         device = HIDGetNextDevice(device)) {
        /* check if its a joystick or game pad device */
        if ((device->usage == kHIDUsage_GD_Joystick) || 
            (device->usage == kHIDUsage_GD_GamePad)) {
    
            devs->device = device;
            devs->vendor_id  = device->vendorID;
            devs->product_id = device->productID;
            devs->serial = joy_hid_get_device_serial(num, devs, d->vendor_id, d->product_id);
            devs->product_name = strdup(device->product);

            d++;
            num++;
        }
    }

    return num_devices;
}

int  joy_hid_map_device(struct joystick_descriptor *joy, joy_hid_device_t *dev)
{
    joy->hid->device = dev->device;
    
    build_button_axis_lists(joy);
    
    joy->mapped = 1;
    return joy->mapped;
}

int  joy_hid_map_axis(struct joystick_descriptor *joy, int id)
{
    joy_axis_t *axis = &joy->axis[id];
    pRecElement element = find_axis_element(joy, axis->name);
    if(element != NULL) {
        joy->hid->mapped_axis[id] = element;
        axis->min_value = element->min;
        axis->max_value = element->max;
        axis->mapped = 1;
    } else {
        axis->mapped = 0;
    }
    return axis->mapped;
}

int  joy_hid_map_button(struct joystick_descriptor *joy, int id)
{
    joy_button_t *button = &joy->buttons[id];
    pRecElement element = find_button_element(joy, id);
    if(element != NULL) {
        joy->hid->mapped_buttons[id] = element;
        button->mapped = 1;
    } else {
        button->mapped = 0;
    }
    return button->mapped;
}

const char *joy_hid_detect_axis(struct joystick_descriptor *joy, int id)
{
    pRecDevice device = joy->hid->device;
    joy_axis_t *axis = &joy->axis[id];
    int i;

    if(device == NULL) {
        return NULL;
    }
    
    for (i = 0; i < joy->num_hid_axis; i++) {
        pRecElement element = joy->hid->all_axis[i];
        if (HIDIsValidElement(device, element)) {
            int value = HIDGetElementValue(device, element);
            
            /* calc threshold for this element */
            int min,max;
            joy_calc_threshold(element->min, element->max, axis->threshold,
                               &min, &max);
            
            if((value < min) || (value > max)) {
                return joy_hid_find_axis_name(element->usage);
            }
        }
    }
    return NULL;    
}

int  joy_hid_detect_button(struct joystick_descriptor *joy)
{
    pRecDevice device = joy->hid->device;
    int i;

    if(device == NULL) {
        return HID_INVALID_BUTTON;
    }

    for (i = 0; i < joy->num_hid_buttons; i++) {
        pRecElement element = joy->hid->all_buttons[i];
        if (HIDIsValidElement(device, element)) {
            int value = HIDGetElementValue(device, element);
            if(value > 0) {
                return element->usage;
            }
        }
    }
    return HID_INVALID_BUTTON;
}

int  joy_hid_read_button(struct joystick_descriptor *jd, int id)
{
    pRecDevice device = jd->hid->device;
    pRecElement element = jd->hid->mapped_buttons[id];
    
    if((device == NULL) || (element == NULL)) {
        return 0;
    }
    
    if (!HIDIsValidElement(device, element)) {
        return 0;
    }

    return (HIDGetElementValue(device, element) > 0);
}

int  joy_hid_read_axis(struct joystick_descriptor *jd, int id)
{
    pRecDevice device = jd->hid->device;
    pRecElement element = jd->hid->mapped_axis[id];
    int value;
    
    if((device == NULL) || (element == NULL)) {
        return 0;
    }
    
    if (!HIDIsValidElement(device, element)) {
        return 0;
    }
 
    value = HIDGetElementValue(device, element);
    joy_axis_t *axis = &jd->axis[id];
    if (value < axis->min_threshold) {
        return -1;
    } else if (value > axis->max_threshold) {
        return 1;
    } else {
        return 0;
    }
}

#endif /* !HAS_HIDMGR */
#endif /* HAS_JOYSTICK */
