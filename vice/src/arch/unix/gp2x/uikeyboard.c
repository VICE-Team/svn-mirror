/*
 * uikeyboard.c
 *
 * Written by
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

#include "ioutil.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


static UI_CALLBACK(set_keymap_type)
{
     int kindex, newindex = (int)UI_MENU_CB_PARAM;

     if (resources_get_value("KeymapIndex", (void *)&kindex) < 0)
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
      (ui_callback_data_t)0, NULL },
    { N_("*Positional mapping (US)"), (ui_callback_t)set_keymap_type,
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

    resources_get_value("KeymapIndex", (void *)&kindex);
    kindex = (kindex & ~1) + (int)UI_MENU_CB_PARAM;
    resname = machine_keymap_res_name_list[kindex];

    vsync_suspend_speed_eval();
    filename = ui_select_file(_("Read Keymap File"), NULL, 0, False, last_dir,
                              "*.vkm", &button, False, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resname, (resource_value_t)filename);
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

static UI_CALLBACK(dump_keymap)
{
    char *wd;
    int len;

    len = ioutil_maxpathlen();
    wd = lib_malloc(len);

    ioutil_getcwd(wd, len);
    vsync_suspend_speed_eval();
    if (ui_input_string(_("VICE setting"), _("Write to Keymap File:"),
                        wd, len) == UI_BUTTON_OK) {
        if (keyboard_keymap_dump(wd) < 0)
            ui_error(strerror(errno));
    }
    lib_free(wd);
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

ui_menu_entry_t uikeyboard_settings_menu[] = {
    { N_("Keyboard settings"),
      NULL, NULL, keyboard_settings_submenu },
    { NULL }
};

