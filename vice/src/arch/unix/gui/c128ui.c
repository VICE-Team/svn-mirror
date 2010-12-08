/*
 * c128ui.c - Implementation of the C128-specific part of the UI.
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
#include <string.h>

#include "c128.h"
#include "debug.h"
#include "icon.h"
#include "machine-video.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uic64_256k.h"
#include "uic64cart.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidigimax.h"
#include "uidqbb.h"
#include "uidrive.h"
#include "uidrivec128.h"
#include "uieasyflash.h"
#include "uiedit.h"
#include "uiexpert.h"
#include "uigeoram.h"
#include "uiide64.h"
#include "uiisepic.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "uimagicvoice.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimmc64.h"
#include "uimmcreplay.h"
#include "uimouse.h"
#include "uilightpen.h"
#include "uiperipheraliec.h"
#include "uiplus256k.h"
#include "uiplus60k.h"
#include "uiramcart.h"
#include "uiretroreplay.h"
#include "uireu.h"
#include "uiromset.h"
#include "uirs232c64c128.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uisoundexpander.h"
#include "uitfe.h"
#include "uitpi.h"
#include "uivdc.h"
#include "uivicii.h"
#include "vsync.h"

UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_viciimodel_submenu[] = {
    { "PAL-G", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { "NTSC-M", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_d4xx_submenu[] = {
    { "$D420", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd420, NULL },
    { "$D440", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd440, NULL },
    { "$D460", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd460, NULL },
    { "$D480", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd480, NULL },
    { "$D4A0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4a0, NULL },
    { "$D4C0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4c0, NULL },
    { "$D4E0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d7xx_submenu[] = {
    { "$D700", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd700, NULL },
    { "$D720", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd720, NULL },
    { "$D740", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd740, NULL },
    { "$D760", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd760, NULL },
    { "$D780", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd780, NULL },
    { "$D7A0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7a0, NULL },
    { "$D7C0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7c0, NULL },
    { "$D7E0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dexx_submenu[] = {
    { "$DE00", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde00, NULL },
    { "$DE20", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde20, NULL },
    { "$DE40", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde40, NULL },
    { "$DE60", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde60, NULL },
    { "$DE80", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde80, NULL },
    { "$DEA0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdea0, NULL },
    { "$DEC0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdec0, NULL },
    { "$DEE0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdee0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dfxx_submenu[] = {
    { "$DF00", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf00, NULL },
    { "$DF20", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf20, NULL },
    { "$DF40", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf40, NULL },
    { "$DF60", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf60, NULL },
    { "$DF80", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf80, NULL },
    { "$DFA0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfa0, NULL },
    { "$DFC0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfc0, NULL },
    { "$DFE0", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfe0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_submenu[] = {
    { "$D4xx", UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_sid_stereo_address_d4xx_submenu },
    { "$D7xx", UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_sid_stereo_address_d7xx_submenu },
    { "$DExx", UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx", UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_TOGGLE(SidFilters)

static ui_menu_entry_t sid_submenu[] = {
    { N_("Second SID"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("Second SID base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_sid_stereo_address_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("SID filters"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
#ifdef HAVE_RESID
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("reSID sampling method"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_resid_sampling_submenu },
    { N_("reSID resampling passband..."), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)set_sid_resid_passband, NULL, NULL },
#endif
    { NULL },
};

static ui_menu_entry_t sid_options_submenu[] = {
    { N_("SID model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(C128FullBanks)
UI_MENU_DEFINE_TOGGLE(SFXSoundSampler)
UI_MENU_DEFINE_TOGGLE(InternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(ExternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(CartridgeReset)

UI_CALLBACK(set_function_rom_name)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    ui_button_t button;
    char *filename;
    static char *last_dir;
    uilib_file_filter_enum_t filter[] = { UILIB_FILTER_ALL };

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Function ROM image"), NULL, 0, last_dir, filter, 1, &button,  0, NULL, UI_FC_LOAD);

    if (button == UI_BUTTON_OK && filename != NULL) {
        if (resources_set_string(resname, filename) < 0) {
            ui_error(_("Could not load function ROM image\n'%s'"), filename);
        }
    }
}

static ui_menu_entry_t functionrom_submenu[] = {
    { N_("Enable internal Function ROM"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_InternalFunctionROM, NULL, NULL },
    { N_("Internal Function ROM name..."), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"InternalFunctionName", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable external Function ROM"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ExternalFunctionROM, NULL, NULL },
    { N_("External Function ROM name..."), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"ExternalFunctionName", NULL },
    { NULL }
};

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("Function ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, functionrom_submenu },
    { N_("Banks 2 & 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_C128FullBanks, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
#if 0
    { N_("256K RAM Expansion"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_256k_submenu },
#endif
    { N_("GEORAM Expansion Unit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, georam_submenu },
#if 0
    { N_("PLUS60K RAM Expansion"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, plus60k_submenu },
    { N_("PLUS256K RAM Expansion"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, plus256k_submenu },
#endif
    { N_("RAM Expansion Cart"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ramcart_submenu },
    { N_("RAM Expansion Unit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, reu_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Double Quick Brown Box cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, dqbb_submenu },
    { N_("Expert cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, expert_submenu },
    { N_("ISEPIC cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, isepic_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("EasyFlash cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, easyflash_submenu },
    { N_("IDE64 emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_submenu },
    { N_("MMC64 emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mmc64_submenu },
    { N_("MMC Replay emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mmcreplay_submenu },
    { N_("Retro Replay cartridge"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, retroreplay_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, tfe_submenu },
#endif
    { N_("IEEE 488 Interface"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, tpi_submenu },
#ifdef HAVE_MOUSE
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Mouse emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mouse_submenu },
    { N_("Lightpen emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, lightpen_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Digimax Cart"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, digimax_submenu },
    { N_("Magic Voice"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, magicvoice_submenu },
#ifdef HAVE_MIDI
    { N_("MIDI emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, midi_c64_submenu },
#endif
    { N_("SFX Sound Expander emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, soundexpander_submenu },
    { N_("SFX Sound Sampler emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SFXSoundSampler, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Emulator identification"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_EmuID, NULL, NULL },
    { N_("Power off on cartridge change"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CartridgeReset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c128ui_main_romset_submenu[] = {
    { N_("Load new Int. Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalIntName", NULL },
    { N_("Load new German Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalDEName", NULL },
    { N_("Load new Basic Lo ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicLoName", NULL },
    { N_("Load new Basic Hi ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicHiName", NULL },
    { N_("Load new Chargen ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t c128_romset_submenu[] = {
    { N_("Load default ROMs"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new computer ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128ui_main_romset_submenu },
    { N_("Load new drive ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_drivec128_romset_submenu },
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

/* ------------------------------------------------------------------------- */

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

static ui_menu_entry_t screenshot_submenu[] = {
    { N_("VIC II Save media file"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)1, NULL },
    { N_("VDC Save media file"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Screenshots..."), UI_MENU_TYPE_NORMAL,
      NULL, NULL, screenshot_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(MachineType)

static ui_menu_entry_t machine_type_submenu[] = {
    { N_("International"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_INT, NULL },
    { N_("Finnish"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_FINNISH, NULL },
    { N_("French"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_FRENCH, NULL },
    { N_("German"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_GERMAN, NULL },
    { N_("Italian"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_ITALIAN, NULL },
    { N_("Norwegian"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_NORWEGIAN, NULL },
    { N_("Swedish"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_SWEDISH, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Go64Mode)

/* FIXME: enable resources in c128 port */
#if 0
UI_MENU_DEFINE_RADIO(CIA1Model)
static ui_menu_entry_t set_cia1model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)0, NULL },
    { N_("6526A (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA2Model)
static ui_menu_entry_t set_cia2model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)0, NULL },
    { N_("6526A (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};
#endif

ui_menu_entry_t c128_model_submenu[] = {
    { N_("Machine type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, machine_type_submenu },
    { N_("VIC-II model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_viciimodel_submenu },
    { N_("SID model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_model_submenu },
    { N_("VDC model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_vdcmodel_submenu },
/* FIXME: enable resources in c128 port */
#if 0
    { N_("CIA 1 model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_cia1model_submenu },
    { N_("CIA 2 model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_cia2model_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Always switch to C64 mode"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Go64Mode, NULL, NULL },
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

static ui_menu_entry_t c128_menu[] = {
    { N_("Model settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_model_submenu },
    { N_("ROM settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_romset_submenu },
    { N_("VIC-II settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vicii_submenu },
    { N_("VDC settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vdc_submenu },
    { N_("SID settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_submenu },
    { N_("Memory and I/O extensions"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, io_extensions_submenu },
    { N_("RS232 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uirs232c64c128_submenu },
    { NULL }
};

static ui_menu_entry_t c128_left_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_disk_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_tape_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_datasette_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_smart_attach_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_c64cart_commands_menu },
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

static ui_menu_entry_t c128_right_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_drivec128_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, joystick_settings_c64_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, c128_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t c128_file_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_smart_attach_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_disk_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uiattach_tape_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_datasette_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_c64cart_commands_menu },
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
static ui_menu_entry_t c128_edit_submenu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_edit_commands_submenu },
    { NULL }
};
#endif

static ui_menu_entry_t c128_snapshot_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, screenshot_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t c128_options_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, joystick_options_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, sid_options_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t c128_settings_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_drivec128_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, joystick_settings_c64_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, c128_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t c128_main_menu[] = {
    { N_("File"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_file_menu },
#ifdef USE_GNOMEUI
    { N_("Edit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_edit_submenu },
#endif
    { N_("Snapshot"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_snapshot_menu },
    { N_("Options"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_options_menu },
    { N_("Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_settings_menu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static ui_menu_entry_t c128_tape_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_tape_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static void c128ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivicii_menu_create();
    uivdc_menu_create();

    memcpy(uikeymap_sym_submenu, keymap_sym_submenu, sizeof(keymap_sym_submenu));
    memcpy(uikeymap_pos_submenu, keymap_pos_submenu, sizeof(keymap_pos_submenu));
}

static void c128ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uivdc_menu_shutdown();
    uisound_menu_shutdown();
}

int c128ui_init(void)
{
    ui_set_application_icon(c128_icon_data);
    c128ui_dynamic_menu_create();
    ui_set_left_menu(c128_left_menu);

    ui_set_right_menu(c128_right_menu);

    ui_set_topmenu(c128_main_menu);
    ui_set_speedmenu(ui_performance_settings_menu);
    ui_set_tape_menu(c128_tape_menu);
    ui_update_menus();

    return 0;
}

void c128ui_shutdown(void)
{
    c128ui_dynamic_menu_shutdown();
}
