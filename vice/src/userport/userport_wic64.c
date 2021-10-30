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
#define DBG(x) log_debug x
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
static void userport_wic64_reset(void);

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
    userport_wic64_reset,                 /* reset function */
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

#define FLAG2_ACTIVE    1
#define FLAG2_INACTIVE  0

uint8_t input_state = 0, input_command = 0, commandptr = 0;
uint8_t wic64_ddr = 1;
uint16_t input_length = 0, reply_length = 0;
uint8_t commandbuffer[0x100];

uint8_t replybuffer[0x100];
uint8_t replyptr = 0;
uint8_t reply_port_value = 0;

static void userport_wic64_reset(void)
{
    /* DBG(("userport_wic64_reset")); */
    commandptr = input_command = input_state = input_length = 0;
    wic64_ddr = 1;
}

static void handshake_flag2(void)
{
    set_userport_flag(FLAG2_ACTIVE);
    set_userport_flag(FLAG2_INACTIVE);
}

static void reply_next_byte(void)
{
    if (replyptr <= reply_length) {
        reply_port_value = replybuffer[replyptr];
        /* DBG(("reply_next_byte: %02x", reply_port_value)); */
        replyptr++;
    } else {
        reply_length = 0;
    }
}

static void userport_wic64_store_pa2(uint8_t value)
{
    /* DBG(("userport_wic64_store_pa2 val:%02x", value)); */
    wic64_ddr = value;
}


static void send_reply(char * reply)
{
    int len;
    len = strlen(reply);
    replybuffer[0] = len & 0xff;
    replybuffer[1] = (len >> 8) & 0xff;
    strcpy((char*)replybuffer + 2, reply);
    reply_length = len + 2;
    replyptr = 0;
}

/* http get */
static void do_command_01(void)
{
    DBG(("command 01: '%s'", commandbuffer));
}

/* set wlan ssid + password */
static void do_command_02(void)
{
    DBG(("command 02: '%s'", commandbuffer));
}

/* get wic64 ip address */
static void do_command_06(void)
{
    send_reply("192.123.100.123");
}

/* get list of all detected wlan ssids */
static void do_command_0c(void)
{
    /* index, sep, ssid, sep, rssi, sep */
    send_reply("0\001vice\0011234");
}

static void do_command_0d(void)
{
    DBG(("command 0d: '%s'", commandbuffer));
}

/* get external ip address */
static void do_command_13(void)
{
    send_reply("208.123.100.123");
}

static void do_command(void)
{
    switch (input_command) {
        case 0x01: /* http get */
            DBG(("command 01: http get"));
            do_command_01();
            break;
        case 0x02: /* set wlan ssid + password */
            DBG(("command 02: set wlan ssid + password"));
            do_command_02();
            break;
        case 0x06: /* get wic64 ip address */
            DBG(("command 06: get wic64 ip address"));
            do_command_06();
            break;
        case 0x0c: /* get list of all detected wlan ssids */
            DBG(("command 0c: get list of all detected wlan ssids"));
            do_command_0c();
            break;
        case 0x0d: /* set wlan via scan id */
            DBG(("command 02: set wlan via scan id"));
            do_command_0d();
            break;
        case 0x13: /* get external ip address */
            DBG(("command 13: get external ip address"));
            do_command_13();
            break;
        case 0x03: /* standard firmware update */
        case 0x04: /* developer firmware update */
        case 0x05: /* developer special update */
        case 0x07: /* get firmware stats */
        case 0x08: /* set default server */
        case 0x09: /* prints output to serial console */
        case 0x0a: /* get udp package */
        case 0x0b: /* send udp package */
        case 0x0e: /* set udp port */
        case 0x0f: /* send http with decoded url for PHP */
        case 0x10: /* get connected wlan name */
        case 0x11: /* get wlan rssi signal level */
        case 0x12: /* get default server */
        case 0x14: /* get wic64 MAC address */
        case 0x63: /* factory reset */
        default:
            log_error(LOG_DEFAULT, "WiC64: unsupported command 0x%02x (len: %d)", input_command, input_length);
            input_state = 0;
        break;
    }
}

static void userport_wic64_store_pbx(uint8_t value, int pulse)
{
#if 0
    DBG(("userport_wic64_store_pbx val:%02x pulse:%d", value, pulse));
#endif
    if (reply_length) {
        if (wic64_ddr == 0) {
            reply_next_byte();
        }
    } else {

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
                        commandbuffer[commandptr] = 0;
                    }
                break;
            }
#if 0
            DBG(("input_state: %d input_length: %d input_command: %02x commandptr: %02x",
                input_state, input_length, input_command, commandptr));
#endif
            if ((input_state == 4) && ((commandptr + 4) >= input_length)) {
                do_command();
                commandptr = input_command = input_state = input_length = 0;
            }

        }
    }

    handshake_flag2();
}

static uint8_t userport_wic64_read_pbx(uint8_t orig)
{
    uint8_t retval = reply_port_value;
#if 0
    DBG(("userport_wic64_read_pbx orig:%02x retval: %02x", orig, retval));
#endif
    return retval;
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

