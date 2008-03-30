/*
 * uidrive.c
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
#include "resources.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_TOGGLE(Drive8RAM2000)
UI_MENU_DEFINE_TOGGLE(Drive8RAM4000)
UI_MENU_DEFINE_TOGGLE(Drive8RAM6000)
UI_MENU_DEFINE_TOGGLE(Drive8RAM8000)
UI_MENU_DEFINE_TOGGLE(Drive8RAMA000)
UI_MENU_DEFINE_TOGGLE(Drive9RAM2000)
UI_MENU_DEFINE_TOGGLE(Drive9RAM4000)
UI_MENU_DEFINE_TOGGLE(Drive9RAM6000)
UI_MENU_DEFINE_TOGGLE(Drive9RAM8000)
UI_MENU_DEFINE_TOGGLE(Drive9RAMA000)
UI_MENU_DEFINE_TOGGLE(Drive8ParallelCable)
UI_MENU_DEFINE_TOGGLE(Drive9ParallelCable)

UI_MENU_DEFINE_RADIO(Drive8ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive9ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive8IdleMethod)
UI_MENU_DEFINE_RADIO(Drive9IdleMethod)


static UI_CALLBACK(radio_Drive8Type)
{
    int current_value;

    resources_get_value("Drive8Type", (resource_value_t *)&current_value);
    if (!CHECK_MENUS) {
        if (current_value != (int)UI_MENU_CB_PARAM) {
            resources_set_value("Drive8Type",
                                (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int)UI_MENU_CB_PARAM);
        if (drive_check_type((int)UI_MENU_CB_PARAM, 0)) {
            ui_menu_set_sensitive(w, True);
        } else {
            ui_menu_set_sensitive(w, False);
        }
    }
}

static UI_CALLBACK(radio_Drive9Type)
{
    int current_value;

    resources_get_value("Drive9Type", (resource_value_t *)&current_value);
    if (!CHECK_MENUS) {
        if (current_value != (int)UI_MENU_CB_PARAM) {
            resources_set_value("Drive9Type",
                                (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int)UI_MENU_CB_PARAM);
        if (drive_check_type((int)UI_MENU_CB_PARAM, 1)) {
            ui_menu_set_sensitive(w, True);
        } else {
            ui_menu_set_sensitive(w, False);
        }
    }
}

ui_menu_entry_t set_drive0_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1571", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1581", (ui_callback_t)radio_Drive8Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
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

ui_menu_entry_t set_drive1_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1541II, NULL },
    { "*1571", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1571, NULL },
    { "*1581", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1581, NULL },
    { "*2031", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_2031, NULL },
    { "*1001", (ui_callback_t)radio_Drive9Type,
      (ui_callback_data_t)DRIVE_TYPE_1001, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive0_expansion_submenu[] = {
    { N_("*$2000-$3FFF RAM expansion"),
      (ui_callback_t)toggle_Drive8RAM2000, NULL, NULL },
    { N_("*$4000-$5FFF RAM expansion"),
      (ui_callback_t)toggle_Drive8RAM4000, NULL, NULL },
    { N_("*$6000-$7FFF RAM expansion"),
      (ui_callback_t)toggle_Drive8RAM6000, NULL, NULL },
    { N_("*$8000-$9FFF RAM expansion"),
      (ui_callback_t)toggle_Drive8RAM8000, NULL, NULL },
    { N_("*$A000-$BFFF RAM expansion"),
      (ui_callback_t)toggle_Drive8RAMA000, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_expansion_submenu[] = {
    { N_("*$2000-$3FFF RAM expansion"),
      (ui_callback_t)toggle_Drive9RAM2000, NULL, NULL },
    { N_("*$4000-$5FFF RAM expansion"),
      (ui_callback_t)toggle_Drive9RAM4000, NULL, NULL },
    { N_("*$6000-$7FFF RAM expansion"),
      (ui_callback_t)toggle_Drive9RAM6000, NULL, NULL },
    { N_("*$8000-$9FFF RAM expansion"),
      (ui_callback_t)toggle_Drive9RAM8000, NULL, NULL },
    { N_("*$A000-$BFFF RAM expansion"),
      (ui_callback_t)toggle_Drive9RAMA000, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive0_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive0_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};


/* ------------------------------------------------------------------------- */

/* This menu is for the C64 */
static ui_menu_entry_t drive_settings_submenu[] = {
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
    { NULL }
};

ui_menu_entry_t ui_drive_options_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { NULL }
};

/* c64 */
ui_menu_entry_t ui_drive_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, drive_settings_submenu },
    { NULL }
};

