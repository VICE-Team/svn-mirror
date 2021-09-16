/*
 * userport_io_sim.c - Userport I/O simulation.
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

/* - Userport I/O simulation

   This device provides a way of simulating the data input and
   output lines of a userport device, can be used to test the
   userport system.
*/

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources.h"
#include "joyport.h"
#include "snapshot.h"
#include "userport.h"
#include "userport_io_sim.h"
#include "machine.h"

#ifdef IO_SIMULATION

static int userport_io_sim_enabled = 0;

static uint8_t userport_io_sim_data_out = 0;
static uint8_t userport_io_sim_data_in = 0;

/* 1 is output, 0 is input */
static uint8_t userport_io_sim_ddr = 0;

/* Some prototypes are needed */
static uint8_t userport_io_sim_read_pbx(uint8_t orig);
static void userport_io_sim_store_pbx(uint8_t value, int pulse);
static int userport_io_sim_write_snapshot_module(snapshot_t *s);
static int userport_io_sim_read_snapshot_module(snapshot_t *s);
static int userport_io_sim_enable(int value);

static userport_device_t userport_io_sim_device = {
    "Userport I/O Simulation",             /* device name */
    JOYSTICK_ADAPTER_ID_NONE,              /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION,    /* device is an I/O simulation */
    userport_io_sim_enable,                /* enable function */
    userport_io_sim_read_pbx,              /* read pb0-pb7 function */
    userport_io_sim_store_pbx,             /* store pb0-pb7 function */
    NULL,                                  /* NO read pa2 pin function */
    NULL,                                  /* store pa2 pin function */
    NULL,                                  /* NO read pa3 pin function */
    NULL,                                  /* NO store pa3 pin function */
    0,                                     /* pc pin is NOT needed */
    NULL,                                  /* NO store sp1 pin function */
    NULL,                                  /* NO read sp1 pin function */
    NULL,                                  /* NO store sp2 pin function */
    NULL,                                  /* NO read sp2 pin function */
    userport_io_sim_write_snapshot_module, /* snapshot write function */
    userport_io_sim_read_snapshot_module   /* snapshot read function */
};

/* ------------------------------------------------------------------------- */

static int userport_io_sim_enable(int value)
{
    int val = value ? 1 : 0;

    if (userport_io_sim_enabled == val) {
        return 0;
    }

    if (val) {
        userport_io_sim_ddr = 0;
        userport_io_sim_data_out = 0;
        userport_io_sim_data_in = 0;
    }

    userport_io_sim_enabled = val;
    return 0;
}

int userport_io_sim_resources_init(void)
{
    return userport_device_register(USERPORT_DEVICE_IO_SIMULATION, &userport_io_sim_device);
}

/* ---------------------------------------------------------------------*/

static void userport_io_sim_store_pbx(uint8_t val, int pulse)
{
    userport_io_sim_data_in = val;
}

static uint8_t userport_io_sim_read_pbx(uint8_t orig)
{
    return userport_io_sim_data_out & userport_io_sim_ddr;
}

/* ---------------------------------------------------------------------*/

void userport_io_sim_set_ddr_lines(uint8_t val)
{
    uint8_t mask = 0xff;

    if (machine_class == VICE_MACHINE_C64DTV) {
        mask = 0x1f;
    }
    userport_io_sim_ddr = val & mask;
}

uint8_t userport_io_sim_get_ddr_lines(void)
{
    return userport_io_sim_ddr;
}

void userport_io_sim_set_data_out_lines(uint8_t val)
{
    uint8_t mask = 0xff;

    if (machine_class == VICE_MACHINE_C64DTV) {
        mask = 0x1f;
    }
    userport_io_sim_data_out = val & mask;
}

uint8_t userport_io_sim_get_data_out_lines(void)
{
    return userport_io_sim_data_out;
}

uint8_t userport_io_sim_get_data_in_lines(void)
{
    return userport_io_sim_data_in & ~userport_io_sim_ddr;
}

/* ---------------------------------------------------------------------*/

/* USERPORT_IO_SIM snapshot module format:

   type  | name     | description
   ------------------------------
   BYTE  | DATA IN  | data in state
   BYTE  | DATA OUT | data out state
   BYTE  | DDR      | data direction register
 */

static char snap_module_name[] = "UPIOSIM";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int userport_io_sim_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
 
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, userport_io_sim_data_in) < 0)
        || (SMW_B(m, userport_io_sim_data_out) < 0)
        || (SMW_B(m, userport_io_sim_ddr) < 0)) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

static int userport_io_sim_read_snapshot_module(snapshot_t *s)
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
        || (SMR_B(m, &userport_io_sim_data_in) < 0)
        || (SMR_B(m, &userport_io_sim_data_out) < 0)
        || (SMR_B(m, &userport_io_sim_ddr) < 0)) {
        goto fail;
    }
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
#endif
