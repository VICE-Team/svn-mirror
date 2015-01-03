/*
 * joy.cc - Joystick support for BeOS
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *
 * FIXME: Due to some problems with Be's BJoystick implementation
 * the current implementation is quite ugly and should be rewritten
 * in the future. For now it works...
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

#include <Joystick.h>
#include <stdlib.h>
#include <String.h>

extern "C" {
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
#include "ui.h"
#include "util.h"
}

#if 0
/* Notice that these are never set.  */
static int keyset1[9], keyset2[9];
#endif

/* ------------------------------------------------------------------------ */

/* objects to access hardware devices */
static BJoystick *bjoystick[4];

/* stick index in the open BJoystick devices */ 
static int stick_nr[4];

/* axes-pair index in the open BJoystick devices */ 
static int axes_nr[4];

/* to allocate buffers for the current axes values */
static int16 *axes[4];

/* to avoid problems opening a BJoystick device twice, we have to remember them */
static int device_used_by[MAX_HARDWARE_JOYSTICK];

/* to check if the joystick module is already initialized */
static int joystick_initialized = 0;

/* array that holds all recognized hardware sticks */
hardware_joystick_t hardware_joystick[MAX_HARDWARE_JOYSTICK];
int hardware_joystick_count = 0;

#if 0
#define KEYSET_FIRE 0
#define KEYSET_SW   1
#define KEYSET_S    2
#define KEYSET_SE   3
#define KEYSET_W    4
#define KEYSET_E    5
#define KEYSET_NW   6
#define KEYSET_N    7
#define KEYSET_NE   8
#endif

static void joystick_close_device(int dev_index)
{
    int device_num;
    int used_by;
    int joy_dev = joystick_port_map[dev_index];

    if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES + hardware_joystick_count) {
        /* it's a hardware-stick; close the device if necessary */
        device_num = hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].device_num;
        used_by = device_used_by[device_num];
        device_used_by[device_num] &= ~(1 << dev_index);
        if (!device_used_by[device_num] && used_by) {
            bjoystick[used_by - 1]->Close();
            delete bjoystick[used_by - 1];
            free (axes[used_by - 1]);
            log_message(LOG_DEFAULT, "Joystick: Closed hardware %s as device %d",
            hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].device_name, dev_index + 1);
        }
    }
}

static void joystick_open_device(int dev_index)
{
    int used_by;
    int joy_dev = joystick_port_map[dev_index];

    if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {
        /* is the needed device already open? */
        int device_num = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_num;
            if (used_by = device_used_by[device_num]) {
                bjoystick[dev_index] = bjoystick[used_by - 1];
                axes[dev_index] = axes[used_by - 1];
                log_message(LOG_DEFAULT, "Joystick: Device %d uses hardware port already opened by device %d", dev_index + 1, used_by);
            } else {
                bjoystick[dev_index] = new BJoystick();
                if (bjoystick[dev_index]->Open(hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].device_name, true) == B_ERROR) {
                    log_message(LOG_DEFAULT, "Joystick: Warning, couldn't open hardware device %d", dev_index + 1);
                    return;
                } else {
                    log_message(LOG_DEFAULT, "Joystick: Opened hardware %s as device %d", hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].device_name, dev_index + 1);
            }
            axes[dev_index] = (int16*) malloc(sizeof(int16) * bjoystick[dev_index]->CountAxes());
        }
        device_used_by[device_num] |= (1 << dev_index);

        stick_nr[dev_index] = hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].stick; 
        axes_nr[dev_index] = hardware_joystick[joy_dev - NUM_OF_SOFTDEVICES].axes;
    }
}

static int set_joystick_device(int val, void *param)
{
    if (val < 0 || val > NUM_OF_SOFTDEVICES + MAX_HARDWARE_JOYSTICK) {
        return -1;
    }

    if (joystick_initialized) {
        joystick_close_device(*((int*)(&param)));
    }

    joystick_port_map[*((int*)(&param))] = (joystick_device_t)val;

    if (joystick_initialized) {
        joystick_open_device(*((int*)(&param)));
    }

    return 0;
}

static const resource_int_t joy1_resources_int[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      (int *)&joystick_port_map[0], set_joystick_device, (void *)0 },
    { NULL }
};

static const resource_int_t joy2_resources_int[] = {
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      (int *)&joystick_port_map[1], set_joystick_device, (void *)1 },
    { NULL }
};

static const resource_int_t joy3_resources_int[] = {
    { "JoyDevice3", JOYDEV_NONE, RES_EVENT_NO, NULL,
      (int *)&joystick_port_map[2], set_joystick_device, (void *)2 },
    { NULL }
};

static const resource_int_t joy4_resources_int[] = {
    { "JoyDevice4", JOYDEV_NONE, RES_EVENT_NO, NULL,
      (int *)&joystick_port_map[3], set_joystick_device, (void *)3 },
    { NULL }
};

int joystick_arch_init_resources(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_C64:
        case VICE_MACHINE_SCPU64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
        case VICE_MACHINE_CBM5x0:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy2_resources_int);
            break;
        case VICE_MACHINE_PLUS4:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy3_resources_int);
            break;
        case VICE_MACHINE_VIC20:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
    }
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t joydev1cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice1", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for joystick #1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4..19: Hardware joysticks)" },
    { NULL }
};

static const cmdline_option_t joydev2cmdline_options[] = {
    { "-joydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice2", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for joystick #2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4..19: Hardware joysticks)" },
    { NULL }
};

static const cmdline_option_t joydev3cmdline_options[] = {
    { "-extrajoydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice3", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for extra joystick #1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4..19: Hardware joysticks)" },
    { NULL }
};

static const cmdline_option_t joydev4cmdline_options[] = {
    { "-extrajoydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice4", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for extra joystick #2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4..19: Hardware joysticks)" },
    { NULL }
};

int joystick_arch_cmdline_options_init(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_C64:
        case VICE_MACHINE_SCPU64:
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
            return cmdline_register_options(joydev4cmdline_options);
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev4cmdline_options);
            break;
        case VICE_MACHINE_CBM5x0:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev2cmdline_options);
            break;
        case VICE_MACHINE_PLUS4:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev3cmdline_options);
            break;
        case VICE_MACHINE_VIC20:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev4cmdline_options);
            break;
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

int joy_arch_init(void)
{
    BJoystick testbjoystick;
    char current_devicename[B_OS_NAME_LENGTH];
    int device_iter, stick_iter, axes_iter;
    int device_count;

    if (joystick_initialized) {
        return 0;
    }

    device_count = testbjoystick.CountDevices();
    for (device_iter = 0; device_iter < device_count; device_iter++) {
        device_used_by[device_iter] = 0;
        testbjoystick.GetDeviceName(device_iter, current_devicename);

        if (testbjoystick.Open(current_devicename, true) != B_ERROR) {
            for (stick_iter = 0; stick_iter < testbjoystick.CountSticks(); stick_iter++) {
                for (axes_iter = 0; axes_iter * 2 < testbjoystick.CountAxes(); axes_iter++) {
                    strcpy(hardware_joystick[hardware_joystick_count].device_name, current_devicename);
                    hardware_joystick[hardware_joystick_count].device_num = device_iter;
                    hardware_joystick[hardware_joystick_count].stick = stick_iter;
                    hardware_joystick[hardware_joystick_count++].axes = axes_iter;

                    if (hardware_joystick_count >= MAX_HARDWARE_JOYSTICK) {
                        return 0;
                    }
                }
            }
            testbjoystick.Close();
        }
    }
    joystick_initialized = 1;
    joystick_open_device(0);
    joystick_open_device(1);
    joystick_open_device(2);
    joystick_open_device(3);

    return 0;
}

int joystick_close(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (device_used_by[i]) {
            delete bjoystick[device_used_by[i] >> 1];
            free(axes[device_used_by[i]>>1]);
        }
    }
    return 0;
}


void joystick_update(void)
{
    int value;
    int dev_index;
    int joy_dev;
    uint32 buttons;
    BJoystick *last_joy = NULL;

    for (dev_index = 0; dev_index < 4; dev_index++) {
        value = 0;
        joy_dev = joystick_port_map[dev_index];
 
        if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {
            if (!last_joy) {
                if (bjoystick[dev_index]->Update() == B_ERROR) {
                    log_error(LOG_DEFAULT,"Joystick: Warning. Couldn't get Joystick value for device %d", dev_index + 1);
                    break;
                } else {
                    bjoystick[dev_index]->GetAxisValues(axes[dev_index], stick_nr[dev_index]);
                    buttons = bjoystick[dev_index]->ButtonValues(stick_nr[dev_index]);
                }
                last_joy = bjoystick[dev_index];
            }

            value = 0;
            if (axes[dev_index][2 * axes_nr[dev_index]] < JOYBORDER_MINX) {
                value |= 4;
            }
            if (axes[dev_index][2 * axes_nr[dev_index]] > JOYBORDER_MAXX) {
                value |= 8;
            }
            if (axes[dev_index][2 * axes_nr[dev_index] + 1] < JOYBORDER_MINY) {
                value |= 1;
            }
            if (axes[dev_index][2 * axes_nr[dev_index] + 1] > JOYBORDER_MAXY) {
                value |= 2;
            }
            if (buttons & (1 << axes_nr[dev_index] * 2) || buttons & (1 << axes_nr[dev_index]* 2 + 1)) {
                value |= 16;
            }
            joystick_set_value_absolute(dev_index+1, value);
        }
    }
}

#if 0
/* Joystick-through-keyboard.
   This code has done nothing since VICE 1.21, but was still executed as
   recently as 2.3.12
*/
int handle_keyset_mapping(joystick_device_t device, int *set,
                          kbd_code_t kcode, int pressed)
{
    if (joystick_port_map[0] == device || joystick_port_map[1] == device ||
        joystick_port_map[2] == device || joystick_port_map[3] == device) {
        BYTE value = 0;

        if (kcode == set[KEYSET_NW]) {    /* North-West */
            value = 5;
        } else if (kcode == set[KEYSET_N]) { /* North */
            value = 1;
        } else if (kcode == set[KEYSET_NE]) { /* North-East */
            value = 9;
        } else if (kcode == set[KEYSET_E]) { /* East */
            value = 8;
        } else if (kcode == set[KEYSET_SE]) { /* South-East */
            value = 10;
        } else if (kcode == set[KEYSET_S]) { /* South */
            value = 2;
        } else if (kcode == set[KEYSET_SW]) { /* South-West */
            value = 6;
        } else if (kcode == set[KEYSET_W]) { /* West */
            value = 4;
        } else if (kcode == set[KEYSET_FIRE]) { /* Fire */
            value = 16;
        } else {
            return 0;
        }

        if (pressed) {
            if (joystick_port_map[0] == device) {
                joystick_set_value_or(1, value);
            }
            if (joystick_port_map[1] == device) {
                joystick_set_value_or(2, value);
            }
            if (joystick_port_map[2] == device) {
                joystick_set_value_or(3, value);
            }
            if (joystick_port_map[3] == device) {
                joystick_set_value_or(4, value);
            }
        } else {
            if (joystick_port_map[0] == device) {
                joystick_set_value_and(1, ~value);
            }
            if (joystick_port_map[1] == device) {
                joystick_set_value_and(2, ~value);
            }
            if (joystick_port_map[2] == device) {
                joystick_set_value_and(3, ~value);
            }
            if (joystick_port_map[3] == device) {
                joystick_set_value_and(4, ~value);
            }
        }
        return 1;
    }
    return 0;
}
#endif

/* FIXME: src/joystick.c has code which handles keysets and also numpad.
   The numpad code here functions slightly differently and overrides the
   common code. The two should be merged. */
int joystick_handle_key(kbd_code_t kcode, int pressed)
{
    int value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_port_map[0] == JOYDEV_NUMPAD || joystick_port_map[1] == JOYDEV_NUMPAD ||
        joystick_port_map[2] == JOYDEV_NUMPAD || joystick_port_map[3] == JOYDEV_NUMPAD) {

        switch (kcode) {
            case K_KP7:               /* North-West */
                value = 5;
                break;
            case K_KP8:               /* North */
                value = 1;
                break;
            case K_KP9:               /* North-East */
                value = 9;
                break;
            case K_KP6:               /* East */
                value = 8;
                break;
            case K_KP3:               /* South-East */
                value = 10;
                break;
            case K_KP2:               /* South */
            case K_KP5:
                value = 2;
                break;
            case K_KP1:               /* South-West */
                value = 6;
                break;
            case K_KP4:                  /* West */
                value = 4;
                break;
            case K_KP0:
            case K_RIGHTCTRL:
                value = 16;
                break;
            default:
                /* (make compiler happy) */
                break;
        }

        if (pressed) {
            if (joystick_port_map[0] == JOYDEV_NUMPAD) {
                joystick_set_value_or(1, value);
            }
            if (joystick_port_map[1] == JOYDEV_NUMPAD) {
                joystick_set_value_or(2, value);
            }
            if (joystick_port_map[2] == JOYDEV_NUMPAD) {
                joystick_set_value_or(3, value);
            }
            if (joystick_port_map[3] == JOYDEV_NUMPAD) {
                joystick_set_value_or(4, value);
            }
        } else {
            if (joystick_port_map[0] == JOYDEV_NUMPAD) {
                joystick_set_value_and(1, ~value);
            }
            if (joystick_port_map[1] == JOYDEV_NUMPAD) {
                joystick_set_value_and(2, ~value);
            }
            if (joystick_port_map[2] == JOYDEV_NUMPAD) {
                joystick_set_value_and(3, ~value);
            }
            if (joystick_port_map[3] == JOYDEV_NUMPAD) {
                joystick_set_value_and(4, ~value);
            }
        }
    }

    return value;

#if 0
    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (value | handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed) | handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed));
#endif
}
