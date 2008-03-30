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
#include "translate.h"
#include "ui.h"
#include "uicart.h"
#include "uidrivec64vic20.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "uirom.h"
#include "uirs232user.h"
#include "uivideo.h"
#include "vic20ui.h"
#include "winmain.h"
#include "uivicset.h"


static const unsigned int romset_dialog_resources[UIROM_TYPE_MAX] = {
    IDD_VIC20ROM_RESOURCE_DIALOG,
    IDD_VIC20ROMDRIVE_RESOURCE_DIALOG,
    0 };


static const ui_menu_toggle_t vic20_ui_menu_toggles[] = {
    { "EmuID", IDM_TOGGLE_EMUID },
    { "VICDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "VICVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "IEEE488", IDM_IEEE488 },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { UIROM_TYPE_MAIN, TEXT("Kernal"), "KernalName",
      IDC_VIC20ROM_KERNAL_FILE, IDC_VIC20ROM_KERNAL_BROWSE,
      IDC_VIC20ROM_KERNAL_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic"), "BasicName",
      IDC_VIC20ROM_BASIC_FILE, IDC_VIC20ROM_BASIC_BROWSE,
      IDC_VIC20ROM_BASIC_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Character"), "ChargenName",
      IDC_VIC20ROM_CHARGEN_FILE, IDC_VIC20ROM_CHARGEN_BROWSE,
      IDC_VIC20ROM_CHARGEN_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1541"), "DosName1541",
      IDC_DRIVEROM_1541_FILE, IDC_DRIVEROM_1541_BROWSE,
      IDC_DRIVEROM_1541_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1541-II"), "DosName1541ii",
      IDC_DRIVEROM_1541II_FILE, IDC_DRIVEROM_1541II_BROWSE,
      IDC_DRIVEROM_1541II_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1570"), "DosName1570",
      IDC_DRIVEROM_1570_FILE, IDC_DRIVEROM_1570_BROWSE,
      IDC_DRIVEROM_1570_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1571"), "DosName1571",
      IDC_DRIVEROM_1571_FILE, IDC_DRIVEROM_1571_BROWSE,
      IDC_DRIVEROM_1571_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1581"), "DosName1581",
      IDC_DRIVEROM_1581_FILE, IDC_DRIVEROM_1581_BROWSE,
      IDC_DRIVEROM_1581_RESOURCE },
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

#define VIC20UI_KBD_NUM_MAP 2

static const uikeyboard_mapping_entry_t mapping_entry[VIC20UI_KBD_NUM_MAP] = {
    { IDC_VIC20KBD_MAPPING_SELECT_SYM, IDC_VIC20KBD_MAPPING_SYM,
      IDC_VIC20KBD_MAPPING_SYM_BROWSE, "KeymapSymFile" },
    { IDC_VIC20KBD_MAPPING_SELECT_POS, IDC_VIC20KBD_MAPPING_POS,
      IDC_VIC20KBD_MAPPING_POS_BROWSE, "KeymapPosFile" }
};

static uikeyboard_config_t uikeyboard_config =
    { IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG, VIC20UI_KBD_NUM_MAP,
      mapping_entry, IDC_VIC20KBD_MAPPING_DUMP };

static const uicart_params_t vic20_ui_cartridges[] = {
    {
        IDM_CART_VIC20_8KB_2000,
        CARTRIDGE_VIC20_16KB_2000,
        IDS_ATTACH_4_8_16_CART_2000,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_16KB_4000,
        CARTRIDGE_VIC20_16KB_4000,
        IDS_ATTACH_4_8_16_CART_4000,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_6000,
        CARTRIDGE_VIC20_16KB_6000,
        IDS_ATTACH_4_8_16_CART_6000,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_A000,
        CARTRIDGE_VIC20_8KB_A000,
        IDS_ATTACH_4_8_CART_A000,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_4KB_B000,
        CARTRIDGE_VIC20_4KB_B000,
        IDS_ATTACH_4_CART_B000,
        UILIB_FILTER_ALL
    },
    {
        0, 0, 0, 0
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
                              translate_res(IDD_VIC20DRIVEROM_SETTINGS_DIALOG),
                              romset_dialog_resources, uirom_settings); 
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VIC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec64vic20_settings_dialog(hwnd);
        break;
      case IDM_RS232USER_SETTINGS:
        ui_rs232user_settings_dialog(hwnd);
        break;
      case IDM_KEYBOARD_SETTINGS:
        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
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

