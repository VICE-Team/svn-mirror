/*
 * petui.c - PET-specific user interface.
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
#include "translate.h"
#include "ui.h"
#include "uiacia.h"
#include "uidrivepetcbm2.h"
#include "uikeyboard.h"
#include "uipetreu.h"
#include "uipetset.h"
#include "uirom.h"
#include "uivideo.h"
#include "winmain.h"


static const unsigned int romset_dialog_resources[UIROM_TYPE_MAX] = {
    IDD_PETROM_RESOURCE_DIALOG,
    IDD_PETROMDRIVE_RESOURCE_DIALOG,
    0 };


static const ui_menu_toggle_t pet_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_CRTCDOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_CRTCDOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_CRTCVIDEOCACHE },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { UIROM_TYPE_MAIN, TEXT("Kernal"), "KernalName",
      IDC_PETROM_KERNAL_FILE, IDC_PETROM_KERNAL_BROWSE,
      IDC_PETROM_KERNAL_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic"), "BasicName",
      IDC_PETROM_BASIC_FILE, IDC_PETROM_BASIC_BROWSE,
      IDC_PETROM_BASIC_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Character"), "ChargenName",
      IDC_PETROM_CHARGEN_FILE, IDC_PETROM_CHARGEN_BROWSE,
      IDC_PETROM_CHARGEN_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Editor"), "EditorName",
      IDC_PETROM_EDITOR_FILE, IDC_PETROM_EDITOR_BROWSE,
      IDC_PETROM_EDITOR_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("ROM9"), "RomModule9Name",
      IDC_PETROM_ROM9_FILE, IDC_PETROM_ROM9_BROWSE,
      IDC_PETROM_ROM9_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("ROMA"), "RomModuleAName",
      IDC_PETROM_ROMA_FILE, IDC_PETROM_ROMA_BROWSE,
      IDC_PETROM_ROMA_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("ROMB"), "RomModuleBName",
      IDC_PETROM_ROMB_FILE, IDC_PETROM_ROMB_BROWSE,
      IDC_PETROM_ROMB_RESOURCE },
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

#define PETUI_KBD_NUM_MAP 6

static const uikeyboard_mapping_entry_t mapping_entry[PETUI_KBD_NUM_MAP] = {
    { IDC_PETKBD_MAPPING_SELECT_UKSYM, IDC_PETKBD_MAPPING_UKSYM,
      IDC_PETKBD_MAPPING_UKSYM_BROWSE, "KeymapBusinessUKSymFile" },
    { IDC_PETKBD_MAPPING_SELECT_UKPOS, IDC_PETKBD_MAPPING_UKPOS,
      IDC_PETKBD_MAPPING_UKPOS_BROWSE, "KeymapBusinessUKPosFile" },
    { IDC_PETKBD_MAPPING_SELECT_GRSYM, IDC_PETKBD_MAPPING_GRSYM,
      IDC_PETKBD_MAPPING_GRSYM_BROWSE, "KeymapGraphicsSymFile" },
    { IDC_PETKBD_MAPPING_SELECT_GRPOS, IDC_PETKBD_MAPPING_GRPOS,
      IDC_PETKBD_MAPPING_GRPOS_BROWSE, "KeymapGraphicsPosFile" },
    { IDC_PETKBD_MAPPING_SELECT_DESYM, IDC_PETKBD_MAPPING_DESYM,
      IDC_PETKBD_MAPPING_DESYM_BROWSE, "KeymapBusinessDESymFile" },
    { IDC_PETKBD_MAPPING_SELECT_DEPOS, IDC_PETKBD_MAPPING_DEPOS,
      IDC_PETKBD_MAPPING_DEPOS_BROWSE, "KeymapBusinessDEPosFile" }
};

static uikeyboard_config_t uikeyboard_config =
    { IDD_PETKBD_MAPPING_SETTINGS_DIALOG, PETUI_KBD_NUM_MAP, mapping_entry,
      IDC_PETKBD_MAPPING_DUMP };


static void pet_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_PET_SETTINGS:
        ui_pet_settings_dialog(hwnd);
        break;
      case IDM_PETREU_SETTINGS:
        ui_petreu_settings_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
        uirom_settings_dialog(hwnd, translate_res(IDD_PETROM_SETTINGS_DIALOG),
                              translate_res(IDD_PETDRIVEROM_SETTINGS_DIALOG),
                              romset_dialog_resources, uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_CRTC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog(hwnd);
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog(hwnd, 0, NULL, 0, 0);
        break;
      case IDM_KEYBOARD_SETTINGS:
        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
        break;
    }
}

int petui_init(void)
{
    ui_register_machine_specific(pet_ui_specific);
    ui_register_menu_toggles(pet_ui_menu_toggles);
    return 0;
}

void petui_shutdown(void)
{
}

