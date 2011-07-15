/*
 * menu_plus4cart.c - Implementation of the plus4 cartridge settings menu for the SDL UI.
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
#include <string.h>

#include "lib.h"
#include "menu_common.h"
#include "menu_plus4cart.h"
#include "plus4cart.h"
#include "resources.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"

enum {
    CART_ATTACH_FUNCLO,
    CART_ATTACH_FUNCHI,
    CART_ATTACH_C1LO,
    CART_ATTACH_C1HI,
    CART_ATTACH_C2LO,
    CART_ATTACH_C2HI
};

static UI_MENU_CALLBACK(attach_cart_callback)
{
    char *name = NULL;

    if (activated) {
        switch (vice_ptr_to_int(param)) {
            case CART_ATTACH_FUNCLO:
                name = sdl_ui_file_selection_dialog("Select function low image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("FunctionLowName", name);
                    if (plus4cart_load_func_lo(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
            case CART_ATTACH_FUNCHI:
                name = sdl_ui_file_selection_dialog("Select function high image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("FunctionHighName", name);
                    if (plus4cart_load_func_hi(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
            case CART_ATTACH_C1LO:
                name = sdl_ui_file_selection_dialog("Select C1 low image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("c1loName", name);
                    if (plus4cart_load_c1lo(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
            case CART_ATTACH_C1HI:
                name = sdl_ui_file_selection_dialog("Select C1 high image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("c1hiName", name);
                    if (plus4cart_load_c1hi(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
            case CART_ATTACH_C2LO:
                name = sdl_ui_file_selection_dialog("Select C2 low image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("c2loName", name);
                    if (plus4cart_load_c2lo(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
            case CART_ATTACH_C2HI:
            default:
                name = sdl_ui_file_selection_dialog("Select C2 high image", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    resources_set_string("c2hiName", name);
                    if (plus4cart_load_c2hi(name) < 0) {
                        ui_error("Cannot load cartridge image.");
                    }
                    lib_free(name);
                }
                break;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(detach_cart_callback)
{
    if (activated) {
        plus4cart_detach_cartridges();
    }
    return NULL;
}

const ui_menu_entry_t plus4cart_menu[] = {
    { "Attach C1 low image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_C1LO },
    { "Attach C1 high image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_C1HI },
    { "Attach C2 low image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_C2LO },
    { "Attach C2 high image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_C2HI },
    { "Attach function low image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_FUNCLO },
    { "Attach function high image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CART_ATTACH_FUNCHI },
    SDL_MENU_ITEM_SEPARATOR,
    { "Detach cartridge image",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      NULL },
    SDL_MENU_LIST_END
};
