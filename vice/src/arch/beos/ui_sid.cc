/*
 * ui_sid.cc - SID settings
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
#include <OptionPopUp.h>
#include <RadioButton.h>
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

static struct _sid_engine_model {
    char *name;
    int id;
} sid_engine_model[] = {
    { "6581 (Fast SID)", SID_FASTSID_6581 },
    { "8580 (Fast SID)", SID_FASTSID_8580 },
#ifdef HAVE_RESID
    { "6581 (ReSID)", SID_RESID_6581 },
    { "8580 (ReSID)", SID_RESID_8580 },
    { "8580 + digiboost (ReSID)", SID_RESID_8580D },
#endif
#ifdef HAVE_RESID_FP
    { "6581R3 4885 (ReSID-fp)", SID_RESIDFP_6581R3_4885 },
    { "6581R3 0486S (ReSID-fp)", SID_RESIDFP_6581R3_0486S },
    { "6581R3 3984 (ReSID-fp)", SID_RESIDFP_6581R3_3984 },
    { "6581R4AR 3789 (ReSID-fp)", SID_RESIDFP_6581R4AR_3789 },
    { "6581R3 4485 (ReSID-fp)", SID_RESIDFP_6581R3_4485 },
    { "6581R4 1986S (ReSID-fp)", SID_RESIDFP_6581R4_1986S },
    { "8580R5 3691 (ReSID-fp)", SID_RESIDFP_8580R5_3691 },
    { "8580R5 3691 + digiboost (ReSID-fp)", SID_RESIDFP_8580R5_3691D },
    { "8580R5 1489 (ReSID-fp)", SID_RESIDFP_8580R5_1489 },
    { "8580R5 1489 + digiboost (ReSID-fp)", SID_RESIDFP_8580R5_1489D, },
#endif
    { NULL, -1 }
};

static int *sidaddressbase;

class SidWindow : public BWindow {
        BOptionPopUp *engine_model_popup;
        BOptionPopUp *address_popup;
        BSlider *passbandslider;
        BBox *residbox;

        void CreateAddressList();
        void EnableReSidControls(int engine);
    public:
        SidWindow();
        ~SidWindow();
        virtual void MessageReceived(BMessage *msg);
};

static SidWindow *sidwindow = NULL;

void SidWindow::CreateAddressList()
{
    char st[12];
    int adr, ladr, hi;
    int *hadr = sidaddressbase;

    for (hi = 0; hadr[hi] >= 0; hi++) {
        for (ladr = (hi == 0 ? 0x20 : 0x0); ladr < 0x100; ladr += 0x20) {
            adr = hadr[hi] * 0x100 + ladr;
            sprintf(st, "$%04X", adr);

            address_popup->AddOption(st, adr);
        }
    }
}

void SidWindow::EnableReSidControls(int engine)
{
    int32 children, i;

    children = residbox->CountChildren();
    for (i = 0; i < children; i++) {
        ((BControl *)residbox->ChildAt(i))->SetEnabled((engine == SID_ENGINE_RESID) || (engine == SID_ENGINE_RESID_FP));
    }

}

SidWindow::SidWindow() 
    : BWindow(BRect(250, 50, 500, 240), "Sid settings", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
    BMessage *msg;
    BCheckBox *checkbox;
    BRect r;
    BBox *box;
    BRadioButton *radiobutton;
    BView *background;
    int engine, res_val, i;

    r = Bounds();
    background = new BView(r, NULL,  B_FOLLOW_NONE, B_WILL_DRAW);
    background->SetViewColor(220, 220, 220, 0);
    AddChild(background);

    /* SID model */
    resources_get_int("SidModel", &i);
    resources_get_int("SidEngine", &engine);
    res_val = engine << 8;
    res_val |= i;
    r.bottom = 35;
    r.InsetBy(10, 5);
    engine_model_popup = new BOptionPopUp(r, "SID Engine/Model", "SID Engine/Model", new BMessage(MESSAGE_SID_MODEL));
    for (i = 0; sid_engine_model[i].name != NULL; i++) {
        engine_model_popup->AddOption(sid_engine_model[i].name, sid_engine_model[i].id);
    }
    engine_model_popup->SelectOptionFor(res_val);
    background->AddChild(engine_model_popup);

    /* SID filter */
    resources_get_int("SidFilters", &res_val);
    checkbox = new BCheckBox(BRect(10, 35, 120, 50), "SID Filters", "SID Filters", new BMessage(MESSAGE_SID_FILTERS));
    checkbox->SetValue(res_val);
    background->AddChild(checkbox);

    /* SID address */
    resources_get_int("SidStereoAddressStart", &res_val);
    address_popup = new BOptionPopUp(BRect(120, 50, 240, 75), "", "", new BMessage(MESSAGE_SID_ADDRESS));
    CreateAddressList();
    address_popup->SelectOptionFor(res_val);
    background->AddChild(address_popup);

    /* Stereo SID */
    resources_get_int("SidStereo", &res_val);
    checkbox = new BCheckBox(BRect(10, 55, 120, 70), "Stereo SID at", "Stereo SID at", new BMessage(MESSAGE_SID_STEREO));
    checkbox->SetValue(res_val);
    background->AddChild(checkbox);
    address_popup->SetEnabled(res_val);

    /* reSID settings */
    residbox = new BBox(BRect(10, 80, 240, 180), "reSID/reSID-fp settings");
    residbox->SetViewColor(220, 220, 220, 0);
    residbox->SetLabel("reSID/reSID-fp settings");
    background->AddChild(residbox);
    r = residbox->Bounds();

    /* sampling method */
    resources_get_int("SidResidSampling", &res_val);
    for (i = 0; samplingmode[i] != NULL; i++) {
        msg = new BMessage(MESSAGE_SID_RESIDSAMPLING);
        msg->AddInt32("mode", i);
        radiobutton = new BRadioButton(BRect(10, 15 + i * 20, r.Width() / 2 - 10, 30 + i * 20), samplingmode[i], samplingmode[i], msg);
        radiobutton->SetValue(res_val == i);
        residbox->AddChild(radiobutton);
    }

    resources_get_int("SidResidPassband", &res_val);
    passbandslider = new BSlider(BRect(r.Width() / 2 + 10, 20, r.Width() - 10, 60), "Passband", "Passband", new BMessage(MESSAGE_SID_RESIDPASSBAND), 0, 90, B_TRIANGLE_THUMB);
    passbandslider->SetValue(res_val);
    passbandslider->SetHashMarkCount(10);
    passbandslider->SetHashMarks(B_HASH_MARKS_BOTTOM);
    passbandslider->SetLimitLabels("0", "90");
    residbox->AddChild(passbandslider);

    EnableReSidControls(engine);

    Show();
}

SidWindow::~SidWindow() 
{
    sidwindow = NULL;
}

void SidWindow::MessageReceived(BMessage *msg)
{
    int32 engine, val;

    switch (msg->what) {
        case MESSAGE_SID_MODEL:
            /* engine_model_popup->SelectedOption((char **)&dummy, &val); */
            val = engine_model_popup->Value();
            engine = val >> 8;
            val &= 0xff;
            sid_set_engine_model(engine, val);
            EnableReSidControls(engine);
            break;
        case MESSAGE_SID_FILTERS:
            resources_toggle("SidFilters", (int *)&val);
            break;
        case MESSAGE_SID_STEREO:
            resources_toggle("SidStereo", (int *)&val);
            /* address_popup->ScrollToSelection(); */
            address_popup->SetEnabled(val);
            break;
        case MESSAGE_SID_ADDRESS:
            resources_set_int("SidStereoAddressStart", address_popup->Value());
            break;
        case MESSAGE_SID_RESIDSAMPLING:
            val = msg->FindInt32("mode");
            resources_set_int("SidResidSampling", val);
            break;
        case MESSAGE_SID_RESIDPASSBAND:
            resources_set_int("SidResidPassband", passbandslider->Value());
            break;
        default:
            BWindow::MessageReceived(msg);
    }
}

void ui_sid(int *stereoaddressbase) {
    thread_id sidthread;
    status_t exit_value;

    if (sidwindow != NULL) {
        return;
    }

    sidaddressbase = stereoaddressbase;

    sidwindow = new SidWindow;

    vsync_suspend_speed_eval();

    /* wait until window closed */
    sidthread = sidwindow->Thread();
    wait_for_thread(sidthread, &exit_value);
}
