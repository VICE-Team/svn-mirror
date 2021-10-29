/*
 * userport_wic64.c - Userport WiC64 wifi interface emulation.
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

#define DEBUG_WIC64

/* - WiC64 (C64/C128)

C64/C128   |  I/O
-----------------
 C (PB0)   |  I/O   databits from/to C64
 D (PB1)   |  I/O
 E (PB2)   |  I/O
 F (PB3)   |  I/O
 H (PB4)   |  I/O
 J (PB5)   |  I/O
 K (PB6)   |  I/O
 L (PB7)   |  I/O
 8 (PC2)   |  O     C64 triggers PC2 IRQ whenever data is read or write
 M (PA2)   |  O     Low=device sends data High=C64 sends data (powerup=high)
 B (FLAG2) |  I     device asserts high->low transition when databyte sent to c64 is ready (triggers irq)
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
#include "userport_wic64.h"
#include "machine.h"
#include "uiapi.h"

#include "log.h"

#ifdef DEBUG_WIC64
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#ifdef USERPORT_EXPERIMENTAL_DEVICES

static int userport_wic64_enabled = 0;

/* Some prototypes are needed */
static uint8_t userport_wic64_read_pbx(uint8_t orig);
static void userport_wic64_store_pbx(uint8_t val, int pulse);
static void userport_wic64_store_pa2(uint8_t value);
static int userport_wic64_write_snapshot_module(snapshot_t *s);
static int userport_wic64_read_snapshot_module(snapshot_t *s);
static int userport_wic64_enable(int value);

static userport_device_t userport_wic64_device = {
    "Userport WiC64",                     /* device name */
    JOYSTICK_ADAPTER_ID_NONE,             /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_WIFI,            /* device is a joystick adapter */
    userport_wic64_enable,                /* enable function */
    userport_wic64_read_pbx,              /* read pb0-pb7 function */
    userport_wic64_store_pbx,             /* store pb0-pb7 function */
    NULL,                                 /* NO read pa2 pin function */
    userport_wic64_store_pa2,             /* store pa2 pin function */
    NULL,                                 /* NO read pa3 pin function */
    NULL,                                 /* NO store pa3 pin function */
    1,                                    /* pc pin IS needed */
    NULL,                                 /* NO store sp1 pin function */
    NULL,                                 /* NO read sp1 pin function */
    NULL,                                 /* NO store sp2 pin function */
    NULL,                                 /* NO read sp2 pin function */
    NULL,                                 /* NO reset function */
    userport_wic64_write_snapshot_module, /* snapshot write function */
    userport_wic64_read_snapshot_module   /* snapshot read function */
};

/* ------------------------------------------------------------------------- */

static int userport_wic64_enable(int value)
{
    int val = value ? 1 : 0;

    if (userport_wic64_enabled == val) {
        return 0;
    }

    userport_wic64_enabled = val;
    return 0;
}

int userport_wic64_resources_init(void)
{
    return userport_device_register(USERPORT_DEVICE_WIC64, &userport_wic64_device);
}

/* ---------------------------------------------------------------------*/

uint8_t input_state = 0, input_command = 0, commandptr = 0;
uint8_t wic64_ddr = 1;
uint16_t input_length = 0;
uint8_t commandbuffer[0x100];

static void userport_wic64_store_pa2(uint8_t value)
{
    DBG(("userport_wic64_store_pa2 val:%02x\n", value));
    wic64_ddr = value;
}

static void do_command(void)
{
    switch (input_command) {
        case 0x01: /* http get */
        case 0x02: /* set wlan ssid + password */
        case 0x03: /* standard firmware update */
        case 0x04: /* developer firmware update */
        case 0x05: /* developer special update */
        case 0x06: /* get wic64 ip address */
        case 0x07: /* get firmware stats */
        case 0x08: /* set default server */
        case 0x09: /* prints output to serial console */
        case 0x0a: /* get udp package */
        case 0x0b: /* send udp package */
        case 0x0c: /* get list of all detected wlan ssids */
        case 0x0d: /* set wlan via scan id */
        case 0x0e: /* set udp port */
        case 0x0f: /* send http with decoded url for PHP */
        case 0x10: /* get connected wlan name */
        case 0x11: /* get wlan rssi signal level */
        case 0x12: /* get default server */
        case 0x13: /* get external ip address */
        case 0x14: /* get wic64 MAC address */
        case 0x63: /* factory reset */
        default:
            log_error(LOG_DEFAULT, "WiC64: unsupported command 0x%02x", input_command);
            input_state = 0;
        break;
    }
}

static void userport_wic64_store_pbx(uint8_t value, int pulse)
{
    DBG(("userport_wic64_store_pbx val:%02x pulse:%d\n", value, pulse));
    if (pulse) {
        switch (input_state) {
            case 0:
                input_length = 0;
                commandptr = 0;
                if (value == 0x57) {    /* 'w' */
                    input_state++;
                }
            break;
            case 1: /* lenght low byte */
                input_length = value;
                input_state++;
            break;
            case 2: /* lenght high byte */
                input_length |= (value << 8);
                input_state++;
            break;
            case 3: /* command */
                input_command = value;
                input_state++;
            break;
            default:    /* additional data depending on command */
                if ((commandptr + 4) < input_length) {
                    commandbuffer[commandptr] = value;
                    commandptr++;
                }
            break;
        }

        DBG(("input_state: %d input_length: %d input_command: %02x\n",
             input_state, input_length, input_command));

        if ((input_state == 4) && ((commandptr + 4) >= input_length)) {
            do_command();
            commandptr = input_command = input_state = input_length = 0;
        }

    }
}

static uint8_t userport_wic64_read_pbx(uint8_t orig)
{
    uint8_t retval = 0;
    DBG(("userport_wic64_read_pbx orig:%02x\n", orig));
    return (~retval);
}

/* ---------------------------------------------------------------------*/

/* USERPORT_WIC64 snapshot module format:

   type  | name           | description
   -----------------------------
   BYTE  | input_state    |
   BYTE  | input_length   |
 */

static char snap_module_name[] = "UPWIC64";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

static int userport_wic64_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
 
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, input_state) < 0)
        || (SMW_B(m, input_command) < 0)) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

static int userport_wic64_read_snapshot_module(snapshot_t *s)
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
        || (SMR_B(m, &input_state) < 0)
        || (SMR_B(m, &input_command) < 0)) {
        goto fail;
    }
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

#endif /* USERPORT_EXPERIMENTAL_DEVICES */

