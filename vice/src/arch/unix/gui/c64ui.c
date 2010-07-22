/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "c64model.h"
#include "debug.h"
#include "icon.h"
#include "machine.h"
#include "machine-video.h"
#include "resources.h"
#include "sid.h"
#include "uiapi.h"
#include "uiattach.h"
#include "uic64_256k.h"
#include "uic64cart.h"
#include "uicommands.h"
#include "uidatasette.h"
#include "uidigimax.h"
#include "uidqbb.h"
#include "uidrive.h"
#include "uidrivec64.h"
#include "uidrivec64vic20.h"
#include "uieasyflash.h"
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
#include "uivicii.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */

struct model_s {
    int video;
    int luma;
    int cia;
    int sid;
    int sidtype;
};

static struct model_s c64models[] = {
    {MACHINE_SYNC_PAL,     1, 0, 0, SID_MODEL_6581R4AR_3789 },
    {MACHINE_SYNC_PAL,     1, 1, 1, SID_MODEL_8580R5_3691   },
    {MACHINE_SYNC_PAL,     0, 0, 0, SID_MODEL_6581R4AR_3789 },
    {MACHINE_SYNC_NTSC,    1, 0, 0, SID_MODEL_6581R4AR_3789 },
    {MACHINE_SYNC_NTSC,    1, 1, 1, SID_MODEL_8580R5_3691   },
    {MACHINE_SYNC_NTSCOLD, 0, 0, 0, SID_MODEL_6581R4AR_3789 },
    {MACHINE_SYNC_PALN,    1, 0, 0, SID_MODEL_6581R4AR_3789 }
};

static int is_new_sid(int model)
{
    switch (model) {
        case SID_MODEL_6581:
        case SID_MODEL_6581R4:
        case SID_MODEL_6581R3_4885:
        case SID_MODEL_6581R3_0486S:
        case SID_MODEL_6581R3_3984:
        case SID_MODEL_6581R4AR_3789:
        case SID_MODEL_6581R3_4485:
        case SID_MODEL_6581R4_1986S:
        default:
            return 0;

        case SID_MODEL_8580:
        case SID_MODEL_8580D:
        case SID_MODEL_8580R5_3691:
        case SID_MODEL_8580R5_3691D:
        case SID_MODEL_8580R5_1489:
        case SID_MODEL_8580R5_1489D:
            return 1;
    }
}

static int model_get_temp(int video, int sid_model,
                      int cia1_model, int cia2_model, int new_luma)
{
    int new_sid;
    int i;

    if (cia1_model != cia2_model) {
        return C64MODEL_UNKNOWN;
    }

    new_sid = is_new_sid(sid_model);

    for (i = 0; i < C64MODEL_NUM; ++i) {
        if ((c64models[i].video == video)
         && (c64models[i].luma == new_luma)
         && (c64models[i].cia == cia1_model)
         && (c64models[i].sid == new_sid)) {
            return i;
        }
    }

    return C64MODEL_UNKNOWN;
}

static int model_get(void)
{
    int video, sid_model, cia1_model, cia2_model, new_luma;

    if ((resources_get_int("MachineVideoStandard", &video) < 0)
     || (resources_get_int("SidModel", &sid_model) < 0)
     || (resources_get_int("CIA1Model", &cia1_model) < 0)
     || (resources_get_int("CIA2Model", &cia2_model) < 0)
     || (resources_get_int("VICIINewLuminances", &new_luma) < 0)) {
        return -1;
    }

    return model_get_temp(video, sid_model,
                            cia1_model, cia2_model, new_luma);
}

static void model_set(int model)
{
    int old_model;
    int old_engine;
    int old_sid_model;
    int old_type;
    int new_sid_model;
    int new_type;

    old_model = model_get();

    if ((model == old_model) || (model == C64MODEL_UNKNOWN)) {
        return;
    }

    resources_set_int("MachineVideoStandard", c64models[model].video);
    resources_set_int("CIA1Model", c64models[model].cia);
    resources_set_int("CIA2Model", c64models[model].cia);
    resources_set_int("VICIINewLuminances", c64models[model].luma);

    /* Only change the SID model if the model changes from 6581 to 8580
       This allows to switch between "pal"/"oldpal" without changing the specific SID model. */
    resources_get_int("SidEngine", &old_engine);
    resources_get_int("SidModel", &old_sid_model);
    if (old_engine == SID_ENGINE_RESID_FP) {
        new_sid_model = c64models[model].sidtype;
    } else {
        new_sid_model = c64models[model].sid;
    }

    old_type = is_new_sid(old_sid_model);
    new_type = is_new_sid(new_sid_model);

    if (old_type != new_type) {
        sid_set_engine_model(old_engine, new_sid_model);
    }
}

#define VICMODEL_UNKNOWN -1
#define VICMODEL_NUM 5

struct vicmodel_s {
    int video;
    int luma;
};

static struct vicmodel_s vicmodels[] = {
    {MACHINE_SYNC_PAL,     1 },
    {MACHINE_SYNC_PAL,     0 },
    {MACHINE_SYNC_NTSC,    1 },
    {MACHINE_SYNC_NTSCOLD, 0 },
    {MACHINE_SYNC_PALN,    1 }
};

static int vicmodel_get_temp(int video,int new_luma)
{
    int i;

    for (i = 0; i < VICMODEL_NUM; ++i) {
        if ((vicmodels[i].video == video)
         && (vicmodels[i].luma == new_luma)) {
            return i;
        }
    }

    return VICMODEL_UNKNOWN;
}

static int vicmodel_get(void)
{
    int video, new_luma;

    if ((resources_get_int("MachineVideoStandard", &video) < 0)
     || (resources_get_int("VICIINewLuminances", &new_luma) < 0)) {
        return -1;
    }

    return vicmodel_get_temp(video, new_luma);
}

static void vicmodel_set(int model)
{
    int old_model;

    old_model = vicmodel_get();

    if ((model == old_model) || (model == VICMODEL_UNKNOWN)) {
        return;
    }

    resources_set_int("MachineVideoStandard", vicmodels[model].video);
    resources_set_int("VICIINewLuminances", vicmodels[model].luma);
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(radio_c64model)
{
    int model, selected;

    selected = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (!CHECK_MENUS) {
        model_set(selected);
        ui_update_menus();
    } else {
        model = model_get();

        if (selected == model) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }
}

static ui_menu_entry_t set_c64_model_submenu[] = {
    { "*C64 PAL", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_PAL, NULL },
    { "*C64C PAL", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64C_PAL, NULL },
    { "*C64 old PAL", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_OLD_PAL, NULL },
    { "*C64 NTSC", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_NTSC, NULL },
    { "*C64C NTSC", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64C_NTSC, NULL },
    { "*C64 old NTSC", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_OLD_NTSC, NULL },
    { "*Drean", (ui_callback_t)radio_c64model,
      (ui_callback_data_t)C64MODEL_C64_PAL_N, NULL },
    { NULL }
};

static UI_CALLBACK(noop_video_standard)
{
    return;
}

static ui_menu_entry_t set_video_standard_c64_submenu[] = {
    /* PAL/NTSC switching is handled via VICII (or C64) model */
    { N_("Switch using C64 or VIC-II model"), (ui_callback_t)noop_video_standard,
      (ui_callback_data_t)0, NULL },
    { NULL }
};

static UI_CALLBACK(radio_VICIIModel)
{
    int model, selected;

    selected = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (!CHECK_MENUS) {
        vicmodel_set(selected);
        ui_update_menus();
    } else {
        model = vicmodel_get();

        if (selected == model) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }
}

static ui_menu_entry_t set_vicii_model_submenu[] = {
    { "*PAL-G", (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)0, NULL },
    { "*Old PAL-G", (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)1, NULL },
    { "*NTSC-M", (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)2, NULL },
    { "*Old NTSC-M", (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)3, NULL },
    { "*PAL-N", (ui_callback_t)radio_VICIIModel,
      (ui_callback_data_t)4, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA1Model)

static ui_menu_entry_t set_cia1model_submenu[] = {
    { N_("*6526 (old)"), (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)0, NULL },
    { N_("*6526A (new)"), (ui_callback_t)radio_CIA1Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(CIA2Model)

static ui_menu_entry_t set_cia2model_submenu[] = {
    { N_("*6526 (old)"), (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)0, NULL },
    { N_("*6526A (new)"), (ui_callback_t)radio_CIA2Model,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static ui_menu_entry_t c64_model_submenu[] = {
    { N_("C64 model"),
      NULL, NULL, set_c64_model_submenu },
    { "--" },
    { N_("VIC-II model"),
      NULL, NULL, set_vicii_model_submenu },
    { N_("SID model"),
      NULL, NULL, sid_model_submenu },
    { N_("CIA 1 model"),
      NULL, NULL, set_cia1model_submenu },
    { N_("CIA 2 model"),
      NULL, NULL, set_cia2model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(SidStereoAddressStart)

static ui_menu_entry_t set_sid_stereo_address_d4xx_submenu[] = {
    { "*$D420", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd420, NULL },
    { "*$D440", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd440, NULL },
    { "*$D460", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd460, NULL },
    { "*$D480", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd480, NULL },
    { "*$D4A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4a0, NULL },
    { "*$D4C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4c0, NULL },
    { "*$D4E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd4e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d5xx_submenu[] = {
    { "*$D500", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd500, NULL },
    { "*$D520", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd520, NULL },
    { "*$D540", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd540, NULL },
    { "*$D560", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd560, NULL },
    { "*$D580", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd580, NULL },
    { "*$D5A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5a0, NULL },
    { "*$D5C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5c0, NULL },
    { "*$D5E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd5e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d6xx_submenu[] = {
    { "*$D600", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd600, NULL },
    { "*$D620", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd620, NULL },
    { "*$D640", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd640, NULL },
    { "*$D660", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd660, NULL },
    { "*$D680", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd680, NULL },
    { "*$D6A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6a0, NULL },
    { "*$D6C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6c0, NULL },
    { "*$D6E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd6e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_d7xx_submenu[] = {
    { "*$D700", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd700, NULL },
    { "*$D720", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd720, NULL },
    { "*$D740", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd740, NULL },
    { "*$D760", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd760, NULL },
    { "*$D780", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd780, NULL },
    { "*$D7A0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7a0, NULL },
    { "*$D7C0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7c0, NULL },
    { "*$D7E0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xd7e0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dexx_submenu[] = {
    { "*$DE00", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde00, NULL },
    { "*$DE20", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde20, NULL },
    { "*$DE40", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde40, NULL },
    { "*$DE60", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde60, NULL },
    { "*$DE80", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xde80, NULL },
    { "*$DEA0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdea0, NULL },
    { "*$DEC0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdec0, NULL },
    { "*$DEE0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdee0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_dfxx_submenu[] = {
    { "*$DF00", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf00, NULL },
    { "*$DF20", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf20, NULL },
    { "*$DF40", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf40, NULL },
    { "*$DF60", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf60, NULL },
    { "*$DF80", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdf80, NULL },
    { "*$DFA0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfa0, NULL },
    { "*$DFC0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfc0, NULL },
    { "*$DFE0", (ui_callback_t)radio_SidStereoAddressStart,
      (ui_callback_data_t)0xdfe0, NULL },
    { NULL }
};

static ui_menu_entry_t set_sid_stereo_address_submenu[] = {
    { "$D4xx",
      NULL, NULL, set_sid_stereo_address_d4xx_submenu },
    { "$D5xx",
      NULL, NULL, set_sid_stereo_address_d5xx_submenu },
    { "$D6xx",
      NULL, NULL, set_sid_stereo_address_d6xx_submenu },
    { "$D7xx",
      NULL, NULL, set_sid_stereo_address_d7xx_submenu },
    { "$DExx",
      NULL, NULL, set_sid_stereo_address_dexx_submenu },
    { "$DFxx",
      NULL, NULL, set_sid_stereo_address_dfxx_submenu },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_TOGGLE(SidFilters)

static ui_menu_entry_t sid_submenu[] = {
    { N_("*Second SID"),
      (ui_callback_t)toggle_SidStereo, NULL, NULL },
    { N_("*Second SID base address"),
      NULL, NULL, set_sid_stereo_address_submenu },
    { "--" },
    { N_("*Emulate filters"),
      (ui_callback_t)toggle_SidFilters, NULL, NULL },
#ifdef HAVE_RESID
    { "--" },
    { N_("reSID sampling method"),
      NULL, NULL, sid_resid_sampling_submenu },
    { N_("reSID resampling passband..."),
      (ui_callback_t)set_sid_resid_passband, NULL, NULL },
#endif
    { NULL },
};

static ui_menu_entry_t model_options_submenu[] = {
    { N_("C64 model"),
      NULL, NULL, set_c64_model_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(SFXSoundSampler)
UI_MENU_DEFINE_TOGGLE(CartridgeReset)

static ui_menu_entry_t io_extensions_submenu[] = {
    { N_("256K RAM Expansion"),
      NULL, NULL, c64_256k_submenu },
    { N_("GEORAM Expansion Unit"),
      NULL, NULL, georam_submenu },
    { N_("PLUS60K RAM Expansion"),
      NULL, NULL, plus60k_submenu },
    { N_("PLUS256K RAM Expansion"),
      NULL, NULL, plus256k_submenu },
    { N_("RAM Expansion Cart"),
      NULL, NULL, ramcart_submenu },
    { N_("RAM Expansion Unit"),
      NULL, NULL, reu_submenu },
    { "--" },
    { N_("Double Quick Brown Box cartridge"),
      NULL, NULL, dqbb_submenu },
    { N_("Expert cartridge"),
      NULL, NULL, expert_submenu },
    { N_("ISEPIC cartridge"),
      NULL, NULL, isepic_submenu },
    { "--" },
    { N_("EasyFlash cartridge"),
      NULL, NULL, easyflash_submenu },
    { N_("IDE64 emulation"),
      NULL, NULL, ide64_submenu },
    { N_("MMC64 emulation"),
      NULL, NULL, mmc64_submenu },
    { N_("MMC Replay emulation"),
      NULL, NULL, mmcreplay_submenu },
    { N_("Retro Replay cartridge"),
      NULL, NULL, retroreplay_submenu },
    { "--" },
#ifdef HAVE_TFE
    { N_("Ethernet emulation"),
      NULL, NULL, tfe_submenu },
#endif
    { N_("IEEE 488 Interface"),
      NULL, NULL, tpi_submenu },
#ifdef HAVE_MOUSE
    { "--" },
    { N_("*Mouse Emulation"),
      NULL, NULL, mouse_submenu },
    { N_("*Lightpen Emulation"),
      NULL, NULL, lightpen_submenu },
#endif
    { "--" },
    { N_("Digimax Cart"),
      NULL, NULL, digimax_submenu },
    { N_("Magic Voice"),
      NULL, NULL, magicvoice_submenu },
#ifdef HAVE_MIDI
    { N_("MIDI Emulation"),
      NULL, NULL, midi_c64_submenu },
#endif
    { N_("*SFX Sound Expander emulation"),
      NULL, NULL, soundexpander_submenu },
    { N_("*SFX Sound Sampler emulation"),
      (ui_callback_t)toggle_SFXSoundSampler, NULL, NULL },
    { "--" },
    { N_("*Emulator identification"),
      (ui_callback_t)toggle_EmuID, NULL, NULL },
    { N_("*Power Off on Cartridge Change"),
      (ui_callback_t)toggle_CartridgeReset, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64ui_main_romset_submenu[] = {
    { N_("Load new kernal ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"KernalName", NULL },
    { N_("Load new BASIC ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"BasicName", NULL },
    { N_("Load new character ROM"),
      (ui_callback_t)ui_load_rom_file,
      (ui_callback_data_t)"ChargenName", NULL },
    { NULL }
};

static ui_menu_entry_t c64_romset_submenu[] = {
    { N_("Load default ROMs"),
      (ui_callback_t)ui_set_romset,
      (ui_callback_data_t)"default.vrs", NULL },
    { "--" },
    { N_("Load new computer ROM"),
      NULL, NULL, c64ui_main_romset_submenu },
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
    { N_("Save media file"),
      (ui_callback_t)save_screenshot, (ui_callback_data_t)0, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t c64_menu[] = {
    { N_("Model settings"),
      NULL, NULL, c64_model_submenu },
    { N_("ROM settings"),
      NULL, NULL, c64_romset_submenu },
    { N_("VIC-II settings"),
      NULL, NULL, vicii_submenu },
    { N_("SID settings"),
      NULL, NULL, sid_submenu },
    { N_("I/O extensions"),
      NULL, NULL, io_extensions_submenu },
    { N_("RS232 settings"),
      NULL, NULL, uirs232c64c128_submenu },
    { NULL }
};

static ui_menu_entry_t x64_left_menu[] = {
    { "",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, ui_c64cart_commands_menu },
    { "",
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

static ui_menu_entry_t x64_right_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec64_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_c64_menu },
    { "--",
      NULL, NULL, c64_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
#ifdef DEBUG
    { "--",
      NULL, NULL, ui_debug_settings_menu },
#endif
    { NULL }
};

static ui_menu_entry_t x64_file_submenu[] = {
    { "",
      NULL, NULL, uiattach_smart_attach_menu },
    { "--",
      NULL, NULL, uiattach_disk_menu },
    { "--",
      NULL, NULL, uiattach_tape_menu },
    { "",
      NULL, NULL, ui_datasette_commands_menu },
    { "--",
      NULL, NULL, ui_c64cart_commands_menu },
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

static ui_menu_entry_t x64_snapshot_submenu[] = {
    { "",
      NULL, NULL, ui_snapshot_commands_submenu },
    { "--",
      NULL, NULL, ui_screenshot_commands_menu },
    { "--",
      NULL, NULL, ui_sound_record_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_options_submenu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--",
      NULL, NULL, joystick_options_submenu },
    { "--",
      NULL, NULL, model_options_submenu },
    { "--",
      NULL, NULL, io_extensions_submenu },
    { NULL }
};

static ui_menu_entry_t x64_settings_submenu[] = {
    { "",
      NULL, NULL, uikeyboard_settings_menu },
    { "",
      NULL, NULL, ui_sound_settings_menu },
    { "",
      NULL, NULL, ui_drivec64_settings_menu },
    { "",
      NULL, NULL, ui_peripheraliec_settings_menu },
    { "",
      NULL, NULL, joystick_settings_c64_menu },
    { "--",
      NULL, NULL, c64_menu },
    { "--",
      NULL, NULL, ui_settings_settings_menu },
    { NULL }
};

static ui_menu_entry_t x64_main_menu[] = {
    { N_("File"),
      NULL, NULL, x64_file_submenu },
    { N_("Snapshot"),
      NULL, NULL, x64_snapshot_submenu },
    { N_("Options"),
      NULL, NULL, x64_options_submenu },
    { N_("Settings"),
      NULL, NULL, x64_settings_submenu },
                   /* Translators: RJ means right justify and should be
                      saved in your tranlation! e.g. german "RJHilfe" */
    { N_("RJHelp"),
      NULL, NULL, ui_help_commands_menu },
    { NULL }
};

static ui_menu_entry_t x64_speed_menu[] = {
    { "",
      NULL, NULL, ui_performance_settings_menu },
    { "--" },
    { "--" },
    { NULL }
};

static ui_menu_entry_t x64_tape_menu[] = {
    { "",
      NULL, NULL, uiattach_tape_menu },
    { "--",
      NULL, NULL, datasette_control_submenu },
    { NULL }
};

static void c64ui_dynamic_menu_create(void)
{
    uisound_menu_create();
    uivicii_menu_create();
}

static void c64ui_dynamic_menu_shutdown(void)
{
    uivicii_menu_shutdown();
    uisound_menu_shutdown();
}

int c64ui_init(void)
{
    memcpy(set_video_standard_submenu, set_video_standard_c64_submenu, sizeof(set_video_standard_c64_submenu));

    ui_set_application_icon(c64_icon_data);
    c64ui_dynamic_menu_create();

    ui_set_left_menu(x64_left_menu);
    ui_set_right_menu(x64_right_menu);
    ui_set_topmenu(x64_main_menu);
    ui_set_speedmenu(x64_speed_menu);
    ui_set_tape_menu(x64_tape_menu);

    ui_update_menus();

    return 0;
}

void c64ui_shutdown(void)
{
    c64ui_dynamic_menu_shutdown();
}
