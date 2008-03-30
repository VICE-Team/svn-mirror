/*
 * c128ui.c - C128-specific user interface.
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

#include "c128ui.h"
#include "res.h"
#include "ui.h"
#include "uic128.h"
#include "uic64cart.h"
#include "uidrivec128.h"
#include "uiide64.h"
#include "uireu.h"
#include "uirom.h"
#include "uisid.h"
#include "uitfe.h"
#include "uivicii.h"
#include "uivideo.h"
#include "uilib.h"


static const unsigned int romset_dialog_resources[UIROM_TYPE_MAX] = {
    IDD_C128ROM_RESOURCE_DIALOG,
    IDD_C128ROMDRIVE_RESOURCE_DIALOG,
    0 };


static const ui_menu_toggle_t c128_ui_menu_toggles[] = {
    { "VICIIDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "VICIIScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "IEEE488", IDM_IEEE488 },
    { "Mouse", IDM_MOUSE },
    { "Mouse", IDM_MOUSE | 0x00010000 },
    { "CartridgeReset", IDM_TOGGLE_CART_RESET },
    { "VDCDoubleSize", IDM_TOGGLE_VDC_DOUBLESIZE },
    { "VDCDoubleScan", IDM_TOGGLE_VDC_DOUBLESCAN },
    { "VDC64KB", IDM_TOGGLE_VDC64KB },
    { "InternalFunctionROM", IDM_TOGGLE_IFUNCTIONROM },
    { "ExternalFunctionROM", IDM_TOGGLE_EFUNCTIONROM },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { UIROM_TYPE_MAIN, TEXT("International Kernal"), "KernalIntName",
      IDC_C128ROM_KERNALINT_FILE, IDC_C128ROM_KERNALINT_BROWSE,
      IDC_C128ROM_KERNALINT_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("German Kernal"), "KernalDEName",
      IDC_C128ROM_KERNALDE_FILE, IDC_C128ROM_KERNALDE_BROWSE,
      IDC_C128ROM_KERNALDE_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Finnish Kernal"), "KernalFRName",
      IDC_C128ROM_KERNALFI_FILE, IDC_C128ROM_KERNALFI_BROWSE,
      IDC_C128ROM_KERNALFI_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("French Kernal"), "KernalFIName",
      IDC_C128ROM_KERNALFR_FILE, IDC_C128ROM_KERNALFR_BROWSE,
      IDC_C128ROM_KERNALFR_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Italian Kernal"), "KernalITName",
      IDC_C128ROM_KERNALIT_FILE, IDC_C128ROM_KERNALIT_BROWSE,
      IDC_C128ROM_KERNALIT_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Norwegain Kernal"), "KernalNOName",
      IDC_C128ROM_KERNALNO_FILE, IDC_C128ROM_KERNALNO_BROWSE,
      IDC_C128ROM_KERNALNO_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Swedish Kernal"), "KernalSEName",
      IDC_C128ROM_KERNALSE_FILE, IDC_C128ROM_KERNALSE_BROWSE,
      IDC_C128ROM_KERNALSE_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic LO"), "BasicLoName",
      IDC_C128ROM_BASICLO_FILE, IDC_C128ROM_BASICLO_BROWSE,
      IDC_C128ROM_BASICLO_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic HI"), "BasicHiName",
      IDC_C128ROM_BASICHI_FILE, IDC_C128ROM_BASICHI_BROWSE,
      IDC_C128ROM_BASICHI_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("International Character"), "ChargenIntName",
      IDC_C128ROM_CHARGENINT_FILE, IDC_C128ROM_CHARGENINT_BROWSE,
      IDC_C128ROM_CHARGENINT_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("German Character"), "ChargenDEName",
      IDC_C128ROM_CHARGENDE_FILE, IDC_C128ROM_CHARGENDE_BROWSE,
      IDC_C128ROM_CHARGENDE_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("French Character"), "ChargenFRName",
      IDC_C128ROM_CHARGENFR_FILE, IDC_C128ROM_CHARGENFR_BROWSE,
      IDC_C128ROM_CHARGENFR_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Swedish Character"), "ChargenSEName",
      IDC_C128ROM_CHARGENSE_FILE, IDC_C128ROM_CHARGENSE_BROWSE,
      IDC_C128ROM_CHARGENSE_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("C64 mode Kernal"), "Kernal64Name",
      IDC_C128ROM_KERNAL64_FILE, IDC_C128ROM_KERNAL64_BROWSE,
      IDC_C128ROM_KERNAL64_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("C64 mode Basic"), "Basic64Name",
      IDC_C128ROM_BASIC64_FILE, IDC_C128ROM_BASIC64_BROWSE,
      IDC_C128ROM_BASIC64_RESOURCE },
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
    { UIROM_TYPE_DRIVE, TEXT("1571CR"), "DosName1571cr",
      IDC_DRIVEROM_1571CR_FILE, IDC_DRIVEROM_1571CR_BROWSE,
      IDC_DRIVEROM_1571CR_RESOURCE },
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


static void c128_ui_specific(WPARAM wparam, HWND hwnd)
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
        uirom_settings_dialog(hwnd, IDD_C128ROM_SETTINGS_DIALOG,
                              IDD_C128DRIVEROM_SETTINGS_DIALOG,
                              romset_dialog_resources, uirom_settings);
        break;
#ifdef HAVE_TFE
      case IDM_TFE_SETTINGS:
        ui_tfe_settings_dialog(hwnd);
        break;
#endif
      case IDM_C128_SETTINGS:
        ui_c128_dialog(hwnd);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VICII, UI_VIDEO_CHIP_VDC);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec128_settings_dialog(hwnd);
        break;
     }
}

int c128ui_init(void)
{
    uic64cart_init();

    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);

    return 0;
}

void c128ui_shutdown(void)
{
}

