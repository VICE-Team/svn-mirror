/*
 * uiretroreplay.c
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
#include "uiretroreplay.h"

UI_MENU_DEFINE_TOGGLE(RRFlashJumper)
UI_MENU_DEFINE_TOGGLE(RRBankJumper)
UI_MENU_DEFINE_TOGGLE(RRBiosWrite)
UI_MENU_DEFINE_RADIO(RRrevision)

static UI_CALLBACK(retroreplay_flush_callback);
static UI_CALLBACK(retroreplay_save_callback);

static ui_menu_entry_t retroreplay_revision_submenu[] = {
    { CARTRIDGE_NAME_RETRO_REPLAY, UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRrevision,
      (ui_callback_data_t)0, NULL },
    { CARTRIDGE_NAME_NORDIC_REPLAY, UI_MENU_TYPE_TICK, (ui_callback_t)radio_RRrevision,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

ui_menu_entry_t retroreplay_submenu[] = {
    { N_("Enable flashjumper"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RRFlashJumper, NULL, NULL },
    { N_("Enable bankjumper"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RRBankJumper, NULL, NULL },
    { N_("Revision"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, retroreplay_revision_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Save image when changed"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RRBiosWrite, NULL, NULL },
    { N_("Save image now"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)retroreplay_flush_callback, NULL, NULL },
    { N_("Save image as"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)retroreplay_save_callback, NULL, NULL },
    { NULL }
};

static UI_CALLBACK(retroreplay_save_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_RETRO_REPLAY));
    } else {
        ui_cartridge_save_dialog(CARTRIDGE_RETRO_REPLAY);
    }
}

static UI_CALLBACK(retroreplay_flush_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_RETRO_REPLAY));
    } else {
        if (cartridge_flush_image(CARTRIDGE_RETRO_REPLAY) < 0) {
            ui_error(_("Can not save cartridge"));
        }
    }
}
