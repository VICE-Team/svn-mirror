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

/* #define HOST_HARDWARE_IO */

/* Enable above define to be able to use host hardware I/O.
   In this mode you will need to write your own code to access
   the host hardware.
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

static int userport_io_sim_enabled = 0;

#ifndef HOST_HARDWARE_IO
static uint8_t userport_io_sim_pbx_out = 0;
static uint8_t userport_io_sim_pbx_in = 0;

/* 1 is output, 0 is input */
static uint8_t userport_io_sim_pbx_ddr = 0;

static uint8_t userport_io_sim_pax_out = 0;
static uint8_t userport_io_sim_pax_in = 0;

/* 1 is output, 0 is input */
static uint8_t userport_io_sim_pax_ddr = 0;
#endif

/* Some prototypes are needed */
#ifndef HOST_HARDWARE_IO
static uint8_t userport_io_sim_read_pbx(uint8_t orig);
static void userport_io_sim_store_pbx(uint8_t value, int pulse);
static uint8_t userport_io_sim_read_pa2(uint8_t orig);
static void userport_io_sim_store_pa2(uint8_t value);
static uint8_t userport_io_sim_read_pa3(uint8_t orig);
static void userport_io_sim_store_pa3(uint8_t value);
static int userport_io_sim_write_snapshot_module(snapshot_t *s);
static int userport_io_sim_read_snapshot_module(snapshot_t *s);
#else
static uint8_t userport_io_hw_read_pbx(uint8_t orig);
static void userport_io_hw_store_pbx(uint8_t value, int pulse);
static uint8_t userport_io_hw_read_pa2(uint8_t orig);
static void userport_io_hw_store_pa2(uint8_t value);
static uint8_t userport_io_hw_read_pa3(uint8_t orig);
static void userport_io_hw_store_pa3(uint8_t value);
#endif

static int userport_io_sim_enable(int value);

#ifndef HOST_HARDWARE_IO
static userport_device_t userport_io_sim_device = {
    "Userport I/O Simulation",             /* device name */
    JOYSTICK_ADAPTER_ID_NONE,              /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION,    /* device is an I/O simulation */
    userport_io_sim_enable,                /* enable function */
    userport_io_sim_read_pbx,              /* read pb0-pb7 function */
    userport_io_sim_store_pbx,             /* store pb0-pb7 function */
    NULL,                                  /* NO read pa2 pin function */
    NULL,                                  /* NO store pa2 pin function */
    NULL,                                  /* NO read pa3 pin function */
    NULL,                                  /* NO store pa3 pin function */
    0,                                     /* pc pin is NOT needed */
    NULL,                                  /* NO store sp1 pin function */
    NULL,                                  /* NO read sp1 pin function */
    NULL,                                  /* NO store sp2 pin function */
    NULL,                                  /* NO read sp2 pin function */
    NULL,                                  /* NO reset function */
    userport_io_sim_write_snapshot_module, /* snapshot write function */
    userport_io_sim_read_snapshot_module   /* snapshot read function */
};

static userport_device_t userport_io_sim_pa2_device = {
    "Userport I/O Simulation",             /* device name */
    JOYSTICK_ADAPTER_ID_NONE,              /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION,    /* device is an I/O simulation */
    userport_io_sim_enable,                /* enable function */
    userport_io_sim_read_pbx,              /* read pb0-pb7 function */
    userport_io_sim_store_pbx,             /* store pb0-pb7 function */
    userport_io_sim_read_pa2,              /* read pa2 pin function */
    userport_io_sim_store_pa2,             /* store pa2 pin function */
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

static userport_device_t userport_io_sim_pa23_device = {
    "Userport I/O Simulation",             /* device name */
    JOYSTICK_ADAPTER_ID_NONE,              /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION,    /* device is an I/O simulation */
    userport_io_sim_enable,                /* enable function */
    userport_io_sim_read_pbx,              /* read pb0-pb7 function */
    userport_io_sim_store_pbx,             /* store pb0-pb7 function */
    userport_io_sim_read_pa2,              /* read pa2 pin function */
    userport_io_sim_store_pa2,             /* store pa2 pin function */
    userport_io_sim_read_pa3,              /* read pa3 pin function */
    userport_io_sim_store_pa3,             /* store pa3 pin function */
    0,                                     /* pc pin is NOT needed */
    NULL,                                  /* NO store sp1 pin function */
    NULL,                                  /* NO read sp1 pin function */
    NULL,                                  /* NO store sp2 pin function */
    NULL,                                  /* NO read sp2 pin function */
    userport_io_sim_write_snapshot_module, /* snapshot write function */
    userport_io_sim_read_snapshot_module   /* snapshot read function */
};
#else
static userport_device_t userport_io_hw_device = {
    "Userport host I/O hardware",       /* device name */
    JOYSTICK_ADAPTER_ID_NONE,           /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION, /* device is an I/O simulation */
    userport_io_sim_enable,             /* enable function */
    userport_io_hw_read_pbx,            /* read pb0-pb7 function */
    userport_io_hw_store_pbx,           /* store pb0-pb7 function */
    NULL,                               /* NO read pa2 pin function */
    NULL,                               /* NO store pa2 pin function */
    NULL,                               /* NO read pa3 pin function */
    NULL,                               /* NO store pa3 pin function */
    0,                                  /* pc pin is NOT needed */
    NULL,                               /* NO store sp1 pin function */
    NULL,                               /* NO read sp1 pin function */
    NULL,                               /* NO store sp2 pin function */
    NULL,                               /* NO read sp2 pin function */
    NULL,                               /* NO snapshot write function */
    NULL                                /* NO snapshot read function */
};

static userport_device_t userport_io_hw_pa2_device = {
    "Userport host I/O hardware",       /* device name */
    JOYSTICK_ADAPTER_ID_NONE,           /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION, /* device is an I/O simulation */
    userport_io_sim_enable,             /* enable function */
    userport_io_hw_read_pbx,            /* read pb0-pb7 function */
    userport_io_hw_store_pbx,           /* store pb0-pb7 function */
    userport_io_hw_read_pa2,            /* read pa2 pin function */
    userport_io_hw_store_pa2,           /* store pa2 pin function */
    NULL,                               /* NO read pa3 pin function */
    NULL,                               /* NO store pa3 pin function */
    0,                                  /* pc pin is NOT needed */
    NULL,                               /* NO store sp1 pin function */
    NULL,                               /* NO read sp1 pin function */
    NULL,                               /* NO store sp2 pin function */
    NULL,                               /* NO read sp2 pin function */
    NULL,                               /* NO snapshot write function */
    NULL                                /* NO snapshot read function */
};

static userport_device_t userport_io_hw_pa23_device = {
    "Userport host I/O hardware",       /* device name */
    JOYSTICK_ADAPTER_ID_NONE,           /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_IO_SIMULATION, /* device is an I/O simulation */
    userport_io_sim_enable,             /* enable function */
    userport_io_hw_read_pbx,            /* read pb0-pb7 function */
    userport_io_hw_store_pbx,           /* store pb0-pb7 function */
    userport_io_hw_read_pa2,            /* read pa2 pin function */
    userport_io_hw_store_pa2,           /* store pa2 pin function */
    userport_io_hw_read_pa3,            /* read pa3 pin function */
    userport_io_hw_store_pa3,           /* store pa3 pin function */
    0,                                  /* pc pin is NOT needed */
    NULL,                               /* NO store sp1 pin function */
    NULL,                               /* NO read sp1 pin function */
    NULL,                               /* NO store sp2 pin function */
    NULL,                               /* NO read sp2 pin function */
    NULL,                               /* NO snapshot write function */
    NULL                                /* NO snapshot read function */
};
#endif

/* ------------------------------------------------------------------------- */

static int userport_io_sim_enable(int value)
{
    int val = value ? 1 : 0;

    if (userport_io_sim_enabled == val) {
        return 0;
    }

#ifndef HOST_HARDWARE_IO
    if (val) {
        userport_io_sim_pbx_ddr = 0;
        userport_io_sim_pbx_out = 0;
        userport_io_sim_pbx_in = 0;
        userport_io_sim_pax_ddr = 0;
        userport_io_sim_pax_out = 0;
        userport_io_sim_pax_in = 0;
    }
#endif

    userport_io_sim_enabled = val;
    return 0;
}

#ifndef HOST_HARDWARE_IO
int userport_io_sim_resources_init(void)
{
    userport_device_t *io_sim_device = NULL;

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_CBM6x0:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
            io_sim_device = &userport_io_sim_pa23_device;
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_VIC20:
            io_sim_device = &userport_io_sim_pa2_device;
            break;
        default:
            io_sim_device = &userport_io_sim_device;
    }

    return userport_device_register(USERPORT_DEVICE_IO_SIMULATION, io_sim_device);
}
#else
int userport_io_sim_resources_init(void)
{
    userport_device_t *io_hw_device = NULL;

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_CBM6x0:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
            io_sim_device = &userport_io_hw_pa23_device;
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_VIC20:
            io_sim_device = &userport_io_hw_pa2_device;
            break;
        default:
            io_sim_device = &userport_io_hw_device;
    }

    return userport_device_register(USERPORT_DEVICE_IO_SIMULATION, io_hw_device);
}
#endif

/* ---------------------------------------------------------------------*/

#ifdef HOST_HARDWARE_IO
static void userport_io_hw_store_pbx(uint8_t val)
{
    /* Create your own code that sets the port B lines for the
       host hardware userport */
}

static uint8_t userport_io_hw_read_pbx(uint8_t orig)
{
    /* Create your own code that reads a byte from the port B
       lines of the host hardware userport, return 'orig' if
       there is nothing to read. */
    return orig;
}

static void userport_io_hw_store_pa2(uint8_t val)
{
    /* Create your own code that sets the bit 2 of port A for the
       host hardware userport */
}

static uint8_t userport_io_hw_read_pa2(uint8_t orig)
{
    /* Create your own code that reads bit 2 of port B for the
       host hardware userport */
}

static void userport_io_hw_store_pa3(uint8_t val)
{
    /* Create your own code that sets the bit 3 of port A for the
       host hardware userport */
}

static uint8_t userport_io_hw_read_pa3(uint8_t orig)
{
    /* Create your own code that reads bit 3 of port B for the
       host hardware userport */
}
#else
static void userport_io_sim_store_pbx(uint8_t val, int pulse)
{
    userport_io_sim_pbx_in = val;
}

static uint8_t userport_io_sim_read_pbx(uint8_t orig)
{
    return userport_io_sim_pbx_out & userport_io_sim_pbx_ddr;
}

static void userport_io_sim_store_pa2(uint8_t value)
{
    uint8_t val = (value) ? 4 : 0;
 
    userport_io_sim_pax_in &= 0xfb;
    userport_io_sim_pax_in |= val;
}

static uint8_t userport_io_sim_read_pa2(uint8_t orig)
{
    return ((userport_io_sim_pax_out & userport_io_sim_pax_ddr) & 4) ? 1 : 0;
}

static void userport_io_sim_store_pa3(uint8_t value)
{
    uint8_t val = (value) ? 8 : 0;
 
    userport_io_sim_pax_in &= 0xf7;
    userport_io_sim_pax_in |= val;
}

static uint8_t userport_io_sim_read_pa3(uint8_t orig)
{
    return ((userport_io_sim_pax_out & userport_io_sim_pax_ddr) & 8) ? 1 : 0;
}

/* ---------------------------------------------------------------------*/

void userport_io_sim_set_pbx_ddr_lines(uint8_t val)
{
    uint8_t mask = 0xff;

    if (machine_class == VICE_MACHINE_C64DTV) {
        mask = 0x1f;
    }
    userport_io_sim_pbx_ddr = val & mask;
}

uint8_t userport_io_sim_get_pbx_ddr_lines(void)
{
    return userport_io_sim_pbx_ddr;
}

void userport_io_sim_set_pbx_out_lines(uint8_t val)
{
    uint8_t mask = 0xff;

    if (machine_class == VICE_MACHINE_C64DTV) {
        mask = 0x1f;
    }
    userport_io_sim_pbx_out = val & mask;
}

uint8_t userport_io_sim_get_pbx_out_lines(void)
{
    return userport_io_sim_pbx_out & userport_io_sim_pbx_ddr;
}

uint8_t userport_io_sim_get_raw_pbx_out_lines(void)
{
    return userport_io_sim_pbx_out;
}

uint8_t userport_io_sim_get_pbx_in_lines(void)
{
    return userport_io_sim_pbx_in & ~userport_io_sim_pbx_ddr;
}

uint8_t userport_io_sim_get_raw_pbx_in_lines(void)
{
    return userport_io_sim_pbx_in;
}

void userport_io_sim_set_pax_ddr_lines(uint8_t val)
{
    uint8_t mask;

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_CBM6x0:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
            mask = 0xc;
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_VIC20:
            mask = 4;
            break;
        default:
            mask = 0;
    }
    userport_io_sim_pax_ddr = val & mask;
}

uint8_t userport_io_sim_get_pax_ddr_lines(void)
{
    return userport_io_sim_pax_ddr;
}

void userport_io_sim_set_pax_out_lines(uint8_t val)
{
    uint8_t mask;

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_CBM6x0:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
            mask = 0xc;
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_VIC20:
            mask = 4;
            break;
        default:
            mask = 0;
    }
    userport_io_sim_pax_out = val & mask;
}

uint8_t userport_io_sim_get_pax_out_lines(void)
{
    return userport_io_sim_pax_out & userport_io_sim_pax_ddr;
}

uint8_t userport_io_sim_get_raw_pax_out_lines(void)
{
    return userport_io_sim_pax_out;
}

uint8_t userport_io_sim_get_pax_in_lines(void)
{
    return userport_io_sim_pax_in & ~userport_io_sim_pax_ddr;
}

uint8_t userport_io_sim_get_raw_pax_in_lines(void)
{
    return userport_io_sim_pax_in;
}

/* ---------------------------------------------------------------------*/

/* USERPORT_IO_SIM snapshot module format:

   type  | name     | description
   ------------------------------
   BYTE  | PBX IN  | port b in state
   BYTE  | PBX OUT | port b out state
   BYTE  | PBX DDR | port b direction register
   BYTE  | PAX IN  | port a in state
   BYTE  | PAX OUT | port a out state
   BYTE  | PAX DDR | port a direction register
 */

static char snap_module_name[] = "UPIOSIM";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

static int userport_io_sim_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
 
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, userport_io_sim_pbx_in) < 0)
        || (SMW_B(m, userport_io_sim_pbx_out) < 0)
        || (SMW_B(m, userport_io_sim_pbx_ddr) < 0)
        || (SMW_B(m, userport_io_sim_pax_in) < 0)
        || (SMW_B(m, userport_io_sim_pax_out) < 0)
        || (SMW_B(m, userport_io_sim_pax_ddr) < 0)) {
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
        || (SMR_B(m, &userport_io_sim_pbx_in) < 0)
        || (SMR_B(m, &userport_io_sim_pbx_out) < 0)
        || (SMR_B(m, &userport_io_sim_pbx_ddr) < 0)
        || (SMR_B(m, &userport_io_sim_pax_in) < 0)
        || (SMR_B(m, &userport_io_sim_pax_out) < 0)
        || (SMR_B(m, &userport_io_sim_pax_ddr) < 0)) {
        goto fail;
    }
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
#endif
