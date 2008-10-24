/*
 * joy.c - Joystick support for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <conio.h>
#include <stdio.h>

#include <allegro.h>

#include "cmdline.h"
#include "keyboard.h"
#include "joy.h"
#include "joystick.h"
#include "resources.h"
#include "translate.h"
#include "tui.h"
#include "types.h"


#ifdef COMMON_KBD
int joystick_port_map[2];
#endif

/* ------------------------------------------------------------------------- */

/* Joystick devices.  */
static int set_joystick_device_1(int val, void *param)
{
    joystick_device_t dev = (joystick_device_t)val;
    joystick_device_t old_joystick_device_1 = joystick_port_map[0];

    joystick_port_map[0] = dev;
    if (dev == JOYDEV_NONE && old_joystick_device_1 != JOYDEV_NONE)
        joystick_set_value_absolute(1, 0);
    return 0;
}

static int set_joystick_device_2(int val, void *param)
{
    joystick_device_t dev = (joystick_device_t)val;
    joystick_device_t old_joystick_device_2 = joystick_port_map[1];

    joystick_port_map[1] = dev;
    if (dev == JOYDEV_NONE && old_joystick_device_2 != JOYDEV_NONE)
        joystick_set_value_absolute(2, 0);
    return 0;
}

static int joystick_hw_type;

static int set_joystick_hw_type(int val, void *param)
{
    if (joystick_hw_type != val){
        int old_joystick_hw_type = joystick_hw_type;
        int old_num_joysticks = num_joysticks;
        
    	joystick_hw_type = val;
        remove_joystick();
        if (joystick_hw_type != 0){
            if(install_joystick(joystick_hw_type)){
                tui_error("Initialization of joystick device failed");
                joystick_hw_type = 0;
            }
            else if (num_joysticks < 2 && old_num_joysticks >= 2){
                if (joystick_port_map[0] == JOYDEV_HW2)
                    joystick_set_value_absolute(1, 0);
                if (joystick_port_map[1] == JOYDEV_HW2)
                    joystick_set_value_absolute(2, 0);
            }
        }
        if (joystick_hw_type == 0 && old_joystick_hw_type != 0){
            if (joystick_port_map[0] == JOYDEV_HW1 || joystick_port_map[0] == JOYDEV_HW2)
                joystick_set_value_absolute(1, 0);
            if (joystick_port_map[1] == JOYDEV_HW1 || joystick_port_map[1] == JOYDEV_HW2)
                joystick_set_value_absolute(2, 0);
        }
    }
    
    return 0;
}

static const resource_int_t resources_int[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[0], set_joystick_device_1, NULL },
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[1], set_joystick_device_2, NULL },
    { "HwJoyType", 0, RES_EVENT_NO, NULL,
      &joystick_hw_type, set_joystick_hw_type, NULL },
    { NULL }
};

int joystick_arch_init_resources(void)
{
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice1", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for joystick #1" },
    { "-joydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice2", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for joystick #2" },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */


/* Initialize joystick support.  */
int joy_arch_init(void)
{
    return 0;
}

/* Update the `joystick_value' variables according to the joystick status.  */
void joystick_update(void)
{
    if (num_joysticks == 0)
	return;

    poll_joystick();

    if (joystick_port_map[0] == JOYDEV_HW1 || joystick_port_map[1] == JOYDEV_HW1) {
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
        if (joystick_port_map[0] == JOYDEV_HW1)
            joystick_set_value_absolute(1, value);
        if (joystick_port_map[1] == JOYDEV_HW1)
            joystick_set_value_absolute(2, value);
    }

    if (num_joysticks >= 2
        && (joystick_port_map[0] == JOYDEV_HW2
            || joystick_port_map[1] == JOYDEV_HW2)) {
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
        if (joystick_port_map[0] == JOYDEV_HW2)
            joystick_set_value_absolute(1, value);
        if (joystick_port_map[1] == JOYDEV_HW2)
            joystick_set_value_absolute(2, value);
    }
}

void joystick_close(void)
{
   /* Nothing to do on MSDOS.  */
   return;
}


/* ------------------------------------------------------------------------- */
