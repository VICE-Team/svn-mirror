/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "uimenu.h"
#include "resources.h"
#include "uicommands.h"
#include "uisettings.h"

UI_MENU_DEFINE_TOGGLE(CheckSsColl)
UI_MENU_DEFINE_TOGGLE(CheckSbColl)

static ui_menu_entry_t vic_submenu[] = {
    { "Sprite-sprite collisions",
      (ui_callback_t) toggle_CheckSsColl, NULL, NULL },
    { "Sprite-background collisions",
      (ui_callback_t) toggle_CheckSsColl, NULL, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidModel)

static ui_menu_entry_t sid_model_submenu[] = {
    { "*6581 (old)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 0, NULL },
    { "*8580 (new)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 1, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidFilters)

static ui_menu_entry_t sid_submenu[] = {
    { "*Emulate filters",
      (ui_callback_t) toggle_SidFilters, NULL, NULL },
    { "Chip model",
      NULL, NULL, sid_model_submenu },
    { NULL },
};

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(IEEE488)
UI_MENU_DEFINE_TOGGLE(REU)
UI_MENU_DEFINE_TOGGLE(ActionReplay)

static ui_menu_entry_t io_extensions_submenu[] = {
    { "*Emulation identification",
      (ui_callback_t) toggle_EmuID, NULL, NULL },
    { "*IEEE488 interface emulation",
      (ui_callback_t) toggle_IEEE488, NULL, NULL },
    { "*512K RAM Expansion Unit",
      (ui_callback_t) toggle_REU, NULL, NULL },
    { "*Action Replay Cartridge",
      (ui_callback_t) toggle_ActionReplay, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t c64_menu[] = {
    { "VIC-II settings",
      NULL, NULL, vic_submenu },
    { "SID settings",
      NULL, NULL, sid_submenu },
    { "I/O extensions at $DFxx",
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

int c64_ui_init(void)
{
    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    ui_disk_commands_menu,
                                    ui_menu_separator,
                                    ui_tape_commands_menu,
                                    ui_menu_separator,
                                    ui_smart_attach_commands_menu,
                                    ui_menu_separator,
                                    ui_directory_commands_menu,
                                    ui_menu_separator,
                                    ui_tool_commands_menu,
                                    ui_menu_separator,
                                    ui_help_commands_menu,
                                    ui_menu_separator,
                                    ui_run_commands_menu,
                                    ui_menu_separator,
                                    ui_exit_commands_menu,
                                    NULL));

    ui_set_right_menu(ui_menu_create("RightMenu",
                                     ui_performance_settings_menu,
                                     ui_menu_separator,
                                     /* ui_joystick_settings_menu, */
                                     ui_video_settings_menu,
                                     /* ui_keyboard_settings_menu, */
                                     ui_sound_settings_menu,
                                     ui_true1541_settings_menu,
                                     ui_serial_settings_menu,
                                     ui_menu_separator,
                                     c64_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
                                     NULL));

    ui_update_menus();

    return 0;
}
