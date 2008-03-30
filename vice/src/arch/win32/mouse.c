/*
 * mouse.c - Mouse handling for Win32
 *
 * Written by
 *  Tibor Biczo (crown@mail.matav.hu)
 *  Ettore Perazzoli (ettore@comm2000.it)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "vice.h"

#include <windows.h>

#include <stdio.h>

#include "mouse.h"

#include "kbd.h"  /* for joystick_value[]...
                     (this is ugly and should be fixed) */
#include "log.h"
#include "resources.h"
#include "cmdline.h"

int _mouse_enabled;
int _mouse_x, _mouse_y;

/* ------------------------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v)
{
    _mouse_enabled = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_mouse_enabled, set_mouse_enabled },
    { NULL }
};

int mouse_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 1, NULL, NULL,
      "Mouse", NULL, NULL, "Enable emulation of the 1351 proportional mouse" },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", NULL, NULL, "Disable emulation of the 1351 proportional mouse" },
    { NULL }
};

int mouse_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */


void mouse_update_mouse(int x, int y, int button)
{
    _mouse_x=x;
    _mouse_y=y;
    if (button & MK_LBUTTON) {
        joystick_value[1] |= 16;
    } else {
        joystick_value[1] &=~16;
    }
    if (button & MK_RBUTTON) {
        joystick_value[1] |= 1;
    } else {
        joystick_value[1] &= ~1;
    }
}

void mouse_init(void)
{
}

