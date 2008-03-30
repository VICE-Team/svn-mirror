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


void create_menus(void);

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(PSIDTune)

static ui_menu_entry_t ui_tune_menu[] = {
  { "Tunes",
    NULL, NULL, NULL },
  { NULL }
};

static UI_CALLBACK(psid_load)
{
  char *filename;
  ui_button_t button;

  filename = ui_select_file("Load PSID file", NULL, False, NULL,
			    "*.[psPS]*", &button);

  switch (button) {
  case UI_BUTTON_OK:
    if (psid_load_file(filename) < 0)
      ui_error("`%s' is not a valid PSID file", filename);
    else {
      char buf[1000];
      sprintf(buf, "load %s\n", filename);
      ui_proc_write_msg(buf);
      psid_set_tune(0);
      create_menus();
    }
    break;
  default:
    /* Do nothing special.  */
    break;
  }
}

static UI_CALLBACK(psid_tune)
{
  char buf[10];
  sprintf(buf, "tune %d\n", (int)client_data);
  ui_proc_write_msg(buf);
}


static ui_menu_entry_t ui_load_commands_menu[] = {
  { "Load PSID file...",
    (ui_callback_t)psid_load, NULL, NULL,
    XK_l, UI_HOTMOD_META },
  { NULL }
};


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(reset)
{
  ui_proc_write_msg("reset\n");
}

static UI_CALLBACK(powerup_reset)
{
  ui_proc_write_msg("powerup\n");
}

static UI_CALLBACK(toggle_pause)
{
  static int is_paused = 0;

  if (call_data == NULL) {
    char buf[10];

    is_paused = !is_paused;
    sprintf(buf, "pause %d\n", is_paused);
    ui_proc_write_msg(buf);
  }

  ui_menu_set_tick(w, is_paused);
}

static ui_menu_entry_t reset_submenu[] = {
  { "Soft",
    (ui_callback_t) reset, NULL, NULL },
  { "Hard",
    (ui_callback_t) powerup_reset, NULL, NULL,
    XK_F12, UI_HOTMOD_META },
  { NULL }
};

static ui_menu_entry_t ui_run_commands_menu[] = {
  { "Reset",
    NULL, NULL, reset_submenu },
  { "*Pause",
    (ui_callback_t) toggle_pause, NULL, NULL },
  { NULL }
};


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(do_exit)
{
  ui_exit();
}

static ui_menu_entry_t ui_exit_commands_menu[] = {
  { "Exit emulator",
    (ui_callback_t) do_exit, NULL, NULL },
  { NULL }
};


/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SoundSampleRate)
UI_MENU_DEFINE_RADIO(SoundBufferSize)
UI_MENU_DEFINE_RADIO(SoundOversample)

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
  { "*3.00 sec", (ui_callback_t) radio_SoundBufferSize,
    (ui_callback_data_t) 3000, NULL },
  { "*1.00 sec", (ui_callback_t) radio_SoundBufferSize,
    (ui_callback_data_t) 1000, NULL },
  { "*0.50 sec", (ui_callback_t) radio_SoundBufferSize,
    (ui_callback_data_t) 500, NULL },
  { "*0.10 sec", (ui_callback_t) radio_SoundBufferSize,
    (ui_callback_data_t) 100, NULL },
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

static ui_menu_entry_t sound_settings_submenu[] = {
  { "*Sample rate",
    NULL, NULL, set_sound_sample_rate_submenu },
  { "*Buffer size",
    NULL, NULL, set_sound_buffer_size_submenu },
  { "*Oversample",
    NULL, NULL, set_sound_oversample_submenu },
  { NULL },
};

static ui_menu_entry_t ui_sound_settings_menu[] = {
  { "Sound settings",
    NULL, NULL, sound_settings_submenu },
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


static ui_menu_entry_t psid_menu[] = {
  { "SID settings",
    NULL, NULL, sid_submenu },
  { NULL }
};


/* ------------------------------------------------------------------------- */

extern int num_checkmark_menu_items;

static void create_menus(void)
{
  static ui_menu_entry_t tune_menu[256]; 
  static Widget wl = NULL, wr = NULL;
  static int tunes = 0;
  int default_tune;
  int i;
  char buf[20] = "*Default Tune";

  /* Free previously allocated memory. */
  for (i = 0; i <= tunes; i++) {
    free(tune_menu[i].string);
  }

  /* Get number of tunes in current PSID. */
  tunes = psid_tunes(&default_tune);

  /* Build tune menu. */
  for (i = 0; i <= tunes; i++) {
    tune_menu[i].string =
      (ui_callback_data_t) stralloc(buf);
    tune_menu[i].callback =
      (ui_callback_t) radio_PSIDTune;
    tune_menu[i].callback_data =
      (ui_callback_data_t) i;
    tune_menu[i].sub_menu = NULL;
    tune_menu[i].hotkey_keysym = i < 10 ? XK_0 + i : 0;
    tune_menu[i].hotkey_modifier =
      (ui_hotkey_modifier_t) i < 10 ? UI_HOTMOD_META : 0;
    sprintf(buf, "*Tune %d", i + 1);
  }

  tune_menu[i].string =
    (ui_callback_data_t) NULL;

  ui_tune_menu[0].sub_menu = tune_menu;

  num_checkmark_menu_items = 0;

  if (wl) {
    XtDestroyWidget(wl);
  }
  if (wr) {
    XtDestroyWidget(wr);
  }

  ui_set_left_menu(wl = ui_menu_create("LeftMenu",
				       ui_load_commands_menu,
				       ui_tune_menu,
				       ui_menu_separator,
				       ui_help_commands_menu,
				       ui_menu_separator,
				       ui_run_commands_menu,
				       ui_menu_separator,
				       ui_exit_commands_menu,
				       NULL));

  ui_set_right_menu(wr = ui_menu_create("RightMenu",
					ui_sound_settings_menu,
					ui_menu_separator,
					psid_menu,
					NULL));

  ui_update_menus();
}

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

  create_menus();
    
  if (ui_proc_create() < 0) {
    return -1;
  }

  return 0;
}


int vsid_ui_exit(void)
{
  ui_proc_wait();
  return 0;
}
