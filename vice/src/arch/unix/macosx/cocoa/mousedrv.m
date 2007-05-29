/*
 * mousedrv.m - MacVICE mouse driver
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
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

#include "mousedrv.h"
#include "vicemachine.h"

// mouse.c
extern int _mouse_enabled;
static int ptr_x=0,ptr_y=0;
static int hw_x=0,hw_y=0;

int mousedrv_resources_init(void)
{
    return 0;
}

int mousedrv_cmdline_options_init(void)
{
    return 0;
}

void mousedrv_init(void)
{
}

void mousedrv_mouse_changed(void)
{
    [[theVICEMachine machineNotifier] postToggleMouseNotification:_mouse_enabled];
}

#define MAX_DELTA  16

BYTE mousedrv_get_x(void)
{
    int dx = ptr_x - hw_x;
    if(dx < -MAX_DELTA)
        dx = -MAX_DELTA;
    else if(dx > MAX_DELTA)
        dx = MAX_DELTA;
    hw_x += dx;
    return (BYTE)((hw_x&0x3f) << 1);
}

BYTE mousedrv_get_y(void)
{
    int dy = ptr_y - hw_y;
    if(dy < -MAX_DELTA)
        dy = -MAX_DELTA;
    else if(dy > MAX_DELTA)
        dy = MAX_DELTA;
    hw_y += dy;
    return (BYTE)((hw_y&0x3f) << 1);
}

void mouse_move(int x, int y)
{
    ptr_x = x;
    ptr_y = y;
}
