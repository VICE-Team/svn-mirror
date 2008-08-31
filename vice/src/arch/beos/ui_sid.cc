/*
 * ui_sid.cc - SID settings
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
#include <ListItem.h>
#include <ListView.h>
#include <RadioButton.h>
#include <ScrollView.h>
#include <Slider.h>
#include <string.h>
#include <Window.h>

#include <stdlib.h>

extern "C" { 
#include "constants.h"
#include "machine.h"
#include "resources.h"
#include "sid.h"
#include "ui.h"
#include "ui_sid.h"
#include "util.h"
#include "vsync.h"
}

static char *samplingmode[] = {
	"fast sampling",
	"interpolating",
	"resampling",
	"fast resampling",
	NULL
};

static char *sidmodel[] = {
	"6581 (old)",
	"8580 (new)",
	"8580 (new+digiboost)",
#ifdef HAVE_RESID
	"DTVSID (reSID)",
#endif
#ifdef HAVE_RESID_FP
	"6581R3 4885 (reSID-fp)",
	"6581R3 0486S (reSID-fp)",
	"6581R3 3984 (reSID-fp)",
	"6581R4AR 3789 (reSID-fp)",
	"6581R3 4485 (reSID-fp)",
	"6581R4 1986S (reSID-fp)",
	"8580R5 3691 (reSID-fp)",
	"8580R5 3691 + digiboost (reSID-fp)",
	"8580R5 1489",
	"8580R5 1489 + digiboost (reSID-fp)",
#endif
	NULL
};

static int sidmodel_values[] = {
	SID_MODEL_6581,
	SID_MODEL_8580,
	SID_MODEL_8580D,
#ifdef HAVE_RESID
	SID_MODEL_DTVSID,
#endif
#ifdef HAVE_RESID_FP
	SID_MODEL_6581R3_4885,
	SID_MODEL_6581R3_0486S,
	SID_MODEL_6581R3_3984,
	SID_MODEL_6581R4AR_3789,
	SID_MODEL_6581R3_4485,
	SID_MODEL_6581R4_1986S,
	SID_MODEL_8580R5_3691,
	SID_MODEL_8580R5_3691D,
	SID_MODEL_8580R5_1489,
	SID_MODEL_8580R5_1489D,
#endif
	-1
};

static int c64sidaddressbase[] =
{ 0xd4, 0xd5, 0xd6, 0xd7, 0xde, 0xdf, -1 };

static int c128sidaddressbase[] =
{ 0xd4, 0xde, 0xdf, -1 };

static int cbm2sidaddressbase[] =
{ 0xda, -1 };


class SidWindow : public BWindow {
		BListView *addresslistview;
		BScrollView *scrollview;
		BSlider *passbandslider;
		BBox *residbox;
	public:
		SidWindow();
		~SidWindow();
		virtual void MessageReceived(BMessage *msg);
};	


static SidWindow *sidwindow = NULL;

void CreateAndGetAddressList(BListView *addresslistview, int mode)
{
    /* mode: 0=Create  1=get */	
    char st[12];
    int res_value;
    int adr, ladr, hi, index = -1;
    int *hadr;
    int cursel = addresslistview->CurrentSelection();
    BListItem *item;

    resources_get_int("SidStereoAddressStart", &res_value);

	switch (machine_class) {
		case VICE_MACHINE_C64:
			hadr = c64sidaddressbase;
			break;
		case VICE_MACHINE_C128:
			hadr = c128sidaddressbase;
			break;
		case VICE_MACHINE_CBM2:
			hadr = cbm2sidaddressbase;
			break;
		default:
			ui_error("CreateAddressList: This machine doesn't have a SID");
			return;
	}
	
    for (hi = 0; hadr[hi] >= 0; hi++)
    {
        for (ladr = (hi==0?0x20:0x0); ladr < 0x100; ladr += 0x20)
        {
            index++;
            sprintf(st, "$%02X%02X", hadr[hi], ladr);
            adr = hadr[hi]*0x100 + ladr;

            if (mode == 0)
            {
				addresslistview->AddItem(item = new BStringItem(st));
                if (adr == res_value)
					addresslistview->Select(
						addresslistview->IndexOf(item));
            } else if (index == cursel)
            {
                resources_set_int("SidStereoAddressStart", adr);
                return;
            }
        }
    }
}


SidWindow::SidWindow() 
	: BWindow(BRect(250,50,500,250),"Sid settings",
		B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BMessage *msg;
	BCheckBox *checkbox;
	BRect r;
	BBox *box;
	BRadioButton *radiobutton;
	BView *background;
	int res_val, i;

	r = Bounds();
	background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);

	/* SID model */
    resources_get_int("SidModel", &res_val);
	r = Bounds();
	r.bottom = r.top + 50;
	r.InsetBy(10,5);
	box = new BBox(r, "SID Model");
	box->SetViewColor(220,220,220,0);
	box->SetLabel("SID Model");
	
	for (i=0; sidmodel[i]!=NULL; i++) {
		msg = new BMessage(MESSAGE_SID_MODEL);
		msg->AddInt32("model", i);
		radiobutton = new BRadioButton(BRect
			(10+i*r.Width()/2,15,(i+1)*r.Width()/2-10,30),
			sidmodel[i], sidmodel[i], msg);
		radiobutton->SetValue(res_val == sidmodel_values[i]);
		box->AddChild(radiobutton);
	}
	background->AddChild(box);

	/* SID filter */
    resources_get_int("SidFilters", &res_val);
	checkbox = new BCheckBox(BRect(10, 60, r.Width()/2-40, 75),
		"SID Filters", "SID Filters",
		new BMessage(MESSAGE_SID_FILTERS));
	checkbox->SetValue(res_val);
	background->AddChild(checkbox);
	
	/* SID address */
    resources_get_int("SidStereoAddressStart", &res_val);
	addresslistview = new BListView(BRect(
		r.Width()-45, 65, r.Width()-10, 100), "");
	addresslistview->SetSelectionMessage(
		new BMessage(MESSAGE_SID_ADDRESS));
	background->AddChild(scrollview = new BScrollView("scroll",
		addresslistview,
		B_FOLLOW_LEFT|B_FOLLOW_TOP,
		0, false, true));
	CreateAndGetAddressList(addresslistview, 0);
	addresslistview->ScrollToSelection();

	/* Stereo SID */
    resources_get_int("SidStereo", &res_val);
	checkbox = new BCheckBox(BRect(r.Width()/2-20, 60, r.Width()-50, 75),
		"Stereo SID at", "Stereo SID at",
		new BMessage(MESSAGE_SID_STEREO));
	checkbox->SetValue(res_val);
	background->AddChild(checkbox);
	if (!res_val)
		scrollview->Hide();
	
	/* ReSID */
    resources_get_int("SidEngine", &res_val);
	checkbox = new BCheckBox(BRect(10, 90, r.Width()/2-20, 105),
		"Enable reSID", "Enable reSID",
		new BMessage(MESSAGE_SID_RESID));
	checkbox->SetValue(res_val == SID_ENGINE_RESID);
	background->AddChild(checkbox);

	/* reSID settings */
	residbox = new BBox(BRect(10,110,r.Width()-10,190), "reSID/reSID-fp settings");
	residbox->SetViewColor(220,220,220,0);
	residbox->SetLabel("reSID/reSID-fp settings");
	background->AddChild(residbox);
	r = residbox->Bounds();

    /* sampling method */
    resources_get_int("SidResidSampling", &res_val);
	for (i=0; samplingmode[i]!=NULL; i++) {
		msg = new BMessage(MESSAGE_SID_RESIDSAMPLING);
		msg->AddInt32("mode", i);
		radiobutton = new BRadioButton(BRect
			(10,15+i*20,r.Width()/2-10,30+i*20),
			samplingmode[i], samplingmode[i], msg);
		radiobutton->SetValue(res_val == i);
		residbox->AddChild(radiobutton);
	}

    resources_get_int("SidResidPassband", &res_val);
	passbandslider = new BSlider(
			BRect(r.Width()/2+10,20,r.Width()-10,60), 
			"Passband", "Passband",
			new BMessage(MESSAGE_SID_RESIDPASSBAND),
			0, 90, B_TRIANGLE_THUMB);
	passbandslider->SetValue(res_val);
	passbandslider->SetHashMarkCount(10);
	passbandslider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	passbandslider->SetLimitLabels("0", "90");
	residbox->AddChild(passbandslider);

    resources_get_int("SidEngine", &res_val);
	if (res_val != SID_ENGINE_RESID) residbox->Hide();

	Show();
}

SidWindow::~SidWindow() 
{
	sidwindow = NULL;	
}

void SidWindow::MessageReceived(BMessage *msg) {
	int32 val;	
	resource_value_t dummy;
	BListItem *item;
	
	switch (msg->what) {
		case MESSAGE_SID_MODEL:
			val = msg->FindInt32("model");
			resources_set_int("SidModel", sidmodel_values[val]);
			break;
		case MESSAGE_SID_FILTERS:
			resources_toggle("SidFilters", (int *)&dummy);
			break;
		case MESSAGE_SID_STEREO:
			resources_toggle("SidStereo", (int *)&dummy);
			addresslistview->ScrollToSelection();
			dummy?scrollview->Show():scrollview->Hide();
			break;
		case MESSAGE_SID_ADDRESS:
			CreateAndGetAddressList(addresslistview, 1);
			break;
		case MESSAGE_SID_RESID:
			resources_toggle("SidEngine", (int *)&dummy);
			dummy?residbox->Show():residbox->Hide();
			break;
		case MESSAGE_SID_RESIDSAMPLING:
			val = msg->FindInt32("mode");
			resources_set_int("SidResidSampling", val);
			break;
		case MESSAGE_SID_RESIDPASSBAND:
			resources_set_int("SidResidPassband",
				passbandslider->Value());
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void ui_sid() {
	thread_id sidthread;
	status_t exit_value;
	
	if (sidwindow != NULL)
		return;

	sidwindow = new SidWindow;

	vsync_suspend_speed_eval();

	/* wait until window closed */
	sidthread=sidwindow->Thread();
	wait_for_thread(sidthread, &exit_value);
}
