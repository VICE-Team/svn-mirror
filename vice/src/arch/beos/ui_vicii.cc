/*
 * ui_vicii.cc - VIC-II settings
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
#include <CheckBox.h>
#include <RadioButton.h>
#include <string.h>
#include <Window.h>

extern "C" { 
#include "resources.h"
#include "ui.h"
#include "ui_vicii.h"
#include "vsync.h"
}

static char *palette_name[] = {
	"default",
	"c64s",
	"ccs64",
	"frodo",
	"godot",
	"pc64",
	NULL
};


class ViciiWindow : public BWindow {
	public:
		ViciiWindow();
		~ViciiWindow();
		virtual void MessageReceived(BMessage *msg);
};	


static ViciiWindow *viciiwindow = NULL;


ViciiWindow::ViciiWindow() 
	: BWindow(BRect(50,50,210,155),"VIC-II settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BMessage *msg;
	BCheckBox *checkbox;
	BRect r;
	BBox *box;
	BView *background;
	int res_val;
	
	r = Bounds();
	background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);
	
	/* sprite collisions */
	r = Bounds();
	r.InsetBy(10,10);
	r.bottom -= 20;
	box = new BBox(r);
	box->SetLabel("Sprite Collision");
	background->AddChild(box);
	
	checkbox = new BCheckBox(
		BRect(10, 20, 120, 35),
		NULL,
		"Sprite-Sprite",
		new BMessage(MESSAGE_VICII_SSCOLL));
	resources_get_value("VICIICheckSsColl", (void *)&res_val);
	checkbox->SetValue(res_val);
	box->AddChild(checkbox);	
	
	checkbox = new BCheckBox(
		BRect(10, 40, 120, 55),
		NULL,
		"Sprite-Background",
		new BMessage(MESSAGE_VICII_SBCOLL));
	resources_get_value("VICIICheckSbColl", (void *)&res_val);
	checkbox->SetValue(res_val);
	box->AddChild(checkbox);	
	
	/* new colors */
	checkbox = new BCheckBox(
		BRect(20, 80, 120, 95),
		NULL,
		"New Colors",
		new BMessage(MESSAGE_VICII_NEWLUMINANCE));
	resources_get_value("VICIINewLuminances", (void *)&res_val);
	checkbox->SetValue(res_val);
	background->AddChild(checkbox);	
	
	Show();
}

ViciiWindow::~ViciiWindow() 
{
	viciiwindow = NULL;	
}

void ViciiWindow::MessageReceived(BMessage *msg) {
	resource_value_t dummy;
	
	switch (msg->what) {
		case MESSAGE_VICII_SSCOLL:
			resources_toggle("VICIICheckSsColl", &dummy);
			break;
		case MESSAGE_VICII_SBCOLL:
			resources_toggle("VICIICheckSbColl", &dummy);
			break;
		case MESSAGE_VICII_NEWLUMINANCE:
			resources_toggle("VICIINewLuminances", &dummy);
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void ui_vicii() {
	
	if (viciiwindow != NULL)
		return;

	viciiwindow = new ViciiWindow;

	vsync_suspend_speed_eval();
	while (viciiwindow); /* wait until window closed */
}

