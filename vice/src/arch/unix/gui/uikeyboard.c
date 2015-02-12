/*
 * uikeyboard.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#include "ioutil.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uilib.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"

static UI_CALLBACK(set_keymap_type)
{
    int kindex, newindex = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (resources_get_int("KeymapIndex", &kindex) < 0) {
        return;
    }

    if (!CHECK_MENUS) {
        if ((kindex & 1) != newindex) {
            resources_set_int("KeymapIndex", (kindex & ~1) + newindex);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, (kindex & 1) == newindex);
    }
}

static ui_menu_entry_t keyboard_maptype_submenu[] = {
    { N_("Symbolic mapping"), UI_MENU_TYPE_TICK, (ui_callback_t)set_keymap_type,
      (ui_callback_data_t)0, NULL },
    { N_("Positional mapping"), UI_MENU_TYPE_TICK, (ui_callback_t)set_keymap_type,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static UI_CALLBACK(select_user_keymap)
{
    char *filename;
    const char *resname;
    ui_button_t button;
    int kindex;
    static char *last_dir;
    uilib_file_filter_enum_t filter[] = { UILIB_FILTER_KEYMAP, UILIB_FILTER_ALL };

    resources_get_int("KeymapIndex", &kindex);
    kindex = (kindex & ~1) + vice_ptr_to_int(UI_MENU_CB_PARAM);
    resname = machine_get_keymap_res_name(kindex);

    vsync_suspend_speed_eval();
    filename = ui_select_file(_("Read Keymap File"), NULL, 0, last_dir, filter, sizeof(filter) / sizeof(*filter), &button, 0, NULL, UI_FC_LOAD);

    switch (button) {
        case UI_BUTTON_OK:
            resources_set_string(resname, filename);
            lib_free(last_dir);
            util_fname_split(filename, &last_dir, NULL);
            break;
        default:
            /* Do nothing special.  */
            break;
    }
    lib_free(filename);
}

static UI_CALLBACK(dump_keymap)
{
    char *wd;
    int len;

    len = ioutil_maxpathlen();
    wd = lib_malloc(len);

    ioutil_getcwd(wd, len);
    if (uilib_input_file(_("VICE setting"), _("Write to Keymap File:"), wd, len) == UI_BUTTON_OK) {
        if (keyboard_keymap_dump(wd) < 0) {
            ui_error(strerror(errno));
        }
    }
    lib_free(wd);
}

void ui_select_keymap(ui_window_t w, int check, char *name, int sympos)
{
    const char *resname;
    int kindex;
    const char *wd;

    resources_get_int("KeymapIndex", &kindex);
    kindex = (kindex & ~1) + sympos;
    resname = machine_get_keymap_res_name(kindex);

    if (name) {
        if (!check) {
            resources_set_string(resname, name);
            ui_update_menus();
        } else {
            resources_get_string(resname, &wd);
            if (!strcmp(wd, name)) {
                ui_menu_set_tick(w, 1);
            } else {
                ui_menu_set_tick(w, 0);
            }
        }
    }
}

UI_CALLBACK(radio_SymKeymap)
{
    ui_select_keymap(w, CHECK_MENUS, UI_MENU_CB_PARAM, 0);
}

UI_CALLBACK(radio_PosKeymap)
{
    ui_select_keymap(w, CHECK_MENUS, UI_MENU_CB_PARAM, 1);
}

/* array for the actual data to be copied into */
struct ui_menu_entry_s uikeymap_sym_submenu[4] = {
    { NULL },
    { NULL },
    { NULL },
    { NULL }
};
struct ui_menu_entry_s uikeymap_pos_submenu[4] = {
    { NULL },
    { NULL },
    { NULL },
    { NULL }
};

static ui_menu_entry_t keyboard_sym_submenu[] = {
    { "", UI_MENU_TYPE_NONE, NULL, NULL, uikeymap_sym_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Set symbolic keymap file"), UI_MENU_TYPE_DOTS, (ui_callback_t)select_user_keymap,
      (ui_callback_data_t)0, NULL},
    { NULL }
};

static ui_menu_entry_t keyboard_pos_submenu[] = {
    { "", UI_MENU_TYPE_NONE, NULL, NULL, uikeymap_pos_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Set positional keymap file"), UI_MENU_TYPE_DOTS, (ui_callback_t)select_user_keymap,
      (ui_callback_data_t)1, NULL},
    { NULL }
};

static ui_menu_entry_t keyboard_settings_submenu[] = {
    { N_("Keyboard mapping type"), UI_MENU_TYPE_NORMAL, NULL, NULL, keyboard_maptype_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Select symbolic keymap"), UI_MENU_TYPE_NORMAL, NULL, NULL, keyboard_sym_submenu},
    { N_("Select positional keymap"), UI_MENU_TYPE_NORMAL, NULL, NULL, keyboard_pos_submenu},
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Dump keymap to file"), UI_MENU_TYPE_DOTS, (ui_callback_t) dump_keymap, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uikeyboard_settings_menu[] = {
    { N_("Keyboard settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, keyboard_settings_submenu },
    { NULL }
};
