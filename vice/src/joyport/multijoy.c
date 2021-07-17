/*
 * multijoy.c - Multi Joy 8-player joystick adapter emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <stdlib.h>
#include <string.h>

#include "joyport.h"
#include "joystick.h"
#include "multijoy.h"
#include "resources.h"
#include "snapshot.h"
#include "userport.h"


#include "log.h"

/* 8 joysticks are wired in parallel with respect to their
   data lines. The ground of each joystick is hooked up to
   the output of a 74138 demultiplexer, and the other control
   port is used to deliver the address to be demultiplexed to
   the 74138:

   JOY PIN | 74138 INPUT
   ---------------------
       1   |    1 (A0)
       2   |    2 (A1)
       3   |    3 (A2)


   74138 OUTPUT | JOY GROUND
   -------------------------
      15 (O0)   |    JOY1
      14 (O1)   |    JOY2
      13 (O2)   |    JOY3
      12 (O3)   |    JOY4
      11 (O4)   |    JOY5
      10 (O5)   |    JOY6
       9 (O6)   |    JOY7
       7 (O7)   |    JOY8
 */

#define JOYPAD_FIRE2 0x20
#define JOYPAD_FIRE3 0x40

static int multijoy_enabled = 0;

static uint8_t multijoy_address = 0;

/* ------------------------------------------------------------------------- */

static joyport_t joyport_multijoy_joy_device;
static joyport_t joyport_multijoy_control_device;

static int joyport_multijoy_joysticks_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (val == multijoy_enabled) {
        return 0;
    }

    if (val) {
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_MULTIJOY, joyport_multijoy_joy_device.name);
        joystick_adapter_set_ports(8);
        if (port == JOYPORT_1) {
            resources_set_int("JoyPort2Device", JOYPORT_ID_MULTIJOY_CONTROL);
        } else {
            resources_set_int("JoyPort1Device", JOYPORT_ID_MULTIJOY_CONTROL);
        }
    } else {
        joystick_adapter_deactivate();
    }

    multijoy_enabled = val;

    return 0;
}

static int joyport_multijoy_control_enable(int port, int value)
{
    return 0;
}

static void multijoy_store(uint8_t val, uint8_t mask)
{
    multijoy_address = val & 7;
}

static uint8_t multijoy_read(int port)
{
    uint8_t retval = 0;
    uint16_t joyval;

    joyval = get_joystick_value(JOYPORT_3 + multijoy_address);
    retval = (uint8_t)(joyval & 0x1f);

    return ~(retval);
}

/* ------------------------------------------------------------------------- */

static joyport_t joyport_multijoy_joy_device = {
    "Joystick Adapter (MultiJoy Joysticks)", /* name of the device */
    JOYPORT_RES_ID_NONE,                     /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,                 /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,                    /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_MULTIJOY,            /* device is a joystick adapter */
    JOYPORT_DEVICE_JOYSTICK_ADAPTER,         /* device is a Joystick adapter */
    joyport_multijoy_joysticks_enable,       /* device enable function */
    multijoy_read,                           /* digital line read function */
    NULL,                                    /* NO digital line store function */
    NULL,                                    /* NO pot-x read function */
    NULL,                                    /* NO pot-y read function */
    NULL,                                    /* NO device write snapshot function */
    NULL                                     /* NO device read snapshot function */
};

static joyport_t joyport_multijoy_control_device = {
    "Joystick Adapter (MultiJoy Logic)", /* name of the device */
    JOYPORT_RES_ID_NONE,                 /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,             /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,                /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,            /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_JOYSTICK_ADAPTER,     /* device is a Joystick adapter */
    joyport_multijoy_control_enable,     /* device enable function */
    NULL,                                /* NO digital line read function */
    multijoy_store,                      /* digital line store function */
    NULL,                                /* NO pot-x read function */
    NULL,                                /* NO pot-y read function */
    NULL,                                /* NO device write snapshot function */
    NULL                                 /* NO device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_multijoy_resources_init(void)
{
    if (joyport_device_register(JOYPORT_ID_MULTIJOY_JOYSTICKS, &joyport_multijoy_joy_device) < 0) {
        return -1;
    }
    return joyport_device_register(JOYPORT_ID_MULTIJOY_CONTROL, &joyport_multijoy_control_device);
}
