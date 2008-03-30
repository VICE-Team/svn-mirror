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

#ifndef _MOUSE_H
#define _MOUSE_H

#include <allegro.h>

#include "types.h"

extern int mouse_init_resources(void);
extern int mouse_init_cmdline_options(void);
extern int mouse_init(void);

extern int _mouse_available;
extern int _mouse_enabled;
extern int _mouse_x, _mouse_y;
extern int _mouse_coords_dirty;

/* ------------------------------------------------------------------------- */

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

inline static BYTE mouse_get_x(void)
{
    if (!_mouse_available || !_mouse_enabled)
        return 0xff;
    _update_mouse();
    return (BYTE) _mouse_x;
}

inline static BYTE mouse_get_y(void)
{
    if (!_mouse_available || !_mouse_enabled)
        return 0xff;
    _update_mouse();
    return (BYTE) _mouse_y;
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

#endif
