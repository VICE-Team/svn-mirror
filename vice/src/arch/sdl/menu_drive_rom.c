/*
 * menu_drive_rom.c - Drive ROM menu for SDL UI.
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
#include "uimenu.h"

UI_MENU_DEFINE_FILE_STRING(DosName1541)
UI_MENU_DEFINE_FILE_STRING(DosName1541ii)
UI_MENU_DEFINE_FILE_STRING(DosName1570)
UI_MENU_DEFINE_FILE_STRING(DosName1571)
UI_MENU_DEFINE_FILE_STRING(DosName1581)
UI_MENU_DEFINE_FILE_STRING(DosName2031)
UI_MENU_DEFINE_FILE_STRING(DosName2040)
UI_MENU_DEFINE_FILE_STRING(DosName3040)
UI_MENU_DEFINE_FILE_STRING(DosName4040)
UI_MENU_DEFINE_FILE_STRING(DosName1001)
UI_MENU_DEFINE_FILE_STRING(DosName1571cr)
UI_MENU_DEFINE_FILE_STRING(DosName1551)

const ui_menu_entry_t c128_drive_rom_menu[] = {
    { "1541 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541_callback,
      (ui_callback_data_t)"Select 1541 ROM image" },
    { "1541-II ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541ii_callback,
      (ui_callback_data_t)"Select 1541-II ROM image" },
    { "1570 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1570_callback,
      (ui_callback_data_t)"Select 1570 ROM image" },
    { "1571 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1571_callback,
      (ui_callback_data_t)"Select 1571 ROM image" },
    { "1571CR ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1571cr_callback,
      (ui_callback_data_t)"Select 1571CR ROM image" },
    { "1581 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1581_callback,
      (ui_callback_data_t)"Select 1581 ROM image" },
    { "2031 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2031_callback,
      (ui_callback_data_t)"Select 2031 ROM image" },
    { "2040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2040_callback,
      (ui_callback_data_t)"Select 2040 ROM image" },
    { "3040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName3040_callback,
      (ui_callback_data_t)"Select 3040 ROM image" },
    { "4040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName4040_callback,
      (ui_callback_data_t)"Select 4040 ROM image" },
    { "1001 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1001_callback,
      (ui_callback_data_t)"Select 1001 ROM image" },
    { NULL }
};

const ui_menu_entry_t plus4_drive_rom_menu[] = {
    { "1541 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541_callback,
      (ui_callback_data_t)"Select 1541 ROM image" },
    { "1541-II ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541ii_callback,
      (ui_callback_data_t)"Select 1541-II ROM image" },
    { "1551 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1551_callback,
      (ui_callback_data_t)"Select 1551 ROM image" },
    { "1570 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1570_callback,
      (ui_callback_data_t)"Select 1570 ROM image" },
    { "1571 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1571_callback,
      (ui_callback_data_t)"Select 1571 ROM image" },
    { "1581 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1581_callback,
      (ui_callback_data_t)"Select 1581 ROM image" },
    { NULL }
};

const ui_menu_entry_t iec_ieee_drive_rom_menu[] = {
    { "1541 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541_callback,
      (ui_callback_data_t)"Select 1541 ROM image" },
    { "1541-II ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541ii_callback,
      (ui_callback_data_t)"Select 1541-II ROM image" },
    { "1570 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1570_callback,
      (ui_callback_data_t)"Select 1570 ROM image" },
    { "1571 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1571_callback,
      (ui_callback_data_t)"Select 1571 ROM image" },
    { "1581 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1581_callback,
      (ui_callback_data_t)"Select 1581 ROM image" },
    { "2031 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2031_callback,
      (ui_callback_data_t)"Select 2031 ROM image" },
    { "2040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2040_callback,
      (ui_callback_data_t)"Select 2040 ROM image" },
    { "3040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName3040_callback,
      (ui_callback_data_t)"Select 3040 ROM image" },
    { "4040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName4040_callback,
      (ui_callback_data_t)"Select 4040 ROM image" },
    { "1001 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1001_callback,
      (ui_callback_data_t)"Select 1001 ROM image" },
    { NULL }
};

const ui_menu_entry_t ieee_drive_rom_menu[] = {
    { "2031 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2031_callback,
      (ui_callback_data_t)"Select 2031 ROM image" },
    { "2040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName2040_callback,
      (ui_callback_data_t)"Select 2040 ROM image" },
    { "3040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName3040_callback,
      (ui_callback_data_t)"Select 3040 ROM image" },
    { "4040 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName4040_callback,
      (ui_callback_data_t)"Select 4040 ROM image" },
    { "1001 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1001_callback,
      (ui_callback_data_t)"Select 1001 ROM image" },
    { NULL }
};

const ui_menu_entry_t iec_drive_rom_menu[] = {
    { "1541 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541_callback,
      (ui_callback_data_t)"Select 1541 ROM image" },
    { "1541-II ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1541ii_callback,
      (ui_callback_data_t)"Select 1541-II ROM image" },
    { "1570 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1570_callback,
      (ui_callback_data_t)"Select 1570 ROM image" },
    { "1571 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1571_callback,
      (ui_callback_data_t)"Select 1571 ROM image" },
    { "1581 ROM file",
      MENU_ENTRY_DIALOG,
      file_string_DosName1581_callback,
      (ui_callback_data_t)"Select 1581 ROM image" },
    { NULL }
};
