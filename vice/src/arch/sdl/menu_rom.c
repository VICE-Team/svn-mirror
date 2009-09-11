/*
 * menu_rom.c - ROM menu for SDL UI.
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

#include "menu_common.h"
#include "menu_drive_rom.h"
#include "menu_rom.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE(InternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(ExternalFunctionROM)
UI_MENU_DEFINE_FILE_STRING(InternalFunctionName)
UI_MENU_DEFINE_FILE_STRING(ExternalFunctionName)

static const ui_menu_entry_t c128_function_rom_menu[] = {
    {"Enable internal function ROM",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_InternalFunctionROM_callback,
     NULL},
    {"Internal function ROM file",
     MENU_ENTRY_DIALOG,
     file_string_InternalFunctionName_callback,
     (ui_callback_data_t)"Select internal function ROM image"},
    {"Enable external function ROM",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_ExternalFunctionROM_callback,
     NULL},
    {"External function ROM file",
     MENU_ENTRY_DIALOG,
     file_string_ExternalFunctionName_callback,
     (ui_callback_data_t)"Select external function ROM image"},
    {NULL}
};

UI_MENU_DEFINE_FILE_STRING(KernalIntName)
UI_MENU_DEFINE_FILE_STRING(KernalDEName)
UI_MENU_DEFINE_FILE_STRING(KernalFIName)
UI_MENU_DEFINE_FILE_STRING(KernalFRName)
UI_MENU_DEFINE_FILE_STRING(KernalITName)
UI_MENU_DEFINE_FILE_STRING(KernalNOName)
UI_MENU_DEFINE_FILE_STRING(KernalSEName)
UI_MENU_DEFINE_FILE_STRING(BasicLoName)
UI_MENU_DEFINE_FILE_STRING(BasicHiName)
UI_MENU_DEFINE_FILE_STRING(ChargenIntName)
UI_MENU_DEFINE_FILE_STRING(ChargenDEName)
UI_MENU_DEFINE_FILE_STRING(ChargenFRName)
UI_MENU_DEFINE_FILE_STRING(ChargenSEName)
UI_MENU_DEFINE_FILE_STRING(Kernal64Name)
UI_MENU_DEFINE_FILE_STRING(Basic64Name)

UI_MENU_DEFINE_FILE_STRING(KernalName)
UI_MENU_DEFINE_FILE_STRING(BasicName)
UI_MENU_DEFINE_FILE_STRING(ChargenName)

const ui_menu_entry_t c128_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)c128_drive_rom_menu},
    {"Function ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)c128_function_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"International kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalIntName_callback,
     (ui_callback_data_t)"Select international kernal ROM image"},
    {"German kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalDEName_callback,
     (ui_callback_data_t)"Select german kernal ROM image"},
    {"Finnish kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalFIName_callback,
     (ui_callback_data_t)"Select finnish kernal ROM image"},
    {"French kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalFRName_callback,
     (ui_callback_data_t)"Select french kernal ROM image"},
    {"Italian kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalITName_callback,
     (ui_callback_data_t)"Select italian kernal ROM image"},
    {"Norwegian kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalNOName_callback,
     (ui_callback_data_t)"Select norwegian kernal ROM image"},
    {"Swedish kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalSEName_callback,
     (ui_callback_data_t)"Select swedish kernal ROM image"},
    {"Basic low",
     MENU_ENTRY_DIALOG,
     file_string_BasicLoName_callback,
     (ui_callback_data_t)"Select basic low ROM image"},
    {"Basic high",
     MENU_ENTRY_DIALOG,
     file_string_BasicHiName_callback,
     (ui_callback_data_t)"Select basic high ROM image"},
    {"International chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenIntName_callback,
     (ui_callback_data_t)"Select international chargen ROM image"},
    {"German chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenDEName_callback,
     (ui_callback_data_t)"Select german chargen ROM image"},
    {"French chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenFRName_callback,
     (ui_callback_data_t)"Select french chargen ROM image"},
    {"Swedish chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenSEName_callback,
     (ui_callback_data_t)"Select swedish chargen ROM image"},
    {"C64 mode kernal",
     MENU_ENTRY_DIALOG,
     file_string_Kernal64Name_callback,
     (ui_callback_data_t)"Select C64 mode kernal ROM image"},
    {"C64 mode basic",
     MENU_ENTRY_DIALOG,
     file_string_Basic64Name_callback,
     (ui_callback_data_t)"Select C64 mode basic ROM image"},
    {NULL}
};

const ui_menu_entry_t c64_vic20_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)iec_ieee_drive_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"Kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalName_callback,
     (ui_callback_data_t)"Select kernal ROM image"},
    {"Basic",
     MENU_ENTRY_DIALOG,
     file_string_BasicName_callback,
     (ui_callback_data_t)"Select basic ROM image"},
    {"Chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenName_callback,
     (ui_callback_data_t)"Select chargen ROM image"},
    {NULL}
};

const ui_menu_entry_t c64dtv_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)iec_drive_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"Kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalName_callback,
     (ui_callback_data_t)"Select kernal ROM image"},
    {"Basic",
     MENU_ENTRY_DIALOG,
     file_string_BasicName_callback,
     (ui_callback_data_t)"Select basic ROM image"},
    {"Chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenName_callback,
     (ui_callback_data_t)"Select chargen ROM image"},
    {NULL}
};

const ui_menu_entry_t cbm2_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)ieee_drive_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"Kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalName_callback,
     (ui_callback_data_t)"Select kernal ROM image"},
    {"Basic",
     MENU_ENTRY_DIALOG,
     file_string_BasicName_callback,
     (ui_callback_data_t)"Select basic ROM image"},
    {"Chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenName_callback,
     (ui_callback_data_t)"Select chargen ROM image"},
    {NULL}
};

UI_MENU_DEFINE_FILE_STRING(EditorName)

const ui_menu_entry_t pet_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)ieee_drive_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"Kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalName_callback,
     (ui_callback_data_t)"Select kernal ROM image"},
    {"Basic",
     MENU_ENTRY_DIALOG,
     file_string_BasicName_callback,
     (ui_callback_data_t)"Select basic ROM image"},
    {"Chargen",
     MENU_ENTRY_DIALOG,
     file_string_ChargenName_callback,
     (ui_callback_data_t)"Select chargen ROM image"},
    {"Editor",
     MENU_ENTRY_DIALOG,
     file_string_EditorName_callback,
     (ui_callback_data_t)"Select editor ROM image"},
    {NULL}
};

UI_MENU_DEFINE_FILE_STRING(3plus1loName)
UI_MENU_DEFINE_FILE_STRING(3plus1hiName)

const ui_menu_entry_t plus4_rom_menu[] = {
    {"Drive ROMs",
     MENU_ENTRY_SUBMENU,
     submenu_callback,
     (ui_callback_data_t)plus4_drive_rom_menu},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Computer ROMs"),
    {"Kernal",
     MENU_ENTRY_DIALOG,
     file_string_KernalName_callback,
     (ui_callback_data_t)"Select kernal ROM image"},
    {"Basic",
     MENU_ENTRY_DIALOG,
     file_string_BasicName_callback,
     (ui_callback_data_t)"Select basic ROM image"},
    {"3+1 low",
     MENU_ENTRY_DIALOG,
     file_string_3plus1loName_callback,
     (ui_callback_data_t)"Select 3+1 low ROM image"},
    {"3+1 high",
     MENU_ENTRY_DIALOG,
     file_string_3plus1hiName_callback,
     (ui_callback_data_t)"Select 3+1 high ROM image"},
    {NULL}
};
