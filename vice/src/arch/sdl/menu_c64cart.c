/*
 * menu_c64cart.c - Implementation of the c64 cartridge settings menu for the SDL UI.
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
#include "menu_common.h"
#include "menu_c64cart.h"
#include "menu_c64cart_common.h"
#include "ui.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE(CartridgeReset)

const ui_menu_entry_t c64cart_menu[] = {
    { "Attach CRT image",
      MENU_ENTRY_DIALOG,
      attach_c64_cart_callback,
      (ui_callback_data_t)CARTRIDGE_CRT },
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
    { NULL }
};
