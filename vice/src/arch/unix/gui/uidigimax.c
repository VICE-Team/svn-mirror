/*
 * uidigimax.c
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
#include "uidigimax.h"

UI_MENU_DEFINE_TOGGLE(DIGIMAX)
UI_MENU_DEFINE_RADIO(DIGIMAXbase)

static ui_menu_entry_t digimax_base_submenu[] = {
    { "*Userport", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdd00, NULL },
    { "*$DE00", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xde00, NULL },
    { "*$DE20", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xde20, NULL },
    { "*$DE40", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xde40, NULL },
    { "*$DE60", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xde60, NULL },
    { "*$DE80", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xde80, NULL },
    { "*$DEA0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdea0, NULL },
    { "*$DEC0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdec0, NULL },
    { "*$DEE0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdee0, NULL },
    { "*$DF00", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdf00, NULL },
    { "*$DF20", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdf20, NULL },
    { "*$DF40", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdf40, NULL },
    { "*$DF60", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdf60, NULL },
    { "*$DF80", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdf80, NULL },
    { "*$DFA0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdfa0, NULL },
    { "*$DFC0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdfc0, NULL },
    { "*$DFE0", (ui_callback_t)radio_DIGIMAXbase,
      (ui_callback_data_t)0xdfe0, NULL },
    { NULL }
};

ui_menu_entry_t digimax_submenu[] = {
    { N_("*Enable digimax"),
      (ui_callback_t)toggle_DIGIMAX, NULL, NULL },
    { N_("Digimax base"),
      NULL, NULL, digimax_base_submenu },
    { NULL }
};
