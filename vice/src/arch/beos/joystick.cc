/*
 * joystick.c - Joystick support for BeOS
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include "joystick.h"
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
static joystick_device_t joystick_device_1, joystick_device_2;

/* objects to access hardware devices */
static BJoystick bjoystick[2];

/* stick index in the open BJoystick devices */ 
static int stick_nr[2];

/* to allocate buffers for the current axes values */
static int16 *axes[2];

/* to check if the joystick module is already initialized */
static int joystick_initialized = 0;

/* array that holds all recognized hardware sticks */
hardware_joystick_t hardware_joystick[MAX_HARDWARE_JOYSTICK];
int hardware_joystick_count = 0;


static void joystick_open_device(int dev_index)
{
	joystick_device_t joy_dev;
	
	if (dev_index == 0)
		joy_dev = joystick_device_1;
	else
		joy_dev = joystick_device_2;

	if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count) {
		if (bjoystick[dev_index].Open(hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_name, true)
			== B_ERROR)
		{
			log_message(LOG_DEFAULT, "Joystick: Warning, couldn't open hardware device %d", dev_index+1);
			return;
		} else {
			log_message(LOG_DEFAULT, "Joystick: Opened hardware %s as device %d",
				hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].device_name, dev_index+1);
		}
		if (axes[dev_index]) 
			free(axes[dev_index]);
		axes[dev_index] = (int16*) malloc(sizeof(int16) * bjoystick[dev_index].CountAxes());		
		stick_nr[dev_index] = hardware_joystick[joy_dev-NUM_OF_SOFTDEVICES].stick; 
	}
}


static int set_joystick_device_1(resource_value_t v, void *param)
{
    joystick_device_1 = (joystick_device_t) (int) v; /* argh */
	
	if (joystick_initialized)
		joystick_open_device(0);

    return 0;
}

static int set_joystick_device_2(resource_value_t v, void *param)
{
    joystick_device_2 = (joystick_device_t) (int) v; /* C++ needs them */

	if (joystick_initialized)
		joystick_open_device(1);

    return 0;
}


#define DEFINE_SET_KEYSET(num, dir)                             \
    static int set_keyset##num##_##dir##(resource_value_t v, void *param)    \
    {                                                           \
        keyset##num##[KEYSET_##dir##] = (int) v;                \
                                                                \
        return 0;                                               \
    }

DEFINE_SET_KEYSET(1, NW)
DEFINE_SET_KEYSET(1, N)
DEFINE_SET_KEYSET(1, NE)
DEFINE_SET_KEYSET(1, E)
DEFINE_SET_KEYSET(1, SE)
DEFINE_SET_KEYSET(1, S)
DEFINE_SET_KEYSET(1, SW)
DEFINE_SET_KEYSET(1, W)
DEFINE_SET_KEYSET(1, FIRE)

DEFINE_SET_KEYSET(2, NW)
DEFINE_SET_KEYSET(2, N)
DEFINE_SET_KEYSET(2, NE)
DEFINE_SET_KEYSET(2, E)
DEFINE_SET_KEYSET(2, SE)
DEFINE_SET_KEYSET(2, S)
DEFINE_SET_KEYSET(2, SW)
DEFINE_SET_KEYSET(2, W)
DEFINE_SET_KEYSET(2, FIRE)

static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_1, set_joystick_device_1, NULL },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_2, set_joystick_device_2, NULL },
    { "KeySet1NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NW], set_keyset1_NW, NULL },
    { "KeySet1North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_N], set_keyset1_N, NULL },
    { "KeySet1NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NE], set_keyset1_NE, NULL },
    { "KeySet1East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_E], set_keyset1_E, NULL },
    { "KeySet1SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SE], set_keyset1_SE, NULL },
    { "KeySet1South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_S], set_keyset1_S, NULL },
    { "KeySet1SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SW], set_keyset1_SW, NULL },
    { "KeySet1West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_W], set_keyset1_W, NULL },
    { "KeySet1Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_FIRE], set_keyset1_FIRE, NULL },
    { "KeySet2NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NW], set_keyset2_NW, NULL },
    { "KeySet2North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_N], set_keyset2_N, NULL },
    { "KeySet2NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NE], set_keyset2_NE, NULL },
    { "KeySet2East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_E], set_keyset2_E, NULL },
    { "KeySet2SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SE], set_keyset2_SE, NULL },
    { "KeySet2South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_S], set_keyset2_S, NULL },
    { "KeySet2SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SW], set_keyset2_SW, NULL },
    { "KeySet2West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_W], set_keyset2_W, NULL },
    { "KeySet2Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_FIRE], set_keyset2_FIRE, NULL },
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

int joystick_init(void)
{
	BJoystick testbjoystick;
	char current_devicename[B_OS_NAME_LENGTH];
	int device_iter, stick_iter;
	int device_count;

	if (joystick_initialized)
		return 0;

	device_count = testbjoystick.CountDevices();
	for (device_iter=0; device_iter<device_count; device_iter++) {
		testbjoystick.GetDeviceName(device_iter, current_devicename);
		
		if (testbjoystick.Open(current_devicename, true) != B_ERROR)
		{
			for (stick_iter=0; stick_iter<testbjoystick.CountSticks(); stick_iter++) {
				strcpy(hardware_joystick[hardware_joystick_count].device_name, current_devicename);
				hardware_joystick[hardware_joystick_count++].stick = stick_iter;
				if (hardware_joystick_count >= MAX_HARDWARE_JOYSTICK)
					return 0;
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
		bjoystick[i].Close();
		if (axes[i])
			free(axes[i]);
	}
}


void joystick_update(void)
{
	int value;
	int dev_index;
	int joy_dev;
	uint32 buttons;
	
    for (dev_index = 0; dev_index < 2; dev_index++) {
    	value = 0;
    	if (dev_index == 0)
    		joy_dev = joystick_device_1;
    	else
	   		joy_dev = joystick_device_2;
 
     	if (joy_dev >= NUM_OF_SOFTDEVICES && joy_dev < NUM_OF_SOFTDEVICES+hardware_joystick_count)
    	{
			if (bjoystick[dev_index].Update() == B_ERROR) {
				log_error(LOG_DEFAULT,"Joystick: Warning. Couldn't get Joystick value for device %d",
					dev_index+1);
				break;
			} else {
				bjoystick[dev_index].GetAxisValues(axes[dev_index], stick_nr[dev_index]);
				buttons = bjoystick[dev_index].ButtonValues(stick_nr[dev_index]);
				value = 0;
				if (axes[dev_index][0] < JOYBORDER_MINX)
					value |= 4;
				if (axes[dev_index][0] > JOYBORDER_MAXX)
					value |= 8;
				if (axes[dev_index][1] < JOYBORDER_MINY)
					value |= 1;
				if (axes[dev_index][1] > JOYBORDER_MAXY)
					value |= 2;
				if (buttons)
					value |= 16;

				joystick_value[dev_index+1]=value;
			}
		}
	}
}

/* Joystick-through-keyboard.  */
int handle_keyset_mapping(joystick_device_t device, int *set,
                          kbd_code_t kcode, int pressed)
{
    if (joystick_device_1 == device || joystick_device_2 == device) {
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
            if (joystick_device_1 == device)
                joystick_set_value_or(1, value);
            if (joystick_device_2 == device)
                joystick_set_value_or(2, value);
        } else {
            if (joystick_device_1 == device)
                joystick_set_value_and(1, ~value);
            if (joystick_device_2 == device)
                joystick_set_value_and(2, ~value);
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
    if (joystick_device_1 == JOYDEV_NUMPAD
        || joystick_device_2 == JOYDEV_NUMPAD) {

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
            if (joystick_device_1 == JOYDEV_NUMPAD)
                joystick_set_value_or(1, value);
            if (joystick_device_2 == JOYDEV_NUMPAD)
                joystick_set_value_or(2, value);
        } else {
            if (joystick_device_1 == JOYDEV_NUMPAD)
                joystick_set_value_and(1, ~value);
            if (joystick_device_2 == JOYDEV_NUMPAD)
                joystick_set_value_and(2, ~value);
        }
    }

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (value
            | handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed)
            | handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed));
}

