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

#include <Button.h>
#include <Box.h>
#include <CheckBox.h>
#include <Joystick.h>
#include <RadioButton.h>
#include <TextControl.h>
#include <Window.h>

extern "C" { 
#include "joy.h"
#include "resources.h"
#include "ui.h"
#include "ui_joystick.h"
#include "vsync.h"
}


/* ugly externs*/
extern hardware_joystick_t hardware_joystick[MAX_HARDWARE_JOYSTICK];
extern int hardware_joystick_count;

/* the classes */
class JoyView : public BView {
	public:
		JoyView(BRect r, int joyport);
		BRadioButton *radio_joystick_device
			[NUM_OF_SOFTDEVICES + MAX_HARDWARE_JOYSTICK];
};

class JoystickWindow : public BWindow {
	public:
		JoystickWindow();
		~JoystickWindow();
		virtual void MessageReceived(BMessage *msg);
};	

class KeysetWindow : public BWindow {
	public:
		KeysetWindow(int nr);
		~KeysetWindow();
		virtual void MessageReceived(BMessage *msg);
		BButton *keyset_button[9];
		BTextView *keyset_instruction;
		int last_key;
		int keyset_setnr;
};	


/* the static variables */
static JoystickWindow *joywindow = NULL;
static KeysetWindow *keysetwindow = NULL;


/* definition for JoyView */
JoyView::JoyView(BRect r, int joyport)
	: BView(r, "joy_view", B_FOLLOW_NONE, B_WILL_DRAW)
{
	int device;
	char *joydevice_name[] = {
		"None",
		"Numpad",
		"Keyset 1",
		"Keyset 2"
	};
	BBox *box;
	BMessage *msg;
	char str[128];
	char *item_name;
	char portname[] = "Port X";
	portname[5] = 'A'+joyport-1;
	
	SetViewColor(220,220,220,0);
	for (
		device = 0; 
		device < NUM_OF_SOFTDEVICES+hardware_joystick_count; 
		device++) 
	{
		if (device < NUM_OF_SOFTDEVICES) {
			item_name = joydevice_name[device];
		} else {
			sprintf(str,"%s (stick %d-%d)",
				hardware_joystick[device-NUM_OF_SOFTDEVICES].device_name,
				hardware_joystick[device-NUM_OF_SOFTDEVICES].stick,
				hardware_joystick[device-NUM_OF_SOFTDEVICES].axes);
			item_name = str;
		}

		msg = new BMessage(JOYMESSAGE_DEVPORT);
		msg->AddInt32("device_num", device);
		msg->AddInt32("joy_port", joyport);

		radio_joystick_device[device] = 
			new BRadioButton(BRect
				(10,20+device*20,160,30+device*20),
			item_name, item_name, msg);

		AddChild(radio_joystick_device[device]);
	}
	
   	resources_get_sprintf("JoyDevice%d",
   		(resource_value_t *)&device, joyport);

    if (device < NUM_OF_SOFTDEVICES+hardware_joystick_count)
		radio_joystick_device[device]->SetValue(1);
	r.InsetBy(5,5);
	r.OffsetTo(5,5);
	box = new BBox(r);
	box->SetLabel(portname);
	AddChild(box);
}


/* definition for JoystickWindow */
JoystickWindow::JoystickWindow() 
	: BWindow(BRect(50,50,400,230+hardware_joystick_count*20),"Joystick settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BRect r;
	BView *background;
	BCheckBox *checkbox;
	int res_value;
	
	r = Bounds();
	background = new BView(r, "backview", B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);

	r = Bounds();
	r.right -= r.Width()/2;
	r.bottom -= 70;
	background->AddChild(new JoyView(r,1));
	
	r = Bounds();
	r.left += r.Width()/2;
	r.bottom -= 70;
	background->AddChild(new JoyView(r,2));
	
	r = Bounds();
	r.top = r.bottom - 70;
	r.bottom -= 30;
	r.right -= r.Width()/2;
	r.InsetBy(5,10);
	background->AddChild(new BButton(r, "Define1", "Define Keyset 1",
		new BMessage(JOYMESSAGE_KEYSET1)));
	
	r.OffsetBy(r.Width()+10, 0);
	background->AddChild(new BButton(r, "Define2", "Define Keyset 2",
		new BMessage(JOYMESSAGE_KEYSET2)));

	r = Bounds();
	r.top = r.bottom-35;
	r.InsetBy(10,10);
	checkbox = new BCheckBox(r, "Joydisplay", "Enable display for joysticks",
		new BMessage(JOYMESSAGE_DISPLAY));
   	resources_get_value("JoystickDisplay", (void *)&res_value);
   	checkbox->SetValue(res_value);
   	background->AddChild(checkbox);

	Show();
}


JoystickWindow::~JoystickWindow() 
{
	joywindow = NULL;	
}


void JoystickWindow::MessageReceived(BMessage *msg) {
	int32 port,device;
	char resource_name[16];
	int res_value;
		
	switch(msg->what) {
		case JOYMESSAGE_DEVPORT:
			msg->FindInt32("device_num", &device);
			msg->FindInt32("joy_port", &port);
			sprintf(resource_name, "JoyDevice%d", port); 	
			resources_set_value(resource_name, (resource_value_t) device);
			break;
		case JOYMESSAGE_KEYSET1:
			keysetwindow = new KeysetWindow(1);
			while (keysetwindow);
			break;
		case JOYMESSAGE_KEYSET2:
			keysetwindow = new KeysetWindow(2);
			while (keysetwindow);
			break;
		case JOYMESSAGE_DISPLAY:
		   	resources_get_value("JoystickDisplay", (void *)&res_value);
		   	resources_set_value("JoystickDisplay", (resource_value_t )!res_value);
		   	break;
		default:
			BWindow::MessageReceived(msg);
	}
}


/* definition for KeysetWindow */
static char *keydefine_texts[] = {
    "NorthWest",
    "North",
    "NorthEast",
    "East",
    "SouthEast",
    "South",
    "SouthWest",
    "West",
    "Fire"
};

static struct _point { int x; int y; } keydefine_pos[] = {
	{ 0, 0 },
	{ 80, 0 },
	{ 160, 0 },
	{ 160, 30 },
	{ 160, 60 },
	{ 80, 60 },
	{ 0, 60 },
	{ 0, 30 },
	{ 80, 30 }
};

static char *keydefine_resource[] = { 
	"KeySet%dNorthWest",
    "KeySet%dNorth",
    "KeySet%dNorthEast",
    "KeySet%dEast",
    "KeySet%dSouthEast",
    "KeySet%dSouth",
    "KeySet%dSouthWest",
    "KeySet%dWest",
    "KeySet%dFire"
};

static char *keyset_instruction_first = 
	"Choose a direction by pressing the corresponding button.";
static char *keyset_instruction_last = 
	"Now press the key for %s or press button again for <None>";


static int keyset[9];

KeysetWindow::KeysetWindow(int set_nr) 
	: BWindow(BRect(410,50,410,50),"",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BRect r;
	BView *background;
	BMessage *msg;
	char str[20];
	int key_nr;
	
	last_key = -1;
	keyset_setnr = set_nr;
	
	sprintf(str, "Keyset %d", set_nr);
	SetTitle(str);
	ResizeTo(240,150);

	r = Bounds();
	background = new BView(r, "backview", B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,255);
	AddChild(background);

	for (key_nr = 0; key_nr <9; key_nr++) {
        resources_get_sprintf(keydefine_resource[key_nr],
        	(resource_value_t *)&keyset[key_nr], set_nr);
		if (keyset[key_nr]>255)
			/* invalid code */
			keyset[key_nr] = 0;
		r = BRect(
			keydefine_pos[key_nr].x,
			keydefine_pos[key_nr].y,
			keydefine_pos[key_nr].x + 80,
			keydefine_pos[key_nr].y + 30);
		r.InsetBy(10,10);
		
		msg = new BMessage(JOYMESSAGE_KEYTEXT);
		msg->AddInt32("direction", key_nr);
		keyset_button[key_nr] = new BButton(r, 
			keydefine_texts[key_nr], 
			kbd_code_to_string(keyset[key_nr]),
			msg);
		
		background->AddChild(keyset_button[key_nr]);
	}

	keyset_instruction = new BTextView(
		BRect(10, 100, 230, 140),
		"instructions",
		BRect(10, 5, 210, 35),
		B_FOLLOW_NONE,
		B_WILL_DRAW);
	background->AddChild(keyset_instruction);
	keyset_instruction->MakeEditable(false);
	keyset_instruction->MakeSelectable(false);
	keyset_instruction->SetViewColor(180,180,180,0);
	keyset_instruction->SetText(keyset_instruction_first);

	Show();
}


KeysetWindow::~KeysetWindow()
{
	keysetwindow = NULL;
}


void KeysetWindow::MessageReceived(BMessage *msg) {
	
	int32 button_nr;
	int32 key;
	char str[256];
	
	switch(msg->what) {
		case JOYMESSAGE_KEYTEXT:
			msg->FindInt32("direction", &button_nr);
			if (last_key != button_nr) {
				last_key = button_nr;
				sprintf(str, keyset_instruction_last, 
					keydefine_texts[last_key]);
				keyset_instruction->SetText(str);
			} else {
				/* select a button twice clears the key */
				resources_set_sprintf(keydefine_resource[last_key],
					(resource_value_t) 0, keyset_setnr);
				keyset_button[last_key]->SetLabel(
					kbd_code_to_string(0));
				keyset_instruction->SetText(keyset_instruction_first);
				last_key = -1;
			}
			break;
		case B_KEY_DOWN:
		case B_UNMAPPED_KEY_DOWN:
			msg->FindInt32("key",(int32*)&key);
			if (last_key >= 0) {
				resources_set_sprintf(keydefine_resource[last_key],
					(resource_value_t) key, keyset_setnr);
				keyset_button[last_key]->SetLabel(
					kbd_code_to_string(key));
			}
			keyset_instruction->SetText(keyset_instruction_first);
			last_key = -1;
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}


/* the interface to the ui */
void ui_joystick() {
	
	if (joywindow != NULL)
		return;

	joywindow = new JoystickWindow;

	vsync_suspend_speed_eval();
	while (joywindow); /* wait until window closed */
}

