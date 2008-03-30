/*
 * uiplus256k.c
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
#include "uiplus256k.h"


UI_MENU_DEFINE_TOGGLE(PLUS256K)

UI_CALLBACK(set_plus256k_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("PLUS256K image name"),
                        _("Name:"));
}

ui_menu_entry_t plus256k_submenu[] = {
    { N_("*Enable PLUS256K"),
      (ui_callback_t)toggle_PLUS256K, NULL, NULL },
    { N_("PLUS256K image name..."),
      (ui_callback_t)set_plus256k_image_name,
      (ui_callback_data_t)"PLUS256Kfilename", NULL },
    { NULL }
};
