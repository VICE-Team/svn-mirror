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
#include "resources.h"
#include "joy.h"
#include "log.h"
#include "keyboard.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
}

/* Notice that this has to be `int' to make resources work.  */
static int keyset1[9], keyset2[9];

/* ------------------------------------------------------------------------ */

/* device resource  */
static joystick_device_t joystick_device[2];

/* flag for display state of joysticks in statusbar */
static int joystickdisplay;

/* objects to access hardware devices */
static BJoystick *bjoystick[2];

/* stick index in the open BJoystick devices */ 
static int stick_nr[2];

/* axes-pair index in the open BJoystick devices */ 
static int axes_nr[2];

/* to allocate buffers for the current axes values */
static int16 *axes[2];

/* to avoid problems opening a BJoystick device twice, we have to remember them */
static int device_used_by[MAX_HARDWARE_JOYSTICK];

/* to check if the joystick module is already initialized */
static int joystick_initialized = 0;

/* array that holds all recognized hardware sticks */
hardware_joystick_t hardware_joystick[MAX_HARDWARE_JOYSTICK];
int hardware_joystick_count = 0;


static void joystick_close_device(int dev_index)
{
	int device_num;
	int used_by;
	joystick_device_t joy_dev = joystick_device[dev_index];
	
	if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {
		/* it's a hardware-stick; close the device if necessary */
		int device_num = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_num;
		used_by = device_used_by[device_num];
		device_used_by[device_num] &= ~(1<<dev_index);
		if (!device_used_by[device_num] && used_by) {
			bjoystick[used_by-1]->Close();
			delete bjoystick[used_by-1];
			free (axes[used_by-1]);
			log_message(LOG_DEFAULT, "Joystick: Closed hardware %s as device %d",
					hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_name, dev_index+1);
		}
	}
}
	

static void joystick_open_device(int dev_index)
{
	int used_by;
	joystick_device_t joy_dev = joystick_device[dev_index];

	if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {
		/* is the needed device already open? */
		int device_num = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_num;
		if (used_by = device_used_by[device_num]) {
			bjoystick[dev_index] = bjoystick[used_by-1];
			axes[dev_index] = axes[used_by-1];
			log_message(LOG_DEFAULT, "Joystick: Device %d uses hardware port already opened by device %d", 
				dev_index+1, used_by);
		} else {
			bjoystick[dev_index] = new BJoystick();			
			if (bjoystick[dev_index]->Open(hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_name, true)
				== B_ERROR)
			{
				log_message(LOG_DEFAULT, "Joystick: Warning, couldn't open hardware device %d",
					dev_index+1);
				return;
			} else {
				log_message(LOG_DEFAULT, "Joystick: Opened hardware %s as device %d",
					hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_name, dev_index+1);
			}
			axes[dev_index] = (int16*) malloc(sizeof(int16) * bjoystick[dev_index]->CountAxes());		
		}
		device_used_by[device_num] |= (1<<dev_index);
		
		stick_nr[dev_index] = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].stick; 
		axes_nr[dev_index] = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].axes; 
	}
}


static int set_joystick_device(resource_value_t v, void *param)
{
	if (joystick_initialized)
		joystick_close_device((int) param);

    joystick_device[(int) param] = (joystick_device_t) (int) v; /* argh */
	
	if (joystick_initialized)
		joystick_open_device((int) param);

    return 0;
}


static int set_keyset1(resource_value_t v, void *param)
{
        keyset1[(int) param] = (int) v;
        return 0;
}

static int set_keyset2(resource_value_t v, void *param)
{
        keyset2[(int) param] = (int) v;
        return 0;
}


static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device[0], set_joystick_device, (void *) 0 },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device[1], set_joystick_device, (void *) 1 },
    { "KeySet1NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NW], set_keyset1, (void *) KEYSET_NW },
    { "KeySet1North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_N], set_keyset1, (void *) KEYSET_N },
    { "KeySet1NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NE], set_keyset1, (void *) KEYSET_NE },
    { "KeySet1East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_E], set_keyset1, (void *) KEYSET_E },
    { "KeySet1SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SE], set_keyset1, (void *) KEYSET_SE },
    { "KeySet1South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_S], set_keyset1, (void *) KEYSET_S },
    { "KeySet1SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SW], set_keyset1, (void *) KEYSET_SW },
    { "KeySet1West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_W], set_keyset1, (void *) KEYSET_W },
    { "KeySet1Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_FIRE], set_keyset1, (void *) KEYSET_FIRE },
    { "KeySet2NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NW], set_keyset2, (void *) KEYSET_NW },
    { "KeySet2North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_N], set_keyset2, (void *) KEYSET_N },
    { "KeySet2NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NE], set_keyset2, (void *) KEYSET_NE },
    { "KeySet2East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_E], set_keyset2, (void *) KEYSET_E },
    { "KeySet2SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SE], set_keyset2, (void *) KEYSET_SE },
    { "KeySet2South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_S], set_keyset2, (void *) KEYSET_S },
    { "KeySet2SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SW], set_keyset2, (void *) KEYSET_SW },
    { "KeySet2West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_W], set_keyset2, (void *) KEYSET_W },
    { "KeySet2Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_FIRE], set_keyset2, (void *) KEYSET_FIRE },
    { NULL }
};


int joystick_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice1", NULL,
      "<number>", "Set input device for joystick #1" },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      "<number>", "Set input device for joystick #2" },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int joy_arch_init(void)
{
	BJoystick testbjoystick;
	char current_devicename[B_OS_NAME_LENGTH];
	int device_iter, stick_iter, axes_iter;
	int device_count;

	if (joystick_initialized)
		return 0;

	device_count = testbjoystick.CountDevices();
	for (device_iter=0; device_iter<device_count; device_iter++) {
		device_used_by[device_iter] = 0;
		testbjoystick.GetDeviceName(device_iter, current_devicename);

		if (testbjoystick.Open(current_devicename, true) != B_ERROR)
		{
			for (stick_iter=0; stick_iter<testbjoystick.CountSticks(); stick_iter++) {
				for (axes_iter=0; axes_iter*2<testbjoystick.CountAxes(); axes_iter++) {
					strcpy(hardware_joystick[hardware_joystick_count].device_name, current_devicename);
					hardware_joystick[hardware_joystick_count].device_num = device_iter;
					hardware_joystick[hardware_joystick_count].stick = stick_iter;
					hardware_joystick[hardware_joystick_count++].axes = axes_iter;

					if (hardware_joystick_count >= MAX_HARDWARE_JOYSTICK)
					return 0;
				}
			}			
			testbjoystick.Close();
		}		
	}
	joystick_initialized = 1;
	joystick_open_device(0);
	joystick_open_device(1);

	return 0;
}

int joystick_close(void)
{
	int i;
	
	for (i=0; i<2; i++) {
		if (device_used_by[i]) {
			delete bjoystick[device_used_by[i]>>1];
			free(axes[device_used_by[i]>>1]);
		}
	}
}


void joystick_update(void)
{
	int value;
	int dev_index;
	int joy_dev;
	uint32 buttons;
	BJoystick *last_joy = NULL;
	
    for (dev_index = 0; dev_index < 2; dev_index++) {
    	value = 0;
    	joy_dev = joystick_device[dev_index];
 
     	if (joy_dev >= NUM_OF_SOFTDEVICES 
     		&& joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {	
    		if (!last_joy)
    		{
				if (bjoystick[dev_index]->Update() == B_ERROR) {
					log_error(LOG_DEFAULT,"Joystick: Warning. Couldn't get Joystick value for device %d",
						dev_index+1);
					break;
				} else {
					bjoystick[dev_index]->GetAxisValues(axes[dev_index], stick_nr[dev_index]);
					buttons = bjoystick[dev_index]->ButtonValues(stick_nr[dev_index]);
				}
				last_joy = bjoystick[dev_index];
			}  

			value = 0;
			if (axes[dev_index][2*axes_nr[dev_index]] < JOYBORDER_MINX)
				value |= 4;
			if (axes[dev_index][2*axes_nr[dev_index]] > JOYBORDER_MAXX)
				value |= 8;
			if (axes[dev_index][2*axes_nr[dev_index]+1] < JOYBORDER_MINY)
				value |= 1;
			if (axes[dev_index][2*axes_nr[dev_index]+1] > JOYBORDER_MAXY)
				value |= 2;
			if (buttons & (1 << axes_nr[dev_index]*2) || buttons & (1 << axes_nr[dev_index]*2+1) )
				value |= 16;
			joystick_set_value_absolute(dev_index+1, value);
			ui_display_joyport_abs(dev_index, value);
		}
	}
}

/* Joystick-through-keyboard.  */
int handle_keyset_mapping(joystick_device_t device, int *set,
                          kbd_code_t kcode, int pressed)
{
    if (joystick_device[0] == device || joystick_device[1] == device) {
        BYTE value = 0;
        if (kcode == set[KEYSET_NW])    /* North-West */
            value = 5;
        else if (kcode == set[KEYSET_N]) /* North */
            value = 1;
        else if (kcode == set[KEYSET_NE]) /* North-East */
            value = 9;
        else if (kcode == set[KEYSET_E]) /* East */
            value = 8;
        else if (kcode == set[KEYSET_SE]) /* South-East */
            value = 10;
        else if (kcode == set[KEYSET_S]) /* South */
            value = 2;
        else if (kcode == set[KEYSET_SW]) /* South-West */
            value = 6;
        else if (kcode == set[KEYSET_W]) /* West */
            value = 4;
        else if (kcode == set[KEYSET_FIRE]) /* Fire */
            value = 16;
        else
            return 0;

        if (pressed) {
            if (joystick_device[0] == device) {
                joystick_set_value_or(1, value);
				ui_display_joyport_or(0, value);
			}
            if (joystick_device[1] == device) {
                joystick_set_value_or(2, value);
				ui_display_joyport_or(1, value);
			}
        } else {
            if (joystick_device[0] == device) {
                joystick_set_value_and(1, ~value);
				ui_display_joyport_and(0, ~value);
			}
            if (joystick_device[1] == device) {
                joystick_set_value_and(2, ~value);
				ui_display_joyport_and(1, ~value);
			}
        }
        return 1;
    }
    return 0;
}

int joystick_handle_key(kbd_code_t kcode, int pressed)
{
    int value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_device[0] == JOYDEV_NUMPAD
        || joystick_device[1] == JOYDEV_NUMPAD) {

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
            ;
        }

        if (pressed) {
            if (joystick_device[0] == JOYDEV_NUMPAD) {
                joystick_set_value_or(1, value);
				ui_display_joyport_or(0, value);
			}
            if (joystick_device[1] == JOYDEV_NUMPAD) {
                joystick_set_value_or(2, value);
				ui_display_joyport_or(1, value);
			}
        } else {
            if (joystick_device[0] == JOYDEV_NUMPAD) {
                joystick_set_value_and(1, ~value);
				ui_display_joyport_and(0, ~value);
			}
            if (joystick_device[1] == JOYDEV_NUMPAD) {
                joystick_set_value_and(2, ~value);
				ui_display_joyport_and(1, ~value);
			}
        }
    }

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (value
            | handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed)
            | handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed));
}

