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
#include "log.h"
#include "machine.h"
#include "machine-video.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidrive.h"
#include "uidrivec64vic20.h"
#include "uidrivevic20.h"
#include "uiedit.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimouse.h"
#include "uilightpen.h"
#include "uiperipheraliec.h"
#include "uiromset.h"
#include "uirs232.h"
#include "uirs232user.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
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
    int blocks = 0;

    switch (vice_ptr_to_int(UI_MENU_CB_PARAM)) {
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
            log_error(LOG_DEFAULT, "vic20ui:set_common_memory_configuration bogus memory config.");
        case MEM_NONE:
            blocks = 0;
            break;
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
    { "$9800", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)0, NULL },
    { "$9C00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidAddress, (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidClock)

static ui_menu_entry_t sidcart_clock_submenu[] = {
    { "C64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidClock,
      (ui_callback_data_t)0, NULL },
    { "VIC20", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidClock,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static ui_menu_entry_t sidcart_submenu[] = {
    { N_("Enable"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SidCart, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("SID model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_model_noresid_submenu },
    { N_("SID filters"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
    { N_("SID address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sidcart_address_submenu },
    /* Translators: "SID clock" as in "CPU Frequency" */
    { N_("SID clock"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sidcart_clock_submenu },
    { NULL }
};

static ui_menu_entry_t vic20ui_main_romset_submenu[] = {
    { N_("Load new kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new BASIC ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Load new character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t vic20_romset_submenu[] = {
    { N_("Load default ROMs"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_set_romset, (ui_callback_data_t)"default.vrs", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new computer ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20ui_main_romset_submenu },
    { N_("Load new drive ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_drivec64vic20_romset_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("ROM set type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiromset_type_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("ROM set archive"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiromset_archive_submenu },
    { N_("ROM set file"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiromset_file_submenu },
    { NULL }
};

static ui_menu_entry_t common_memory_configurations_submenu[] = {
    { N_("No expansion memory"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_NONE, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("3K (block 0)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_3K, NULL },
    { N_("8K (block 1)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_8K, NULL },
    { N_("16K (blocks 1/2)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_16K, NULL },
    { N_("24K (blocks 1/2/3)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_24K, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("All (blocks 0/1/2/3/5)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t) set_common_memory_configuration,
      (ui_callback_data_t)MEM_ALL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(RAMBlock0)
UI_MENU_DEFINE_TOGGLE(RAMBlock1)
UI_MENU_DEFINE_TOGGLE(RAMBlock2)
UI_MENU_DEFINE_TOGGLE(RAMBlock3)
UI_MENU_DEFINE_TOGGLE(RAMBlock5)

static ui_menu_entry_t memory_settings_submenu[] = {
    { N_("Common configurations"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, common_memory_configurations_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Block 0 (3K at $0400-$0FFF)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RAMBlock0, NULL, NULL },
    { N_("Block 1 (8K at $2000-$3FFF)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RAMBlock1, NULL, NULL },
    { N_("Block 2 (8K at $4000-$5FFF)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RAMBlock2, NULL, NULL },
    { N_("Block 3 (8K at $6000-$7FFF)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RAMBlock3, NULL, NULL },
    { N_("Block 5 (8K at $A000-$BFFF)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RAMBlock5, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(attach_cartridge)
{
    int type = vice_ptr_to_int(UI_MENU_CB_PARAM);
    char *filename;
    ui_button_t button;
    static char *last_dir;
    uilib_file_filter_enum_t filter[] = { UILIB_FILTER_VIC20CART, UILIB_FILTER_ALL };

    vsync_suspend_speed_eval();
    filename = ui_select_file(_("Attach cartridge image"), NULL, 0, last_dir, filter, sizeof(filter) / sizeof(*filter), &button, 0, NULL, UI_FC_LOAD);
    switch (button) {
        case UI_BUTTON_OK:
            if (cartridge_attach_image(type, filename) < 0) {
                ui_error(_("Invalid cartridge image"));
            }
            lib_free(last_dir);
            util_fname_split(filename, &last_dir, NULL);
            ui_update_menus();
            break;
        default:
            /* Do nothing special.  */
            break;
    }
    lib_free(filename);
}

static ui_menu_entry_t add_to_generic_cart_submenu[] = {
    { N_("Smart-attach cartridge image"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_DETECT, NULL,
      KEYSYM_c, UI_HOTMOD_META },
    { N_("Attach 4/8/16KB cartridge image at $2000"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_2000, NULL },
    { N_("Attach 4/8/16KB cartridge image at $4000"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_4000, NULL },
    { N_("Attach 4/8/16KB cartridge image at $6000"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_16KB_6000, NULL },
    { N_("Attach 4/8KB cartridge image at $A000"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_8KB_A000, NULL },
    { N_("Attach 4KB cartridge image at $B000"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_4KB_B000, NULL },
    { NULL }
};

static UI_CALLBACK(detach_cartridge)
{
    cartridge_detach_image(-1);
}

static UI_CALLBACK(default_cartridge)
{
    cartridge_set_default();
}

static ui_menu_entry_t attach_cartridge_image_submenu[] = {
    { N_("Attach generic image"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_GENERIC, NULL },
    { N_("Attach Mega-Cart image"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_MEGACART, NULL },
    { N_("Attach Final Expansion image"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_FINAL_EXPANSION, NULL },
    { N_("Attach Vic Flash Plugin image"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)attach_cartridge,
      (ui_callback_data_t)CARTRIDGE_VIC20_FP, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Add to generic cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, add_to_generic_cart_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Set cartridge as default"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)default_cartridge, NULL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CartridgeReset)
UI_MENU_DEFINE_TOGGLE(FinalExpansionWriteBack)
UI_MENU_DEFINE_TOGGLE(VicFlashPluginWriteBack)
UI_MENU_DEFINE_TOGGLE(MegaCartNvRAMWriteBack)

UI_CALLBACK(set_mc_nvram_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("Mega-Cart NvRAM image"), UILIB_FILTER_ALL);
}

static ui_menu_entry_t fexp_submenu[] = {
    { N_("Enable image write back"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FinalExpansionWriteBack, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t vicflash_submenu[] = {
    { N_("Enable image write back"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VicFlashPluginWriteBack, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t megacart_submenu[] = {
    { N_("Enable NvRAM image write back"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_MegaCartNvRAMWriteBack, NULL, NULL },
    { N_("NvRAM image file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_mc_nvram_image_name,
      (ui_callback_data_t)"MegaCartNvRAMfilename", NULL },
    { NULL }
};

static ui_menu_entry_t vic20_cartridge_commands_menu[] = {
    { N_("Attach cartridge image"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, attach_cartridge_image_submenu },
    { N_("Detach cartridge image(s)"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)detach_cartridge, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(set_joystick_device)
{
    vsync_suspend_speed_eval();
    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice1", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        int tmp;

        resources_get_int("JoyDevice1", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t joystick_settings_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { NULL }
};

/*------------------------------------------------------------*/

UI_MENU_DEFINE_TOGGLE(RsUserEnable)

static ui_menu_entry_t vic20_rs232_submenu[] = {
    { N_("Userport RS232 emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_device_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Serial 1 device"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser1_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Serial 2 device"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser2_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Dump filename"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Program name to exec"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};

static ui_menu_entry_t rs232_settings_menu[] = {
    { N_("RS232 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_rs232_submenu },
    { NULL }
};

/*------------------------------------------------------------*/

static UI_CALLBACK(save_screenshot)
{
    /* Where does the 1024 come from?  */
    char filename[1024];
    unsigned int wid = vice_ptr_to_uint(UI_MENU_CB_PARAM);

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_screenshot_dialog(filename, machine_video_canvas_get(wid)) < 0) {
        return;
    }
}

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Save media file"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_vic_model_submenu[] = {
    { "PAL-G", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { "NTSC-M", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

static ui_menu_entry_t vic20_model_submenu[] = {
    { N_("VIC model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_vic_model_submenu },
    { N_("Memory expansions"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, memory_settings_submenu },
    { NULL }
};

static ui_menu_entry_t io_extensions_submenu[] = {
    { CARTRIDGE_VIC20_NAME_MEGACART, UI_MENU_TYPE_NORMAL,
      NULL, NULL, megacart_submenu },
    { CARTRIDGE_VIC20_NAME_FINAL_EXPANSION, UI_MENU_TYPE_NORMAL,
      NULL, NULL, fexp_submenu },
    { CARTRIDGE_VIC20_NAME_FP, UI_MENU_TYPE_NORMAL,
      NULL, NULL, vicflash_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("SID cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sidcart_submenu },
    { N_("Paddle emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mouse_vic20_submenu },
#if defined(HAVE_MOUSE)
    { N_("Lightpen emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, lightpen_submenu },
#endif
#ifdef HAVE_MIDI
    { N_("MIDI emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, midi_vic20_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Reset on cart change"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CartridgeReset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t keymap_sym_submenu[] = {
    { "US", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SymKeymap, (ui_callback_data_t)"x11_sym.vkm", NULL },
/*    { N_("German"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SymKeymap, (ui_callback_data_t)"x11_symger.vkm", NULL }, */
    { NULL }
};

static ui_menu_entry_t keymap_pos_submenu[] = {
    { "US", UI_MENU_TYPE_TICK, (ui_callback_t)radio_PosKeymap, (ui_callback_data_t)"x11_pos.vkm", NULL },
/*    { N_("German"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_PosKeymap, (ui_callback_data_t)"x11_posger.vkm", NULL }, */
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t vic20_menu[] = {
    { N_("Model settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_model_submenu },
    { N_("ROM settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_romset_submenu },
    { N_("VIC settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic_submenu },
    { N_("I/O extensions"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, io_extensions_submenu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, rs232_settings_menu },
    { NULL }
};

static ui_menu_entry_t vic20_left_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_disk_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_tape_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_datasette_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_smart_attach_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, vic20_cartridge_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_directory_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_snapshot_commands_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_screenshot_commands_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_record_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_tool_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_help_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_run_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_exit_commands_menu },
    { NULL }
};

static ui_menu_entry_t vic20_right_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_drivevic20_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, joystick_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, vic20_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t vic20_tape_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_tape_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static ui_menu_entry_t vic20_file_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_smart_attach_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_disk_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_tape_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_datasette_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, vic20_cartridge_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_directory_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_tool_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_run_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_exit_commands_menu },
    { NULL }
};

#ifdef USE_GNOMEUI
static ui_menu_entry_t vic20_edit_submenu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_edit_commands_submenu },
    { NULL }
};
#endif

static ui_menu_entry_t vic20_snapshot_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_screenshot_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t vic20_options_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t vic20_settings_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_drivevic20_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, joystick_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, vic20_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t vic20_top_menu[] = {
    { N_("File"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_file_menu },
#ifdef USE_GNOMEUI
    { N_("Edit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_edit_submenu },
#endif
    { N_("Snapshot"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_snapshot_menu },
    { N_("Options"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_options_menu },
    { N_("Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vic20_settings_menu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static void vic20ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivic_menu_create();

    memcpy(uikeymap_sym_submenu, keymap_sym_submenu, sizeof(keymap_sym_submenu));
    memcpy(uikeymap_pos_submenu, keymap_pos_submenu, sizeof(keymap_pos_submenu));
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
