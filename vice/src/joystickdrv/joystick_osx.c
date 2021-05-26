/** \file   joystick_osx.c
 * \brief   Mac OS X joystick support using IOHIDManager
 *
 * \author  Christian Vogelgsang <chris@vogelgsang.org>
 */

/*
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

#include "joystick.h"
#include "log.h"
#include "lib.h"


#include <IOKit/hid/IOHIDManager.h>

/* ----- Statics ----- */

static IOHIDManagerRef mgr;

/* ----- Tools ----- */

static Boolean IOHIDDevice_GetLongProperty( IOHIDDeviceRef inIOHIDDeviceRef, CFStringRef inKey, long * outValue )
{
    Boolean result = FALSE;

    if ( inIOHIDDeviceRef ) {
        CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty( inIOHIDDeviceRef, inKey );
        if ( tCFTypeRef ) {
            /* if this is a number */
            if ( CFNumberGetTypeID() == CFGetTypeID( tCFTypeRef ) ) {
                /* get it's value */
                result = CFNumberGetValue((CFNumberRef)tCFTypeRef, kCFNumberLongType, outValue);
            }
        }
    }
    return result;
}

/* HID Mgr Types */
typedef IOHIDDeviceRef  hid_device_ref_t;
typedef IOHIDElementRef hid_element_ref_t;

/* model a hid element */
struct joy_hid_element {
    int     usage_page;
    int     usage;

    int     min_pvalue; /* physical value */
    int     max_pvalue;
    int     min_lvalue; /* logical value */
    int     max_lvalue;

    int ordinal;

    hid_element_ref_t internal_element;
};
typedef struct joy_hid_element joy_hid_element_t;
typedef struct joy_hid_element *joy_hid_element_ptr_t;

/* model a hid device */
struct joy_hid_device {
    int     vendor_id;
    int     product_id;
    char    *product_name;

    int     num_elements;    /* number of elements in device */
    joy_hid_element_t *elements;

    hid_device_ref_t internal_device; /* pointer to native device */

    CFArrayRef internal_elements;
};
typedef struct joy_hid_device joy_hid_device_t;
typedef struct joy_hid_device *joy_hid_device_ptr_t;

static int is_joystick(IOHIDDeviceRef ref)
{
    return
        IOHIDDeviceConformsTo( ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick ) ||
        IOHIDDeviceConformsTo( ref, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad );
}

static void joy_hidlib_close_device(joy_hid_device_t *device)
{
    /* close old device */
    if(device->internal_device != NULL) {
        IOHIDDeviceClose(device->internal_device, 0);
    }
}

static void joy_hidlib_enumerate_elements(joy_hid_device_t *device,
    int *num_axes,
    int *num_buttons,
    int *num_hats)
{
    *num_axes = 0;
    *num_buttons = 0;
    *num_hats = 0;

    IOHIDDeviceRef dev = device->internal_device;
    if(dev == NULL) {
        return;
    }

    /* get all elements of device */
    CFArrayRef internal_elements = IOHIDDeviceCopyMatchingElements( dev, NULL, 0 );
    if(!internal_elements) {
        return;
    }

    /* get number of elements */
    CFIndex cnt = CFArrayGetCount( internal_elements );
    device->num_elements = (int)cnt;

    /* create elements array */
    joy_hid_element_t *elements = (joy_hid_element_t *)
        lib_malloc(sizeof(joy_hid_element_t) * cnt);
    if(elements == NULL) {
        CFRelease(internal_elements);
        internal_elements = NULL;
        return;
    }

    /* enumerate and convert all elements */
    CFIndex i;
    joy_hid_element_t *e = elements;
    char x_axis_found = 0;
    char y_axis_found = 0;
    int axis_ordinal_to_assign = 2;
    for(i=0;i<cnt;i++) {
        IOHIDElementRef internal_element =
            ( IOHIDElementRef ) CFArrayGetValueAtIndex( internal_elements, i );
        if ( internal_element ) {
            uint32_t usage_page = IOHIDElementGetUsagePage( internal_element );
            uint32_t usage = IOHIDElementGetUsage( internal_element );
            CFIndex pmin = IOHIDElementGetPhysicalMin( internal_element );
            CFIndex pmax = IOHIDElementGetPhysicalMax( internal_element );
            CFIndex lmin = IOHIDElementGetLogicalMin( internal_element );
            CFIndex lmax = IOHIDElementGetLogicalMax( internal_element );

            e->usage_page = (int)usage_page;
            e->usage      = (int)usage;
            e->min_pvalue = (int)pmin;
            e->max_pvalue = (int)pmax;
            e->min_lvalue = (int)lmin;
            e->max_lvalue = (int)lmax;
            e->internal_element = internal_element;
            if(usage_page == kHIDPage_GenericDesktop) {
                switch(usage) {
                    case kHIDUsage_GD_X:
                    case kHIDUsage_GD_Y:
                    case kHIDUsage_GD_Z:
                    case kHIDUsage_GD_Rx:
                    case kHIDUsage_GD_Ry:
                    case kHIDUsage_GD_Rz:
                    case kHIDUsage_GD_Slider:
                    /* axis found */
                    /* check for valid axis */
                    if(e->min_pvalue != e->max_pvalue) {

                        /* check if axis already occured ?
                           this works around broken HID devices
                           that register multiple times e.g. an x axis
                           but only the last one works actually...
                        */
                        int j;
                        for(j=0;j<i;j++) {
                            if(elements[j].usage_page == kHIDPage_GenericDesktop && elements[j].usage == usage) {
                                break;
                            }
                        }
                        if(j==i) {
                            if (!x_axis_found && (usage == kHIDUsage_GD_X || usage == kHIDUsage_GD_Rx)) {
                                x_axis_found = 1;
                                e->ordinal = 0;
                            } else if (!y_axis_found && (usage == kHIDUsage_GD_Y || usage == kHIDUsage_GD_Ry)) {
                                y_axis_found = 1;
                                e->ordinal = 1;
                            } else {
                                e->ordinal = axis_ordinal_to_assign++;
                            }
                            (*num_axes)++;
                        } else {
                            log_message(LOG_DEFAULT, "joy-hid: ignoring multiple occurrence of axis element (0x%x)! (broken HID device?)", usage);
                            e->ordinal = elements[j].ordinal;
                            elements[j].ordinal = -1;
                        }
                    } else {
                        log_message(LOG_DEFAULT, "joy-hid: ignoring element (0x%x) with invalid range! (broken HID device?)", usage);
                    }
                    break;
                    case kHIDUsage_GD_Hatswitch:
                    e->ordinal = (*num_hats)++;
                    break;
                }
            }
            else if(usage_page == kHIDPage_Button) {
                /* buttons found */
                e->ordinal = (*num_buttons)++;
            }
        } else {
            e->usage_page = -1;
            e->usage      = -1;
            e->min_pvalue = -1;
            e->max_pvalue = -1;
            e->min_lvalue = -1;
            e->max_lvalue = -1;
            e->ordinal    = -1;
            e->internal_element = NULL;
        }
        e++;
    }
    if (!x_axis_found || !y_axis_found) {
        axis_ordinal_to_assign = 0;
        for(i=0;i<cnt;i++) {
            if (elements[i].usage_page == kHIDPage_GenericDesktop && (
                elements[i].usage == kHIDUsage_GD_Z
                || elements[i].usage == kHIDUsage_GD_Rz
                || elements[i].usage == kHIDUsage_GD_Slider
            ) && elements[i].ordinal >= *num_axes) {
                elements[i].ordinal = axis_ordinal_to_assign++;
            }
        }
    }

    /* keep the reference until the elements are free'ed again */
    device->internal_elements = internal_elements;
    device->elements = elements;
}

static void joy_hidlib_free_elements(joy_hid_device_t *device)
{
    if(device == NULL) {
        return;
    }
    if(device->elements) {
        lib_free(device->elements);
        device->elements = NULL;
    }
    if(device->internal_elements) {
        CFRelease(device->internal_elements);
        device->internal_elements = NULL;
    }
}

static int joy_hidlib_get_value(joy_hid_device_t *device,
                          joy_hid_element_t *element,
                          int *value, int phys)
{
    IOHIDValueRef value_ref;
    IOReturn result = IOHIDDeviceGetValue( device->internal_device,
                                           element->internal_element,
                                           &value_ref );
    if(result == kIOReturnSuccess) {
        if(phys) {
            *value = (int)IOHIDValueGetScaledValue( value_ref, kIOHIDValueScaleTypePhysical );
        } else {
            *value = (int)IOHIDValueGetIntegerValue( value_ref );
        }
        return 0;
    } else {
        return -1;
    }
}

static void osx_joystick_read(int joyport, void* priv) {
    joy_hid_device_t *device = priv;
    int i;
    int value;
    for (i = 0; i < device->num_elements; i++) {
        joy_hid_element_t e = device->elements[i];
        if (e.ordinal >= 0) {
            if(e.usage_page == kHIDPage_GenericDesktop) {
                switch(e.usage) {
                    case kHIDUsage_GD_X:
                    case kHIDUsage_GD_Y:
                    case kHIDUsage_GD_Z:
                    case kHIDUsage_GD_Rx:
                    case kHIDUsage_GD_Ry:
                    case kHIDUsage_GD_Rz:
                    case kHIDUsage_GD_Slider:
                    if (joy_hidlib_get_value(device,
                                        &e,
                                        &value, 1) >= 0) {
                        if (value < e.min_pvalue*3/4 + e.max_pvalue/4) {
                            joy_axis_event(joyport, e.ordinal, JOY_AXIS_NEGATIVE);
                        } else if (value > e.min_pvalue/4 + e.max_pvalue*3/4) {
                            joy_axis_event(joyport, e.ordinal, JOY_AXIS_POSITIVE);
                        } else {
                            joy_axis_event(joyport, e.ordinal, JOY_AXIS_MIDDLE);
                        }
                    }
                    break;
                    case kHIDUsage_GD_Hatswitch:
                    if (joy_hidlib_get_value(device,
                                        &e,
                                        &value, 0) >= 0) {
                        if (value >= 0 && value <= 8) {
                            joy_hat_event(joyport, e.ordinal, hat_map[value]);
                        }
                    }
                }
            } else if (e.usage_page == kHIDPage_Button) {
                if (joy_hidlib_get_value(device, &e, &value, 0) >= 0) {
                    joy_button_event(joyport, e.ordinal, value);
                }
            }
        }
    }
}

static void osx_joystick_close(void *priv) {
    joy_hid_device_t *device = priv;
    joy_hidlib_free_elements(device);
    joy_hidlib_close_device(device);
    lib_free(device);
}

static joystick_driver_t osx_joystick_driver = {
    .poll = osx_joystick_read,
    .close = osx_joystick_close
};

/* ----- API ----- */

void joy_hidlib_init(void)
{
    if ( !mgr ) {
        /* create the manager */
        mgr = IOHIDManagerCreate( kCFAllocatorDefault, 0L );
    }
    if( !mgr ) {
        return;
    }
    /* open it */
    IOReturn tIOReturn = IOHIDManagerOpen( mgr, 0L);
    if ( kIOReturnSuccess != tIOReturn ) {
        return;
    }
    IOHIDManagerSetDeviceMatching( mgr, NULL );
    /* create set of devices */
    CFSetRef device_set = IOHIDManagerCopyDevices( mgr );
    if ( !device_set ) {
        return;
    }

    int i;
    CFIndex num_devices =     /* get size */
        CFSetGetCount( device_set );
    int axes;
    int buttons;
    int hats;
    IOHIDDeviceRef *all_devices = lib_malloc(sizeof(IOHIDDeviceRef) * num_devices);
    CFSetGetValues(device_set, (const void **)all_devices);

    joy_hid_device_t *d;
    for ( i = 0; i < num_devices ; i++ ) {
        IOHIDDeviceRef dev = all_devices[i];
        if(is_joystick(dev)) {

            long vendor_id = 0;
            IOHIDDevice_GetLongProperty( dev, CFSTR( kIOHIDVendorIDKey ), &vendor_id );
            long product_id = 0;
            IOHIDDevice_GetLongProperty( dev, CFSTR( kIOHIDProductIDKey ), &product_id );
            CFStringRef product_key;
            product_key = IOHIDDeviceGetProperty( dev, CFSTR( kIOHIDProductKey ) );
            char *product_name = "N/A";
            if(product_key) {
               char buffer[256];
               if(CFStringGetCString(product_key, buffer, 256, kCFStringEncodingUTF8)) {
                   product_name = strdup(buffer);
               }
            }

            d = lib_malloc(sizeof(joy_hid_device_t));

            d->internal_device = dev;
            d->vendor_id = (int)vendor_id;
            d->product_id = (int)product_id;
            d->product_name = product_name;
            joy_hidlib_enumerate_elements(d, &axes, &buttons, &hats);
            register_joystick_driver(&osx_joystick_driver, product_name, d, axes, buttons, hats);
        }
    }
    lib_free(all_devices);
    CFRelease( device_set );
}

void joy_hidlib_exit(void)
{
    if(mgr) {
        IOHIDManagerClose( mgr, 0 );
        mgr = NULL;
    }
}
