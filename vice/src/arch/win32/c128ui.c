/*
 * c128ui.c - C128-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "c128ui.h"
#include "res.h"
#include "ui.h"
#include "uivicii.h"

ui_menu_toggle  c128_ui_menu_toggles[]={
    { "DoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "DoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "REU", IDM_TOGGLE_REU },
    { "IEEE488", IDM_IEEE488 },
    { "Mouse", IDM_MOUSE },
    { "SidFilters", IDM_TOGGLE_SIDFILTERS },
#ifdef HAVE_RESID
    { "SidUseResid", IDM_TOGGLE_SOUND_RESID },
#endif
    { NULL, 0 }
};

static ui_res_possible_values SidType[] = {
    {0, IDM_SIDTYPE_6581},
    {1, IDM_SIDTYPE_8580},
    {-1,0}
};

ui_res_value_list c128_ui_res_values[] = {
    {"SidModel", SidType},
    {NULL,NULL}
};

void c128_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_VICII_SETTINGS:
        ui_vicii_settings_dialog(hwnd);
        break;
    }
}

int c128_ui_init(void)
{
    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);
    ui_register_res_values(c128_ui_res_values);
    return 0;
}

