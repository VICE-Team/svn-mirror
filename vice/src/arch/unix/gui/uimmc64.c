/*
 * uimmc64.c
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
#include "uimmc64.h"


UI_MENU_DEFINE_TOGGLE(MMC64)
UI_MENU_DEFINE_RADIO(MMC64_revision)
UI_MENU_DEFINE_TOGGLE(MMC64_flashjumper)
UI_MENU_DEFINE_TOGGLE(MMC64_bios_write)
UI_MENU_DEFINE_TOGGLE(MMC64_RO)

UI_CALLBACK(set_mmc64_bios_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("MMC64 BIOS name"),
                        _("Name:"));
}

UI_CALLBACK(set_mmc64_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("MMC64 image name"),
                        _("Name:"));
}

static ui_menu_entry_t mmc64_revision_submenu[] = {
    { "*Rev. A", (ui_callback_t)radio_MMC64_revision,
      (ui_callback_data_t)0, NULL },
    { "*Rev. B", (ui_callback_t)radio_MMC64_revision,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

ui_menu_entry_t mmc64_submenu[] = {
    { N_("*Enable MMC64"),
      (ui_callback_t)toggle_MMC64, NULL, NULL },
    { N_("*MMC64 Revision"),
      NULL, NULL, mmc64_revision_submenu },
    { N_("*Enable MMC64 flashjumper"),
      (ui_callback_t)toggle_MMC64_flashjumper, NULL, NULL },
    { N_("*Enable MMC64 BIOS save when changed"),
      (ui_callback_t)toggle_MMC64_bios_write, NULL, NULL },
    { N_("MMC64 BIOS name..."),
      (ui_callback_t)set_mmc64_bios_name,
      (ui_callback_data_t)"MMC64BIOSfilename", NULL },
    { N_("*Enable MMC64 image read-only"),
      (ui_callback_t)toggle_MMC64_RO, NULL, NULL },
    { N_("MMC64 image name..."),
      (ui_callback_t)set_mmc64_image_name,
      (ui_callback_data_t)"MMC64imagefilename", NULL },
    { NULL }
};
