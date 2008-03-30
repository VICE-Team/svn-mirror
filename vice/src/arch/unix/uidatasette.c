/*
 * uidatasette.c
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "datasette.h"
#include "uimenu.h"

static UI_CALLBACK(ui_datasette_control)
{
    int command = (int)UI_MENU_CB_PARAM;
    datasette_control(command);
}

ui_menu_entry_t datasette_control_submenu[] = {
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

