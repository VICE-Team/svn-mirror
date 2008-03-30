/*
 * c610ui.c - Implementation of the PET-specific part of the UI.
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

#include "petui.h"
#include "uimenu.h"
#include "resources.h"
#include "uicommands.h"
#include "uisettings.h"
#include "joystick.h"
#include "c610mem.h"
#include "vsync.h"

#ifdef XPM
#include <X11/xpm.h>
#include "c610icon.xpm"
#endif

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_STRING_RADIO(PaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { "*Default (Green)",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "default", NULL },
    { "*Amber",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "amber", NULL },
    { "*White",
      (ui_callback_t) radio_PaletteFile, (ui_callback_data_t) "white", NULL },
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
#ifdef USE_VIDMODE_EXTENSION
    ui_restore_windowmode();
#endif

    cbm2_set_model(client_data, NULL);
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
    int current_value, new_value = 2 * (int) client_data;
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
    { "*Graphics",
        (ui_callback_t) set_KeyboardType, (ui_callback_data_t) 1, NULL },
    { "*Business (UK)",
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
    { "Model defaults",
      NULL, NULL, model_defaults_submenu },
    { "--" },
    { "Memory size",
      NULL, NULL, c610_memsize_submenu },
    { "Hardwired switches",
      NULL, NULL, c610_modelline_submenu },
    { "--" },
    { "*Bank 15 $0800-$0FFF RAM",
      (ui_callback_t) toggle_Ram08, NULL, NULL },
    { "*Bank 15 $1000-$1FFF RAM",
      (ui_callback_t) toggle_Ram1, NULL, NULL },
    { "*Bank 15 $2000-$3FFF RAM",
      (ui_callback_t) toggle_Ram2, NULL, NULL },
    { "*Bank 15 $4000-$5FFF RAM",
      (ui_callback_t) toggle_Ram4, NULL, NULL },
    { "*Bank 15 $6000-$7FFF RAM",
      (ui_callback_t) toggle_Ram6, NULL, NULL },
    { "*Bank 15 $C000-$CFFF RAM",
      (ui_callback_t) toggle_RamC, NULL, NULL },
#if 0
    { "--" },
    { "Keyboard type",
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
      (ui_callback_t) set_joystick_device_1, (ui_callback_data_t) JOYDEV_NUMPAD,
 NULL },
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
      (ui_callback_t) set_joystick_device_2, (ui_callback_data_t) JOYDEV_NUMPAD,
 NULL },
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

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c610_rs232_submenu[] = {
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


/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c610_menu[] = {
    { "CBM-II model settings",
      NULL, NULL, model_settings_submenu },
    { "RS232 settings",
      NULL, NULL, c610_rs232_submenu },
    { "--" },
    { "Screen color",
      NULL, NULL, palette_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int c610_ui_init(void)
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

    ui_update_menus();

    return 0;
}
