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
#include "uiacia.h"
#include "uicbm2set.h"
#include "uidrivepetcbm2.h"
#include "uikeyboard.h"
#include "uirom.h"
#include "uisid.h"
#include "uivideo.h"
#include "winmain.h"


static const unsigned int romset_dialog_resources[UIROM_TYPE_MAX] = {
    IDD_CBM2ROM_RESOURCE_DIALOG,
    IDD_CBM2ROMDRIVE_RESOURCE_DIALOG,
    0 };


static const ui_menu_toggle_t cbm2_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_CRTCDOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_CRTCDOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_CRTCVIDEOCACHE },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { UIROM_TYPE_MAIN, TEXT("Kernal"), "KernalName",
      IDC_CBM2ROM_KERNAL_FILE, IDC_CBM2ROM_KERNAL_BROWSE,
      IDC_CBM2ROM_KERNAL_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic"), "BasicName",
      IDC_CBM2ROM_BASIC_FILE, IDC_CBM2ROM_BASIC_BROWSE,
      IDC_CBM2ROM_BASIC_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Character"), "ChargenName",
      IDC_CBM2ROM_CHARGEN_FILE, IDC_CBM2ROM_CHARGEN_BROWSE,
      IDC_CBM2ROM_CHARGEN_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Cart 1"), "Cart1Name",
      IDC_CBM2ROM_CART1_FILE, IDC_CBM2ROM_CART1_BROWSE,
      IDC_CBM2ROM_CART1_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Cart 2"), "Cart2Name",
      IDC_CBM2ROM_CART2_FILE, IDC_CBM2ROM_CART2_BROWSE,
      IDC_CBM2ROM_CART2_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Cart 4"), "Cart4Name",
      IDC_CBM2ROM_CART4_FILE, IDC_CBM2ROM_CART4_BROWSE,
      IDC_CBM2ROM_CART4_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Cart 6"), "Cart6Name",
      IDC_CBM2ROM_CART6_FILE, IDC_CBM2ROM_CART6_BROWSE,
      IDC_CBM2ROM_CART6_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("2031"), "DosName2031",
      IDC_DRIVEROM_2031_FILE, IDC_DRIVEROM_2031_BROWSE,
      IDC_DRIVEROM_2031_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("2040"), "DosName2040",
      IDC_DRIVEROM_2040_FILE, IDC_DRIVEROM_2040_BROWSE,
      IDC_DRIVEROM_2040_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("3040"), "DosName3040",
      IDC_DRIVEROM_3040_FILE, IDC_DRIVEROM_3040_BROWSE,
      IDC_DRIVEROM_3040_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("4040"), "DosName4040",
      IDC_DRIVEROM_4040_FILE, IDC_DRIVEROM_4040_BROWSE,
      IDC_DRIVEROM_4040_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1001"), "DosName1001",
      IDC_DRIVEROM_1001_FILE, IDC_DRIVEROM_1001_BROWSE,
      IDC_DRIVEROM_1001_RESOURCE },
    { 0, NULL, NULL, 0, 0, 0 }
};

static const ui_res_value_list_t cbm2_ui_res_values[] = {
    { NULL, NULL, 0 }
};

#define CBM2UI_KBD_NUM_MAP 6

static const uikeyboard_mapping_entry_t mapping_entry[CBM2UI_KBD_NUM_MAP] = {
    { IDC_CBM2KBD_MAPPING_SELECT_UKSYM, IDC_CBM2KBD_MAPPING_UKSYM,
      IDC_CBM2KBD_MAPPING_UKSYM_BROWSE, "KeymapBusinessUKSymFile" },
    { IDC_CBM2KBD_MAPPING_SELECT_UKPOS, IDC_CBM2KBD_MAPPING_UKPOS,
      IDC_CBM2KBD_MAPPING_UKPOS_BROWSE, "KeymapBusinessUKPosFile" },
    { IDC_CBM2KBD_MAPPING_SELECT_GRSYM, IDC_CBM2KBD_MAPPING_GRSYM,
      IDC_CBM2KBD_MAPPING_GRSYM_BROWSE, "KeymapGraphicsSymFile" },
    { IDC_CBM2KBD_MAPPING_SELECT_GRPOS, IDC_CBM2KBD_MAPPING_GRPOS,
      IDC_CBM2KBD_MAPPING_GRPOS_BROWSE, "KeymapGraphicsPosFile" },
    { IDC_CBM2KBD_MAPPING_SELECT_DESYM, IDC_CBM2KBD_MAPPING_DESYM,
      IDC_CBM2KBD_MAPPING_DESYM_BROWSE, "KeymapBusinessDESymFile" },
    { IDC_CBM2KBD_MAPPING_SELECT_DEPOS, IDC_CBM2KBD_MAPPING_DEPOS,
      IDC_CBM2KBD_MAPPING_DEPOS_BROWSE, "KeymapBusinessDEPosFile" }
};

static uikeyboard_config_t uikeyboard_config =
    { IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG, CBM2UI_KBD_NUM_MAP, mapping_entry };


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
        uirom_settings_dialog(hwnd, IDD_CBM2ROM_SETTINGS_DIALOG,
                              IDD_CBM2DRIVEROM_SETTINGS_DIALOG,
                              romset_dialog_resources, uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_CRTC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog(hwnd);
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog(hwnd, 0, NULL, 0);
        break;
      case IDM_KEYBOARD_SETTINGS:
        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
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

