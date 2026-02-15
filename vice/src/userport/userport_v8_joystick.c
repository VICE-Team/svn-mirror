/*
 * userport_v8_joystick.c - Userport V8 8-joy joystick adapter emulation.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

/*
  C64  | I/O | NOTES
--------------------

   B   |  O  | PB0 -> SELECT bit0
   K   |  O  | PB1 -> SELECT bit1
   4   |  O  | PB2 -> SELECT bit2
   5   |  I  | PB3 <- JOY1/2/3/4/5/6/8 UP
   6   |  I  | PB4 <- JOY1/2/3/4/5/6/8 DOWN
   7   |  I  | PB5 <- JOY1/2/3/4/5/6/8 LEFT
   J   |  I  | PB6 <- JOY1/2/3/4/5/6/8 RIGHT
   F   |  I  | PB7 <- JOY1/2/3/4/5/6/8 FIRE
*/

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "resources.h"
#include "joyport.h"
#include "joystick.h"
#include "snapshot.h"
#include "userport.h"
#include "userport_v8_joystick.h"
#include "machine.h"
#include "uiapi.h"

#include "log.h"

static int userport_joy_v8_enable = 0;
static int userport_joystick_v8_select = 0;

/* Some prototypes are needed */
static uint8_t userport_joystick_v8_read_pbx(uint8_t orig);
static void userport_joystick_v8_store_pbx(uint8_t value, int pulse);
static int userport_joystick_v8_write_snapshot_module(snapshot_t *s);
static int userport_joystick_v8_read_snapshot_module(snapshot_t *s);
static int userport_joystick_v8_enable(int value);

static userport_device_t v8_device = {
    "V8 userport joy adapter",                  /* device name */
    JOYSTICK_ADAPTER_ID_GENERIC_USERPORT,       /* this is a joystick adapter */
    USERPORT_DEVICE_TYPE_JOYSTICK_ADAPTER,      /* device is a joystick adapter */
    userport_joystick_v8_enable,                /* enable function */
    userport_joystick_v8_read_pbx,              /* read pb0-pb7 function */
    userport_joystick_v8_store_pbx,             /* store pb0-pb7 function */
    NULL,                                       /* NO read pa2 pin function */
    NULL,                                       /* NO store pa2 pin function */
    NULL,                                       /* NO read pa3 pin function */
    NULL,                                       /* NO store pa3 pin function */
    0,                                          /* pc pin is NOT needed */
    NULL,                                       /* NO store sp1 pin function */
    NULL,                                       /* NO read sp1 pin function */
    NULL,                                       /* NO store sp2 pin function */
    NULL,                                       /* NO read sp2 pin function */
    NULL,                                       /* NO reset function */
    NULL,                                       /* NO powerup function */
    userport_joystick_v8_write_snapshot_module, /* snapshot write function */
    userport_joystick_v8_read_snapshot_module   /* snapshot read function */
};

/* ------------------------------------------------------------------------- */

static int userport_joystick_v8_enable(int value)
{
    int val = value ? 1 : 0;

    if (userport_joy_v8_enable == val) {
        return 0;
    }

    if (val) {
        /* check if a different joystick adapter is already active */
        if (joystick_adapter_get_id()) {
            ui_error("Joystick adapter %s is already active", joystick_adapter_get_name());
            return -1;
        }
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_GENERIC_USERPORT, "Userport Synergy joystick adapter");

        /* Enable 8 extra joystick ports, without +5VDC support */
        joystick_adapter_set_ports(8, 0);
    } else {
        joystick_adapter_deactivate();
    }

    userport_joy_v8_enable = val;

    return 0;
}

int userport_joystick_v8_resources_init(void)
{
    return userport_device_register(USERPORT_DEVICE_JOYSTICK_V8, &v8_device);
}

/* ---------------------------------------------------------------------*/

static uint8_t userport_joystick_v8_read_pbx(uint8_t orig)
{
    uint8_t retval;
    uint8_t jv3 = ~read_joyport_dig(JOYPORT_3);
    uint8_t jv4 = ~read_joyport_dig(JOYPORT_4);
    uint8_t jv5 = ~read_joyport_dig(JOYPORT_5);
    uint8_t jv6 = ~read_joyport_dig(JOYPORT_6);
    uint8_t jv7 = ~read_joyport_dig(JOYPORT_7);
    uint8_t jv8 = ~read_joyport_dig(JOYPORT_8);
    uint8_t jv9 = ~read_joyport_dig(JOYPORT_9);
    uint8_t jv10 = ~read_joyport_dig(JOYPORT_10);

    switch (userport_joystick_v8_select) {
        case 0:
            retval = (uint8_t)~(jv3 & 0x1f);
            break;
        case 1:
            retval = (uint8_t)~(jv4 & 0x1f);
            break;
        case 2:
            retval = (uint8_t)~(jv5 & 0x1f);
            break;
        case 3:
            retval = (uint8_t)~(jv6 & 0x1f);
            break;
        case 4:
            retval = (uint8_t)~(jv7 & 0x1f);
            break;
        case 5:
            retval = (uint8_t)~(jv8 & 0x1f);
            break;
        case 6:
            retval = (uint8_t)~(jv9 & 0x1f);
            break;
        case 7:
            retval = (uint8_t)~(jv10 & 0x1f);
            break;
    }

    return (retval << 3);
}

static void userport_joystick_v8_store_pbx(uint8_t value, int pulse)
{
    /*uint8_t j1 = value & 0xf8;*/

    userport_joystick_v8_select = value & 7;
}

/* ---------------------------------------------------------------------*/

/* UP_JOY_V8 snapshot module format:

   type  | name   | description
   ----------------------------
   BYTE  | select | joyport select
 */

static const char v8_module_name[] = "UPJOYV8";
#define V8_VER_MAJOR   0
#define V8_VER_MINOR   1

static int userport_joystick_v8_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, v8_module_name, V8_VER_MAJOR, V8_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (SMW_B(m, (uint8_t)userport_joystick_v8_select) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    if (0
        || joyport_snapshot_write_module(s, JOYPORT_3) < 0
        || joyport_snapshot_write_module(s, JOYPORT_4) < 0
        || joyport_snapshot_write_module(s, JOYPORT_5) < 0
        || joyport_snapshot_write_module(s, JOYPORT_6) < 0
        || joyport_snapshot_write_module(s, JOYPORT_7) < 0
        || joyport_snapshot_write_module(s, JOYPORT_8) < 0
        || joyport_snapshot_write_module(s, JOYPORT_9) < 0
        || joyport_snapshot_write_module(s, JOYPORT_10) < 0) {
        return -1;
    }
    return 0;
}

static int userport_joystick_v8_read_snapshot_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, v8_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, V8_VER_MAJOR, V8_VER_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (SMR_B_INT(m, &userport_joystick_v8_select) < 0) {
        goto fail;
    }
    snapshot_module_close(m);

    if (0
        || joyport_snapshot_read_module(s, JOYPORT_3) < 0
        || joyport_snapshot_read_module(s, JOYPORT_4) < 0
        || joyport_snapshot_read_module(s, JOYPORT_5) < 0
        || joyport_snapshot_read_module(s, JOYPORT_6) < 0
        || joyport_snapshot_read_module(s, JOYPORT_7) < 0
        || joyport_snapshot_read_module(s, JOYPORT_8) < 0
        || joyport_snapshot_read_module(s, JOYPORT_9) < 0
        || joyport_snapshot_read_module(s, JOYPORT_10) < 0) {
        return -1;
    }
    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}
