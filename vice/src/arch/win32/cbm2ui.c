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
#include "ui.h"
#include "uicbm2set.h"
#include "uidrivepetcbm2.h"
#include "uirom.h"
#include "uisid.h"
#include "uivideo.h"
#include "winmain.h"


static const ui_menu_toggle_t cbm2_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_CRTCDOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_CRTCDOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_CRTCVIDEOCACHE },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { "Kernal", "KernalName",
      IDC_CBM2ROM_KERNAL_FILE, IDC_CBM2ROM_KERNAL_BROWSE },
    { "Basic", "BasicName",
      IDC_CBM2ROM_BASIC_FILE, IDC_CBM2ROM_BASIC_BROWSE },
    { "Character", "ChargenName",
      IDC_CBM2ROM_CHARGEN_FILE, IDC_CBM2ROM_CHARGEN_BROWSE },
    { "Cart 1", "Cart1Name",
      IDC_CBM2ROM_CART1_FILE, IDC_CBM2ROM_CART1_BROWSE },
    { "Cart 2", "Cart2Name",
      IDC_CBM2ROM_CART2_FILE, IDC_CBM2ROM_CART2_BROWSE },
    { "Cart 4", "Cart4Name",
      IDC_CBM2ROM_CART4_FILE, IDC_CBM2ROM_CART4_BROWSE },
    { "Cart 6", "Cart6Name",
      IDC_CBM2ROM_CART6_FILE, IDC_CBM2ROM_CART6_BROWSE },
    { NULL, NULL, 0, 0 }
};

static const ui_res_value_list_t cbm2_ui_res_values[] = {
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
      case IDM_ROM_SETTINGS:
        uirom_settings_dialog(hwnd, IDD_C64ROM_SETTINGS_DIALOG,
                              uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_CRTC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog(hwnd);
        break;
    }
}

int cbm2ui_init(void)
{
    ui_register_machine_specific(cbm2_ui_specific);
    ui_register_menu_toggles(cbm2_ui_menu_toggles);
    ui_register_res_values(cbm2_ui_res_values);
    return 0;
}

void cbm2ui_shutdown(void)
{
}

