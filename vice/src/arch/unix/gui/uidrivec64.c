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
UI_MENU_DEFINE_TOGGLE_COND(Drive8ParallelCable, Drive8Type,
                           drive_check_parallel_cable)
UI_MENU_DEFINE_TOGGLE_COND(Drive9ParallelCable, Drive9Type,
                           drive_check_parallel_cable)
UI_MENU_DEFINE_TOGGLE_COND(Drive10ParallelCable, Drive10Type,
                           drive_check_parallel_cable)
UI_MENU_DEFINE_TOGGLE_COND(Drive11ParallelCable, Drive11Type,
                           drive_check_parallel_cable)


static ui_menu_entry_t drivec64_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 model"),
      NULL, NULL, uidrivec64vic20_set_drive0_type_submenu },
    { N_("*Drive #8 RAM expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)0,
      set_drive0_expansion_submenu },
    { N_("*Drive #8 enable parallel cable"),
      (ui_callback_t)toggle_Drive8ParallelCable, NULL, NULL },
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
    { N_("*Drive #9 enable parallel cable"),
      (ui_callback_t)toggle_Drive9ParallelCable, NULL, NULL },
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
    { N_("*Drive #10 enable parallel cable"),
      (ui_callback_t)toggle_Drive10ParallelCable, NULL, NULL },
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
    { N_("*Drive #11 enable parallel cable"),
      (ui_callback_t)toggle_Drive11ParallelCable, NULL, NULL },
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

