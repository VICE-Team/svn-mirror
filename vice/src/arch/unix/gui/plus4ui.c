/*
 * plus4ui.c - Implementation of the Plus4-specific part of the UI.
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
#include <stdlib.h>

#include "debug.h"
#include "icon.h"
#include "machine-video.h"
#include "plus4ui.h"
#include "resources.h"
#include "uiattach.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uidriveplus4.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "uiperipheraliec.h"
#include "uiromset.h"
#include "uirs232petplus4cbm2.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisound.h"
#include "uited.h"
#include "vsync.h"


static UI_CALLBACK(save_screenshot)
{
    /* Where does the 1024 come from?  */
    char filename[1024];
    unsigned int wid = (unsigned int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_screenshot_dialog(filename, machine_video_canvas_get(wid)) < 0)
        return;
}

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Save media file"),
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t plus4ui_main_romset_submenu[] = {
    { N_("Load new kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new BASIC ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Load new 3 plus 1 LO ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"3plus1loName", NULL },
    { N_("Load new 3 plus 1 HI ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"3plus1hiName", NULL },
    { NULL }
};

static ui_menu_entry_t plus4_romset_submenu[] = {
    { N_("Load default ROMs"),
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, plus4ui_main_romset_submenu },
    { N_("Load new drive ROM"),
      NULL, NULL, ui_driveplus4_romset_submenu },
    { "--" },
    { N_("ROM set type"),
      NULL, NULL, uiromset_type_submenu },
    { "--" },
    { N_("ROM set archive"),
      NULL, NULL, uiromset_archive_submenu },
    { N_("ROM set file"),
      NULL, NULL, uiromset_file_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(RamSize)
UI_MENU_DEFINE_RADIO(H256K)
UI_MENU_DEFINE_RADIO(CS256K)

ui_menu_entry_t set_ram_submenu[] = {
    { N_("*16KB"), (ui_callback_t)radio_RamSize,
      (ui_callback_data_t)16, NULL },
    { N_("*32KB"), (ui_callback_t)radio_RamSize,
      (ui_callback_data_t)32, NULL },
    { N_("*64KB"), (ui_callback_t)radio_RamSize,
      (ui_callback_data_t)64, NULL },
    { N_("*256KB (CSORY)"), (ui_callback_t)radio_CS256K,
      (ui_callback_data_t)1, NULL },
    { N_("*256KB (HANNES)"), (ui_callback_t)radio_H256K,
      (ui_callback_data_t)1, NULL },
    { N_("*1024KB (HANNES)"), (ui_callback_t)radio_H256K,
      (ui_callback_data_t)2, NULL },
    { N_("*4096KB (HANNES)"), (ui_callback_t)radio_H256K,
      (ui_callback_data_t)3, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t plus4_menu[] = {
    { N_("ROM settings"),
      NULL, NULL, plus4_romset_submenu },
    { N_("RAM settings"),
      NULL, NULL, set_ram_submenu },
    { N_("TED settings"),
      NULL, NULL, ted_submenu },
    { N_("RS232 settings"),
      NULL, NULL, uirs232petplus4cbm2_submenu },
    { NULL }
};

static void plus4ui_dynamic_menu_create(void)
{
    uited_menu_create();
}

static void plus4ui_dynamic_menu_shutdown(void)
{
    uited_menu_shutdown();
}

int plus4ui_init(void)
{
    ui_set_application_icon(plus4_icon_data);
    plus4ui_dynamic_menu_create();
    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    uiattach_disk_menu,
                                    ui_menu_separator,
                                    uiattach_tape_menu,
                                    ui_datasette_commands_menu,
                                    ui_menu_separator,
                                    uiattach_smart_attach_menu,
                                    ui_menu_separator,
                                    ui_directory_commands_menu,
                                    ui_menu_separator,
                                    ui_snapshot_commands_menu,
                                    ui_screenshot_commands_menu,
                                    ui_sound_record_commands_menu,
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
                                     uikeyboard_settings_menu,
                                     ui_sound_settings_menu,
                                     ui_driveplus4_settings_menu,
                                     ui_peripheraliec_plus4_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     plus4_menu,
				     ui_menu_separator,
				     ui_settings_settings_menu,
#ifdef DEBUG
                                     ui_menu_separator,
                                     ui_debug_settings_menu,
#endif
                                     NULL));

    ui_set_topmenu("TopLevelMenu",
                   _("File"),
                   ui_menu_create("File",
                                  uiattach_smart_attach_menu,
                                  ui_menu_separator,
                                  uiattach_disk_menu,
                                  ui_menu_separator,
                                  uiattach_tape_menu,
                                  ui_datasette_commands_menu,
                                  ui_menu_separator,
                                  ui_directory_commands_menu,
                                  ui_menu_separator,
                                  ui_tool_commands_menu,
                                  ui_menu_separator,
                                  ui_run_commands_menu,
                                  ui_menu_separator,
                                  ui_exit_commands_menu,
                                  NULL),
                   _("Snapshot"),
                   ui_menu_create("Snapshot",
                                  ui_snapshot_commands_submenu,
                                  ui_menu_separator,
                                  ui_screenshot_commands_menu,
				  ui_sound_record_commands_menu,
                                  NULL),
                   _("Options"),
                   ui_menu_create("Options",
                                  ui_performance_settings_menu,
                                  ui_menu_separator,
                                  joystick_options_submenu,
                                  ui_menu_separator,
                                  ui_drive_options_submenu,
                                  NULL),
                   _("Settings"),
                   ui_menu_create("Settings",
				  uikeyboard_settings_menu,
				  ui_sound_settings_menu,
				  ui_driveplus4_settings_menu,
				  ui_peripheraliec_plus4_settings_menu,
				  joystick_settings_menu,
                                  ui_menu_separator,
				  plus4_menu,
				  ui_menu_separator,
                                  ui_settings_settings_menu,
                                  NULL),
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
                   _("RJHelp"),
                   ui_menu_create("Help",
                                  ui_help_commands_menu,
                                  NULL),
                   NULL);

    ui_set_speedmenu(ui_menu_create("SpeedMenu",
                                    ui_performance_settings_menu,
                                    ui_menu_separator,
                                    ui_menu_separator,
                                    NULL));
    ui_set_tape_menu(ui_menu_create("TapeMenu",
                                    uiattach_tape_menu,
                                    ui_menu_separator,
                                    datasette_control_submenu,
                                    NULL));

    ui_update_menus();

    return 0;
}

void plus4ui_shutdown(void)
{
    plus4ui_dynamic_menu_shutdown();
}

