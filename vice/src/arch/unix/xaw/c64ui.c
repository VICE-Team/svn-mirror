/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#include "cartridge.h"
#include "joystick.h"
#include "resources.h"
#include "uicommands.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "vsync.h"
#include "c64mem.h"

#ifdef XPM
#include <X11/xpm.h>
#include "c64icon.xpm"
#endif


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(attach_cartridge)
{
    int type = (int)client_data;
    char *filename;
    ui_button_t button;

    suspend_speed_eval();
    filename = ui_select_file("Attach cartridge image",
                              NULL, False, NULL, NULL, &button);

    switch (button) {
      case UI_BUTTON_OK:
        if (cartridge_attach_image(type, filename) < 0)
            ui_error("Invalid cartridge image");
        break;
      default:
        /* Do nothing special.  */
        break;
    }
}

static UI_CALLBACK(detach_cartridge)
{
    cartridge_detach_image();
}

static UI_CALLBACK(default_cartridge)
{
    cartridge_set_default();
}

static UI_CALLBACK(freeze_cartridge)
{
    cartridge_trigger_freeze();
}

static ui_menu_entry_t attach_cartridge_image_submenu[] = {
    { "Smart attach CRT image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_CRT, NULL },
    { "--" },
    { "Attach generic 8KB image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_GENERIC_8KB, NULL },
    { "Attach generic 16KB image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_GENERIC_16KB, NULL },
    { "Attach Action Replay image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_ACTION_REPLAY, NULL },
    { "Attach Super Snapshot 4 image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_SUPER_SNAPSHOT, NULL },
    { "--" },
    { "Set cartridge as default", (ui_callback_t)
      default_cartridge, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t ui_cartridge_commands_menu[] = {
    { "Attach a cartridge image",
      NULL, NULL, attach_cartridge_image_submenu },
    { "Detach cartridge image",
      (ui_callback_t) detach_cartridge, NULL, NULL },
    { "Cartridge freeze",
      (ui_callback_t) freeze_cartridge, NULL, NULL, XK_f, UI_HOTMOD_META },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_STRING_RADIO(PaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { "*Default",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "default", NULL },
    { "*C64S",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "c64s", NULL },
    { "*CCS64",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "ccs64", NULL },
    { "*Frodo",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "frodo", NULL },
    { "*GoDot",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "godot", NULL },
    { "*PC64",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "pc64", NULL },
    { "--" },
    { "Load custom",
      (ui_callback_t) ui_load_palette, NULL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CheckSsColl)
UI_MENU_DEFINE_TOGGLE(CheckSbColl)

static ui_menu_entry_t vic_submenu[] = {
    { "*Sprite-sprite collisions",
      (ui_callback_t) toggle_CheckSsColl, NULL, NULL },
    { "*Sprite-background collisions",
      (ui_callback_t) toggle_CheckSbColl, NULL, NULL },
    { "--",
      NULL, NULL, NULL },
    { "Color set",
      NULL, NULL, palette_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidModel)

static ui_menu_entry_t sid_model_submenu[] = {
    { "*6581 (old)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 0, NULL },
    { "*8580 (new)",
      (ui_callback_t) radio_SidModel, (ui_callback_data_t) 1, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidFilters)
#ifdef HAVE_RESID
UI_MENU_DEFINE_TOGGLE(SidUseResid)
#endif

static ui_menu_entry_t sid_submenu[] = {
    { "*Emulate filters",
      (ui_callback_t) toggle_SidFilters, NULL, NULL },
    { "Chip model",
      NULL, NULL, sid_model_submenu },
#ifdef HAVE_RESID
    { "--" },
    { "*Use reSID emulation",
      (ui_callback_t) toggle_SidUseResid, NULL, NULL },
#endif
    { NULL },
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(IEEE488)
UI_MENU_DEFINE_TOGGLE(REU)
UI_MENU_DEFINE_TOGGLE(Mouse)

static ui_menu_entry_t io_extensions_submenu[] = {
    { "*Emulator identification",
      (ui_callback_t) toggle_EmuID, NULL, NULL },
    { "*IEEE488 interface emulation",
      (ui_callback_t) toggle_IEEE488, NULL, NULL },
    { "*512K RAM Expansion Unit",
      (ui_callback_t) toggle_REU, NULL, NULL },
    { "*1351 Mouse Emulation",
      (ui_callback_t) toggle_Mouse, NULL, NULL, XK_m, UI_HOTMOD_META },
    { NULL }
};

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
      (ui_callback_t) swap_joystick_ports, NULL, NULL, XK_j, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { "Joystick settings",
      NULL, NULL, joystick_settings_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_romset_submenu[] = {
    { "default ROMs",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { "Load new kernal ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"KernalName", NULL },
    { "Load new BASIC ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"BasicName", NULL },
    { "Load new character ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"ChargenName", NULL },
    { "--" },
    { "Load new 1541 ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"DosName1541", NULL },
    { "Load new 1541-II ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"DosName1541ii", NULL },
    { "Load new 1571 ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"DosName1571", NULL },
    { "Load new 1581 ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"DosName1581", NULL },
    { "Load new 2031 ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"DosName2031", NULL },
    { "--" },
    { "Load custom ROM set from file",
      (ui_callback_t) ui_load_romset, NULL, NULL },
    { "Dump ROM set definition to file",
      (ui_callback_t) ui_dump_romset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_menu[] = {
    { "ROM settings",
      NULL, NULL, c64_romset_submenu },
    { "VIC-II settings",
      NULL, NULL, vic_submenu },
    { "SID settings",
      NULL, NULL, sid_submenu },
    { "I/O extensions at $DFxx",
      NULL, NULL, io_extensions_submenu },
    { "RS232 settings",
      NULL, NULL, rs232_submenu },
    { NULL }
};


int c64_ui_init(void)
{

#ifdef XPM
    {
        Pixmap icon_pixmap;

        /* Create the icon pixmap. */
        XpmCreatePixmapFromData(display, DefaultRootWindow(display),
                                (char **) icon_data, &icon_pixmap, NULL, NULL);
        ui_set_application_icon(icon_pixmap);
    }
#endif

    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    ui_disk_commands_menu,
                                    ui_menu_separator,
                                    ui_tape_commands_menu,
                                    ui_menu_separator,
                                    ui_smart_attach_commands_menu,
                                    ui_menu_separator,
                                    ui_cartridge_commands_menu,
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
                                     ui_drive_settings_menu,
                                     ui_peripheral_settings_menu,
                                     joystick_settings_menu,
                                     ui_menu_separator,
                                     c64_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
                                     NULL));

    ui_update_menus();

    return 0;
}
