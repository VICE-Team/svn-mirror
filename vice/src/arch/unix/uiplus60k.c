/*
 * uiplus60k.c
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
#include "uiplus60k.h"


UI_MENU_DEFINE_TOGGLE(PLUS60K)
UI_MENU_DEFINE_RADIO(PLUS60Kbase)

UI_CALLBACK(set_plus60k_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("PLUS60K image name"),
                        _("Name:"));
}

static ui_menu_entry_t plus60k_base_submenu[] = {
    { "*$D040", (ui_callback_t)radio_PLUS60Kbase,
      (ui_callback_data_t)0xd040, NULL },
    { "*$D100", (ui_callback_t)radio_PLUS60Kbase,
      (ui_callback_data_t)0xd100, NULL },
    { NULL }
};

ui_menu_entry_t plus60k_submenu[] = {
    { N_("*Enable PLUS60K"),
      (ui_callback_t)toggle_PLUS60K, NULL, NULL },
    { N_("PLUS60K base address"),
      NULL, NULL, plus60k_base_submenu },
    { N_("PLUS60K image name..."),
      (ui_callback_t)set_plus60k_image_name,
      (ui_callback_data_t)"PLUS60Kfilename", NULL },
    { NULL }
};
