/*
 * uidrivec64.c
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

#include "drive.h"
#include "resources.h"
#include "uidrive.h"
#include "uidrivec64.h"
#include "uidrivec64vic20.h"
#include "uidriveiec.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_RADIO(Drive8ParallelCable)
UI_MENU_DEFINE_RADIO(Drive9ParallelCable)
UI_MENU_DEFINE_RADIO(Drive10ParallelCable)
UI_MENU_DEFINE_RADIO(Drive11ParallelCable)


UI_CALLBACK(uidrivec64_parallel_cable_control)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int type;

        resources_get_int_sprintf("Drive%iType", &type,
                                  (int)UI_MENU_CB_PARAM + 8);

        if (drive_check_parallel_cable(type))
            ui_menu_set_sensitive(w, True);
        else
            ui_menu_set_sensitive(w, False);
    }
}

ui_menu_entry_t set_drive0_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

ui_menu_entry_t set_drive1_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

ui_menu_entry_t set_drive2_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

static ui_menu_entry_t drivec64_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 model"),
      NULL, NULL, uidrivec64vic20_set_drive0_type_submenu },
    { N_("*Drive #8 RAM expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)0,
      set_drive0_expansion_submenu },
    { N_("*Drive #8 parallel cable"),
      (ui_callback_t)uidrivec64_parallel_cable_control, (ui_callback_data_t)0,
      set_drive0_parallel_cable_submenu },
    { N_("*Drive #8 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)0,
      set_drive0_extend_image_policy_submenu },
    { N_("*Drive #8 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)0,
      set_drive0_idle_method_submenu },
    { "--" },
    { N_("Drive #9 model"),
      NULL, NULL, uidrivec64vic20_set_drive1_type_submenu },
    { N_("*Drive #9 RAM expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)1,
      set_drive1_expansion_submenu },
    { N_("*Drive #9 parallel cable"),
      (ui_callback_t)uidrivec64_parallel_cable_control, (ui_callback_data_t)1,
      set_drive1_parallel_cable_submenu },
    { N_("*Drive #9 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)1,
      set_drive1_extend_image_policy_submenu },
    { N_("*Drive #9 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)1,
      set_drive1_idle_method_submenu },
    { "--" },
    { N_("Drive #10 model"),
      NULL, NULL, uidrivec64vic20_set_drive2_type_submenu },
    { N_("*Drive #10 RAM expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)2,
      set_drive2_expansion_submenu },
    { N_("*Drive #10 parallel cable"),
      (ui_callback_t)uidrivec64_parallel_cable_control, (ui_callback_data_t)2,
      set_drive2_parallel_cable_submenu },
    { N_("*Drive #10 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)2,
      set_drive2_extend_image_policy_submenu },
    { N_("*Drive #10 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)2,
      set_drive2_idle_method_submenu },
    { "--" },
    { N_("Drive #11 model"),
      NULL, NULL, uidrivec64vic20_set_drive3_type_submenu },
    { N_("*Drive #11 RAM expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)3,
      set_drive3_expansion_submenu },
    { N_("*Drive #11 parallel cable"),
      (ui_callback_t)uidrivec64_parallel_cable_control, (ui_callback_data_t)3,
      set_drive3_parallel_cable_submenu },
    { N_("*Drive #11 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)3,
      set_drive3_extend_image_policy_submenu },
    { N_("*Drive #11 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)3,
      set_drive3_idle_method_submenu },
    { NULL }
};

ui_menu_entry_t ui_drivec64_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, drivec64_settings_submenu },
    { NULL }
};

