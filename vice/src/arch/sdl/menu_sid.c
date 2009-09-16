/*
 * menu_sid.c - Implementation of the SID settings menu for the SDL UI.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include <stdlib.h>

#include "lib.h"
#include "menu_common.h"
#include "menu_sid.h"
#include "resources.h"
#include "sid.h"
#include "uimenu.h"

UI_MENU_DEFINE_RADIO(SidModel)

static const ui_menu_entry_t sid_model_menu[] = {
    { "6581 (old)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581 },
    { "8580 (new)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580 },
    { "8580 + digi boost",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580D },
#ifdef HAVE_RESID_FP
    { "6581R3 4885 (very light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_4885 },
    { "6581R3 0486S (light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_0486S },
    { "6581R3 3984 (light avg)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_3984 },
    { "6581R4AR 3789 (avg)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R4AR_3789 },
    { "6581R3 4485 (dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_4485 },
    { "6581R4 1986S (very dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R4_1986S },
    { "8580 3691 (light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_3691 },
    { "8580 3691 + digi boost",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_3691D },
    { "8580 1489 (dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_1489 },
    { "8580 1489 + digi boost",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_1489D },
#endif
    { NULL }
};

static const ui_menu_entry_t sid_dtv_model_menu[] = {
    { "DTVSID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_DTVSID },
    { "6581 (old)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581 },
    { "8580 (new)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580 },
#ifdef HAVE_RESID_FP
    { "6581R3 4885 (very light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_4885 },
    { "6581R3 0486S (light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_0486S },
    { "6581R3 3984 (light avg)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_3984 },
    { "6581R4AR 3789 (avg)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R4AR_3789 },
    { "6581R3 4485 (dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R3_4485 },
    { "6581R4 1986S (very dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581R4_1986S },
    { "8580 3691 (light)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_3691 },
    { "8580 3691 + digi boost",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_3691D },
    { "8580 1489 (dark)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_1489 },
    { "8580 1489 + digi boost",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580R5_1489D },
#endif
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidEngine)

static const ui_menu_entry_t sid_engine_menu[] = {
    { "Fast SID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_FASTSID },
#ifdef HAVE_RESID
    { "ReSID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_RESID },
#endif
#ifdef HAVE_RESID_FP
    { "ReSID-FP",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_RESID_FP },
#endif
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII },
#endif
#ifdef HAVE_HARDSID
    { "HardSID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_HARDSID },
#endif
#ifdef HAVE_PARSID
    { "ParSID Port 1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1 },
    { "ParSID Port 2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2 },
    { "ParSID Port 3",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3 },
#endif
    { NULL }
};

static const ui_menu_entry_t sid_dtv_engine_menu[] = {
    { "Fast SID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_FASTSID },
#ifdef HAVE_RESID
    { "ReSID-DTV",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_RESID },
#endif
#ifdef HAVE_RESID_FP
    { "ReSID-FP",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_RESID_FP },
#endif
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII },
#endif
#ifdef HAVE_HARDSID
    { "HardSID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_HARDSID },
#endif
#ifdef HAVE_PARSID
    { "ParSID Port 1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1 },
    { "ParSID Port 2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2 },
    { "ParSID Port 3",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3 },
#endif
    { NULL }
};

static const ui_menu_entry_t sid_noresid_engine_menu[] = {
    { "Fast SID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_FASTSID },
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII },
#endif
#ifdef HAVE_HARDSID
    { "HardSID",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_HARDSID },
#endif
#ifdef HAVE_PARSID
    { "ParSID Port 1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1 },
    { "ParSID Port 2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2 },
    { "ParSID Port 3",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidEngine_callback,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3 },
#endif
    { NULL }
};

#ifdef HAVE_RESID
UI_MENU_DEFINE_RADIO(SidResidSampling)

static const ui_menu_entry_t sid_sampling_menu[] = {
    { "Fast",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidResidSampling_callback,
      (ui_callback_data_t)0 },
    { "Interpolating",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidResidSampling_callback,
      (ui_callback_data_t)1 },
    { "Resampling",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidResidSampling_callback,
      (ui_callback_data_t)2 },
    { "Fast Resampling",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidResidSampling_callback,
      (ui_callback_data_t)3 },
    { NULL }
};

UI_MENU_DEFINE_INT(SidResidPassband)
#endif

UI_MENU_DEFINE_TOGGLE(SidFilters)
UI_MENU_DEFINE_TOGGLE(SidStereo)
UI_MENU_DEFINE_RADIO(SidStereoAddressStart)

static UI_MENU_CALLBACK(show_SidStereoAddressStart_callback)
{
    static char buf[20];
    int value;

    resources_get_int("SidStereoAddressStart", &value);

    sprintf(buf,"$%04x", value);
    return buf;
}

static const ui_menu_entry_t sid_d4x0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$D420",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd420 },
    { "$D440",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd440 },
    { "$D460",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd460 },
    { "$D480",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd480 },
    { "$D4A0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd4a0 },
    { "$D4C0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd4c0 },
    { "$D4E0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd4e0 },
    { NULL }
};

static const ui_menu_entry_t sid_d5x0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$D500",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd500 },
    { "$D520",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd520 },
    { "$D540",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd540 },
    { "$D560",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd560 },
    { "$D580",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd580 },
    { "$D5A0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd5a0 },
    { "$D5C0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd5c0 },
    { "$D5E0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd5e0 },
    { NULL }
};

static const ui_menu_entry_t sid_d6x0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$D600",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd600 },
    { "$D620",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd620 },
    { "$D640",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd640 },
    { "$D660",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd660 },
    { "$D680",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd680 },
    { "$D6A0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd6a0 },
    { "$D6C0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd6c0 },
    { "$D6E0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd6e0 },
    { NULL }
};

static const ui_menu_entry_t sid_d7x0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$D700",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd700 },
    { "$D720",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd720 },
    { "$D740",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd740 },
    { "$D760",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd760 },
    { "$D780",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd780 },
    { "$D7A0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd7a0 },
    { "$D7C0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd7c0 },
    { "$D7E0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xd7e0 },
    { NULL }
};

static const ui_menu_entry_t sid_dex0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$DE00",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xde00 },
    { "$DE20",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xde20 },
    { "$DE40",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xde40 },
    { "$DE60",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xde60 },
    { "$DE80",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xde80 },
    { "$DEA0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdea0 },
    { "$DEC0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdec0 },
    { "$DEE0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdee0 },
    { NULL }
};

static const ui_menu_entry_t sid_dfx0_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$DF00",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdf00 },
    { "$DF20",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdf20 },
    { "$DF40",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdf40 },
    { "$DF60",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdf60 },
    { "$DF80",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdf80 },
    { "$DFA0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdfa0 },
    { "$DFC0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdfc0 },
    { "$DFE0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdfe0 },
    { NULL }
};

static const ui_menu_entry_t cbm2_stereo_sid_base_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$DA20",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xda20 },
    { "$DA40",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xda40 },
    { "$DA60",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xda60 },
    { "$DA80",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xda80 },
    { "$DAA0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdaa0 },
    { "$DAC0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdac0 },
    { "$DAE0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidStereoAddressStart_callback,
      (ui_callback_data_t)0xdae0 },
    { NULL }
};

static const ui_menu_entry_t c128_stereo_sid_base_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL},
    { "$D4x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d4x0_menu },
    { "$D7x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d7x0_menu },
    { "$DEx0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_dex0_menu },
    { "$DFx0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_dfx0_menu },
    { NULL }
};

static const ui_menu_entry_t c64_stereo_sid_base_menu[] = {
    { "Second SID base address",
      MENU_ENTRY_TEXT,
      show_SidStereoAddressStart_callback,
      NULL },
    { "$D4x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d4x0_menu },
    { "$D5x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d5x0_menu },
    { "$D6x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d6x0_menu },
    { "$D7x0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_d7x0_menu },
    { "$DEx0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_dex0_menu },
    { "$DFx0",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_dfx0_menu },
    { NULL }
};

const ui_menu_entry_t sid_c64_menu[] = {
    { "SID Model",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_model_menu },
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_engine_menu },
    { "Second SID",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidStereo_callback,
      NULL },
    { "Second SID base address",
      MENU_ENTRY_SUBMENU,
      show_SidStereoAddressStart_callback,
      (ui_callback_data_t)c64_stereo_sid_base_menu },
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
#ifdef HAVE_RESID
    { "reSID sampling method",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_sampling_menu },
    { "reSID resampling passband",
      MENU_ENTRY_RESOURCE_INT,
      int_SidResidPassband_callback,
      (ui_callback_data_t)"Enter passband in percentage of total bandwidth (0 - 90, lower is faster, higher is better)" },
#endif
    { NULL }
};

const ui_menu_entry_t sid_c128_menu[] = {
    { "SID Model",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_model_menu },
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_engine_menu },
    { "Second SID",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidStereo_callback,
      NULL },
    { "Second SID base address",
      MENU_ENTRY_SUBMENU,
      show_SidStereoAddressStart_callback,
      (ui_callback_data_t)c128_stereo_sid_base_menu },
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
#ifdef HAVE_RESID
    { "reSID sampling method",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_sampling_menu },
    { "reSID resampling passband",
      MENU_ENTRY_RESOURCE_INT,
      int_SidResidPassband_callback,
      (ui_callback_data_t)"Enter passband in percentage of total bandwidth (0 - 90, lower is faster, higher is better)" },
#endif
    { NULL }
};

const ui_menu_entry_t sid_cbm2_menu[] = {
    { "SID Model",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_model_menu },
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_engine_menu },
    { "Second SID",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidStereo_callback,
      NULL },
    { "Second SID base address",
      MENU_ENTRY_SUBMENU,
      show_SidStereoAddressStart_callback,
      (ui_callback_data_t)cbm2_stereo_sid_base_menu },
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
#ifdef HAVE_RESID
    { "reSID sampling method",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_sampling_menu },
    { "reSID resampling passband",
      MENU_ENTRY_RESOURCE_INT,
      int_SidResidPassband_callback,
      (ui_callback_data_t)"Enter passband in percentage of total bandwidth (0 - 90, lower is faster, higher is better)" },
#endif
    { NULL }
};

const ui_menu_entry_t sid_dtv_menu[] = {
    { "SID Model",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_dtv_model_menu },
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_dtv_engine_menu },
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
#ifdef HAVE_RESID
    { "reSID sampling method",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_sampling_menu },
    { "reSID resampling passband",
      MENU_ENTRY_RESOURCE_INT,
      int_SidResidPassband_callback,
      (ui_callback_data_t)"Enter passband in percentage of total bandwidth (0 - 90, lower is faster, higher is better)" },
#endif
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(SidCart)
UI_MENU_DEFINE_RADIO(SidAddress)
UI_MENU_DEFINE_RADIO(SidClock)

const ui_menu_entry_t sid_vic_menu[] = {
    { "Enable SID cart emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidCart_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID model"),
    { "6581 (old)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581 },
    { "8580 (new)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580 },
    SDL_MENU_ITEM_SEPARATOR,
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_noresid_engine_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID address"),
    { "$9800",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)0 },
    { "$9C00",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID clock"),
    { "C64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)0 },
    { "VIC20",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)1 },
    { NULL }
};

const ui_menu_entry_t sid_pet_menu[] = {
    { "Enable SID cart emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidCart_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID model"),
    { "6581 (old)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581 },
    { "8580 (new)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580 },
    SDL_MENU_ITEM_SEPARATOR,
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_noresid_engine_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID address"),
    { "$8F00",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)0 },
    { "$E900",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID clock"),
    { "C64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)0 },
    { "PET",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)1 },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(DIGIBLASTER)

const ui_menu_entry_t sid_plus4_menu[] = {
    { "Enable SID cart emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidCart_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID model"),
    { "6581 (old)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_6581 },
    { "8580 (new)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidModel_callback,
      (ui_callback_data_t)SID_MODEL_8580 },
    SDL_MENU_ITEM_SEPARATOR,
    { "SID Engine",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)sid_noresid_engine_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Emulate filters",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SidFilters_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID address"),
    { "$FD40",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)0 },
    { "$FE80",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidAddress_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID clock"),
    { "C64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)0 },
    { "PLUS4",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SidClock_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    { "Enable SID cart digiblaster add-on",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DIGIBLASTER_callback,
      NULL },
    { NULL }
};
