/*
 * uidriveplus4.c
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
#include "uidriveiec.h"
#include "uidriveplus4.h"
#include "uimenu.h"
#include "uiromset.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_TOGGLE(Drive8ParallelCable)
UI_MENU_DEFINE_TOGGLE(Drive9ParallelCable)
UI_MENU_DEFINE_TOGGLE(Drive10ParallelCable)
UI_MENU_DEFINE_TOGGLE(Drive11ParallelCable)


static ui_menu_entry_t set_drive0_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1551", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1551, NULL },
    { "*1570", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1570, NULL },
    { "*1571", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1581", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1570", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1570, NULL },
    { "*1571", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1551", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1551, NULL },
    { "*1581", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive2_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1570", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1570, NULL },
    { "*1571", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1551", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1551, NULL },
    { "*1581", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive3_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1570", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1570, NULL },
    { "*1571", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1551", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1551, NULL },
    { "*1581", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t driveplus4_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 model"),
      NULL, NULL, set_drive0_type_submenu },
    { N_("Drive #8 RAM expansion"),
      NULL, NULL, set_drive0_expansion_submenu },
    { N_("*Drive #8 enable parallel cable"),
      (ui_callback_t)toggle_Drive8ParallelCable, NULL, NULL },
    { N_("Drive #8 40-track image support"),
      NULL, NULL, set_drive0_extend_image_policy_submenu },
    { N_("Drive #8 idle method"),
      NULL, NULL, set_drive0_idle_method_submenu },
    { "--" },
    { N_("Drive #9 model"),
      NULL, NULL, set_drive1_type_submenu },
    { N_("Drive #9 RAM expansion"),
      NULL, NULL, set_drive1_expansion_submenu },
    { N_("*Drive #9 enable parallel cable"),
      (ui_callback_t)toggle_Drive9ParallelCable, NULL, NULL },
    { N_("Drive #9 40-track image support"),
      NULL, NULL, set_drive1_extend_image_policy_submenu },
    { N_("Drive #9 idle method"),
      NULL, NULL, set_drive1_idle_method_submenu },
    { "--" },
    { N_("Drive #10 model"),
      NULL, NULL, set_drive2_type_submenu },
    { N_("Drive #10 RAM expansion"),
      NULL, NULL, set_drive2_expansion_submenu },
    { N_("*Drive #10 enable parallel cable"),
      (ui_callback_t)toggle_Drive10ParallelCable, NULL, NULL },
    { N_("Drive #10 40-track image support"),
      NULL, NULL, set_drive2_extend_image_policy_submenu },
    { N_("Drive #10 idle method"),
      NULL, NULL, set_drive2_idle_method_submenu },
    { "--" },
    { N_("Drive #11 model"),
      NULL, NULL, set_drive3_type_submenu },
    { N_("Drive #11 RAM expansion"),
      NULL, NULL, set_drive3_expansion_submenu },
    { N_("*Drive #11 enable parallel cable"),
      (ui_callback_t)toggle_Drive11ParallelCable, NULL, NULL },
    { N_("Drive #11 40-track image support"),
      NULL, NULL, set_drive3_extend_image_policy_submenu },
    { N_("Drive #11 idle method"),
      NULL, NULL, set_drive3_idle_method_submenu },
    { NULL }
};

ui_menu_entry_t ui_driveplus4_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, driveplus4_settings_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

ui_menu_entry_t ui_driveplus4_romset_submenu[] = {
    { N_("Load new 1541 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1541", NULL },
    { N_("Load new 1541-II ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1541ii", NULL },
    { N_("Load new 1551 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1551", NULL },
    { N_("Load new 1570 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1570", NULL },
    { N_("Load new 1571 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1571", NULL },
    { N_("Load new 1581 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1581", NULL },
    { NULL }
};

