/*
 * uiautostart.c
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include "autostart-prg.h"
#include "uiautostart.h"
#include "uilib.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE(AutostartHandleTrueDriveEmulation)
UI_MENU_DEFINE_TOGGLE(AutostartWarp)
UI_MENU_DEFINE_TOGGLE(AutostartRunWithColon)
UI_MENU_DEFINE_RADIO(AutostartPrgMode)

UI_CALLBACK(set_autostart_image_name)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("Disk image for PRG autostart"),
                        _("Name:"));
}

static ui_menu_entry_t uiautostart_prg_mode_submenu[] = {
    { N_("*VirtualFS"),
      (ui_callback_t)radio_AutostartPrgMode, (ui_callback_data_t)AUTOSTART_PRG_MODE_VFS, NULL },
    { N_("*Inject to RAM"),
      (ui_callback_t)radio_AutostartPrgMode, (ui_callback_data_t)AUTOSTART_PRG_MODE_INJECT, NULL },
    { N_("*Disk image"),
      (ui_callback_t)radio_AutostartPrgMode, (ui_callback_data_t)AUTOSTART_PRG_MODE_DISK, NULL },
    { NULL }
};

ui_menu_entry_t uiautostart_settings_submenu[] = {
    { N_("*Handle True Drive Emulation on autostart"),
      (ui_callback_t)toggle_AutostartHandleTrueDriveEmulation, NULL, NULL },
    { N_("*Autostart warp"),
      (ui_callback_t)toggle_AutostartWarp, NULL, NULL },
    { N_("*RUN with colon"),
      (ui_callback_t)toggle_AutostartRunWithColon, NULL, NULL },
    { N_("Autostart PRG mode"),
      NULL, NULL, uiautostart_prg_mode_submenu },
    { N_("Autostart PRG disk image..."),
      (ui_callback_t)set_autostart_image_name,
      (ui_callback_data_t)"AutostartPrgDiskImage", NULL },
    { NULL }
};
