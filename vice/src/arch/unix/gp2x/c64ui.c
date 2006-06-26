/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "debug.h"
#include "icon.h"
#include "machine.h"
#include "machine-video.h"
#include "resources.h"
#include "uiattach.h"
#include "uic64_256k.h"
#include "uic64cart.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uidrivec64vic20.h"
#include "uigeoram.h"
#include "uiide64.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "uiperipheraliec.h"
#include "uiplus256k.h"
#include "uiplus60k.h"
#include "uiramcart.h"
#include "uireu.h"
#include "uiromset.h"
#include "uirs232c64c128.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uivicii.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(MachineVideoStandard)

ui_menu_entry_t set_video_standard_submenu[] = {
    { N_("*PAL-G"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { N_("*NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { N_("*Old NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSCOLD, NULL },
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

static ui_menu_entry_t set_sid_stereo_address_d5xx_submenu[] = {
    { "*$D500", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd500, NULL },
    { "*$D520", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd520, NULL },
    { "*$D540", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd540, NULL },
    { "*$D560", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd560, NULL },
    { "*$D580", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd580, NULL },
    { "*$D5A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5a0, NULL },
    { "*$D5C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5c0, NULL },
    { "*$D5E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d6xx_submenu[] = {
    { "*$D600", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd600, NULL },
    { "*$D620", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd620, NULL },
    { "*$D640", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd640, NULL },
    { "*$D660", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd660, NULL },
    { "*$D680", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd680, NULL },
    { "*$D6A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6a0, NULL },
    { "*$D6C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6c0, NULL },
    { "*$D6E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d7xx_submenu[] = {
    { "*$D700", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd700, NULL },
    { "*$D720", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd720, NULL },
    { "*$D740", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd740, NULL },
    { "*$D760", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd760, NULL },
    { "*$D780", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd780, NULL },
    { "*$D7A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7a0, NULL },
    { "*$D7C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7c0, NULL },
    { "*$D7E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7e0, NULL },
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
    { "$D5xx",
      NULL, NULL, set_sid_stereo_address_d5xx_submenu },
    { "$D6xx",
      NULL, NULL, set_sid_stereo_address_d6xx_submenu },
    { "$D7xx",
      NULL, NULL, set_sid_stereo_address_d7xx_submenu },
    { "$DExx",
      NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx",
      NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_TOGGLE(SidFilters)

ui_menu_entry_t sid_submenu[] = {
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

UI_MENU_DEFINE_TOGGLE(EmuID)
#ifdef HAVE_MOUSE
UI_MENU_DEFINE_TOGGLE(Mouse)
#endif

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("RAM Expansion Unit"),
      NULL, NULL, reu_submenu },
    { N_("GEORAM Expansion Unit"),
      NULL, NULL, georam_submenu },
    { N_("RAM Expansion Cart"),
      NULL, NULL, ramcart_submenu },
    { N_("PLUS60K RAM Expansion"),
      NULL, NULL, plus60k_submenu },
    { N_("PLUS256K RAM Expansion"),
      NULL, NULL, plus256k_submenu },
    { N_("256K RAM Expansion"),
      NULL, NULL, c64_256k_submenu },
    { N_("IDE64 emulation"),
      NULL, NULL, ide64_submenu },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"),
      NULL, NULL, tfe_submenu },
#endif
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
#ifdef HAVE_MOUSE
    { N_("*1351 Mouse Emulation"),
      (ui_callback_t)toggle_Mouse, NULL, NULL, XK_m, UI_HOTMOD_META },
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

static ui_menu_entry_t c64_menu[] = {
    { N_("ROM settings"),
      NULL, NULL, c64_romset_submenu },
    { N_("VIC-II settings"),
      NULL, NULL, vicii_submenu },
    { N_("SID settings"),
      NULL, NULL, sid_submenu },
    { N_("I/O extensions"),
      NULL, NULL, io_extensions_submenu },
    { N_("RS232 settings"),
      NULL, NULL, uirs232c64c128_submenu },
    { NULL }
};

static void c64ui_dynamic_menu_create(void)
{
    uivicii_menu_create();
}

static void c64ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
}

int c64ui_init(void)
{
    ui_set_application_icon(c64_icon_data);
    c64ui_dynamic_menu_create();
    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    uiattach_disk_menu,
                                    ui_menu_separator,
                                    uiattach_tape_menu,
                                    ui_datasette_commands_menu,
                                    ui_menu_separator,
                                    uiattach_smart_attach_menu,
                                    ui_menu_separator,
                                    ui_c64cart_commands_menu,
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
                                     ui_drivec64vic20_settings_menu,
                                     ui_peripheraliec_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     c64_menu,
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
                                  ui_c64cart_commands_menu,
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
                                  sid_options_submenu,
                                  ui_menu_separator,
                                  ui_drive_options_submenu,
                                  ui_menu_separator,
                                  io_extensions_submenu,
                                  NULL),
                   _("Settings"),
                   ui_menu_create("Settings",
                                  uikeyboard_settings_menu,
                                  ui_sound_settings_menu,
                                  ui_drivec64vic20_settings_menu,
                                  ui_peripheraliec_settings_menu,
                                  joystick_settings_menu,
                                  ui_menu_separator,
                                  c64_menu,
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

void c64ui_shutdown(void)
{
    c64ui_dynamic_menu_shutdown();
}

