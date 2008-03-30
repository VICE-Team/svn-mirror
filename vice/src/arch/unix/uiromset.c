/*
 * uiromset.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <string.h>

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uimenu.h"
#include "uiromset.h"
#include "util.h"
#include "vsync.h"


UI_CALLBACK(ui_set_romset)
{
    machine_romset_load(UI_MENU_CB_PARAM);
    ui_update_menus();
}

UI_CALLBACK(ui_load_romset)
{
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("Load custom ROM set definition"));
    filename = ui_select_file(title, NULL, 0, False, last_dir, "*.vrs",
                              &button, False, NULL);

    lib_free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (machine_romset_load(filename) < 0)
            ui_error(_("Could not load ROM set file\n'%s'"), filename);
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
    if (filename != NULL)
        lib_free(filename);
}

UI_CALLBACK(ui_dump_romset)
{
    char *title, *new_value;
    ui_button_t button;
    int len = 512;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("File to dump ROM set definition to"));

    new_value = lib_malloc(len + 1);
    strcpy(new_value, "");

    button = ui_input_string(title, _("ROM set file:"), new_value, len);
    lib_free(title);

    if (button == UI_BUTTON_OK)
        machine_romset_save(new_value);

    lib_free(new_value);
}

UI_CALLBACK(ui_load_rom_file)
{
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("Load ROM file"));
    filename = ui_select_file(title, NULL, 0, False, last_dir, "*", &button,
                              False, NULL);

    lib_free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (resources_set_value(UI_MENU_CB_PARAM,
            (resource_value_t)filename) < 0)
            ui_error(_("Could not load ROM file\n'%s'"), filename);
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
    if (filename != NULL)
        lib_free(filename);
}

UI_CALLBACK(ui_unload_rom_file)
{
    resources_set_value((char*)UI_MENU_CB_PARAM, (resource_value_t)NULL);
}

UI_CALLBACK(ui_list_romset)
{
    char *list;

    list = machine_romset_file_list("\n");

    ui_show_text(_("Current ROM set"), list, -1, -1);

    lib_free(list);
}

