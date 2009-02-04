/*
 * vic20ui.c - Implementation of the VIC20-specific part of the UI.
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
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "debug.h"
#include "icon.h"
#include "joy.h"
#include "lib.h"
#include "machine-video.h"
#include "resources.h"
#include "uiattach.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uidrivec64vic20.h"
#include "uidrivevic20.h"
#include "uikeyboard.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimouse.h"
#include "uiperipheraliec.h"
#include "uiromset.h"
#include "uirs232.h"
#include "uirs232user.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisidcart.h"
#include "uisound.h"
#include "uivic.h"
#include "util.h"
#include "vsync.h"


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
    int blocks;

    switch ((int) UI_MENU_CB_PARAM) {
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
        fprintf(stderr, _("What?!\n"));
        blocks = 0;         /* Make compiler happy.  */
    }
    resources_set_int("RamBlock0", blocks & BLOCK_0 ? 1 : 0);
    resources_set_int("RamBlock1", blocks & BLOCK_1 ? 1 : 0);
    resources_set_int("RamBlock2", blocks & BLOCK_2 ? 1 : 0);
    resources_set_int("RamBlock3", blocks & BLOCK_3 ? 1 : 0);
    resources_set_int("RamBlock5", blocks & BLOCK_5 ? 1 : 0);
    ui_menu_update_all();
    vsync_suspend_speed_eval();
}

UI_MENU_DEFINE_TOGGLE(SidCart)
UI_MENU_DEFINE_TOGGLE(SidFilters)

UI_MENU_DEFINE_RADIO(SidAddress)

static ui_menu_entry_t sidcart_address_submenu[] = {
    { "*$9800",
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)0, NULL },
    { "*$9C00",
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidClock)

static ui_menu_entry_t sidcart_clock_submenu[] = {
    { "*C64", (ui_callback_t)radio_SidClock,
      (ui_callback_data_t)0, NULL },
    { "*VIC20", (ui_callback_t)radio_SidClock,
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

static ui_menu_entry_t vic20ui_main_romset_submenu[] = {
    { N_("Load new Kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new Basic ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Load new Character ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t vic20_romset_submenu[] = {
    { N_("Load default ROMs"),
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, vic20ui_main_romset_submenu },
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

static ui_menu_entry_t common_memory_configurations_submenu[] = {
    { N_("No expansion memory"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_NONE, NULL },
    { "--" },
    { N_("3K (block 0)"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_3K, NULL },
    { N_("8K (block 1)"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_8K, NULL },
    { N_("16K (blocks 1/2)"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_16K, NULL },
    { N_("24K (blocks 1/2/3)"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_24K, NULL },
    { "--" },
    { N_("All (blocks 0/1/2/3/5)"),
      (ui_callback_t) set_common_memory_configuration, 
      (ui_callback_data_t)MEM_ALL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(RAMBlock0)
UI_MENU_DEFINE_TOGGLE(RAMBlock1)
UI_MENU_DEFINE_TOGGLE(RAMBlock2)
UI_MENU_DEFINE_TOGGLE(RAMBlock3)
UI_MENU_DEFINE_TOGGLE(RAMBlock5)

UI_MENU_DEFINE_TOGGLE(EmuID)

static ui_menu_entry_t memory_settings_submenu[] = {
    { N_("Common configurations"),
      NULL, NULL, common_memory_configurations_submenu },
    { "--" },
    { N_("ROM sets"),
      NULL, NULL, vic20_romset_submenu },
    { "--" },
    { N_("*Block 0 (3K at $0400-$0FFF)"),
      (ui_callback_t)toggle_RAMBlock0, NULL, NULL },
    { N_("*Block 1 (8K at $2000-$3FFF)"),
      (ui_callback_t)toggle_RAMBlock1, NULL, NULL },
    { N_("*Block 2 (8K at $4000-$5FFF)"),
      (ui_callback_t)toggle_RAMBlock2, NULL, NULL },
    { N_("*Block 3 (8K at $6000-$7FFF)"),
      (ui_callback_t)toggle_RAMBlock3, NULL, NULL },
    { N_("*Block 5 (8K at $A000-$BFFF)"),
      (ui_callback_t)toggle_RAMBlock5, NULL, NULL },
    { "--" },
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
    { NULL }
};

#if 0
static ui_menu_entry_t memory_settings_menu[] = {
    { N_("Memory expansions"),
      NULL, NULL, memory_settings_submenu },
    { NULL }
};
#endif

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(attach_cartridge)
{
    int type = (int)UI_MENU_CB_PARAM;
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    filename = ui_select_file(_("Attach cartridge image"),
                              NULL, 0, last_dir, "*.prg", &button,
                              0, NULL, UI_FC_LOAD);
    switch (button) {
      case UI_BUTTON_OK:
        if (cartridge_attach_image(type, filename) < 0)
            ui_error(_("Invalid cartridge image"));
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        ui_update_menus();
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
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
    { N_("Smart-attach cartridge image..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_DETECT, NULL,
      KEYSYM_c, UI_HOTMOD_META },
    { "--" },
    { N_("Attach 4/8/16KB image at $2000..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_2000, NULL },
    { N_("Attach 4/8/16KB image at $4000..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_4000, NULL },
    { N_("Attach 4/8/16KB image at $6000..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_6000, NULL },
    { N_("Attach 4/8KB image at $A000..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_8KB_A000, NULL },
    { N_("Attach 4KB image at $B000..."),
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_4KB_B000, NULL },
    { "--" },
    { N_("Set cartridge as default"),
      (ui_callback_t)default_cartridge, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t vic20_cartridge_commands_menu[] = {
    { N_("Attach a cartridge image"),
      NULL, NULL, attach_cartridge_image_submenu },
    { N_("Detach cartridge image(s)"),
      (ui_callback_t)detach_cartridge, NULL, NULL },
    { NULL }
};


/* ------------------------------------------------------------------------- */

static UI_CALLBACK(set_joystick_device)
{
    vsync_suspend_speed_eval();
    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice1", (int)UI_MENU_CB_PARAM);
        ui_update_menus();
    } else {
        int tmp;

        resources_get_int("JoyDevice1", &tmp);
        ui_menu_set_tick(w, tmp == (int)UI_MENU_CB_PARAM);
    }
}

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { N_("*None"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("*Numpad"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("*Keyset 1"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("*Keyset 2"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("*Analog Joystick 0"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("*Analog Joystick 1"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("*Digital Joystick 0"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("*Digital Joystick 1"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("*USB Joystick 0"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("*USB Joystick 1"),
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { N_("Joystick settings"),
      NULL, NULL, set_joystick_device_1_submenu },
    { NULL }
};

/*------------------------------------------------------------*/

UI_MENU_DEFINE_TOGGLE(RsUserEnable)

static ui_menu_entry_t vic20_rs232_submenu[] = {
    { N_("*Userport RS232 emulation"),
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"),
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"),
      NULL, NULL, rs232user_device_submenu },
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

static ui_menu_entry_t rs232_settings_menu[] = {
    { N_("RS232 settings"),
      NULL, NULL, vic20_rs232_submenu },
    { NULL }
};

/*------------------------------------------------------------*/

/*
UI_MENU_DEFINE_TOGGLE(IEEE488)

static ui_menu_entry_t vic20_io_submenu[] = {
    { "*VIC-1112 IEEE 488 module",
      (ui_callback_t)toggle_IEEE488, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t vic20_io_settings_menu[] = {
    { "I/O settings",
      NULL, NULL, vic20_io_submenu },
    { NULL }
};
*/

/*------------------------------------------------------------*/

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

static ui_menu_entry_t vic20_menu[] = {
    { N_("VIC settings"),
      NULL, NULL, vic_submenu },
    { N_("Memory expansions"),
      NULL, NULL, memory_settings_submenu },
    { N_("SID cartridge settings"),
      NULL, NULL, sidcart_submenu },
    { N_("Paddle emulation"),
      NULL, NULL, mouse_vic20_submenu },
#ifdef HAVE_MIDI
    { N_("MIDI emulation"),
      NULL, NULL, midi_vic20_submenu },
#endif
    { NULL }
};

static ui_menu_entry_t vic20_left_menu[] = {
    { "",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, vic20_cartridge_commands_menu },
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

static ui_menu_entry_t vic20_right_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivevic20_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
/*
                                     vic20_io_settings_menu,
*/
    { "",
      NULL, NULL, joystick_settings_menu },
    { "",
      NULL, NULL, rs232_settings_menu },
    { "--",
      NULL, NULL, vic20_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--",
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t vic20_tape_menu[] = {
    { "",
      NULL, NULL, uiattach_tape_menu },
    { "--",
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static ui_menu_entry_t vic20_file_menu[] = {
    { "",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, vic20_cartridge_commands_menu },
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

static ui_menu_entry_t vic20_snapshot_menu[] = {
    { "",
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--",
      NULL, NULL, ui_screenshot_commands_menu },
    { "",
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t vic20_options_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, ui_drive_options_submenu },
    { NULL }
};

static ui_menu_entry_t vic20_settings_menu[] = {
    { "",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivevic20_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_menu },
    { "",
      NULL, NULL, rs232_settings_menu },
    { "--",
      NULL, NULL, vic20_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t vic20_top_menu[] = {
    { N_("File"),
      NULL, NULL, vic20_file_menu },
    { N_("Snapshot"),
      NULL, NULL, vic20_snapshot_menu },
    { N_("Options"),
      NULL, NULL, vic20_options_menu },
    { N_("Settings"),
      NULL, NULL, vic20_settings_menu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"),
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static void vic20ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivic_menu_create();
}

static void vic20ui_dynamic_menu_shutdown(void)
{
    uivic_menu_shutdown();
    uisound_menu_shutdown();
}

int vic20ui_init(void)
{
    ui_set_application_icon(vic20_icon_data);
    vic20ui_dynamic_menu_create();
    ui_set_left_menu(vic20_left_menu);

    ui_set_right_menu(vic20_right_menu);

    ui_set_tape_menu(vic20_tape_menu);
    ui_set_topmenu(vic20_top_menu);
    ui_set_speedmenu(ui_performance_settings_menu);
    ui_update_menus();

    return 0;
}

void vic20ui_shutdown(void)
{
    vic20ui_dynamic_menu_shutdown();
}

