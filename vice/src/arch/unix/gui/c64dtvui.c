/*
 * c64dtvui.c - Implementation of the C64DTV-specific part of the UI.
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
#include <string.h>

#include "debug.h"
#include "icon.h"
#include "machine.h"
#include "machine-video.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uic64dtv.h"
#include "uicommands.h"
#include "uidrive.h"
#include "uidrivec64dtv.h"
#include "uidrivec64vic20.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "uiperipheraliec.h"
#include "uips2mouse.h"
#include "uiromset.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uitfe.h"
#include "uivicii.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_video_standard_c64dtv_submenu[] = {
    { N_("*PAL-G"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { N_("*NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(SidFilters)

ui_menu_entry_t sid_submenu[] = {
    { N_("SID engine"),
      NULL, NULL, sid_dtv_engine_submenu },
    { "--" },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_dtv_model_submenu },
#ifdef HAVE_RESID
    { "--" },
    { N_("reSID sampling method"),
      NULL, NULL, sid_resid_sampling_submenu },
    { N_("reSID resampling passband..."),
      (ui_callback_t)set_sid_resid_passband, NULL, NULL },
#endif
    { NULL },
};

UI_MENU_DEFINE_TOGGLE(Sound)

static ui_menu_entry_t sid_options_submenu[] = {
    { N_("SID engine"),
      NULL, NULL, sid_engine_submenu },
    { N_("*Enable sound playback"),
      (ui_callback_t)toggle_Sound, NULL, NULL },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(EmuID)

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
#ifdef HAVE_MOUSE
    { N_("*PS/2 Mouse Emulation"),
      NULL, NULL, ps2_mouse_submenu },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64ui_main_romset_submenu[] = {
    { N_("Load new kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new BASIC ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Load new character ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t c64_romset_submenu[] = {
    { N_("Load default ROMs"),
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, c64ui_main_romset_submenu },
    { N_("Load new drive ROM"),
      NULL, NULL, ui_drivec64vic20_romset_submenu },
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

static UI_CALLBACK(save_screenshot)
{
    /* Where does the 1024 come from?  */
    char filename[1024];
    unsigned int wid = vice_ptr_to_uint(UI_MENU_CB_PARAM);

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

static ui_menu_entry_t c64_menu[] = {
    { N_("ROM settings"),
      NULL, NULL, c64_romset_submenu },
    { N_("VIC-II settings"),
      NULL, NULL, vicii_submenu },
    { N_("SID settings"),
      NULL, NULL, sid_submenu },
    { N_("C64DTV settings"),
      NULL, NULL, c64dtv_submenu },
    { NULL }
};

static ui_menu_entry_t x64_left_menu[] = {
    { "",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_smart_attach_menu },
    { "",
      NULL, NULL, ui_directory_commands_menu },
    { "--",
      NULL, NULL, ui_snapshot_commands_menu },
    { "",
      NULL, NULL, ui_screenshot_commands_menu },
    { "",
      NULL, NULL, ui_sound_record_commands_menu },
    { "--",
      NULL, NULL, ui_tool_commands_menu },
    { "--",
      NULL, NULL, ui_help_commands_menu },
    { "--",
      NULL, NULL, ui_run_commands_menu },
    { "--",
      NULL, NULL, ui_exit_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_right_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec64dtv_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, c64_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--",
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t x64_file_submenu[] = {
    { "",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, ui_directory_commands_menu },
    { "--",
      NULL, NULL, ui_tool_commands_menu },
    { "--",
      NULL, NULL, ui_run_commands_menu },
    { "--",
      NULL, NULL, ui_exit_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_snapshot_submenu[] = {
    { "",
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--",
      NULL, NULL, ui_screenshot_commands_menu },
    { "",
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_options_submenu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, joystick_options_submenu },
    { "--",
      NULL, NULL, sid_options_submenu },
    { "--",
      NULL, NULL, c64dtv_submenu },
    { "--",
      NULL, NULL, ui_drive_options_submenu },
    { "--",
      NULL, NULL, ui_flash_options_submenu },
    { "--",
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t x64_settings_submenu[] = {
    { "",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec64dtv_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, c64_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t x64_main_menu[] = {
    { N_("File"),
      NULL, NULL, x64_file_submenu },
    { N_("Snapshot"),
      NULL, NULL, x64_snapshot_submenu },
    { N_("Options"),
      NULL, NULL, x64_options_submenu },
    { N_("Settings"),
      NULL, NULL, x64_settings_submenu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"),
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_speed_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--" },
    { "--" },
    { NULL }
};

static void c64ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivicii_menu_create();
}

static void c64ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uisound_menu_shutdown();
}

int c64dtvui_init(void)
{
    memcpy(set_video_standard_submenu, 
           set_video_standard_c64dtv_submenu,
           sizeof(set_video_standard_c64dtv_submenu));

    ui_set_application_icon(c64dtv_icon_data);
    c64ui_dynamic_menu_create();

    ui_set_left_menu(x64_left_menu);
    ui_set_right_menu(x64_right_menu);
    ui_set_topmenu(x64_main_menu);
    ui_set_speedmenu(x64_speed_menu);

    ui_update_menus();

    return 0;
}

void c64dtvui_shutdown(void)
{
    c64ui_dynamic_menu_shutdown();
}
