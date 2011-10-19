/*
 * ui_video.cc - Video settings
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *  Marcus Sutton <loggedoubt@gmail.com>
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

typedef struct control_res_s {
    char *name;
    char *res_name;
    int multiplier;
} control_res_t;

static control_res_t color_controls[] = {
    { "Gamma", "ColorGamma", 4000 },
    { "Tint", "ColorTint", 2000 },
    { "Saturation", "ColorSaturation", 2000 },
    { "Contrast", "ColorContrast", 2000 },
    { "Brightness", "ColorBrightness", 2000 },
    { NULL, NULL, 0 }
};

static control_res_t crt_controls[] = {
    { "Scanline Shade", "PALScanLineShade", 1000 },
    { "Blur", "PALBlur", 1000 },
    { "Odd Lines Phase", "PALOddLinePhase", 2000 },
    { "Odd Lines Offset", "PALOddLineOffset", 2000 },
    { NULL, NULL, 0 }
};

static char *chip_name_table[] = { "VICII", "VIC", "CRTC", "VDC", "TED" };

static int chip[] = { -1, -1 };

class VideoWindow : public BWindow {
        BBox *color_ctrlsbox;
        BBox *crt_ctrlsbox;
        BListView *palettelistview;
        int chip_no;
        char *chip_name;

        void CreateSliders(BBox *parent, control_res_t *ctrls);
        //~ void EnableSliders(BBox *parent);
    public:
        VideoWindow(int chipno);
        ~VideoWindow();
        virtual void MessageReceived(BMessage *msg);
};

static VideoWindow *videowindow[] = { NULL, NULL };

void VideoWindow::CreateSliders(BBox *parent, control_res_t *ctrls)
{
    int res_val;
    char *resname;
    BMessage *msg;
    BSlider *slider;
    BRect r;

    r = parent->Bounds();
    r.bottom = 50;
    r.InsetBy(5, 15);
    for (int i = 0; ctrls[i].name; i++) {
        resname = util_concat(chip_name, ctrls[i].res_name, NULL);
        if (resources_get_int(resname, &res_val) == 0) {
            slider = new BSlider(r.OffsetByCopy(0, i * 45), resname, ctrls[i].name, NULL, 0, ctrls[i].multiplier, B_TRIANGLE_THUMB);
            msg = new BMessage(MESSAGE_VIDEO_COLOR);
            msg->AddString("resname", resname);
            msg->AddPointer("slider", slider);
            slider->SetMessage(msg);
            slider->SetValue(res_val);
            slider->SetHashMarkCount(11);
            slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
            parent->AddChild(slider);
        }
        lib_free(resname);
    }
}

VideoWindow::VideoWindow(int chipno) 
    : BWindow(BRect(250, 50, 640, 345), "Video settings", B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
    BMessage *msg;
    BCheckBox *checkbox;
    BRect r;
    BView *background;
    BDirectory dir;
    BEntry entry;
    int res_val;
    char *resname;
    char *dirpath;
    const char *palettefile_const;
    char *palettefile;
    //~ BRadioButton *rb_mode;

    switch (machine_class) {
        case VICE_MACHINE_C128:
            chip[1] = 3;
            /* fall through */
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_C64DTV:
        case VICE_MACHINE_CBM5x0:
        default:
            chip[0] = 0;
            break;
        case VICE_MACHINE_CBM6x0:
        case VICE_MACHINE_PET:
            chip[0] = 2;
            break;
        case VICE_MACHINE_PLUS4:
            chip[0] = 4;
            break;
        case VICE_MACHINE_VIC20:
            chip[0] = 1;
            break;
    }

    chip_no = chipno;
    chip_name = chip_name_table[chip[chipno]];

    if (machine_class == VICE_MACHINE_C128) {
        if (chipno == 0) {
            SetTitle("Video settings (VIC-II)");
        } else {
            SetTitle("Video settings (VDC)");
        }
    }


    r = Bounds();
    background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
    background->SetViewColor(220, 220, 220, 0);
    AddChild(background);

    /* Sliders for color control */
    color_ctrlsbox = new BBox(BRect(10, 10, 120, 255), "Color controls");
    color_ctrlsbox->SetLabel("Color controls");
    CreateSliders(color_ctrlsbox, color_controls);
    background->AddChild(color_ctrlsbox);
    //~ color_ctrlsbox->SetViewColor(216, 216, 216, 0);

    /* Sliders for CRT Emulation control */
    crt_ctrlsbox = new BBox(BRect(130, 10, 240, 210), "CRT emulation");
    crt_ctrlsbox->SetLabel("CRT emulation");
    CreateSliders(crt_ctrlsbox, crt_controls);
    background->AddChild(crt_ctrlsbox);

    /* External Palette check box */
    resname = util_concat(chip_name, "ExternalPalette", NULL);
    msg = new BMessage(MESSAGE_VIDEO_EXTERNALPALETTE);
    msg->AddString("resname", resname);
    checkbox = new BCheckBox(BRect(250, 10, 380, 25), "ExternalPalette", "External Palette", msg);
    background->AddChild(checkbox);
    resources_get_int(resname, &res_val);
    checkbox->SetValue(res_val);
    lib_free(resname);

    /* External Palette File list box */
    resname = util_concat(chip_name, "PaletteFile", NULL);
    msg = new BMessage(MESSAGE_VIDEO_PALETTEFILE);
    msg->AddString("resname", resname);
    palettelistview = new BListView(BRect(250, 35, 360, 125), "PaletteFile");
    palettelistview->SetSelectionMessage(msg);
    background->AddChild(new BScrollView("scroll", palettelistview, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));

    /* Fill External Palette File list box */
    resources_get_string(resname, &palettefile_const);
    palettefile = lib_stralloc(palettefile_const);
    util_add_extension(&palettefile, "vpl");
    dirpath = util_concat(archdep_boot_path(), "/", machine_name, NULL);
    dir = BDirectory(dirpath);
    while (dir.GetNextEntry(&entry) != B_ENTRY_NOT_FOUND) {
        char s[255];
        BListItem *item;

        entry.GetName(s);
        if (strstr(s, ".vpl")) {
            palettelistview->AddItem(item = new BStringItem(s));
            if (strncmp(s, palettefile, strlen(s)) == 0) {
                palettelistview->Select(palettelistview->IndexOf(item));
            }
        }
    }
    lib_free(dirpath);
    lib_free(palettefile);
    lib_free(resname);

    Show();
}

VideoWindow::~VideoWindow() 
{
    videowindow[chip_no] = NULL;
}

void VideoWindow::MessageReceived(BMessage *msg)
{
    const char *resname;
    int32 val;
    BMessage *msr;
    BSlider *slider;
    BListItem *item;

    msg->FindString("resname", &resname);

    switch (msg->what) {
        case MESSAGE_VIDEO_COLOR:
            msg->FindPointer("slider", (void **)&slider);
            val = slider->Value();
            msr = new BMessage(MESSAGE_SET_RESOURCE);
            msr->AddString("resname", resname);
            msr->AddInt32("resval", val);
            ui_add_event((void*)msr);
            delete msr;
            break;
        case MESSAGE_VIDEO_EXTERNALPALETTE:
            resources_get_int(resname, (int *)&val);
            msr = new BMessage(MESSAGE_SET_RESOURCE);
            msr->AddString("resname", resname);
            msr->AddInt32("resval", 1 - val);
            ui_add_event((void*)msr);
            delete msr;
            break;
        case MESSAGE_VIDEO_PALETTEFILE:
            item = palettelistview->ItemAt(palettelistview->CurrentSelection());
            if (item) {
                msr = new BMessage(MESSAGE_SET_RESOURCE);
                msr->AddString("resname", resname);
                msr->AddString("resvalstr", ((BStringItem*) item)->Text());
                ui_add_event((void*)msr);
                delete msr;
            }
            break;
        default:
            BWindow::MessageReceived(msg);
    }
}

void ui_video(int chip_no)
{
    if (videowindow[chip_no] != NULL) {
        videowindow[chip_no]->Activate();
        return;
    }

    videowindow[chip_no] = new VideoWindow(chip_no);
}
