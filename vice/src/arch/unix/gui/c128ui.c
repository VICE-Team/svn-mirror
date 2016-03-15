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
#include "c128model.h"
#include "cartridge.h"
#include "debug.h"
#include "icon.h"
#include "machine-video.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uic64cart.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidigimax.h"
#include "uidqbb.h"
#include "uidrive.h"
#include "uidrivec64c128.h"
#include "uidrivec128.h"
#include "uids12c887rtc.h"
#include "uieasyflash.h"
#include "uiedit.h"
#include "uiexpert.h"
#include "uigeoram.h"
#include "uiide64.h"
#include "uiisepic.h"
#include "uijoyport.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "uimagicvoice.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimmc64.h"
#include "uimmcreplay.h"
#include "uimouse.h"
#include "uinetplay.h"
#include "uiperipheraliec.h"
#include "uiprinteriec.h"
#include "uiram.h"
#include "uiramcart.h"
#include "uiretroreplay.h"
#include "uireu.h"
#include "uirrnetmk3.h"
#include "uiromset.h"

#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
#include "uirs232c64c128.h"
#endif

#include "uisampler.h"
#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uisoundexpander.h"
#include "uisupersnapshot.h"
#include "uitapeport.h"
#include "uitfe.h"
#include "uitpi.h"
#include "uiuserport.h"
#include "uivdc.h"
#include "uivicii.h"
#include "vsync.h"

UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_viciimodel_submenu[] = {
    { "PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { "NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)
UI_MENU_DEFINE_RADIO(SidTripleAddressStart)

SID_D4XX_MENU(set_sid_stereo_address_d4xx_submenu, radio_SidStereoAddressStart)
SID_D7XX_MENU(set_sid_stereo_address_d7xx_submenu, radio_SidStereoAddressStart)
SID_DEXX_MENU(set_sid_stereo_address_dexx_submenu, radio_SidStereoAddressStart)
SID_DFXX_MENU(set_sid_stereo_address_dfxx_submenu, radio_SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_submenu[] = {
    { "$D4xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d4xx_submenu },
    { "$D7xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d7xx_submenu },
    { "$DExx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

SID_D4XX_MENU(set_sid_triple_address_d4xx_submenu, radio_SidTripleAddressStart)
SID_D7XX_MENU(set_sid_triple_address_d7xx_submenu, radio_SidTripleAddressStart)
SID_DEXX_MENU(set_sid_triple_address_dexx_submenu, radio_SidTripleAddressStart)
SID_DFXX_MENU(set_sid_triple_address_dfxx_submenu, radio_SidTripleAddressStart)

static ui_menu_entry_t set_sid_triple_address_submenu[] = {
    { "$D4xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_d4xx_submenu },
    { "$D7xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_d7xx_submenu },
    { "$DExx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_dexx_submenu },
    { "$DFxx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_dfxx_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidFilters)

static ui_menu_entry_t sid_submenu[] = {
    { N_("Extra SIDs"), UI_MENU_TYPE_NORMAL, NULL, NULL, sid_extra_sids_submenu },
    { N_("Second SID base address"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_submenu },
    { N_("Third SID base address"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("SID filters"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_SidFilters, NULL, NULL },
#ifdef HAVE_RESID
    { "--", UI_MENU_TYPE_SEPARATOR }, 
    { N_("reSID sampling method"), UI_MENU_TYPE_NORMAL, NULL, NULL, sid_resid_sampling_submenu }, 
#if !defined(USE_GNOMEUI)
    { N_("reSID resampling passband"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_sid_resid_passband, NULL, NULL },
#endif
#endif
    { NULL },
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(C128FullBanks)
UI_MENU_DEFINE_TOGGLE(SFXSoundSampler)
UI_MENU_DEFINE_TOGGLE(CartridgeReset)

UI_MENU_DEFINE_RADIO(InternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(InternalFunctionROMRTCSave)
UI_MENU_DEFINE_RADIO(ExternalFunctionROM)
UI_MENU_DEFINE_TOGGLE(ExternalFunctionROMRTCSave)

static ui_menu_entry_t int_function_type_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_InternalFunctionROM,
      (ui_callback_data_t)0, NULL },
    { "ROM", UI_MENU_TYPE_TICK, (ui_callback_t)radio_InternalFunctionROM,
      (ui_callback_data_t)1, NULL },
    { "RAM", UI_MENU_TYPE_TICK, (ui_callback_t)radio_InternalFunctionROM,
      (ui_callback_data_t)2, NULL },
    { "RTC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_InternalFunctionROM,
      (ui_callback_data_t)3, NULL },
    { NULL }
};

static ui_menu_entry_t ext_function_type_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_ExternalFunctionROM,
      (ui_callback_data_t)0, NULL },
    { "ROM", UI_MENU_TYPE_TICK, (ui_callback_t)radio_ExternalFunctionROM,
      (ui_callback_data_t)1, NULL },
    { "RAM", UI_MENU_TYPE_TICK, (ui_callback_t)radio_ExternalFunctionROM,
      (ui_callback_data_t)2, NULL },
    { "RTC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_ExternalFunctionROM,
      (ui_callback_data_t)3, NULL },
    { NULL }
};

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
    { N_("Internal Function ROM type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, int_function_type_submenu },
    { N_("Internal Function ROM file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"InternalFunctionName", NULL },
    { N_("enable internal Function ROM RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_InternalFunctionROMRTCSave, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("External Function ROM type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ext_function_type_submenu },
    { N_("External Function ROM file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_function_rom_name,
      (ui_callback_data_t)"ExternalFunctionName", NULL },
    { N_("enable external Function ROM RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ExternalFunctionROMRTCSave, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("Function ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, functionrom_submenu },
    { N_("Banks 2 & 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_C128FullBanks, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { CARTRIDGE_NAME_GEORAM, UI_MENU_TYPE_NORMAL,
      NULL, NULL, georam_c64_submenu },
    { CARTRIDGE_NAME_REU, UI_MENU_TYPE_NORMAL,
      NULL, NULL, reu_submenu },
    { CARTRIDGE_NAME_RAMCART, UI_MENU_TYPE_NORMAL,
      NULL, NULL, ramcart_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { CARTRIDGE_NAME_DQBB, UI_MENU_TYPE_NORMAL,
      NULL, NULL, dqbb_submenu },
    { CARTRIDGE_NAME_EXPERT, UI_MENU_TYPE_NORMAL,
      NULL, NULL, expert_submenu },
    { CARTRIDGE_NAME_ISEPIC, UI_MENU_TYPE_NORMAL,
      NULL, NULL, isepic_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { CARTRIDGE_NAME_EASYFLASH, UI_MENU_TYPE_NORMAL,
      NULL, NULL, easyflash_submenu },
    { CARTRIDGE_NAME_IDE64, UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_submenu },
    { CARTRIDGE_NAME_MMC64, UI_MENU_TYPE_NORMAL,
      NULL, NULL, mmc64_submenu },
    { CARTRIDGE_NAME_MMC_REPLAY, UI_MENU_TYPE_NORMAL,
      NULL, NULL, mmcreplay_submenu },
    { CARTRIDGE_NAME_RETRO_REPLAY, UI_MENU_TYPE_NORMAL,
      NULL, NULL, retroreplay_submenu },
    { CARTRIDGE_NAME_SUPER_SNAPSHOT_V5, UI_MENU_TYPE_NORMAL,
      NULL, NULL, supersnapshot_v5_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, tfe_c64_submenu },
#endif
    { CARTRIDGE_NAME_RRNETMK3, UI_MENU_TYPE_NORMAL,
      NULL, NULL, rrnetmk3_submenu },
    { CARTRIDGE_NAME_IEEE488, UI_MENU_TYPE_NORMAL,
      NULL, NULL, tpi_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { CARTRIDGE_NAME_DIGIMAX, UI_MENU_TYPE_NORMAL,
      NULL, NULL, digimax_c64_submenu },
    { CARTRIDGE_NAME_MAGIC_VOICE, UI_MENU_TYPE_NORMAL,
      NULL, NULL, magicvoice_submenu },
#ifdef HAVE_MIDI
    { N_("MIDI emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, midi_c64_submenu },
#endif
    { CARTRIDGE_NAME_SFX_SOUND_EXPANDER, UI_MENU_TYPE_NORMAL,
      NULL, NULL, soundexpander_c64_submenu },
    { CARTRIDGE_NAME_SFX_SOUND_SAMPLER, UI_MENU_TYPE_NORMAL,
      (ui_callback_t)toggle_SFXSoundSampler, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { CARTRIDGE_NAME_DS12C887RTC, UI_MENU_TYPE_NORMAL,
      NULL, NULL, ds12c887rtc_c128_submenu },
    { "Userport devices", UI_MENU_TYPE_NORMAL,
      NULL, NULL, userport_c64_cbm2_submenu },
    { "Tape port devices", UI_MENU_TYPE_NORMAL,
      NULL, NULL, tapeport_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Reset on cart change"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CartridgeReset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c128ui_main_romset_submenu[] = {
    { N_("Load new International Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalIntName", NULL },
    { N_("Load new German Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalDEName", NULL },
    { N_("Load new Finnish Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalFIName", NULL },
    { N_("Load new French Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalFRName", NULL },
    { N_("Load new Italian Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalITName", NULL },
    { N_("Load new Norwegian Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalNOName", NULL },
    { N_("Load new Swedish Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalSEName", NULL },
    { N_("Load new Swiss Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalCHName", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new Basic Lo ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicLoName", NULL },
    { N_("Load new Basic Hi ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicHiName", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new International character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenIntName", NULL },
    { N_("Load new German character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenDEName", NULL },
    { N_("Load new French character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenFRName", NULL },
    { N_("Load new Swedish character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenSEName", NULL },
    { N_("Load new Swiss character ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenCHName", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new C64 Kernal ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Kernal64Name", NULL },
    { N_("Load new C64 Basic ROM"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"Basic64Name", NULL },
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
    { N_("Load new drive expansion ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uidrivec64c128_expansion_romset_submenu },
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
    { N_("VIC II Save media file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)1, NULL },
    { N_("VDC Save media file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Screenshots"), UI_MENU_TYPE_DOTS,
      NULL, NULL, screenshot_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(radio_c128model)
{
    int model, selected;

    selected = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (!CHECK_MENUS) {
        c128model_set(selected);
        ui_update_menus();
    } else {
        model = c128model_get();

        if (selected == model) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }
}

static ui_menu_entry_t set_c128_model_submenu[] = {
    { "C128 PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c128model,
      (ui_callback_data_t)C128MODEL_C128_PAL, NULL },
    { "C128DCR PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c128model,
      (ui_callback_data_t)C128MODEL_C128DCR_PAL, NULL },
    { "C128 NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c128model,
      (ui_callback_data_t)C128MODEL_C128_NTSC, NULL },
    { "C128DCR NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c128model,
      (ui_callback_data_t)C128MODEL_C128DCR_NTSC, NULL },
    { NULL }
};

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
    { N_("Swiss"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MachineType,
      (ui_callback_data_t)C128_MACHINE_SWISS, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Go64Mode)

UI_MENU_DEFINE_RADIO(CIA1Model)
static ui_menu_entry_t set_cia1model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)0, NULL },
    { N_("6526 (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA2Model)
static ui_menu_entry_t set_cia2model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)0, NULL },
    { N_("6526 (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

ui_menu_entry_t c128_model_submenu[] = {
    { N_("C128 model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_c128_model_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Machine type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, machine_type_submenu },
    { N_("VIC-II model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_viciimodel_submenu },
    { N_("SID model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_model_submenu },
    { N_("VDC model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_vdcmodel_submenu },
    { N_("CIA 1 model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_cia1model_submenu },
    { N_("CIA 2 model"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_cia2model_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Always switch to C64 mode on reset"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Go64Mode, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c128_menu[] = {
    { N_("Model settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_model_submenu },
    { N_("RAM reset pattern"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_ram_pattern_submenu },
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
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    { N_("RS232 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uirs232_c128_submenu },
#endif
    { NULL }
};

static ui_menu_entry_t c128_left_menu[] = {
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
      NULL, NULL, ui_snapshot_commands_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_screenshot_commands_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_record_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_edit_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_tool_commands_menu },
#ifdef HAVE_NETWORK
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, netplay_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_run_commands_menu },
#if defined(USE_XAWUI)
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_help_commands_menu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_exit_commands_menu },
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
#ifdef HAVE_NETWORK
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, netplay_submenu },
#endif
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

UI_MENU_DEFINE_TOGGLE(VirtualDevices)

static ui_menu_entry_t c128_settings_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_runmode_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sampler_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_drivec128_settings_menu },
    { N_("Printer settings"), UI_MENU_TYPE_NORMAL, 
      NULL, NULL, printeriec_settings_menu },
    { N_("Enable Virtual Devices"), UI_MENU_TYPE_TICK, 
      (ui_callback_t)toggle_VirtualDevices, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_joyport_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, joystick_settings_c64_menu },
#ifdef HAVE_MOUSE
    { N_("Mouse emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mouse_submenu },
#endif
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

static ui_menu_entry_t c128_main_menu[] = {
    { N_("File"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_file_menu },
#ifdef USE_GNOMEUI
    { N_("Edit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_edit_submenu },
#endif
    { N_("Snapshot"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_snapshot_menu },
    { N_("Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c128_settings_menu },
#ifdef DEBUG
    { N_("Debug"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, debug_settings_submenu },
#endif
    /* Translators: RJ means right justify and should be
        saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static ui_menu_entry_t c128_speed_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_runmode_commands_menu },
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
    uisampler_menu_create();
    uivicii_menu_create();
    uivdc_menu_create();
    uicart_menu_create();
    uikeyboard_menu_create();
    uijoyport_menu_create(1, 1, 1, 1, 0);
}

static void c128ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uivdc_menu_shutdown();
    uisound_menu_shutdown();
    uisampler_menu_shutdown();
    uikeyboard_menu_shutdown();
    uijoyport_menu_shutdown();
}

int c128ui_init(void)
{
    ui_set_application_icon(c128_icon_data);
    c128ui_dynamic_menu_create();
    ui_set_left_menu(c128_left_menu);

    ui_set_right_menu(c128_settings_menu);

    ui_set_topmenu(c128_main_menu);
    ui_set_speedmenu(c128_speed_menu);
    ui_set_tape_menu(c128_tape_menu);

    ui_set_drop_callback(uiattach_autostart_file);

    ui_update_menus();

    return 0;
}

void c128ui_shutdown(void)
{
    c128ui_dynamic_menu_shutdown();
}
