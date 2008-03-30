/*
 * mousedrv.h - Mouse handling for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <allegro.h>
#include <stdio.h>

#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "mousedrv.h"
#include "log.h"


int _mouse_available;
int _mouse_x, _mouse_y;
int _mouse_coords_dirty;

static void my_mouse_callback(int flags);

/* ------------------------------------------------------------------------- */

void mousedrv_mouse_changed(void)
{
    if (_mouse_enabled && _mouse_available)
        mouse_callback = my_mouse_callback;
    else
        mouse_callback = NULL;
}

int mousedrv_resources_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

int mousedrv_cmdline_options_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

static void my_mouse_callback(int flags)
{
    if (flags & MOUSE_FLAG_MOVE)
        _mouse_coords_dirty = 1;
    if (flags & MOUSE_FLAG_LEFT_UP)
        joystick_set_value_and(1, ~16);
    if (flags & MOUSE_FLAG_LEFT_DOWN)
        joystick_set_value_or(1, 16);
    if (flags & MOUSE_FLAG_RIGHT_UP)
        joystick_set_value_and(1, ~1);
    if (flags & MOUSE_FLAG_RIGHT_DOWN)
        joystick_set_value_or(1, 1);
}

void mousedrv_init(void)
{
    log_message(LOG_DEFAULT, "Attempting to initialize the mouse driver... ");

    if (install_mouse() == -1) {
        log_error(LOG_DEFAULT, "Mouse initialization failed!");
	_mouse_available = 0;
        return;
    }
    log_message(LOG_DEFAULT, "Mouse is available.");
    _mouse_available = 1;

    set_mouse_range(0, 0, ~0, ~0); /* This should not really be important... */
    set_mouse_speed(1, 1);

    mouse_callback = my_mouse_callback;
}

inline static void _update_mouse(void)
{
    if (_mouse_coords_dirty) {
        int x, y;

        get_mouse_mickeys(&x, &y);
        _mouse_x = (_mouse_x + (x / 3)) & 0xff;
        _mouse_y = (_mouse_y + (y / 3)) & 0xff;
        _mouse_coords_dirty = 0;
    }
}

BYTE mousedrv_get_x(void)
{
    if (!_mouse_available || !_mouse_enabled)
        return 0xff;
    _update_mouse();
    return (BYTE)(_mouse_x >> 1) & 0x7e;
}

BYTE mousedrv_get_y(void)
{
    if (!_mouse_available || !_mouse_enabled)
        return 0xff;
    _update_mouse();
    return (BYTE)(~_mouse_y >> 1) & 0x7e;
}

#if 0
inline static int mouse_get_left_button(void)
{
    return mouse_b & 1;
}

inline static int mouse_get_right_button(void)
{
    return mouse_b & 2;
}
#endif

