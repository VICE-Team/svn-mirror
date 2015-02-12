/*
 * uidrive.c
 *
 * Written by
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#include "drive.h"
#include "resources.h"
#include "uiapi.h"
#include "uidrive.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_TOGGLE(DriveSoundEmulation)
UI_MENU_DEFINE_TOGGLE(FlashTrueFS)
UI_MENU_DEFINE_RADIO(Drive8ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive9ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive10ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive11ExtendImagePolicy)

UI_CALLBACK(uidrive_extend_policy_control)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int type;

        resources_get_int_sprintf("Drive%iType", &type, vice_ptr_to_int(UI_MENU_CB_PARAM) + 8);

        if (drive_check_extend_policy(type)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

ui_menu_entry_t set_drive0_extend_image_policy_submenu[] = {
    { N_("Never extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("Ask on extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("Extend on access"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive1_extend_image_policy_submenu[] = {
    { N_("Never extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("Ask on extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("Extend on access"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive2_extend_image_policy_submenu[] = {
    { N_("Never extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("Ask on extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("Extend on access"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_extend_image_policy_submenu[] = {
    { N_("Never extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("Ask on extend"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("Extend on access"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

UI_CALLBACK(radio_Drive8Type)
{
    int current_value;

    resources_get_int("Drive8Type", &current_value);
    if (!CHECK_MENUS) {
        if (current_value != vice_ptr_to_int(UI_MENU_CB_PARAM)) {
            resources_set_int("Drive8Type", vice_ptr_to_int(UI_MENU_CB_PARAM));
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == vice_ptr_to_int(UI_MENU_CB_PARAM));
        if (drive_check_type(vice_ptr_to_int(UI_MENU_CB_PARAM), 0)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

UI_CALLBACK(radio_Drive9Type)
{
    int current_value;

    resources_get_int("Drive9Type", &current_value);
    if (!CHECK_MENUS) {
        if (current_value != vice_ptr_to_int(UI_MENU_CB_PARAM)) {
            resources_set_int("Drive9Type", vice_ptr_to_int(UI_MENU_CB_PARAM));
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == vice_ptr_to_int(UI_MENU_CB_PARAM));
        if (drive_check_type(vice_ptr_to_int(UI_MENU_CB_PARAM), 1)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

UI_CALLBACK(radio_Drive10Type)
{
    int current_value;

    resources_get_int("Drive10Type", &current_value);
    if (!CHECK_MENUS) {
        if (current_value != vice_ptr_to_int(UI_MENU_CB_PARAM)) {
            resources_set_int("Drive10Type", vice_ptr_to_int(UI_MENU_CB_PARAM));
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == vice_ptr_to_int(UI_MENU_CB_PARAM));
        if (drive_check_type(vice_ptr_to_int(UI_MENU_CB_PARAM), 2)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

UI_CALLBACK(radio_Drive11Type)
{
    int current_value;

    resources_get_int("Drive11Type", &current_value);
    if (!CHECK_MENUS) {
        if (current_value != vice_ptr_to_int(UI_MENU_CB_PARAM)) {
            resources_set_int("Drive11Type", vice_ptr_to_int(UI_MENU_CB_PARAM));
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == vice_ptr_to_int(UI_MENU_CB_PARAM));
        if (drive_check_type(vice_ptr_to_int(UI_MENU_CB_PARAM), 3)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

ui_menu_entry_t ui_drive_options_submenu[] = {
    { N_("True drive emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { N_("Drive sound emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_DriveSoundEmulation, NULL, NULL },
    { NULL }
};

static UI_CALLBACK(flash_select_dir)
{
    uilib_select_dir("FSFlashDir", _("Select Flash filesystem directory"), _("Path:"));
}

ui_menu_entry_t ui_flash_options_submenu[] = {
    { N_("Enable true hardware flash file system"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FlashTrueFS, NULL, NULL },
    { N_("Set Flash filesystem directory"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)flash_select_dir, NULL, NULL },
    { NULL }
};
