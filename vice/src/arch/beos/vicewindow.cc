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
#include <FilePanel.h>
#include <MenuItem.h>

#include "vicemenu.h"

extern "C" {
#include "attach.h"
#include "constants.h"
#include "datasette.h"
#include "drive.h"
#include "info.h"
#include "interrupt.h"
#include "kbd.h"
#include "log.h"
#include "main.h"
#include "keyboard.h"
#include "machine.h"
#include "maincpu.h"
#include "main_exit.h"
#include "resources.h"
#include "sound.h"
#include "tape.h"
#include "ui.h"
#include "ui_file.h"
#include "utils.h"
#include "vicewindow.h"
#include "vsync.h"
}

/* FIXME: some stuff we need from the ui module */
extern ViceWindow 			*windowlist[];
extern int 					window_count;
extern ui_res_value_list   	*machine_specific_values;
extern ui_menu_toggle      	*machine_specific_toggles;


void ViceWindow::Update_Menus(
		ui_menu_toggle *toggle_list,
		ui_res_value_list *value_list)
{
    int i,j;
    int value;
    int result;
	BMenuItem *item;
	
	for (i = 0; toggle_list[i].name != NULL; i++) {
        resources_get_value(toggle_list[i].name, (resource_value_t *) &value);
        if (item = menubar->FindItem(toggle_list[i].item_id))
        	item->SetMarked(value ? true : false);
    }
    
    if (machine_specific_toggles) {
        for (i = 0; machine_specific_toggles[i].name != NULL; i++) {
            resources_get_value(machine_specific_toggles[i].name, (resource_value_t *) &value);
    	    if (item = menubar->FindItem(machine_specific_toggles[i].item_id))
	        	item->SetMarked(value ? true : false);
        }
    }

    for (i = 0; value_list[i].name != NULL; i++) {
        result=resources_get_value(value_list[i].name,
                                   (resource_value_t *) &value);
        if (result==0) {
            for (j = 0; value_list[i].vals[j].item_id != 0; j++) {
                if (value == value_list[i].vals[j].value) {
                	/* the corresponding menu is supposed to be in RadioMode */
                    if (item = menubar->FindItem(value_list[i].vals[j].item_id))
                    	item->SetMarked(true);
                }
            }
        }
    }

    if (machine_specific_values){
        for (i = 0; machine_specific_values[i].name != NULL; i++) {
            result=resources_get_value(machine_specific_values[i].name,
                                       (resource_value_t *) &value);
            if (result==0) {
                for (j = 0; machine_specific_values[i].vals[j].item_id != 0; j++) {
                    if (value == machine_specific_values[i].vals[j].value) {
                        if (item = menubar->FindItem(machine_specific_values[i].vals[j].item_id))
                        	item->SetMarked(true);
                    }
                }
            }
        }
    }
}


/* the view for the emulators bitmap, we need a special Draw() */
class ViceView : public BView {
	public:
		ViceView(BRect rect);
		virtual void Draw(BRect rect);
};

ViceView::ViceView(BRect rect) 
	: BView(rect,"view",B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW)
{
}

void ViceView::Draw(BRect rect) {
	ViceWindow *wnd = (ViceWindow *)Window();
	DrawBitmap(wnd->bitmap,rect,rect);
}



void about_vice() {
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
	suspend_speed_eval();
}

ViceWindow::ViceWindow(BRect frame, char const *title) 
		: BWindow(BRect(frame.left,frame.top,frame.right,frame.bottom+20), title,
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) {

	BMenu *menu, *submenu;
	BMenuItem *item;
	
	/* create the menubar */
	menubar = menu_create(machine_class);
	AddChild(menubar);

	/* create the File Panel */
	filepanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
				B_FILE_NODE, false);

	/* create the Save Panel */
	savepanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL,
				B_FILE_NODE, false);
		
	bitmap = new BBitmap(frame,B_CMAP8,false,true);
	view = new ViceView(BRect(frame.left,frame.top+20,frame.right,frame.bottom+20));
	AddChild(view);

	/* register the window */
	windowlist[window_count++] = this;

	/* finally display the window */
	Show();
}

ViceWindow::~ViceWindow() {
	RemoveChild(menubar);
	delete menubar;
	RemoveChild(view);
	delete view;
	delete filepanel;
	delete savepanel;
}


bool ViceWindow::QuitRequested() {

	BMessage msg;
	msg.what = MENU_EXIT;
	
	sound_suspend();
	BAlert *alert = new BAlert("Quit BeVICE", 
		"Do you really want to exit BeVICE??",
		"Yes","No", NULL, B_WIDTH_AS_USUAL, B_INFO_ALERT);
	int32 button = alert->Go();
	if (button == 0) {
		ui_command_type_t cmd = {UICMD_EXIT,0};
		ui_add_event(&msg);
	} else {
		sound_resume();
	}
	/* don't close the window here, the vicethread has to be killed first */
	return false;
}

void ViceWindow::MessageReceived(BMessage *message) {
	/* FIXME: sometimes the menubar holds the focus so we have to delete it */ 
	if (CurrentFocus()) {
		menubar->MakeFocus(false);
	}
		
	ui_add_event(message);
	switch(message->what) {
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void ViceWindow::Resize(unsigned int width, unsigned int height) {
	BRect oldsize;
	if (BWindow::Lock()) {
		view->ResizeTo(width, height);
		BWindow::Unlock();
	}
	BWindow::ResizeTo(width,height+20);
	/* create new bitmap if size has changed */
	/* notice, that BRect sizes are simply differences of coordinates */
	oldsize = bitmap->Bounds();
	if ((oldsize.Width()+1) != width || (oldsize.Height()+1) != height) {
		delete bitmap;
		bitmap = new BBitmap(BRect(0,0,width-1,height-1),B_CMAP8,false,true);
	}
}

void ViceWindow::DrawBitmap(void) {
	if	(BWindow::Lock()) {
		view->DrawBitmap(bitmap);
		view->Flush();
		BWindow::Unlock();
	} 
}
