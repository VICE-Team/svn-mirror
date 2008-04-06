/*
 * joystick.c - Joystick support for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "vice.h"

#include <stdio.h>
#include <allegro.h>

#include "resources.h"
#include "kbd.h"              /* FIXME: Maybe we should move `joy[]' here...  */

/* ------------------------------------------------------------------------- */

/* Joystick devices.  */
static joystick_device_t joystick_device_1, joystick_device_2;

static int set_joystick_device_1(resource_value_t v)
{
    joystick_device_t dev = (joystick_device_t) v;

    joystick_device_1 = dev;
    return 0;
}

static int set_joystick_device_2(resource_value_t v)
{
    joystick_device_t dev = (joystick_device_t) v;

    joystick_device_2 = dev;
    return 0;
}

static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NUMPAD,
      (resource_value_t *) &joystick_device_1, set_joystick_device_1 },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NUMPAD,
      (resource_value_t *) &joystick_device_2, set_joystick_device_2 },
    { NULL }
};

int joystick_init_resources(void)
{
    return resources_register(joystick_resources);
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

/* Flag: is joystick present?  */
int num_joysticks = 0;

/* Flag: have we initialized the Allegro joystick driver?  */
static int joystick_init_done = 0;

/* ------------------------------------------------------------------------- */

/* Initialize joystick support.  */
void joystick_init(void)
{
    if (joystick_init_done)
	return;

    joy_type = JOY_TYPE_2PADS;

    if (!initialise_joystick()) {
	printf("Two joysticks found.\n");
	num_joysticks = 2;
    } else {
        joy_type = JOY_TYPE_STANDARD;
	if (!initialise_joystick()) {
            printf("One joystick found.\n");
            num_joysticks = 1;
        } else {
            num_joysticks = 0;
            printf("No joysticks found.\n");
        }
    }

    joystick_init_done = 1;
}

/* Update the `joy' variables according to the joystick status.  */
void joystick_update(void)
{
    if (num_joysticks == 0)
	return;

    poll_joystick();

    if (joystick_device_1 == JOYDEV_HW1 || joystick_device_2 == JOYDEV_HW1) {
        int value = 0;

        if (joy_left)
            value |= 4;
        if (joy_right)
            value |= 8;
        if (joy_up)
            value |= 1;
        if (joy_down)
            value |= 2;
        if (joy_b1 || joy_b2)
            value |= 16;
        if (joystick_device_1 == JOYDEV_HW1)
            joy[1] = value;
        if (joystick_device_2 == JOYDEV_HW1)
            joy[2] = value;
    }

    if (num_joysticks >= 2
        && (joystick_device_1 == JOYDEV_HW2
            || joystick_device_2 == JOYDEV_HW2)) {
        int value = 0;

        if (joy2_left)
            value |= 4;
        if (joy2_right)
            value |= 8;
        if (joy2_up)
            value |= 1;
        if (joy2_down)
            value |= 2;
        if (joy2_b1 || joy2_b2)
            value |= 16;
        if (joystick_device_1 == JOYDEV_HW2)
            joy[1] = value;
        if (joystick_device_2 == JOYDEV_HW2)
            joy[2] = value;
    }
}

/* Handle keys to emulate the joystick.  Warning: this is called within the
   keyboard interrupt!  */
void joystick_handle_key(int kcode, int pressed)
{
    int value = 0;

    switch (kcode) {
      case K_KP8:
        value |= 1;
        break;
      case K_KP2:
      case K_KP5:
        value |= 2;
        break;
      case K_KP4:
        value |= 4;
        break;
      case K_KP6:
        value |= 8;
        break;
      case K_KP0:
      case K_RIGHTCTRL:
        value |= 16;
        break;
      default:
        return 0;
    }

    if (pressed) {
        if (joystick_device_1 == JOYDEV_NUMPAD)
            joy[1] |= value;
        if (joystick_device_2 == JOYDEV_NUMPAD)
            joy[2] |= value;
    } else {
        if (joystick_device_1 == JOYDEV_NUMPAD)
            joy[1] &= ~value;
        if (joystick_device_2 == JOYDEV_NUMPAD)
            joy[2] &= ~value;
    }

    return 1;
}
