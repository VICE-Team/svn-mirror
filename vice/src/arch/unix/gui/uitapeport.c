/*
 * uitapeport.c
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "uilib.h"
#include "uimenu.h"
#include "uitapeport.h"

UI_MENU_DEFINE_TOGGLE(Datasette)
UI_MENU_DEFINE_TOGGLE(TapeLog)
UI_MENU_DEFINE_TOGGLE(TapeLogDestination)
UI_MENU_DEFINE_TOGGLE(CPClockF83)
UI_MENU_DEFINE_TOGGLE(CPClockF83Save)
UI_MENU_DEFINE_TOGGLE(TapeSenseDongle)
UI_MENU_DEFINE_TOGGLE(DTLBasicDongle)

UI_CALLBACK(set_tapelog_filename)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("Tape log file"), UILIB_FILTER_ALL);
}

ui_menu_entry_t tapeport_submenu[] = {
    { N_("Enable datasette"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Datasette, NULL, NULL },
    { N_("Enable tape sense dongle"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TapeSenseDongle, NULL, NULL },
    { N_("Enable DTL basic dongle"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_DTLBasicDongle, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable tape log device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TapeLog, NULL, NULL },
    { N_("Enable tape log save to user specified file"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TapeLogDestination, NULL, NULL },
    { N_("User specified tape log filename"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_tapelog_filename,
      (ui_callback_data_t)"TapeLogfilename", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable CP CLock F83"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CPClockF83, NULL, NULL },
    { N_("Save RTC data when changed"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CPClockF83Save, NULL, NULL },
    { NULL }
};
