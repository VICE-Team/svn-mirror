/*
 * inception.c - Inception 8-player joystick adapter emulation.
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
#include "inception.h"
#include "resources.h"
#include "snapshot.h"
#include "snespad.h"

#include "log.h"

/* Control port <--> INCEPTION connections:

   cport |   INCEPTION
   -------------------
     1   |      D0
     2   |      D1
     3   |      D2
     4   |      D3
     6   |      D4
 */

static int inception_enabled = 0;

static uint8_t counter = INCEPTION_STATE_IDLE;

static uint8_t clock_line = 1;

/* ------------------------------------------------------------------------- */

static joyport_t joyport_inception_device;

static int joyport_inception_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (val == inception_enabled) {
        return 0;
    }

    if (val) {
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_INCEPTION, joyport_inception_device.name);
        counter = 0;
        joystick_adapter_set_ports(8);
    } else {
        joystick_adapter_deactivate();
    }

    inception_enabled = val;

    return 0;
}

static uint8_t inception_read(int port)
{
    uint8_t retval;
    uint16_t joyval1 = get_joystick_value(JOYPORT_3);
    uint16_t joyval2 = get_joystick_value(JOYPORT_4);
    uint16_t joyval3 = get_joystick_value(JOYPORT_5);
    uint16_t joyval4 = get_joystick_value(JOYPORT_6);
    uint16_t joyval5 = get_joystick_value(JOYPORT_7);
    uint16_t joyval6 = get_joystick_value(JOYPORT_8);
    uint16_t joyval7 = get_joystick_value(JOYPORT_9);
    uint16_t joyval8 = get_joystick_value(JOYPORT_10);

    switch (counter) {
        case INCEPTION_STATE_IDLE:
            retval = 0xff;
            break;
        case INCEPTION_STATE_HI_JOY1:
            retval = (uint8_t)((joyval1 & 0x20) >> 2);
            retval |= (uint8_t)(joyval1 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY1:
            retval = joyval1 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY2:
            retval = (uint8_t)((joyval2 & 0x20) >> 2);
            retval |= (uint8_t)(joyval2 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY2:
            retval = joyval2 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY3:
            retval = (uint8_t)((joyval3 & 0x20) >> 2);
            retval |= (uint8_t)(joyval3 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY3:
            retval = joyval3 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY4:
            retval = (uint8_t)((joyval4 & 0x20) >> 2);
            retval |= (uint8_t)(joyval4 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY4:
            retval = joyval4 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY5:
            retval = (uint8_t)((joyval5 & 0x20) >> 2);
            retval |= (uint8_t)(joyval5 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY5:
            retval = joyval5 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY6:
            retval = (uint8_t)((joyval6 & 0x20) >> 2);
            retval |= (uint8_t)(joyval6 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY6:
            retval = joyval6 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY7:
            retval = (uint8_t)((joyval7 & 0x20) >> 2);
            retval |= (uint8_t)(joyval7 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY7:
            retval = joyval7 & 0xf;
            break;
        case INCEPTION_STATE_HI_JOY8:
            retval = (uint8_t)((joyval8 & 0x20) >> 2);
            retval |= (uint8_t)(joyval8 & 0x10) >> 4;
            break;
        case INCEPTION_STATE_LO_JOY8:
            retval = joyval8 & 0xf;
            break;
        default:
            retval = 0xff;
    }

    return retval;
}

static void inception_store(int port, uint8_t val)
{
    uint8_t new_clock = (val & 0x10) >> 4;
    uint8_t lines = val & 0x1f;

    if (!lines) {
        counter = INCEPTION_STATE_IDLE;
    } else {
        if (clock_line != new_clock) {
            if (counter < INCEPTION_STATE_EOS) {
                counter++;
            } else {
                counter = INCEPTION_STATE_IDLE;
            }
        }
    }

    clock_line = new_clock;
}

/* ------------------------------------------------------------------------- */

static int inception_write_snapshot(struct snapshot_s *s, int port);
static int inception_read_snapshot(struct snapshot_s *s, int port);

static joyport_t joyport_inception_device = {
    "Joystick Adapter (Inception)",   /* name of the device */
    JOYPORT_RES_ID_NONE,              /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,          /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,             /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_INCEPTION,    /* device is a joystick adapter */
    JOYPORT_DEVICE_JOYSTICK_ADAPTER,  /* device is a Joystick adapter */
    joyport_inception_enable,         /* device enable function */
    inception_read,                   /* digital line read function */
    inception_store,                  /* digital line store function */
    NULL,                             /* NO pot-x read function */
    NULL,                             /* NO pot-y read function */
    inception_write_snapshot,         /* device write snapshot function */
    inception_read_snapshot           /* device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_inception_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_INCEPTION, &joyport_inception_device);
}

/* ------------------------------------------------------------------------- */

/* INCEPTION snapshot module format:

   type  |   name  | description
   ----------------------------------
   BYTE  | COUNTER | counter value
   BYTE  | CLOCK   | clock line state
 */

static char snap_module_name[] = "INCEPTION";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int inception_write_snapshot(struct snapshot_s *s, int p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0 
        || SMW_B(m, counter) < 0
        || SMW_B(m, clock_line) < 0) {
            snapshot_module_close(m);
            return -1;
    }
    return snapshot_module_close(m);
}

static int inception_read_snapshot(struct snapshot_s *s, int p)
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
        || SMR_B(m, &clock_line) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
