/*
 * menu_c64cart.c - Implementation of the C64/C128 cartridge settings menu for the SDL UI.
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
#include "easyflash.h"
#include "keyboard.h"
#include "lib.h"
#include "menu_common.h"
#include "menu_c64cart.h"
#include "resources.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"

static UI_MENU_CALLBACK(attach_c64_cart_callback)
{
    char *title;
    char *name = NULL;

    if (activated) {
        switch (vice_ptr_to_int(param)) {
            case CARTRIDGE_CRT:
                title = "Select CRT image";
                break;
            case CARTRIDGE_GENERIC_8KB:
                title = "Select generic 8kB image";
                break;
            case CARTRIDGE_GENERIC_16KB:
                title = "Select generic 16kB image";
                break;
            case CARTRIDGE_ACTION_REPLAY:
                title = "Select Action Replay image";
                break;
            case CARTRIDGE_ACTION_REPLAY3:
                title = "Select Action Replay 3 image";
                break;
            case CARTRIDGE_ACTION_REPLAY4:
                title = "Select Action Replay 4 image";
                break;
            case CARTRIDGE_ATOMIC_POWER:
                title = "Select Atomic Power image";
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                title = "Select Epyx Fastload image";
                break;
            case CARTRIDGE_IDE64:
                title = "Select IDE64 interface image";
                break;
            case CARTRIDGE_IEEE488:
                title = "Select IEEE488 interface image";
                break;
            case CARTRIDGE_RETRO_REPLAY:
                title = "Select Retro Replay image";
                break;
            case CARTRIDGE_MMC_REPLAY:
                title = "Select MMC Replay image";
                break;
            case CARTRIDGE_STARDOS:
                title = "Select StarDOS image";
                break;
            case CARTRIDGE_STRUCTURED_BASIC:
                title = "Select Structured BASIC image";
                break;
            case CARTRIDGE_SUPER_SNAPSHOT:
                title = "Select Super Snapshot 4 image";
                break;
            case CARTRIDGE_SUPER_SNAPSHOT_V5:
            default:
                title = "Select Super Snapshot 5 image";
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

static const ui_menu_entry_t attach_raw_cart_menu[] = {
    { "Attach generic 8kB image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_GENERIC_8KB },
    { "Attach generic 16kB image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_GENERIC_16KB },
    { "Attach Action Replay image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_ACTION_REPLAY },
    { "Attach Action Replay 3 image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_ACTION_REPLAY3 },
    { "Attach Action Replay 4 image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_ACTION_REPLAY4 },
    { "Attach Atomic Power image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_ATOMIC_POWER },
    { "Attach Epyx Fastload image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_EPYX_FASTLOAD },
    { "Attach IDE64 interface image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_IDE64 },
    { "Attach IEEE488 interface image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_IEEE488 },
    { "Attach Retro Replay image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_RETRO_REPLAY },
    { "Attach StarDOS image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_STARDOS },
    { "Attach Structured BASIC image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_STRUCTURED_BASIC },
    { "Attach Super Snapshot 4 image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_SUPER_SNAPSHOT },
    { "Attach Super Snapshot 5 image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_SUPER_SNAPSHOT_V5 },
    { "Attach MMC Replay image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_MMC_REPLAY },
    { NULL }
};

static UI_MENU_CALLBACK(detach_c64_cart_callback)
{
    if (activated) {
        cartridge_detach_image();
    }
    return NULL;
}

static UI_MENU_CALLBACK(c64_cart_freeze_callback)
{
    if (activated) {
        keyboard_clear_keymatrix();
        cartridge_trigger_freeze();
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_c64_cart_default_callback)
{
    if (activated) {
        cartridge_set_default();
    }
    return NULL;
}


/* Expert cartridge */

UI_MENU_DEFINE_RADIO(CartridgeMode)

static UI_MENU_CALLBACK(enable_expert_callback)
{
    if (activated) {
        if (cartridge_attach_image(CARTRIDGE_EXPERT, NULL) < 0) {
            ui_error("Cannot enable Expert cartridge.");
        }
    }
    return NULL;
}

static const ui_menu_entry_t expert_cart_menu[] = {
    { "Enable Expert cartridge",
      MENU_ENTRY_OTHER,
      enable_expert_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Expert cartridge mode"),
    { "Off",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_CartridgeMode_callback,
      (ui_callback_data_t)CARTRIDGE_MODE_OFF },
    { "Prg",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_CartridgeMode_callback,
      (ui_callback_data_t)CARTRIDGE_MODE_PRG },
    { "On",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_CartridgeMode_callback,
      (ui_callback_data_t)CARTRIDGE_MODE_ON },
    { NULL }
};


/* EasyFlash */

UI_MENU_DEFINE_TOGGLE(EasyFlashJumper)
UI_MENU_DEFINE_TOGGLE(EasyFlashWriteCRT)

static UI_MENU_CALLBACK(easyflash_save_callback)
{
    if (activated) {
        if (easyflash_save_crt() < 0) {
            ui_error("Cannot save cartridge image.");
        }
    }
    return NULL;
}

static const ui_menu_entry_t easyflash_cart_menu[] = {
    { "Jumper",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_EasyFlashJumper_callback,
      NULL },
    { "Save CRT on detach",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_EasyFlashWriteCRT_callback,
      NULL },
    { "Save CRT now",
      MENU_ENTRY_OTHER,
      easyflash_save_callback,
      NULL },
    { NULL }
};


/* MMC64 */

UI_MENU_DEFINE_RADIO(MMC64_sd_type)

static const ui_menu_entry_t mmc64_sd_type_menu[] = {
    { "Auto",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_sd_type_callback,
      (ui_callback_data_t)0 },
    { "MMC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_sd_type_callback,
      (ui_callback_data_t)1 },
    { "SD",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_sd_type_callback,
      (ui_callback_data_t)2 },
    { "SDHC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_sd_type_callback,
      (ui_callback_data_t)3 },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(MMC64)
UI_MENU_DEFINE_RADIO(MMC64_revision)
UI_MENU_DEFINE_TOGGLE(MMC64_flashjumper)
UI_MENU_DEFINE_TOGGLE(MMC64_bios_write)
UI_MENU_DEFINE_FILE_STRING(MMC64BIOSfilename)
UI_MENU_DEFINE_TOGGLE(MMC64_RO)
UI_MENU_DEFINE_FILE_STRING(MMC64imagefilename)

static const ui_menu_entry_t mmc64_cart_menu[] = {
    { "Enable MMC64",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMC64_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 revision"),
    { "Rev A",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_revision_callback,
      (ui_callback_data_t)0 },
    { "Rev B",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMC64_revision_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    { "BIOS flash jumper",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMC64_flashjumper_callback,
      NULL },
    { "BIOS writes",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMC64_bios_write_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 BIOS image"),
    { "MMC64 BIOS image file",
      MENU_ENTRY_DIALOG,
      file_string_MMC64BIOSfilename_callback,
      (ui_callback_data_t)"Select MMC64 BIOS image" },
    SDL_MENU_ITEM_SEPARATOR,
    { "MMC64 image read-only",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMC64_RO_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("MMC64 MMC/SD image"),
    { "MMC64 MMC/SD image file",
      MENU_ENTRY_DIALOG,
      file_string_MMC64imagefilename_callback,
      (ui_callback_data_t)"Select MMC64 MMC/SD image" },
    { "Card type",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)mmc64_sd_type_menu },
    { NULL }
};


/* MMC Replay */
UI_MENU_DEFINE_RADIO(MMCRSDType)

static const ui_menu_entry_t mmcreplay_sd_type_menu[] = {
    { "Auto",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMCRSDType_callback,
      (ui_callback_data_t)0 },
    { "MMC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMCRSDType_callback,
      (ui_callback_data_t)1 },
    { "SD",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMCRSDType_callback,
      (ui_callback_data_t)2 },
    { "SDHC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MMCRSDType_callback,
      (ui_callback_data_t)3 },
    { NULL }
};

UI_MENU_DEFINE_FILE_STRING(MMCRCardImage)
UI_MENU_DEFINE_FILE_STRING(MMCREEPROMImage)
UI_MENU_DEFINE_TOGGLE(MMCRCardRW)
UI_MENU_DEFINE_TOGGLE(MMCREEPROMRW)
UI_MENU_DEFINE_TOGGLE(MMCRRescueMode)

static const ui_menu_entry_t mmcreplay_cart_menu[] = {
    { "Card image file",
      MENU_ENTRY_DIALOG,
      file_string_MMCRCardImage_callback,
      (ui_callback_data_t)"Select MMC Replay card image" },
    { "EEPROM image file",
      MENU_ENTRY_DIALOG,
      file_string_MMCREEPROMImage_callback,
      (ui_callback_data_t)"Select MMC Replay EEPROM image" },
    { "Enable writes to card image",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMCRCardRW_callback,
      NULL },
    { "Enable writes to EEPROM image",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMCREEPROMRW_callback,
      NULL },
    { "Enable rescue mode",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MMCRRescueMode_callback,
      NULL },
    { "Card type",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)mmcreplay_sd_type_menu },
    { NULL }
};


UI_MENU_DEFINE_TOGGLE(CartridgeReset)

const ui_menu_entry_t c64cart_menu[] = {
    { "Attach CRT image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_CRT },
    { "Attach raw image",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)attach_raw_cart_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Detach cartridge image",
      MENU_ENTRY_OTHER,
      detach_c64_cart_callback,
      NULL },
    { "Cartridge freeze",
      MENU_ENTRY_OTHER,
      c64_cart_freeze_callback,
      NULL },
    { "Set current cartridge as default",
      MENU_ENTRY_OTHER,
      set_c64_cart_default_callback,
      NULL },
    { "Reset on cartridge change",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CartridgeReset_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Expert cartridge settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)expert_cart_menu },
    { "EasyFlash cartridge settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)easyflash_cart_menu },
    { "MMC64 cartridge settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)mmc64_cart_menu },
    { "MMC Replay cartridge settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)mmcreplay_cart_menu },
    { NULL }
};
