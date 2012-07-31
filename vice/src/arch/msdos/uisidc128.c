/*
 * uisidc128.c - C128 SID UI interface for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "resources.h"
#include "sid.h"
#include "tui.h"
#include "tuimenu.h"
#include "uisidc128.h"

static TUI_MENU_CALLBACK(sid_engine_model_submenu_callback)
{
    char *s;
    int temp;
    int value;

    resources_get_int("SidModel", &temp);
    resources_get_int("SidEngine", &value);
    value <<= 8;
    value |= temp;
    switch (value) {
        case SID_FASTSID_6581:
            s = "6581 (Fast SID)";
            break;
        case SID_FASTSID_8580:
            s = "8580 (Fast SID)";
            break;
#ifdef HAVE_RESID
        case SID_RESID_6581:
            s = "6581 (ReSID)";
            break;
        case SID_RESID_8580:
            s = "8580 (ReSID)";
            break;
        case SID_RESID_8580D:
            s = "8580 + digi boost (ReSID)";
            break;
#endif
#ifdef HAVE_PARSID
        case SID_PARSID_PORT1:
            s = "ParSID in Port 1";
            break;
        case SID_PARSID_PORT2:
            s = "ParSID in Port 2";
            break;
        case SID_PARSID_PORT3:
            s = "ParSID in Port 3";
            break;
#endif
#ifdef HAVE_RESID_FP
        case SID_RESIDFP_6581R3_4885:
            s = "6581R3 4885 (ReSID-fp)";
            break;
        case SID_RESIDFP_6581R3_0486S:
            s = "6581R3 0486S (ReSID-fp)";
            break;
        case SID_RESIDFP_6581R3_3984:
            s = "6581R3 3984 (ReSID-fp)";
            break;
        case SID_RESIDFP_6581R4AR_3789:
            s = "6581R4AR 3789 (ReSID-fp)";
            break;
        case SID_RESIDFP_6581R3_4485:
            s = "6581R3 4485 (ReSID-fp)";
            break;
        case SID_RESIDFP_6581R4_1986S:
            s = "6581R4 1986S (ReSID-fp)";
            break;
        case SID_RESIDFP_8580R5_3691:
            s = "8580R5 3691 (ReSID-fp)";
            break;
        case SID_RESIDFP_8580R5_3691D:
            s = "8580R5 3691 + digi boost (ReSID-fp)";
            break;
        case SID_RESIDFP_8580R5_1489:
            s = "8580R5 1489 (ReSID-fp)";
            break;
        case SID_RESIDFP_8580R5_1489D:
            s = "8580R5 1489 + digi boost (ReSID-fp)";
            break;
#endif
        case SID_CATWEASELMKIII:
            s = "Catweasel";
            break;
        case SID_HARDSID:
            s = "HardSID";
            break;
    }
    return s;
}

static TUI_MENU_CALLBACK(sid_radio_engine_model_callback)
{
    int engine;
    int model;

    if (been_activated) {
        engine = (int)param;
        engine >>= 8;
        model = (int)param;
        model &= 0xff;
        sid_set_engine_model(engine, model);
        *become_default = 1;
    } else {
        resource_value_t v;
        resources_get_int("SidEngine", &engine);
        resources_get_int("SidModel", &model);
        engine <<= 8;
        engine |= model;
        if (engine == (int)param) {
            *become_default = 1;
        }
    }
    return NULL;
}

static tui_menu_item_def_t sid_engine_model_submenu[] = {
    { "_6581 (Fast SID)",
      "Fast SID 6581 emulation",
      sid_radio_engine_model_callback, (void *)SID_FASTSID_6581, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580 (Fast SID)",
      "Fast SID 8580 emulation",
      sid_radio_engine_model_callback, (void *)SID_FASTSID_8580, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Catweasel",
      "Catweasel emulation",
      sid_radio_engine_model_callback, (void *)SID_CATWEASELMKIII, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_HardSID",
      "HardSID emulation",
      sid_radio_engine_model_callback, (void *)SID_HARDSID, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#ifdef HAVE_RESID
    { "_6581 (ReSID)",
      "ReSID 6581 emulation",
      sid_radio_engine_model_callback, (void *)SID_RESID_6581, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580 (ReSID)",
      "ReSID 8580 emulation",
      sid_radio_engine_model_callback, (void *)SID_RESID_8580, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "8580 + _digi boost (ReSID)",
      "ReSID 8580 + digi boost emulation",
      sid_radio_engine_model_callback, (void *)SID_RESID_8580D, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef HAVE_PARSID
    { "ParSID in Port 1",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID in Port 2",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT2, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID in Port 3",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT3, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef HAVE_RESID_FP
    { "_6581R3 4885 (ReSID-fp)",
      "6581R3 4885 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R3_4885, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 0486S (ReSID-fp)",
      "6581R3 0486S emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R3_0486S, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 3984 (ReSID-fp)",
      "6581R3 3984 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R3_3984, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R4AR 3789 (ReSID-fp)",
      "6581R4AR 3789 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R4AR_3789, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 4485 (ReSID-fp)",
      "6581R3 4485 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R3_4485, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R4 1986S (ReSID-fp)",
      "6581R4 1986S emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_6581R4_1986S, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 3691 (ReSID-fp)",
      "8580R5 3691 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_8580R5_3691, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 3691 + digi boost (ReSID-fp)",
      "8580R5 3691 emulation + digi boost (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_8580R5_3691D, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 1489 (ReSID-fp)",
      "8580R5 1489 emulation (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_8580R5_1489, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 1489 + digi boost (ReSID-fp)",
      "8580R5 1489 emulation + digi boost (reSID-fp)",
      sid_radio_engine_model_callback, (void *)SID_RESIDFP_8580R5_1489D, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
    { NULL }
};

static TUI_MENU_CALLBACK(toggle_ResidSampling_callback)
{
    int value;

    resources_get_int("SidResidSampling", &value);
    if (been_activated) {
        value = (value + 1) % 4;
        resources_set_int("SidResidSampling", value);
    }

    return (value == 0) ? "fast" : ((value == 1) ? "interpolate" : "resample");
}

TUI_MENU_DEFINE_TOGGLE(SidFilters)
TUI_MENU_DEFINE_RADIO(SidStereo)
TUI_MENU_DEFINE_RADIO(SidStereoAddressStart)
TUI_MENU_DEFINE_RADIO(SidTripleAddressStart)

static TUI_MENU_CALLBACK(sid_amount_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_int("SidStereo", &value);
    sprintf(s, "%d", value);

    return s;
}

static tui_menu_item_def_t sid_amount_submenu[] = {
    { "0", "No extra SID chips",
      radio_SidStereo_callback, (void *)0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "1", "One extra SID chip",
      radio_SidStereo_callback, (void *)1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "2", "Two extra SID chips",
      radio_SidStereo_callback, (void *)2, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(sid_stereo_address_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_int("SidStereoAddressStart", &value);
    sprintf(s, "%04X", value);

    return s;
}

static TUI_MENU_CALLBACK(sid_triple_address_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_int("SidTripleAddressStart", &value);
    sprintf(s, "%04X", value);

    return s;
}

static tui_menu_item_def_t sid_stereo_address_d4xx_submenu[] = {
    { "$D420", "$D420",
      radio_SidStereoAddressStart_callback, (void *)0xd420, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D440", "$D440",
      radio_SidStereoAddressStart_callback, (void *)0xd440, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D460", "$D460",
      radio_SidStereoAddressStart_callback, (void *)0xd460, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D480", "$D480",
      radio_SidStereoAddressStart_callback, (void *)0xd480, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4A0", "$D4A0",
      radio_SidStereoAddressStart_callback, (void *)0xd4a0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4C0", "$D4C0",
      radio_SidStereoAddressStart_callback, (void *)0xd4c0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4E0", "$D4E0",
      radio_SidStereoAddressStart_callback, (void *)0xd4e0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_triple_address_d4xx_submenu[] = {
    { "$D420", "$D420",
      radio_SidTripleAddressStart_callback, (void *)0xd420, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D440", "$D440",
      radio_SidTripleAddressStart_callback, (void *)0xd440, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D460", "$D460",
      radio_SidTripleAddressStart_callback, (void *)0xd460, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D480", "$D480",
      radio_SidTripleAddressStart_callback, (void *)0xd480, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4A0", "$D4A0",
      radio_SidTripleAddressStart_callback, (void *)0xd4a0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4C0", "$D4C0",
      radio_SidTripleAddressStart_callback, (void *)0xd4c0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D4E0", "$D4E0",
      radio_SidTripleAddressStart_callback, (void *)0xd4e0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_stereo_address_d7xx_submenu[] = {
    { "$D700", "$D700",
      radio_SidStereoAddressStart_callback, (void *)0xd700, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D720", "$D720",
      radio_SidStereoAddressStart_callback, (void *)0xd720, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D740", "$D740",
      radio_SidStereoAddressStart_callback, (void *)0xd740, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D760", "$D760",
      radio_SidStereoAddressStart_callback, (void *)0xd760, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D780", "$D780",
      radio_SidStereoAddressStart_callback, (void *)0xd780, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7A0", "$D7A0",
      radio_SidStereoAddressStart_callback, (void *)0xd7a0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7C0", "$D7C0",
      radio_SidStereoAddressStart_callback, (void *)0xd7c0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7E0", "$D7E0",
      radio_SidStereoAddressStart_callback, (void *)0xd7e0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_triple_address_d7xx_submenu[] = {
    { "$D700", "$D700",
      radio_SidTripleAddressStart_callback, (void *)0xd700, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D720", "$D720",
      radio_SidTripleAddressStart_callback, (void *)0xd720, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D740", "$D740",
      radio_SidTripleAddressStart_callback, (void *)0xd740, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D760", "$D760",
      radio_SidTripleAddressStart_callback, (void *)0xd760, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D780", "$D780",
      radio_SidTripleAddressStart_callback, (void *)0xd780, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7A0", "$D7A0",
      radio_SidTripleAddressStart_callback, (void *)0xd7a0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7C0", "$D7C0",
      radio_SidTripleAddressStart_callback, (void *)0xd7c0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D7E0", "$D7E0",
      radio_SidTripleAddressStart_callback, (void *)0xd7e0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_stereo_address_dexx_submenu[] = {
    { "$DE00", "$DE00",
      radio_SidStereoAddressStart_callback, (void *)0xde00, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE20", "$DE20",
      radio_SidStereoAddressStart_callback, (void *)0xde20, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE40", "$DE40",
      radio_SidStereoAddressStart_callback, (void *)0xde40, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE60", "$DE60",
      radio_SidStereoAddressStart_callback, (void *)0xde60, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE80", "$DE80",
      radio_SidStereoAddressStart_callback, (void *)0xde80, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEA0", "$DEA0",
      radio_SidStereoAddressStart_callback, (void *)0xdea0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEC0", "$DEC0",
      radio_SidStereoAddressStart_callback, (void *)0xdec0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEE0", "$DEE0",
      radio_SidStereoAddressStart_callback, (void *)0xdee0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_triple_address_dexx_submenu[] = {
    { "$DE00", "$DE00",
      radio_SidTripleAddressStart_callback, (void *)0xde00, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE20", "$DE20",
      radio_SidTripleAddressStart_callback, (void *)0xde20, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE40", "$DE40",
      radio_SidTripleAddressStart_callback, (void *)0xde40, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE60", "$DE60",
      radio_SidTripleAddressStart_callback, (void *)0xde60, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE80", "$DE80",
      radio_SidTripleAddressStart_callback, (void *)0xde80, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEA0", "$DEA0",
      radio_SidTripleAddressStart_callback, (void *)0xdea0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEC0", "$DEC0",
      radio_SidTripleAddressStart_callback, (void *)0xdec0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEE0", "$DEE0",
      radio_SidTripleAddressStart_callback, (void *)0xdee0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_stereo_address_dfxx_submenu[] = {
    { "$DF00", "$DF00",
      radio_SidStereoAddressStart_callback, (void *)0xdf00, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF20", "$DF20",
      radio_SidStereoAddressStart_callback, (void *)0xdf20, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF40", "$DF40",
      radio_SidStereoAddressStart_callback, (void *)0xdf40, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF60", "$DF60",
      radio_SidStereoAddressStart_callback, (void *)0xdf60, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF80", "$DF80",
      radio_SidStereoAddressStart_callback, (void *)0xdf80, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFA0", "$DFA0",
      radio_SidStereoAddressStart_callback, (void *)0xdfa0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFC0", "$DFC0",
      radio_SidStereoAddressStart_callback, (void *)0xdfc0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFE0", "$DFE0",
      radio_SidStereoAddressStart_callback, (void *)0xdfe0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_triple_address_dfxx_submenu[] = {
    { "$DF00", "$DF00",
      radio_SidTripleAddressStart_callback, (void *)0xdf00, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF20", "$DF20",
      radio_SidTripleAddressStart_callback, (void *)0xdf20, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF40", "$DF40",
      radio_SidTripleAddressStart_callback, (void *)0xdf40, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF60", "$DF60",
      radio_SidTripleAddressStart_callback, (void *)0xdf60, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF80", "$DF80",
      radio_SidTripleAddressStart_callback, (void *)0xdf80, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFA0", "$DFA0",
      radio_SidTripleAddressStart_callback, (void *)0xdfa0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFC0", "$DFC0",
      radio_SidTripleAddressStart_callback, (void *)0xdfc0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFE0", "$DFE0",
      radio_SidTripleAddressStart_callback, (void *)0xdfe0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sid_stereo_address_submenu[] = {
    { "$D4xx:",
      "Stereo SID in the $D4xx range",
      sid_stereo_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_stereo_address_d4xx_submenu, "Stereo SID in the $D4xx range" },
    { "$D7xx:",
      "Stereo SID in the $D7xx range",
      sid_stereo_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_stereo_address_d7xx_submenu, "Stereo SID in the $D7xx range" },
    { "$DExx:",
      "Stereo SID in the $DExx range",
      sid_stereo_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_stereo_address_dexx_submenu, "Stereo SID in the $DExx range" },
    { "$DFxx:",
      "Stereo SID in the $DFxx range",
      sid_stereo_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_stereo_address_dfxx_submenu, "Stereo SID in the $DFxx range" },
    { NULL }
};

static tui_menu_item_def_t sid_triple_address_submenu[] = {
    { "$D4xx:",
      "Triple SID in the $D4xx range",
      sid_triple_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_triple_address_d4xx_submenu, "Triple SID in the $D4xx range" },
    { "$D7xx:",
      "Triple SID in the $D7xx range",
      sid_triple_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_triple_address_d7xx_submenu, "Triple SID in the $D7xx range" },
    { "$DExx:",
      "Triple SID in the $DExx range",
      sid_triple_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_triple_address_dexx_submenu, "Triple SID in the $DExx range" },
    { "$DFxx:",
      "Triple SID in the $DFxx range",
      sid_triple_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_triple_address_dfxx_submenu, "Triple SID in the $DFxx range" },
    { NULL }
};

tui_menu_item_def_t sid_c128_ui_menu_items[] = {
    { "--" },
    { "SID _Engine/Model:",
      "Select the SID engine and model to emulate",
      sid_engine_model_submenu_callback, NULL, 16,
      TUI_MENU_BEH_CONTINUE, sid_engine_model_submenu, "SID engine/model" },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--"},
    { "Extra SID amount:",
      "Select the amount of extra SID chips",
      sid_amount_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_amount_submenu, "Amount of extra SID chips" },
    { "Stereo SID address:",
      "Select the address of the second SID chip",
      sid_stereo_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_stereo_address_submenu, "Address of the second SID chip" },
    { "Triple SID address:",
      "Select the address of the third SID chip",
      sid_triple_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_triple_address_submenu, "Address of the third SID chip" },
#if defined(HAVE_RESID) || defined(HAVE_RESID_FP)
    { "--"},
    { "reSID/reSID-fp s_ampling method:",
      "How the reSID/reSID-fp engine generates the samples",
      toggle_ResidSampling_callback, NULL, 12,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};
