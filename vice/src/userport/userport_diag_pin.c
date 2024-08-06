/*
 * userport_diag_pin.c: Userport PET diag pin
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "resources.h"
#include "joyport.h"
#include "userport.h"
#include "userport_diag_pin.h"

static int userport_diag_pin_read_snapshot_module(snapshot_t *s);
static int userport_diag_pin_write_snapshot_module(snapshot_t *s);

static int diagnostic_pin_enabled = 0;

static uint8_t userport_diag_pin_read_sp1(uint8_t orig)
{
    /*printf("userport_diag_pin_read_sp1: %d\n", diagnostic_pin_enabled);*/
    return diagnostic_pin_enabled;
}

static userport_device_t diag_pin_device = {
    "PET diagnostic pin",                           /* device name */
    JOYSTICK_ADAPTER_ID_NONE,                       /* NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_DIAGNOSTIC,                /* device is a diagnostic device */
    NULL,                                           /* NO enable function */
    NULL,                                           /* NO read pb0-pb7 function */
    NULL,                                           /* NO store pb0-pb7 function */
    NULL,                                           /* NO read pa2 pin function */
    NULL,                                           /* NO store pa2 pin function */
    NULL,                                           /* NO read pa3 pin function */
    NULL,                                           /* NO store pa3 pin function */
    0,                                              /* pc pin is NOT needed */
    NULL,                                           /* NO store sp1 pin function */
    userport_diag_pin_read_sp1,                     /* read sp1 pin function */
    NULL,                                           /* NO store sp2 pin function */
    NULL,                                           /* NO read sp2 pin function */
    NULL,                                           /* NO reset function */
    NULL,                                           /* NO powerup function */
    userport_diag_pin_write_snapshot_module,        /* snapshot write function */
    userport_diag_pin_read_snapshot_module          /* snapshot read function */
};

static int set_diagnostic_pin_enabled(int val, void *param)
{
    diagnostic_pin_enabled = val ? 1 : 0;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "DiagPin", 0, RES_EVENT_SAME, NULL,
      &diagnostic_pin_enabled, set_diagnostic_pin_enabled, NULL },
    RESOURCE_INT_LIST_END
};

int userport_diag_pin_resources_init(void)
{
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }
    return userport_device_register(USERPORT_DEVICE_DIAGNOSTIC_PIN, &diag_pin_device);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-diagpin", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "DiagPin", (resource_value_t)1,
      NULL, "Enable userport diagnostic pin" },
    { "+diagpin", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "DiagPin", (resource_value_t)1,
      NULL, "Disable userport diagnostic pin" },
    CMDLINE_LIST_END
};

int userport_diag_pin_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

/* USERPORT_DIGIMAX snapshot module format:

   type  | name    | description
   -----------------------------
   BYTE  | address | current state of the diag pin
 */

static const char snap_module_name[] = "UPDIAGPIN";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

static int userport_diag_pin_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, (uint8_t)diagnostic_pin_enabled) < 0)
        ) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

static int userport_diag_pin_read_snapshot_module(snapshot_t *s)
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
        || (SMR_B_INT(m, &diagnostic_pin_enabled) < 0)
        ) {
        goto fail;
    }
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

