/*
 * ui_file.cc - UI stuff dealing with files
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

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <Font.h>
#include <ListView.h>
#include <Message.h>
#include <Path.h>
#include <ScrollView.h>
#include <stdio.h>
#include <string.h>
#include <View.h>
#include <Window.h>

extern "C" {
#include "attach.h"
#include "autostart.h"
#include "c64ui.h"
#include "constants.h"
#include "imagecontents.h"
#include "machine.h"
#include "resources.h"
#include "tape.h"
#include "ui.h"
#include "uiapi.h"
#include "ui_file.h"
#include "util.h"
#include "vic20ui.h"
#include "vicemenu.h"
}

static int last_fileparam[2]; /* 0=filepanel, 1=savepanel */
static int last_filetype[2];
static BCheckBox *cb_readonly;


char *read_disk_image_contents(const char *name)
{
   return image_contents_read_string(IMAGE_CONTENTS_DISK, name, 0,
                                     IMAGE_CONTENTS_STRING_ASCII);
}

char *read_tape_image_contents(const char *name)
{
   return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, 0,
                                     IMAGE_CONTENTS_STRING_ASCII);
}

char *read_disk_or_tape_image_contents(const char *name)
{
   return image_contents_read_string(IMAGE_CONTENTS_AUTO, name, 0,
                                     IMAGE_CONTENTS_STRING_ASCII);
}



static void create_content_list(BListView *contentlist, char *text)
{
char    *start;
char    buffer[256];
int     index;

	while (contentlist->CountItems()) {
		BListItem *item = contentlist->FirstItem();
		contentlist->RemoveItem(item);
		delete item;
	}

    if (text==NULL) return;
    start=text;
    index=0;
    while (1) {
        if (*start=='\n') {
            buffer[index]=0;
            index=0;
            contentlist->AddItem(new BStringItem(buffer));
        } else if (*start==0x0d) {
        } else if (*start==0) {
            break;
        } else {
            buffer[index++]=*start;
        }
        start++;
    }
}


VicePreview::VicePreview(BPoint origin, ViceFilePanel *f)
	: BWindow(
		BRect(origin.x,origin.y,origin.x+300,origin.y+200),
		"Image Contents", B_MODAL_WINDOW_LOOK, B_FLOATING_APP_WINDOW_FEEL, B_AVOID_FOCUS)
{
	BView *background;
	BRect r;
	BFont font(be_fixed_font);
	
	father = f;		
	
	r = Bounds();
	background = new BView(r, "backview", B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);

	r.InsetBy(10,10);
	r.right -= 80;
	contentlist = new BListView(r, "contents", B_SINGLE_SELECTION_LIST);
	contentlist->SetInvocationMessage(new BMessage(AUTOSTART_MESSAGE));
	
	font.SetSize(10.0);
	contentlist->SetFont(&font);
	
	background->AddChild(new BScrollView("scroll_contents", contentlist, 
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
	
	background->AddChild(new BButton(BRect(r.right+20,10,r.right+80,30),
		"Autostart", "Autostart", new BMessage(AUTOSTART_MESSAGE)));

	Minimize(true);
	Show();
	Hide();
}

void VicePreview::DisplayContent(char* content)
{
	Lock();
	if (content)
	{
		if (IsHidden())
		{
			Minimize(false);
			Show();
		}
		create_content_list(contentlist, content);
	} else {
		if (!IsHidden())
			Hide();			
	}
	Unlock();
}


void VicePreview::MessageReceived(BMessage *msg)
{
	int file_num;
	
	switch (msg->what) {
		case AUTOSTART_MESSAGE:
			file_num = contentlist->CurrentSelection();
			if (file_num >= 0) {
				father->Hide();
				autostart_autodetect(image_name, NULL, file_num, AUTOSTART_MODE_RUN);
			}
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}


ViceFilePanel::ViceFilePanel(
	file_panel_mode mode,
	BMessenger* target,
	entry_ref* panel_directory,
	uint32 node_flavors,
	bool allow_multiple_selection)
	
	: BFilePanel(mode, target, panel_directory, node_flavors, allow_multiple_selection)
{
	if (mode == B_OPEN_PANEL) {
		/* readonly checkbox */
		if (Window()->Lock()) {
			BView *back = Window()->ChildAt(0);
			BRect rect = back->Bounds();
			rect.InsetBy(20,20);
			rect.right = rect.left + 150;
			rect.top = rect.bottom - 10;
			cb_readonly = new BCheckBox(rect,
				"Attach read only", "Attach read only", NULL);
			cb_readonly->SetTarget(Messenger());
			Window()->Unlock();
		}
		
		/* create window for the previewlist */
		previewwindow = new VicePreview(
			BPoint(Window()->Frame().left, Window()->Frame().bottom),
			this);
	} else {
		previewwindow = NULL;
	}
}


void ViceFilePanel::WasHidden(void)
{
	if (previewwindow)
		previewwindow->DisplayContent(NULL);
}
	
void ViceFilePanel::SelectionChanged(void)
{
	entry_ref ref;
	BPath *path;
	
	Rewind();
	if (GetNextSelectedRef(&ref) != B_ENTRY_NOT_FOUND && previewwindow)
	{
		path = new BPath(&ref);
		if (path->Path() && read_disk_or_tape_image_contents(path->Path()))
		{
			strncpy(previewwindow->image_name, path->Path(), 255); 
			previewwindow->DisplayContent(read_disk_or_tape_image_contents(path->Path()));
			previewwindow->MoveTo(BPoint(Window()->Frame().left, Window()->Frame().bottom+5));
			Window()->SendBehind(previewwindow);
		} else
		{
			previewwindow->DisplayContent(NULL);
		}
		delete path;
	}
	
	BFilePanel::SelectionChanged();
}


void ui_select_file(ViceFilePanel *filepanel, 
					filetype_t filetype, 
					void *fileparam) {

	int panelnr = (filepanel->PanelMode() == B_OPEN_PANEL ? 0 : 1);
	char title[40];
	sprintf(title,"VICE filepanel"); /* default */

	/* Modify the panel */
	/* first we may remove the readonly checkbox */
	if (filepanel->Window()->Lock()) {
		filepanel->Window()->ChildAt(0)->RemoveChild(cb_readonly);
		filepanel->Window()->Unlock();
	}
	if (filetype == DISK_FILE) {
		static BMessage msg = BMessage(MESSAGE_ATTACH_READONLY);
		int n;

		/* attach readonly checkbox */
		if (filepanel->Window()->Lock()) {
			filepanel->Window()->ChildAt(0)->AddChild(cb_readonly);
			msg.MakeEmpty();
			msg.AddInt32("device", *(int*)fileparam);
			cb_readonly->SetMessage(&msg);
	    	resources_get_sprintf("AttachDevice%dReadonly",
				(resource_value_t *)&n, *(int*)fileparam);
			cb_readonly->SetValue(n);
			filepanel->Window()->Unlock();
		}
			
		sprintf(title,"Attach Disk %d",*(int*)fileparam);
		last_fileparam[panelnr] = *(int*)fileparam;
	}
	if (filetype == TAPE_FILE)
		sprintf(title,"Attach Tape");
	if (filetype == AUTOSTART_FILE)
		sprintf(title,"Autostart");
	if (filetype == SNAPSHOTSAVE_FILE)
		sprintf(title,"Save snapshot");
	if (filetype == SNAPSHOTLOAD_FILE)
		sprintf(title,"Load snapshot");
	if (filetype == C64_CARTRIDGE_FILE || filetype == VIC20_CARTRIDGE_FILE) {
		sprintf(title,"Attach Cartridge (%s)",
			((ui_cartridge_t*)fileparam)->cart_name);
		last_fileparam[panelnr] = ((ui_cartridge_t*)fileparam)->cart_type;
	}
	if (filetype == VSID_FILE)
		sprintf(title,"Load psid file");

	filepanel->Window()->SetTitle(title);

	filepanel->Show();

	/* remember for later action */

	last_filetype[panelnr] = filetype;
}
	
void ui_select_file_action(BMessage *msg) {
		entry_ref 	ref;
		status_t 	err;
		BPath		*path;

	if (msg->what == B_REFS_RECEIVED) {
		/* an open action */		
		/* extract the selected filename from the message */
		if ((err = msg->FindRef("refs", 0, &ref)) != B_OK) {
			ui_error("No File selected ?!");
			return;
		}
		path = new BPath(&ref);
	
		/* now the ACTION */
    	if (last_filetype[0] == DISK_FILE) {
    		/* it's a disk-attach */
    		if (file_system_attach_disk(last_fileparam[0], path->Path()) < 0)
        		ui_error("Cannot attach specified file");
		} else if (last_filetype[0] == TAPE_FILE) {
			/* it's a tape-attach */
    		if (tape_image_attach(1, path->Path()) < 0)
        		ui_error("Cannot attach specified file");
		} else if (last_filetype[0] == AUTOSTART_FILE) {
			if (autostart_autodetect(path->Path(), NULL, 0, AUTOSTART_MODE_RUN) < 0)
  				ui_error("Cannot autostart specified file.");
		} else if (last_filetype[0] == SNAPSHOTLOAD_FILE) {
	    	if (machine_read_snapshot(path->Path(),0)<0) {
        		ui_error("Cannot read snapshot image");
        	}
    	} else if (last_filetype[0] == C64_CARTRIDGE_FILE) {
    		BMessage *msg = new BMessage(ATTACH_C64_CART);
    		msg->AddInt32("type", last_fileparam[0]);
    		msg->AddString("filename", path->Path());
    		ui_add_event(msg);
    	} else if (last_filetype[0] == VIC20_CARTRIDGE_FILE) {
    		BMessage *msg = new BMessage(ATTACH_VIC20_CART);
    		msg->AddInt32("type", last_fileparam[0]);
    		msg->AddString("filename", path->Path());
    		ui_add_event(msg);
    	} else if (last_filetype[0] == VSID_FILE) {
    		BMessage *msg = new BMessage(PLAY_VSID);
    		msg->AddString("filename", path->Path());
    		ui_add_event(msg);
   		}
		
		delete path;	
	}
	
	if (msg->what == B_SAVE_REQUESTED) {
		char *fullpath;
		const char *name;
		/* a save action */
		/* first create the full path */
		if ((err = msg->FindRef("directory", &ref)) != B_OK) {
			ui_error("Wrong directory");
			return;
		}
		if ((err = msg->FindString("name", &name)) != B_OK) {
			ui_error("Wrong name");
			return;
		}
		path = new BPath(&ref);
		fullpath = util_concat(path->Path(),"/",name,NULL);
		
		/* now the action */
		if (last_filetype[1] == SNAPSHOTSAVE_FILE) {
			if (machine_write_snapshot(fullpath, 1, 1, 0) < 0)
            	ui_error("Cannot write snapshot file.");
		}
		
		delete path;
		delete fullpath;
	}
}

