/*
 * uimmcreplay.c
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

#include "cartridge.h"
#include "uiapi.h"
#include "uicartridge.h"
#include "uilib.h"
#include "uimenu.h"
#include "uimmcreplay.h"

UI_MENU_DEFINE_TOGGLE(MMCRCardRW)
UI_MENU_DEFINE_TOGGLE(MMCREEPROMRW)
UI_MENU_DEFINE_TOGGLE(MMCRRescueMode)
UI_MENU_DEFINE_RADIO(MMCRSDType)
UI_MENU_DEFINE_TOGGLE(MMCRImageWrite) /* FIXME */

static UI_CALLBACK(mmcreplay_flush_callback);
static UI_CALLBACK(mmcreplay_save_callback);

UI_CALLBACK(set_mmcreplay_card_filename);

UI_CALLBACK(set_mmcreplay_eeprom_filename);

static ui_menu_entry_t mmcreplay_sd_type_submenu[] = {
    { N_("Auto"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MMCRSDType,
      (ui_callback_data_t)0, NULL },
    { "MMC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MMCRSDType,
      (ui_callback_data_t)1, NULL },
    { "SD", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MMCRSDType,
      (ui_callback_data_t)2, NULL },
    { "SDHC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MMCRSDType,
      (ui_callback_data_t)3, NULL },
    { NULL }
};


ui_menu_entry_t mmcreplay_submenu[] = {
    { N_("Enable rescue mode"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_MMCRRescueMode, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Save image when changed"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_MMCRImageWrite, NULL, NULL },
    { N_("Save image now"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)mmcreplay_flush_callback, NULL, NULL },
    { N_("Save image as"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)mmcreplay_save_callback, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    /* Translators: this means card as in SD/MMC card, not a cartridge! */
    { N_("Card image file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_mmcreplay_card_filename,
      (ui_callback_data_t)"MMCRCardImage", NULL },
    /* Translators: this means card as in SD/MMC card, not a cartridge! */
    { N_("Enable writes to card image"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_MMCRCardRW, NULL, NULL },
    /* Translators: this means card as in SD/MMC card, not a cartridge! */
    { N_("Card type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mmcreplay_sd_type_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("EEPROM image file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_mmcreplay_eeprom_filename,
      (ui_callback_data_t)"MMCREEPROMImage", NULL },
    { N_("Enable writes to MMC Replay EEPROM image"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_MMCREEPROMRW, NULL, NULL },
    { NULL }
};

static UI_CALLBACK(mmcreplay_save_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_MMC_REPLAY));
    } else {
        ui_cartridge_save_dialog(CARTRIDGE_MMC_REPLAY);
    }
}

static UI_CALLBACK(mmcreplay_flush_callback)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cartridge_type_enabled(CARTRIDGE_MMC_REPLAY));
    } else {
        if (cartridge_flush_image(CARTRIDGE_MMC_REPLAY) < 0) {
            ui_error(_("Can not save cartridge"));
        }
    }
}

UI_CALLBACK(set_mmcreplay_card_filename)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("MMC Replay card image filename"),
                        UILIB_FILTER_ALL);
}

UI_CALLBACK(set_mmcreplay_eeprom_filename)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("MMC Replay EEPROM image filename"),
                        UILIB_FILTER_ALL);
}

