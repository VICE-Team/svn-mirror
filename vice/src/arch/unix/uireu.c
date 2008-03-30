/*
 * uireu.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include "uireu.h"


UI_MENU_DEFINE_TOGGLE(REU)
UI_MENU_DEFINE_RADIO(REUsize)

UI_CALLBACK(set_reu_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("REU image name"),
                        _("Name:"));
}

static ui_menu_entry_t reu_size_submenu[] = {
    { "*128KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)128, NULL },
    { "*256KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)256, NULL },
    { "*512KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)512, NULL },
    { "*1024KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)1024, NULL },
    { "*2048KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)2048, NULL },
    { "*4096KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)4096, NULL },
    { "*8192KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)8192, NULL },
    { "*16384KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)16384, NULL },
    { NULL }
};

ui_menu_entry_t reu_submenu[] = {
    { N_("*Enable REU"),
      (ui_callback_t)toggle_REU, NULL, NULL },
    { N_("REU size"),
      NULL, NULL, reu_size_submenu },
    { N_("REU image name..."),
      (ui_callback_t)set_reu_image_name,
      (ui_callback_data_t)"REUfilename", NULL },
    { NULL }
};

