/*
 * mouse.cc - Mouse handling for BeOS
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
#include "cmdline.h"
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
}

int                 _mouse_enabled;
int                 _mouse_x, _mouse_y;
/* ------------------------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v, void *param)
{
    _mouse_enabled = (int) v;
    return 0;
}

static const resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t) 0,
      (void *)&_mouse_enabled, set_mouse_enabled, NULL },
    { NULL }
};

int mouse_resources_init(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 1, NULL, NULL,
      "Mouse", NULL, NULL, "Enable emulation of the 1351 proportional mouse" },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", NULL, NULL, "Disable emulation of the 1351 proportional mouse" },
    { NULL }
};

int mouse_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
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

void mouse_init(void)
{
}

