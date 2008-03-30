/*
 * uidrivec64c128.c
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
#include "uidrivec64c128.h"
#include "uidriveiec.h"
#include "uimenu.h"


UI_MENU_DEFINE_RADIO(Drive8ParallelCable)
UI_MENU_DEFINE_RADIO(Drive9ParallelCable)
UI_MENU_DEFINE_RADIO(Drive10ParallelCable)
UI_MENU_DEFINE_RADIO(Drive11ParallelCable)
UI_MENU_DEFINE_TOGGLE_COND(Drive8ProfDOS, Drive8Type,
                           drive_check_profdos)
UI_MENU_DEFINE_TOGGLE_COND(Drive9ProfDOS, Drive9Type,
                           drive_check_profdos)
UI_MENU_DEFINE_TOGGLE_COND(Drive10ProfDOS, Drive10Type,
                           drive_check_profdos)
UI_MENU_DEFINE_TOGGLE_COND(Drive11ProfDOS, Drive11Type,
                           drive_check_profdos)


static UI_CALLBACK(parallel_cable_control)
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

static ui_menu_entry_t set_drive0_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive8ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive9ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive2_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive10ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive3_parallel_cable_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_NONE, NULL },
    { N_("*Standard Userport"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_STANDARD, NULL },
    { N_("*Dolphin DOS 3"), (ui_callback_t)radio_Drive11ParallelCable,
      (ui_callback_data_t)DRIVE_PC_DD3, NULL },
    { NULL }
};

ui_menu_entry_t uidrivec64c128_drive0_expansion_submenu[] = {
    { N_("*Parallel cable"),
      (ui_callback_t)parallel_cable_control, (ui_callback_data_t)0,
      set_drive0_parallel_cable_submenu },
    { "--" },
    { "",
      NULL, NULL, uidriveiec_drive0_ram_expansion_submenu },
    { "--" },
    { N_("*Enable Professional DOS"),
      (ui_callback_t)toggle_Drive8ProfDOS, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidrivec64c128_drive1_expansion_submenu[] = {
    { N_("*Parallel cable"),
      (ui_callback_t)parallel_cable_control, (ui_callback_data_t)1,
      set_drive1_parallel_cable_submenu },
    { "--" },
    { "",
      NULL, NULL, uidriveiec_drive1_ram_expansion_submenu },
    { "--" },
    { N_("*Enable Professional DOS"),
      (ui_callback_t)toggle_Drive9ProfDOS, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidrivec64c128_drive2_expansion_submenu[] = {
    { N_("*Parallel cable"),
      (ui_callback_t)parallel_cable_control, (ui_callback_data_t)2,
      set_drive2_parallel_cable_submenu },
    { "--" },
    { "",
      NULL, NULL, uidriveiec_drive2_ram_expansion_submenu },
    { "--" },
    { N_("*Enable Professional DOS"),
      (ui_callback_t)toggle_Drive10ProfDOS, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidrivec64c128_drive3_expansion_submenu[] = {
    { N_("*Parallel cable"),
      (ui_callback_t)parallel_cable_control, (ui_callback_data_t)3,
      set_drive3_parallel_cable_submenu },
    { "--" },
    { "",
      NULL, NULL, uidriveiec_drive3_ram_expansion_submenu },
    { "--" },
    { N_("*Enable Professional DOS"),
      (ui_callback_t)toggle_Drive11ProfDOS, NULL, NULL },
    { NULL }
};

