/*
 * menu_c64_common_expansions.c - C64/C128 expansions menu for SDL UI.
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

#include "menu_c64_common_expansions.h"
#include "menu_common.h"
#include "uimenu.h"

/* SFS SOUNDEXPANDER MENU */

UI_MENU_DEFINE_TOGGLE(SFXSoundExpander)
UI_MENU_DEFINE_RADIO(SFXSoundExpanderChip)

const ui_menu_entry_t soundexpander_menu[] = {
    {"Enable SFX Sound Expander",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_SFXSoundExpander_callback,
     NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("YM chip type"),
    {"3526",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_SFXSoundExpanderChip_callback,
     (ui_callback_data_t)3526},
    {"3812",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_SFXSoundExpanderChip_callback,
     (ui_callback_data_t)3812},
    {NULL}
};


/* DIGIMAX MENU */

UI_MENU_DEFINE_TOGGLE(DIGIMAX)
UI_MENU_DEFINE_RADIO(DIGIMAXbase)

const ui_menu_entry_t digimax_menu[] = {
    {"Enable Digimax",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_DIGIMAX_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Base address"),
    {"Userport",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdd00},
    {"$DE00",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xde00},
    {"$DE20",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xde20},
    {"$DE40",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xde40},
    {"$DE60",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xde60},
    {"$DE80",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xde80},
    {"$DEA0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdea0},
    {"$DEC0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdec0},
    {"$DEE0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdee0},
    {"$DF00",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdf00},
    {"$DF20",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdf20},
    {"$DF40",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdf40},
    {"$DF60",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdf60},
    {"$DF80",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdf80},
    {"$DFA0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdfa0},
    {"$DFC0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdfc0},
    {"$DFE0",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_DIGIMAXbase_callback,
     (ui_callback_data_t)0xdfe0},
    {NULL}
};


/* GEORAM MEMORY EXPANSION MENU */

UI_MENU_DEFINE_TOGGLE(GEORAM)
UI_MENU_DEFINE_RADIO(GEORAMsize)
UI_MENU_DEFINE_FILE_STRING(GEORAMfilename)

const ui_menu_entry_t georam_menu[] = {
    {"Enable GEORAM",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_GEORAM_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Memory size"),
    {"64kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)64},
    {"128kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)128},
    {"256kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)256},
    {"512kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)512},
    {"1024kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)1024},
    {"2048kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)2048},
    {"4096kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_GEORAMsize_callback,
     (ui_callback_data_t)4096},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("RAM image"),
    {"GEORAM image file",
     MENU_ENTRY_DIALOG,
     file_string_GEORAMfilename_callback,
     (ui_callback_data_t)"Select GEORAM image"},
    {NULL}
};


/* IDE64 CART MENU */

UI_MENU_DEFINE_FILE_STRING(IDE64Image)
UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize)
UI_MENU_DEFINE_INT(IDE64Cylinders)
UI_MENU_DEFINE_INT(IDE64Heads)
UI_MENU_DEFINE_INT(IDE64Sectors)

const ui_menu_entry_t ide64_menu[] = {
    SDL_MENU_ITEM_TITLE("HD image"),
    {"IDE64 HD image file",
     MENU_ENTRY_DIALOG,
     file_string_IDE64Image_callback,
     (ui_callback_data_t)"Select IDE64 HD image" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("HD geometry"),
    {"Autodetect geometry",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_IDE64AutodetectSize_callback,
     NULL},
    {"Cylinders",
     MENU_ENTRY_RESOURCE_INT,
     int_IDE64Cylinders_callback,
     (ui_callback_data_t)"Enter amount of cylinders (1-1024)"},
    {"Heads",
     MENU_ENTRY_RESOURCE_INT,
     int_IDE64Heads_callback,
     (ui_callback_data_t)"Enter amount of heads (1-16)"},
    {"Sectors",
     MENU_ENTRY_RESOURCE_INT,
     int_IDE64Sectors_callback,
     (ui_callback_data_t)"Enter amount of sectors (0-63)"},
    {NULL}
};


/* MMC64 CART MENU */

UI_MENU_DEFINE_TOGGLE(MMC64)
UI_MENU_DEFINE_RADIO(MMC64_revision)
UI_MENU_DEFINE_TOGGLE(MMC64_flashjumper)
UI_MENU_DEFINE_TOGGLE(MMC64_bios_write)
UI_MENU_DEFINE_FILE_STRING(MMC64BIOSfilename)
UI_MENU_DEFINE_TOGGLE(MMC64_RO)
UI_MENU_DEFINE_FILE_STRING(MMC64imagefilename)

const ui_menu_entry_t mmc64_menu[] = {
    {"Enable MMC64",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_MMC64_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 revision"),
    {"Rev A",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_MMC64_revision_callback,
     (ui_callback_data_t)0},
    {"Rev B",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_MMC64_revision_callback,
     (ui_callback_data_t)1},
    SDL_MENU_ITEM_SEPARATOR,
    {"BIOS flash jumper",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_MMC64_flashjumper_callback,
     NULL},
    {"BIOS writes",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_MMC64_bios_write_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 BIOS image"),
    {"MMC64 BIOS image file",
     MENU_ENTRY_DIALOG,
     file_string_MMC64BIOSfilename_callback,
     (ui_callback_data_t)"Select MMC64 BIOS image"},
    SDL_MENU_ITEM_SEPARATOR,
    {"MMC64 image read-only",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_MMC64_RO_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 MMC/SD image"),
    {"MMC64 MMC/SD image file",
     MENU_ENTRY_DIALOG,
     file_string_MMC64imagefilename_callback,
     (ui_callback_data_t)"Select MMC64 MMC/SD image"},
    {NULL}
};


/* RAMCART MEMORY EXPANSION MENU */

UI_MENU_DEFINE_TOGGLE(RAMCART)
UI_MENU_DEFINE_TOGGLE(RAMCART_RO)
UI_MENU_DEFINE_RADIO(RAMCARTsize)
UI_MENU_DEFINE_FILE_STRING(RAMCARTfilename)

const ui_menu_entry_t ramcart_menu[] = {
    {"Enable RAMCART",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_RAMCART_callback,
     NULL},
    {"RAMCART read-only",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_RAMCART_RO_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Memory size"),
    {"64kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_RAMCARTsize_callback,
     (ui_callback_data_t)64},
    {"128kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_RAMCARTsize_callback,
     (ui_callback_data_t)128},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("RAM image"),
    {"RAMCART image file",
     MENU_ENTRY_DIALOG,
     file_string_RAMCARTfilename_callback,
     (ui_callback_data_t)"Select RAMCART image"},
    {NULL}
};


UI_MENU_DEFINE_TOGGLE(REU)
UI_MENU_DEFINE_RADIO(REUsize)
UI_MENU_DEFINE_FILE_STRING(REUfilename)

const ui_menu_entry_t reu_menu[] = {
    {"Enable REU",
     MENU_ENTRY_RESOURCE_TOGGLE,
     toggle_REU_callback,
     NULL},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Memory size"),
    {"128kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)128},
    {"256kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)256},
    {"512kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)512},
    {"1024kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)1024},
    {"2048kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)2048},
    {"4096kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)4096},
    {"8192kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)8192},
    {"16384kB",
     MENU_ENTRY_RESOURCE_RADIO,
     radio_REUsize_callback,
     (ui_callback_data_t)16384},
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("RAM image"),
    {"REU image file",
     MENU_ENTRY_DIALOG,
     file_string_REUfilename_callback,
     (ui_callback_data_t)"Select REU image"},
    {NULL}
};
