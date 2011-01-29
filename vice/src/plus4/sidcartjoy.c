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
#include "translate.h"
#include "types.h"
#include "uiapi.h"


int sidcartjoy_enabled = 0;

static int set_sidcartjoy_enabled(int val, void *param)
{
    sidcartjoy_enabled = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "SIDCartJoy", 0, RES_EVENT_SAME, NULL,
      &sidcartjoy_enabled, set_sidcartjoy_enabled, NULL },
    { NULL }
};

int sidcartjoy_resources_init(void)
{
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
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
    { NULL }
};

int sidcartjoy_cmdline_options_init(void)
{
  return cmdline_register_options(cmdline_options);
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
