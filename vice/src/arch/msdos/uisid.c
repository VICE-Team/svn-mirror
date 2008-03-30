/*
 * uisid.c - SID UI interface for MS-DOS.
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

#include "resources.h"
#include "sid.h"
#include "tui.h"
#include "tuimenu.h"
#include "uisid.h"


TUI_MENU_DEFINE_RADIO(SidModel)

static TUI_MENU_CALLBACK(sid_model_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_value("SidModel", (void *)&value);

    switch (value) {
      case SID_MODEL_6581:
        sprintf(s, "6581");
        break;
      case SID_MODEL_8580:
        sprintf(s, "8580");
        break;
      case SID_MODEL_8580D:
        sprintf(s, "8580 + digi boost");
        break;
    }

    return s;
}

static tui_menu_item_def_t sid_model_submenu[] = {
    { "_6581",
      "SID 6581 emulation",
      radio_SidModel_callback, (void *)SID_MODEL_6581, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580",
      "SID 8580 emulation",
      radio_SidModel_callback, (void *)SID_MODEL_8580, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "8580 + _digi boost",
      "SID 8580 + digi boost emulation",
      radio_SidModel_callback, (void *)SID_MODEL_8580D, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(toggle_ResidSampling_callback)
{
    int value;

    resources_get_value("SidResidSampling", (void *)&value);
    if (been_activated) {
        value = (value + 1) % 4;
        resources_set_value("SidResidSampling", (resource_value_t) value);
    }

    return (value == 0) ? "fast" : ((value == 1) ? "interpolate" : "resample");
}


TUI_MENU_DEFINE_TOGGLE(SidFilters)

TUI_MENU_DEFINE_RADIO(SidEngine)

static TUI_MENU_CALLBACK(sid_engine_submenu_callback)
{
    int value;

    resources_get_value("SidEngine", (void *)&value);

    switch (value) {
      case SID_ENGINE_FASTSID:
        return "FastSID";
        break;
#ifdef HAVE_RESID
      case SID_ENGINE_RESID:
        return "ReSID";
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

tui_menu_item_def_t sid_ui_menu_items[] = {
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
#ifdef HAVE_RESID
    { "--" },
    { "reSID s_ampling method:",
      "How the reSID engine generates the samples",
      toggle_ResidSampling_callback, NULL, 12,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};
