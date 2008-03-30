/*
 * mouse.h - Mouse handling for MS-DOS.
 *
 * Written by
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

/* This is a first rough implementation of mouse emulation for MS-DOS.
   A smarter and less buggy emulation is of course possible. */

#include "vice.h"

#include <stdio.h>

#include "mouse.h"

#include "kbd.h"  /* for joystick_value[]...
                     (this is ugly and should be fixed) */
#include "log.h"
#include "resources.h"
#include "cmdline.h"

int _mouse_enabled;
int _mouse_available;
int _mouse_x, _mouse_y;
int _mouse_coords_dirty;

static void my_mouse_callback(int flags);

/* ------------------------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v)
{
    _mouse_enabled = (int) v;
    if (_mouse_enabled && _mouse_available)
        mouse_callback = my_mouse_callback;
    else
        mouse_callback = NULL;
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

static void my_mouse_callback(int flags)
{
    if (flags & MOUSE_FLAG_MOVE)
        _mouse_coords_dirty = 1;
    if (flags & MOUSE_FLAG_LEFT_UP)
        joystick_value[1] &= ~16;
    if (flags & MOUSE_FLAG_LEFT_DOWN)
        joystick_value[1] |= 16;
    if (flags & MOUSE_FLAG_RIGHT_UP)
        joystick_value[1] &= ~1;
    if (flags & MOUSE_FLAG_RIGHT_DOWN)
        joystick_value[1] |= 1;
}

int mouse_init(void)
{
    log_message (LOG_DEFAULT, "Attempting to initialize the mouse driver... ");

    if (install_mouse() == -1) {
        log_error(LOG_DEFAULT, "Mouse initialization failed!");
	_mouse_available = 0;
        return -1;
    }
    log_message(LOG_DEFAULT, "Mouse is available.");
    _mouse_available = 1;

    set_mouse_range(0, 0, ~0, ~0); /* This should not really be important... */
    set_mouse_speed(1, 1);

    mouse_callback = my_mouse_callback;

    return 0;
}
