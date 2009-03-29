/*
 * petui.c - Implementation of the PET-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
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
#include "machine-video.h"
#include "pets.h"
#include "petui.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uicommands.h"
#include "uicrtc.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uidrivepetcbm2.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "uiperipheralieee.h"
#include "uipetreu.h"
#include "uiromset.h"
#include "uirs232petplus4cbm2.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisidcart.h"
#include "uisound.h"
#include "vsync.h"


UI_MENU_DEFINE_TOGGLE(DiagPin)
UI_MENU_DEFINE_TOGGLE(Crtc)
UI_MENU_DEFINE_TOGGLE(Ram9)
UI_MENU_DEFINE_TOGGLE(RamA)
UI_MENU_DEFINE_RADIO(VideoSize)
UI_MENU_DEFINE_RADIO(RamSize)
UI_MENU_DEFINE_RADIO(IOSize)
UI_MENU_DEFINE_TOGGLE(Basic1)
UI_MENU_DEFINE_TOGGLE(Basic1Chars)

static UI_CALLBACK(petui_set_model)
{
    pet_set_model(UI_MENU_CB_PARAM, NULL);
    ui_update_menus();
}

/* this is partially modeled after the radio_* callbacks */
static UI_CALLBACK(set_KeyboardType)
{
    int current_value, new_value = 2 * vice_ptr_to_int(UI_MENU_CB_PARAM);

    resources_get_int("KeymapIndex", &current_value);
    if (!CHECK_MENUS) {
        if ((current_value & ~1) != new_value) {
            resources_set_int("KeymapIndex", (current_value & 1) + new_value);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == new_value);
    }
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(SidCart)
UI_MENU_DEFINE_TOGGLE(SidFilters)

UI_MENU_DEFINE_RADIO(SidAddress)

static ui_menu_entry_t sidcart_address_submenu[] = {
    { "*$8F00",
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)0, NULL },
    { "*$E900",
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidClock)

static ui_menu_entry_t sidcart_clock_submenu[] = {
    { "*C64", (ui_callback_t)radio_SidClock,
      (ui_callback_data_t)0, NULL },
    { "*PET", (ui_callback_t)radio_SidClock,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static ui_menu_entry_t sidcart_submenu[] = {
    { N_("*Enable SID cart"),
      (ui_callback_t)toggle_SidCart, NULL, NULL },
    { N_("SID engine"),
      NULL, NULL, sidcart_engine_submenu },
    { N_("Chip model"),
      NULL, NULL, sidcart_model_submenu },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("SID address"),
      NULL, NULL, sidcart_address_submenu },
    { N_("SID clock"),
      NULL, NULL, sidcart_clock_submenu },
    { NULL }
};

static ui_menu_entry_t pet_memsize_submenu[] = {
    { N_("*4 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)4, NULL },
    { N_("*8 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)8, NULL },
    { N_("*16 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)16, NULL },
    { N_("*32 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)32, NULL },
    { N_("*96 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)96, NULL },
    { N_("*128 kByte"),
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)128, NULL },
    { NULL }
};

static ui_menu_entry_t pet_iosize_submenu[] = {
    { N_("*2 kByte"),
        (ui_callback_t)radio_IOSize, (ui_callback_data_t)0x800, NULL },
    { N_("*256 Byte"),
        (ui_callback_t)radio_IOSize, (ui_callback_data_t)0x100, NULL },
    { NULL }
};

static ui_menu_entry_t pet_keybd_submenu[] = {
    { N_("*Graphics"),
        (ui_callback_t)set_KeyboardType, (ui_callback_data_t)1, NULL },
    { N_("*Business (UK)"),
        (ui_callback_t)set_KeyboardType, (ui_callback_data_t)0, NULL },
    { N_("*Business (DE)"),
        (ui_callback_t)set_KeyboardType, (ui_callback_data_t)2, NULL },
    { NULL }
};

static ui_menu_entry_t pet_video_submenu[] = {
    { N_("*Auto (from ROM)"),
        (ui_callback_t)radio_VideoSize, (ui_callback_data_t)0, NULL },
    { N_("*40 Columns"),
        (ui_callback_t)radio_VideoSize, (ui_callback_data_t)40, NULL },
    { N_("*80 Columns"),
        (ui_callback_t)radio_VideoSize, (ui_callback_data_t)80, NULL },
    { NULL }
};

static ui_menu_entry_t model_defaults_submenu[] = {
    { "PET 2001-8N",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"2001", NULL },
    { "PET 3008",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"3008", NULL },
    { "PET 3016",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"3016", NULL },
    { "PET 3032",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"3032", NULL },
    { "PET 3032B",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"3032B", NULL },
    { "PET 4016",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"4016", NULL },
    { "PET 4032",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"4032", NULL },
    { "PET 4032B",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"4032B", NULL },
    { "PET 8032",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"8032", NULL },
    { "PET 8096",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"8096", NULL },
    { "PET 8296",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"8296", NULL },
    { "SuperPET",
      (ui_callback_t)petui_set_model, (ui_callback_data_t)"SuperPET", NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(ChargenName)

static ui_menu_entry_t petui_main_romset_submenu[] = {
    { N_("Load new kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new editor ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"EditorName", NULL },
    { N_("Load new BASIC ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(EmuID)

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("PET RAM and Expansion Unit"),
      NULL, NULL, petreu_submenu },
    { N_("SID cartridge"),
      NULL, NULL, sidcart_submenu },
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t pet_romset_submenu[] = {
    { "Basic 1",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom1g.vrs", NULL },
    { "Basic 2, graphics",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom2g.vrs", NULL },
    { "Basic 2, Business",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom2b.vrs", NULL },
    { "Basic 4, 40 cols, graphics",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom4g40.vrs", NULL },
    { "Basic 4, 40 cols, business",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom4b40.vrs", NULL },
    { "Basic 4, 80 cols, business",
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom4b80.vrs", NULL },
    { "--" },
    { "*Basic 1 Patch (if loaded)",
      (ui_callback_t)toggle_Basic1, NULL, NULL },
    { "*Basic 1 character set",
      (ui_callback_t)toggle_Basic1Chars, NULL, NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, petui_main_romset_submenu },
    { N_("Load new drive ROM"),
      NULL, NULL, ui_drivepetcbm2_romset_submenu },
    { "--" },
    { N_("ROM set type"),
      NULL, NULL, uiromset_type_submenu },
    { "--" },
    { N_("Load new character ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { N_("*Original character set"),
      (ui_callback_t)radio_ChargenName,
      (ui_callback_data_t)"chargen", NULL },
    { N_("*German character set"),
      (ui_callback_t)radio_ChargenName,
      (ui_callback_data_t)"chargen.de", NULL },
    { "--" },
    { N_("Load new $9*** ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"RomModule9Name", NULL },
    { N_("Unload $9*** ROM"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"RomModule9Name", NULL },
    { "--" },
    { N_("Load new $A*** ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"RomModuleAName", NULL },
    { N_("Unload $A*** ROM"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"RomModuleAName", NULL },
    { "--" },
    { N_("Load new $B*** ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"RomModuleBName", NULL },
    { N_("Unload $B*** ROM"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"RomModuleBName", NULL },
    { "--" },
    { N_("ROM set archive"),
      NULL, NULL, uiromset_archive_submenu },
    { N_("ROM set file"),
      NULL, NULL, uiromset_file_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SuperPET)

static ui_menu_entry_t model_settings_submenu[] = {
    { N_("Model defaults"),
      NULL, NULL, model_defaults_submenu },
    { "--" },
    { N_("ROM sets"),
      NULL, NULL, pet_romset_submenu },
    { "--" },
    { N_("Video size"),
      NULL, NULL, pet_video_submenu },
    { N_("Memory size"),
      NULL, NULL, pet_memsize_submenu },
    { N_("I/O size"),
      NULL, NULL, pet_iosize_submenu },
    { N_("*CRTC chip enable"),
      (ui_callback_t)toggle_Crtc, NULL, NULL },
    { "--" },
    { N_("*SuperPET I/O enable (disables 8x96)"),
      (ui_callback_t)toggle_SuperPET, NULL, NULL },
    { N_("SuperPET ACIA"),
      NULL, NULL, uirs232petplus4cbm2_submenu },
    { "--" },
    { N_("*$9*** as RAM (8296 only)"),
      (ui_callback_t)toggle_Ram9, NULL, NULL },
    { N_("*$A*** as RAM (8296 only)"),
      (ui_callback_t)toggle_RamA, NULL, NULL },
    { "--" },
    { N_("Keyboard type"),
      NULL, NULL, pet_keybd_submenu },
    { NULL }
};

static ui_menu_entry_t pet_menu[] = {
    { N_("PET model settings"),
      NULL, NULL, model_settings_submenu },
    { N_("*PET userport diagnostic pin"),
      (ui_callback_t)toggle_DiagPin, NULL, NULL },
    { "--" },
    { N_("I/O extensions"),
      NULL, NULL, io_extensions_submenu },
    { "--" },
    { N_("Crtc settings"),
      NULL, NULL, crtc_submenu },
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

static ui_menu_entry_t petui_left_menu[] = {
    { "",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
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

static ui_menu_entry_t petui_right_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivepetcbm2_settings_menu },
    { "",
      NULL, NULL, ui_peripheralieee_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, pet_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--",
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t petui_tape_menu[] = {
    { "",
      NULL, NULL, uiattach_tape_menu },
    { "--",
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static ui_menu_entry_t petui_file_menu[] = {
    { "",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
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

static ui_menu_entry_t petui_snapshot_menu[] = {
    { "",
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--",
      NULL, NULL, ui_screenshot_commands_menu },
    { "",
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t petui_options_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, joystick_options_submenu },
    { "--",
      NULL, NULL, ui_drive_options_submenu },
    { "--",
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t petui_settings_menu[] = {
    { "",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivepetcbm2_settings_menu },
    { "",
      NULL, NULL, ui_peripheralieee_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, pet_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t petui_top_menu[] = {
    { N_("File"),
      NULL, NULL, petui_file_menu },
    { N_("Snapshot"),
      NULL, NULL, petui_snapshot_menu },
    { N_("Options"),
      NULL, NULL, petui_options_menu },
    { N_("Settings"),
      NULL, NULL, petui_settings_menu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"),
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static void petui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uicrtc_menu_create();
}

static void petui_dynamic_menu_shutdown(void)
{
    uicrtc_menu_shutdown();
    uisound_menu_shutdown();
}

int petui_init(void)
{
    ui_set_application_icon(pet_icon_data);
    petui_dynamic_menu_create();
    ui_set_left_menu(petui_left_menu);

    ui_set_right_menu(petui_right_menu);

    ui_set_tape_menu(petui_tape_menu);
    ui_set_topmenu(petui_top_menu);
    ui_set_speedmenu(ui_performance_settings_menu);
    ui_update_menus();
    /* ui_toggle_drive_status(0); */

    return 0;
}

void petui_shutdown(void)
{
    petui_dynamic_menu_shutdown();
}

