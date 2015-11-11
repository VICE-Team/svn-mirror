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
#include "lib.h"
#include "mouse.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uijoyport.h"
#include "uijoystick.h"

TUI_MENU_DEFINE_TOGGLE(Mouse)
TUI_MENU_DEFINE_TOGGLE(SmartMouseRTCSave)
TUI_MENU_DEFINE_RADIO(JoyPort1Device)
TUI_MENU_DEFINE_RADIO(JoyPort2Device)
TUI_MENU_DEFINE_RADIO(JoyPort3Device)

static TUI_MENU_CALLBACK(joyport1_submenu_callback)
{
    int value;
    char *s;
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_1);

    resources_get_int("JoyPort1Device", &value);
    s = devices[value].name;
    lib_free(devices);
    return s;
}

static TUI_MENU_CALLBACK(joyport2_submenu_callback)
{
    int value;
    char *s;
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_2);

    resources_get_int("JoyPort2Device", &value);
    s = devices[value].name;
    lib_free(devices);
    return s;
}

static TUI_MENU_CALLBACK(joyport3_submenu_callback)
{
    int value;
    char *s;
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_3);

    resources_get_int("JoyPort3Device", &value);
    s = devices[value].name;
    lib_free(devices);
    return s;
}

static tui_menu_item_def_t joyport1_submenu[JOYPORT_MAX_DEVICES];
static tui_menu_item_def_t joyport2_submenu[JOYPORT_MAX_DEVICES];
static tui_menu_item_def_t joyport3_submenu[JOYPORT_MAX_DEVICES];

static tui_menu_item_def_t joyport1_menu_items[] = {
    { "Control port device:", "Select the device for the control port",
      joyport1_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport1_submenu,
      "Control port device" },
    { "Save Smart Mouse RTC data when changed",
      "Save Smart Mouse RTC data when changed",
      toggle_SmartMouseRTCSave_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Grab mouse events:",
      "Emulate a mouse",
      toggle_Mouse_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t joyport2_menu_items[] = {
    { "Control port _1 device:", "Select the device for control port 1",
      joyport1_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport1_submenu,
      "Control port 1 device" },
    { "Control port _2 device:", "Select the device for control port 2",
      joyport2_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport2_submenu,
      "Control port 2 device" },
    { "Save Smart Mouse RTC data when changed",
      "Save Smart Mouse RTC data when changed",
      toggle_SmartMouseRTCSave_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Grab mouse events:",
      "Emulate a mouse",
      toggle_Mouse_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t joyport3_menu_items[] = {
    { "Control port _1 device:", "Select the device for control port 1",
      joyport1_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport1_submenu,
      "Control port 1 device" },
    { "Control port _2 device:", "Select the device for control port 2",
      joyport2_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport2_submenu,
      "Control port 2 device" },
    { "SIDCard control port device:", "Select the device for the SIDCard control port",
      joyport3_submenu_callback, NULL, 25,
      TUI_MENU_BEH_CONTINUE, joyport3_submenu,
      "SIDCard control port device" },
    { "Save Smart Mouse RTC data when changed",
      "Save Smart Mouse RTC data when changed",
      toggle_SmartMouseRTCSave_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Grab mouse events:",
      "Emulate a mouse",
      toggle_Mouse_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uijoyport_init(struct tui_menu *parent_submenu, int ports)
{
    tui_menu_t ui_joyport_submenu;
    joyport_desc_t *devices_port_1 = joyport_get_valid_devices(JOYPORT_1);
    joyport_desc_t *devices_port_2 = joyport_get_valid_devices(JOYPORT_2);
    joyport_desc_t *devices_port_3 = joyport_get_valid_devices(JOYPORT_3);
    int i;

    ui_joyport_submenu = tui_menu_create("Control port settings", 1);

    for (i = 0; devices[i].name; ++i) {
        joyport1_submenu[i].label = devices_port_1[i].name;
        joyport1_submenu[i].help_string = NULL;
        joyport1_submenu[i].callback = radio_JoyPort1Device_callback;
        joyport1_submenu[i].callback_param = (void *)devices_port_1[i].id;
        joyport1_submenu[i].par_string_max_len = 20;
        joyport1_submenu[i].behavior = TUI_MENU_BEH_CLOSE;
        joyport1_submenu[i].submenu = NULL;
        joyport1_submenu[i].submenu_title = NULL;
        if (ports > 1) {
            joyport2_submenu[i].label = devices_port_2[i].name;
            joyport2_submenu[i].help_string = NULL;
            joyport2_submenu[i].callback = radio_JoyPort2Device_callback;
            joyport2_submenu[i].callback_param = (void *)devices_port_2[i].id;
            joyport2_submenu[i].par_string_max_len = 20;
            joyport2_submenu[i].behavior = TUI_MENU_BEH_CLOSE;
            joyport2_submenu[i].submenu = NULL;
            joyport2_submenu[i].submenu_title = NULL;
        }
        if (ports > 2) {
            joyport3_submenu[i].label = devices_port_3[i].name;
            joyport3_submenu[i].help_string = NULL;
            joyport3_submenu[i].callback = radio_JoyPort3Device_callback;
            joyport3_submenu[i].callback_param = (void *)devices_port_3[i].id;
            joyport3_submenu[i].par_string_max_len = 20;
            joyport3_submenu[i].behavior = TUI_MENU_BEH_CLOSE;
            joyport3_submenu[i].submenu = NULL;
            joyport3_submenu[i].submenu_title = NULL;
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
    if (ports > 1) {
        joyport2_submenu[i].label = NULL;
        joyport2_submenu[i].help_string = NULL;
        joyport2_submenu[i].callback = NULL;
        joyport2_submenu[i].callback_param = NULL;
        joyport2_submenu[i].par_string_max_len = 0;
        joyport2_submenu[i].behavior = 0;
        joyport2_submenu[i].submenu = NULL;
        joyport2_submenu[i].submenu_title = NULL;
    }
    if (ports > 2) {
        joyport3_submenu[i].label = NULL;
        joyport3_submenu[i].help_string = NULL;
        joyport3_submenu[i].callback = NULL;
        joyport3_submenu[i].callback_param = NULL;
        joyport3_submenu[i].par_string_max_len = 0;
        joyport3_submenu[i].behavior = 0;
        joyport3_submenu[i].submenu = NULL;
        joyport3_submenu[i].submenu_title = NULL;
    }

    switch (ports) {
        case 1:
            tui_menu_add(ui_joyport_submenu, joyport1_menu_items);
            break;
        case 2:
            tui_menu_add(ui_joyport_submenu, joyport2_menu_items);
            break;
        case 3:
            tui_menu_add(ui_joyport_submenu, joyport3_menu_items);
            break;
    }

    lib_free(devices_port_1);
    lib_free(devices_port_2);
    lib_free(devices_port_3);

    tui_menu_add_submenu(parent_submenu, "_Control port settings...",
                         "Control port settings",
                         ui_joyport_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    uijoystick_init(ui_joyport_submenu);
}
