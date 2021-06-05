/*
 * snespad.c - Single SNES PAD emulation.
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
#include "snespad.h"
#include "resources.h"
#include "snapshot.h"


#include "log.h"

/* Control port <--> SNES PAD connections:

   cport | SNES PAD | I/O
   -------------------------
     1   |   DATA   |  I
     4   |   CLOCK  |  O
     6   |   LATCH  |  O
 */

/* TODO: expand to support the following:

          cport        | SNES PADS | I/O
   -------------------------------------
   port 1 pin 1 (joy0) | PAD1 DATA |  I
   port 1 pin 2 (joy1) | PAD2 DATA |  I
   port 1 pin 3 (joy2) | PAD3 DATA |  I
   port 1 pin 4 (joy3) |   CLOCK   |  O
   port 1 pin 6 (joy4) |   LATCH   |  O
   port 2 pin 1 (joy0) | PAD4 DATA |  I
   port 2 pin 2 (joy1) | PAD5 DATA |  I
   port 2 pin 3 (joy2) | PAD6 DATA |  I
   port 2 pin 4 (joy3) | PAD7 DATA |  I
   port 2 pin 6 (joy4) | PAD8 DATA |  I
 */

static int snespad_enabled = 0;

static int counter = 0;

static uint8_t clock_line = 0;
static uint8_t latch_line = 0;

/* ------------------------------------------------------------------------- */

static joyport_t joyport_snespad_device;

static int joyport_snespad_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (val == snespad_enabled) {
        return 0;
    }

    if (val) {
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_JOYPORT_SNES, joyport_snespad_device.name);
        counter = 0;
    } else {
        joystick_adapter_deactivate();
    }

    snespad_enabled = val;

    return 0;
}

static uint8_t snespad_read(int port)
{
    uint8_t retval;
    uint16_t joyval = get_joystick_value(port);

    switch (counter) {
        case SNESPAD_BUTTON_A:
            retval = (uint8_t)((joyval & 0x10) >> 4);
            break;
        case SNESPAD_BUTTON_B:
            retval = (uint8_t)((joyval & 0x20) >> 5);
            break;
        case SNESPAD_BUTTON_X:
            retval = (uint8_t)((joyval & 0x40) >> 6);
            break;
        case SNESPAD_BUTTON_Y:
            retval = (uint8_t)((joyval & 0x80) >> 7);
            break;
        case SNESPAD_BUMPER_LEFT:
            retval = (uint8_t)((joyval & 0x100) >> 8);
            break;
        case SNESPAD_BUMPER_RIGHT:
            retval = (uint8_t)((joyval & 0x200) >> 9);
            break;
        case SNESPAD_BUTTON_SELECT:
            retval = (uint8_t)((joyval & 0x400) >> 10);
            break;
        case SNESPAD_BUTTON_START:
            retval = (uint8_t)((joyval & 0x800) >> 11);
            break;
        case SNESPAD_UP:
            retval = (uint8_t)(joyval & 1);
            break;
        case SNESPAD_DOWN:
            retval = (uint8_t)((joyval & 2) >> 1);
            break;
        case SNESPAD_LEFT:
            retval = (uint8_t)((joyval & 4) >> 2);
            break;
        case SNESPAD_RIGHT:
            retval = (uint8_t)((joyval & 8) >> 3);
            break;
        case SNESPAD_EOS:
            retval = 1;
            break;
        default:
            retval = 0;
    }

    return ~(retval);
}

static void snespad_store(uint8_t val)
{
    uint8_t new_clock = (val & 0x08) >> 3;
    uint8_t new_latch = (val & 0x10) >> 4;

    if (latch_line && !new_latch) {
        counter = 0;
    }

    if (clock_line && !new_clock) {
        if (counter != SNESPAD_EOS) {
            counter++;
        }
    }

    latch_line = new_latch;
    clock_line = new_clock;
}

/* ------------------------------------------------------------------------- */

static joyport_t joyport_snespad_device = {
    "SNES PAD",                       /* name of the device */
    JOYPORT_RES_ID_NONE,              /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,          /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,             /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_JOYPORT_SNES, /* device is a joystick adapter */
    joyport_snespad_enable,           /* device enable function */
    snespad_read,                     /* digital line read function */
    snespad_store,                    /* digital line store function */
    NULL,                             /* NO pot-x read function */
    NULL,                             /* NO pot-y read function */
    NULL,                             /* NO device write snapshot function */
    NULL                              /* NO device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_snespad_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_SNESPAD, &joyport_snespad_device);
}
