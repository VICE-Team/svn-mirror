/*
 * uijoyport.c - Implementation of joyport UI settings.
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

#include "lib.h"
#include "joyport.h"
#include "translate.h"
#include "uilib.h"
#include "uimenu.h"
#include "uijoyport.h"

UI_MENU_DEFINE_RADIO(JoyPort1Device)
UI_MENU_DEFINE_RADIO(JoyPort2Device)

ui_menu_entry_t joyport1_settings_submenu[] = {
    { N_("Joyport 1 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, NULL },
    { NULL },
};

ui_menu_entry_t joyport2_settings_submenu[] = {
    { N_("Joyport 1 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, NULL },
    { N_("Joyport 2 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, NULL },
    { NULL },
};

ui_menu_entry_t ui_joyport1_settings_menu[] = {
    { N_("Joyport settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joyport1_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_joyport2_settings_menu[] = {
    { N_("Joyport settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joyport2_settings_submenu },
    { NULL }
};

void uijoyport_menu_create(int ports)
{
    unsigned int i, num;
    ui_menu_entry_t *devices_submenu1;
    ui_menu_entry_t *devices_submenu2;
    joyport_desc_t *devices = joyport_get_valid_devices();

    for (i = 0; devices[i].name; ++i) {}
    num = i;

    if (!num) {
        return;
    }

    devices_submenu1 = lib_calloc((size_t)(num + 1), sizeof(ui_menu_entry_t));
    if (ports == 2) {
        devices_submenu2 = lib_calloc((size_t)(num + 1), sizeof(ui_menu_entry_t));
    }

    for (i = 0; i < num ; i++) {
        devices_submenu1[i].string = (ui_callback_data_t)lib_msprintf("%s", translate_text(devices[i].trans_name));
        devices_submenu1[i].type = UI_MENU_TYPE_TICK;
        devices_submenu1[i].callback = (ui_callback_t)radio_JoyPort1Device;
        devices_submenu1[i].callback_data = (ui_callback_data_t)devices[i].id);
        if (ports == 2) {
            devices_submenu2[i].string = (ui_callback_data_t)lib_msprintf("%s", translate_text(devices[i].trans_name));
            devices_submenu2[i].type = UI_MENU_TYPE_TICK;
            devices_submenu2[i].callback = (ui_callback_t)radio_JoyPort2Device;
            devices_submenu2[i].callback_data = (ui_callback_data_t)devices[i].id);
        }
    }

    if (ports == 2) {
        joyport2_settings_submenu[0].sub_menu = devices_submenu1;
        joyport2_settings_submenu[1].sub_menu = devices_submenu2;
    } else {
        joyport1_settings_submenu[0].sub_menu = devices_submenu1;
    }
}

void uisound_menu_shutdown(int ports)
{
    unsigned int i;
    ui_menu_entry_t *devices_submenu1 = NULL;
    ui_menu_entry_t *devices_submenu2 = NULL;

    if (ports == 2) {
        devices_submenu1 = joyport2_settings_submenu[0].sub_menu;
        joyport2_settings_submenu[0].sub_menu = NULL;
        devices_submenu2 = joyport2_settings_submenu[1].sub_menu;
        joyport2_settings_submenu[1].sub_menu = NULL;
    } else {
        devices_submenu1 = joyport1_settings_submenu[0].sub_menu;
        joyport2_settings_submenu[0].sub_menu = NULL;
    }

    i = 0;

    while (devices_submenu1[i].string != NULL) {
        lib_free(devices_submenu1[i].string);
        lib_free(devices_submenu1[i].callback_data);
        if (ports == 2) {
            lib_free(devices_submenu2[i].string);
            lib_free(devices_submenu2[i].callback_data);
        }
        i++;
    }

    lib_free(devices_submenu1);
    if (ports == 2) {
        lib_free(devices_submenu2);
    }
}
