/*
 * c128ui.c - Implementation of the C128-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <string.h>

#include "c128.h"
#include "debug.h"
#include "icon.h"
#include "machine-video.h"
#include "machine.h"
#include "resources.h"
#include "uiattach.h"
#include "uic64cart.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidigimax.h"
#include "uidrive.h"
#include "uidrivec128.h"
#include "uiide64.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimouse.h"
#include "uiperipheraliec.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uireu.h"
#include "uigeoram.h"
#include "uimmc64.h"
#include "uiramcart.h"
#include "uiromset.h"
#include "uirs232c64c128.h"
#include "uisid.h"
#include "uisound.h"
#include "uitfe.h"
#include "uivdc.h"
#include "uivicii.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_video_standard_c128_submenu[] = {
    { N_("*PAL-G"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { N_("*NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_d4xx_submenu[] = {
    { "*$D420", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd420, NULL },
    { "*$D440", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd440, NULL },
    { "*$D460", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd460, NULL },
    { "*$D480", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd480, NULL },
    { "*$D4A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4a0, NULL },
    { "*$D4C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4c0, NULL },
    { "*$D4E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dexx_submenu[] = {
    { "*$DE00", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde00, NULL },
    { "*$DE20", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde20, NULL },
    { "*$DE40", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde40, NULL },
    { "*$DE60", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde60, NULL },
    { "*$DE80", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde80, NULL },
    { "*$DEA0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdea0, NULL },
    { "*$DEC0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdec0, NULL },
    { "*$DEE0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdee0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dfxx_submenu[] = {
    { "*$DF00", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf00, NULL },
    { "*$DF20", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf20, NULL },
    { "*$DF40", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf40, NULL },
    { "*$DF60", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf60, NULL },
    { "*$DF80", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf80, NULL },
    { "*$DFA0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfa0, NULL },
    { "*$DFC0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfc0, NULL },
    { "*$DFE0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfe0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_submenu[] = {
    { "$D4xx",
      NULL, NULL, set_sid_stereo_address_d4xx_submenu },
    { "$DExx",
      NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx",
      NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_TOGGLE(SidFilters)

static ui_menu_entry_t sid_submenu[] = {
    { N_("SID engine"),
      NULL, NULL, sid_engine_submenu },
    { "--" },
    { N_("*Second SID"),
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("*Second SID base address"),
      NULL, NULL, set_sid_stereo_address_submenu },
    { "--" },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_model_submenu },
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
    { N_("*Second SID"),
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("*Second SID base address"),
      NULL, NULL, set_sid_stereo_address_submenu },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(IEEE488)
UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(InternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(ExternalFunctionROM)

UI_CALLBACK(set_function_rom_name)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    ui_button_t button;
    char *filename;
    static char *last_dir;
    uilib_file_filter_enum_t filter[] = { UILIB_FILTER_ALL };

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Function ROM image"), NULL, 0,
                              last_dir, filter, 1, &button,  0, NULL, UI_FC_LOAD);

    if (button == UI_BUTTON_OK && filename != NULL) {
        if (resources_set_string(resname, filename) < 0)
            ui_error(_("Could not load function ROM image\n'%s'"), filename);
    }
}

static ui_menu_entry_t functionrom_submenu[] = {
    { N_("*Enable internal Function ROM"),
      (ui_callback_t)toggle_InternalFunctionROM, NULL, NULL },
    { N_("Internal Function ROM name..."),
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"InternalFunctionName", NULL },
    { "--" },
    { N_("*Enable external Function ROM"),
      (ui_callback_t)toggle_ExternalFunctionROM, NULL, NULL },
    { N_("External Function ROM name..."),
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"ExternalFunctionName", NULL },
    { NULL }
};

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("Function ROM"),
      NULL, NULL, functionrom_submenu },
    { "--" },
    { N_("RAM Expansion Unit"),
      NULL, NULL, reu_submenu },
    { N_("GEORAM Expansion Unit"),
      NULL, NULL, georam_submenu },
    { N_("RAM Expansion Cart"),
      NULL, NULL, ramcart_submenu },
    { N_("MMC64 emulation"),
      NULL, NULL, mmc64_submenu },
    { N_("Digimax Cart"),
      NULL, NULL, digimax_submenu },
    { N_("IDE64 emulation"),
      NULL, NULL, ide64_submenu },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"),
      NULL, NULL, tfe_submenu },
#endif
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
    { N_("*IEEE488 interface emulation"),
      (ui_callback_t)toggle_IEEE488, NULL, NULL },
#ifdef HAVE_MOUSE
    { N_("*Mouse Emulation"),
      NULL, NULL, mouse_submenu },
#endif
#ifdef HAVE_MIDI
    { N_("MIDI Emulation"),
      NULL, NULL, midi_c64_submenu },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c128ui_main_romset_submenu[] = {
    { N_("Load new Int. Kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalIntName", NULL },
    { N_("Load new German Kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalDEName", NULL },
    { N_("Load new Basic Lo ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicLoName", NULL },
    { N_("Load new Basic Hi ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicHiName", NULL },
    { N_("Load new Chargen ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t c128_romset_submenu[] = {
    { N_("Load default ROMs"),
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, c128ui_main_romset_submenu },
    { N_("Load new drive ROM"),
      NULL, NULL, ui_drivec128_romset_submenu },
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
    unsigned int wid = (unsigned int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_screenshot_dialog(filename, machine_video_canvas_get(wid)) < 0)
        return;
}

static ui_menu_entry_t screenshot_submenu[] = {
    { N_("VIC II Save media file"),
      (ui_callback_t)save_screenshot, (ui_callback_data_t)1, NULL },
    { N_("VDC Save media file"),
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Screenshots..."),
      NULL, NULL, screenshot_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(MachineType)


static ui_menu_entry_t machine_type_submenu[] = {
    { "*International", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_INT, NULL },
    { "*Finnish", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_FINNISH, NULL },
    { "*French", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_FRENCH, NULL },
    { "*German", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_GERMAN, NULL },
    { "*Italian", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_ITALIAN, NULL },
    { "*Norwegian", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_NORWEGIAN, NULL },
    { "*Swedish", (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_SWEDISH, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Go64Mode)

ui_menu_entry_t c128_model_submenu[] = {
    { N_("Machine type"),
      NULL, NULL, machine_type_submenu },
    { N_("ROM sets"),
      NULL, NULL, c128_romset_submenu },
    { "--" },
    { N_("*Always switch to C64 mode"),
      (ui_callback_t)toggle_Go64Mode, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c128_menu[] = {
    { N_("VIC-II settings"),
      NULL, NULL, vicii_submenu },
    { N_("VDC settings"),
      NULL, NULL, vdc_submenu },
    { N_("SID settings"),
      NULL, NULL, sid_submenu },
    { N_("Memory and I/O extensions"),
      NULL, NULL, io_extensions_submenu },
    { N_("RS232 settings"),
      NULL, NULL, uirs232c64c128_submenu },
    { N_("Model settings"),
      NULL, NULL, c128_model_submenu },
    { NULL }
};

static ui_menu_entry_t c128_left_menu[] = {
    { "",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, ui_c64cart_commands_menu },
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

static ui_menu_entry_t c128_right_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec128_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, c128_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--",
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t c128_file_menu[] = {
    { "",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, ui_c64cart_commands_menu },
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

static ui_menu_entry_t c128_snapshot_menu[] = {
    { "",
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--",
      NULL, NULL, screenshot_submenu },
    { "",
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t c128_options_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, joystick_options_submenu },
    { "--",
      NULL, NULL, sid_options_submenu },
    { "--",
      NULL, NULL, ui_drive_options_submenu },
    { "--",
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t c128_settings_menu[] = {
    { "",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec128_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "--",
      NULL, NULL, c128_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t c128_main_menu[] = {
    { N_("File"),
      NULL, NULL, c128_file_menu },
    { N_("Snapshot"),
      NULL, NULL, c128_snapshot_menu },
    { N_("Options"),
      NULL, NULL, c128_options_menu },
    { N_("Settings"),
      NULL, NULL, c128_settings_menu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"),
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static ui_menu_entry_t c128_tape_menu[] = {
    { "",
      NULL, NULL, uiattach_tape_menu },
    { "--",
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static void c128ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivicii_menu_create();
    uivdc_menu_create();
}

static void c128ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uivdc_menu_shutdown();
    uisound_menu_shutdown();
}

int c128ui_init(void)
{
    memcpy(set_video_standard_submenu,
           set_video_standard_c128_submenu,
           sizeof(set_video_standard_c128_submenu));

    ui_set_application_icon(c128_icon_data);
    c128ui_dynamic_menu_create();
    ui_set_left_menu(c128_left_menu);

    ui_set_right_menu(c128_right_menu);

    ui_set_topmenu(c128_main_menu);
    ui_set_speedmenu(ui_performance_settings_menu);
    ui_set_tape_menu(c128_tape_menu);
    ui_update_menus();

    return 0;
}

void c128ui_shutdown(void)
{
    c128ui_dynamic_menu_shutdown();
}

