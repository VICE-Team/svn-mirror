/*
 * uiisepic.c
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

#include "cartridge.h"
#include "ui.h"
#include "uicartridge.h"
#include "uilib.h"
#include "uimenu.h"
#include "uiisepic.h"

static UI_CALLBACK(isepic_save_cartridge)
{
    ui_cartridge_save_dialog(CARTRIDGE_ISEPIC);
}

UI_MENU_DEFINE_TOGGLE(IsepicCartridgeEnabled)
UI_MENU_DEFINE_TOGGLE(IsepicSwitch)

ui_menu_entry_t isepic_submenu[] = {
    { N_("*Enable ISEPIC"),
      (ui_callback_t)toggle_IsepicCartridgeEnabled, NULL, NULL },
    { N_("*Enable ISEPIC Switch"),
      (ui_callback_t)toggle_IsepicSwitch, NULL, NULL },
    { "--" },
    { N_("Save ISEPIC image..."),
      (ui_callback_t)isepic_save_cartridge, NULL, NULL },
    { NULL }
};
