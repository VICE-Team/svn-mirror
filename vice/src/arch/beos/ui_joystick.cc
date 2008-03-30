/*
 * ui_joystick.cc - Joystick settings
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */
 
#include <Window.h>

extern "C" { 
#include "ui_joystick.h"
#include "vsync.h"
}

class joystickwindow : public BWindow {
	public:
		joystickwindow();
		~joystickwindow();
};	

static joystickwindow *joywindow;

joystickwindow::joystickwindow() 
	: BWindow(BRect(50,50,400,250),"Joystick settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	Show();
}

joystickwindow::~joystickwindow() 
{
	joywindow = NULL;	
}

void ui_joystick() {
	
	if (joywindow != NULL)
		return;

	joywindow = new joystickwindow;

	suspend_speed_eval();
	while (joywindow);
}
