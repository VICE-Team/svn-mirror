/*
 * uigeoram.c
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
#include "uigeoram.h"


UI_MENU_DEFINE_TOGGLE(GEORAM)
UI_MENU_DEFINE_RADIO(GEORAMsize)

UI_CALLBACK(set_georam_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("GEORAM image name"),
                        _("Name:"));
}

static ui_menu_entry_t georam_size_submenu[] = {
    { "*64KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)64, NULL },
    { "*128KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)128, NULL },
    { "*256KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)256, NULL },
    { "*512KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)512, NULL },
    { "*1024KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)1024, NULL },
    { "*2048KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)2048, NULL },
    { "*4096KB", (ui_callback_t)radio_GEORAMsize,
      (ui_callback_data_t)4096, NULL },
    { NULL }
};

ui_menu_entry_t georam_submenu[] = {
    { N_("*Enable GEORAM"),
      (ui_callback_t)toggle_GEORAM, NULL, NULL },
    { N_("GEORAM size"),
      NULL, NULL, georam_size_submenu },
    { N_("GEORAM image name..."),
      (ui_callback_t)set_georam_image_name,
      (ui_callback_data_t)"GEORAMfilename", NULL },
    { NULL }
};

