/*
 * ui_drive.cc - Drive settings
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
#include <TabView.h>
#include <Window.h>

extern "C" { 
#include "drive.h"
#include "resources.h"
#include "ui.h"
#include "ui_drive.h"
#include "vsync.h"
}

static struct _drive_type {
	char *name;
	int  id;
} drive_type[] = {
	{"1541", DRIVE_TYPE_1541 },
	{"1541-II", DRIVE_TYPE_1541II },
	{"1551", DRIVE_TYPE_1551 },
	{"1571", DRIVE_TYPE_1571 },
	{"1581", DRIVE_TYPE_1581 },
	{"2031", DRIVE_TYPE_2031 },
	{"2040", DRIVE_TYPE_2040 },
	{"3040", DRIVE_TYPE_3040 },
	{"4040", DRIVE_TYPE_4040 },
	{"1001", DRIVE_TYPE_1001 },
	{"8050", DRIVE_TYPE_8050 },
	{"8250", DRIVE_TYPE_8250 },
	{"None", DRIVE_TYPE_NONE },
	{NULL, 0}
};
	
struct _drive_extendimagepolicy {
	char *name;
	int  id;
	BRadioButton *radiobutton; /* we have to remeber for enable/disable them */
} drive_extendimagepolicy[] = {
	{"Never extend", DRIVE_EXTEND_NEVER, NULL },
	{"Ask on extend", DRIVE_EXTEND_ASK, NULL },
	{"Extend on access", DRIVE_EXTEND_ACCESS, NULL },
	{NULL, 0, NULL }
};

struct _drive_expansion {
	char *name;
	char *resource_name;
} drive_expansion[] = {
	{"$2000-$3FFF RAM", "Drive%dRAM2000" },
	{"$4000-$5FFF RAM", "Drive%dRAM4000" },
	{"$6000-$7FFF RAM", "Drive%dRAM6000" },
	{"$8000-$9FFF RAM", "Drive%dRAM8000" },
	{"$A000-$BFFF RAM", "Drive%dRAMA000" },
	{NULL, NULL }
};

struct _drive_idlemethod {
	char *name;
	int  id;
} drive_idlemethod[] = {
	{"None", DRIVE_IDLE_NO_IDLE },
	{"Trap idle", DRIVE_IDLE_TRAP_IDLE },
	{"Skip cycles", DRIVE_IDLE_SKIP_CYCLES },
	{NULL, 0 }
};


class DriveView : public BView {
	public:
		DriveView(BRect r, int drive_num);
		void EnableControlsForDriveSettings(int type_index);
	private:
		/* we have to remember some controls to enable/disable them */
		BRadioButton *rb_extendimagepolicy[3];
		BRadioButton *rb_idlemethod[3];
		BCheckBox    *cb_expansion[5];
		BCheckBox    *cb_parallelcable;
		
};

void DriveView::EnableControlsForDriveSettings(int type_index) {
	int i;
	int current_drive_type;
	bool expand_is_possible;
	
	current_drive_type = drive_type[type_index].id;
	
	for (i=0; drive_extendimagepolicy[i].name; i++) {
		rb_extendimagepolicy[i]->SetEnabled(
			drive_check_extend_policy(current_drive_type));
	}
	for (i=0; drive_idlemethod[i].name; i++) {
		rb_idlemethod[i]->SetEnabled(
			drive_check_idle_method(current_drive_type));
	}
	cb_parallelcable->SetEnabled(
		drive_check_parallel_cable(current_drive_type));
	for (i=0; drive_expansion[i].name; i++) {
		switch (i) {
			case 0:
				expand_is_possible = DRIVE_EXPANSION_2000(current_drive_type);
				break;
			case 1:
				expand_is_possible = DRIVE_EXPANSION_4000(current_drive_type);	
				break;
			case 2:
				expand_is_possible = DRIVE_EXPANSION_6000(current_drive_type);	
				break;
			case 3:
				expand_is_possible = DRIVE_EXPANSION_8000(current_drive_type);	
				break;
			case 4:
				expand_is_possible = DRIVE_EXPANSION_A000(current_drive_type);
				break;
			default:
				expand_is_possible = false;
		}
		cb_expansion[i]->SetEnabled(expand_is_possible);
	}
}
	

DriveView::DriveView(BRect r, int drive_num)
	: BView(r, "drive_view", B_FOLLOW_NONE, B_WILL_DRAW)
{
	int i;
	int current_type;
	int current_extendimagepolicy;
	int current_idlemethod;
	int current_expansion;
	int current_parallelcable;
	BMessage *msg;
		
	BBox *box;
	BRadioButton *radiobutton;
	BCheckBox *checkbox;
			
	BView::SetViewColor(220,220,220,0);
	
   	resources_get_sprintf("Drive%dType",
   		(resource_value_t *)&current_type, drive_num);
   	resources_get_sprintf("Drive%dExtendImagePolicy",
   		(resource_value_t *)&current_extendimagepolicy, drive_num);
   	resources_get_sprintf("Drive%dIdleMethod",
   		(resource_value_t *)&current_idlemethod, drive_num);
	
	/* extend image policy */
	r.OffsetTo(90,0);
	r.right = 220;
	r.bottom = 95;
	box = new BBox(r);
	box->SetLabel("40 track handling");
	AddChild(box);
	r.InsetBy(10,10);
	for (i=0; drive_extendimagepolicy[i].name; i++) {
		msg = new BMessage(MESSAGE_DRIVE_EXTENDIMAGEPOLICY);
		msg->AddInt32("drive_num", drive_num);
		msg->AddInt32("resource_index", i);
		radiobutton = new BRadioButton(
			BRect(10,20+i*25,120,30+i*25),
			drive_extendimagepolicy[i].name,
			drive_extendimagepolicy[i].name,
			msg);
		box->AddChild(radiobutton);
		rb_extendimagepolicy[i] = radiobutton;
		if (drive_extendimagepolicy[i].id == current_extendimagepolicy)
			radiobutton->SetValue(1);
	}

	/* drive expansion */
	r.OffsetTo(90,100);
	r.right = 220;
	r.bottom = 245;
	box = new BBox(r);
	box->SetLabel("Drive expansion");
	AddChild(box);
	r.InsetBy(10,10);
	for (i=0; drive_expansion[i].name; i++) {
		msg = new BMessage(MESSAGE_DRIVE_EXPANSION);
		msg->AddInt32("drive_num", drive_num);
		msg->AddInt32("resource_index", i);
		checkbox = new BCheckBox(
			BRect(10,20+i*25,120,30+i*25),
			drive_expansion[i].name,
			drive_expansion[i].name,
			msg);
		box->AddChild(checkbox);
		cb_expansion[i] = checkbox;
    	resources_get_sprintf(drive_expansion[i].resource_name,
    		(resource_value_t *)&current_expansion, drive_num);
		checkbox->SetValue(current_expansion);
	}
	
	/* parallel cable */
	msg = new BMessage(MESSAGE_DRIVE_PARALLELCABLE);
	msg->AddInt32("drive_num", drive_num);
	msg->AddInt32("resource_index", i);
	checkbox = new BCheckBox(
		BRect(240,120,360,130),
		"Parallel Cable",
		"Parallel Cable",
		msg);
	AddChild(checkbox);
	cb_parallelcable = checkbox;
   	resources_get_sprintf("Drive%dParallelCable",
   		(resource_value_t *)&current_parallelcable, drive_num);
   	checkbox->SetValue(current_parallelcable);
   	
	/* idle method */
	r.OffsetTo(230,0);
	r.right = 330;
	r.bottom = 95;
	box = new BBox(r);
	box->SetLabel("Idle method");
	AddChild(box);
	r.InsetBy(10,10);
	for (i=0; drive_idlemethod[i].name; i++) {
		msg = new BMessage(MESSAGE_DRIVE_IDLEMETHOD);
		msg->AddInt32("drive_num", drive_num);
		msg->AddInt32("resource_index", i);
		radiobutton = new BRadioButton(
			BRect(10,20+i*25,90,30+i*25),
			drive_idlemethod[i].name,
			drive_idlemethod[i].name,
			msg);
		box->AddChild(radiobutton);
		rb_idlemethod[i] = radiobutton;
		if (drive_idlemethod[i].id == current_idlemethod)
			radiobutton->SetValue(1);
	}

	/* at last drive type, so we can enable/disable other controls */
	r.OffsetTo(0,0);
	r.right = 80;
	r.bottom = 345;
	box = new BBox(r);
	box->SetLabel("Drive type");
	AddChild(box);
	r.InsetBy(10,10);
	for (i=0; drive_type[i].name; i++) {
		msg = new BMessage(MESSAGE_DRIVE_TYPE);
		msg->AddInt32("drive_num", drive_num);
		msg->AddInt32("resource_index", i);
		radiobutton = new BRadioButton(
			BRect(10,20+i*25,70,30+i*25),
			drive_type[i].name,
			drive_type[i].name,
			msg);
		radiobutton->SetEnabled(drive_check_type(drive_type[i].id, drive_num-8));
		box->AddChild(radiobutton);
		
		if (drive_type[i].id == current_type) {
			radiobutton->SetValue(1);
			EnableControlsForDriveSettings(i);
		}
	}

}


class DriveWindow : public BWindow {
	public:
		DriveWindow();
		~DriveWindow();
		virtual void MessageReceived(BMessage *msg);
	private:
		DriveView *dv[2]; /* pointers to the DriveViews 8 and 9 */
};	


static DriveWindow *drivewindow = NULL;


DriveWindow::DriveWindow() 
	: BWindow(BRect(50,50,400,435),"Drive settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BRect r;
	BTabView *tabview;
	BTab *tab;
	
	r = Bounds();
	
	tabview = new BTabView(r, "tab_view");
	tabview->SetViewColor(220,220,220,0);
	
	r = tabview->Bounds();
	r.InsetBy(5,5);
	r.bottom -= tabview->TabHeight();

	tab = new BTab();
	tabview->AddTab(dv[0] = new DriveView(r,8), tab);
	tab->SetLabel("Drive 8");

	tab = new BTab();
	tabview->AddTab(dv[1] = new DriveView(r,9), tab);
	tab->SetLabel("Drive 9");

	AddChild(tabview);
	tabview->SetTabWidth(B_WIDTH_FROM_WIDEST);
	Show();
}

DriveWindow::~DriveWindow() 
{
	drivewindow = NULL;	
}

void DriveWindow::MessageReceived(BMessage *msg) {
	int32 drive_num;
	int32 resource_index;
	resource_value_t dummy;
		
	msg->FindInt32("drive_num", &drive_num);
	msg->FindInt32("resource_index", &resource_index); 	

	switch (msg->what) {
		case MESSAGE_DRIVE_TYPE:
			resources_set_sprintf("Drive%dType",
				(resource_value_t) drive_type[resource_index].id,
				drive_num);
			dv[drive_num-8]->EnableControlsForDriveSettings(resource_index);
			break;
		case MESSAGE_DRIVE_EXTENDIMAGEPOLICY:
			resources_set_sprintf("Drive%dExtendImagePolicy",
				(resource_value_t) drive_extendimagepolicy[resource_index].id,
				drive_num);
			break;
		case MESSAGE_DRIVE_IDLEMETHOD:
			resources_set_sprintf("Drive%dIdleMethod",
				(resource_value_t) drive_idlemethod[resource_index].id,
				drive_num);
			break;
		case MESSAGE_DRIVE_EXPANSION:
			resources_get_sprintf(drive_expansion[resource_index].resource_name,
				&dummy, drive_num);
			resources_set_sprintf(drive_expansion[resource_index].resource_name,
				(resource_value_t) !dummy, drive_num);
			break;
		case MESSAGE_DRIVE_PARALLELCABLE:
			resources_get_sprintf("Drive%dParallelCable", &dummy, drive_num);
			resources_set_sprintf("Drive%dParallelCable", 
				(resource_value_t) !dummy, drive_num);
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void ui_drive() {
	
	if (drivewindow != NULL)
		return;

	drivewindow = new DriveWindow;

	vsync_suspend_speed_eval();
	while (drivewindow); /* wait until window closed */
}

