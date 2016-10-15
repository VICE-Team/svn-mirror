/*
 * uirrnetmk3.c
 *
 * Written by
 *  Groepaz <groepaz@gmx.net>
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

#include "cartridge.h"
#include "uiapi.h"
#include "uicartridge.h"
#include "uilib.h"
#include "uimenu.h"
#include "uirrnetmk3.h"

#include "clockport.h"

UI_MENU_DEFINE_TOGGLE(RRNETMK3_flashjumper)
UI_MENU_DEFINE_TOGGLE(RRNETMK3_bios_write)
UI_MENU_DEFINE_RADIO(RRNETMK3ClockPort);

static UI_CALLBACK(rrnetmk3_flush_callback);
static UI_CALLBACK(rrnetmk3_save_callback);


/** \brief  Submenu for clock port device selection
 *
 * Remove #if 0's when support for those devices has been implemented
 *
 * TODO: add string constants in src/c64/cart/clockport.h for the device names
 */
static ui_menu_entry_t rrnetmk3_clockport_device_submenu[] = {
    { "None", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_NONE, NULL },
#if 0
    { "ETH64-II", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_ETH64_II, NULL },
#endif
    { "RRNet", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_RRNET, NULL }
#if 0
,
    { "Silver Surfer", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_SILVER_SURFER, NULL },
    { "MP3@64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_MP3_64, NULL },
    { "CatWeasel MKIII SID", UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRNETMK3ClockPort,
        (ui_callback_data_t)CLOCKPORT_DEVICE_CW3_SID, NULL }
#endif
};


ui_menu_entry_t rrnetmk3_submenu[] = {
    { N_("Clockport device"), UI_MENU_TYPE_NORMAL,
        NULL, NULL, rrnetmk3_clockport_device_submenu },
    { N_("Enable flashjumper"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RRNETMK3_flashjumper, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Save image when changed"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RRNETMK3_bios_write, NULL, NULL },
    { N_("Save image now"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)rrnetmk3_flush_callback, NULL, NULL },
    { N_("Save image as"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)rrnetmk3_save_callback, NULL, NULL },
    { NULL }
};


static UI_CALLBACK(rrnetmk3_save_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_RRNETMK3));
    } else {
        ui_cartridge_save_dialog(CARTRIDGE_RRNETMK3);
    }
}

static UI_CALLBACK(rrnetmk3_flush_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_RRNETMK3));
    } else {
        if (cartridge_flush_image(CARTRIDGE_RRNETMK3) < 0) {
            ui_error(_("Can not save cartridge"));
        }
    }
}

