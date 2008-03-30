/*
 * uisettings.c - Implementation of common UI settings.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "archapi.h"
#include "debug.h"
#include "fliplist.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif
#ifdef USE_XF86_VIDMODE_EXT
#include "x11/vidmode.h"
#endif
#include "keyboard.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "romset.h"
#include "types.h"
#include "uidrive.h"
#include "uimenu.h"
#include "uiperipheral.h"
#include "uirs232.h"
#include "uisettings.h"
#include "uisound.h"
#include "utils.h"
#include "vsync.h"


/* Big kludge to get the ticks right in the refresh rate submenu.  This only
   works if the callback for the "custom" setting is the last one to be
   called, and the "Auto" one is the first one.  */
static int have_custom_refresh_rate;

static UI_CALLBACK(set_refresh_rate)
{
    int current_refresh_rate;

    resources_get_value("RefreshRate",
                        (resource_value_t *)&current_refresh_rate);
    if (!CHECK_MENUS) {
        if (current_refresh_rate != (int)UI_MENU_CB_PARAM) {
            resources_set_value("RefreshRate",
                                (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        if ((int)UI_MENU_CB_PARAM == 0)
            have_custom_refresh_rate = 1;
        if ((int)UI_MENU_CB_PARAM == current_refresh_rate) {
            ui_menu_set_tick(w, 1);
            have_custom_refresh_rate = 0;
        } else {
            ui_menu_set_tick(w, 0);
        }
        if (UI_MENU_CB_PARAM == 0) {
            int speed;

            resources_get_value("Speed", (resource_value_t *)&speed);
            if (speed == 0) {
                /* Cannot enable the `automatic' setting if a speed limit is
                   not specified. */
                ui_menu_set_sensitive(w, False);
            } else {
                ui_menu_set_sensitive(w, True);
            }
        }
    }
}

static UI_CALLBACK(set_custom_refresh_rate)
{
    static char input_string[32];
    char *msg_string;
    ui_button_t button;
    int i;
    int current_refresh_rate;

    resources_get_value("RefreshRate",
                        (resource_value_t *)&current_refresh_rate);

    if (!*input_string)
        sprintf(input_string, "%d", current_refresh_rate);

    if (CHECK_MENUS) {
        if (have_custom_refresh_rate)
            ui_menu_set_tick(w, 1);
        else
            ui_menu_set_tick(w, 0);
        have_custom_refresh_rate = 0;
    } else {
        int current_speed;

        vsync_suspend_speed_eval();
        msg_string = stralloc(_("Enter refresh rate"));
        button = ui_input_string(_("Refresh rate"), msg_string, input_string,
                                 32);
        free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            resources_get_value("Speed", (resource_value_t *)&current_speed);
            if (!(current_speed <= 0 && i <= 0) && i >= 0
                && current_refresh_rate != i) {
                resources_set_value("RefreshRate", (resource_value_t)i);
                ui_update_menus();
            }
        }
    }
}

/* ------------------------------------------------------------------------- */

/* Big kludge to get the ticks right in the maximum speed submenu.  This only
   works if the callback for the "custom" setting is the last one to be
   called, and the "100%" one is the first one.  */
static int have_custom_maximum_speed;

static UI_CALLBACK(set_maximum_speed)
{
    int current_speed;

    resources_get_value("Speed", (resource_value_t *)&current_speed);

    if (!CHECK_MENUS) {
        if (current_speed != (int)UI_MENU_CB_PARAM) {
            resources_set_value("Speed", (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        if ((int)UI_MENU_CB_PARAM == 100)
            have_custom_maximum_speed = 1;
        if (current_speed == (int)UI_MENU_CB_PARAM) {
            ui_menu_set_tick(w, 1);
            have_custom_maximum_speed = 0;
        } else {
            ui_menu_set_tick(w, 0);
        }
        if (UI_MENU_CB_PARAM == 0) {
            int current_refresh_rate;

            resources_get_value("RefreshRate",
                                (resource_value_t *)&current_refresh_rate);
            ui_menu_set_sensitive(w, current_refresh_rate != 0);
        }
    }
}

static UI_CALLBACK(set_custom_maximum_speed)
{
    static char input_string[32];
    char *msg_string;
    ui_button_t button;
    int i;
    int current_speed;

    resources_get_value("Speed", (resource_value_t *)&current_speed);
    if (!*input_string)
        sprintf(input_string, "%d", current_speed);

    if (CHECK_MENUS) {
        if (have_custom_maximum_speed)
            ui_menu_set_tick(w, 1);
        else
            ui_menu_set_tick(w, 0);
        have_custom_maximum_speed = 0;
    } else {
        vsync_suspend_speed_eval();
        msg_string = stralloc(_("Enter speed"));
        button = ui_input_string(_("Maximum run speed"), msg_string,
                                 input_string, 32);
        free(msg_string);
        if (button == UI_BUTTON_OK) {
            int current_refresh_rate;

            resources_get_value("RefreshRate",
                                (resource_value_t *)&current_refresh_rate);
            i = atoi(input_string);
            if (!(current_refresh_rate <= 0 && i <= 0) && i >= 0
                && current_speed != i) {
                resources_set_value("Speed", (resource_value_t)i);
                ui_update_menus();
            } else {
                ui_error(_("Invalid speed value"));
            }
        }
    }
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(save_resources)
{
    vsync_suspend_speed_eval();
    if (resources_save(NULL) < 0)
        ui_error(_("Cannot save settings."));
    else {
        if (w != NULL)
            ui_message(_("Settings saved successfully."));
    }
    flip_save_list((unsigned int) -1, archdep_default_fliplist_file_name());
    ui_update_menus();
}

static UI_CALLBACK(load_resources)
{
    int r;

    vsync_suspend_speed_eval();
    r = resources_load(NULL);

    if (r < 0) {
        if (r == RESERR_FILE_INVALID)
            ui_error(_("Cannot load settings:\nresource file not valid."));
        else
            ui_error(_("Cannot load settings:\nresource file not found."));
    }
#if 0
    else if (w != NULL)
        ui_message(_("Settings loaded."));
#endif

    ui_update_menus();
}

static UI_CALLBACK(set_default_resources)
{
    vsync_suspend_speed_eval();
    resources_set_defaults();
    ui_update_menus();
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(UseXSync)
UI_MENU_DEFINE_TOGGLE(SaveResourcesOnExit)
UI_MENU_DEFINE_TOGGLE(WarpMode)

/* ------------------------------------------------------------------------- */

/* this is modelled after the toggle_* calls */

static UI_CALLBACK(set_keymap_type)
{
     int kindex, newindex = (int)UI_MENU_CB_PARAM;

     if (resources_get_value("KeymapIndex", (resource_value_t*)&kindex) < 0)
         return;

     if (!CHECK_MENUS) {
        if ((kindex & 1) != newindex) {
            resources_set_value("KeymapIndex", (resource_value_t)
                                ((kindex & ~1) + newindex));
            ui_update_menus();
        }
     } else {
        ui_menu_set_tick(w, (kindex & 1) == newindex);
     }
}

static ui_menu_entry_t keyboard_maptype_submenu[] = {
    { N_("*Symbolic mapping"), (ui_callback_t)set_keymap_type,
        (ui_callback_data_t) 0, NULL },
    { N_("*Positional mapping (US)"), (ui_callback_t)set_keymap_type,
        (ui_callback_data_t) 1, NULL },
    { NULL }
};

static UI_CALLBACK(select_user_keymap)
{
    char *filename;
    const char *resname;
    ui_button_t button;
    int kindex;
    static char *last_dir;

    resources_get_value("KeymapIndex", (resource_value_t)&kindex);
    kindex = (kindex & ~1) + (int)UI_MENU_CB_PARAM;
    resname = machine_keymap_res_name_list[kindex];

    vsync_suspend_speed_eval();
    filename = ui_select_file(_("Read Keymap File"), NULL, False, last_dir,
                              "*.vkm", &button, False, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resname, (resource_value_t)filename);
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        free(filename);
}

static UI_CALLBACK(dump_keymap)
{
    char *wd;
    wd = xmalloc(MAXPATHLEN);

    getcwd(wd, MAXPATHLEN);
    vsync_suspend_speed_eval();
    if (ui_input_string(_("VICE setting"), _("Write to Keymap File:"),
                        wd, MAXPATHLEN) == UI_BUTTON_OK) {
        if (keyboard_keymap_dump(wd) < 0)
            ui_error(strerror(errno));
    }
    free(wd);
}

static ui_menu_entry_t keyboard_settings_submenu[] = {
    { N_("Keyboard mapping type"),
      NULL, NULL, keyboard_maptype_submenu },
    { "--" },
    { N_("Set symbolic keymap file"), (ui_callback_t)select_user_keymap,
                (ui_callback_data_t)0, NULL},
    { N_("Set positional keymap file"), (ui_callback_t)select_user_keymap,
                (ui_callback_data_t)1, NULL},
    { "--" },
    { N_("Dump keymap to file"),
      (ui_callback_t) dump_keymap, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

/* ROM set stuff */

UI_CALLBACK(ui_set_romset)
{
    romset_load(UI_MENU_CB_PARAM);
    ui_update_menus();
}

UI_CALLBACK(ui_load_romset)
{
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = stralloc(_("Load custom ROM set definition"));
    filename = ui_select_file(title, NULL, False, last_dir, "*.vrs", &button,
                              False, NULL);

    free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (romset_load(filename) < 0)
            ui_error(_("Could not load ROM set file\n'%s'"), filename);
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
    if (filename != NULL)
        free(filename);
}

UI_CALLBACK(ui_dump_romset)
{
    char *title, *new_value;
    ui_button_t button;
    int len = 512;

    vsync_suspend_speed_eval();
    title = stralloc(_("File to dump ROM set definition to"));

    new_value = xmalloc(len + 1);
    strcpy(new_value, "");

    button = ui_input_string(title, _("ROM set file:"), new_value, len);
    free(title);

    if (button == UI_BUTTON_OK)
        romset_dump(new_value, mem_romset_resources_list);

    free(new_value);
}

UI_CALLBACK(ui_load_rom_file)
{
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = stralloc(_("Load ROM file"));
    filename = ui_select_file(title, NULL, False, last_dir, "*", &button,
                              False, NULL);

    free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (resources_set_value(UI_MENU_CB_PARAM,
            (resource_value_t)filename) < 0)
            ui_error(_("Could not load ROM file\n'%s'"), filename);
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
    if (filename != NULL)
        free(filename);
}

UI_CALLBACK(ui_unload_rom_file)
{
    resources_set_value((char*)UI_MENU_CB_PARAM, (resource_value_t)NULL);
}

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t set_refresh_rate_submenu[] = {
    { N_("*Auto"),
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)0, NULL },
    { "*1/1",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)1, NULL },
    { "*1/2",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)2, NULL },
    { "*1/3",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)3, NULL },
    { "*1/4",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)4, NULL },
    { "*1/5",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)5, NULL },
    { "*1/6",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)6, NULL },
    { "*1/7",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)7, NULL },
    { "*1/8",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)8, NULL },
    { "*1/9",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)9, NULL },
    { "*1/10",
      (ui_callback_t)set_refresh_rate, (ui_callback_data_t)10, NULL },
    { "--" },
    { N_("*Custom..."),
      (ui_callback_t)set_custom_refresh_rate, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t set_maximum_speed_submenu[] = {
    { "*200%",
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)200, NULL },
    { "*100%",
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)100, NULL },
    { "*50%",
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)50, NULL },
    { "*20%",
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)20, NULL },
    { "*10%",
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)10, NULL },
    { N_("*No limit"),
      (ui_callback_t)set_maximum_speed, (ui_callback_data_t)0, NULL },
    { "--" },
    { N_("*Custom..."),
      (ui_callback_t)set_custom_maximum_speed, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

#if defined (USE_XF86_DGA2_EXTENSIONS) || defined (USE_XF86_VIDMODE_EXT)

#ifdef USE_XF86_DGA2_EXTENSIONS
UI_MENU_DEFINE_TOGGLE(UseFullscreen)
#endif
#ifdef USE_XF86_VIDMODE_EXT
UI_MENU_DEFINE_TOGGLE(UseFullscreenVidMode)
#endif
#if 0
UI_MENU_DEFINE_TOGGLE(FullscreenDoubleSize)
UI_MENU_DEFINE_TOGGLE(FullscreenDoubleScan)
#endif

ui_menu_entry_t ui_fullscreen_settings_submenu[] = {
#ifdef USE_XF86_DGA2_EXTENSIONS
    { N_("*Enable DGA2 fullscreen"),
      (ui_callback_t)toggle_UseFullscreen, NULL, NULL, XK_d, UI_HOTMOD_META },
    /* Translators: 'DGA2' must remain in the beginning of the translation
       e.g. German: "DGA2 Auflösungen" */
    { N_("DGA2 Resolutions"),
      (ui_callback_t) NULL, NULL, NULL },
    { "--"},
#endif
#ifdef USE_XF86_VIDMODE_EXT
    { N_("*Enable VidMode fullscreen"),
      (ui_callback_t)toggle_UseFullscreenVidMode, NULL, NULL, XK_v, 
      UI_HOTMOD_META },
    /* Translators: 'VidMode' must remain in the beginning of the translation
       e.g. German: "VidMode Auflösungen" */
    { N_("VidMode Resolutions"),
      (ui_callback_t) NULL, NULL, NULL },
#endif
#if 0
    { N_("*Double size"),
      (ui_callback_t)toggle_FullscreenDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_FullscreenDoubleScan, NULL, NULL },
    { "--"},
#endif
    { NULL }
};

#endif

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

ui_menu_entry_t video_settings_submenu[] = {
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_video_settings_menu[] = {
    { N_("VIC-II Video settings"),
      NULL, NULL, video_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_vic_video_settings_menu[] = {
    { N_("VIC Video settings"),
      NULL, NULL, video_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_ted_video_settings_menu[] = {
    { N_("TED Video settings"),
      NULL, NULL, video_settings_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

ui_menu_entry_t ui_performance_settings_menu[] = {
    { N_("Refresh rate"),
      NULL, NULL, set_refresh_rate_submenu },
    { N_("Maximum speed"),
      NULL, NULL, set_maximum_speed_submenu },
    { N_("*Enable warp mode"),
      (ui_callback_t)toggle_WarpMode, NULL, NULL, XK_w, UI_HOTMOD_META },
    { NULL }
};

#ifdef USE_XF86_EXTENSIONS

static UI_CALLBACK(FullscreenMenu)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, fullscreen_available());
    }
}

ui_menu_entry_t ui_fullscreen_settings_menu[] = {
    { N_("*Fullscreen settings"),
      (ui_callback_t)FullscreenMenu, NULL, ui_fullscreen_settings_submenu },
    { NULL }
};
#endif


ui_menu_entry_t ui_keyboard_settings_menu[] = {
    { N_("Keyboard settings"),
      NULL, NULL, keyboard_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_settings_settings_menu[] = {
    { N_("Save settings"),
      (ui_callback_t)save_resources, NULL, NULL },
    { N_("Load settings"),
      (ui_callback_t)load_resources, NULL, NULL },
    { N_("Restore default settings"),
      (ui_callback_t)set_default_resources, NULL, NULL },
    { N_("*Save settings on exit"),
      (ui_callback_t)toggle_SaveResourcesOnExit, NULL, NULL },
    { NULL }
};

#ifdef DEBUG
UI_MENU_DEFINE_TOGGLE(MainCPU_TRACE)
UI_MENU_DEFINE_TOGGLE(Drive0CPU_TRACE)
UI_MENU_DEFINE_TOGGLE(Drive1CPU_TRACE)

ui_menu_entry_t debug_settings_submenu[] = {
    { "*Main CPU Trace",
      (ui_callback_t)toggle_MainCPU_TRACE, NULL, NULL },
    { "*Drive0 CPU Trace",
      (ui_callback_t)toggle_Drive0CPU_TRACE, NULL, NULL },
    { "*Drive1 CPU Trace",
      (ui_callback_t)toggle_Drive1CPU_TRACE, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_debug_settings_menu[] = {
    { "Debug settings",
      NULL, NULL, debug_settings_submenu },
    { NULL }
};
#endif

