/*
 * cbm2ui.c - CBM2-specific user interface.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "petui.h"
#include "res.h"
#include "resources.h"
#include "sid.h"
#include "ui.h"
#include "uicbm2set.h"
#include "uidrivepetcbm2.h"
#include "uisid.h"
#include "uivideo.h"
#include "winmain.h"


static const ui_menu_toggle cbm2_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_CRTCDOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_CRTCDOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_CRTCVIDEOCACHE },
    { NULL, 0 }
};

static const ui_res_value_list cbm2_ui_res_values[] = {
    { NULL, NULL, 0 }
};

static void cbm2_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CBM2_SETTINGS:
        ui_cbm2_settings_dialog(hwnd);
        break;
      case IDM_SID_SETTINGS:
        ui_sid_settings_dialog(hwnd);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_RGB);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog(hwnd);
        break;
    }
}

int cbm2_ui_init(void)
{
    ui_register_machine_specific(cbm2_ui_specific);
    ui_register_menu_toggles(cbm2_ui_menu_toggles);
    ui_register_res_values(cbm2_ui_res_values);
    return 0;
}

