/*
 * menu_vic20cart.c - Implementation of the vic20 cartridge settings menu for the SDL UI.
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

#include "cartridge.h"
#include "keyboard.h"
#include "lib.h"
#include "menu_common.h"
#include "menu_vic20cart.h"
#include "resources.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"

static UI_MENU_CALLBACK(attach_cart_callback)
{
    char *title;
    char *name = NULL;

    if (activated) {
        switch (vice_ptr_to_int(param)) {
            case CARTRIDGE_VIC20_FP:
                title = "Select Vic Flash Plugin image";
                break;
            case CARTRIDGE_VIC20_MEGACART:
                title = "Select Mega-Cart image";
                break;
            case CARTRIDGE_VIC20_FINAL_EXPANSION:
                title = "Select Final Expansion image";
                break;
            case CARTRIDGE_VIC20_DETECT:
            case CARTRIDGE_VIC20_GENERIC:
                title = "Select cartridge image";
                break;
            case CARTRIDGE_VIC20_16KB_2000:
            case CARTRIDGE_VIC20_16KB_4000:
            case CARTRIDGE_VIC20_16KB_6000:
                title = "Select 4/8/16kB image";
                break;
            case CARTRIDGE_VIC20_8KB_A000:
                title = "Select 4/8kB image";
                break;
            case CARTRIDGE_VIC20_4KB_B000:
            default:
                title = "Select 4kB image";
                break;
        }
        name = sdl_ui_file_selection_dialog(title, FILEREQ_MODE_CHOOSE_FILE);
        if (name != NULL) {
            if (cartridge_attach_image(vice_ptr_to_int(param), name) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            lib_free(name);
        }
    }
    return NULL;
}

static const ui_menu_entry_t add_to_generic_cart_submenu[] = {
    { "Smart-attach cartridge image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_DETECT },
    { "Attach 4/8/16kB image at $2000",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_2000 },
    { "Attach 4/8/16kB image at $4000",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_4000 },
    { "Attach 4/8/16kB image at $6000",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_6000 },
    { "Attach 4/8kB image at $A000",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_8KB_A000 },
    { "Attach 4kB image at $B000",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_4KB_B000 },
    { NULL }
};

static UI_MENU_CALLBACK(detach_cart_callback)
{
    if (activated) {
        cartridge_detach_image(-1);
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_cart_default_callback)
{
    if (activated) {
        cartridge_set_default();
    }
    return NULL;
}

UI_MENU_DEFINE_TOGGLE(CartridgeReset)
UI_MENU_DEFINE_TOGGLE(FinalExpansionWriteBack)
UI_MENU_DEFINE_TOGGLE(VicFlashPluginWriteBack)
UI_MENU_DEFINE_TOGGLE(MegaCartNvRAMWriteBack)
UI_MENU_DEFINE_FILE_STRING(MegaCartNvRAMfilename)

const ui_menu_entry_t vic20cart_menu[] = {
    { "Attach generic cartridge image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_GENERIC },
    { "Attach Mega-Cart image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_MEGACART },
    { "Attach Final Expansion image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_FINAL_EXPANSION },
    { "Attach Vic Flash Plugin image",
      MENU_ENTRY_DIALOG,
      attach_cart_callback,
      (ui_callback_data_t)CARTRIDGE_VIC20_FP },
    SDL_MENU_ITEM_SEPARATOR,
    { "Add to generic cartridge",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)add_to_generic_cart_submenu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Detach cartridge image",
      MENU_ENTRY_OTHER,
      detach_cart_callback,
      NULL },
    { "Set current cartridge as default",
      MENU_ENTRY_OTHER,
      set_cart_default_callback,
      NULL },
    { "Reset on cartridge change",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CartridgeReset_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Final Expansion write back",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_FinalExpansionWriteBack_callback,
      NULL },
    { "Vic Flash Plugin write back",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VicFlashPluginWriteBack_callback,
      NULL },
    { "Mega-Cart NvRAM write back",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MegaCartNvRAMWriteBack_callback,
      NULL },
    { "Mega-Cart NvRAM file",
      MENU_ENTRY_DIALOG,
      file_string_MegaCartNvRAMfilename_callback,
      (ui_callback_data_t)"Select Mega-Cart NvRAM image" },
    { NULL }
};
