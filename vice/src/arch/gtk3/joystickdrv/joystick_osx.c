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
#include <pthread.h>

// Forward declarations for functions defined in this file and called by joy_arch_osx.c
void joy_hidlib_init(void);
void joy_hidlib_exit(void);

// Global mutex for protecting HID device access
static pthread_mutex_t hid_mutex = PTHREAD_MUTEX_INITIALIZER;
static int mutex_initialized = 0;

// Helper function to safely lock/unlock mutex
static int safe_mutex_lock(void) {
    if (!mutex_initialized) {
        log_message(LOG_DEFAULT, "joy-hid: mutex not initialized");
        return -1;
    }
    int result = pthread_mutex_lock(&hid_mutex);
    if (result != 0) {
        log_message(LOG_DEFAULT, "joy-hid: mutex lock failed: %d", result);
    }
    return result;
}

static int safe_mutex_unlock(void) {
    if (!mutex_initialized) {
        log_message(LOG_DEFAULT, "joy-hid: mutex not initialized");
        return -1;
    }
    int result = pthread_mutex_unlock(&hid_mutex);
    if (result != 0) {
        log_message(LOG_DEFAULT, "joy-hid: mutex unlock failed: %d", result);
    }
    return result;
}

void joystick_arch_init(void)
{
    log_message(LOG_DEFAULT, "Initializing joystick support");
    mutex_initialized = 1;
    joy_hidlib_init();
}

void joystick_arch_shutdown(void)
{
    log_message(LOG_DEFAULT, "Shutting down joystick support");
    if (mutex_initialized) {
        safe_mutex_lock();
        safe_mutex_unlock();
        mutex_initialized = 0;
    }
}

/*
 * This hat map was created from values observed on macOS 12 with PS4 and PS5 controller (bluetooth),
 * and based on various searches other controllers use this scheme. Xbox controllers are slightly
 * different which is handled via a hack later.
 */
#define MAX_HAT_MAP_INDEX 8
static const uint8_t hat_map[MAX_HAT_MAP_INDEX + 1] = {
    JOYSTICK_DIRECTION_UP,                              /* 0 */
    JOYSTICK_DIRECTION_UP | JOYSTICK_DIRECTION_RIGHT,   /* 1 */
    JOYSTICK_DIRECTION_RIGHT,                           /* 2 */
    JOYSTICK_DIRECTION_RIGHT | JOYSTICK_DIRECTION_DOWN, /* 3 */
    JOYSTICK_DIRECTION_DOWN,                            /* 4 */
    JOYSTICK_DIRECTION_DOWN | JOYSTICK_DIRECTION_LEFT,  /* 5 */
    JOYSTICK_DIRECTION_LEFT,                            /* 6 */
    JOYSTICK_DIRECTION_LEFT | JOYSTICK_DIRECTION_UP,    /* 7 */
    0,                                                  /* 8 */
};

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

    hid_element_ref_t internal_element_ref;
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

    // The persistent IOHIDDeviceRef obtained from IOHIDManager.
    IOHIDDeviceRef  persistent_iohid_ref; 
    
    // The IOHIDDeviceRef currently used for I/O (result of IOHIDDeviceOpen).
    IOHIDDeviceRef  opened_io_handle;     

    // Stores the CFArrayRef for elements, released when elements are freed.
    CFArrayRef elements_cfarray; 
    
    // Timestamp of last failed open attempt - used for cool-down
    time_t    last_failed_open_attempt_time;

    // --- NEW: flag to mark that internal resources were already freed ---
    int freed;
};
typedef struct joy_hid_device joy_hid_device_t;
typedef struct joy_hid_device *joy_hid_device_ptr_t;

static void setupC64JoystickEventHandlers(joy_hid_device_t *device);

static int is_joystick(IOHIDDeviceRef ref)
{
    return
        IOHIDDeviceConformsTo( ref, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick ) ||
        IOHIDDeviceConformsTo( ref, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad );
}

static bool ensure_device_open_for_io(joy_hid_device_t *device) {
    if (device == NULL) {
        return false;
    }
    // If already have an opened handle, assume it's good (caller will handle I/O errors)
    if (device->opened_io_handle != NULL) {
        return true;
    }
    // If no persistent ref, cannot open
    if (device->persistent_iohid_ref == NULL) {
        log_message(LOG_DEFAULT, "joy-hid: ensure_device_open_for_io: persistent_iohid_ref is NULL for %s", device->product_name ? device->product_name : "Unknown device");
        return false;
    }

    // Check if we're in a cool-down period after a failed open
    time_t now = time(NULL);
    if (device->last_failed_open_attempt_time > 0 && 
        (now - device->last_failed_open_attempt_time) < 2) { // 2-second cool-down
        // Too soon after last failed attempt, skip this poll cycle
        return false;
    }

    log_message(LOG_DEFAULT, "joy-hid: Attempting to IOHIDDeviceOpen %s", device->product_name ? device->product_name : "Unknown device");
    
    // Special handling for TheC64 Joystick
    bool is_thec64 = (device->vendor_id == 0x1c59 && device->product_id == 0x0023);
    
    IOReturn ret = IOHIDDeviceOpen(device->persistent_iohid_ref, kIOHIDOptionsTypeNone);

    if (ret == kIOReturnSuccess || ret == kIOReturnStillOpen) {
        log_message(LOG_DEFAULT, "joy-hid: IOHIDDeviceOpen for %s returned %s. Performing readiness check.", 
                   device->product_name ? device->product_name : "Unknown device", 
                   ret == kIOReturnSuccess ? "kIOReturnSuccess" : "kIOReturnStillOpen");
        
        // Set the handle before sleep to ensure it's available for closing if needed
        device->opened_io_handle = device->persistent_iohid_ref;
        
        // Add a short delay to allow device to initialize properly
        // This is particularly important for TheC64 Joystick which needs more time
        if (is_thec64) {
            usleep(250000); // 250ms delay for TheC64 Joystick
            log_message(LOG_DEFAULT, "joy-hid: Added 250ms delay for TheC64 Joystick initialization");
            
            // For TheC64 joystick, just assume it's ready and return success
            // based on the behavior in your test framework
            log_message(LOG_DEFAULT, "joy-hid: TheC64 Joystick - skipping readiness check, returning success");
            device->last_failed_open_attempt_time = 0;
            
            // Setup special event handlers for TheC64 Joystick if needed
            setupC64JoystickEventHandlers(device);
            
            return true;
        }
        
        // For other joysticks, do the normal readiness check
        CFTypeRef productKey = IOHIDDeviceGetProperty(device->opened_io_handle, CFSTR(kIOHIDProductKey));
        if (productKey != NULL) {
            log_message(LOG_DEFAULT, "joy-hid: Device %s is open and responsive (got product key).", 
                       device->product_name ? device->product_name : "Unknown device");
            device->last_failed_open_attempt_time = 0; // Reset cool-down on success
            return true; // Successfully opened and seems ready
        } else {
            log_message(LOG_DEFAULT, "joy-hid: Device %s opened (or was still open) but GetProperty check failed. Marking as not ready.", 
                       device->product_name ? device->product_name : "Unknown device");
            IOHIDDeviceClose(device->opened_io_handle, kIOHIDOptionsTypeNone);
            device->opened_io_handle = NULL; // Device not responsive, so clear the handle.
            device->last_failed_open_attempt_time = now; // Update timestamp for cool-down
            return false; // Not ready yet
        }
    } else {
        log_message(LOG_DEFAULT, "joy-hid: IOHIDDeviceOpen for %s failed (error code %d)", 
                   device->product_name ? device->product_name : "Unknown device", ret);
        device->opened_io_handle = NULL;
        device->last_failed_open_attempt_time = now; // Update timestamp for cool-down
        return false;
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

    // Enumeration requires an open device handle.
    if (device == NULL || device->opened_io_handle == NULL) {
        log_message(LOG_DEFAULT, "joy-hid: enumerate_elements: Device or opened_io_handle is NULL.");
        return;
    }
    IOHIDDeviceRef dev_handle_for_enum = device->opened_io_handle;

    // Previous elements_cfarray should be released if re-enumerating
    if (device->elements_cfarray != NULL) {
        CFRelease(device->elements_cfarray);
        device->elements_cfarray = NULL;
    }
    if (device->elements != NULL) { // Should have been freed by caller if re-enumerating
        lib_free(device->elements);
        device->elements = NULL;
    }
    device->num_elements = 0;

    device->elements_cfarray = IOHIDDeviceCopyMatchingElements(dev_handle_for_enum, NULL, 0);
    
    if(!device->elements_cfarray) {
        log_message(LOG_DEFAULT, "joy-hid: enumerate_elements: IOHIDDeviceCopyMatchingElements failed for %s.", device->product_name ? device->product_name : "Unknown device");
        return;
    }

    CFIndex cnt = CFArrayGetCount(device->elements_cfarray);
    if (cnt == 0) {
        log_message(LOG_DEFAULT, "joy-hid: enumerate_elements: No elements found for %s.", device->product_name ? device->product_name : "Unknown device");
        CFRelease(device->elements_cfarray);
        device->elements_cfarray = NULL;
        return;
    }
    
    device->elements = (joy_hid_element_t *)lib_calloc(1, sizeof(joy_hid_element_t) * cnt);
    if(device->elements == NULL) {
        log_message(LOG_DEFAULT, "joy-hid: enumerate_elements: Failed to allocate memory for elements.");
        CFRelease(device->elements_cfarray);
        device->elements_cfarray = NULL;
        return;
    }

    CFIndex i;
    joy_hid_element_t *e = device->elements;
    char x_axis_found = 0;
    char y_axis_found = 0;

    // Special handling for TheC64 Joystick: only register first X, Y, and button 1
    bool is_thec64 = (device->vendor_id == 0x1c59 && device->product_id == 0x0023);
    char thec64_x = 0, thec64_y = 0, thec64_btn1 = 0, thec64_btn2 = 0;

    for(i=0;i<cnt;i++) {
        IOHIDElementRef internal_element =
            (IOHIDElementRef)CFArrayGetValueAtIndex(device->elements_cfarray, i);
        if (internal_element) {
            IOHIDElementType elem_type = IOHIDElementGetType(internal_element);
            /* Skip collection elements – they don't return values and will cause
               IOHIDDeviceGetValue() failures. */
            if (elem_type == kIOHIDElementTypeCollection) {
                continue;
            }

            uint32_t usage_page = IOHIDElementGetUsagePage(internal_element);
            uint32_t usage = IOHIDElementGetUsage(internal_element);
            CFIndex pmin = IOHIDElementGetPhysicalMin(internal_element);
            CFIndex pmax = IOHIDElementGetPhysicalMax(internal_element);
            CFIndex lmin = IOHIDElementGetLogicalMin(internal_element);
            CFIndex lmax = IOHIDElementGetLogicalMax(internal_element);

            // For TheC64: only register first X, Y, and button 1
            if (is_thec64) {
                if (usage_page == kHIDPage_GenericDesktop && usage == kHIDUsage_GD_X && !thec64_x) {
                    thec64_x = 1;
                } else if (usage_page == kHIDPage_GenericDesktop && usage == kHIDUsage_GD_Y && !thec64_y) {
                    thec64_y = 1;
                } else if (usage_page == kHIDPage_Button && usage == 1 && !thec64_btn1) {
                    thec64_btn1 = 1;
                } else if (usage_page == kHIDPage_Button && usage == 2 && !thec64_btn2) {
                    /* Accept second fire button; treat same as button 1 by giving same ordinal (0) */
                    thec64_btn2 = 1;
                } else {
                    continue; // Ignore all other elements for TheC64
                }
            }

            e->usage_page = (int)usage_page;
            e->usage      = (int)usage;
            e->min_pvalue = (int)pmin;
            e->max_pvalue = (int)pmax;
            e->min_lvalue = (int)lmin;
            e->max_lvalue = (int)lmax;
            e->internal_element_ref = internal_element;

            if(usage_page == kHIDPage_GenericDesktop) {
                switch(usage) {
                    case kHIDUsage_GD_X:
                        if (!x_axis_found) {
                            x_axis_found = 1;
                            e->ordinal = 0;
                            (*num_axes)++;
                        } else {
                            continue;
                        }
                        break;
                    case kHIDUsage_GD_Y:
                        if (!y_axis_found) {
                            y_axis_found = 1;
                            e->ordinal = 1;
                            (*num_axes)++;
                        } else {
                            continue;
                        }
                        break;
                    default:
                        continue;
                }
            } else if(usage_page == kHIDPage_Button) {
                if (usage == 1 && *num_buttons == 0) {
                    /* First fire button for any device */
                    e->ordinal = 0;
                    (*num_buttons)++;
                } else if (is_thec64 && usage == 2) {
                    /* THEC64 second fire button: same ordinal, do NOT bump num_buttons */
                    e->ordinal = 0;
                } else if (!is_thec64) {
                    /* Other devices: normal sequential buttons */
                    e->ordinal = (int)(*num_buttons);
                    (*num_buttons)++;
                } else {
                    continue;
                }
            } else {
                continue;
            }
        } else {
            e->usage_page = -1;
            e->usage      = -1;
            e->min_pvalue = -1;
            e->max_pvalue = -1;
            e->min_lvalue = -1;
            e->max_lvalue = -1;
            e->ordinal    = -1;
            e->internal_element_ref = NULL;
        }
        e++;
    }
    device->num_elements = (int)(e - device->elements);
    log_message(LOG_DEFAULT, "joy-hid: Enumerated %d elements for %s", device->num_elements, device->product_name ? device->product_name : "Unknown");
}

static void joy_hidlib_free_elements(joy_hid_device_t *device)
{
    // Disabled: freeing these during runtime causes use-after-free in core.
    (void)device;
}

static int joy_hidlib_get_value(joy_hid_device_t *device,
                          joy_hid_element_t *element,
                          int *value, int phys)
{
    if (device == NULL || element == NULL || value == NULL ) {
        log_message(LOG_DEFAULT, "joy-hid: get_value called with NULL pointers");
        return -1;
    }
    // Assumes caller (osx_joystick_read) called ensure_device_open_for_io
    if (device->opened_io_handle == NULL) {
        log_message(LOG_DEFAULT, "joy-hid: get_value: opened_io_handle is NULL for %s. Device not open.", 
                   device->product_name ? device->product_name : "Unknown");
        return -1; 
    }
    if (element->internal_element_ref == NULL) {
        log_message(LOG_DEFAULT, "joy-hid: get_value: internal_element_ref is NULL.");
        return -1;
    }
    
    // No special synthetic handling anymore; treat TheC64 like regular HID.
    bool is_thec64 = (device->vendor_id == 0x1c59 && device->product_id == 0x0023);
    
    IOHIDDeviceRef current_handle = device->opened_io_handle;
    IOHIDValueRef value_ref;
    IOReturn io_result = IOHIDDeviceGetValue(current_handle, element->internal_element_ref, &value_ref);
    if (io_result == kIOReturnSuccess) {
        if (phys) {
            *value = (int)IOHIDValueGetScaledValue(value_ref, kIOHIDValueScaleTypePhysical);
        } else {
            *value = (int)IOHIDValueGetIntegerValue(value_ref);
        }
        return 0;
    } else {
        log_message(LOG_DEFAULT, "joy-hid: get_value for %s failed (element usage 0x%X, error code %d)", 
                   device->product_name ? device->product_name : "Unknown", (unsigned int)element->usage, io_result);
        if (io_result == kIOReturnNoDevice || io_result == kIOReturnNotOpen || 
            io_result == kIOReturnNotResponding || io_result == kIOReturnError) {
            log_message(LOG_DEFAULT, "joy-hid: Closing opened_io_handle for %s due to IOKit error.", 
                       device->product_name ? device->product_name : "Unknown");
            if (device->opened_io_handle) {
                IOHIDDeviceClose(device->opened_io_handle, kIOHIDOptionsTypeNone);
            }
            device->opened_io_handle = NULL; // Mark as needing re-open
            device->last_failed_open_attempt_time = time(NULL); // Update timestamp for cool-down
        }
        return -1;
    }
}

static void osx_joystick_read(joystick_device_t *core_dev) {
    if (core_dev == NULL) {
        return;
    }
    // Lock early.
    if (safe_mutex_lock() != 0) {
        log_message(LOG_DEFAULT, "joy-hid: osx_joystick_read could not lock mutex at start.");
        return;
    }
    // Get device pointer AFTER acquiring lock.
    joy_hid_device_t *device = (joy_hid_device_t *)core_dev->priv; 
    if (device == NULL) {
        safe_mutex_unlock();
        return;
    }
    /* Make sure the HID is open and ready; if not, skip this poll cycle. */
    if (!ensure_device_open_for_io(device)) {
        safe_mutex_unlock();
        return;
    }
    if (device->elements == NULL || device->num_elements <= 0) {
        safe_mutex_unlock();
        return;
    }
    int i;
    int value;
    /* ------------------------------------------------------------------
     * THEC64 joystick special-case: aggregate duplicate fire buttons
     * ------------------------------------------------------------------
     * The USB THEC64 joystick exposes two HID button usages (1 and 2)
     * that are supposed to map to *one* logical fire button in VICE.
     * When we treat the two elements independently the second element
     * processed during the poll loop may overwrite the state generated
     * by the first element.  This results in only one of the two
     * physical buttons seeming to work (whichever element happens to be
     * processed last).
     *
     * To fix this we collect the state for all HID button elements first
     * and generate *one* logical button event afterwards, effectively
     * OR-ing the two usages together.  We only need this for the THEC64
     * joystick (vid 0x1c59 / pid 0x0023) – everything else keeps the
     * existing fast path.
     */

    bool is_thec64_device = (device->vendor_id == 0x1c59 && device->product_id == 0x0023);
    /* Support up to 32 logical buttons which is more than enough here */
    int aggregated_button_state[32];
    if (is_thec64_device) {
        /* Initialise aggregated state with 0 (not pressed) */
        for (int b = 0; b < 32; b++) {
            aggregated_button_state[b] = 0;
        }
    }
    for (i = 0; i < device->num_elements; i++) {
        if (core_dev->priv == NULL || device->opened_io_handle == NULL) {
            log_message(LOG_DEFAULT, "joy-hid: osx_joystick_read: device became invalid or closed mid-polling for %s.", device->product_name ? device->product_name : "Unknown");
            break; 
        }
        if (i >= device->num_elements) { 
            log_message(LOG_DEFAULT, "joy-hid: osx_joystick_read: index i (%d) out of bounds for num_elements (%d) mid-loop.", i, device->num_elements);
            break;
        }
        joy_hid_element_t *e = &device->elements[i];
        if (e->ordinal < 0) { 
            continue; 
        }
        int ret = joy_hidlib_get_value(device, e, &value, 0);
        if (ret >= 0) {
            if (e->usage_page == kHIDPage_GenericDesktop &&
                (e->usage == kHIDUsage_GD_X || e->usage == kHIDUsage_GD_Y ||
                 e->usage == kHIDUsage_GD_Z || e->usage == kHIDUsage_GD_Rx ||
                 e->usage == kHIDUsage_GD_Ry || e->usage == kHIDUsage_GD_Rz ||
                 e->usage == kHIDUsage_GD_Slider || e->usage == kHIDUsage_GD_DPadUp ||
                 e->usage == kHIDUsage_GD_DPadDown || e->usage == kHIDUsage_GD_DPadLeft ||
                 e->usage == kHIDUsage_GD_DPadRight) &&
                core_dev->axes && e->ordinal < core_dev->num_axes) {
                /* --- Special-case THEC64 joystick (vid 0x1c59 / pid 0x0023)
                   ---------------------------------------------------------
                   The USB THEC64 stick only reports hard-digital extremes on
                   its X and Y axes (0, 127, 255).  The Swift test-utility
                   simply treats any value < 127 as negative, any value > 127
                   as positive and 127 as neutral.  Do the same here so that
                   it behaves identically inside VICE while leaving the
                   existing adaptive thresholds in place for every other
                   joystick. */

                bool is_thec64 = (device->vendor_id == 0x1c59 && device->product_id == 0x0023);
                if (is_thec64) {
                    if (value < 127) {
                        joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_NEGATIVE);
                    } else if (value > 127) {
                        joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_POSITIVE);
                    } else {
                        joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_MIDDLE);
                    }
                    /* Done – go on to next HID element. */
                    continue;
                }

                int range = e->max_pvalue - e->min_pvalue;
                int low_threshold, high_threshold;
                if (range == 0) {
                    if (device->vendor_id == 0x1c59 && device->product_id == 0x0023) {
                        low_threshold = 120;
                        high_threshold = 135;
                    } else {
                        low_threshold = e->min_pvalue - 1;
                        high_threshold = e->max_pvalue + 1;
                    }
                } else {
                    low_threshold = (e->min_pvalue + e->max_pvalue) / 2 - range/4;
                    high_threshold = (e->min_pvalue + e->max_pvalue) / 2 + range/4;
                }
                if (value < low_threshold) {
                    joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_NEGATIVE);
                } else if (value > high_threshold) {
                    joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_POSITIVE);
                } else {
                    joy_axis_event(core_dev->axes[e->ordinal], JOY_AXIS_MIDDLE);
                }
            } else if (e->usage_page == kHIDPage_Button) {
                int button_index = e->ordinal;
                if (device->vendor_id == 0x1c59 && device->product_id == 0x0023 && e->usage == 2) {
                    /* Map THEC64 second fire button to the same core button 0 */
                    button_index = 0;
                }
                if (core_dev->buttons && button_index < core_dev->num_buttons) {
                    if (is_thec64_device) {
                        /* Collect state – use logical OR so either physical
                           button keeps the aggregated state high. */
                        if (value) {
                            aggregated_button_state[button_index] = 1;
                        }
                    } else {
                        /* Normal path for all other devices */
                        joy_button_event(core_dev->buttons[button_index], value);
                    }
                }
            } else if (e->usage_page == kHIDPage_GenericDesktop) {
                switch(e->usage) {
                    case kHIDUsage_GD_Hatswitch:
                        if (value >= 0 && value <= MAX_HAT_MAP_INDEX) {
                            if (core_dev->hats && e->ordinal < core_dev->num_hats) {
                                joy_hat_event(core_dev->hats[e->ordinal], hat_map[value]);
                            }
                        }
                        break;
                }
            }
        }
    }
    /* Emit aggregated button events for THEC64 after processing all elements */
    if (is_thec64_device && core_dev->buttons) {
        for (int b = 0; b < core_dev->num_buttons && b < 32; b++) {
            joy_button_event(core_dev->buttons[b], aggregated_button_state[b]);
        }
    }
    safe_mutex_unlock();
}

static void osx_joystick_close(joystick_device_t *core_dev) {
    log_message(LOG_DEFAULT, "osx_joystick_close called for core_dev %p, priv %p", (void*)core_dev, core_dev ? (void*)core_dev->priv : NULL);
    if (core_dev == NULL) {
        return;
    }
    if (safe_mutex_lock() != 0) {
        return;
    }
    if (core_dev->priv == NULL) {
        safe_mutex_unlock();
        return;
    }
    joy_hid_device_t *device = (joy_hid_device_t *)core_dev->priv;
    if (device->freed) {
        safe_mutex_unlock();
        return;
    }
    log_message(LOG_DEFAULT, "joy-hid: Closing I/O handle for device %s", device->product_name ? device->product_name : "unknown");
    if (device->opened_io_handle != NULL) {
        IOHIDDeviceClose(device->opened_io_handle, kIOHIDOptionsTypeNone);
        device->opened_io_handle = NULL;
    }
    // Do not free elements or product_name; leave cleanup to process exit.
    device->freed = 1;
    safe_mutex_unlock();
}

/* ---------------------------------------------------------------------
 *  priv_free: invoked by joystick_device_free() to release per-device
 *             HID resources that are not owned by the common layers.
 * ---------------------------------------------------------------------*/
static void osx_joystick_priv_free(void *priv)
{
    joy_hid_device_t *device = (joy_hid_device_t *)priv;
    if (!device) {
        return;
    }

    /* Make really sure the device is closed. */
    if (device->opened_io_handle != NULL) {
        IOHIDDeviceClose(device->opened_io_handle, kIOHIDOptionsTypeNone);
        device->opened_io_handle = NULL;
    }

    if (device->elements_cfarray) {
        CFRelease(device->elements_cfarray);
        device->elements_cfarray = NULL;
    }

    if (device->elements) {
        lib_free(device->elements);
        device->elements = NULL;
    }

    if (device->product_name && strcmp(device->product_name, "N/A") != 0) {
        free(device->product_name);
        device->product_name = NULL;
    }

    lib_free(device);
}

/* ---------------------------------------------------------------------
 *  open: called when user assigns the device to an emulated port
 * ---------------------------------------------------------------------*/
static bool osx_joystick_open(joystick_device_t *core_dev)
{
    if (!core_dev || !core_dev->priv) {
        return false;
    }
    joy_hid_device_t *device = (joy_hid_device_t *)core_dev->priv;
    /* Just make sure we can open the HID for subsequent polling */
    return ensure_device_open_for_io(device);
}

static joystick_driver_t osx_joystick_driver = {
    .open      = osx_joystick_open,
    .poll      = osx_joystick_read,
    .close     = osx_joystick_close,
    .priv_free = osx_joystick_priv_free
};

static CFDictionaryRef CreateHIDDeviceMatchDictionary(const int page, const int usage)
{
    CFDictionaryRef retval = NULL;
    CFNumberRef pageNumRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
    CFNumberRef usageNumRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
    const void *keys[2] = { (void *) CFSTR(kIOHIDDeviceUsagePageKey), (void *) CFSTR(kIOHIDDeviceUsageKey) };
    const void *vals[2] = { (void *) pageNumRef, (void *) usageNumRef };

    if (pageNumRef && usageNumRef) {
        retval = CFDictionaryCreate(kCFAllocatorDefault, keys, vals, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    }

    if (pageNumRef) {
        CFRelease(pageNumRef);
    }
    if (usageNumRef) {
        CFRelease(usageNumRef);
    }

    return retval;
}

/* ----- API ----- */

void joy_hidlib_init(void)
{
    // Register the driver once
    // Check if it's already registered by another part of the system if necessary,
    // but for now, assume this is the sole registration point for this driver.
    static bool driver_registered = false;
    if (!driver_registered) {
        joystick_driver_register(&osx_joystick_driver);
        driver_registered = true;
    }

    if (!mgr) {
        /* create the manager */
        mgr = IOHIDManagerCreate(kCFAllocatorDefault, 0L);
    }
    if (!mgr) {
        return;
    }

    {
        const void *vals[] = {
            (void *) CreateHIDDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick),
            (void *) CreateHIDDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad),
            (void *) CreateHIDDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_MultiAxisController),
        };
        CFArrayRef array = CFArrayCreate(kCFAllocatorDefault, vals, 3, &kCFTypeArrayCallBacks);

        IOHIDManagerSetDeviceMatchingMultiple(mgr, array);

        CFRelease(array);
        CFRelease(vals[0]);
        CFRelease(vals[1]);
        CFRelease(vals[2]);
    }

    /* open it */
    IOReturn tIOReturn = IOHIDManagerOpen(mgr, 0L);
    if (kIOReturnSuccess != tIOReturn) {
        return;
    }

    /* create set of devices */
    CFSetRef device_set = IOHIDManagerCopyDevices(mgr);
    if (!device_set) {
        return;
    }

    int i;
    CFIndex num_devices = CFSetGetCount(device_set);
    int axes;
    int buttons;
    int hats;
    
    // Check for empty device set
    if (num_devices == 0) {
        CFRelease(device_set);
        return;
    }

    IOHIDDeviceRef *all_devices = lib_malloc(sizeof(IOHIDDeviceRef) * num_devices);
    if (!all_devices) {
        CFRelease(device_set);
        return;
    }
    
    CFSetGetValues(device_set, (const void **)all_devices);

    joy_hid_device_t *d_hid; // Renamed from 'd' to avoid conflict with joystick_device_t
    for (i = 0; i < num_devices; i++) {
        IOHIDDeviceRef dev_ref = all_devices[i]; // Renamed from 'dev'
        if (is_joystick(dev_ref)) {
            long vendor_id = 0;
            long product_id = 0;
            char *product_name_str = "N/A"; // Renamed from product_name
            
            // Get device properties safely
            if (!IOHIDDevice_GetLongProperty(dev_ref, CFSTR(kIOHIDVendorIDKey), &vendor_id) ||
                !IOHIDDevice_GetLongProperty(dev_ref, CFSTR(kIOHIDProductIDKey), &product_id)) {
                // Skip devices with invalid properties
                continue;
            }
            
            CFStringRef product_key = IOHIDDeviceGetProperty(dev_ref, CFSTR(kIOHIDProductKey));
            if (product_key) {
                char buffer[256];
                if (CFStringGetCString(product_key, buffer, 256, kCFStringEncodingUTF8)) {
                    product_name_str = strdup(buffer);
                }
            }

            d_hid = lib_malloc(sizeof(joy_hid_device_t));
            if (!d_hid) {
                if (product_name_str && strcmp(product_name_str, "N/A") != 0) free(product_name_str);
                continue;
            }
            
            // Initialize to safe values
            memset(d_hid, 0, sizeof(joy_hid_device_t));

            d_hid->last_failed_open_attempt_time = 0; // No failed attempts yet
            
            d_hid->persistent_iohid_ref = dev_ref;
            d_hid->opened_io_handle = NULL;

            // Get vendor/product ID using the persistent_iohid_ref (needs to be open for this)
            // Temporarily open to get essential properties for identification and element enum
            IOReturn temp_open_ret = IOHIDDeviceOpen(d_hid->persistent_iohid_ref, kIOHIDOptionsTypeNone);
            if (temp_open_ret != kIOReturnSuccess && temp_open_ret != kIOReturnStillOpen) {
                log_message(LOG_DEFAULT, "joy-hid: Failed to temporarily open %p for props (err %d)", (void *)dev_ref, temp_open_ret);
                lib_free(d_hid);
                continue;
            }
            
            // Use the temporarily opened ref for getting properties
            IOHIDDevice_GetLongProperty(d_hid->persistent_iohid_ref, CFSTR(kIOHIDVendorIDKey), &vendor_id);
            IOHIDDevice_GetLongProperty(d_hid->persistent_iohid_ref, CFSTR(kIOHIDProductIDKey), &product_id);
            if (product_key) {
                char buffer[256];
                if (CFStringGetCString(product_key, buffer, 256, kCFStringEncodingUTF8)) {
                    d_hid->product_name = strdup(buffer);
                }
            }

            d_hid->vendor_id = (int)vendor_id;
            d_hid->product_id = (int)product_id;

            // Now that we have basic info, assign the opened handle for enumeration
            d_hid->opened_io_handle = d_hid->persistent_iohid_ref; 
            joy_hidlib_enumerate_elements(d_hid, &axes, &buttons, &hats);
            // After enumeration, close the handle. It will be re-opened by ensure_device_open_for_io when needed.
            if (d_hid->opened_io_handle) {
                IOHIDDeviceClose(d_hid->opened_io_handle, kIOHIDOptionsTypeNone);
                d_hid->opened_io_handle = NULL;
            }
            
            if (d_hid->num_elements == 0) { // Enumeration failed or no useful elements
                log_message(LOG_DEFAULT, "joy-hid: No usable elements found for device VID:0x%04X PID:0x%04X (%s), skipping.", (unsigned int)d_hid->vendor_id, (unsigned int)d_hid->product_id, d_hid->product_name ? d_hid->product_name : "N/A");
                /* Do not free d_hid here; joystick_device_free (if called) will own it */
                /* skip manual frees to avoid double free */
                continue;
            }

            joystick_device_t *core_dev = joystick_device_new();
            if (!core_dev) {
                /* core_dev allocation failed: clean up private resources */
                if (d_hid->elements) {
                    lib_free(d_hid->elements);
                    d_hid->elements = NULL;
                }
                if (d_hid->elements_cfarray) {
                    CFRelease(d_hid->elements_cfarray);
                    d_hid->elements_cfarray = NULL;
                }
                if (d_hid->product_name && strcmp(d_hid->product_name, "N/A") != 0) {
                    free(d_hid->product_name);
                    d_hid->product_name = NULL;
                }
                lib_free(d_hid);
                joy_hidlib_free_elements(d_hid);
                /* Avoid double-free: only free core_dev which owns d_hid */
                joystick_device_free(core_dev);
                continue;
            }

            joystick_device_set_name(core_dev, d_hid->product_name);
            joystick_device_set_node(core_dev, d_hid->product_name);
            core_dev->priv = d_hid;

            // Populate axes, buttons, hats for the core device
            for (int j = 0; j < d_hid->num_elements; ++j) {
                joy_hid_element_t *hid_elem = &d_hid->elements[j];
                if (hid_elem->ordinal < 0) continue;

                char elem_name_buf[128];

                if (hid_elem->usage_page == kHIDPage_GenericDesktop) {
                    switch (hid_elem->usage) {
                        case kHIDUsage_GD_X:
                        case kHIDUsage_GD_Y:
                        case kHIDUsage_GD_Z:
                        case kHIDUsage_GD_Rx:
                        case kHIDUsage_GD_Ry:
                        case kHIDUsage_GD_Rz:
                        case kHIDUsage_GD_Slider:
                            /* Accept axes even when min == max for TheC64 (reports digital) */
                            if (hid_elem->min_pvalue != hid_elem->max_pvalue ||
                                (d_hid->vendor_id == 0x1c59 && d_hid->product_id == 0x0023)) {
                                snprintf(elem_name_buf, sizeof(elem_name_buf), "Axis %d (Usage 0x%X)", hid_elem->ordinal, (unsigned int)hid_elem->usage);
                                joystick_axis_t *core_axis = joystick_axis_new(elem_name_buf);
                                if (core_axis) {
                                    core_axis->code  = hid_elem->usage;
                                    core_axis->index = hid_elem->ordinal;
                                    if (d_hid->vendor_id == 0x1c59 && d_hid->product_id == 0x0023) {
                                        /* Fabricate a small range and flag as digital */
                                        core_axis->minimum = 0;
                                        core_axis->maximum = 1;
                                        core_axis->digital = true;
                                    } else {
                                        core_axis->minimum = hid_elem->min_lvalue;
                                        core_axis->maximum = hid_elem->max_lvalue;
                                    }
                                    joystick_device_add_axis(core_dev, core_axis);
                                }
                            }
                            break;
                        case kHIDUsage_GD_DPadUp:
                        case kHIDUsage_GD_DPadDown:
                        case kHIDUsage_GD_DPadRight:
                        case kHIDUsage_GD_DPadLeft:
                            snprintf(elem_name_buf, sizeof(elem_name_buf), "DPad %d (Usage 0x%X)", hid_elem->ordinal, (unsigned int)hid_elem->usage);
                            joystick_axis_t *core_axis = joystick_axis_new(elem_name_buf);
                            if (core_axis) {
                                core_axis->code = hid_elem->usage;
                                core_axis->index = hid_elem->ordinal;
                                core_axis->minimum = 0;
                                core_axis->maximum = 1;
                                joystick_device_add_axis(core_dev, core_axis);
                            }
                            break;
                        case kHIDUsage_GD_Hatswitch:
                            snprintf(elem_name_buf, sizeof(elem_name_buf), "Hat %d", hid_elem->ordinal);
                            joystick_hat_t *core_hat = joystick_hat_new(elem_name_buf);
                            if (core_hat) {
                                core_hat->code = hid_elem->usage;
                                core_hat->index = hid_elem->ordinal;
                                joystick_device_add_hat(core_dev, core_hat);
                            }
                            break;
                    }
                } else if (hid_elem->usage_page == kHIDPage_Button) {
                    if (d_hid->vendor_id == 0x1c59 && d_hid->product_id == 0x0023 && hid_elem->usage == 2) {
                        /* Skip creating a second logical button; usage 2 will be mapped to button 0 at poll time */
                        continue;
                    }
                    snprintf(elem_name_buf, sizeof(elem_name_buf), "Button %d", hid_elem->ordinal);
                    joystick_button_t *core_button = joystick_button_new(elem_name_buf);
                    if (core_button) {
                        core_button->code = hid_elem->usage;
                        core_button->index = hid_elem->ordinal;
                        joystick_device_add_button(core_dev, core_button);
                    }
                }
            }
            // After adding all elements, their indices in core_dev->axes/buttons/hats should match hid_elem->ordinal

            bool register_success = joystick_device_register(core_dev);
            
            if (register_success) {
                // If this is TheC64 joystick, send a dummy button press/release to show activity
                if (d_hid->vendor_id == 0x1c59 && d_hid->product_id == 0x0023 && core_dev->num_buttons > 0) {
                    joy_button_event(core_dev->buttons[0], 1);
                    joy_button_event(core_dev->buttons[0], 0);
                    log_message(LOG_DEFAULT, "joy-hid: Sent dummy button event for TheC64 to keep device active");
                }
            } else {
                /* register_success == false cleanup */
                joy_hidlib_free_elements(d_hid);
                joystick_device_free(core_dev);
            }
        }
    }
    
    lib_free(all_devices);
    CFRelease(device_set);
}

void joy_hidlib_exit(void)
{
    log_message(LOG_DEFAULT, "joy-hid: exiting HID library");
    
    if (mutex_initialized) {
        // Ensure mutex is unlocked before exit
        safe_mutex_lock();
        safe_mutex_unlock();
        
        // Clean up the mutex
        int result = pthread_mutex_destroy(&hid_mutex);
        if (result != 0) {
            log_message(LOG_DEFAULT, "joy-hid: mutex destroy failed: %d", result);
        }
        mutex_initialized = 0;
    }
}

static void setupC64JoystickEventHandlers(joy_hid_device_t *device) {
    // Stub: No-op for now
    (void)device;
}
