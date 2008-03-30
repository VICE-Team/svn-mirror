/*
 * vsidui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *  Dag Lem (resid@nimrod.no)
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

#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "machine.h"
#include "psid.h"
#include "resources.h"
#include "uicommands.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "vsync.h"
#include "c64mem.h"

#ifdef XPM
#include <X11/xpm.h>
#include "c64icon.xpm"
#endif


/* ------------------------------------------------------------------------- */

/* Sound support. */

UI_MENU_DEFINE_TOGGLE(Sound)
UI_MENU_DEFINE_RADIO(SoundSpeedAdjustment)
UI_MENU_DEFINE_RADIO(SoundSampleRate)
UI_MENU_DEFINE_RADIO(SoundBufferSize)
UI_MENU_DEFINE_RADIO(SoundSuspendTime)
UI_MENU_DEFINE_RADIO(SoundOversample)

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t set_sound_sample_rate_submenu[] = {
    { "*8000Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 8000, NULL },
    { "*11025Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 11025, NULL },
    { "*22050Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 22050, NULL },
    { "*44100Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 44100, NULL },
    { "*48000Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 48000, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_buffer_size_submenu[] = {
    { "*1.00 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 1000, NULL },
    { "*0.75 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 750, NULL },
    { "*0.50 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 500, NULL },
    { "*0.35 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 350, NULL },
    { "*0.30 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 300, NULL },
    { "*0.25 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 250, NULL },
    { "*0.20 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 200, NULL },
    { "*0.15 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 150, NULL },
    { "*0.10 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 100, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_suspend_time_submenu[] = {
    { "*Keep going", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 0, NULL },
    { "*1 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 1, NULL },
    { "*2 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 2, NULL },
    { "*5 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 5, NULL },
    { "*10 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 10, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_oversample_submenu [] = {
    { "*1x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 0, NULL },
    { "*2x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 1, NULL },
    { "*4x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 2, NULL },
    { "*8x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 3, NULL },
    { NULL }
};

static ui_menu_entry_t psid_sound_settings_submenu[] = {
    { "*Enable sound playback",
      (ui_callback_t) toggle_Sound, NULL, NULL },
    { "--" },
    { "*Sample rate",
      NULL, NULL, set_sound_sample_rate_submenu },
    { "*Buffer size",
      NULL, NULL, set_sound_buffer_size_submenu },
    { "*Suspend time",
      NULL, NULL, set_sound_suspend_time_submenu },
    { "*Oversample",
      NULL, NULL, set_sound_oversample_submenu },
    { NULL },
};

ui_menu_entry_t ui_psid_sound_settings_menu[] = {
    { "Sound settings",
      NULL, NULL, psid_sound_settings_submenu },
    { NULL }
};

static UI_CALLBACK(load_psid)
{
    char *filename;
    ui_button_t button;

    filename = ui_select_file("Load PSID file", NULL, False, NULL,
                              "*.[psPS]*", &button);

    switch (button) {
      case UI_BUTTON_OK:
 	if (psid_load_file(filename) < 0)
	    ui_error("Invalid PSID File");
	else
	    ui_thread_op = PSID_LOAD;
	break;
      default:
	/* Do nothing special.  */
        break;
    }
}

ui_menu_entry_t ui_load_psid_commands_menu[] = {
    { "Load PSID file...",
      (ui_callback_t)load_psid, NULL, NULL,
      XK_l, UI_HOTMOD_META },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidModel)

static ui_menu_entry_t sid_model_submenu[] = {
    { "*6581 (old)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 0, NULL },
    { "*8580 (new)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 1, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidFilters)
#ifdef HAVE_RESID
UI_MENU_DEFINE_TOGGLE(SidUseResid)
#endif

static ui_menu_entry_t sid_submenu[] = {
    { "*Emulate filters",
      (ui_callback_t) toggle_SidFilters, NULL, NULL },
    { "Chip model",
      NULL, NULL, sid_model_submenu },
#ifdef HAVE_RESID
    { "--" },
    { "*Use reSID emulation",
      (ui_callback_t) toggle_SidUseResid, NULL, NULL },
#endif
    { NULL },
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_psid_menu[] = {
    { "SID settings",
      NULL, NULL, sid_submenu },
    { NULL }
};


int vsid_ui_init(void)
{

#ifdef XPM
    {
        Pixmap icon_pixmap;

        /* Create the icon pixmap. */
        XpmCreatePixmapFromData(display, DefaultRootWindow(display),
                                (char **) icon_data, &icon_pixmap, NULL, NULL);
        ui_set_application_icon(icon_pixmap);
    }
#endif

    ui_set_left_menu(ui_menu_create("LeftMenu",
				    ui_load_psid_commands_menu,
				    ui_menu_separator,
				    ui_run_commands_menu,
				    ui_menu_separator,
				    ui_exit_commands_menu,
				    NULL));
    
    ui_set_right_menu(ui_menu_create("RightMenu",
				     ui_psid_sound_settings_menu,
				     ui_menu_separator,
				     c64_psid_menu,
				     NULL));

    ui_update_menus();


    if (ui_create_thread() < 0) {
        return -1;
    }

    return 0;
}


int vsid_ui_exit(void)
{
    ui_join_thread();
    return 0;
}
