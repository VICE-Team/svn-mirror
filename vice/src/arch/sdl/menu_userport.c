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


#include "lib.h"
#include "machine.h"
#include "menu_common.h"
#include "types.h"
#include "ui.h"
#include "uiactions.h"
#include "uimenu.h"
#include "userport.h"
#include "userport_wic64.h"

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
        userport_dyn_menu[i].action   = ACTION_NONE;
        userport_dyn_menu[i].string   = lib_strdup(devices[i].name);
        userport_dyn_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        userport_dyn_menu[i].callback = radio_UserportDevice_callback;
        userport_dyn_menu[i].data     = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }
    userport_dyn_menu[i].string = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

/*
 * WiC64 menu stuff
 */
#ifdef HAVE_LIBCURL
static ui_menu_entry_t *dyn_menu_wic64;

UI_MENU_DEFINE_TOGGLE(WIC64Logenabled)
UI_MENU_DEFINE_INT(WIC64Hexdumplines);
UI_MENU_DEFINE_STRING(WIC64DefaultServer)
UI_MENU_DEFINE_TOGGLE(WIC64Resetuser)
UI_MENU_DEFINE_RADIO(WIC64Timezone);

/** \brief  Generate WiC64 runtime timezones menu
 *
 * Iterate WiC64 timezones table and create radio buttons for each zone.
 */
static void wic64_timezones_menu_new(void)
{
    const tzones_t *zones;
    size_t          num_zones;
    size_t          z;

    zones          = userport_wic64_get_timezones(&num_zones);
    dyn_menu_wic64 = lib_malloc((num_zones + 1u) * sizeof *dyn_menu_wic64);

    for (z = 0; z < num_zones; z++) {
        dyn_menu_wic64[z].action   = ACTION_NONE;
        dyn_menu_wic64[z].string   = lib_msprintf("%d: %s", zones[z].idx, zones[z].tz_name);
        dyn_menu_wic64[z].type     = MENU_ENTRY_RESOURCE_RADIO;
        dyn_menu_wic64[z].callback = radio_WIC64Timezone_callback;
        dyn_menu_wic64[z].data     = int_to_void_ptr(zones[z].idx);
    }
    dyn_menu_wic64[z].string = NULL;
}

/** \brief  Free WiC64 runtime timezones menu */
static void wic64_timezones_menu_free(void)
{
    int z;

    for (z = 0; dyn_menu_wic64[z].string != NULL; z++) {
        lib_free(dyn_menu_wic64[z].string);
    }
}

static UI_MENU_CALLBACK(custom_wic64_reset_callback)
{
    if (activated) {
        userport_wic64_factory_reset();
        ui_message("WiC64 has been reset to factory default.");
    }
    return NULL;
}

#endif

#ifdef HAVE_LIBCURL
ui_menu_entry_t userport_menu[13];
#else
ui_menu_entry_t userport_menu[4];
#endif

UI_MENU_DEFINE_TOGGLE(UserportRTCDS1307Save)
UI_MENU_DEFINE_TOGGLE(UserportRTC58321aSave)

void uiuserport_menu_create(int rtc)
{
    int j = 0;

    if (rtc) {
        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Save DS1307 RTC data when changed";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_UserportRTCDS1307Save_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Save 58321a RTC data when changed";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_UserportRTC58321aSave_callback;
        userport_menu[j].data     = NULL;
        j++;
    }

#ifdef HAVE_LIBCURL
    if (machine_class == VICE_MACHINE_C64 ||
        machine_class == VICE_MACHINE_C64SC ||
        machine_class == VICE_MACHINE_C128 ||
        machine_class == VICE_MACHINE_VIC20 ||
        machine_class == VICE_MACHINE_SCPU64) {

        if (dyn_menu_wic64 == NULL) {
            wic64_timezones_menu_new();
        }

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "";
        userport_menu[j].type     = MENU_ENTRY_TEXT;
        userport_menu[j].callback = seperator_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "WiC64 Settings";
        userport_menu[j].type     = MENU_ENTRY_TEXT;
        userport_menu[j].callback = seperator_callback;
        userport_menu[j].data     = (ui_callback_data_t)1;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "WiC64 tracing";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_WIC64Logenabled_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Hexdump lines (0=unlimited):";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_INT;
        userport_menu[j].callback = int_WIC64Hexdumplines_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Default server:";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_STRING;
        userport_menu[j].callback = string_WIC64DefaultServer_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Timezone";
        userport_menu[j].type     = MENU_ENTRY_DYNAMIC_SUBMENU;
        userport_menu[j].callback = submenu_radio_callback;
        userport_menu[j].data     = dyn_menu_wic64;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Reset User when resetting WiC64";
        userport_menu[j].type     = MENU_ENTRY_RESOURCE_TOGGLE;
        userport_menu[j].callback = toggle_WIC64Resetuser_callback;
        userport_menu[j].data     = NULL;
        j++;

        userport_menu[j].action   = ACTION_NONE;
        userport_menu[j].string   = "Reset WiC64 to factory default";
        userport_menu[j].type     = MENU_ENTRY_OTHER;
        userport_menu[j].callback = custom_wic64_reset_callback;
        userport_menu[j].data     = NULL;
        j++;
    }
#endif

    userport_menu[j].action   = ACTION_NONE;
    userport_menu[j].string   = "";
    userport_menu[j].type     = MENU_ENTRY_TEXT;
    userport_menu[j].callback = seperator_callback;
    userport_menu[j].data     = NULL;
    j++;

    userport_menu[j].action   = ACTION_NONE;
    userport_menu[j].string   = "Userport devices";
    userport_menu[j].type     = MENU_ENTRY_DYNAMIC_SUBMENU;
    userport_menu[j].callback = UserportDevice_dynmenu_callback;
    userport_menu[j].data     = (ui_callback_data_t)userport_dyn_menu;
    j++;

    userport_menu[j].string = NULL;
}


/** \brief  Clean up memory used by the dynamically created userport menus
 */
void uiuserport_menu_shutdown(void)
{
    if (userport_dyn_menu_init) {
        sdl_menu_userport_free();
    }
    if (dyn_menu_wic64 != NULL) {
        wic64_timezones_menu_free();
    }
}
