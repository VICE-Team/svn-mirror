/*
 * vic20ui.c - Implementation of the C64-specific part of the UI.
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

#include "cartridge.h"
#include "joystick.h"
#include "resources.h"
#include "uicommands.h"
#include "uimenu.h"
#include "uisettings.h"
#include "vsync.h"

#ifdef XPM
#include <X11/xpm.h>
#include "vic20icon.xpm"
#endif

/* ------------------------------------------------------------------------- */

enum {
    MEM_NONE,
    MEM_ALL,
    MEM_3K,
    MEM_8K,
    MEM_16K,
    MEM_24K
};

enum {
    BLOCK_0 = 1,
    BLOCK_1 = 1 << 1,
    BLOCK_2 = 1 << 2,
    BLOCK_3 = 1 << 3,
    BLOCK_5 = 1 << 5
};

static UI_CALLBACK(set_common_memory_configuration)
{
    if (!call_data) {
        int blocks;

        switch ((int) client_data) {
          case MEM_NONE:
            blocks = 0;
            break;
          case MEM_ALL:
            blocks = BLOCK_0 | BLOCK_1 | BLOCK_2 | BLOCK_3 | BLOCK_5;
            break;
          case MEM_3K:
            blocks = BLOCK_0;
            break;
          case MEM_8K:
            blocks = BLOCK_1;
            break;
          case MEM_16K:
            blocks = BLOCK_1 | BLOCK_2;
            break;
          case MEM_24K:
            blocks = BLOCK_1 | BLOCK_2 | BLOCK_3;
            break;
          default:
            /* Shouldn't happen.  */
            fprintf(stderr, "What?!\n");
            blocks = 0;         /* Make compiler happy.  */
        }
        resources_set_value("RamBlock0",
                            (resource_value_t) (blocks & BLOCK_0 ? 1 : 0));
        resources_set_value("RamBlock1",
                            (resource_value_t) (blocks & BLOCK_1 ? 1 : 0));
        resources_set_value("RamBlock2",
                            (resource_value_t) (blocks & BLOCK_2 ? 1 : 0));
        resources_set_value("RamBlock3",
                            (resource_value_t) (blocks & BLOCK_3 ? 1 : 0));
        resources_set_value("RamBlock5",
                            (resource_value_t) (blocks & BLOCK_5 ? 1 : 0));
        ui_menu_update_all();
        suspend_speed_eval();
    }
}

static ui_menu_entry_t vic20_romset_submenu[] = {
    { "Load default ROMs",
      (ui_callback_t) ui_set_romset, (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { "Load new Kernal ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"KernalName", NULL },
    { "Load new Basic ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"BasicName", NULL },
    { "Load new Character ROM",
      (ui_callback_t) ui_load_rom_file, (ui_callback_data_t)"ChargenName", NULL },
    { "--" },
    { "Load custom ROM set from file",
      (ui_callback_t) ui_load_romset, NULL, NULL },
    { "Dump ROM set definition to file",
      (ui_callback_t) ui_dump_romset, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t common_memory_configurations_submenu[] = {
    { "No expansion memory",
      set_common_memory_configuration, (ui_callback_data_t) MEM_NONE, NULL },
    { "--" },
    { "3K (block 0)",
      set_common_memory_configuration, (ui_callback_data_t) MEM_3K, NULL },
    { "8K (block 1)",
      set_common_memory_configuration, (ui_callback_data_t) MEM_8K, NULL },
    { "16K (blocks 1/2)",
      set_common_memory_configuration, (ui_callback_data_t) MEM_16K, NULL },
    { "24K (blocks 1/2/3)",
      set_common_memory_configuration, (ui_callback_data_t) MEM_24K, NULL },
    { "--" },
    { "All (blocks 0/1/2/3/5)",
      set_common_memory_configuration, (ui_callback_data_t) MEM_ALL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(RAMBlock0)
UI_MENU_DEFINE_TOGGLE(RAMBlock1)
UI_MENU_DEFINE_TOGGLE(RAMBlock2)
UI_MENU_DEFINE_TOGGLE(RAMBlock3)
UI_MENU_DEFINE_TOGGLE(RAMBlock4)
UI_MENU_DEFINE_TOGGLE(RAMBlock5)

UI_MENU_DEFINE_TOGGLE(EmuID)

static ui_menu_entry_t memory_settings_submenu[] = {
    { "Common configurations",
      NULL, NULL, common_memory_configurations_submenu },
    { "--" },
    { "ROM sets",
      NULL, NULL, vic20_romset_submenu },
    { "--" },
    { "*Block 0 (3K at $0400-$0FFF)",
      (ui_callback_t) toggle_RAMBlock0, NULL, NULL },
    { "*Block 1 (8K at $2000-$3FFF)",
      (ui_callback_t) toggle_RAMBlock1, NULL, NULL },
    { "*Block 2 (8K at $4000-$5FFF)",
      (ui_callback_t) toggle_RAMBlock2, NULL, NULL },
    { "*Block 3 (8K at $6000-$7FFF)",
      (ui_callback_t) toggle_RAMBlock3, NULL, NULL },
    { "*Block 5 (8K at $A000-$BFFF)",
      (ui_callback_t) toggle_RAMBlock5, NULL, NULL },
    { "--" },
    { "*Emulator identification",
      (ui_callback_t) toggle_EmuID, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t memory_settings_menu[] = {
    { "Memory expansions",
      NULL, NULL, memory_settings_submenu },
    { NULL }
};


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
	ui_update_menus();
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

static ui_menu_entry_t attach_cartridge_image_submenu[] = {
    { "Smart-attach cartridge image...",
      (ui_callback_t) attach_cartridge,
      (ui_callback_data_t) CARTRIDGE_VIC20_DETECT, NULL,
      XK_c, UI_HOTMOD_META },
    { "--" },
    { "Attach 4/8KB image at $2000...",
      (ui_callback_t) attach_cartridge,
      (ui_callback_data_t) CARTRIDGE_VIC20_8KB_2000, NULL },
    { "Attach 4/8KB image at $6000...",
      (ui_callback_t) attach_cartridge,
      (ui_callback_data_t) CARTRIDGE_VIC20_8KB_6000, NULL },
    { "Attach 4/8KB image at $A000...",
      (ui_callback_t) attach_cartridge,
      (ui_callback_data_t) CARTRIDGE_VIC20_8KB_A000, NULL },
    { "Attach 4KB image at $B000...",
      (ui_callback_t) attach_cartridge,
      (ui_callback_data_t) CARTRIDGE_VIC20_4KB_B000, NULL },
/*
    { "Attach 16KB image...",
      (ui_callback_t) attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_VIC20_16KB, NULL },
*/
    { "--" },
    { "Set cartridge as default",
      (ui_callback_t) default_cartridge, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t vic20_cartridge_commands_menu[] = {
    { "Attach a cartridge image",
      NULL, NULL, attach_cartridge_image_submenu },
    { "Detach cartridge image(s)",
      (ui_callback_t) detach_cartridge, NULL, NULL },
    { NULL }
};


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(set_joystick_device)
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

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { "*None",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_NONE, NULL },
    { "*Numpad",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_NUMPAD, NULL },
    { "*Custom Keys",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { "*Analog Joystick 0",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_ANALOG_0, NULL },
    { "*Analog Joystick 1",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { "*Digital Joystick 0",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_DIGITAL_0, NULL },
    { "*Digital Joystick 1",
      (ui_callback_t) set_joystick_device, (ui_callback_data_t) JOYDEV_DIGITAL_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { "Joystick settings",
      NULL, NULL, set_joystick_device_1_submenu },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RsUser)

static ui_menu_entry_t vic20_rs232_submenu[] = {
    { "*No Userport RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 0, NULL },
    { "*Userport 300 baud RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 300, NULL },
    { "*Userport 1200 baud RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 1200, NULL },
    { "Userport RS232 device",
      NULL, NULL, rsuser_device_submenu },
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

static ui_menu_entry_t rs232_settings_menu[] = {
    { "RS232 settings",
      NULL, NULL, vic20_rs232_submenu },
    { NULL }
};

int vic20_ui_init(void)
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
				    vic20_cartridge_commands_menu,
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
                                     rs232_settings_menu,
                                     ui_menu_separator,
                                     memory_settings_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
                                     NULL));

    ui_update_menus();

    return 0;
}
