/*
 * uidrivepetcbm2.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include "uidrivepetcbm2.h"
#include "uimenu.h"
#include "uiromset.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)


static ui_menu_entry_t set_drive0_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*2031", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_2031, NULL },
    { "*2040", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_2040, NULL },
    { "*3040", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_3040, NULL },
    { "*4040", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_4040, NULL },
    { "*1001", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1001, NULL },
    { "*8050", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_8050, NULL },
    { "*8250", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_8250, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*2031", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_2031, NULL },
    { "*1001", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1001, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive2_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*2031", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_2031, NULL },
    { "*1001", (ui_callback_t)radio_Drive10Type,
      (ui_callback_data_t)DRIVE_TYPE_1001, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive3_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*2031", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_2031, NULL },
    { "*1001", (ui_callback_t)radio_Drive11Type,
      (ui_callback_data_t)DRIVE_TYPE_1001, NULL },
    { NULL }
};

static ui_menu_entry_t drivepetcbm2_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 floppy disk type"),
      NULL, NULL, set_drive0_type_submenu },
#if 0
    { N_("Drive #8 40-track image support"),
      NULL, NULL, set_drive0_extend_image_policy_submenu },
    { N_("Drive #8 idle method"),
      NULL, NULL, set_drive0_idle_method_submenu },
#endif
    { "--" },
    { N_("Drive #9 floppy disk type"),
      NULL, NULL, set_drive1_type_submenu },
#if 0
    { N_("Drive #9 40-track image support"),
      NULL, NULL, set_drive1_extend_image_policy_submenu },
    { N_("Drive #9 idle method"),
      NULL, NULL, set_drive1_idle_method_submenu },
#endif
    { "--" },
    { N_("Drive #10 floppy disk type"),
      NULL, NULL, set_drive2_type_submenu },
#if 0
    { N_("Drive #10 40-track image support"),
      NULL, NULL, set_drive2_extend_image_policy_submenu },
    { N_("Drive #10 idle method"),
      NULL, NULL, set_drive2_idle_method_submenu },
#endif
    { "--" },
    { N_("Drive #11 floppy disk type"),
      NULL, NULL, set_drive3_type_submenu },
#if 0
    { N_("Drive #11 40-track image support"),
      NULL, NULL, set_drive3_extend_image_policy_submenu },
    { N_("Drive #11 idle method"),
      NULL, NULL, set_drive3_idle_method_submenu },
#endif
    { NULL }
};

ui_menu_entry_t ui_drivepetcbm2_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, drivepetcbm2_settings_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

ui_menu_entry_t ui_drivepetcbm2_romset_submenu[] = {
    { N_("Load new 2031 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName2031", NULL },
    { N_("Load new 2040 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName2040", NULL },
    { N_("Load new 3040 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName3040", NULL },
    { N_("Load new 4040 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName4040", NULL },
    { N_("Load new 1001 ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"DosName1001", NULL },
    { NULL }
};

