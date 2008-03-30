/*
 * sidui.c - SID UI interface for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include "sidui.h"
#include "tui.h"
#include "tuimenu.h"

static TUI_MENU_CALLBACK(toggle_SidModel_callback)
{
    int value;

    if (been_activated) {
        resources_toggle("SidModel", (resource_value_t *) &value);
    } else {
        resources_get_value("SidModel", (void *)&value);
    }

    return value ? "8580 (New)" : "6581 (Old)";
}

static TUI_MENU_CALLBACK(toggle_ResidSampling_callback)
{
    int value;

    resources_get_value("SidResidSampling", (void *)&value);
    if (been_activated) {
        value = (value + 1) % 3;
        resources_set_value("SidResidSampling", (resource_value_t) value);
    }

    return (value==0) ? "fast" : ((value==1)?"interpolate":"resample");
}


TUI_MENU_DEFINE_TOGGLE(SidFilters)

#ifdef HAVE_RESID
TUI_MENU_DEFINE_TOGGLE(SidUseResid)
#endif

tui_menu_item_def_t sid_ui_menu_items[] = {
    { "--" },
    { "SID _Model:",
      "Select the SID model to emulate",
      toggle_SidModel_callback, NULL, 10,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#ifdef HAVE_RESID
    { "_Hi-Fi reSID engine:",
      "Enable/disable usage of the slower reSID engine",
      toggle_SidUseResid_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "reSID s_ampling method:",
      "How the reSID engine generates the samples",
      toggle_ResidSampling_callback, NULL, 12,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};
