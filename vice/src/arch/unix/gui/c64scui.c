/*
 * c64scui.c - Implementation of the C64SC-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#include "c64-resources.h"
#include "c64model.h"
#include "cartridge.h"
#include "cia.h"
#include "debug.h"
#include "icon.h"
#include "machine.h"
#include "machine-video.h"
#include "resources.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uic64cart.h"
#include "uic64memoryhacks.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidigimax.h"
#include "uidqbb.h"
#include "uidrive.h"
#include "uidrivec64.h"
#include "uidrivec64c128.h"
#include "uidrivec64vic20.h"
#include "uids12c887rtc.h"
#include "uieasyflash.h"
#include "uiedit.h"
#include "uiexpert.h"
#include "uigeoram.h"
#include "uiide64.h"
#include "uiisepic.h"
#include "uijoystick2.h"
#include "uikeyboard.h"
#include "uimagicvoice.h"
#include "uimenu.h"
#include "uimidi.h"
#include "uimmc64.h"
#include "uimmcreplay.h"
#include "uimouse.h"
#include "uinetplay.h"
#include "uilightpen.h"
#include "uiperipheraliec.h"
#include "uiprinteriec.h"
#include "uiram.h"
#include "uiramcart.h"
#include "uiretroreplay.h"
#include "uireu.h"
#include "uiromset.h"

#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
#include "uirs232c64c128.h"
#endif

#include "uiscreenshot.h"
#include "uisettings.h"
#include "uisid.h"
#include "uisound.h"
#include "uisoundexpander.h"
#include "uitfe.h"
#include "uitpi.h"
#include "uiuserportrtc.h"
#include "uivicii.h"
#include "vsync.h"
#include "vicii.h"

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(radio_c64model)
{
    int model, selected;

    selected = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (!CHECK_MENUS) {
        c64model_set(selected);
        ui_update_menus();
    } else {
        model = c64model_get();

        if (selected == model) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }
}

static ui_menu_entry_t set_c64_model_submenu[] = {
    { "C64 PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_PAL, NULL },
    { "C64C PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64C_PAL, NULL },
    { N_("C64 old PAL"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_OLD_PAL, NULL },
    { "C64 NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_NTSC, NULL },
    { "C64C NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64C_NTSC, NULL },
    { N_("C64 old NTSC"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_OLD_NTSC, NULL },
    { "Drean", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_PAL_N, NULL },
    { "C64 SX PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64SX_PAL, NULL },
    { "C64 SX NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64SX_NTSC, NULL },
    { "Japanese", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_JAP, NULL },
    { "C64 GS", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_GS, NULL },
    { "PET64 PAL", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_PET64_PAL, NULL },
    { "PET64 NTSC", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_PET64_NTSC, NULL },
    { "MAX Machine", UI_MENU_TYPE_TICK, (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_ULTIMAX, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(VICIIModel)

static ui_menu_entry_t set_vicii_model_submenu[] = {
    { "6569 (PAL)", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_6569, NULL },
    { "8565 (PAL)", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_8565, NULL },
    { N_("6569R1 (old PAL)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_6569R1, NULL },
    { "6567 (NTSC)", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_6567, NULL },
    { "8562 (NTSC)", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_8562, NULL },
    { N_("6567R56A (old NTSC)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_6567R56A, NULL },
    { "6572 (PAL-N)", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)VICII_MODEL_6572, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA1Model)

static ui_menu_entry_t set_cia1model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)CIA_MODEL_6526, NULL },
    { N_("6526 (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)CIA_MODEL_6526A, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA2Model)

static ui_menu_entry_t set_cia2model_submenu[] = {
    { N_("6526 (old)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)CIA_MODEL_6526, NULL },
    { N_("6526 (new)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)CIA_MODEL_6526A, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(GlueLogic)

static ui_menu_entry_t set_gluelogic_submenu[] = {
    { N_("Discrete"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_GlueLogic,
      (ui_callback_data_t)0, NULL },
    { N_("Custom IC"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_GlueLogic,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(IECReset)

static ui_menu_entry_t set_iecreset_submenu[] = {
    { N_("yes"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_IECReset,
      (ui_callback_data_t)1, NULL },
    { N_("no"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_IECReset,
      (ui_callback_data_t)0, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(KernalRev)

static ui_menu_entry_t set_kernalrev_submenu[] = {
    { N_("Rev 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_REV1, NULL },
    { N_("Rev 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_REV2, NULL },
    { N_("Rev 3"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_REV3, NULL },
    { N_("SX-64"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_SX64, NULL },
    { N_("4064"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_4064, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("other"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_KernalRev, (ui_callback_data_t)C64_KERNAL_UNKNOWN, NULL },
    { NULL }
};

static ui_menu_entry_t c64_model_submenu[] = {
    { N_("C64 model"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_c64_model_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("VIC-II model"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_vicii_model_submenu },
    { N_("SID model"), UI_MENU_TYPE_NORMAL, NULL, NULL, sid_model_submenu },
    { N_("CIA 1 model"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_cia1model_submenu },
    { N_("CIA 2 model"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_cia2model_submenu },
    { N_("Glue logic"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_gluelogic_submenu },
    { N_("Reset goes to IEC"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_iecreset_submenu },
    { N_("Kernal Revision"), UI_MENU_TYPE_NORMAL, NULL, NULL, set_kernalrev_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)
UI_MENU_DEFINE_RADIO(SidTripleAddressStart)

SID_D4XX_MENU(set_sid_stereo_address_d4xx_submenu, radio_SidStereoAddressStart)
SID_D5XX_MENU(set_sid_stereo_address_d5xx_submenu, radio_SidStereoAddressStart)
SID_D6XX_MENU(set_sid_stereo_address_d6xx_submenu, radio_SidStereoAddressStart)
SID_D7XX_MENU(set_sid_stereo_address_d7xx_submenu, radio_SidStereoAddressStart)
SID_DEXX_MENU(set_sid_stereo_address_dexx_submenu, radio_SidStereoAddressStart)
SID_DFXX_MENU(set_sid_stereo_address_dfxx_submenu, radio_SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_submenu[] = {
    { "$D4xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d4xx_submenu },
    { "$D5xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d5xx_submenu },
    { "$D6xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d6xx_submenu },
    { "$D7xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_d7xx_submenu },
    { "$DExx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

SID_D4XX_MENU(set_sid_triple_address_d4xx_submenu, radio_SidTripleAddressStart)
SID_D5XX_MENU(set_sid_triple_address_d5xx_submenu, radio_SidTripleAddressStart)
SID_D6XX_MENU(set_sid_triple_address_d6xx_submenu, radio_SidTripleAddressStart)
SID_D7XX_MENU(set_sid_triple_address_d7xx_submenu, radio_SidTripleAddressStart)
SID_DEXX_MENU(set_sid_triple_address_dexx_submenu, radio_SidTripleAddressStart)
SID_DFXX_MENU(set_sid_triple_address_dfxx_submenu, radio_SidTripleAddressStart)

static ui_menu_entry_t set_sid_triple_address_submenu[] = {
    { "$D4xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_d4xx_submenu },
    { "$D5xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_d5xx_submenu },
    { "$D6xx", UI_MENU_TYPE_NORMAL, NULL, NULL, set_sid_triple_address_d6xx_submenu },
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

UI_MENU_DEFINE_RADIO(BurstMod)

static ui_menu_entry_t burstmod_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_BurstMod, (ui_callback_data_t)0, NULL },
    { N_("CIA1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_BurstMod, (ui_callback_data_t)1, NULL },
    { N_("CIA2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_BurstMod, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(IOCollisionHandling)

static ui_menu_entry_t iocollision_submenu[] = {
    { N_("detach all"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_IOCollisionHandling, (ui_callback_data_t)0, NULL },
    { N_("detach last"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_IOCollisionHandling, (ui_callback_data_t)1, NULL },
    { N_("AND values"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_IOCollisionHandling, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SFXSoundSampler)
UI_MENU_DEFINE_TOGGLE(CartridgeReset)

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("C64 Memory Expansions Hack"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_memory_hacks_submenu },
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
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, tfe_c64_submenu },
#endif
    { CARTRIDGE_NAME_IEEE488, UI_MENU_TYPE_NORMAL,
      NULL, NULL, tpi_submenu },
    { N_("Burst Mode Modification"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, burstmod_submenu },
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
      NULL, NULL, ds12c887rtc_c64_submenu },
    { "Userport RTC", UI_MENU_TYPE_NORMAL,
      NULL, NULL, userportrtc_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("I/O collision handling"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, iocollision_submenu },
    { N_("Reset on cart change"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CartridgeReset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64ui_main_romset_submenu[] = {
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

static ui_menu_entry_t c64_romset_submenu[] = {
    { N_("Load default ROMs"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load new computer ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64ui_main_romset_submenu },
    { N_("Load new drive ROM"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_drivec64vic20_romset_submenu },
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

static ui_menu_entry_t ui_screenshot_commands_menu[] = {
    { N_("Save media file"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t keymap_sym_submenu[] = {
    { "US", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SymKeymap, (ui_callback_data_t)"x11_sym.vkm", NULL },
    { N_("German"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SymKeymap, (ui_callback_data_t)"x11_symger.vkm", NULL },
    { NULL }
};

static ui_menu_entry_t keymap_pos_submenu[] = {
    { "US", UI_MENU_TYPE_TICK, (ui_callback_t)radio_PosKeymap, (ui_callback_data_t)"x11_pos.vkm", NULL },
    { N_("German"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_PosKeymap, (ui_callback_data_t)"x11_posger.vkm", NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_menu[] = {
    { N_("Model settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_model_submenu },
    { N_("RAM reset pattern"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ui_ram_pattern_submenu },
    { N_("ROM settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_romset_submenu },
    { N_("VIC-II settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vicii_submenu },
    { N_("SID settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, sid_submenu },
    { N_("I/O extensions"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, io_extensions_submenu },
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    { N_("RS232 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uirs232_c64_submenu },
#endif
    { NULL }
};

static ui_menu_entry_t x64_left_menu[] = {
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

static ui_menu_entry_t x64_file_submenu[] = {
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
static ui_menu_entry_t x64_edit_submenu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_edit_commands_submenu },
    { NULL }
};
#endif

static ui_menu_entry_t x64_snapshot_submenu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_screenshot_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VirtualDevices)

static ui_menu_entry_t x64_settings_submenu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_runmode_commands_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, uikeyboard_settings_menu },
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_sound_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_drivec64_settings_menu },
    { N_("Printer settings"), UI_MENU_TYPE_NORMAL, 
      NULL, NULL, printeriec_settings_menu },
    { N_("Enable Virtual Devices"), UI_MENU_TYPE_TICK, 
      (ui_callback_t)toggle_VirtualDevices, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, joystick_settings_c64_menu },
#ifdef HAVE_MOUSE
    { N_("Mouse emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mouse_submenu },
    { N_("Lightpen emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, lightpen_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, c64_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t x64_main_menu[] = {
    { N_("File"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, x64_file_submenu },
#ifdef USE_GNOMEUI
    { N_("Edit"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, x64_edit_submenu },
#endif
    { N_("Snapshot"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, x64_snapshot_submenu },
    { N_("Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, x64_settings_submenu },
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

static ui_menu_entry_t x64_speed_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, ui_performance_settings_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, ui_runmode_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_tape_menu[] = {
    { "", UI_MENU_TYPE_NONE,
      NULL, NULL, uiattach_tape_menu },
    { "--", UI_MENU_TYPE_SEPARATOR,
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static void c64ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivicii_menu_create();
    uicart_menu_create();

    memcpy(uikeymap_sym_submenu, keymap_sym_submenu, sizeof(keymap_sym_submenu));
    memcpy(uikeymap_pos_submenu, keymap_pos_submenu, sizeof(keymap_pos_submenu));
}

static void c64ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uisound_menu_shutdown();
}

int c64scui_init(void)
{
    ui_set_application_icon(c64_icon_data);
    c64ui_dynamic_menu_create();

    ui_set_left_menu(x64_left_menu);
    ui_set_right_menu(x64_settings_submenu);
    ui_set_topmenu(x64_main_menu);
    ui_set_speedmenu(x64_speed_menu);
    ui_set_tape_menu(x64_tape_menu);

    ui_set_drop_callback(uiattach_autostart_file);

    ui_update_menus();

    return 0;
}

void c64scui_shutdown(void)
{
    c64ui_dynamic_menu_shutdown();
}
