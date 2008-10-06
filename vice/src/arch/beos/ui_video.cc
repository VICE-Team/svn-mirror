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
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "ui_video.h"
#include "util.h"
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
	{ "PAL Blur", "PALBlur", 1000, NULL },
	{ "Tint", "ColorTint", 2000, NULL },
	{ "Odd Lines Phase", "PALOddLinePhase", 2000, NULL },
	{ "Odd Lines Offset", "PALOddLineOffset", 2000, NULL },
	{ NULL, NULL, 0, NULL}
};

static char *modes[]=
{
	"Fast PAL",
	"Old PAL emulation",
	"New PAL emulation",
	NULL
};

typedef struct {
    char *res_PaletteFile_name;
    char *res_ExternalPalette_name;
    char *page_title;
} Chip_Parameters;

static Chip_Parameters chip_param_table[] =
{
    { "VICIIPaletteFile", "VICIIExternalPalette",
      "VICII Palette"},
    { "VICPaletteFile", "VICExternalPalette",
      "VIC Palette"},
    { "CRTCPaletteFile", NULL,
      "CRTC Palette"},
    { "VDCPaletteFile", NULL,
      "VDC Palette"},
    { "TEDPaletteFile", "TEDExternalPalette",
      "TED Palette"},
};

static int chip[] = { -1, -1 };

BListView *palettelistview;


class VideoWindow : public BWindow {
	public:
		VideoWindow();
		~VideoWindow();
		virtual void MessageReceived(BMessage *msg);
};	


static VideoWindow *videowindow = NULL;


VideoWindow::VideoWindow() 
	: BWindow(BRect(250,50,500,345),"Video settings",
		B_TITLED_WINDOW, 
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
	BMessage *msg;
	BCheckBox *checkbox;
	BRect r;
	BBox *box;
	BView *background;
	BDirectory dir;
	BEntry entry;
	int res_val, i;
	char *dirpath;
	const char *palettefile_const;
	char *palettefile;
	BRadioButton *rb_mode;

	switch (machine_class) {
		case VICE_MACHINE_C128:
			chip[0] = 0;
			chip[1] = 3;
			break;
		case VICE_MACHINE_C64:
			chip[0] = 0;
			break;
		case VICE_MACHINE_CBM2:
			chip[0] = 2;
			break;
		case VICE_MACHINE_PET:
			chip[0] = 2;
			break;
		case VICE_MACHINE_PLUS4:
			chip[0] = 4;
			break;
		case VICE_MACHINE_VIC20:
			chip[0] = 1;
			break;
		default:
			chip[0] = 0;
			break;
	}			

	r = Bounds();
	background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
	background->SetViewColor(220,220,220,0);
	AddChild(background);
	
	/* Sliders for color control */
	for (i=0; color_control[i].name; i++)
	{
		if (chip_param_table[chip[0]].res_ExternalPalette_name != NULL
			&& resources_get_int(color_control[i].res_name, 
				&res_val) == 0)
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
	checkbox = new BCheckBox(BRect(110, 10, 240, 25),
		"External Palette", "External Palette",
		new BMessage(MESSAGE_VIDEO_EXTERNALPALETTE));
	background->AddChild(checkbox);
	if (chip_param_table[chip[0]].res_ExternalPalette_name != NULL) {
		resources_get_int(chip_param_table[chip[0]].res_ExternalPalette_name,
							&res_val);
	} else {
		res_val = 1;
		checkbox->SetEnabled(0);
	}
	checkbox->SetValue(res_val);
	
	resources_get_string(chip_param_table[chip[0]].res_PaletteFile_name,	&palettefile_const);
        palettefile=lib_stralloc(palettefile_const);
	util_add_extension(&palettefile, "vpl");
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

	Show();
	lib_free(palettefile);
}

VideoWindow::~VideoWindow() 
{
	videowindow = NULL;	
}

void VideoWindow::MessageReceived(BMessage *msg) {
	int32 index, val;	
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
			resources_get_int(chip_param_table[chip[0]].res_ExternalPalette_name,
								(int *)&val);
			msr = new BMessage(MESSAGE_SET_RESOURCE);
			msr->AddString("resname", chip_param_table[chip[0]].res_ExternalPalette_name);
			msr->AddInt32("resval", 1-val);
			ui_add_event((void*)msr);
			break;
		case MESSAGE_VIDEO_PALETTEFILE:
			item = palettelistview->ItemAt(
				palettelistview->CurrentSelection());
			if (item)
			{
				msr = new BMessage(MESSAGE_SET_RESOURCE);
				msr->AddString("resname", chip_param_table[chip[0]].res_PaletteFile_name);
				msr->AddString("resvalstr", ((BStringItem*) item)->Text());
				ui_add_event((void*)msr);
			}
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void ui_video() {
	
	if (videowindow != NULL)
		return;

	videowindow = new VideoWindow();

}

