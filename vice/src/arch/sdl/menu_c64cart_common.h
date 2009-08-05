/*
 * menu_c64cart_common.h - Implementation of the c64/c128 cartridge settings menu for the SDL UI.
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

#ifndef VICE_UIMENU_C64CART_COMMON_H
#define VICE_UIMENU_C64CART_COMMON_H

#include "vice.h"

#include "uimenu.h"

extern UI_MENU_CALLBACK(attach_c64_cart_callback);
extern UI_MENU_CALLBACK(detach_c64_cart_callback);
extern UI_MENU_CALLBACK(c64_cart_freeze_callback);
extern UI_MENU_CALLBACK(set_c64_cart_default_callback);
extern UI_MENU_CALLBACK(enable_expert_callback);

extern const ui_menu_entry_t expert_cart_menu[];
extern const ui_menu_entry_t easyflash_cart_menu[];

#endif
