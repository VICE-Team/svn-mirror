/*
 * uiram.c
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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
#include <string.h>

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uilib.h"
#include "uimenu.h"
#include "util.h"

UI_MENU_DEFINE_RADIO(RAMInitStartValue)
UI_MENU_DEFINE_RADIO(RAMInitValueInvert)
UI_MENU_DEFINE_RADIO(RAMInitPatternInvert)

static ui_menu_entry_t constant_values_menu[] = {
    { "0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)0, NULL },
    { "1", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)1, NULL },
    { "2", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)2, NULL },
    { "4", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)4, NULL },
    { "8", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)8, NULL },
    { "16", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)16, NULL },
    { "32", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)32, NULL },
    { "64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)64, NULL },
    { "128", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)128, NULL },
    { "256", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)256, NULL },
    { "512", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)512, NULL },
    { "1024", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitValueInvert,
      (ui_callback_data_t)1024, NULL },
    { NULL }
};

static ui_menu_entry_t constant_pattern_menu[] = {
    { "0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)0, NULL },
    { "1", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)1, NULL },
    { "2", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)2, NULL },
    { "4", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)4, NULL },
    { "8", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)8, NULL },
    { "16", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)16, NULL },
    { "32", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)32, NULL },
    { "64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)64, NULL },
    { "128", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)128, NULL },
    { "256", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)256, NULL },
    { "512", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)512, NULL },
    { "1024", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitPatternInvert,
      (ui_callback_data_t)1024, NULL },
    { NULL }
};

static ui_menu_entry_t first_byte_menu[] = {
    { "0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitStartValue,
      (ui_callback_data_t)0, NULL },
    { "255", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RAMInitStartValue,
      (ui_callback_data_t)255, NULL },
    { NULL }
};

ui_menu_entry_t ui_ram_pattern_submenu[] = {
    { N_("Value of first byte"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, first_byte_menu },
    { N_("Length of constant values"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, constant_pattern_menu },
    { N_("Length of constant pattern"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, constant_values_menu },
    { NULL }
};

