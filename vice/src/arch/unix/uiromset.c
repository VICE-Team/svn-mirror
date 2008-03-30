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
#include "romset.h"
#include "uimenu.h"
#include "uiromset.h"
#include "util.h"
#include "vsync.h"


UI_CALLBACK(ui_set_romset)
{
    machine_romset_file_load(UI_MENU_CB_PARAM);
    ui_update_menus();
}

UI_CALLBACK(ui_load_rom_file)
{
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Load ROM file"),
                              NULL, 0, False, last_dir, "*", &button,
                              False, NULL);

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

UI_MENU_DEFINE_RADIO(RomsetSourceFile)

ui_menu_entry_t uiromset_type_submenu[] = {
    { "*Archive", (ui_callback_t)radio_RomsetSourceFile,
      (ui_callback_data_t)0, NULL },
    { "*File", (ui_callback_t)radio_RomsetSourceFile,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static UI_CALLBACK(uiromset_archive_load)
{
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Load custom ROM set archive"),
                              NULL, 0, False, last_dir, "*.vra",
                              &button, False, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (romset_archive_load(filename, 0) < 0)
            ui_error(_("Could not load ROM set archive\n'%s'"), filename);
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

static UI_CALLBACK(uiromset_archive_save)
{
    char *new_value;
    ui_button_t button;
    int len = 512;

    vsync_suspend_speed_eval();

    new_value = lib_malloc(len + 1);
    strcpy(new_value, "");

    button = ui_input_string(_("File to dump ROM set archive to"),
                             _("ROM set archive:"), new_value, len);

    if (button == UI_BUTTON_OK)
        romset_archive_save(new_value);

    lib_free(new_value);
}

static UI_CALLBACK(uiromset_archive_list)
{
}

static UI_CALLBACK(uiromset_archive_item_create)
{
    char *new_value;
    ui_button_t button;
    int len = 512;

    vsync_suspend_speed_eval();

    new_value = lib_malloc(len + 1);
    strcpy(new_value, "");

    button = ui_input_string(_("ROM set item name to create"),
                             _("ROM set name:"), new_value, len);

    if (button == UI_BUTTON_OK)
        machine_romset_archive_item_create(new_value);

    lib_free(new_value);
}

ui_menu_entry_t uiromset_archive_submenu[] = {
    { N_("Load custom ROM set from file"),
      (ui_callback_t)uiromset_archive_load, NULL, NULL },
    { N_("Dump ROM set definition to file"),
      (ui_callback_t)uiromset_archive_save, NULL, NULL },
    { N_("List current ROM set"),
      (ui_callback_t)uiromset_archive_list, NULL, NULL },
    { "--" },
    { N_("Create ROM set item"),
      (ui_callback_t)uiromset_archive_item_create, NULL, NULL },
    { NULL }
};

static UI_CALLBACK(uiromset_file_load)
{
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Load custom ROM set file"),
                              NULL, 0, False, last_dir, "*.vrs",
                              &button, False, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (machine_romset_file_load(filename) < 0)
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

static UI_CALLBACK(uiromset_file_save)
{
    char *new_value;
    ui_button_t button;
    int len = 512;

    vsync_suspend_speed_eval();

    new_value = lib_malloc(len + 1);
    strcpy(new_value, "");

    button = ui_input_string(_("File to dump ROM set definition to"),
                             _("ROM set file:"), new_value, len);

    if (button == UI_BUTTON_OK)
        machine_romset_file_save(new_value);

    lib_free(new_value);
}

static UI_CALLBACK(uiromset_file_list)
{
    char *list;

    list = machine_romset_file_list("\n");

    ui_show_text(_("Current ROM set"), list, -1, -1);

    lib_free(list);
}

ui_menu_entry_t uiromset_file_submenu[] = {
    { N_("Load custom ROM set from file"),
      (ui_callback_t)uiromset_file_load, NULL, NULL },
    { N_("Dump ROM set definition to file"),
      (ui_callback_t)uiromset_file_save, NULL, NULL },
    { N_("List current ROM set"),
      (ui_callback_t)uiromset_file_list, NULL, NULL },
    { NULL }
};

