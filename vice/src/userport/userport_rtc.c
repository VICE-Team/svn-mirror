/*
 * userport_rtc.c - Generic userport RTC emulation.
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

/* Userport RTC (C64/C128/CBM2/PET/VIC20)

C64/C128 | CBM2 | PET | VIC20 | NAME
------------------------------------
    C    |  14  |  C  |   C   | PB0 <-> D0
    D    |  13  |  D  |   D   | PB1 <-> D1
    E    |  12  |  E  |   E   | PB2 <-> D2
    F    |  11  |  F  |   F   | PB3 <-> D3
    H    |  10  |  H  |   H   | PB4 -> ADDRESS /DATA
    J    |   9  |  J  |   J   | PB5 -> READ
    K    |   8  |  K  |   K   | PB6 -> WRITE
*/

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "lib.h"
#include "maincpu.h"
#include "resources.h"
#include "rtc-58321a.h"
#include "translate.h"
#include "uiapi.h"
#include "userport.h"
#include "userport_rtc.h"

int userport_rtc_enabled = 0;

/* rtc context */
static rtc_58321a_t *rtc58321a_context = NULL;

/* rtc save */
static int rtc58321a_rtc_save;

static int read_line_active = 0;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static void userport_rtc_read_pbx(void);
static void userport_rtc_store_pbx(BYTE value);

static userport_device_t rtc_device = {
    "Userport RTC (RTC58321A)",
    IDGS_USERPORT_RTC58321A,
    userport_rtc_read_pbx,
    userport_rtc_store_pbx,
    NULL, /* NO pa2 read */
    NULL, /* NO pa2 write */
    NULL, /* NO pa3 read */
    NULL, /* NO pa3 write */
    NULL, /* NO flag read */
    NULL, /* NO flag write */
    NULL, /* NO pc read */
    NULL, /* NO sp1 write */
    NULL, /* NO sp2 read */
    "UserportRTC",
    0xff,
    0xf, /* validity mask doesn't change */
    0,
    0
};

static userport_device_list_t *userport_rtc_list_item = NULL;

/* ------------------------------------------------------------------------- */

static int set_userport_rtc_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (userport_rtc_enabled == val) {
        return 0;
    }

    if (val) {
        rtc58321a_context = rtc58321a_init("USER");
        userport_rtc_list_item = userport_device_register(&rtc_device);
        if (userport_rtc_list_item == NULL) {
            return -1;
        }
    } else {
        if (rtc58321a_context) {
            rtc58321a_destroy(rtc58321a_context, rtc58321a_rtc_save);
            rtc58321a_context = NULL;
        }
        userport_device_unregister(userport_rtc_list_item);
        userport_rtc_list_item = NULL;
    }

    userport_rtc_enabled = val;
    return 0;
}

static int set_userport_rtc_save(int val, void *param)
{
    rtc58321a_rtc_save = val ? 1 : 0;

    return 0;
}


static const resource_int_t resources_int[] = {
    { "UserportRTC", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &userport_rtc_enabled, set_userport_rtc_enabled, NULL },
    { "UserportRTCSave", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &rtc58321a_rtc_save, set_userport_rtc_save, NULL },
    { NULL }
};

int userport_rtc_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-userportrtc", SET_RESOURCE, 0,
      NULL, NULL, "UserportRTC", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_USERPORT_RTC,
      NULL, NULL },
    { "+userportrtc", SET_RESOURCE, 0,
      NULL, NULL, "UserportRTC", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_USERPORT_RTC,
      NULL, NULL },
    { "-userportrtcsave", SET_RESOURCE, 0,
      NULL, NULL, "UserportRTCSave", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_USERPORT_RTC,
      NULL, NULL },
    { "+userportrtcsave", SET_RESOURCE, 0,
      NULL, NULL, "UserportRTCSave", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_USERPORT_RTC,
      NULL, NULL },
    { NULL }
};

int userport_rtc_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void userport_rtc_resources_shutdown(void)
{
    if (rtc58321a_context) {
        rtc58321a_destroy(rtc58321a_context, rtc58321a_rtc_save);
        rtc58321a_context = NULL;
    }
}

/* ---------------------------------------------------------------------*/

static void userport_rtc_store_pbx(BYTE value)
{
    if (value & 0x10) {
        rtc58321a_write_address(rtc58321a_context, (BYTE)(value & 0xf));
    }
    if (value & 0x20) {
        read_line_active = 1;
    } else {
        read_line_active = 0;
    }
    if (value & 0x40) {
        rtc58321a_write_data(rtc58321a_context, (BYTE)(value & 0xf));
    }
}

static void userport_rtc_read_pbx(void)
{
    BYTE retval = 0xf;

    if (read_line_active) {
        retval = rtc58321a_read(rtc58321a_context);
    }
    rtc_device.retval = retval;
}
