/*
 * protopad.c - Protopad emulation.
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
#include "protopad.h"

#include "log.h"

/* Control port <--> Protopad connections:

   Compatibility mode:

   cport |  pin name | protopad   | I/O
   ------------------------------------
     1   |  JOY0     | dpad up    |  I
     2   |  JOY1     | dpad down  |  I
     3   |  JOY2     | dpad left  |  I
     4   |  JOY3     | dpad right |  I
     5   |  POTY     | A button   |  I
     6   |  JOY4     | Y button   |  I
     9   |  POTX     | X button   |  I

   in compatibility mode the following extra mappings exist:

   Select toggles dpad up on/off
   Start toggles permanent rapid fire on/off
   L button cycles through the rapid fire speeds
   R button rapid fires which ever fire button is pressed at the same time


   Native mode:

   cport |  pin name | protopad    | I/O
   -------------------------------------
     1   |  JOY0     | data bit 0  |  I
     2   |  JOY1     | data bit 1  |  I
     3   |  JOY2     | data bit 2  |  I
     4   |  JOY3     | clock       |  O
     6   |  JOY4     | native mode |  O

    Keeping JOY4 low enables native mode
 */

static int protopad_enabled[JOYPORT_MAX_PORTS] = {0};

static uint8_t counter[JOYPORT_MAX_PORTS] = {0};

static uint8_t clock_line[JOYPORT_MAX_PORTS] = {0};
static uint8_t mode_line[JOYPORT_MAX_PORTS] = {0};
static uint8_t dpad_mode[JOYPORT_MAX_PORTS] = {0};
static uint8_t rapid_button[JOYPORT_MAX_PORTS] = {0};
static uint8_t permanent_rapid[JOYPORT_MAX_PORTS] = {0};
static uint8_t rapid_speed[JOYPORT_MAX_PORTS] = {0};

/* ------------------------------------------------------------------------- */

static joyport_t joyport_protopad_device;

static int joyport_protopad_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (val == protopad_enabled[port]) {
        return 0;
    }

    if (val) {
        counter[port] = 0;
        clock_line[port] = 0;
        mode_line[port] = 1;
        dpad_mode[port] = 0;
        rapid_button[port] = 0;
        permanent_rapid[port] = 0;
        rapid_speed[port] = 0;
    }

    protopad_enabled[port] = val;

    return 0;
}

/* TODO: handle extra buttons in compat mode */
static uint8_t protopad_read(int port)
{
    uint8_t retval;
    uint16_t joyval = get_joystick_value(port);

    /* if the mode line is high we have compat mode, if low we have native mode */
    if (mode_line[port]) {
        retval = (uint8_t)(joyval & 0x1f);
    } else {
        switch (counter[port]) {
            case PROTOPAD_TRIPPLE_0: /* return B A Right */
                retval = (uint8_t)((joyval & 0x38) >> 3);
                break;
            case PROTOPAD_TRIPPLE_1: /* return Left Down Up */
                retval = (uint8_t)(joyval & 0x07);
                break;
            case PROTOPAD_TRIPPLE_2: /* return Start Select Bumber-R */
                retval = (uint8_t)((joyval & 0xE00) >> 9);
                break;
            case PROTOPAD_TRIPPLE_3: /* return Bumber-L Y X */
                retval = (uint8_t)((joyval & 0x1C0) >> 6);
                break;
            default:
                retval = 0xff;
        }
    }

    return ~(retval & 0x1f);
}

static void protopad_store(int port, uint8_t val)
{
    uint8_t new_mode = (val & 0x10) >> 4;
    uint8_t new_clock = (val & 0x08) >> 3;

    if (clock_line[port] != new_clock) {
        counter[port]++;
        if (counter[port] == PROTOPAD_COUNT_MAX) {
            counter[port] = PROTOPAD_TRIPPLE_0;
        }
    }

    if (mode_line[port] && !new_mode) {
        if (!new_mode) {
            counter[port] = PROTOPAD_HANDSHAKE;
        }
    }

    mode_line[port] = new_mode;
    clock_line[port] = new_clock;
}

static uint8_t protopad_read_potx(int port)
{
    return (uint8_t)(get_joystick_value(port) & 0x20 ? 0x00 : 0xff);
}

static uint8_t protopad_read_poty(int port)
{
    return (uint8_t)(get_joystick_value(port) & 0x40 ? 0x00 : 0xff);
}

/* ------------------------------------------------------------------------- */

static int protopad_write_snapshot(struct snapshot_s *s, int p);
static int protopad_read_snapshot(struct snapshot_s *s, int p);

static joyport_t joyport_protopad_device = {
    "Protopad",                  /* name of the device */
    JOYPORT_RES_ID_NONE,         /* device can be used in multiple ports at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,     /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,        /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,    /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_SNES_ADAPTER, /* device is a SNES adapter */
    joyport_protopad_enable,     /* device enable function */
    protopad_read,               /* digital line read function */
    protopad_store,              /* digital line store function */
    protopad_read_potx,          /* pot-x read function */
    protopad_read_poty,          /* pot-y read function */
    protopad_write_snapshot,     /* device write snapshot function */
    protopad_read_snapshot       /* device read snapshot function */
};

/* ------------------------------------------------------------------------- */

int joyport_protopad_resources_init(void)
{
    return joyport_device_register(JOYPORT_ID_PROTOPAD, &joyport_protopad_device);
}

/* ------------------------------------------------------------------------- */

/* PROTOPAD snapshot module format:

   type  |   name      | description
   --------------------------------------
   BYTE  | COUNTER     | counter value
   BYTE  | CLOCK       | clock line state
   BYTE  | MODE        | mode line state
   BYTE  | DPADMODE    | dpad up mode state
   BYTE  | PERMRAPID   | permanent rapid mode state
   BYTE  | RAPIDSPEED  | rapid speed selection
 */

static char snap_module_name[] = "PROTOPAD";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int protopad_write_snapshot(struct snapshot_s *s, int p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0 
        || SMW_B(m, counter[p]) < 0
        || SMW_B(m, clock_line[p]) < 0
        || SMW_B(m, mode_line[p]) < 0
        || SMW_B(m, dpad_mode[p]) < 0
        || SMW_B(m, permanent_rapid[p]) < 0
        || SMW_B(m, rapid_speed[p]) < 0) {
            snapshot_module_close(m);
            return -1;
    }
    return snapshot_module_close(m);
}

static int protopad_read_snapshot(struct snapshot_s *s, int p)
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
        || SMR_B(m, &counter[p]) < 0
        || SMR_B(m, &clock_line[p]) < 0
        || SMR_B(m, &mode_line[p]) < 0
        || SMR_B(m, &dpad_mode[p]) < 0
        || SMR_B(m, &permanent_rapid[p]) < 0
        || SMR_B(m, &rapid_speed[p]) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
