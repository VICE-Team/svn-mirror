/*
 * menu_cbm2cart.c - Implementation of the cbm2 cartridge settings menu for the SDL UI.
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

#include "menu_cbm2cart.h"
#include "menu_common.h"
#include "resources.h"
#include "ui.h"
#include "uimenu.h"

UI_MENU_DEFINE_FILE_STRING(Cart1Name)
UI_MENU_DEFINE_FILE_STRING(Cart2Name)
UI_MENU_DEFINE_FILE_STRING(Cart4Name)
UI_MENU_DEFINE_FILE_STRING(Cart6Name)

static UI_MENU_CALLBACK(detach_cart_callback)
{
    if (activated) {
        resources_set_string((char *)param, "");
    }
    return NULL;
}

const ui_menu_entry_t cbm2cart_menu[] = {
    { "Attach Cart 1",
      MENU_ENTRY_DIALOG,
      file_string_Cart1Name_callback,
      (ui_callback_data_t)"Select Cart 1 image" },
    { "Detach Cart 1",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      (ui_callback_data_t)"Cart1Name" },
    { "Attach Cart 2",
      MENU_ENTRY_DIALOG,
      file_string_Cart2Name_callback,
      (ui_callback_data_t)"Select Cart 2 image" },
    { "Detach Cart 2",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      (ui_callback_data_t)"Cart2Name" },
    { "Attach Cart 4",
      MENU_ENTRY_DIALOG,
      file_string_Cart4Name_callback,
      (ui_callback_data_t)"Select Cart 4 image" },
    { "Detach Cart 4",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      (ui_callback_data_t)"Cart4Name" },
    { "Attach Cart 6",
      MENU_ENTRY_DIALOG,
      file_string_Cart6Name_callback,
      (ui_callback_data_t)"Select Cart 6 image" },
    { "Detach Cart 6",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      (ui_callback_data_t)"Cart6Name" },
    { NULL }
};
