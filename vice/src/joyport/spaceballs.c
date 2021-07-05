/*
 * spaceballs.c - Single SNES PAD emulation.
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
#include "spaceballs.h"
#include "resources.h"
#include "snapshot.h"
#include "userport.h"


#include "log.h"

/* 8 joysticks are wired in parallel with respect to their
   data lines. The ground of each joystick is hooked up to
   the data pins of the userport:

   GND  | USERPORT
   -----------------
   JOY1 | pin C (D0)
   JOY2 | pin D (D1)
   JOY3 | pin E (D2)
   JOY4 | pin F (D3)
   JOY5 | pin H (D4)
   JOY6 | pin J (D5)
   JOY7 | pin K (D6)
   JOY8 | pin L (D7)
   
   The userport is driven in such a way that only 1 joystick
   has ground.
 */

#define JOYPAD_FIRE2 0x20
#define JOYPAD_FIRE3 0x40

static int spaceballs_enabled = 0;

static uint8_t spaceballs_grounds = 0xff;

/* ------------------------------------------------------------------------- */

static joyport_t joyport_spaceballs_device;

static userport_device_t userport_spaceballs_device;

static userport_device_list_t *userport_spaceballs_list_item = NULL;

static int joyport_spaceballs_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (val == spaceballs_enabled) {
        return 0;
    }

    if (val) {
        userport_spaceballs_list_item = userport_device_register(&userport_spaceballs_device);
        if (userport_spaceballs_list_item == NULL) {
            return -1;
        }
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_SPACEBALLS, joyport_spaceballs_device.name);
    } else {
        joystick_adapter_deactivate();
        userport_device_unregister(userport_spaceballs_list_item);
        userport_spaceballs_list_item = NULL;
    }

    spaceballs_enabled = val;

    return 0;
}

static uint8_t spaceballs_read(int port)
{
    uint8_t retval = 0;
    uint16_t joyval = 0;

    int i;

    for (i = 0; i < 8; i++) {
        if (!((spaceballs_grounds >> i) & 1)) {
            joyval = get_joystick_value(JOYPORT_3 + i);
            retval |= (joyval & 0x1f);
        }
    }

    return ~(retval);
}

static uint8_t spaceballs_potx_read(int port)
{
    uint8_t retval = 0xff;
    int i;

    for (i = 0; i < 8; i++) {
        if (!((spaceballs_grounds >> i) & 1)) {
            retval &= (get_joystick_value(JOYPORT_3 + i) & JOYPAD_FIRE2) ? 0x00 : 0xff;
        }
    }
    return retval;
}

static uint8_t spaceballs_poty_read(int port)
{
    uint8_t retval = 0xff;
    int i;

    for (i = 0; i < 8; i++) {
        if (!((spaceballs_grounds >> i) & 1)) {
            retval &= (get_joystick_value(JOYPORT_3 + i) & JOYPAD_FIRE3) ? 0x00 : 0xff;
        }
    }
    return retval;
}

/* ------------------------------------------------------------------------- */

static joyport_t joyport_spaceballs_device = {
    "Spaceballs",                     /* name of the device */
    JOYPORT_RES_ID_NONE,              /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,          /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,             /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_SPACEBALLS,   /* device is a joystick adapter */
    joyport_spaceballs_enable,        /* device enable function */
    spaceballs_read,                  /* digital line read function */
    NULL,                             /* NO digital line store function */
    spaceballs_potx_read,             /* pot-x read function */
    spaceballs_poty_read,             /* NO pot-y read function */
    NULL,                             /* NO device write snapshot function */
    NULL                              /* NO device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_spaceballs_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_SPACEBALLS, &joyport_spaceballs_device);
}

/* ------------------------------------------------------------------------- */

int userport_spaceballs_enabled = 0;

static void userport_spaceballs_store_pbx(uint8_t value)
{
    spaceballs_grounds = value;
}

static userport_device_t userport_spaceballs_device = {
    USERPORT_DEVICE_SPACEBALLS,    /* device id */
    "Spaceballs",                  /* device name */
    JOYSTICK_ADAPTER_ID_NONE,      /* NOT a joystick adapter, the joyport part is the joystick adapter */
    NULL,                          /* NO read pb0-pb7 function */
    userport_spaceballs_store_pbx, /* store pb0-pb7 function */
    NULL,                          /* NO read pa2 pin function */
    NULL,                          /* NO store pa2 pin function */
    NULL,                          /* NO read pa3 pin function */
    NULL,                          /* NO store pa3 pin function */
    0,                             /* pc pin is NOT needed */
    NULL,                          /* NO store sp1 pin function */
    NULL,                          /* NO read sp1 pin function */
    NULL,                          /* NO store sp2 pin function */
    NULL,                          /* NO read sp2 pin function */
    NULL,                          /* resource used by the device */
    0xff,                          /* NO return value */
    0xff,                          /* validity mask of the device, doesn't change */
    0,                             /* device involved in a read collision, to be filled in by the collision detection system */
    0                              /* a tag to indicate the order of insertion */
};
