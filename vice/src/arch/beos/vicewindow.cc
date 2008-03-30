/*
 * vicewindow.cc - Implementation of the BeVICE's window
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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


#include <Alert.h>
#include <Application.h>
#include <MenuItem.h>


extern "C" {
#include "attach.h"
#include "constants.h"
#include "interrupt.h"
#include "log.h"
#include "kbd.h"
#include "main.h"
#include "keyboard.h"
#include "machine.h"
#include "maincpu.h"
#include "main_exit.h"
#include "uiapi.h"
#include "ui_file.h"
#include "vicewindow.h"
}


static void about_vice() {
	char abouttext[2000];
	BAlert *aboutalert;
	sprintf(abouttext,
"BeVICE Version %s alpha\n\
Copyright (c) 1996-1999 Ettore Perazzoli\n\
Copyright (c) 1997-2000 Daniel Sladic\n\
Copyright (c) 1998-2000 Andreas Boose\n\
Copyright (c) 1998-2000 Tibor Biczo\n\
Copyright (c) 1999-2000 Andreas Dehmel\n\
Copyright (c) 1999-2000 Thomas Bretz\n\
Copyright (c) 1999-2000 Andreas Matthies\n\
Copyright (c) 1999-2000 Martin Pottendorfer\n\
Copyright (c) 1996-1999 Andre' Fachat\n\
Copyright (c) 1993-1994, 1997-1999 Teemu Rantanen\n\n\n\
reSID engine:\n\
Copyright (c) 2000 Dag Lem\n\n\
Official VICE homepage:\n\
http://www.cs.cmu.edu/~dsladic/vice/vice.html",
		VERSION);
	aboutalert = new BAlert("about",abouttext,"OK");
	aboutalert->Go();	
}

ViceWindow::ViceWindow(BRect frame, char const *title) 
		: BWindow(BRect(frame.left,frame.top,frame.right,frame.bottom+20), title,
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) {

	BMenu *menu;
	BMenuItem *item;
	
	/* create the menubar */
	menubar = new BMenuBar (BRect(0,0,10,10),"Menubar");
	AddChild(menubar);
	
	/* create the FILE menu */
	menu = new BMenu("File");
	menu->AddItem(new BMenuItem("Attach Disk to Drive 8", 
		new BMessage(MENU_ATTACH_DISK8), '8'));
	menu->AddItem(new BMenuItem("Attach Disk to Drive 9", 
		new BMessage(MENU_ATTACH_DISK9), '9'));
	menu->AddItem(new BMenuItem("Attach Tape", 
		new BMessage(MENU_ATTACH_TAPE), 'T'));
	menu->AddSeparatorItem();
	menu->AddItem(item = new BMenuItem("Soft Reset", 
		new BMessage(MENU_RESET_SOFT), 'R'));
	menu->AddItem(new BMenuItem("Hard Reset", 
		new BMessage(MENU_RESET_HARD), 'R', B_CONTROL_KEY));
	menubar->AddItem(menu);
	
	/* create the HELP menu */
	menu = new BMenu("Help");
	menu->AddItem(new BMenuItem("About BeVICE...", 
		new BMessage(MENU_ABOUT)));
	menubar->AddItem(menu);
	
	/* create the File Panel */
	filepanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
				B_FILE_NODE, false);
		
	bitmap = new BBitmap(frame,B_CMAP8,false,true);
	view = new BView(BRect(frame.left,frame.top+20,frame.right,frame.bottom+20),
		"title",	B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_WILL_DRAW);
	AddChild(view);

	/* finally display the window */
	Show();
}

ViceWindow::~ViceWindow() {
	RemoveChild(menubar);
	delete menubar;
	RemoveChild(view);
	delete view;
	delete filepanel;
}


bool ViceWindow::QuitRequested() {
	
	BAlert *alert = new BAlert("Quit BeVICE", 
		"Do you really want to exit BeVICE??",
		"Yes","No", NULL, B_WIDTH_AS_USUAL, B_INFO_ALERT);
	int32 button = alert->Go();
	if (button == 0) {
		BMessenger messenger(APP_SIGNATURE);
		BMessage message(WINDOW_CLOSED);
	
		messenger.SendMessage(&message, this);
	}
	/* don't close the window here, the vicethread has to be killed first */
	return false;
}

void ViceWindow::MessageReceived(BMessage *message) {

	int key;
	int attachdrive;
	char *s;

	switch(message->what) {
		case MENU_RESET_SOFT:
			maincpu_trigger_reset();
			break;
		case MENU_RESET_HARD:
			machine_powerup();
			break;
		case MENU_ATTACH_DISK8:
			attachdrive = 8;
			ui_select_file(filepanel,DISKFILE,attachdrive);
			break;
		case MENU_ATTACH_DISK9:
			attachdrive = 9;
			ui_select_file(filepanel,DISKFILE,attachdrive);
			break;
		case MENU_ATTACH_TAPE:
			attachdrive = 1;
			ui_select_file(filepanel,TAPEFILE,1);
			break;
		case B_KEY_DOWN:
		case B_UNMAPPED_KEY_DOWN:
			message->FindInt32("key",(int32*)&key);
			kbd_handle_keydown(key);
			break;
		case B_KEY_UP:
		case B_UNMAPPED_KEY_UP:
			message->FindInt32("key",(int32*)&key);
			kbd_handle_keyup(key);
			break;
		case MENU_ABOUT:
			about_vice();
			break;
		case B_REFS_RECEIVED:
			/* the file panel was closed, now we can use the selected file */
			ui_select_file_action(message);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}

}

void ViceWindow::Resize(unsigned int width, unsigned int height) {
	BWindow::ResizeTo(width,height+20);
	if (BWindow::Lock()) {
		view->ResizeTo(width, height);
		BWindow::Unlock();
	}
	delete bitmap;
	bitmap = new BBitmap(BRect(0,0,width-1,height-1),B_CMAP8,false,true);
}

void ViceWindow::DrawBitmap(void) {
	if	(BWindow::Lock()) {
		view->DrawBitmap(bitmap);
		view->Sync();
		BWindow::Unlock();
	}
}
