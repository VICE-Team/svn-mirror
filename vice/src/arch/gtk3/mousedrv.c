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

#include "mouse.h"
#include "mousedrv.h"


#ifndef MACOSX_COCOA


static mouse_func_t mouse_funcs;


int mousedrv_cmdline_options_init(void)
{
    return 0;
}

unsigned long mousedrv_get_timestamp(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

int mousedrv_get_x(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

int mousedrv_get_y(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void mousedrv_init(void)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void mousedrv_mouse_changed(void)
{
    NOT_IMPLEMENTED();
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


