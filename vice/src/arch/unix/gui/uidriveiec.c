/*
 * uidriveiec.c
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
#include "uiapi.h"
#include "uidriveiec.h"
#include "uimenu.h"

UI_MENU_DEFINE_TOGGLE_COND(Drive8RAM2000, Drive8Type, drive_check_expansion2000)
UI_MENU_DEFINE_TOGGLE_COND(Drive8RAM4000, Drive8Type, drive_check_expansion4000)
UI_MENU_DEFINE_TOGGLE_COND(Drive8RAM6000, Drive8Type, drive_check_expansion6000)
UI_MENU_DEFINE_TOGGLE_COND(Drive8RAM8000, Drive8Type, drive_check_expansion8000)
UI_MENU_DEFINE_TOGGLE_COND(Drive8RAMA000, Drive8Type, drive_check_expansionA000)
UI_MENU_DEFINE_TOGGLE_COND(Drive9RAM2000, Drive9Type, drive_check_expansion2000)
UI_MENU_DEFINE_TOGGLE_COND(Drive9RAM4000, Drive9Type, drive_check_expansion4000)
UI_MENU_DEFINE_TOGGLE_COND(Drive9RAM6000, Drive9Type, drive_check_expansion6000)
UI_MENU_DEFINE_TOGGLE_COND(Drive9RAM8000, Drive9Type, drive_check_expansion8000)
UI_MENU_DEFINE_TOGGLE_COND(Drive9RAMA000, Drive9Type, drive_check_expansionA000)
UI_MENU_DEFINE_TOGGLE_COND(Drive10RAM2000, Drive10Type, drive_check_expansion2000)
UI_MENU_DEFINE_TOGGLE_COND(Drive10RAM4000, Drive10Type, drive_check_expansion4000)
UI_MENU_DEFINE_TOGGLE_COND(Drive10RAM6000, Drive10Type, drive_check_expansion6000)
UI_MENU_DEFINE_TOGGLE_COND(Drive10RAM8000, Drive10Type, drive_check_expansion8000)
UI_MENU_DEFINE_TOGGLE_COND(Drive10RAMA000, Drive10Type, drive_check_expansionA000)
UI_MENU_DEFINE_TOGGLE_COND(Drive11RAM2000, Drive11Type, drive_check_expansion2000)
UI_MENU_DEFINE_TOGGLE_COND(Drive11RAM4000, Drive11Type, drive_check_expansion4000)
UI_MENU_DEFINE_TOGGLE_COND(Drive11RAM6000, Drive11Type, drive_check_expansion6000)
UI_MENU_DEFINE_TOGGLE_COND(Drive11RAM8000, Drive11Type, drive_check_expansion8000)
UI_MENU_DEFINE_TOGGLE_COND(Drive11RAMA000, Drive11Type, drive_check_expansionA000)
UI_MENU_DEFINE_RADIO(Drive8IdleMethod)
UI_MENU_DEFINE_RADIO(Drive9IdleMethod)
UI_MENU_DEFINE_RADIO(Drive10IdleMethod)
UI_MENU_DEFINE_RADIO(Drive11IdleMethod)

UI_CALLBACK(uidriveiec_expansion_control)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int type;

        resources_get_int_sprintf("Drive%iType", &type, vice_ptr_to_int(UI_MENU_CB_PARAM) + 8);

        if (drive_check_expansion(type)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

UI_CALLBACK(uidriveiec_idle_method_control)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int type;

        resources_get_int_sprintf("Drive%iType", &type, vice_ptr_to_int(UI_MENU_CB_PARAM) + 8);

        if (drive_check_idle_method(type)) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}

ui_menu_entry_t uidriveiec_drive0_ram_expansion_submenu[] = {
    { "$2000-$3FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RAM2000, NULL, NULL },
    { "$4000-$5FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RAM4000, NULL, NULL },
    { "$6000-$7FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RAM6000, NULL, NULL },
    { "$8000-$9FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RAM8000, NULL, NULL },
    { "$A000-$BFFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidriveiec_drive1_ram_expansion_submenu[] = {
    { "$2000-$3FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RAM2000, NULL, NULL },
    { "$4000-$5FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RAM4000, NULL, NULL },
    { "$6000-$7FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RAM6000, NULL, NULL },
    { "$8000-$9FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RAM8000, NULL, NULL },
    { "$A000-$BFFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidriveiec_drive2_ram_expansion_submenu[] = {
    { "$2000-$3FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RAM2000, NULL, NULL },
    { "$4000-$5FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RAM4000, NULL, NULL },
    { "$6000-$7FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RAM6000, NULL, NULL },
    { "$8000-$9FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RAM8000, NULL, NULL },
    { "$A000-$BFFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t uidriveiec_drive3_ram_expansion_submenu[] = {
    { "$2000-$3FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RAM2000, NULL, NULL },
    { "$4000-$5FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RAM4000, NULL, NULL },
    { "$6000-$7FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RAM6000, NULL, NULL },
    { "$8000-$9FFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RAM8000, NULL, NULL },
    { "$A000-$BFFF RAM", UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t set_drive0_idle_method_submenu[] = {
    { N_("No traps"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("Skip cycles"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("Trap idle"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive1_idle_method_submenu[] = {
    { N_("No traps"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("Skip cycles"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("Trap idle"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive2_idle_method_submenu[] = {
    { N_("No traps"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("Skip cycles"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("Trap idle"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_idle_method_submenu[] = {
    { N_("No traps"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("Skip cycles"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("Trap idle"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};
