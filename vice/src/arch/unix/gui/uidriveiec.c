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
#include "uidriveiec.h"
#include "uimenu.h"


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
UI_MENU_DEFINE_TOGGLE(Drive10RAM2000)
UI_MENU_DEFINE_TOGGLE(Drive10RAM4000)
UI_MENU_DEFINE_TOGGLE(Drive10RAM6000)
UI_MENU_DEFINE_TOGGLE(Drive10RAM8000)
UI_MENU_DEFINE_TOGGLE(Drive10RAMA000)
UI_MENU_DEFINE_TOGGLE(Drive11RAM2000)
UI_MENU_DEFINE_TOGGLE(Drive11RAM4000)
UI_MENU_DEFINE_TOGGLE(Drive11RAM6000)
UI_MENU_DEFINE_TOGGLE(Drive11RAM8000)
UI_MENU_DEFINE_TOGGLE(Drive11RAMA000)
UI_MENU_DEFINE_RADIO(Drive8ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive9ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive10ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive11ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive8IdleMethod)
UI_MENU_DEFINE_RADIO(Drive9IdleMethod)
UI_MENU_DEFINE_RADIO(Drive10IdleMethod)
UI_MENU_DEFINE_RADIO(Drive11IdleMethod)


ui_menu_entry_t set_drive0_expansion_submenu[] = {
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

ui_menu_entry_t set_drive1_expansion_submenu[] = {
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

ui_menu_entry_t set_drive2_expansion_submenu[] = {
    { N_("*$2000-$3FFF RAM expansion"),
      (ui_callback_t)toggle_Drive10RAM2000, NULL, NULL },
    { N_("*$4000-$5FFF RAM expansion"),
      (ui_callback_t)toggle_Drive10RAM4000, NULL, NULL },
    { N_("*$6000-$7FFF RAM expansion"),
      (ui_callback_t)toggle_Drive10RAM6000, NULL, NULL },
    { N_("*$8000-$9FFF RAM expansion"),
      (ui_callback_t)toggle_Drive10RAM8000, NULL, NULL },
    { N_("*$A000-$BFFF RAM expansion"),
      (ui_callback_t)toggle_Drive10RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_expansion_submenu[] = {
    { N_("*$2000-$3FFF RAM expansion"),
      (ui_callback_t)toggle_Drive11RAM2000, NULL, NULL },
    { N_("*$4000-$5FFF RAM expansion"),
      (ui_callback_t)toggle_Drive11RAM4000, NULL, NULL },
    { N_("*$6000-$7FFF RAM expansion"),
      (ui_callback_t)toggle_Drive11RAM6000, NULL, NULL },
    { N_("*$8000-$9FFF RAM expansion"),
      (ui_callback_t)toggle_Drive11RAM8000, NULL, NULL },
    { N_("*$A000-$BFFF RAM expansion"),
      (ui_callback_t)toggle_Drive11RAMA000, NULL, NULL },
    { NULL }
};

ui_menu_entry_t set_drive0_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive1_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive2_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive10ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_extend_image_policy_submenu[] = {
    { N_("*Never extend"), (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_NEVER, NULL },
    { N_("*Ask on extend"), (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ASK, NULL },
    { N_("*Extend on access"), (ui_callback_t)radio_Drive11ExtendImagePolicy,
      (ui_callback_data_t)DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

ui_menu_entry_t set_drive0_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive8IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive1_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive9IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive2_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive10IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

ui_menu_entry_t set_drive3_idle_method_submenu[] = {
    { N_("*No traps"), (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_NO_IDLE, NULL },
    { N_("*Skip cycles"), (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_SKIP_CYCLES, NULL },
    { N_("*Trap idle"), (ui_callback_t)radio_Drive11IdleMethod,
      (ui_callback_data_t)DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

