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
 
#include <Box.h>
#include <RadioButton.h>
#include <TextView.h>
#include <Window.h>

extern "C" { 
#include "joystick.h"
#include "resources.h"
#include "ui.h"
#include "ui_joystick.h"
#include "vsync.h"
}


class JoyView : public BView {
	public:
		JoyView(BRect r, int joyport);
		BRadioButton *radio_joystick_device[NUM_OF_JOYDEVICES];
};


JoyView::JoyView(BRect r, int joyport)
	: BView(r, "joy_view", B_FOLLOW_NONE, B_WILL_DRAW)
{
	int device;
	char *joydevice_name[] = {
		"None",
		"Numpad",
		"Keyset 1",
		"Keyset 2",
		"Hardware 1",
		"Hardware 2"
	};
	BBox *box;
	char portname[] = "Port X";
	portname[5] = 'A'+joyport;
	
	SetViewColor(220,220,220,0);
	for (device = 0; device < NUM_OF_JOYDEVICES; device++) {
		radio_joystick_device[device] = 
			new BRadioButton(BRect
				(10,20+device*20,110,30+device*20),
			joydevice_name[device],
			joydevice_name[device],
			/* code the port and device to the message */
			new BMessage(JOYMESSAGE_DEVPORT + 
				joyport*NUM_OF_JOYDEVICES + device));
		AddChild(radio_joystick_device[device]);
	}
	if (joyport==0)
    	resources_get_value("JoyDevice1",(resource_value_t *)&device);
	else
    	resources_get_value("JoyDevice2",(resource_value_t *)&device);
	radio_joystick_device[device]->SetValue(1);
	r.InsetBy(5,5);
	r.OffsetTo(5,5);
	box = new BBox(r);
	box->SetLabel(portname);
	AddChild(box);
}


class JoystickWindow : public BWindow {
	public:
		JoystickWindow();
		~JoystickWindow();
		virtual void MessageReceived(BMessage *msg);
};	


static JoystickWindow *joywindow = NULL;


JoystickWindow::JoystickWindow() 
	: BWindow(BRect(50,50,300,200),"Joystick settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BRect r;
	
	r = Bounds();
	r.right -= r.Width()/2;
	AddChild(new JoyView(r,0));
	
	r = Bounds();
	r.left += r.Width()/2;
	AddChild(new JoyView(r,1));
	
	Show();
}

JoystickWindow::~JoystickWindow() 
{
	joywindow = NULL;	
}

void JoystickWindow::MessageReceived(BMessage *msg) {
	int port,device;
	char resource_name[] = "JoyDeviceX";
	
	if (msg->what >= JOYMESSAGE_DEVPORT && 
		msg->what < JOYMESSAGE_DEVPORT + 2*NUM_OF_JOYDEVICES) {
		/* it's a port/device selection message */
		port = (msg->what - JOYMESSAGE_DEVPORT) / NUM_OF_JOYDEVICES;
		device = (msg->what - JOYMESSAGE_DEVPORT) % NUM_OF_JOYDEVICES;
		resource_name[9] = '1' + port;
		resources_set_value(resource_name, (resource_value_t) device);
	}
}

void ui_joystick() {
	
	if (joywindow != NULL)
		return;

	joywindow = new JoystickWindow;

	suspend_speed_eval();
	while (joywindow); /* wait until window closed */
}

