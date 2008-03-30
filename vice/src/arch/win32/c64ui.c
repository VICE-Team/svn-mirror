/*
 * c64ui.c - C64-specific user interface.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@axelero.hu>
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
#include <stdlib.h>
#include <windows.h>

#include "c64ui.h"
#include "res.h"
#include "ui.h"
#include "uic64cart.h"
#include "uidrivec64vic20.h"
#include "uiide64.h"
#include "uireu.h"
#include "uirom.h"
#include "uisid.h"
#include "uitfe.h"
#include "uivicii.h"
#include "uivideo.h"


static const ui_menu_toggle_t c64_ui_menu_toggles[] = {
    { "VICIIDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICIIScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "VICIIVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "Mouse", IDM_MOUSE },
    { "Mouse", IDM_MOUSE | 0x00010000 },
    { "CartridgeReset", IDM_TOGGLE_CART_RESET },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { "Kernal", "KernalName",
      IDC_C64ROM_KERNAL_FILE, IDC_C64ROM_KERNAL_BROWSE },
    { "Basic", "BasicName",
      IDC_C64ROM_BASIC_FILE, IDC_C64ROM_BASIC_BROWSE },
    { "Character", "ChargenName",
      IDC_C64ROM_CHARGEN_FILE, IDC_C64ROM_CHARGEN_BROWSE },
    { NULL, NULL, 0, 0 }
};


static void c64_ui_specific(WPARAM wparam, HWND hwnd)
{
    uic64cart_proc(wparam, hwnd);

    switch (wparam) {
      case IDM_VICII_SETTINGS:
        ui_vicii_settings_dialog(hwnd);
        break;
      case IDM_SID_SETTINGS:
        ui_sid_settings_dialog(hwnd);
        break;
      case IDM_REU_SETTINGS:
        ui_reu_settings_dialog(hwnd);
        break;
      case IDM_IDE64_SETTINGS:
        uiide64_settings_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
        uirom_settings_dialog(hwnd, IDD_C64ROM_SETTINGS_DIALOG,
                              uirom_settings);
        break;
#ifdef HAVE_TFE
      case IDM_TFE_SETTINGS:
        ui_tfe_settings_dialog(hwnd);
        break;
#endif
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VICII, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec64vic20_settings_dialog(hwnd);
        break;
    }
}

int c64ui_init(void)
{
    uic64cart_init();

    ui_register_machine_specific(c64_ui_specific);
    ui_register_menu_toggles(c64_ui_menu_toggles);

    return 0;
}

void c64ui_shutdown(void)
{
}

