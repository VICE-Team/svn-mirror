/*
 * userport_superpad64.c - Userport SuperPad64 8-player SNES pad emulation.
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

/* - SuperPad64 SNES PAD (C64/C128/PET/VIC20)

C64/C128 | CBM2 | SNES PAD | I/O
--------------------------------
 C (PB0) | 14  |   DATA1  |  I
 D (PB1) | 13  |   DATA2  |  I
 E (PB2) | 12  |   DATA3  |  I
 F (PB3) | 11  |   DATA4  |  I
 H (PB4) | 10  |   DATA5  |  I
 J (PB5) |  9  |   DATA6  |  I
 K (PB6) |  8  |   DATA7  |  I
 L (PB7) |  7  |   DATA8  |  I
 8 (PC2) |  5  |   CLOCK  |  O
 M (PA2) |  2  |   LATCH  |  O
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
#include "snespad.h"
#include "userport.h"
#include "userport_superpad64.h"
#include "machine.h"
#include "uiapi.h"

#include "log.h"

static int userport_superpad64_enabled = 0;

static uint8_t counter = 0;

static uint8_t latch_line = 0;

/* Some prototypes are needed */
static void userport_superpad64_read_pbx(void);
static void userport_superpad64_store_pa2(uint8_t value);
static int userport_superpad64_write_snapshot_module(snapshot_t *s);
static int userport_superpad64_read_snapshot_module(snapshot_t *s);

static userport_device_t userport_superpad64_device = {
    USERPORT_DEVICE_SUPERPAD64,              /* device id */
    "Userport SuperPad64",                   /* device name */
    JOYSTICK_ADAPTER_ID_USERPORT_SUPERPAD64, /* this is a joystick adapter */
    userport_superpad64_read_pbx,            /* read pb0-pb7 function */
    NULL,                                    /* NO store pb0-pb7 function */
    NULL,                                    /* NO read pa2 pin function */
    userport_superpad64_store_pa2,           /* store pa2 pin function */
    NULL,                                    /* NO read pa3 pin function */
    NULL,                                    /* NO store pa3 pin function */
    1,                                       /* pc pin IS needed */
    NULL,                                    /* NO store sp1 pin function */
    NULL,                                    /* NO read sp1 pin function */
    NULL,                                    /* NO store sp2 pin function */
    NULL,                                    /* NO read sp2 pin function */
    "UserportSuperPad64",                    /* resource used by the device */
    0xff,                                    /* return value from a read, to be filled in by the device */
    0xff,                                    /* validity mask of the device, does not change */
    0,                                       /* device involved in a read collision, to be filled in by the collision detection system */
    0                                        /* a tag to indicate the order of insertion */
};

static userport_device_list_t *userport_superpad64_list_item = NULL;

static userport_snapshot_t superpad64_snapshot = {
    USERPORT_DEVICE_SUPERPAD64,
    userport_superpad64_write_snapshot_module,
    userport_superpad64_read_snapshot_module
};

/* ------------------------------------------------------------------------- */

static int set_userport_superpad64_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (userport_superpad64_enabled == val) {
        return 0;
    }

    if (val) {
        /* check if a different joystick adapter is already active */
        if (joystick_adapter_get_id()) {
            ui_error("%s is a joystick adapter, but joystick adapter %s is already active", userport_superpad64_device.name, joystick_adapter_get_name());
            return -1;
        }
        counter = 0;
        userport_superpad64_list_item = userport_device_register(&userport_superpad64_device);
        if (userport_superpad64_list_item == NULL) {
            return -1;
        }
        joystick_adapter_activate(JOYSTICK_ADAPTER_ID_USERPORT_SUPERPAD64, userport_superpad64_device.name);
        joystick_adapter_set_ports(8);
    } else {
        userport_device_unregister(userport_superpad64_list_item);
        userport_superpad64_list_item = NULL;
        joystick_adapter_deactivate();
    }

    userport_superpad64_enabled = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "UserportSuperPad64", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &userport_superpad64_enabled, set_userport_superpad64_enabled, NULL },
    RESOURCE_INT_LIST_END
};

int userport_superpad64_resources_init(void)
{
    userport_snapshot_register(&superpad64_snapshot);

    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-userportsuperpad64", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "UserportSuperPad64", (resource_value_t)1,
      NULL, "Enable Userport SuperPad64" },
    { "+userportsuperpad64", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "UserportSuperPad64", (resource_value_t)0,
      NULL, "Disable Userport SuperPad64" },
    CMDLINE_LIST_END
};

int userport_superpad64_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static void userport_superpad64_store_pa2(uint8_t value)
{
    uint8_t new_latch = 0;

    new_latch = value & 1;

    if (latch_line && !new_latch) {
        counter = 0;
    }

    latch_line = new_latch;
}

static void userport_superpad64_read_pbx(void)
{
    uint8_t retval;
    uint16_t portval1 = get_joystick_value(JOYPORT_3);
    uint16_t portval2 = get_joystick_value(JOYPORT_4);
    uint16_t portval3 = get_joystick_value(JOYPORT_5);
    uint16_t portval4 = get_joystick_value(JOYPORT_6);
    uint16_t portval5 = get_joystick_value(JOYPORT_7);
    uint16_t portval6 = get_joystick_value(JOYPORT_8);
    uint16_t portval7 = get_joystick_value(JOYPORT_9);
    uint16_t portval8 = get_joystick_value(JOYPORT_10);

    switch (counter) {
        case SNESPAD_BUTTON_A:
            retval = (uint8_t)((portval1 & 0x10) >> 4);
            retval |= (uint8_t)((portval2 & 0x10) >> 3);
            retval |= (uint8_t)((portval3 & 0x10) >> 2);
            retval |= (uint8_t)((portval4 & 0x10) >> 1);
            retval |= (uint8_t)(portval5 & 0x10);
            retval |= (uint8_t)((portval6 & 0x10) << 1);
            retval |= (uint8_t)((portval7 & 0x10) << 2);
            retval |= (uint8_t)((portval8 & 0x10) << 3);
            break;
        case SNESPAD_BUTTON_B:
            retval = (uint8_t)((portval1 & 0x20) >> 5);
            retval |= (uint8_t)((portval2 & 0x20) >> 4);
            retval |= (uint8_t)((portval3 & 0x20) >> 3);
            retval |= (uint8_t)((portval4 & 0x20) >> 2);
            retval |= (uint8_t)((portval5 & 0x20) >> 1);
            retval |= (uint8_t)(portval6 & 0x20);
            retval |= (uint8_t)((portval7 & 0x20) << 1);
            retval |= (uint8_t)((portval8 & 0x20) << 2);
            break;
        case SNESPAD_BUTTON_X:
            retval = (uint8_t)((portval1 & 0x40) >> 6);
            retval |= (uint8_t)((portval2 & 0x40) >> 5);
            retval |= (uint8_t)((portval3 & 0x40) >> 4);
            retval |= (uint8_t)((portval4 & 0x40) >> 3);
            retval |= (uint8_t)((portval5 & 0x40) >> 2);
            retval |= (uint8_t)((portval6 & 0x40) >> 1);
            retval |= (uint8_t)(portval7 & 0x40);
            retval |= (uint8_t)((portval8 & 0x40) << 1);
            break;
        case SNESPAD_BUTTON_Y:
            retval = (uint8_t)((portval1 & 0x80) >> 7);
            retval |= (uint8_t)((portval2 & 0x80) >> 6);
            retval |= (uint8_t)((portval3 & 0x80) >> 5);
            retval |= (uint8_t)((portval4 & 0x80) >> 4);
            retval |= (uint8_t)((portval5 & 0x80) >> 3);
            retval |= (uint8_t)((portval6 & 0x80) >> 2);
            retval |= (uint8_t)((portval7 & 0x80) >> 1);
            retval |= (uint8_t)(portval8 & 0x80);
            break;
        case SNESPAD_BUMPER_LEFT:
            retval = (uint8_t)((portval1 & 0x100) >> 8);
            retval |= (uint8_t)((portval2 & 0x100) >> 7);
            retval |= (uint8_t)((portval3 & 0x100) >> 6);
            retval |= (uint8_t)((portval4 & 0x100) >> 5);
            retval |= (uint8_t)((portval5 & 0x100) >> 4);
            retval |= (uint8_t)((portval6 & 0x100) >> 3);
            retval |= (uint8_t)((portval7 & 0x100) >> 2);
            retval |= (uint8_t)((portval8 & 0x100) >> 1);
            break;
        case SNESPAD_BUMPER_RIGHT:
            retval = (uint8_t)((portval1 & 0x200) >> 9);
            retval |= (uint8_t)((portval2 & 0x200) >> 8);
            retval |= (uint8_t)((portval3 & 0x200) >> 7);
            retval |= (uint8_t)((portval4 & 0x200) >> 6);
            retval |= (uint8_t)((portval5 & 0x200) >> 5);
            retval |= (uint8_t)((portval6 & 0x200) >> 4);
            retval |= (uint8_t)((portval7 & 0x200) >> 3);
            retval |= (uint8_t)((portval8 & 0x200) >> 2);
            break;
        case SNESPAD_BUTTON_SELECT:
            retval = (uint8_t)((portval1 & 0x400) >> 10);
            retval |= (uint8_t)((portval2 & 0x400) >> 9);
            retval |= (uint8_t)((portval3 & 0x400) >> 8);
            retval |= (uint8_t)((portval4 & 0x400) >> 7);
            retval |= (uint8_t)((portval5 & 0x400) >> 6);
            retval |= (uint8_t)((portval6 & 0x400) >> 5);
            retval |= (uint8_t)((portval7 & 0x400) >> 4);
            retval |= (uint8_t)((portval8 & 0x400) >> 3);
            break;
        case SNESPAD_BUTTON_START:
            retval = (uint8_t)((portval1 & 0x800) >> 11);
            retval |= (uint8_t)((portval2 & 0x800) >> 10);
            retval |= (uint8_t)((portval3 & 0x800) >> 9);
            retval |= (uint8_t)((portval4 & 0x800) >> 8);
            retval |= (uint8_t)((portval5 & 0x800) >> 7);
            retval |= (uint8_t)((portval6 & 0x800) >> 6);
            retval |= (uint8_t)((portval7 & 0x800) >> 5);
            retval |= (uint8_t)((portval8 & 0x800) >> 4);
            break;
        case SNESPAD_UP:
            retval = (uint8_t)(portval1 & 1);
            retval |= (uint8_t)((portval2 & 1) << 1);
            retval |= (uint8_t)((portval3 & 1) << 2);
            retval |= (uint8_t)((portval4 & 1) << 3);
            retval |= (uint8_t)((portval5 & 1) << 4);
            retval |= (uint8_t)((portval6 & 1) << 5);
            retval |= (uint8_t)((portval7 & 1) << 6);
            retval |= (uint8_t)((portval8 & 1) << 7);
            break;
        case SNESPAD_DOWN:
            retval = (uint8_t)((portval1 & 2) >> 1);
            retval |= (uint8_t)(portval2 & 2);
            retval |= (uint8_t)((portval3 & 2) << 1);
            retval |= (uint8_t)((portval4 & 2) << 2);
            retval |= (uint8_t)((portval5 & 2) << 3);
            retval |= (uint8_t)((portval6 & 2) << 4);
            retval |= (uint8_t)((portval7 & 2) << 5);
            retval |= (uint8_t)((portval8 & 2) << 6);
            break;
        case SNESPAD_LEFT:
            retval = (uint8_t)((portval1 & 4) >> 2);
            retval |= (uint8_t)((portval2 & 4) >> 1);
            retval |= (uint8_t)(portval3 & 4);
            retval |= (uint8_t)((portval4 & 4) << 1);
            retval |= (uint8_t)((portval5 & 4) << 2);
            retval |= (uint8_t)((portval6 & 4) << 3);
            retval |= (uint8_t)((portval7 & 4) << 4);
            retval |= (uint8_t)((portval8 & 4) << 5);
           break;
        case SNESPAD_RIGHT:
            retval = (uint8_t)((portval1 & 8) >> 3);
            retval |= (uint8_t)((portval2 & 8) >> 2);
            retval |= (uint8_t)((portval3 & 8) >> 1);
            retval |= (uint8_t)(portval4 & 8);
            retval |= (uint8_t)((portval5 & 8) << 1);
            retval |= (uint8_t)((portval6 & 8) << 2);
            retval |= (uint8_t)((portval7 & 8) << 3);
            retval |= (uint8_t)((portval8 & 8) << 4);
            break;
        case SNESPAD_BIT_12_1:
        case SNESPAD_BIT_13_1:
        case SNESPAD_BIT_14_1:
        case SNESPAD_BIT_15_1:
            retval = 0x00;
            break;
        case SNESPAD_EOS:
            retval = 0xff;
            break;
        default:
            retval = 0xff;
    }
    userport_superpad64_device.retval = ~retval;

    if (counter < SNESPAD_EOS) {
        counter++;
    }
}

/* ---------------------------------------------------------------------*/

/* USERPORT_SUPERPAD64 snapshot module format:

   type  | name    | description
   -----------------------------
   BYTE  | COUNTER | current count
   BYTE  | LATCH   | latch line state
 */

static char snap_module_name[] = "USERPORT_SUPERPAD64";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int userport_superpad64_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
 
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, counter) < 0)
        || (SMW_B(m, latch_line) < 0)) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

static int userport_superpad64_read_snapshot_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    /* enable device */
    set_userport_superpad64_enabled(1, NULL);

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
        || (SMR_B(m, &counter) < 0)
        || (SMR_B(m, &latch_line) < 0)) {
        goto fail;
    }
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
