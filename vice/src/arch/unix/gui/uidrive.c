/*
 * uidrive.c
 *
 * Written by
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
#include "uidrive.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)


UI_CALLBACK(radio_Drive8Type)
{
    int current_value;

    resources_get_value("Drive8Type", (void *)&current_value);
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

UI_CALLBACK(radio_Drive9Type)
{
    int current_value;

    resources_get_value("Drive9Type", (void *)&current_value);
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

UI_CALLBACK(radio_Drive10Type)
{
    int current_value;

    resources_get_value("Drive10Type", (void *)&current_value);
    if (!CHECK_MENUS) {
        if (current_value != (int)UI_MENU_CB_PARAM) {
            resources_set_value("Drive10Type",
                                (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int)UI_MENU_CB_PARAM);
        if (drive_check_type((int)UI_MENU_CB_PARAM, 2)) {
            ui_menu_set_sensitive(w, True);
        } else {
            ui_menu_set_sensitive(w, False);
        }
    }
}

UI_CALLBACK(radio_Drive11Type)
{
    int current_value;

    resources_get_value("Drive11Type", (void *)&current_value);
    if (!CHECK_MENUS) {
        if (current_value != (int)UI_MENU_CB_PARAM) {
            resources_set_value("Drive11Type",
                                (resource_value_t)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int)UI_MENU_CB_PARAM);
        if (drive_check_type((int)UI_MENU_CB_PARAM, 3)) {
            ui_menu_set_sensitive(w, True);
        } else {
            ui_menu_set_sensitive(w, False);
        }
    }
}

ui_menu_entry_t ui_drive_options_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { NULL }
};

