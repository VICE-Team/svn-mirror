/*
 * vsidui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
 * based on c64ui.c written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#define VSIDUI 1

#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "c64mem.h"
#include "c64ui.h"
#include "icon.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "psid.h"
#include "resources.h"
#include "uimenu.h"
#include "uisound.h"
#include "videoarch.h"
#include "vsync.h"
#include "vsidui.h"
#include "vsiduiunix.h"
#include "gp2xui.h"


extern struct ui_menu_entry_s sid_submenu[];


static log_t vsid_log = LOG_ERR;
static void vsid_create_menus(void);

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(PSIDTune)

static ui_menu_entry_t ui_tune_menu[] = {
  { N_("Tunes"),
    NULL, NULL, NULL },
  { NULL }
};


/* ------------------------------------------------------------------------- */

extern int num_checkmark_menu_items;

static void vsid_create_menus(void)
{
    static ui_menu_entry_t tune_menu[256];
    static int tunes = 0;
    int default_tune;
    int i;
    char *buf;

    buf = lib_stralloc(_("*Default Tune"));

    /* Free previously allocated memory. */
    for (i = 0; i <= tunes; i++) {
        lib_free(tune_menu[i].string);
    }

    /* Get number of tunes in current PSID. */
    tunes = psid_tunes(&default_tune);

    /* Build tune menu. */
    for (i = 0; i <= tunes; i++) {
        tune_menu[i].string =
            (ui_callback_data_t)lib_stralloc(buf);
        tune_menu[i].callback =
            (ui_callback_t) radio_PSIDTune;
        tune_menu[i].callback_data =
            (ui_callback_data_t) i;
        tune_menu[i].sub_menu = NULL;
        tune_menu[i].hotkey_keysym = (i < 10) ? XK_0 + i : 0;
        tune_menu[i].hotkey_modifier =
            (ui_hotkey_modifier_t) (i < 10) ? UI_HOTMOD_META : 0;
        lib_free(buf);
        buf = lib_msprintf(_("*Tune %d"), i + 1);
    }

    lib_free(buf);

    tune_menu[i].string =
        (ui_callback_data_t) NULL;

    ui_tune_menu[0].sub_menu = tune_menu;

    num_checkmark_menu_items = 0;
}

int vsid_ui_init(void)
{
    video_canvas_t canvas;

    video_add_handlers(&canvas);

    vsid_create_menus();

    return 0;
}

void vsid_ui_display_name(const char *name)
{
    log_message(LOG_DEFAULT, "Name: %s", name);
}

void vsid_ui_display_author(const char *author)
{
    log_message(LOG_DEFAULT, "Author: %s", author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    log_message(LOG_DEFAULT, "Copyright: %s", copyright);
}

void vsid_ui_display_sync(int sync)
{
    char buf[50];
    sprintf(buf, "Using %s sync",
	    sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
    log_message(LOG_DEFAULT, buf);
}

void vsid_ui_display_sid_model(int model)
{
    log_message(LOG_DEFAULT, "Using %s emulation",
		model == 0 ? "MOS6581" : "MOS8580");
}

void vsid_ui_set_default_tune(int nr)
{
    log_message(LOG_DEFAULT, "Default tune: %i", nr);
}

void vsid_ui_display_tune_nr(int nr)
{
    log_message(LOG_DEFAULT, "Playing tune: %i", nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
    log_message(LOG_DEFAULT, "Number of tunes: %i", count);
}

void vsid_ui_display_time(unsigned int sec)
{
  /* needed */
}

void vsid_ui_display_irqtype(const char *irq)
{
    log_message(LOG_DEFAULT, "Using %s interrupt", irq);
}

void vsid_ui_close(void)
{
  /* needed */
}

void vsid_ui_setdrv(char* driver_info_text)
{
}
