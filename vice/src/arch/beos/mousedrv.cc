/*
 * mousedrv.cc - Mouse handling for BeOS
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <Application.h>
#include <stdio.h>

#include "vicewindow.h"

extern "C" {
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "mousedrv.h"
#include "types.h"
#include "ui.h"
}

int                 _mouse_x, _mouse_y;
/* ------------------------------------------------------------------------- */

void mousedrv_mouse_changed(void)
{
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

void mouse_set_format(void)
{
}

extern ViceWindow 			*windowlist[];
extern int 					window_count;

void mouse_update_mouse(void)
{
	/* this implementation is realy ugly, but the MouseMoved event
	   doesn't work as expected
	*/
	static BPoint last_point;

	BPoint point;
	BRect bounds;
	uint32 buttons;

	if (!_mouse_enabled)
		return;
		
	windowlist[0]->Lock();
	windowlist[0]->view->GetMouse(&point, &buttons);
	bounds = windowlist[0]->view->Bounds();
	windowlist[0]->Unlock();

	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		last_point = point;
		return;
	}

	_mouse_x += (int) ((point.x - last_point.x) 
		/ bounds.Width() * 1024) % 1024;
	_mouse_y += (int) ((point.y - last_point.y) 
		/ bounds.Height() * 1024) % 1024;
	
	last_point = point;
	
}

void mousedrv_init(void)
{
}

BYTE mousedrv_get_x(void)
{
    if (!_mouse_enabled)
        return 0xff;
        mouse_update_mouse();
    return (BYTE) _mouse_x;
}

BYTE mousedrv_get_y(void)
{
    if (!_mouse_enabled)
        return 0xff;
    mouse_update_mouse();
    return (BYTE) ~_mouse_y;
}

