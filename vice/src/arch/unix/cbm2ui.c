/*
 * cbm2ui.c - Implementation of the CBM-II specific part of the UI.
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

#include "cbm2.h"
#include "cbm2mem.h"
#include "debug.h"
#include "icon.h"
#include "machine.h"
#include "resources.h"
#include "uicommands.h"
#include "uicrtc.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uijoystick2.h"
#include "uiperipheral.h"
#include "uirs232.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uimenu.h"
#include "uivicii.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(MachineVideoStandard)

ui_menu_entry_t set_video_standard_submenu[] = {
    { N_("*PAL-G"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { N_("*NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t cbm2_romset_submenu[] = {
    { N_("Basic 128k, low chars"),
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom128l.vrs", NULL },
    { N_("Basic 256k, low chars"),
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom256l.vrs", NULL },
    { N_("Basic 128k, high chars"),
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom128h.vrs", NULL },
    { N_("Basic 256k, high chars"),
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"rom256h.vrs", NULL },
    { "--" },
    { N_("Load new Kernal"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new Chargen"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { "--" },
    { N_("Load new Basic"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Unload Basic ROM"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { "--" },
    { N_("Load new Cart $1***"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Cart1Name", NULL },
    { N_("Unload Cart $1***"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"Cart1Name", NULL },
    { "--" },
    { N_("Load new Cart $2-3***"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Cart2Name", NULL },
    { N_("Unload Cart $2-3***"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"Cart2Name", NULL },
    { "--" },
    { N_("Load new Cart $4-5***"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Cart4Name", NULL },
    { N_("Unload Cart $4-5***"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"Cart4Name", NULL },
    { "--" },
    { N_("Load new Cart $6-7***"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Cart6Name", NULL },
    { N_("Unload Cart $6-7***"),
      (ui_callback_t)ui_unload_rom_file,
      (ui_callback_data_t)"Cart6Name", NULL },
    { "--" },
    { N_("Load custom ROM set from file"),
      (ui_callback_t)ui_load_romset, NULL, NULL },
    { N_("Dump ROM set definition to file"),
      (ui_callback_t)ui_dump_romset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(ModelLine)

static ui_menu_entry_t cbm2_modelline_submenu[] = {
    { "*7x0 (50 Hz)",
        (ui_callback_t)radio_ModelLine, (ui_callback_data_t)0, NULL },
    { "*6x0 60 Hz",
        (ui_callback_t)radio_ModelLine, (ui_callback_data_t)1, NULL },
    { "*6x0 50 Hz",
        (ui_callback_t)radio_ModelLine, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RamSize)

static ui_menu_entry_t cbm2_memsize_submenu[] = {
    { "*128 kByte",
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)128, NULL },
    { "*256 kByte",
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)256, NULL },
    { "*512 kByte",
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)512, NULL },
    { "*1024 kByte",
        (ui_callback_t)radio_RamSize, (ui_callback_data_t)1024, NULL },
    { NULL }
};

static UI_CALLBACK(ui_set_model)
{
    cbm2_set_model(UI_MENU_CB_PARAM, NULL);
    ui_update_menus();
}

static ui_menu_entry_t model_defaults_submenu[] = {
    { "CBM 510",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"510", NULL },
    { "CBM 610",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"610", NULL },
    { "CBM 620",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"620", NULL },
    { "CBM 620+ (1M)",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"620+", NULL },
    { "CBM 710",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"710", NULL },
    { "CBM 720",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"720", NULL },
    { "CBM 720+ (1M)",
      (ui_callback_t)ui_set_model, (ui_callback_data_t)"720+", NULL },
    { NULL }
};

#if 0

/* this is partially modeled after the radio_* callbacks */
static UI_CALLBACK(set_KeyboardType)
{
    int current_value, new_value = 2 * (int)UI_MENU_CB_PARAM;
    extern char *keymap_file_resource_names[];

    resources_get_value("KeymapIndex", (resource_value_t)&current_value);
    if (!call_data) {
        if ((current_value & ~1) != new_value) {
            resources_set_value("KeymapIndex", (resource_value_t)
                (current_value & 1) + new_value);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == new_value);
    }
}

static ui_menu_entry_t cbm2_keybd_submenu[] = {
    { N_("*Graphics"),
        (ui_callback_t)set_KeyboardType, (ui_callback_data_t)1, NULL },
    { N_("*Business (UK)"),
        (ui_callback_t)set_KeyboardType, (ui_callback_data_t)0, NULL },
    { NULL }
};

#endif

UI_MENU_DEFINE_TOGGLE(Ram08)
UI_MENU_DEFINE_TOGGLE(Ram1)
UI_MENU_DEFINE_TOGGLE(Ram2)
UI_MENU_DEFINE_TOGGLE(Ram4)
UI_MENU_DEFINE_TOGGLE(Ram6)
UI_MENU_DEFINE_TOGGLE(RamC)

UI_CALLBACK(Cbm2modelMenu)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, !cbm2_is_c500());
    }
}

static ui_menu_entry_t model_settings_submenu[] = {
    { N_("Model defaults"),
      (ui_callback_t)Cbm2modelMenu, NULL, model_defaults_submenu },
    { "--" },
    { N_("ROM sets"),
      NULL, NULL, cbm2_romset_submenu },
    { "--" },
    { N_("Memory size"),
      NULL, NULL, cbm2_memsize_submenu },
    { N_("Hardwired switches"),
      NULL, NULL, cbm2_modelline_submenu },
    { "--" },
    { N_("*Bank 15 $0800-$0FFF RAM"),
      (ui_callback_t)toggle_Ram08, NULL, NULL },
    { N_("*Bank 15 $1000-$1FFF RAM"),
      (ui_callback_t)toggle_Ram1, NULL, NULL },
    { N_("*Bank 15 $2000-$3FFF RAM"),
      (ui_callback_t)toggle_Ram2, NULL, NULL },
    { N_("*Bank 15 $4000-$5FFF RAM"),
      (ui_callback_t)toggle_Ram4, NULL, NULL },
    { N_("*Bank 15 $6000-$7FFF RAM"),
      (ui_callback_t)toggle_Ram6, NULL, NULL },
    { N_("*Bank 15 $C000-$CFFF RAM"),
      (ui_callback_t)toggle_RamC, NULL, NULL },
#if 0
    { "--" },
    { N_("Keyboard type"),
      NULL, NULL, cbm2_keybd_submenu },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_daxx_submenu[] = {
    { "*$DA20", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xda20, NULL },
    { "*$DA40", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xda40, NULL },
    { "*$DA60", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xda60, NULL },
    { "*$DA80", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xda80, NULL },
    { "*$DAA0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdaa0, NULL },
    { "*$DAC0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdac0, NULL },
    { "*$DAE0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdae0, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_TOGGLE(SidFilters)

#ifdef HAVE_RESID
UI_MENU_DEFINE_TOGGLE(SidUseResid)
#endif

static ui_menu_entry_t sid_submenu[] = {
    { N_("*Second SID"),
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("*Second SID base address"),
      NULL, NULL, set_sid_stereo_address_daxx_submenu },
    { "--" },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_model_submenu },
#ifdef HAVE_RESID
    { "--" },
    { N_("*Use reSID emulation"),
      (ui_callback_t)toggle_SidUseResid, NULL, NULL },
    { N_("reSID sampling method"),
      NULL, NULL, sid_resid_sampling_submenu },
    { N_("reSID resampling passband..."),
      (ui_callback_t)set_sid_resid_passband, NULL, NULL },
#endif
    { NULL },
};

UI_MENU_DEFINE_TOGGLE(Sound)

static ui_menu_entry_t sid_options_submenu[] = {
    { N_("*Enable sound playback"),
      (ui_callback_t)toggle_Sound, NULL, NULL },
    { N_("*Second SID"),
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("*Second SID base address"),
      NULL, NULL, set_sid_stereo_address_daxx_submenu },
    { "--" },
#ifdef HAVE_RESID
    { N_("*Use reSID emulation"),
      (ui_callback_t)toggle_SidUseResid, NULL, NULL },
#endif
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("Chip model"),
      NULL, NULL, sid_model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t cbm2_rs232_submenu[] = {
    { N_("ACIA device"),
      NULL, NULL, acia1_device_submenu },
    { "--" },
    { N_("Serial 1 device..."), (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"),
      NULL, NULL, ser1_baud_submenu },
    { "--" },
    { N_("Serial 2 device..."), (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"),
      NULL, NULL, ser2_baud_submenu },
    { "--" },
    { N_("Dump filename..."), (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--" },
    { N_("Program name to exec..."), (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(CrtcMenu)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, !cbm2_is_c500());
    }
}

static UI_CALLBACK(VicMenu)
{
    if (CHECK_MENUS) {
        ui_menu_set_sensitive(w, cbm2_is_c500());
    }
}

static ui_menu_entry_t cbm2_menu[] = {
    { N_("CBM-II model settings"),
      NULL, NULL, model_settings_submenu },
    { N_("RS232 settings"),
      NULL, NULL, cbm2_rs232_submenu },
    { "--" },
    { N_("*CRTC Screen color"),
      (ui_callback_t)CrtcMenu, NULL, crtc_palette_submenu },
    { N_("*VIC-II settings"),
      (ui_callback_t)VicMenu, NULL, vic_submenu },
    { N_("SID settings"),
      NULL, NULL, sid_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(save_screenshot)
{
    /* Where does the 1024 come from?  */
    char filename[1024];
    int wid = (int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_screenshot_dialog(filename, wid) < 0)
        return;
}

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Screenshot..."),
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int cbm2_ui_init(void)
{
    ui_set_application_icon(cbm2_icon_data);
    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    ui_disk_commands_menu,
                                    ui_menu_separator,
                                    ui_tape_commands_menu,
                                    ui_datasette_commands_menu,
                                    ui_menu_separator,
                                    ui_directory_commands_menu,
                                    ui_menu_separator,
                                    ui_snapshot_commands_menu,
                                    ui_screenshot_commands_menu,
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
                                     ui_crtc_video_settings_menu,
                                     ui_video_settings_menu,
#ifdef USE_XF86_EXTENSIONS
                                     ui_fullscreen_settings_menu,
#endif
                                     ui_keyboard_settings_menu,
                                     ui_sound_settings_menu,
                                     ui_par_drive_settings_menu,
                                     ui_peripheral_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     cbm2_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
#ifdef DEBUG
                                     ui_menu_separator,
                                     ui_debug_settings_menu,
#endif
                                     NULL));

    ui_set_tape_menu(ui_menu_create("TapeMenu",
                                    ui_tape_commands_menu,
                                    ui_menu_separator,
                                    datasette_control_submenu,
                                    NULL));
    ui_set_topmenu("TopLevelMenu",
                   _("File"),
                   ui_menu_create("File",
                                  ui_smart_attach_commands_menu,
                                  ui_menu_separator,
                                  ui_disk_commands_menu,
                                  ui_menu_separator,
                                  ui_tape_commands_menu,
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
                                  NULL),
                   _("Options"),
                   ui_menu_create("Options",
                                  ui_performance_settings_menu,
                                  ui_menu_separator,
#ifdef USE_XF86_EXTENSIONS
                                  ui_fullscreen_settings_menu,
                                  ui_menu_separator,
#endif
                                  joystick_options_submenu,
                                  ui_menu_separator,
                                  sid_options_submenu,
                                  ui_menu_separator,
                                  ui_drive_options_submenu,
                                  NULL),
                   _("Settings"),
                   ui_menu_create("Settings",
                                  ui_crtc_video_settings_menu,
                                  ui_video_settings_menu,
                                  ui_peripheral_settings_menu,
                                  ui_drive_settings_menu,
                                  ui_keyboard_settings_menu,
                                  joystick_settings_menu,
                                  ui_sound_settings_menu,
                                  ui_par_drive_settings_menu,
                                  ui_menu_separator,
                                  cbm2_menu,
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
                                    NULL));
    ui_update_menus();

    return 0;
}

