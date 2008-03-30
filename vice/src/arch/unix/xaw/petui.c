/*
 * petui.c - Implementation of the PET-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (a.fachat@physik.tu-chemnitz.de)
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

#include "datasette.h"
#include "joystick.h"
#include "pets.h"
#include "petui.h"
#include "resources.h"
#include "uicommands.h"
#include "uimenu.h"
#include "uisettings.h"
#include "vsync.h"

#ifdef XPM
#include <X11/xpm.h>
#include "peticon.xpm"
#endif

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_STRING_RADIO(CrtcPaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { "*Default (Green)", (ui_callback_t) radio_CrtcPaletteFile, 
	(ui_callback_data_t) "default", NULL },
    { "*Amber", (ui_callback_t) radio_CrtcPaletteFile, 
	(ui_callback_data_t) "amber", NULL },
    { "*White", (ui_callback_t) radio_CrtcPaletteFile, 
	(ui_callback_data_t) "white", NULL },
    { "--" },
    { "Load custom",
      (ui_callback_t) ui_load_palette, NULL, NULL },
    { NULL }
};

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
    pet_set_model(client_data, NULL);
    ui_update_menus();
}

/* this is partially modeled after the radio_* callbacks */
static UI_CALLBACK(set_KeyboardType)
{
    int current_value, new_value = 2 * (int) client_data;

    resources_get_value("KeymapIndex", (resource_value_t*) &current_value);
    if(!call_data) {
	if((current_value & ~1) != new_value) {
	    resources_set_value("KeymapIndex",
                                (resource_value_t) ((current_value & 1)
                                                    + new_value));
	    ui_update_menus();
	}
    } else {
	ui_menu_set_tick(w, current_value == new_value);
    }
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(set_joystick_device_1)
{
    int tmp;

    suspend_speed_eval();
    if (!call_data) {
        resources_set_value("JoyDevice1", (resource_value_t) client_data);
	ui_update_menus();
    } else {
        resources_get_value("JoyDevice1", (resource_value_t *) &tmp);
	ui_menu_set_tick(w, tmp == (int) client_data);
    }
}

static UI_CALLBACK(set_joystick_device_2)
{
    int tmp;

    suspend_speed_eval();
    if (!call_data) {
        resources_set_value("JoyDevice2", (resource_value_t) client_data);
	ui_update_menus();
    } else {
        resources_get_value("JoyDevice2", (resource_value_t *) &tmp);
	ui_menu_set_tick(w, tmp == (int) client_data);
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
    { "*None",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_NONE, NULL },
    { "*Numpad",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_NUMPAD, NULL },
    { "*Custom Keys",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { "*Analog Joystick 0",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_ANALOG_0, NULL },
    { "*Analog Joystick 1",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { "*Digital Joystick 0",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_DIGITAL_0, NULL },
    { "*Digital Joystick 1",
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_DIGITAL_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t set_joystick_device_2_submenu[] = {
    { "*None",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_NONE, NULL },
    { "*Numpad",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_NUMPAD, NULL },
    { "*Custom Keys",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { "*Analog Joystick 0",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_ANALOG_0, NULL },
    { "*Analog Joystick 1",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { "*Digital Joystick 0",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_DIGITAL_0, NULL },
    { "*Digital Joystick 1",
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_DIGITAL_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};

static ui_menu_entry_t joystick_settings_submenu[] = {
    { "Joystick device in port 1",
      NULL, NULL, set_joystick_device_1_submenu },
    { "Joystick device in port 2",
      NULL, NULL, set_joystick_device_2_submenu },
    { "--" },
    { "Swap joystick ports",
      (ui_callback_t) swap_joystick_ports, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { "Joystick settings",
      NULL, NULL, joystick_settings_submenu },
    { NULL }
};

static ui_menu_entry_t pet_memsize_submenu[] = {
    { "*4 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 4, NULL },
    { "*8 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 8, NULL },
    { "*16 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 16, NULL },
    { "*32 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 32, NULL },
    { "*96 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 96, NULL },
    { "*128 kByte",
	(ui_callback_t) radio_RamSize, (ui_callback_data_t) 128, NULL },
    { NULL }
};

static ui_menu_entry_t pet_iosize_submenu[] = {
    { "*2 kByte",
	(ui_callback_t) radio_IOSize, (ui_callback_data_t) 0x800, NULL },
    { "*256 Byte",
	(ui_callback_t) radio_IOSize, (ui_callback_data_t) 0x100, NULL },
    { NULL }
};

static ui_menu_entry_t pet_keybd_submenu[] = {
    { "*Graphics",
	(ui_callback_t) set_KeyboardType, (ui_callback_data_t) 1, NULL },
    { "*Business (UK)",
	(ui_callback_t) set_KeyboardType, (ui_callback_data_t) 0, NULL },
    { "*Business (DE)",
	(ui_callback_t) set_KeyboardType, (ui_callback_data_t) 2, NULL },
    { NULL }
};

static ui_menu_entry_t pet_video_submenu[] = {
    { "*Auto (from ROM)",
	(ui_callback_t) radio_VideoSize, (ui_callback_data_t) 0, NULL },
    { "*40 Columns",
	(ui_callback_t) radio_VideoSize, (ui_callback_data_t) 40, NULL },
    { "*80 Columns",
	(ui_callback_t) radio_VideoSize, (ui_callback_data_t) 80, NULL },
    { NULL }
};

static ui_menu_entry_t model_defaults_submenu[] = {
    { "PET 2001-8N",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"2001", NULL },
    { "PET 3008",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"3008", NULL },
    { "PET 3016",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"3016", NULL },
    { "PET 3032",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"3032", NULL },
    { "PET 3032B",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"3032B", NULL },
    { "PET 4016",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"4016", NULL },
    { "PET 4032",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"4032", NULL },
    { "PET 4032B",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"4032B", NULL },
    { "PET 8032",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"8032", NULL },
    { "PET 8096",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"8096", NULL },
    { "PET 8296",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"8296", NULL },
    { "SuperPET",
      (ui_callback_t) petui_set_model, (ui_callback_data_t)"SuperPET", NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(ChargenName)

static ui_menu_entry_t pet_romset_submenu[] = {
    { "Basic 1",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom1g.vrs", NULL },
    { "Basic 2, graphics",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom2g.vrs", NULL },
    { "Basic 2, Business",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom2b.vrs", NULL },
    { "Basic 4, 40 cols, graphics",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom4g40.vrs", NULL },
    { "Basic 4, 40 cols, business",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom4b40.vrs", NULL },
    { "Basic 4, 80 cols, business",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"rom4b80.vrs", NULL },
    { "--" },
    { "*Basic 1 Patch (if loaded)", 
      (ui_callback_t) toggle_Basic1, NULL, NULL },
    { "*Basic 1 character set", 
      (ui_callback_t) toggle_Basic1Chars, NULL, NULL },
    { "--" },
    { "Load new kernal ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"KernalName", NULL },
    { "Load new editor ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"EditorName", NULL },
    { "Load new BASIC ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"BasicName", NULL },
    { "--" },
    { "Load new character ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"ChargenName", NULL },
    { "*Original character set", 
      (ui_callback_t) radio_ChargenName, (ui_callback_data_t)"chargen", NULL },
    { "*German character set", 
      (ui_callback_t) radio_ChargenName, (ui_callback_data_t)"chargen.de", NULL },
    { "--" },
    { "Load new $9*** ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"RomModule9Name", NULL },
    { "Unload $9*** ROM",
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"RomModule9Name", NULL },
    { "--" },
    { "Load new $A*** ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"RomModuleAName", NULL },
    { "Unload $A*** ROM",
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"RomModuleAName", NULL },
    { "--" },
    { "Load new $B*** ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"RomModuleBName", NULL },
    { "Unload $B*** ROM",
      (ui_callback_t) ui_unload_rom_file, (ui_callback_data_t)"RomModuleBName", NULL },
    { "--" },
    { "Load custom ROM set from file",
      (ui_callback_t) ui_load_romset, NULL, NULL },
    { "Dump ROM set definition to file",
      (ui_callback_t) ui_dump_romset, NULL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(SuperPET)

static ui_menu_entry_t pet_rs232_submenu[] = {
    { "ACIA device",
      NULL, NULL, acia1_device_submenu },
    { "--" },
    { "Serial 1 device...", (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice1", NULL },
    { "Serial 1 baudrate",
      NULL, NULL, ser1_baud_submenu },
    { "--" },
    { "Serial 2 device...", (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice2", NULL },
    { "Serial 2 baudrate",
      NULL, NULL, ser2_baud_submenu },
    { "--" },
    { "Dump filename...", (ui_callback_t) set_rs232_dump_file,
      (ui_callback_data_t) "RsDevice3", NULL },
    { "--" },
    { "Program name to exec...", (ui_callback_t) set_rs232_exec_file,
      (ui_callback_data_t) "RsDevice4", NULL },
    { NULL }
};

static ui_menu_entry_t model_settings_submenu[] = {
    { "Model defaults",
      NULL, NULL, model_defaults_submenu },
    { "--" },
    { "ROM sets",
      NULL, NULL, pet_romset_submenu },
    { "--" },
    { "Video size",
      NULL, NULL, pet_video_submenu },
    { "Memory size",
      NULL, NULL, pet_memsize_submenu },
    { "I/O size",
      NULL, NULL, pet_iosize_submenu },
    { "*CRTC chip enable",
      (ui_callback_t) toggle_Crtc, NULL, NULL },
    { "--" },
    { "*SuperPET I/O enable (disables 8x96)",
      (ui_callback_t) toggle_SuperPET, NULL, NULL },
    { "SuperPET ACIA",
      NULL, NULL, pet_rs232_submenu },
    { "--" },
    { "*$9*** as RAM (8296 only)",
      (ui_callback_t) toggle_Ram9, NULL, NULL },
    { "*$A*** as RAM (8296 only)",
      (ui_callback_t) toggle_RamA, NULL, NULL },
    { "--" },
    { "Keyboard type",
      NULL, NULL, pet_keybd_submenu },
    { "--" },
    { "*Emulator identification",
      (ui_callback_t) toggle_EmuID, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t pet_menu[] = {
    { "PET model settings",
      NULL, NULL, model_settings_submenu },
    { "*PET userport diagnostic pin",
      (ui_callback_t) toggle_DiagPin, NULL, NULL },
    { "--" },
    { "Screen color",
      NULL, NULL, palette_submenu },
    { NULL }
};

static UI_CALLBACK(ui_datasette_control)
{
    int command = (int)client_data;
    datasette_control(command);
}

static ui_menu_entry_t datasette_control_submenu[] = {
    { "Stop", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_STOP, NULL },
    { "Play", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_START, NULL },
    { "Forward", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_FORWARD, NULL },
    { "Rewind", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_REWIND, NULL },
    { "Record", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_RECORD, NULL },
    { "Reset", (ui_callback_t) ui_datasette_control,
      (ui_callback_data_t) DATASETTE_CONTROL_RESET, NULL },
    { NULL }
};

ui_menu_entry_t ui_datasette_commands_menu[] = {
    { "Datassette control",
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int pet_ui_init(void)
{
#ifdef XPM
    {
        Pixmap icon_pixmap;

        /* Create the icon pixmap.  The evil `char *' prototype is needed
           because `XpmCreatePixmapFromData()' has a broken non-const-safe
           prototype.  */
        XpmCreatePixmapFromData(display, DefaultRootWindow(display),
                                (char **) icon_data,
                                &icon_pixmap, NULL, NULL);
        ui_set_application_icon(icon_pixmap);
    }
#endif

    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    ui_disk_commands_menu,
                                    ui_menu_separator,
                                    ui_tape_commands_menu,
                                    ui_datasette_commands_menu,
                                    ui_menu_separator,
                                    ui_smart_attach_commands_menu,
                                    ui_menu_separator,
                                    ui_directory_commands_menu,
                                    ui_menu_separator,
                                    ui_snapshot_commands_menu,
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
                                     ui_video_settings_menu,
#ifdef USE_VIDMODE_EXTENSION
				     ui_fullscreen_settings_menu,
#endif
                                     ui_keyboard_settings_menu,
                                     ui_sound_settings_menu,
                                     ui_par_drive_settings_menu,
                                     ui_peripheral_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     pet_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
                                     NULL));

    ui_update_menus();
    /* ui_toggle_drive_status(0); */

    return 0;
}
