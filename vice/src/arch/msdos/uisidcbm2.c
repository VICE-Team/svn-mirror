/*
 * uisidcbm2.c - CBM2 SID UI interface for MS-DOS.
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
#include "uisidcbm2.h"

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
    { NULL }
};

#ifdef HAVE_RESID
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

static TUI_MENU_CALLBACK(ui_set_ResidPassBand_callback)
{
    if (been_activated) {
        int passband, value;
        char buf[10];

        resources_get_int("SidResidPassband", &passband);
        sprintf(buf, "%d", passband);

        if (tui_input_string("ReSID passband", "Enter ReSID passband to use:", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 90) {
                value = 90;
            } else if (value < 0) {
                value = 0;
            }
            resources_set_int("SidResidPassband", value);
            tui_message("ReSID passband set to : %d",value);
        } else {
            return NULL;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ui_set_ResidGain_callback)
{
    if (been_activated) {
        int gain, value;
        char buf[10];

        resources_get_int("SidResidGain", &gain);
        sprintf(buf, "%d", gain);

        if (tui_input_string("ReSID gain", "Enter ReSID gain to use:", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 100) {
                value = 100;
            } else if (value < 90) {
                value = 90;
            }
            resources_set_int("SidResidGain", value);
            tui_message("ReSID gain set to : %d",value);
        } else {
            return NULL;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ui_set_ResidBias_callback)
{
    if (been_activated) {
        int bias, value;
        char buf[10];

        resources_get_int("SidResidFilterBias", &bias);
        sprintf(buf, "%d", bias);

        if (tui_input_string("ReSID filter bias", "Enter ReSID bia to use:", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 5000) {
                value = 5000;
            } else if (value < -5000) {
                value = -5000;
            }
            resources_set_int("SidResidFilterBias", value);
            tui_message("ReSID filter bias set to : %d",value);
        } else {
            return NULL;
        }
    }
    return NULL;
}
#endif

TUI_MENU_DEFINE_TOGGLE(SidFilters)

tui_menu_item_def_t sid_cbm2_ui_menu_items[] = {
    { "--" },
    { "SID _Engine/Model:",
      "Select the SID engine and model to emulate",
      sid_engine_model_submenu_callback, NULL, 16,
      TUI_MENU_BEH_CONTINUE, sid_engine_model_submenu, "SID engine/model" },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#ifdef HAVE_RESID
    { "--"},
    { "ReSID s_ampling method:",
      "How the reSID engine generates the samples",
      toggle_ResidSampling_callback, NULL, 12,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "ReSID passband",
      "Set the ReSID passband to use",
      ui_set_ResidPassBand_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "ReSID gain",
      "Set the ReSID gain to use",
      ui_set_ResidGain_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "ReSID filter bias",
      "Set the ReSID filter bias to use",
      ui_set_ResidBias_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};
