/*
 * c610ui.c - Implementation of the PET-specific part of the UI.
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

#define C610UI 1
#include "videoarch.h"

#include "c610mem.h"
#include "datasette.h"
#include "joystick.h"
#include "petui.h"
#include "resources.h"
#include "uicommands.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uimenu.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_STRING_RADIO(CrtcPaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { N_("*Default (Green)"),
      (ui_callback_t) radio_CrtcPaletteFile, (ui_callback_data_t) "default", 
	NULL },
    { N_("*Amber"),
      (ui_callback_t) radio_CrtcPaletteFile, (ui_callback_data_t) "amber", 
	NULL },
    { N_("*White"),
      (ui_callback_t) radio_CrtcPaletteFile, (ui_callback_data_t) "white", 
	NULL },
    { "--" },
    { N_("Load custom"),
      (ui_callback_t) ui_load_palette,
      (ui_callback_data_t) "CrtcPaletteFile", NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t cbm2_romset_submenu[] = {
    { N_("Basic 128k, low chars"),
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom128l.vrs", NULL },
    { N_("Basic 256k, low chars"),
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom256l.vrs", NULL },
    { N_("Basic 128k, high chars"),
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom128h.vrs", NULL },
    { N_("Basic 256k, high chars"),
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom256h.vrs", NULL },
    { "--" },
    { N_("Load new Kernal"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new Chargen"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"ChargenName", NULL },
    { "--" },
    { N_("Load new Basic"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"BasicName", NULL },
    { N_("Unload Basic ROM"),
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"BasicName", NULL },
    { "--" },
    { N_("Load new Cart $1***"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"Cart1Name", NULL },
    { N_("Unload Cart $1***"),
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"Cart1Name", NULL },
    { "--" },
    { N_("Load new Cart $2-3***"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"Cart2Name", NULL },
    { N_("Unload Cart $2-3***"),
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"Cart2Name", NULL },
    { "--" },
    { N_("Load new Cart $4-5***"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"Cart4Name", NULL },
    { N_("Unload Cart $4-5***"),
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"Cart4Name", NULL },
    { "--" },
    { N_("Load new Cart $6-7***"),
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"Cart6Name", NULL },
    { N_("Unload Cart $6-7***"),
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"Cart6Name", NULL },
    { "--" },
    { N_("Load custom ROM set from file"),
      (ui_callback_t) ui_load_romset, NULL, NULL },
    { N_("Dump ROM set definition to file"),
      (ui_callback_t) ui_dump_romset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(ModelLine)

static ui_menu_entry_t c610_modelline_submenu[] = {
    { "*7x0 (50 Hz)",
        (ui_callback_t) radio_ModelLine, (ui_callback_data_t) 0, NULL },
    { "*6x0 60 Hz",
        (ui_callback_t) radio_ModelLine, (ui_callback_data_t) 1, NULL },
    { "*6x0 50 Hz",
        (ui_callback_t) radio_ModelLine, (ui_callback_data_t) 2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RamSize)

static ui_menu_entry_t c610_memsize_submenu[] = {
    { "*128 kByte",
        (ui_callback_t) radio_RamSize, (ui_callback_data_t) 128, NULL },
    { "*256 kByte",
        (ui_callback_t) radio_RamSize, (ui_callback_data_t) 256, NULL },
    { "*512 kByte",
        (ui_callback_t) radio_RamSize, (ui_callback_data_t) 512, NULL },
    { "*1024 kByte",
        (ui_callback_t) radio_RamSize, (ui_callback_data_t) 1024, NULL },
    { NULL }
};

static UI_CALLBACK(ui_set_model)
{
    cbm2_set_model(UI_MENU_CB_PARAM, NULL);
    ui_update_menus();
}

static ui_menu_entry_t model_defaults_submenu[] = {
    { "CBM 610",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"610", NULL },
    { "CBM 620",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"620", NULL },
    { "CBM 620+ (1M)",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"620+", NULL },
    { "CBM 710",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"710", NULL },
    { "CBM 720",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"720", NULL },
    { "CBM 720+ (1M)",
      (ui_callback_t) ui_set_model, (ui_callback_data_t)"720+", NULL },
    { NULL }
};

#if 0

/* this is partially modeled after the radio_* callbacks */
static UI_CALLBACK(set_KeyboardType)
{
    int current_value, new_value = 2 * (int) UI_MENU_CB_PARAM;
    extern char *keymap_file_resource_names[];

    resources_get_value("KeymapIndex", (resource_value_t) &current_value);
    if(!call_data) {
        if((current_value & ~1) != new_value) {
            resources_set_value("KeymapIndex", (resource_value_t)
                (current_value & 1) + new_value);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == new_value);
    }
}

static ui_menu_entry_t c610_keybd_submenu[] = {
    { N_("*Graphics"),
        (ui_callback_t) set_KeyboardType, (ui_callback_data_t) 1, NULL },
    { N_("*Business (UK)"),
        (ui_callback_t) set_KeyboardType, (ui_callback_data_t) 0, NULL },
    { NULL }
};

#endif

UI_MENU_DEFINE_TOGGLE(Ram08)
UI_MENU_DEFINE_TOGGLE(Ram1)
UI_MENU_DEFINE_TOGGLE(Ram2)
UI_MENU_DEFINE_TOGGLE(Ram4)
UI_MENU_DEFINE_TOGGLE(Ram6)
UI_MENU_DEFINE_TOGGLE(RamC)

static ui_menu_entry_t model_settings_submenu[] = {
    { N_("Model defaults"),
      NULL, NULL, model_defaults_submenu },
    { "--" },
    { N_("ROM sets"),
      NULL, NULL, cbm2_romset_submenu },
    { "--" },
    { N_("Memory size"),
      NULL, NULL, c610_memsize_submenu },
    { N_("Hardwired switches"),
      NULL, NULL, c610_modelline_submenu },
    { "--" },
    { N_("*Bank 15 $0800-$0FFF RAM"),
      (ui_callback_t) toggle_Ram08, NULL, NULL },
    { N_("*Bank 15 $1000-$1FFF RAM"),
      (ui_callback_t) toggle_Ram1, NULL, NULL },
    { N_("*Bank 15 $2000-$3FFF RAM"),
      (ui_callback_t) toggle_Ram2, NULL, NULL },
    { N_("*Bank 15 $4000-$5FFF RAM"),
      (ui_callback_t) toggle_Ram4, NULL, NULL },
    { N_("*Bank 15 $6000-$7FFF RAM"),
      (ui_callback_t) toggle_Ram6, NULL, NULL },
    { N_("*Bank 15 $C000-$CFFF RAM"),
      (ui_callback_t) toggle_RamC, NULL, NULL },
#if 0
    { "--" },
    { N_("Keyboard type"),
      NULL, NULL, c610_keybd_submenu },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */
/* FIXME: this is the same for all emulators besides the VIC20 so this
   should better go to uisettings.c, so we have only one copy for 
   all of them, not 4 copies */

static UI_CALLBACK(set_joystick_device_1)
{
    int tmp;

    suspend_speed_eval();
    if (!CHECK_MENUS) {
        resources_set_value("JoyDevice1", (resource_value_t) UI_MENU_CB_PARAM);
        ui_update_menus();
    } else {
        resources_get_value("JoyDevice1", (resource_value_t *) &tmp);
        ui_menu_set_tick(w, tmp == (int) UI_MENU_CB_PARAM);
    }
}

static UI_CALLBACK(set_joystick_device_2)
{
    int tmp;

    suspend_speed_eval();
    if (!CHECK_MENUS) {
        resources_set_value("JoyDevice2", (resource_value_t) UI_MENU_CB_PARAM);
        ui_update_menus();
    } else {
        resources_get_value("JoyDevice2", (resource_value_t *) &tmp);
        ui_menu_set_tick(w, tmp == (int) UI_MENU_CB_PARAM);
    }
}

static UI_CALLBACK(swap_joystick_ports)
{
    int tmp1, tmp2;

    if (w != NULL)
        suspend_speed_eval();
    resources_get_value("JoyDevice1", (resource_value_t *) &tmp1);
    resources_get_value("JoyDevice2", (resource_value_t *) &tmp2);
    resources_set_value("JoyDevice1", (resource_value_t) tmp2);
    resources_set_value("JoyDevice2", (resource_value_t) tmp1);
    ui_update_menus();
}

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { N_("*None"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_NONE, NULL },
    { N_("*Numpad"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_NUMPAD,
 NULL },
    { N_("*Custom Keys"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { N_("*Analog Joystick 0"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_ANALOG_0, NULL },
    { N_("*Analog Joystick 1"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("*Digital Joystick 0"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_DIGITAL_0, NULL },
    { N_("*Digital Joystick 1"),
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_DIGITAL_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t set_joystick_device_2_submenu[] = {
    { N_("*None"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_NONE, NULL },
    { N_("*Numpad"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_NUMPAD,
 NULL },
    { N_("*Custom Keys"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { N_("*Analog Joystick 0"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_ANALOG_0, NULL },
    { N_("*Analog Joystick 1"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("*Digital Joystick 0"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_DIGITAL_0, NULL },
    { N_("*Digital Joystick 1"),
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_DIGITAL_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};

static ui_menu_entry_t joystick_settings_submenu[] = {
    { N_("Joystick device in port 1"),
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick device in port 2"),
      NULL, NULL, set_joystick_device_2_submenu },
    { "--" },
    { N_("Swap joystick ports"),
      (ui_callback_t) swap_joystick_ports, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t joystick_options_submenu[] = {
    { N_("Swap joystick ports"),
      (ui_callback_t) swap_joystick_ports, NULL, NULL, XK_j, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { N_("Joystick settings"),
      NULL, NULL, joystick_settings_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c610_rs232_submenu[] = {
    { N_("ACIA device"),
      NULL, NULL, acia1_device_submenu },
    { "--" },
    { N_("Serial 1 device..."), (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice1", NULL },
    { N_("Serial 1 baudrate"),
      NULL, NULL, ser1_baud_submenu },
    { "--" },
    { N_("Serial 2 device..."), (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice2", NULL },
    { N_("Serial 2 baudrate"),
      NULL, NULL, ser2_baud_submenu },
    { "--" },
    { N_("Dump filename..."), (ui_callback_t) set_rs232_dump_file,
      (ui_callback_data_t) "RsDevice3", NULL },
    { "--" },
    { N_("Program name to exec..."), (ui_callback_t) set_rs232_exec_file,
      (ui_callback_data_t) "RsDevice4", NULL },
    { NULL }
};


/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c610_menu[] = {
    { N_("CBM-II model settings"),
      NULL, NULL, model_settings_submenu },
    { N_("RS232 settings"),
      NULL, NULL, c610_rs232_submenu },
    { "--" },
    { N_("Screen color"),
      NULL, NULL, palette_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(ui_datasette_control)
{
    int command = (int)UI_MENU_CB_PARAM;
    datasette_control(command);
}

static ui_menu_entry_t datasette_control_submenu[] = {
    { N_("Stop"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_STOP, NULL },
    { N_("Play"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_START, NULL },
    { N_("Forward"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_FORWARD, NULL },
    { N_("Rewind"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_REWIND, NULL },
    { N_("Record"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_RECORD, NULL },
    { N_("Reset"), (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_RESET, NULL },
    { NULL }
};

ui_menu_entry_t ui_datasette_commands_menu[] = {
    { N_("Datassette control"),
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(save_screenshot)
{
    /* Where does the 1024 come from?  */
    char filename[1024];
    int wid = (int) UI_MENU_CB_PARAM;

    suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_screenshot_dialog(filename, wid) < 0) 
	return;
}

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Screenshot..."),
      (ui_callback_t)save_screenshot, (ui_callback_data_t) 0, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int c610_ui_init(void)
{
    ui_set_application_icon(icon_data);
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
#ifdef USE_XF86_EXTENSIONS
				     ui_fullscreen_settings_menu,
#endif
                                     ui_keyboard_settings_menu,
                                     ui_sound_settings_menu,
                                     ui_par_drive_settings_menu,
                                     ui_peripheral_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     c610_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
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
				  ui_drive_options_submenu,
				  NULL),
		   _("Settings"),
		   ui_menu_create("Settings",
				  ui_crtc_video_settings_menu,
				  ui_peripheral_settings_menu,
				  ui_drive_settings_menu,
				  ui_keyboard_settings_menu,
				  joystick_settings_menu,
				  ui_sound_settings_menu,
				  ui_par_drive_settings_menu,
				  ui_menu_separator,
				  c610_menu,
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
