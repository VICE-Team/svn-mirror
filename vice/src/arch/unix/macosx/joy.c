/*
 * macosx/joy.c - Mac OS X joystick support.
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

#include "cmdline.h"
#include "joy.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"

#ifdef HAS_JOYSTICK

/* ----- Static Data ------------------------------------------------------ */

static int joy_done_init = 0;

/* number of joyports and extra joyports */
int joy_num_ports;
int joy_num_extra_ports;

static joy_hid_dev_t joy_hid_dev_a;
static joy_hid_dev_t joy_hid_dev_b;

/* the driver holds up to two USB joystick definitions */
joystick_descriptor_t joy_a = { .hid = &joy_hid_dev_a };
joystick_descriptor_t joy_b = { .hid = &joy_hid_dev_b };

/* ----- VICE Resources --------------------------------------------------- */

static void setup_axis_mapping(joystick_descriptor_t *joy);
static void setup_button_mapping(joystick_descriptor_t *joy);
static void setup_auto(void);

static int joyport1select(int val, void *param)
{
    joystick_port_map[0] = val;
    return 0;
}

static int joyport2select(int val, void *param)
{
    joystick_port_map[1] = val;
    return 0;
}

static int joyport3select(int val, void *param)
{
    joystick_port_map[2] = val;
    return 0;
}

static int joyport4select(int val, void *param)
{
    joystick_port_map[3] = val;
    return 0;
}

static int set_joy_a_device_name(const char *val,void *param)
{
    util_string_set(&joy_a.device_name, val);
    if (joy_done_init) {
        setup_auto();
    }
    return 0;
}

static int set_joy_a_x_axis_name(const char *val,void *param)
{
    util_string_set(&joy_a.axis[HID_X_AXIS].name, val);
    if (joy_done_init) {
        setup_axis_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_a_y_axis_name(const char *val,void *param)
{
    util_string_set(&joy_a.axis[HID_Y_AXIS].name, val);
    if (joy_done_init) {
        setup_axis_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_a_button_mapping(const char *val,void *param)
{
    util_string_set(&joy_a.button_mapping, val);
    if (joy_done_init) {
        setup_button_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_a_x_threshold(int val, void *param)
{
    joy_a.axis[HID_X_AXIS].threshold = val;
    if (joy_done_init) {
        setup_axis_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_a_y_threshold(int val, void *param)
{
    joy_a.axis[HID_Y_AXIS].threshold = val;
    if (joy_done_init) {
        setup_axis_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_b_device_name(const char *val,void *param)
{
    util_string_set(&joy_b.device_name, val);
    if (joy_done_init) {
        setup_auto();
    }
    return 0;
}

static int set_joy_b_x_axis_name(const char *val,void *param)
{
    util_string_set(&joy_b.axis[HID_X_AXIS].name, val);
    if (joy_done_init) {
        setup_axis_mapping(&joy_b);
    }
    return 0;
}

static int set_joy_b_y_axis_name(const char *val,void *param)
{
    util_string_set(&joy_b.axis[HID_Y_AXIS].name, val);
    if (joy_done_init) {
        setup_axis_mapping(&joy_b);
    }
    return 0;
}

static int set_joy_b_button_mapping(const char *val,void *param)
{
    util_string_set(&joy_b.button_mapping, val);
    if (joy_done_init) {
        setup_button_mapping(&joy_a);
    }
    return 0;
}

static int set_joy_b_x_threshold(int val, void *param)
{
    joy_b.axis[HID_X_AXIS].threshold = val;
    if (joy_done_init) {
        setup_axis_mapping(&joy_b);
    }
    return 0;
}

static int set_joy_b_y_threshold(int val, void *param)
{
    joy_b.axis[HID_Y_AXIS].threshold = val;
    if (joy_done_init) {
        setup_axis_mapping(&joy_b);
    }
    return 0;
}

static const resource_string_t resources_string[] = {
    { "JoyADevice", "", RES_EVENT_NO, NULL,
      &joy_a.device_name, set_joy_a_device_name, NULL },
    { "JoyAXAxis", "X", RES_EVENT_NO, NULL,
      &joy_a.axis[HID_X_AXIS].name, set_joy_a_x_axis_name, NULL },
    { "JoyAYAxis", "Y", RES_EVENT_NO, NULL,
      &joy_a.axis[HID_Y_AXIS].name, set_joy_a_y_axis_name, NULL },
    { "JoyAButtons", "1:2:0:0:0:0", RES_EVENT_NO, NULL,
      &joy_a.button_mapping, set_joy_a_button_mapping, NULL },
    { "JoyBDevice", "", RES_EVENT_NO, NULL,
      &joy_b.device_name, set_joy_b_device_name, NULL },
    { "JoyBXAxis", "X", RES_EVENT_NO, NULL,
      &joy_b.axis[HID_X_AXIS].name, set_joy_b_x_axis_name, NULL },
    { "JoyBYAxis", "Y", RES_EVENT_NO, NULL,
      &joy_b.axis[HID_Y_AXIS].name, set_joy_b_y_axis_name, NULL },
    { "JoyBButtons", "1:2:0:0:0:0", RES_EVENT_NO, NULL,
      &joy_b.button_mapping, set_joy_b_button_mapping, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "JoyDevice1", 0, RES_EVENT_NO, NULL,
      &joystick_port_map[0], joyport1select, NULL },
    { "JoyDevice2", 0, RES_EVENT_NO, NULL,
      &joystick_port_map[1], joyport2select, NULL },
    { "JoyDevice3", 0, RES_EVENT_NO, NULL,
      &joystick_port_map[2], joyport3select, NULL },
    { "JoyDevice4", 0, RES_EVENT_NO, NULL,
      &joystick_port_map[3], joyport4select, NULL },
    { "JoyAXThreshold", 50, RES_EVENT_NO, NULL,
      &joy_a.axis[HID_X_AXIS].threshold, set_joy_a_x_threshold, NULL },
    { "JoyAYThreshold", 50, RES_EVENT_NO, NULL,
      &joy_a.axis[HID_Y_AXIS].threshold, set_joy_a_y_threshold, NULL },
    { "JoyBXThreshold", 50, RES_EVENT_NO, NULL,
      &joy_b.axis[HID_X_AXIS].threshold, set_joy_b_x_threshold, NULL },
    { "JoyBYThreshold", 50, RES_EVENT_NO, NULL,
      &joy_b.axis[HID_Y_AXIS].threshold, set_joy_b_y_threshold, NULL },
    { NULL }
};

/* ----- VICE Command-line options ----- */

static const cmdline_option_t cmdline_options[] = {
    { "-joyAdevice", SET_RESOURCE, 1,
      NULL, NULL, "JoyADevice", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<vid:pid:sn>", N_("Set HID A device") },
    { "-joyAxaxis", SET_RESOURCE, 1,
      NULL, NULL, "JoyAXAxis", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<X,Y,Z,Rx,Ry,Rz>", N_("Set X Axis for HID A device") },
    { "-joyAyaxis", SET_RESOURCE, 1,
      NULL, NULL, "JoyAYAxis", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<X,Y,Z,Rx,Ry,Rz>", N_("Set Y Axis for HID A device") },
    { "-joyAbuttons", SET_RESOURCE, 1,
      NULL, NULL, "JoyAButtons", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<f:af:l:r:u:d>", N_("Set Y Axis for HID A device") },
    { "-joyAxthreshold", SET_RESOURCE, 1,
      NULL, NULL, "JoyAXThreshold", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-100>", N_("Set X Axis Threshold in Percent of HID A device") },
    { "-joyAythreshold", SET_RESOURCE, 1,
      NULL, NULL, "JoyAYThreshold", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-100>", N_("Set Y Axis Threshold in Percent of HID A device") },
    { "-joyBdevice", SET_RESOURCE, 1,
      NULL, NULL, "JoyBDevice", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<vid:pid:sn>", N_("Set HID B device") },
    { "-joyBxaxis", SET_RESOURCE, 1,
      NULL, NULL, "JoyBXAxis", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<X,Y,Z,Rx,Ry,Rz>", N_("Set X Axis for HID B device") },
    { "-joyByaxis", SET_RESOURCE, 1,
      NULL, NULL, "JoyBYAxis", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<X,Y,Z,Rx,Ry,Rz>", N_("Set Y Axis for HID B device") },
    { "-joyBbuttons", SET_RESOURCE, 1,
      NULL, NULL, "JoyBButtons", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<f:af:l:r:u:d>", N_("Set Y Axis for HID B device") },
    { "-joyBxthreshold", SET_RESOURCE, 1,
      NULL, NULL, "JoyBXThreshold", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-100>", N_("Set X Axis Threshold in Percent of HID B device") },
    { "-joyBythreshold", SET_RESOURCE, 1,
      NULL, NULL, "JoyBYThreshold", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-100>", N_("Set Y Axis Threshold in Percent of HID B device") },
    { NULL },
};

static const cmdline_option_t joydev1cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice1", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-5>", N_("Set device for joystick port 1") },
    { NULL },
};

static const cmdline_option_t joydev2cmdline_options[] = {
    { "-joydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice2", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-5>", N_("Set device for joystick port 2") },
    { NULL },
};

static const cmdline_option_t joydev3cmdline_options[] = {
    { "-extrajoydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice3", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-5>", N_("Set device for extra joystick port 1") },
    { NULL },
};

static const cmdline_option_t joydev4cmdline_options[] = {
    { "-extrajoydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice4", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-5>", N_("Set device for extra joystick port 2") },
    { NULL },
};

int joystick_arch_init_resources(void)
{
    int ok = resources_register_string(resources_string);
    if (ok < 0) {
        return ok;
    }
    return resources_register_int(resources_int);
}

int joystick_init_cmdline_options(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            joy_num_ports = 2;
            joy_num_extra_ports = 2;
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            joy_num_ports = 0;
            joy_num_extra_ports = 2;
            break;
        case VICE_MACHINE_CBM5x0:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            joy_num_ports = 2;
            joy_num_extra_ports = 0;
            break;
        case VICE_MACHINE_PLUS4:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            joy_num_ports = 2;
            joy_num_extra_ports = 1;
            break;
        case VICE_MACHINE_VIC20:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            joy_num_ports = 1;
            joy_num_extra_ports = 2;
            break;
    }
    return cmdline_register_options(cmdline_options);
}

/* ----- Setup Joystick Descriptor ---------------------------------------- */

void joy_calc_threshold(int min, int max, int threshold, int *t_min, int *t_max)
{
    int range = max - min;;
    int safe  = range * threshold / 200;
    *t_min = min + safe;
    *t_max = max - safe;
}

static void setup_axis_calibration(joy_axis_t *axis, const char *desc)
{
    joy_calc_threshold(axis->min_value, axis->max_value, axis->threshold,
                       &axis->min_threshold, &axis->max_threshold);

    log_message(LOG_DEFAULT, "mac_joy: %s axis: mapped to %s with range=[%d;%d] and null=[%d;%d] from threshold=%d%%",
        desc, axis->name, 
        axis->min_value, axis->max_value, 
        axis->min_threshold, axis->max_threshold,
        axis->threshold);
}

static void setup_axis_mapping(joystick_descriptor_t *joy)
{
    /* extract tag from resource */
    int x_ok = joy_hid_map_axis(joy, HID_X_AXIS);
    int y_ok = joy_hid_map_axis(joy, HID_Y_AXIS);

    /* setup calibration for axis */
    if (x_ok) {
        setup_axis_calibration(&joy->axis[HID_X_AXIS], "horizontal");
    } else {
        log_message(LOG_DEFAULT, "mac_joy: horizontal axis not mapped!");
    }
    if (y_ok) {
        setup_axis_calibration(&joy->axis[HID_Y_AXIS], "vertical");
    } else {
        log_message(LOG_DEFAULT, "mac_joy: vertical axis not mapped!");
    }
}

static void setup_button_mapping(joystick_descriptor_t *joy)
{
    int i;
    int ids[HID_NUM_BUTTONS];
    
    /* preset button id */
    for(i = 0 ; i < HID_NUM_BUTTONS; i++) {
        ids[i] = (i<2) ? i+1 : HID_INVALID_BUTTON;
    }

    /* decode button mapping resource */
    if (joy->button_mapping && strlen(joy->button_mapping) > 0) {
        if (sscanf(joy->button_mapping, "%d:%d:%d:%d:%d:%d", &ids[0], &ids[1], &ids[2], &ids[3], &ids[4], &ids[5]) != 6) {
            log_message(LOG_DEFAULT, "mac_joy: invalid button mapping!");
        }
    }
 
    /* try to map buttons in HID device */
    for (i = 0; i < HID_NUM_BUTTONS; i++) {
        joy->buttons[i].id = ids[i];
        if(ids[i] != HID_INVALID_BUTTON) {
            if( joy_hid_map_button(joy, i) == 0 ) {
                log_message(LOG_DEFAULT, "mac_joy: can't map button %d!", i);
            }
        } 
    }
    
    /* show button mapping */
    log_message(LOG_DEFAULT, "mac_joy: buttons: fire=%d alt_fire=%d left=%d right=%d up=%d down=%d",
        ids[HID_FIRE], ids[HID_ALT_FIRE], ids[HID_LEFT], ids[HID_RIGHT], ids[HID_UP], ids[HID_DOWN]);
}

/* determine if the given device matches the joystick descriptor */
static int match_joystick(joystick_descriptor_t *joy, joy_hid_device_t *dev)
{
    /* match by device name */
    if (joy->device_name && strlen(joy->device_name) > 0) {
        int vid, pid, serial;
        if (sscanf(joy->device_name, "%x:%x:%d", &vid, &pid, &serial) != 3) {
            return 0;
        }
        return (vid == dev->vendor_id) && (pid == dev->product_id) && (dev->serial == serial);
    }
    /* no match */
    return 0;
}

static void setup_joystick(joystick_descriptor_t *joy, joy_hid_device_t *dev, const char *desc)
{
    if(joy_hid_map_device(joy, dev) == 1) {
        log_message(LOG_DEFAULT, "mac_joy: set up %s HID joystick", desc);
        setup_axis_mapping(joy);
        setup_button_mapping(joy);
    } else {
        log_message(LOG_DEFAULT, "mac_joy: ERROR setting up %s HID joystick", desc);
    }
}

/* is the joystick auto assignable? */
static int do_auto_assign(joystick_descriptor_t *joy)
{
    return ((joy->device_name == NULL) || (strlen(joy->device_name) == 0));
}

static void setup_auto(void)
{
    int auto_assign_a = do_auto_assign(&joy_a);
    int auto_assign_b = do_auto_assign(&joy_b);
    int i;
    int num_devices;
    joy_hid_device_t *devices;

    /* unmap both joysticks */
    joy_a.mapped = 0;
    joy_b.mapped = 0;
    
    /* query device list */
    num_devices = joy_hid_enumerate_devices(&devices);
    if(num_devices == 0) {
        log_message(LOG_DEFAULT, "mac_joy: can't find any HID devices!");
        return;
    }
    
    /* walk through all enumerated devices */
    for(i=0;i<num_devices;i++) {
        joy_hid_device_t *dev = &devices[i];
        
        log_message(LOG_DEFAULT, "mac_joy: found #%d joystick/gamepad: %04x:%04x:%d %s",
                    i, dev->vendor_id, dev->product_id, dev->serial, dev->product_name);
        
        /* query joy A */
        if (!auto_assign_a && match_joystick(&joy_a, dev)) {
            setup_joystick(&joy_a, dev, "matched A");
        }
        /* query joy B */
        else if (!auto_assign_b && match_joystick(&joy_b, dev)) {
            setup_joystick(&joy_b, dev, "matched B");
        }
        /* auto assign a */
        else if (auto_assign_a && (joy_a.mapped == 0)) {
            setup_joystick(&joy_a, dev, "auto-assigned A");
        }
        /* auto assign b */
        else if (auto_assign_b && (joy_b.mapped == 0)) {
            setup_joystick(&joy_b, dev, "auto-assigned B");
        }
    }
    
    joy_hid_free_devices(num_devices, devices);

    /* check if matched */
    if (!auto_assign_a && (joy_a.mapped == 0)) {
        log_message(LOG_DEFAULT, "mac_joy: joystick A not matched!");
    }
    if (!auto_assign_b && (joy_b.mapped == 0)) {
        log_message(LOG_DEFAULT, "mac_joy: joystick B not matched!");
    }   
}

/* helper for UI to reload device list */
void joy_reload_device_list(void)
{
    joy_hid_unload_device_list();
    if (joy_hid_load_device_list() > 0) {
        setup_auto();
    }
}

/* query for available joysticks and set them up */
int joy_arch_init(void)
{
    if (joy_hid_load_device_list() == 0) {
        return 0;
    }

    /* now assign HID joystick A,B if available */
    setup_auto();

    joy_done_init = 1;

    return 0;
}

/* close the device */
void joystick_close(void)
{
    joy_hid_unload_device_list();
}

/* ----- Read Joystick ----- */

static BYTE read_button(joystick_descriptor_t *joy, int id, BYTE resValue)
{
    /* button not mapped? */
    if(joy->buttons[id].mapped == 0)
        return 0;
        
    return joy_hid_read_button(joy, id) ? resValue : 0;
}

static BYTE read_axis(joystick_descriptor_t *joy, int axis, BYTE min, BYTE max)
{
    if(joy->axis[axis].mapped == 0)
        return 0;

    int value = joy_hid_read_axis(joy, axis);
    if(value < 0)
        return min;
    else if(value > 0)
        return max;
    else
        return 0;
}

static BYTE read_joystick(joystick_descriptor_t *joy)
{
    /* read buttons */
    BYTE joy_bits = read_button(joy, HID_FIRE, 16) |
                    read_button(joy, HID_ALT_FIRE, 16) |
                    read_button(joy, HID_LEFT, 4) |
                    read_button(joy, HID_RIGHT, 8) |
                    read_button(joy, HID_UP, 1) |
                    read_button(joy, HID_DOWN, 2);

    /* axis */
    joy_bits |= read_axis(joy, HID_X_AXIS, 4, 8) |
                read_axis(joy, HID_Y_AXIS, 1, 2);

    return joy_bits;
}

/* poll joystick */
void joystick(void)
{
    int i;

    /* handle both virtual cbm joystick ports */
    for (i = 0; i < 4; i++) {
        /* what kind of device is connected to the virtual port? */ 
        int joy_port = joystick_port_map[i];
    
        /* is HID joystick A mapped? */
        if (joy_port == JOYDEV_HID_0) {
            if (joy_a.mapped) {
                BYTE joy_bits = read_joystick(&joy_a);
                joystick_set_value_absolute(i + 1, joy_bits);
            }
        }
        /* is HID joystick B mapped? */
        else if (joy_port == JOYDEV_HID_1) {
            if (joy_b.mapped) {
                BYTE joy_bits = read_joystick(&joy_b);
                joystick_set_value_absolute(i + 1, joy_bits);
            }
        }
    }
}

#endif /* HAS_JOYSTICK */
