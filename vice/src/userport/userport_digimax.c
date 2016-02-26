/*
 * userport_digimax.c - Digimax DAC userport device emulation.
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

#include "cmdline.h"
#include "lib.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sound.h"
#include "uiapi.h"
#include "userport.h"
#include "userport_digimax.h"
#include "util.h"
#include "translate.h"

#include "digimaxcore.c"

/*
    Digimax userport device

    This device is an 8bit 4-channel digital sound output
    interface.

C64/C128 | CBM2 | TLC7226 DAC | NOTES
-------------------------------------
    8    |  5   |      15     | /PC2 -> /Write
    9    |  4   |      16     | /PA3 -> A1
    C    | 14   |      14     | PB0 -> DB0
    D    | 13   |      13     | PB1 -> DB1
    E    | 12   |      12     | PB2 -> DB2
    F    | 11   |      11     | PB3 -> DB3
    H    | 10   |      10     | PB4 -> DB4
    J    |  9   |       9     | PB5 -> DB5
    K    |  8   |       8     | PB6 -> DB6
    L    |  7   |       7     | PB7 -> DB7
    M    |  2   |      17     | PA2 -> A0
*/

/* Some prototypes are needed */
static void userport_digimax_store_pbx(BYTE value);
static void userport_digimax_store_pa2(BYTE value);
static void userport_digimax_store_pa3(BYTE value);

static userport_device_t digimax_device = {
    "Userport DigiMAX",
    IDGS_USERPORT_DIGIMAX,
    NULL, /* NO pbx read */
    userport_digimax_store_pbx,
    NULL, /* NO pa2 read */
    userport_digimax_store_pa2,
    NULL, /* NO pa3 read */
    userport_digimax_store_pa3,
    0, /* NO pc pin needed */
    NULL, /* NO sp1 write */
    NULL, /* NO sp2 read */
    "UserportDigiMax",
    0xff,
    0xf, /* validity mask doesn't change */
    0,
    0
};

static userport_device_list_t *userport_digimax_list_item = NULL;

/* ------------------------------------------------------------------------- */

static BYTE userport_digimax_address = 3;

void userport_digimax_sound_chip_init(void)
{
    digimax_sound_chip_offset = sound_chip_register(&digimax_sound_chip);
}

static void userport_digimax_store_pa2(BYTE value)
{
    userport_digimax_address &= 2;
    userport_digimax_address |= (value & 1);
}

static void userport_digimax_store_pa3(BYTE value)
{
    userport_digimax_address &= 1;
    userport_digimax_address |= ((value & 1) << 1);
}

static void userport_digimax_store_pbx(BYTE value)
{
    BYTE addr = 0;

    switch (userport_digimax_address) {
        case 0x0:
            addr = 2;
            break;
        case 0x4:
            addr = 3;
            break;
        case 0x8:
            addr = 0;
            break;
        case 0xc:
            addr = 1;
            break;
    }

    digimax_sound_data[addr] = value;
    sound_store((WORD)(digimax_sound_chip_offset | addr), value, 0);
}

/* ---------------------------------------------------------------------*/

static int set_digimax_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (!digimax_sound_chip.chip_enabled && val) {
        userport_digimax_list_item = userport_device_register(&digimax_device);
        digimax_sound_chip.chip_enabled = 1;
    } else if (digimax_sound_chip.chip_enabled && !val) {
        if (userport_digimax_list_item != NULL) {
            userport_device_unregister(userport_digimax_list_item);
            userport_digimax_list_item = NULL;
        }
        digimax_sound_chip.chip_enabled = 0;
    }
    return 0;
}

/* ---------------------------------------------------------------------*/

static const resource_int_t resources_int[] = {
    { "UserportDIGIMAX", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &digimax_sound_chip.chip_enabled, set_digimax_enabled, NULL },
    { NULL }
};

int userport_digimax_resources_init(void)
{
    return resources_register_int(resources_int);
}

/* ---------------------------------------------------------------------*/

static const cmdline_option_t cmdline_options[] =
{
    { "-userportdigimax", SET_RESOURCE, 0,
      NULL, NULL, "UserportDIGIMAX", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_USERPORT_DIGIMAX,
      NULL, NULL },
    { "+userportdigimax", SET_RESOURCE, 0,
      NULL, NULL, "UserportDIGIMAX", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_USERPORT_DIGIMAX,
      NULL, NULL },
    { NULL }
};

int userport_digimax_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
