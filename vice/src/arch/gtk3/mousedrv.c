/*
 * mousedrv.c - Native GTK3 UI mouse driver stuff.
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

#include "not_implemented.h"

#include "vsyncapi.h"
#include "mouse.h"
#include "mousedrv.h"


#ifndef MACOSX_COCOA


static mouse_func_t mouse_funcs;

static float mouse_x = 0.0;
static float mouse_y = 0.0;
static unsigned long mouse_timestamp = 0;


int mousedrv_cmdline_options_init(void)
{
    return 0;
}

unsigned long mousedrv_get_timestamp(void)
{
    return mouse_timestamp;
}

int mousedrv_get_x(void)
{
    return (int)mouse_x;
}

int mousedrv_get_y(void)
{
    return (int)mouse_y;
}


void mouse_move(float dx, float dy)
{
    mouse_x += dx;
    mouse_y -= dy;  /* why ? */

    /* can't this be done with int modulo ? */
    while (mouse_x < 0.0) {
        mouse_x += 65536.0;
    }
    while (mouse_x >= 65536.0) {
        mouse_x -= 65536.0;
    }
    while (mouse_y < 0.0) {
        mouse_y += 65536.0;
    }
    while (mouse_y >= 65536.0) {
        mouse_y -= 65536.0;
    }

    mouse_timestamp = vsyncarch_gettime();
}


void mousedrv_init(void)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void mousedrv_mouse_changed(void)
{
    /* ui_check_mouse_cursor(); */
    NOT_IMPLEMENTED_WARN_ONLY();
}

int mousedrv_resources_init(mouse_func_t *funcs)
{
    mouse_funcs.mbl = funcs->mbl;
    mouse_funcs.mbr = funcs->mbr;
    mouse_funcs.mbm = funcs->mbm;
    mouse_funcs.mbu = funcs->mbu;
    mouse_funcs.mbd = funcs->mbd;
    return 0;
}



#endif  /* ifndef MACOSX_COCOA */


