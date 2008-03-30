/*
 * uidrivevic20.c
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
#include "uidrive.h"
#include "uidrivec64vic20.h"
#include "uidriveiec.h"
#include "uidrivevic20.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)


static ui_menu_entry_t uidrivevic20_drive0_expansion_submenu[] = {
    { "",
      NULL, NULL, uidriveiec_drive0_ram_expansion_submenu },
    { NULL }
};

static ui_menu_entry_t uidrivevic20_drive1_expansion_submenu[] = {
    { "",
      NULL, NULL, uidriveiec_drive1_ram_expansion_submenu },
    { NULL }
};

static ui_menu_entry_t uidrivevic20_drive2_expansion_submenu[] = {
    { "",
      NULL, NULL, uidriveiec_drive2_ram_expansion_submenu },
    { NULL }
};

static ui_menu_entry_t uidrivevic20_drive3_expansion_submenu[] = {
    { "",
      NULL, NULL, uidriveiec_drive3_ram_expansion_submenu },
    { NULL }
};

static ui_menu_entry_t drivevic20_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 model"),
      NULL, NULL, uidrivec64vic20_set_drive0_type_submenu },
    { N_("*Drive #8 expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)0,
      uidrivevic20_drive0_expansion_submenu },
    { N_("*Drive #8 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)0,
      set_drive0_extend_image_policy_submenu },
    { N_("*Drive #8 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)0,
      set_drive0_idle_method_submenu },
    { "--" },
    { N_("Drive #9 model"),
      NULL, NULL, uidrivec64vic20_set_drive1_type_submenu },
    { N_("*Drive #9 expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)1,
      uidrivevic20_drive1_expansion_submenu },
    { N_("*Drive #9 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)1,
      set_drive1_extend_image_policy_submenu },
    { N_("*Drive #9 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)1,
      set_drive1_idle_method_submenu },
    { "--" },
    { N_("Drive #10 model"),
      NULL, NULL, uidrivec64vic20_set_drive2_type_submenu },
    { N_("*Drive #10 expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)2,
      uidrivevic20_drive2_expansion_submenu },
    { N_("*Drive #10 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)2,
      set_drive2_extend_image_policy_submenu },
    { N_("*Drive #10 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)2,
      set_drive2_idle_method_submenu },
    { "--" },
    { N_("Drive #11 model"),
      NULL, NULL, uidrivec64vic20_set_drive3_type_submenu },
    { N_("*Drive #11 expansion"),
      (ui_callback_t)uidriveiec_expansion_control, (ui_callback_data_t)3,
      uidrivevic20_drive3_expansion_submenu },
    { N_("*Drive #11 40-track image support"),
      (ui_callback_t)uidrive_extend_policy_control, (ui_callback_data_t)3,
      set_drive3_extend_image_policy_submenu },
    { N_("*Drive #11 idle method"),
      (ui_callback_t)uidriveiec_idle_method_control, (ui_callback_data_t)3,
      set_drive3_idle_method_submenu },
    { NULL }
};

ui_menu_entry_t ui_drivevic20_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, drivevic20_settings_submenu },
    { NULL }
};

