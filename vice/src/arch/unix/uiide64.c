/*
 * uiide64.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "lib.h"
#include "resources.h"
#include "uiide64.h"
#include "uimenu.h"
#include "vsync.h"


UI_CALLBACK(set_ide64_image_name)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    char *title;
    ui_button_t button;
    char *value;
    char *new_value;
    int len;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("IDE64 image name"));

    resources_get_value(resname, (void *)&value);

    if (value == NULL)
        value = "";

    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = lib_malloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Name:"), new_value, len);
    lib_free(title);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    lib_free(new_value);
}

UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize)

static UI_CALLBACK(set_cylinders)
{
    static char input_string[32];

    if (CHECK_MENUS) {
        int autosize;

        resources_get_value("IDE64AutodetectSize", (void *)&autosize);

        if (autosize)
            ui_menu_set_sensitive(w, False);
        else
            ui_menu_set_sensitive(w, True);
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int cylinders;

        vsync_suspend_speed_eval();

        resources_get_value("IDE64Cylinders", (void *)&cylinders);

        sprintf(input_string, "%d", cylinders);

        msg_string = lib_stralloc(_("Enter number of cylinders"));
        button = ui_input_string(_("IDE64 cylinders"), msg_string, input_string,
                                 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (cylinders > 0 && cylinders <= 1024 && cylinders != i) {
                resources_set_value("IDE64Cylinders", (resource_value_t)i);
                ui_update_menus();
            }
        }
    }
}

static UI_CALLBACK(set_heads)
{
    static char input_string[32];

    if (CHECK_MENUS) {
        int autosize;

        resources_get_value("IDE64AutodetectSize", (void *)&autosize);

        if (autosize)
            ui_menu_set_sensitive(w, False);
        else
            ui_menu_set_sensitive(w, True);
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int heads;

        vsync_suspend_speed_eval();

        resources_get_value("IDE64Heads", (void *)&heads);

        sprintf(input_string, "%d", heads);

        msg_string = lib_stralloc(_("Enter number of heads"));
        button = ui_input_string(_("IDE64 heads"), msg_string, input_string,
                                 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (heads > 0 && heads <= 16 && heads != i) {
                resources_set_value("IDE64Heads", (resource_value_t)i);
                ui_update_menus();
            }
        }
    }
}

static UI_CALLBACK(set_sectors)
{
    static char input_string[32];

    if (CHECK_MENUS) {
        int autosize;

        resources_get_value("IDE64AutodetectSize", (void *)&autosize);

        if (autosize)
            ui_menu_set_sensitive(w, False);
        else
            ui_menu_set_sensitive(w, True);
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int sectors;

        vsync_suspend_speed_eval();

        resources_get_value("IDE64Sectors", (void *)&sectors);

        sprintf(input_string, "%d", sectors);

        msg_string = lib_stralloc(_("Enter number of sectors"));
        button = ui_input_string(_("IDE64 sectors"), msg_string, input_string,
                                 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (sectors >= 0 && sectors <= 63 && sectors != i) {
                resources_set_value("IDE64Sectors", (resource_value_t)i);
                ui_update_menus();
            }
        }
    }
}

ui_menu_entry_t ide64_submenu[] = {
    { N_("IDE64 image name..."),
      (ui_callback_t)set_ide64_image_name,
      (ui_callback_data_t)"IDE64Image", NULL },
    { N_("*Autodetect image size"),
      (ui_callback_t)toggle_IDE64AutodetectSize, NULL, NULL },
    { N_("*Cylinders..."),
      (ui_callback_t)set_cylinders, NULL, NULL },
    { N_("*Heads..."),
      (ui_callback_t)set_heads, NULL, NULL },
    { N_("*Sectors..."),
      (ui_callback_t)set_sectors, NULL, NULL },
    { NULL }
};

