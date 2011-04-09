/*
 * uic64_256k.c
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

#include "uilib.h"
#include "uimenu.h"
#include "uic64_256k.h"

UI_MENU_DEFINE_TOGGLE(C64_256K)
UI_MENU_DEFINE_RADIO(C64_256Kbase)

UI_CALLBACK(set_c64_256k_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("256K image"), UILIB_FILTER_ALL);
}

static ui_menu_entry_t c64_256k_base_submenu[] = {
    { "$DE00-$DE7F", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xde00, NULL },
    { "$DE80-$DEFF", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xde80, NULL },
    { "$DF00-$DF7F", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xdf00, NULL },
    { "$DF80-$DFFF", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xdf80, NULL },
    { NULL }
};

ui_menu_entry_t c64_256k_submenu[] = {
    { N_("Enable"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_C64_256K, NULL, NULL },
    { N_("Base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_256k_base_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_c64_256k_image_name,
      (ui_callback_data_t)"C64_256Kfilename", NULL },
    { NULL }
};
