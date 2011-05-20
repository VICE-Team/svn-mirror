/*
 * sidcartjoy.c - SIDCART joystick port emulation.
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
#include "keyboard.h"
#include "resources.h"
#include "sid-cmdline-options.h"
#include "sid-resources.h"
#include "sound.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"


int sidcartjoy_enabled = 0;

int sidcart_enabled;
int sidcart_address;
int sidcart_clock;

static int set_sidcart_enabled(int val, void *param)
{
    if (val != sidcart_enabled) {
        sidcart_enabled = val;
        sound_state_changed = 1;
    }
    return 0;
}

static int set_sid_address(int val, void *param)
{
    if (val != sidcart_address) {
        sidcart_address = val;
    }
    return 0;
}

static int set_sid_clock(int val, void *param)
{
    if (val != sidcart_clock) {
        sidcart_clock = val;
        sid_state_changed = 1;
    }
    return 0;
}

static int set_sidcartjoy_enabled(int val, void *param)
{
    sidcartjoy_enabled = val;
    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_int_t sidcart_resources_int[] = {
    { "SIDCartJoy", 0, RES_EVENT_SAME, NULL,
      &sidcartjoy_enabled, set_sidcartjoy_enabled, NULL },
    { "SidCart", 0, RES_EVENT_SAME, NULL,
      &sidcart_enabled, set_sidcart_enabled, NULL },
    { "SidAddress", 0, RES_EVENT_SAME, NULL,
      &sidcart_address, set_sid_address, NULL },
    { "SidClock", 1, RES_EVENT_SAME, NULL,
      &sidcart_clock, set_sid_clock, NULL },
    { NULL }
};

int sidcart_resources_init(void)
{
    if (sid_common_resources_init() < 0) {
        return -1;
    }
    return resources_register_int(sidcart_resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t sidcart_cmdline_options[] = {
    { "-sidenginemodel", CALL_FUNCTION, 1,
      sid_common_set_engine_model, NULL, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_ENGINE_MODEL, IDCLS_SPECIFY_SIDCART_ENGINE_MODEL,
      NULL, NULL },
    { "-sidcart", SET_RESOURCE, 1,
      NULL, NULL, "SidCart", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SIDCART,
      NULL, NULL },
    { "+sidcart", SET_RESOURCE, 0,
      NULL, NULL, "SidCart", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SIDCART,
      NULL, NULL },
    { "-sidcartjoy", SET_RESOURCE, 0,
      NULL, NULL, "SIDCartJoy", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SIDCARTJOY,
      NULL, NULL },
    { "+sidcartjoy", SET_RESOURCE, 0,
      NULL, NULL, "SIDCartJoy", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SIDCARTJOY,
      NULL, NULL },
    { "-sidfilters", SET_RESOURCE, 0,
      NULL, NULL, "SidFilters", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SID_FILTERS,
      NULL, NULL },
    { "+sidfilters", SET_RESOURCE, 0,
      NULL, NULL, "SidFilters", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SID_FILTERS,
      NULL, NULL },
    { NULL }
};

int sidcart_cmdline_options_init(void)
{
    return cmdline_register_options(sidcart_cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* dummy function for now, since only joystick support
   has been added, might be expanded when other devices
   get supported */

void sidcartjoy_store(WORD addr, BYTE value)
{
}

BYTE sidcartjoy_read(WORD addr)
{
  return ~joystick_value[3];
}
