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
#include "joystick.h"

#ifdef XPM
#include <X11/xpm.h>
#include "c64icon.xpm"
#endif

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_STRING_RADIO(PaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { "*Default",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "default", NULL },
    { "*CCS64",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "ccs64", NULL },
    { "*Frodo",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "frodo", NULL },
    { "*GoDot",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "godot", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CheckSsColl)
UI_MENU_DEFINE_TOGGLE(CheckSbColl)

static ui_menu_entry_t vic_submenu[] = {
    { "*Sprite-sprite collisions",
      (ui_callback_t) toggle_CheckSsColl, NULL, NULL },
    { "*Sprite-background collisions",
      (ui_callback_t) toggle_CheckSsColl, NULL, NULL },
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

static ui_menu_entry_t sid_submenu[] = {
    { "*Emulate filters",
      (ui_callback_t) toggle_SidFilters, NULL, NULL },
    { "Chip model",
      NULL, NULL, sid_model_submenu },
    { NULL },
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(Acia1)

static ui_menu_entry_t acia1_device_submenu[] = {
    { "*Serial 1",
      (ui_callback_t) radio_Acia1, (ui_callback_data_t) 0, NULL },
    { "*Serial 2",
      (ui_callback_t) radio_Acia1, (ui_callback_data_t) 1, NULL },
    { "*Dump to file",
      (ui_callback_t) radio_Acia1, (ui_callback_data_t) 2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia2)

static ui_menu_entry_t acia2_device_submenu[] = {
    { "*Serial 1",
      (ui_callback_t) radio_Acia2, (ui_callback_data_t) 0, NULL },
    { "*Serial 2",
      (ui_callback_t) radio_Acia2, (ui_callback_data_t) 1, NULL },
    { "*Dump to file",
      (ui_callback_t) radio_Acia2, (ui_callback_data_t) 2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice1Baud)

static ui_menu_entry_t ser1_baud_submenu[] = {
  { "*300",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)   300, NULL },
  { "*1200",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  1200, NULL },
  { "*2400",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  2400, NULL },
  { "*9600",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  9600, NULL },
  { "*19200",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t) 19200, NULL },
  { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice2Baud)

static ui_menu_entry_t ser2_baud_submenu[] = {
  { "*300",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)   300, NULL },
  { "*1200",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  1200, NULL },
  { "*2400",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  2400, NULL },
  { "*9600",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  9600, NULL },
  { "*19200",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t) 19200, NULL },
  { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice3Baud)

static ui_menu_entry_t ser3_baud_submenu[] = {
  { "*300",
      (ui_callback_t) radio_RsDevice3Baud, (ui_callback_data_t)   300, NULL },
  { "*1200",
      (ui_callback_t) radio_RsDevice3Baud, (ui_callback_data_t)  1200, NULL },
  { "*2400",
      (ui_callback_t) radio_RsDevice3Baud, (ui_callback_data_t)  2400, NULL },
  { "*9600",
      (ui_callback_t) radio_RsDevice3Baud, (ui_callback_data_t)  9600, NULL },
  { "*19200",
      (ui_callback_t) radio_RsDevice3Baud, (ui_callback_data_t) 19200, NULL },
  { NULL }
};

static UI_CALLBACK(set_rs232_device_file)
{
    char *resource = (char*) client_data;
    char *filename;
    ui_button_t button;

    suspend_speed_eval();

    filename = ui_select_file("Select RS232 device or dump file",
                              NULL, False, &button);
    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resource, (resource_value_t) filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
}

UI_MENU_DEFINE_TOGGLE(AciaDE)
UI_MENU_DEFINE_TOGGLE(AciaD6)

static ui_menu_entry_t rs232_submenu[] = {
    { "*ACIA $D6xx RS232 interface emulation",
      (ui_callback_t) toggle_AciaD6, NULL, NULL },
    { "ACIA $D6** device",
      NULL, NULL, acia2_device_submenu },
    { "--" },
    { "*ACIA $DExx RS232 interface emulation",
      (ui_callback_t) toggle_AciaDE, NULL, NULL },
    { "ACIA $DExx device",
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
    { "Dump filename...", (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice3", NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(IEEE488)
UI_MENU_DEFINE_TOGGLE(REU)

static ui_menu_entry_t io_extensions_submenu[] = {
    { "*Emulation identification",
      (ui_callback_t) toggle_EmuID, NULL, NULL },
    { "*IEEE488 interface emulation",
      (ui_callback_t) toggle_IEEE488, NULL, NULL },
    { "*512K RAM Expansion Unit",
      (ui_callback_t) toggle_REU, NULL, NULL },
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
      (ui_callback_t) swap_joystick_ports, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_menu[] = {
    { "VIC-II settings",
      NULL, NULL, vic_submenu },
    { "SID settings",
      NULL, NULL, sid_submenu },
    { "I/O extensions at $DFxx",
      NULL, NULL, io_extensions_submenu },
    { "RS232 settings",
      NULL, NULL, rs232_submenu },
    { "Joystick settings",
      NULL, NULL, joystick_settings_submenu },
    { NULL }
};

int c64_ui_init(void)
{
#ifdef XPM
    {
        Pixmap icon_pixmap;

        /* Create the icon pixmap. */
        XpmCreatePixmapFromData(display, DefaultRootWindow(display), icon_data,
                                &icon_pixmap, NULL, NULL);
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
