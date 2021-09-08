/*
 * menu_userport.c - Userport menu for SDL UI.
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

#include "types.h"

#include "lib.h"
#include "menu_common.h"
#include "uimenu.h"
#include "userport.h"

#include "menu_userport.h"

UI_MENU_DEFINE_RADIO(UserportDevice)

static ui_menu_entry_t userport_dyn_menu[USERPORT_MAX_DEVICES + 1];

static int userport_dyn_menu_init = 0;

static void sdl_menu_userport_free(void)
{
    int i;

    for (i = 0; userport_dyn_menu[i].string != NULL; i++) {
        lib_free(userport_dyn_menu[i].string);
    }
}

static UI_MENU_CALLBACK(UserportDevice_dynmenu_callback)
{
    userport_desc_t *devices = userport_get_valid_devices(1);
    int i;

    /* rebuild menu if it already exists. */
    if (userport_dyn_menu_init != 0) {
        sdl_menu_userport_free();
    } else {
        userport_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        userport_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        userport_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        userport_dyn_menu[i].callback = radio_UserportDevice_callback;
        userport_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    userport_dyn_menu[i].string = NULL;
    userport_dyn_menu[i].type = 0;
    userport_dyn_menu[i].callback = NULL;
    userport_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

ui_menu_entry_t userport_menu[4];

UI_MENU_DEFINE_TOGGLE(UserportRTCDS1307Save)
UI_MENU_DEFINE_TOGGLE(UserportRTC58321aSave)

void uiuserport_menu_create(int rtc)
{
    int j = 0;

    if (rtc) {
        userport_menu[j].string = "Save DS1307 RTC data when changed";
        userport_menu[j].type = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_UserportRTCDS1307Save_callback;
        userport_menu[j].data = NULL;
        j++;

        userport_menu[j].string = "Save 58321a RTC data when changed";
        userport_menu[j].type = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_UserportRTC58321aSave_callback;
        userport_menu[j].data = NULL;
        j++;
    }

    userport_menu[j].string = "Userport devices";
    userport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
    userport_menu[j].callback = UserportDevice_dynmenu_callback;
    userport_menu[j].data = (ui_callback_data_t)userport_dyn_menu;
    j++;

    userport_menu[j].string = NULL;
    userport_menu[j].type = MENU_ENTRY_TEXT;
    userport_menu[j].callback = NULL;
    userport_menu[j].data = NULL;
}


/** \brief  Clean up memory used by the dynamically created joyport menus
 */
void uiuserport_menu_shutdown(void)
{
    if (userport_dyn_menu_init) {
        sdl_menu_userport_free();
    }
}
