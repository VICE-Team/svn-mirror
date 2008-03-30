/*
 * vic20ui.c - VIC20-specific user interface.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include "cartridge.h"
#include "lib.h"
#include "res.h"
#include "ui.h"
#include "uicart.h"
#include "uidrivec64vic20.h"
#include "uilib.h"
#include "uirom.h"
#include "uivideo.h"
#include "vic20ui.h"
#include "winmain.h"
#include "uivicset.h"


static const ui_menu_toggle_t vic20_ui_menu_toggles[] = {
    { "VICDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "VICVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "IEEE488", IDM_IEEE488 },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { TEXT("Kernal"), "KernalName",
      IDC_VIC20ROM_KERNAL_FILE, IDC_VIC20ROM_KERNAL_BROWSE },
    { TEXT("Basic"), "BasicName",
      IDC_VIC20ROM_BASIC_FILE, IDC_VIC20ROM_BASIC_BROWSE },
    { TEXT("Character"), "ChargenName",
      IDC_VIC20ROM_CHARGEN_FILE, IDC_VIC20ROM_CHARGEN_BROWSE },
    { NULL, NULL, 0, 0 }
};

static const uicart_params_t vic20_ui_cartridges[] = {
    {
        IDM_CART_VIC20_8KB_2000,
        CARTRIDGE_VIC20_16KB_2000,
        TEXT("Attach 4/8/16KB cartridge image at $2000"),
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_16KB_4000,
        CARTRIDGE_VIC20_16KB_4000,
        TEXT("Attach 4/8/16KB cartridge image at $4000"),
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_6000,
        CARTRIDGE_VIC20_16KB_6000,
        TEXT("Attach 4/8/16KB cartridge image at $6000"),
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_A000,
        CARTRIDGE_VIC20_8KB_A000,
        TEXT("Attach 8KB cartridge image at $A000"),
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_4KB_B000,
        CARTRIDGE_VIC20_4KB_B000,
        TEXT("Attach 4KB cartridge image at $B000"),
        UILIB_FILTER_ALL
    },
    {
        0, 0, NULL, 0
    }
};

/* Probably one should simply remove the size numbers from the IDM_* stuff */
static void vic20_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CART_VIC20_8KB_2000:
      case IDM_CART_VIC20_16KB_4000:
      case IDM_CART_VIC20_8KB_6000:
      case IDM_CART_VIC20_8KB_A000:
      case IDM_CART_VIC20_4KB_B000:
        uicart_attach(wparam, hwnd, vic20_ui_cartridges);
        break;
      case IDM_CART_SET_DEFAULT:
        cartridge_set_default();
        break;
      case IDM_CART_DETACH:
        cartridge_detach_image();
        break;
      case IDM_VIC_SETTINGS:
        ui_vic_settings_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
        uirom_settings_dialog(hwnd, IDD_VIC20ROM_SETTINGS_DIALOG,
                              uirom_settings); 
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VIC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec64vic20_settings_dialog(hwnd);
        break;
    }
}

int vic20ui_init(void)
{
    ui_register_machine_specific(vic20_ui_specific);
    ui_register_menu_toggles(vic20_ui_menu_toggles);
    return 0;
}

void vic20ui_shutdown(void)
{
}

