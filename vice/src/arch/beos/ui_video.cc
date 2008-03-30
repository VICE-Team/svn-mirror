/*
 * ui_video.cc - Video settings
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
#include <Directory.h>
#include <Entry.h>
#include <ListItem.h>
#include <ListView.h>
#include <RadioButton.h>
#include <ScrollView.h>
#include <Slider.h>
#include <string.h>
#include <Window.h>

#include <stdlib.h>

extern "C" { 
#include "archapi.h"
#include "constants.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "ui_video.h"
#include "utils.h"
#include "vsync.h"
}

static struct _colorcontrol {
	char *name;
	char *res_name;
	int multiplier;
	BSlider *slider;
} color_control[] =
{
	{ "Saturation", "ColorSaturation", 2000, NULL },
	{ "Contrast", "ColorContrast", 2000, NULL },
	{ "Brightness", "ColorBrightness", 2000, NULL },
	{ "Gamma", "ColorGamma", 2000, NULL },
	{ "PAL Scanline Shade", "PALScanLineShade", 1000, NULL },
	{ NULL, NULL, 0, NULL}
};

static char *modes[]=
{
	"Fast PAL",
	"Y/C cable (sharp)",
	"Composite (blurry)",
	NULL
};


BListView *palettelistview;


class VideoWindow : public BWindow {
	public:
		VideoWindow();
		~VideoWindow();
		virtual void MessageReceived(BMessage *msg);
};	


static VideoWindow *videowindow = NULL;


VideoWindow::VideoWindow() 
	: BWindow(BRect(250,50,500,300),"Video settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BMessage *msg;
	BSlider *slider;
	BCheckBox *checkbox;
	BRect r;
	BBox *box;
	BView *background;
	BDirectory dir;
	BEntry entry;
	int res_val, i;
	char *dirpath;
	char *palettefile;
	BRadioButton *rb_mode;

	r = Bounds();
	background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);
	
	/* Sliders for color control */
	for (i=0; color_control[i].name; i++)
	{
		if (resources_get_value(color_control[i].res_name, 
			(void *)&res_val) == 0)
		{
			msg = new BMessage(MESSAGE_VIDEO_COLOR);
			msg->AddInt32("index", i);
			color_control[i].slider = new BSlider(
				BRect(5,10+i*45,100,30+i*45), 
				color_control[i].name, color_control[i].name,
				msg, 0, color_control[i].multiplier, B_TRIANGLE_THUMB);
			color_control[i].slider->SetValue(res_val);
			color_control[i].slider->SetHashMarkCount(11);
			color_control[i].slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
			background->AddChild(color_control[i].slider);
		}
	}
	
	/* External Palette */
	resources_get_value("ExternalPalette", (void *)&res_val);
	checkbox = new BCheckBox(BRect(110, 10, 240, 25),
		"External Palette", "External Palette",
		new BMessage(MESSAGE_VIDEO_EXTERNALPALETTE));
	checkbox->SetValue(res_val);
	background->AddChild(checkbox);
	
	resources_get_value("PaletteFile",	(void *)&palettefile);
	palettelistview = new BListView(BRect(110, 35, 220, 125), "Palette File");
	palettelistview->SetSelectionMessage(
		new BMessage(MESSAGE_VIDEO_PALETTEFILE));
	background->AddChild(new BScrollView("scroll", palettelistview,
		B_FOLLOW_LEFT|B_FOLLOW_TOP,
		0, false, true));
	dirpath = util_concat(archdep_boot_path(), "/", machine_name, NULL);
    dir = BDirectory(dirpath);
    free(dirpath);
    while (dir.GetNextEntry(&entry) != B_ENTRY_NOT_FOUND)
	{
		char s[255];
		BListItem *item;
		entry.GetName(s);
		if (strstr(s, ".vpl"))
			palettelistview->AddItem(item = new BStringItem(s));
		if (strncmp(s, palettefile, strlen(s))==0)
			palettelistview->Select(palettelistview->IndexOf(item));
	}

	/* PAL settings */
	if (resources_get_value("PALMode", 
		(void *)&res_val) == 0)
	{
		box = new BBox(BRect(110, 140, 240, 220));
		box->SetLabel("PAL Mode");
		background->AddChild(box);
		for (i=0; modes[i]!=NULL; i++)
		{
			msg = new BMessage(MESSAGE_VIDEO_PALMODE);
			msg->AddInt32("index", i);
			rb_mode = new BRadioButton(BRect
				(5,15+i*20,120,30+i*20),
				modes[i], modes[i], msg);
			
			if (res_val == i)
				rb_mode->SetValue(1);

			box->AddChild(rb_mode);
		}		
	}
	
	Show();
}

VideoWindow::~VideoWindow() 
{
	videowindow = NULL;	
}

void VideoWindow::MessageReceived(BMessage *msg) {
	int32 index, val;	
	resource_value_t dummy;
	BMessage *msr;
	BListItem *item;
	
	switch (msg->what) {
		case MESSAGE_VIDEO_COLOR:
			index = msg->FindInt32("index");
			val = color_control[index].slider->Value();
			msr = new BMessage(MESSAGE_SET_RESOURCE);
			msr->AddString("resname", color_control[index].res_name);
			msr->AddInt32("resval", val);
			ui_add_event((void*)msr);
			break;
		case MESSAGE_VIDEO_EXTERNALPALETTE:
			resources_get_value("ExternalPalette", (void *)&val);
			msr = new BMessage(MESSAGE_SET_RESOURCE);
			msr->AddString("resname", "ExternalPalette");
			msr->AddInt32("resval", 1-val);
			ui_add_event((void*)msr);
			break;
		case MESSAGE_VIDEO_PALETTEFILE:
			item = palettelistview->ItemAt(
				palettelistview->CurrentSelection());
			if (item)
			{
				msr = new BMessage(MESSAGE_SET_RESOURCE);
				msr->AddString("resname", "PaletteFile");
				msr->AddString("resvalstr", ((BStringItem*) item)->Text());
				ui_add_event((void*)msr);
			}
			break;
		case MESSAGE_VIDEO_PALMODE:
			index = msg->FindInt32("index");
			msr = new BMessage(MESSAGE_SET_RESOURCE);
			msr->AddString("resname", "PALMode");
			msr->AddInt32("resval", index);
			ui_add_event((void*)msr);
			/* reset ExternalPalette to update the canvas */
			msr = new BMessage(MESSAGE_SET_RESOURCE);
			resources_get_value("ExternalPalette", (void *)&val);
			msr->AddString("resname", "ExternalPalette");
			msr->AddInt32("resval", val);
			ui_add_event((void*)msr);
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void ui_video() {
	
	if (videowindow != NULL)
		return;

	videowindow = new VideoWindow;

}

