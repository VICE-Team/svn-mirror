/*
 * ninja_snespad.c - Ninja SNES PAD emulation.
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
#include "ninja_snespad.h"
#include "resources.h"
#include "snapshot.h"
#include "snespad.h"

#include "log.h"

/* Control port <--> SNES PAD connections:

   cport |   SNES PAD   | I/O
   -------------------------
     1   |   DATA PAD 1 |  I
     2   |   DATA PAD 2 |  I
     3   |   DATA PAD 3 |  I
     4   |     CLOCK    |  O
     6   |     LATCH    |  O
 */

/* Since there are currently no games that use more than 3 snes pads,
   only 3 snes pads are emulated, this is easily extendable in the
   future.
 */

static int snespad_enabled = 0;

static uint8_t counter = 0;

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
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_NINJA_SNES, joyport_snespad_device.name);
        counter = 0;
        joystick_adapter_set_ports(3);
    } else {
        joystick_adapter_deactivate();
    }

    snespad_enabled = val;

    return 0;
}

static uint8_t snespad_read(int port)
{
    uint8_t retval;
    uint16_t joyval1 = get_joystick_value(JOYPORT_3);
    uint16_t joyval2 = get_joystick_value(JOYPORT_4);
    uint16_t joyval3 = get_joystick_value(JOYPORT_5);

    switch (counter) {
        case SNESPAD_BUTTON_A:
            retval = (uint8_t)((joyval1 & 0x10) >> 4);
            retval |= (uint8_t)((joyval2 & 0x10) >> 3);
            retval |= (uint8_t)((joyval3 & 0x10) >> 2);
            break;
        case SNESPAD_BUTTON_B:
            retval = (uint8_t)((joyval1 & 0x20) >> 5);
            retval |= (uint8_t)((joyval2 & 0x20) >> 4);
            retval |= (uint8_t)((joyval3 & 0x20) >> 3);
            break;
        case SNESPAD_BUTTON_X:
            retval = (uint8_t)((joyval1 & 0x40) >> 6);
            retval |= (uint8_t)((joyval2 & 0x40) >> 5);
            retval |= (uint8_t)((joyval3 & 0x40) >> 4);
            break;
        case SNESPAD_BUTTON_Y:
            retval = (uint8_t)((joyval1 & 0x80) >> 7);
            retval |= (uint8_t)((joyval2 & 0x80) >> 6);
            retval |= (uint8_t)((joyval3 & 0x80) >> 5);
            break;
        case SNESPAD_BUMPER_LEFT:
            retval = (uint8_t)((joyval1 & 0x100) >> 8);
            retval |= (uint8_t)((joyval2 & 0x100) >> 7);
            retval |= (uint8_t)((joyval3 & 0x100) >> 6);
            break;
        case SNESPAD_BUMPER_RIGHT:
            retval = (uint8_t)((joyval1 & 0x200) >> 9);
            retval |= (uint8_t)((joyval2 & 0x200) >> 8);
            retval |= (uint8_t)((joyval3 & 0x200) >> 7);
            break;
        case SNESPAD_BUTTON_SELECT:
            retval = (uint8_t)((joyval1 & 0x400) >> 10);
            retval |= (uint8_t)((joyval2 & 0x400) >> 9);
            retval |= (uint8_t)((joyval3 & 0x400) >> 8);
            break;
        case SNESPAD_BUTTON_START:
            retval = (uint8_t)((joyval1 & 0x800) >> 11);
            retval |= (uint8_t)((joyval2 & 0x800) >> 10);
            retval |= (uint8_t)((joyval3 & 0x800) >> 9);
            break;
        case SNESPAD_UP:
            retval = (uint8_t)(joyval1 & 1);
            retval |= (uint8_t)((joyval2 & 1) << 1);
            retval |= (uint8_t)((joyval3 & 1) << 2);
            break;
        case SNESPAD_DOWN:
            retval = (uint8_t)((joyval1 & 2) >> 1);
            retval |= (uint8_t)(joyval2 & 2);
            retval |= (uint8_t)((joyval3 & 2) << 1);
            break;
        case SNESPAD_LEFT:
            retval = (uint8_t)((joyval1 & 4) >> 2);
            retval |= (uint8_t)((joyval2 & 4) >> 1);
            retval |= (uint8_t)(joyval3 & 4);
            break;
        case SNESPAD_RIGHT:
            retval = (uint8_t)((joyval1 & 8) >> 3);
            retval |= (uint8_t)((joyval2 & 8) >> 2);
            retval |= (uint8_t)((joyval3 & 8) >> 1);
            break;
        case SNESPAD_BIT_12_1:
        case SNESPAD_BIT_13_1:
        case SNESPAD_BIT_14_1:
        case SNESPAD_BIT_15_1:
            retval = 7;
            break;
        case SNESPAD_EOS:
            retval = 0;
            break;
        default:
            retval = 1;
    }

    return ~(retval);
}

static void snespad_store(int port, uint8_t val)
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

static int ninja_snespad_write_snapshot(struct snapshot_s *s, int p);
static int ninja_snespad_read_snapshot(struct snapshot_s *s, int p);

static joyport_t joyport_snespad_device = {
    "SNES Pad Adapter (Ninja)",       /* name of the device */
    JOYPORT_RES_ID_NONE,              /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,          /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,             /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NINJA_SNES,   /* device is a joystick adapter */
    JOYPORT_DEVICE_SNES_ADAPTER,      /* device is a SNES adapter */
    0,                                /* NO output bits */
    joyport_snespad_enable,           /* device enable function */
    snespad_read,                     /* digital line read function */
    snespad_store,                    /* digital line store function */
    NULL,                             /* NO pot-x read function */
    NULL,                             /* NO pot-y read function */
    ninja_snespad_write_snapshot,     /* device write snapshot function */
    ninja_snespad_read_snapshot       /* device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_ninja_snespad_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_NINJA_SNESPAD, &joyport_snespad_device);
}

/* ------------------------------------------------------------------------- */

/* NINJASNESPAD snapshot module format:

   type  |   name  | description
   ----------------------------------
   BYTE  | COUNTER | counter value
   BYTE  | LATCH   | latch line state
   BYTE  | CLOCK   | clock line state
 */

static char snap_module_name[] = "NINJASNESPAD";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int ninja_snespad_write_snapshot(struct snapshot_s *s, int p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0 
        || SMW_B(m, counter) < 0
        || SMW_B(m, latch_line) < 0
        || SMW_B(m, clock_line) < 0) {
            snapshot_module_close(m);
            return -1;
    }
    return snapshot_module_close(m);
}

static int ninja_snespad_read_snapshot(struct snapshot_s *s, int p)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B(m, &counter) < 0
        || SMR_B(m, &latch_line) < 0
        || SMR_B(m, &clock_line) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
