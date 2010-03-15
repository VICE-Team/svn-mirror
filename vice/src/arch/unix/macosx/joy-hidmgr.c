/*
 * joy-hidmgr.c - Mac OS X joystick support using IOHIDManager.
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
#ifdef HAS_HIDMGR

/* ----- Statics ----- */

static IOHIDManagerRef mgr;
static CFMutableArrayRef devices;

/* ----- Tools ----- */

static void CFSetApplierFunctionCopyToCFArray(const void *value, void *context)
{
    CFArrayAppendValue( ( CFMutableArrayRef ) context, value );
}

static Boolean IOHIDDevice_GetLongProperty( IOHIDDeviceRef inIOHIDDeviceRef, CFStringRef inKey, long * outValue )
{
    Boolean result = FALSE;
    
    if ( inIOHIDDeviceRef ) {
        CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty( inIOHIDDeviceRef, inKey );
        if ( tCFTypeRef ) {
            // if this is a number
            if ( CFNumberGetTypeID() == CFGetTypeID( tCFTypeRef ) ) {
                // get it's value
                result = CFNumberGetValue( ( CFNumberRef ) tCFTypeRef, kCFNumberSInt32Type, outValue );
            }
        }
    }
    return result;
}

/* ----- API ----- */

int  joy_hid_load_device_list(void)
{
    if ( !mgr ) {
        // create the manager
        mgr = IOHIDManagerCreate( kCFAllocatorDefault, 0L );
    }
    if ( mgr ) {
        // open it
        IOReturn tIOReturn = IOHIDManagerOpen( mgr, 0L);
        if ( kIOReturnSuccess != tIOReturn ) {
            log_message(LOG_DEFAULT, "mac_hidmgr: error opening manager!");
            return 0;
        } else {
            IOHIDManagerSetDeviceMatching( mgr, NULL );
            CFSetRef deviceSet = IOHIDManagerCopyDevices( mgr );
            if ( deviceSet ) {
                if ( devices ) {
                    CFRelease( devices );
                }
                devices = CFArrayCreateMutable( kCFAllocatorDefault, 0, & kCFTypeArrayCallBacks );
                CFSetApplyFunction( deviceSet, CFSetApplierFunctionCopyToCFArray, devices );
                CFIndex cnt = CFArrayGetCount( devices );
                CFRelease( deviceSet );
                int num_devices = (int)cnt;
                log_message(LOG_DEFAULT, "mac_joy: found %d devices (with IOHIDManager)", num_devices);
                return num_devices;
            }
        }
    } else {
        log_message(LOG_DEFAULT, "mac_hidmgr: can't create manager!");
    }
    return 0;
}

void joy_hid_unload_device_list(void)
{
    if(devices) {
        CFRelease( devices );
        devices = NULL;
    }
    if(mgr) {
        IOHIDManagerClose( mgr, 0 );
        mgr = NULL;
    }
}

int  joy_hid_enumerate_devices(joy_hid_device_t **result)
{
    CFIndex cnt = CFArrayGetCount(devices);

    /* first count joystick/gamepad devices */
    CFIndex idx;
    int num_devices = 0;
    for ( idx = 0; idx < cnt; idx++ ) {
        IOHIDDeviceRef dev = ( IOHIDDeviceRef ) CFArrayGetValueAtIndex( devices, idx );
        if(IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick) ||
           IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad)) {
            num_devices++;
        }
    }

    if(num_devices == 0) {
        return 0;
    }

    /* allocate devices */
    joy_hid_device_t *devs = (joy_hid_device_t *)lib_malloc(sizeof(joy_hid_device_t) * num_devices);
    if(devs == NULL) {
        return 0;
    }
    *result = devs;

    /* build joy_hid_device */
    joy_hid_device_t *d = devs;
    for ( idx = 0; idx < cnt; idx++ ) {
        IOHIDDeviceRef dev = ( IOHIDDeviceRef ) CFArrayGetValueAtIndex( devices, idx );
        if(IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick) ||
           IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad)) {
               
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
               
               d->device = dev;
               d->vendor_id = (int)vendor_id;
               d->product_id = (int)product_id;
               d->serial = joy_hid_get_device_serial(idx, devs, d->vendor_id, d->product_id);
               d->product_name = product_name;
               
               d++;
        }
    }
    
    return num_devices;
}

int  joy_hid_map_device(struct joystick_descriptor *joy, joy_hid_device_t *device)
{
    /* close old device */
    if(joy->hid->device) {
        IOHIDDeviceClose(joy->hid->device, 0);
    }
    
    /* release old elments array */
    if(joy->hid->all_elements) {
        CFRelease(joy->hid->all_elements);
    }
    
    /* store device */
    IOHIDDeviceRef dev = device->device;    
    
    /* reset all axis and buttons */
    joy->num_hid_axis = 0;
    joy->num_hid_buttons = 0;
    
    /* get all elements of device */
    CFArrayRef elements = IOHIDDeviceCopyMatchingElements( dev, NULL, 0 );    
    joy->hid->all_elements = elements;
    if ( elements ) {
        CFIndex idx;
        CFIndex cnt = CFArrayGetCount( elements );
        for ( idx = 0; idx < cnt; idx++ ) {
            IOHIDElementRef element = ( IOHIDElementRef ) CFArrayGetValueAtIndex( elements, idx );
            if ( element ) {
                uint32_t usagePage = IOHIDElementGetUsagePage( element );
                if(usagePage == kHIDPage_GenericDesktop) {
                    uint32_t usage = IOHIDElementGetUsage( element );
                    switch(usage) {
                    case kHIDUsage_GD_Pointer:
                    case kHIDUsage_GD_GamePad:
                    case kHIDUsage_GD_Joystick:
                        /* ignore */
                        break;
                    case kHIDUsage_GD_X:
                    case kHIDUsage_GD_Y:
                    case kHIDUsage_GD_Z:
                    case kHIDUsage_GD_Rx:
                    case kHIDUsage_GD_Ry:
                    case kHIDUsage_GD_Rz:
                        /* axis found */
                        if (joy->num_hid_axis == JOYSTICK_DESCRIPTOR_MAX_AXIS) {
                            log_message(LOG_DEFAULT, "mac_joy: TOO MANY AXIS FOUND!");
                        } else {
                            joy->hid->all_axis[joy->num_hid_axis] = element;
                            joy->num_hid_axis++;
                        }
                        break;
                    case kHIDUsage_GD_Hatswitch:
                        joy->num_hid_hat_switches++;
                        break;
                    case kHIDUsage_GD_Slider:
                        log_message(LOG_DEFAULT, "mac_joy:  (unsupported Slider found)");
                        break;
                    case kHIDUsage_GD_Wheel:
                        log_message(LOG_DEFAULT, "mac_joy:  (unsupported Wheel found)");
                        break;
                    case kHIDUsage_GD_Dial:
                        log_message(LOG_DEFAULT, "mac_joy:  (unsupported Dial found)");
                        break;
                    default:
                        log_message(LOG_DEFAULT, "mac_joy:  (unsupported HID element with Generic Desktip usage 0x%04x)",
                                    usage);
                        break;
                    }
                }
                else if(usagePage == kHIDPage_Button) {
                    /* buttons found */
                    if (joy->num_hid_buttons == JOYSTICK_DESCRIPTOR_MAX_BUTTONS) {
                        log_message(LOG_DEFAULT, "mac_joy: TOO MANY BUTTONS FOUND!");
                    } else {
                        joy->hid->all_buttons[joy->num_hid_buttons] = element;
                        joy->num_hid_buttons++;
                    }
                }
            }
        }
        
        /* open device */
        IOReturn result = IOHIDDeviceOpen( dev, 0 );
        if(result != kIOReturnSuccess) {
            log_message(LOG_DEFAULT, "mac_hidmgr: error opening device!");
            return 0;
        }
        
        joy->hid->device = dev;
        joy->mapped = 1;
    } else {
        joy->mapped = 0;
    }
    return joy->mapped;
}

int  joy_hid_map_axis(struct joystick_descriptor *joy, int id)
{
    joy_axis_t *axis = &joy->axis[id];
    
    /* default unmapped */
    axis->mapped = 0;
    
    /* no name given */
    if(axis->name == NULL) {
        return 0;
    }
    
    /* map tag */
    int tag = joy_hid_find_axis_tag(axis->name, -1);
    if(tag == -1) {
        return 0;
    }
    
    /* find all axis for tag */
    int i;
    for(i=0;i<joy->num_hid_axis;i++) {
        IOHIDElementRef element = joy->hid->all_axis[i];
        uint32_t usage = IOHIDElementGetUsage(element);
        if(usage == tag) {
            
            /* get range */
            CFIndex min = IOHIDElementGetPhysicalMin(element);
            CFIndex max = IOHIDElementGetPhysicalMax(element);
            axis->min_value = (int)min;
            axis->max_value = (int)max;
            
            /* found axis */
            joy->hid->mapped_axis[id] = element;
            axis->mapped = 1;
            return 1;
            
        }
    }
    
    return axis->mapped;
}

int  joy_hid_map_button(struct joystick_descriptor *joy, int id)
{
    int i;
    IOHIDElementRef element;
    int tag = joy->buttons[id].id;
    for(i=0;i<joy->num_hid_buttons;i++) {
        element = joy->hid->all_buttons[i];
        uint32_t usage = IOHIDElementGetUsage(element);
        if(usage == tag) {
            
            /* found button -> map it */
            joy->hid->mapped_buttons[id] = element;
            joy->buttons[id].mapped = 1;
            return 1;
            
        }
    }
    
    /* mapping failed */
    joy->buttons[id].mapped = 0;
    return 0;
}

const char *joy_hid_detect_axis(struct joystick_descriptor *joy, int id)
{
    int i;
    for(i=0;i<joy->num_hid_axis;i++) {
        IOHIDElementRef element = joy->hid->all_axis[i];
            
        /* find name */
        uint32_t usage = IOHIDElementGetUsage(element);
        const char *name = joy_hid_find_axis_name(usage);
        if(name != NULL) {
            
            /* get range */
            int min = (int)IOHIDElementGetPhysicalMin(element);
            int max = (int)IOHIDElementGetPhysicalMax(element);
            int t_min, t_max;
            joy_calc_threshold(min, max, joy->axis[id].threshold, &t_min, &t_max);
            
            IOHIDValueRef value;
            IOReturn result = IOHIDDeviceGetValue( joy->hid->device, 
                                                   element,
                                                   &value );
            if(result == kIOReturnSuccess) {
                int v = (int)IOHIDValueGetIntegerValue( value );
                if((v < t_min)||(v > t_max)) {
                    return name;
                }
            }
        }
    }
    return NULL;
}

int  joy_hid_detect_button(struct joystick_descriptor *joy)
{
    int i;
    for(i=0;i<joy->num_hid_buttons;i++) {
        IOHIDElementRef element = joy->hid->all_buttons[i];
        IOHIDValueRef value;
        IOReturn result = IOHIDDeviceGetValue( joy->hid->device, 
                                               element,
                                               &value );
        if(result == kIOReturnSuccess) {
            CFIndex v = IOHIDValueGetIntegerValue( value );
            if(v > 0) {
                return (int)IOHIDElementGetUsage(element);
            }
        }
    }
    return HID_INVALID_BUTTON;
}

int  joy_hid_read_button(struct joystick_descriptor *joy, int id)
{
    IOHIDValueRef value;
    IOReturn result = IOHIDDeviceGetValue( joy->hid->device, 
                                           joy->hid->mapped_buttons[id],
                                           &value );
    if(result == kIOReturnSuccess) {
        CFIndex v = IOHIDValueGetIntegerValue( value );
        return (v > 0);
    } else {
        return 0;
    }
}

int  joy_hid_read_axis(struct joystick_descriptor *joy, int id)
{
    IOHIDValueRef value;
    IOReturn result = IOHIDDeviceGetValue( joy->hid->device, 
                                           joy->hid->mapped_axis[id],
                                           &value );
    if(result == kIOReturnSuccess) {
        CFIndex v = IOHIDValueGetIntegerValue( value );
        joy_axis_t *axis = &joy->axis[id];
        if(v < axis->min_threshold) {
            return -1;
        } else if(v > axis->max_threshold) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

#endif /* HAS_HIDMGR */
#endif /* HAS_JOYSTICK */
