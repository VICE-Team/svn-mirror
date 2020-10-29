/*
 * menu_speed.c - Implementation of the speed settings menu for the SDL UI.
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
#include <stdlib.h>

#include "lib.h"
#include "menu_common.h"
#include "menu_speed.h"
#include "resources.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE(WarpMode)
UI_MENU_DEFINE_RADIO(Speed)


static UI_MENU_CALLBACK(custom_Speed_callback)
{
    static char buf[20];
    char *value = NULL;
    int previous, new_value;

    resources_get_int("Speed", &previous);

    if (activated) {
        sprintf(buf, "%i", previous > 0 ? previous : 0);
        value = sdl_ui_text_input_dialog("Enter custom maximum speed", buf);
        if (value > 0) {
            new_value = (int)strtol(value, NULL, 0);
            if (new_value != previous) {
                resources_set_int("Speed", new_value);
            }
            lib_free(value);
        }
    } else {
        if ((previous > 0) && (previous != 10) && (previous != 25) &&
            (previous != 50) && (previous != 100) && (previous != 200)) {
            sprintf(buf, "%i%%", previous);
            return buf;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(custom_Fps_callback)
{
    static char buf[20];
    char *value = NULL;
    int previous, new_value;

    resources_get_int("Speed", &previous);

    if (activated) {
        sprintf(buf, "%i", previous < 0 ? -previous : 0);
        value = sdl_ui_text_input_dialog("Enter target Fps", buf);
        if (value > 0) {
            new_value = -(int)strtol(value, NULL, 0);
            if (new_value != previous) {
                resources_set_int("Speed", new_value);
            }
            lib_free(value);
        }
    } else {
        if (previous < 0) {
            sprintf(buf, "%i%%", -previous);
        }
    }
    return NULL;
}

const ui_menu_entry_t speed_menu[] = {
    { "Warp mode",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_WarpMode_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Maximum CPU speed"),
    { "10%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)10 },
    { "25%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)25 },
    { "50%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)50 },
    { "100%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)100 },
    { "200%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)200 },
    { "Custom CPU speed",
      MENU_ENTRY_DIALOG,
      custom_Speed_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Target Fps"),
    { "50 Fps",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)-50 },
    { "60 Fps",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)-60 },
    { "Custom Fps",
      MENU_ENTRY_DIALOG,
      custom_Fps_callback,
      NULL },
    SDL_MENU_LIST_END
};


/* VSID specific speed menu */
const ui_menu_entry_t speed_menu_vsid[] = {
    { "Warp mode",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_WarpMode_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Maximum speed"),
    { "10%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)10 },
    { "25%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)25 },
    { "50%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)50 },
    { "100%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)100 },
    { "200%",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)200 },
    { "No limit",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_Speed_callback,
      (ui_callback_data_t)0 },
    { "Custom speed",
      MENU_ENTRY_DIALOG,
      custom_Speed_callback,
      NULL },
    SDL_MENU_LIST_END
};
