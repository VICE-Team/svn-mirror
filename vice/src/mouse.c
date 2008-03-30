/*
 * mouse.c - Common mouse handling
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "cmdline.h"
#include "joystick.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"


int _mouse_enabled = 0;

static int mouse_port;


static int set_mouse_enabled(resource_value_t v, void *param)
{
    _mouse_enabled = (int)v;
    mousedrv_mouse_changed();
    return 0;
}

static int set_mouse_port(resource_value_t v, void *param)
{
    mouse_port = (int)v;

    if ((int)v < 1 || (int)v > 2)
        return -1;

    mouse_port = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t)0,
      (void *)&_mouse_enabled, set_mouse_enabled, NULL },
    { "Mouseport", RES_INTEGER, (resource_value_t)1,
      (void *)&mouse_port, set_mouse_port, NULL },
    { NULL }
};

int mouse_resources_init(void)
{
    if (resources_register(resources) < 0)
        return -1;

    return mousedrv_resources_init();
}

static const cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 1, NULL, NULL,
      "Mouse", NULL, NULL, N_("Enable emulation of the 1351 proportional mouse") },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", NULL, NULL, N_("Disable emulation of the 1351 proportional mouse") },
    { "-mouseport", SET_RESOURCE, 1, NULL, NULL,
      "Mouseport", NULL, "<value>", "Select the joystick port the mouse is attached to" },
    { NULL }
};

int mouse_cmdline_options_init(void)
{
    if (cmdline_register_options(cmdline_options) < 0)
        return -1;

    return mousedrv_cmdline_options_init();
}

void mouse_init(void)
{
    mousedrv_init();
}

void mouse_button_left(int pressed)
{
    if (pressed) {
        joystick_set_value_or(mouse_port, 16);
    } else {
        joystick_set_value_and(mouse_port, ~16);
    }
}

void mouse_button_right(int pressed)
{
    if (pressed) {
        joystick_set_value_or(mouse_port, 1);
    } else {
        joystick_set_value_and(mouse_port, ~1);
    }
}

BYTE mouse_get_x(void)
{
    return mousedrv_get_x();
}

BYTE mouse_get_y(void)
{
    return mousedrv_get_y();
}

