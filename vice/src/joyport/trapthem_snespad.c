/*
 * trapthem_snespad.c - Trapthem SNES PAD emulation.
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
#include "resources.h"
#include "snapshot.h"
#include "snespad.h"
#include "trapthem_snespad.h"

#include "log.h"

/* Control port <--> SNES PAD connections:

   cport |  SNES PAD  | I/O
   ------------------------
     3   |  DATA PAD  |  I
     4   |   CLOCK    |  O
     6   |   LATCH    |  O
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
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_TRAPTHEM_SNES, joyport_snespad_device.name);
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
    uint16_t joyval = get_joystick_value(JOYPORT_3);

    switch (counter) {
        case SNESPAD_BUTTON_A:
            retval = (uint8_t)((joyval & 0x10) >> 2);
            break;
        case SNESPAD_BUTTON_B:
            retval = (uint8_t)((joyval & 0x20) >> 3);
            break;
        case SNESPAD_BUTTON_X:
            retval = (uint8_t)((joyval & 0x40) >> 4);
            break;
        case SNESPAD_BUTTON_Y:
            retval = (uint8_t)((joyval & 0x80) >> 5);
            break;
        case SNESPAD_BUMPER_LEFT:
            retval = (uint8_t)((joyval & 0x100) >> 6);
            break;
        case SNESPAD_BUMPER_RIGHT:
            retval = (uint8_t)((joyval & 0x200) >> 7);
            break;
        case SNESPAD_BUTTON_SELECT:
            retval = (uint8_t)((joyval & 0x400) >> 8);
            break;
        case SNESPAD_BUTTON_START:
            retval = (uint8_t)((joyval & 0x800) >> 9);
            break;
        case SNESPAD_UP:
            retval = (uint8_t)((joyval & 1) << 2);
            break;
        case SNESPAD_DOWN:
            retval = (uint8_t)((joyval & 2) << 1);
            break;
        case SNESPAD_LEFT:
            retval = (uint8_t)(joyval & 4);
            break;
        case SNESPAD_RIGHT:
            retval = (uint8_t)((joyval & 8) >> 1);
            break;
        case SNESPAD_BIT_12_1:
        case SNESPAD_BIT_13_1:
        case SNESPAD_BIT_14_1:
        case SNESPAD_BIT_15_1:
            retval = 4;
            break;
        case SNESPAD_EOS:
            retval = 0;
            break;
        default:
            retval = 1;
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
    "Trapthem SNES PAD",               /* name of the device */
    JOYPORT_RES_ID_NONE,               /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,           /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,              /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_TRAPTHEM_SNES, /* device is a joystick adapter */
    joyport_snespad_enable,            /* device enable function */
    snespad_read,                      /* digital line read function */
    snespad_store,                     /* digital line store function */
    NULL,                              /* NO pot-x read function */
    NULL,                              /* NO pot-y read function */
    NULL,                              /* NO device write snapshot function */
    NULL                               /* NO device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_trapthem_snespad_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_TRAPTHEM_SNESPAD, &joyport_snespad_device);
}
