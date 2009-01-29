/*
 * uisiddtv.c - DTV SID UI interface for MS-DOS.
 *
 * Written by
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
#include "uisiddtv.h"


TUI_MENU_DEFINE_RADIO(SidModel)

static TUI_MENU_CALLBACK(sid_model_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_int("SidModel", &value);

    switch (value) {
#ifdef HAVE_RESID
      case SID_MODEL_DTVSID:
        sprintf(s, "DTVSID");
        break;
#endif
      case SID_MODEL_6581:
        sprintf(s, "6581");
        break;
      case SID_MODEL_8580:
        sprintf(s, "8580");
        break;
#ifdef HAVE_RESID_FP
      case SID_MODEL_6581R3_4885:
        sprintf(s, "6581R3 4885");
        break;
      case SID_MODEL_6581R3_0486S:
        sprintf(s, "6581R3 0486S");
        break;
      case SID_MODEL_6581R3_3984:
        sprintf(s, "6581R3 3984");
        break;
      case SID_MODEL_6581R4AR_3789:
        sprintf(s, "6581R4AR 3789");
        break;
      case SID_MODEL_6581R3_4485:
        sprintf(s, "6581R3 4485");
        break;
      case SID_MODEL_6581R4_1986S:
        sprintf(s, "6581R4 1986S");
        break;
      case SID_MODEL_8580R5_3691:
        sprintf(s, "8580R5 3691");
        break;
      case SID_MODEL_8580R5_3691D:
        sprintf(s, "8580R5 3691 + digi boost");
        break;
      case SID_MODEL_8580R5_1489:
        sprintf(s, "8580R5 1489");
        break;
      case SID_MODEL_8580R5_1489D:
        sprintf(s, "8580R5 1489 + digi boost");
        break;
#endif
    }

    return s;
}

static tui_menu_item_def_t sid_model_submenu[] = {
#ifdef HAVE_RESID
    { "_DTVSID",
      "DTVSID emulation (reSID)",
      radio_SidModel_callback, (void *)SID_MODEL_DTVSID, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
    { "_6581",
      "SID 6581 emulation",
      radio_SidModel_callback, (void *)SID_MODEL_6581, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580",
      "SID 8580 emulation",
      radio_SidModel_callback, (void *)SID_MODEL_8580, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#ifdef HAVE_RESID_FP
    { "_6581R3 4885",
      "6581R3 4885 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R3_4885, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 0486S",
      "6581R3 0486S emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R3_0486S, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 3984",
      "6581R3 3984 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R3_3984, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R4AR 3789",
      "6581R4AR 3789 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R4AR_3789, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R3 4485",
      "6581R3 4485 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R3_4485, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6581R4 1986S",
      "6581R4 1986S emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_6581R4_1986S, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 3691",
      "8580R5 3691 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_8580R5_3691, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 3691 + digi boost",
      "8580R5 3691 emulation + digi boost (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_8580R5_3691D, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 1489",
      "8580R5 1489 emulation (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_8580R5_1489, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580R5 1489 + digi boost",
      "8580R5 1489 emulation + digi boost (reSID-fp)",
      radio_SidModel_callback, (void *)SID_MODEL_8580R5_1489D, 0,
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

TUI_MENU_DEFINE_RADIO(SidEngine)

static TUI_MENU_CALLBACK(sid_engine_submenu_callback)
{
    int value;

    resources_get_int("SidEngine", &value);

    switch (value) {
      case SID_ENGINE_FASTSID:
        return "FastSID";
        break;
#ifdef HAVE_RESID
      case SID_ENGINE_RESID:
        return "ReSID";
        break;
#endif
#ifdef HAVE_RESID_FP
      case SID_ENGINE_RESID_FP:
        return "ReSID-fp";
        break;
#endif
#ifdef HAVE_PARSID
      case SID_ENGINE_PARSID_PORT1:
        return "ParSID Port 1";
        break;
      case SID_ENGINE_PARSID_PORT2:
        return "ParSID Port 2";
        break;
      case SID_ENGINE_PARSID_PORT3:
        return "ParSID Port 3";
        break;
#endif
      default:
        return "Unknown";
    }
}

static tui_menu_item_def_t sid_engine_submenu[] = {
    { "_FastSID",
      "Fast SID emulation",
      radio_SidEngine_callback, (void *)SID_ENGINE_FASTSID, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#ifdef HAVE_RESID
    { "_ReSID",
      "Cycle accurate SID emulation",
      radio_SidEngine_callback, (void *)SID_ENGINE_RESID, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef HAVE_RESID_FP
    { "_ReSID-fp",
      "Cycle accurate SID emulation",
      radio_SidEngine_callback, (void *)SID_ENGINE_RESID_FP, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef HAVE_PARSID
    { "ParSID Port _1",
      "Parallel Port 1 SID adapter",
      radio_SidEngine_callback, (void *)SID_ENGINE_PARSID_PORT1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID Port _2",
      "Parallel Port 2 SID adapter",
      radio_SidEngine_callback, (void *)SID_ENGINE_PARSID_PORT2, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID Port _3",
      "Parallel Port 3 SID adapter",
      radio_SidEngine_callback, (void *)SID_ENGINE_PARSID_PORT3, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
    { NULL }
};

tui_menu_item_def_t siddtv_ui_menu_items[] = {
    { "--" },
    { "SID _Model:",
      "Select the SID model to emulate",
      sid_model_submenu_callback, NULL, 16,
      TUI_MENU_BEH_CONTINUE, sid_model_submenu, "SID model" },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Engine:",
      "Select the SID engine",
      sid_engine_submenu_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, sid_engine_submenu, "SID engine" },
#if defined(HAVE_RESID) || defined(HAVE_RESID_FP)
    { "--" },
    { "reSID/reSID-fp s_ampling method:",
      "How the reSID/reSID-fp engine generates the samples",
      toggle_ResidSampling_callback, NULL, 12,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};
