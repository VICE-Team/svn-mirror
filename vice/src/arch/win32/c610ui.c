/*
 * c610ui.c - C610-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <windows.h>
#include <windowsx.h>

#include "petui.h"
#include "res.h"
#include "ui.h"
#include "uicbm2set.h"
#include "winmain.h"

ui_menu_toggle  cbm2_ui_menu_toggles[]={
    { "CrtcDoubleSize", IDM_TOGGLE_CRTCDOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_CRTCDOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_CRTCVIDEOCACHE },
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

ui_res_value_list cbm2_ui_res_values[] = {
    {"SidModel", SidType},
    {NULL,NULL}
};

void cbm2_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CBM2_SETTINGS:
        ui_cbm2_settings_dialog(hwnd);
    }
}

int c610_ui_init(void)
{
    ui_register_machine_specific(cbm2_ui_specific);
    ui_register_menu_toggles(cbm2_ui_menu_toggles);
    ui_register_res_values(cbm2_ui_res_values);
    return 0;
}

