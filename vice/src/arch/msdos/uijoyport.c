/*
 * uijoyport.c - Joyport UI interface for MS-DOS.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "joyport.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uijoyport.h"

TUI_MENU_DEFINE_RADIO(JoyPort1Device)
TUI_MENU_DEFINE_RADIO(JoyPort2Device)

static TUI_MENU_CALLBACK(joyport1_submenu_callback)
{
    int value;
    char *s;
    joyport_desc_t *devices = joyport_get_valid_devices();

    resources_get_int("JoyPort1Device", &value);
    s = devices[value].name;
    lib_free(devices);
    return s;
}

static TUI_MENU_CALLBACK(joyport2_submenu_callback)
{
    int value;
    char *s;
    joyport_desc_t *devices = joyport_get_valid_devices();

    resources_get_int("JoyPort2Device", &value);
    s = devices[value].name;
    lib_free(devices);
    return s;
}

static tui_menu_item_def_t joyport1_submenu[JOYPORT_MAX_DEVICES];
static tui_menu_item_def_t joyport2_submenu[JOYPORT_MAX_DEVICES];

static tui_menu_item_def_t joyport1_menu_items[] = {
    { "Joyport _1 device:", "Select the device for joyport 1",
      joyport1_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, joyport1_submenu,
      "Joyport 1 device" },
    { NULL }
};

static tui_menu_item_def_t joyport2_menu_items[] = {
    { "Joyport _1 device:", "Select the device for joyport 1",
      joyport1_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, joyport1_submenu,
      "Joyport 1 device" },
    { "Joyport _2 device:", "Select the device for joyport 2",
      joyport2_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, joyport2_submenu,
      "Joyport 2 device" },
    { NULL }
};

void uijoyport_init(struct tui_menu *parent_submenu, int ports)
{
    tui_menu_t ui_joyport_submenu;
    joyport_desc_t *devices = joyport_get_valid_devices();
    int i;

    ui_joyport_submenu = tui_menu_create("Joyport settings", 1);

    for (i = 0; devices[i].name; ++i) {
        joyport1_submenu[i].label = devices[i].name;
        joyport1_submenu[i].help_string = NULL;
        joyport1_submenu[i].callback = radio_JoyPort1Device_callback;
        joyport1_submenu[i].callback_param = (void *)devices[i].id;
        joyport1_submenu[i].par_string_max_len = 20;
        joyport1_submenu[i].behavior = TUI_MENU_BEH_CLOSE;
        joyport1_submenu[i].submenu = NULL;
        joyport1_submenu[i].submenu_title = NULL;
        if (ports == 2) {
            joyport2_submenu[i].label = devices[i].name;
            joyport2_submenu[i].help_string = NULL;
            joyport2_submenu[i].callback = radio_JoyPort2Device_callback;
            joyport2_submenu[i].callback_param = (void *)devices[i].id;
            joyport2_submenu[i].par_string_max_len = 20;
            joyport2_submenu[i].behavior = TUI_MENU_BEH_CLOSE;
            joyport2_submenu[i].submenu = NULL;
            joyport2_submenu[i].submenu_title = NULL;
        }
    }

    joyport1_submenu[i].label = NULL;
    joyport1_submenu[i].help_string = NULL;
    joyport1_submenu[i].callback = NULL;
    joyport1_submenu[i].callback_param = NULL;
    joyport1_submenu[i].par_string_max_len = 0;
    joyport1_submenu[i].behavior = 0;
    joyport1_submenu[i].submenu = NULL;
    joyport1_submenu[i].submenu_title = NULL;
    if (ports == 2) {
        joyport2_submenu[i].label = NULL;
        joyport2_submenu[i].help_string = NULL;
        joyport2_submenu[i].callback = NULL;
        joyport2_submenu[i].callback_param = NULL;
        joyport2_submenu[i].par_string_max_len = 0;
        joyport2_submenu[i].behavior = 0;
        joyport2_submenu[i].submenu = NULL;
        joyport2_submenu[i].submenu_title = NULL;
    }

    if (ports == 2) {
        tui_menu_add(ui_joyport_submenu, joyport2_menu_items);
    } else {
        tui_menu_add(ui_joyport_submenu, joyport1_menu_items);
    }

    tui_menu_add_submenu(parent_submenu, "_Joyport settings...",
                         "Joyport settings",
                         ui_joyport_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}
