/*
 * uidatasette.c
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

#include "datasette.h"
#include "resources.h"
#include "uimenu.h"
#include "util.h"


static UI_CALLBACK(ui_datasette_control)
{
    int command = (int)UI_MENU_CB_PARAM;
    datasette_control(command);
}

static UI_CALLBACK(datasette_settings)
{
    int what = (int)UI_MENU_CB_PARAM;
    char *prompt, *title, *resource;
    char buf[50];
    ui_button_t button;
    int current;
    long res;

    if (what) {
        prompt = title = _("Datasette speed tuning");
        resource = "DatasetteSpeedTuning";
    } else {
        prompt = title = _("Datasette zero gap delay");
        resource = "DatasetteZeroGapDelay";
    }

    resources_get_value(resource, (void *)&current);

    sprintf(buf, "%d", current);
    button = ui_input_string(title, prompt, buf, 50);
    switch (button) {
      case UI_BUTTON_OK:
        if (util_string_to_long(buf, NULL, 10, &res) != 0) {
             ui_error(_("Invalid value: %s"), buf);
             return;
        }
        resources_set_value(resource, (resource_value_t)res);
        break;
    default:
        break;
    }
}


UI_MENU_DEFINE_TOGGLE(DatasetteResetWithCPU)

ui_menu_entry_t datasette_control_submenu[] = {
    { N_("*Reset Datasette on CPU Reset"),
      (ui_callback_t)toggle_DatasetteResetWithCPU, NULL, NULL },
    { N_("Datasette zero gap delay"),
      (ui_callback_t)datasette_settings, (ui_callback_data_t)0, NULL },
    { N_("Datasette speed tuning"),
      (ui_callback_t)datasette_settings, (ui_callback_data_t)1, NULL },
    { "--" },
    { N_("Stop"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_STOP, NULL },
    { N_("Play"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_START, NULL },
    { N_("Forward"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_FORWARD, NULL },
    { N_("Rewind"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_REWIND, NULL },
    { N_("Record"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_RECORD, NULL },
    { N_("Reset"), (ui_callback_t)ui_datasette_control,
      (ui_callback_data_t)DATASETTE_CONTROL_RESET, NULL },
    { NULL }
};

ui_menu_entry_t ui_datasette_commands_menu[] = {
    { N_("Datassette control"),
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

