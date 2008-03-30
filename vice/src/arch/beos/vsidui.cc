/*
 * vsidui.cc - Implementation of the VSID UI.
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

#include "vice.h"

#include <Box.h>
#include <TextControl.h>
#include "vicewindow.h"

extern "C" {
#include "c64ui.h"
#include "log.h"
#include "machine.h"

extern ViceWindow *windowlist[];  /* arghh */
static BWindow *window;
static BTextControl *tc_name;
static BTextControl *tc_author;
static BTextControl *tc_copyright;
static BTextControl *tc_sync;
static BTextControl *tc_default_tune;
static BTextControl *tc_current_tune;
static BTextControl *tc_last_tune;
static BTextControl *tc_time;
static BTextControl *tc_irqtype;
static ViceWindow	*vsidwindow;

int vsid_ui_init(void)
{
	BRect frame;
	BView *view;
	BBox *box;
	
	vsidwindow = new ViceWindow(BRect(0, 0, 384, 272),
						"C64 in SID player mode");
	vsidwindow->MoveTo(30, 30);

	window = windowlist[0];
	frame = window->Bounds();
	view = new BView(frame, "vsid", B_FOLLOW_NONE, B_WILL_DRAW);
	view->SetViewColor(160,160,160,0);
	frame.InsetBy(10, 40);
	frame.OffsetBy(0, -10);
	box = new BBox(frame);
	box->SetLabel("Current SID file information");
	view->AddChild(box);
	
	tc_name = new BTextControl(
		BRect(10,20,240,30), NULL, "Name of SID:", "", NULL);
	tc_name->SetDivider(70);
	box->AddChild(tc_name);

	tc_author = new BTextControl(
		BRect(10,40,240,50), NULL, "Author:", "", NULL);
	tc_author->SetDivider(70);
	box->AddChild(tc_author);
	
	tc_copyright = new BTextControl(
		BRect(10,60,240,70), NULL, "Copyright:", "", NULL);
	tc_copyright->SetDivider(70);
	box->AddChild(tc_copyright);
	
	tc_current_tune = new BTextControl(
		BRect(10,80,105,90), NULL, "Playing tune", "", NULL);
	tc_current_tune->SetDivider(70);
	tc_current_tune->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_current_tune);
	
	tc_last_tune = new BTextControl(
		BRect(110,80,150,90), NULL, "of", "", NULL);
	tc_last_tune->SetDivider(15);
	tc_last_tune->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_last_tune);
	
	tc_default_tune = new BTextControl(
		BRect(170,80,240,90), NULL, "Default:", "", NULL);
	tc_default_tune->SetDivider(45);
	tc_default_tune->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_default_tune);
	
	tc_sync = new BTextControl(
		BRect(10,100,120,110), NULL, "Sync/IRQ:", "", NULL);
	tc_sync->SetDivider(70);
	tc_sync->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_sync);
	
	tc_time = new BTextControl(
		BRect(170,100,240,110), NULL, "Time:", "", NULL);
	tc_time->SetDivider(30);
	tc_time->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_time);
	
	tc_irqtype = new BTextControl(
		BRect(125,100,165,130), NULL, "", "", NULL);
	tc_irqtype->SetDivider(0);
	tc_irqtype->SetAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER);
	box->AddChild(tc_irqtype);
	
	window->AddChild(view);
	
/*
	c64_ui_init();
*/
    return 0;
}

void vsid_ui_display_name(const char *name)
{
	if (window->Lock()) {
		tc_name->SetText(name);
		window->Unlock();
	}
}	

void vsid_ui_display_author(const char *author)
{
	if (window->Lock()) {
		tc_author->SetText(author);
		window->Unlock();
	}
}

void vsid_ui_display_copyright(const char *copyright)
{
	if (window->Lock()) {
		tc_copyright->SetText(copyright);
		window->Unlock();
	}
}

void vsid_ui_display_sync(int sync)
{
	if (window->Lock()) {
		tc_sync->SetText(sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
		window->Unlock();
	}
}

void vsid_ui_display_sid_model(int model)
{
    /* FIXME */
    log_message(LOG_DEFAULT, "Using %s emulation",
		model == 0 ? "MOS6581" : "MOS8580");
}

void vsid_ui_display_irqtype(const char *irq)
{
	if (window->Lock()) {
		tc_irqtype->SetText(irq);
		window->Unlock();
	}
}

void vsid_ui_set_default_tune(int nr)
{
	char s[16];

	sprintf(s, "%d", nr);
	if (window->Lock()) {
		tc_default_tune->SetText(s);
		window->Unlock();
	}
}

void vsid_ui_display_tune_nr(int nr)
{
	char s[16];

	sprintf(s, "%d", nr);
	if (window->Lock()) {
		tc_current_tune->SetText(s);
		window->Unlock();
	}
}

void vsid_ui_display_nr_of_tunes(int count)
{
	char s[16];

	sprintf(s, "%d", count);
	if (window->Lock()) {
		tc_last_tune->SetText(s);
		window->Unlock();
	}
}

void vsid_ui_display_time(unsigned int sec)
{
	char s[16];

	sprintf(s, "%02d:%02d", sec/60, sec%60);
	if (window->Lock()) {
		tc_time->SetText(s);
		window->Unlock();
	}
}

void vsid_ui_close(void)
{
	delete vsidwindow;
}


} /* extern "C" */
