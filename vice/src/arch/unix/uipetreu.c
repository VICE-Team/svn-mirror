/*
 * uipetreu.c
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
#include "uipetreu.h"


UI_MENU_DEFINE_TOGGLE(PETREU)
UI_MENU_DEFINE_RADIO(PETREUsize)

UI_CALLBACK(set_petreu_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("PETREU image name"),
                        _("Name:"));
}

static ui_menu_entry_t petreu_size_submenu[] = {
    { "*128KB", (ui_callback_t)radio_PETREUsize,
      (ui_callback_data_t)128, NULL },
    { NULL }
};

ui_menu_entry_t petreu_submenu[] = {
    { N_("*Enable PET REU"),
      (ui_callback_t)toggle_PETREU, NULL, NULL },
    { N_("PET REU size"),
      NULL, NULL, petreu_size_submenu },
    { N_("PET REU image name..."),
      (ui_callback_t)set_petreu_image_name,
      (ui_callback_data_t)"PETREUfilename", NULL },
    { NULL }
};
