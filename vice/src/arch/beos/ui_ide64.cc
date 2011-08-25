/*
 * ui_ide64.cc - IDE64 settings
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <stdlib.h>
#include <string.h>
#include <TextControl.h>
#include <Window.h>

extern "C" {
#include "resources.h"
#include "ui.h"
#include "ui_ide64.h"
#include "vsync.h"
}

class IDE64Window : public BWindow {
    public:
        IDE64Window();
        ~IDE64Window();
        virtual void MessageReceived(BMessage *msg);
    private:
        BTextControl *cylinderstextcontrol;
        BTextControl *headstextcontrol;
        BTextControl *sectorstextcontrol;
};

static IDE64Window *ide64window = NULL;

IDE64Window::IDE64Window() 
    : BWindow(BRect(50, 50, 200, 150), "IDE64 size settings", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE) 
{
    int cylinders;
    int heads;
    int sectors;
    char cyl_str[256];
    char hds_str[256];
    char sec_str[256];
    BView *background;
    BRect r;

    r = Bounds();
    background = new BView(r, "backview", B_FOLLOW_NONE, B_WILL_DRAW);
    background->SetViewColor(220, 220, 220, 0);
    AddChild(background);

    resources_get_int("IDE64Cylinders", &cylinders);
    resources_get_int("IDE64Heads", &heads);
    resources_get_int("IDE64Sectors", &sectors);
    sprintf(cyl_str, "%d", cylinders);
    sprintf(hds_str, "%d", heads);
    sprintf(sec_str, "%d", sectors);

    r.bottom -= r.Height() * 2 / 3;
    r.InsetBy(5, 5);

    cylinderstextcontrol = new BTextControl(r, "cylinders", "Cylinders", cyl_str, NULL);
    cylinderstextcontrol->SetDivider(50);
    background->AddChild(cylinderstextcontrol);

    r.OffsetBy(0, r.Height() + 5);

    headstextcontrol = new BTextControl(r, "heads", "Heads", hds_str, NULL);
    headstextcontrol->SetDivider(50);
    background->AddChild(headstextcontrol);

    r.OffsetBy(0, r.Height() + 5);

    sectorstextcontrol = new BTextControl(r, "sectors", "Sectors", sec_str, NULL);
    sectorstextcontrol->SetDivider(50);
    background->AddChild(sectorstextcontrol);

    r.OffsetBy(0, r.Height() + 5);

    background->AddChild(new BButton(r, "apply", "Apply settings", new BMessage(MESSAGE_IDE64_APPLY)));

    Show();
}

IDE64Window::~IDE64Window() 
{
    ide64window = NULL;       
}

void IDE64Window::MessageReceived(BMessage *msg)
{
    int cylinders;
    int heads;
    int sectors;

    switch (msg->what) {
        case MESSAGE_IDE64_APPLY:
            cylinders = atoi(cylinderstextcontrol->Text());
            if (cylinders < 1) {
                ui_error("Value for IDE64 cylinders was invalid, using 1 instead.");
                cylinders = 1;
            }
            if (cylinders > 1024) {
                ui_error("Value for IDE64 cylinders was invalid, using 1024 instead.");
                cylinders = 1024;
            }
            resources_set_int("IDE64Cylinders", cylinders);

            heads = atoi(headstextcontrol->Text());
            if (heads < 1) {
                ui_error("Value for IDE64 heads was invalid, using 1 instead.");
                heads = 1;
            }
            if (heads > 16) {
                ui_error("Value for IDE64 heads was invalid, using 16 instead.");
                heads = 16;
            }
            resources_set_int("IDE64Heads", heads);

            sectors = atoi(sectorstextcontrol->Text());
            if (sectors < 1) {
                ui_error("Value for IDE64 sectors was invalid, using 1 instead.");
                sectors = 1;
            }
            if (sectors > 63) {
                ui_error("Value for IDE64 sectors was invalid, using 63 instead.");
                sectors = 63;
            }
            resources_set_int("IDE64Sectors", sectors);
            BWindow::Quit();
            break;
        default:
            BWindow::MessageReceived(msg);
    }
}

void ui_ide64()
{
    thread_id ide64thread;
    status_t exit_value;

    if (ide64window != NULL) {
        return;
    }

    ide64window = new IDE64Window;

    vsync_suspend_speed_eval();

    /* wait until window closed */
    ide64thread=ide64window->Thread();
    wait_for_thread(ide64thread, &exit_value);
}

