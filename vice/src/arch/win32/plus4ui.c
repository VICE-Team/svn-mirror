/*
 * plus4ui.c - PLUS4-specific user interface.
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
#include <windows.h>

#include "plus4ui.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uiacia.h"
#include "uiplus4cart.h"
#include "uidriveplus4.h"
#include "uilib.h"
#include "uiplus4mem.h"
#include "uirom.h"
#include "uivideo.h"
#include "winmain.h"


static const unsigned int romset_dialog_resources[UIROM_TYPE_MAX] = {
    IDD_PLUS4ROM_RESOURCE_DIALOG,
    IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG,
    0 };


static const ui_menu_toggle_t plus4_ui_menu_toggles[] = {
    { "TEDDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "TEDDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "TEDVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "TEDScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { NULL, 0 }
};

static const uirom_settings_t uirom_settings[] = {
    { UIROM_TYPE_MAIN, TEXT("Kernal"), "KernalName",
      IDC_PLUS4ROM_KERNAL_FILE, IDC_PLUS4ROM_KERNAL_BROWSE,
      IDC_PLUS4ROM_KERNAL_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("Basic"), "BasicName",
      IDC_PLUS4ROM_BASIC_FILE, IDC_PLUS4ROM_BASIC_BROWSE,
      IDC_PLUS4ROM_BASIC_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("3 plus 1 LO"), "3plus1loName",
      IDC_PLUS4ROM_3P1LO_FILE, IDC_PLUS4ROM_3P1LO_BROWSE,
      IDC_PLUS4ROM_3P1LO_RESOURCE },
    { UIROM_TYPE_MAIN, TEXT("3 plus 1 HI"), "3plus1hiName",
      IDC_PLUS4ROM_3P1HI_FILE, IDC_PLUS4ROM_3P1HI_BROWSE,
      IDC_PLUS4ROM_3P1HI_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1541"), "DosName1541",
      IDC_DRIVEROM_1541_FILE, IDC_DRIVEROM_1541_BROWSE,
      IDC_DRIVEROM_1541_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1541-II"), "DosName1541ii",
      IDC_DRIVEROM_1541II_FILE, IDC_DRIVEROM_1541II_BROWSE,
      IDC_DRIVEROM_1541II_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1551"), "DosName1551",
      IDC_DRIVEROM_1551_FILE, IDC_DRIVEROM_1551_BROWSE,
      IDC_DRIVEROM_1551_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1570"), "DosName1570",
      IDC_DRIVEROM_1570_FILE, IDC_DRIVEROM_1570_BROWSE,
      IDC_DRIVEROM_1570_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1571"), "DosName1571",
      IDC_DRIVEROM_1571_FILE, IDC_DRIVEROM_1571_BROWSE,
      IDC_DRIVEROM_1571_RESOURCE },
    { UIROM_TYPE_DRIVE, TEXT("1581"), "DosName1581",
      IDC_DRIVEROM_1581_FILE, IDC_DRIVEROM_1581_BROWSE,
      IDC_DRIVEROM_1581_RESOURCE },
    { 0, NULL, NULL, 0, 0, 0 }
};

static const ui_res_value_list_t plus4_ui_res_values[] = {
    { NULL, NULL, 0 }
};

static void plus4_ui_specific(WPARAM wparam, HWND hwnd)
{
    uiplus4cart_proc(wparam, hwnd);

    switch (wparam) {
      case IDM_PLUS4_SETTINGS:
        ui_plus4_memory_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
        uirom_settings_dialog(hwnd, IDD_PLUS4ROM_SETTINGS_DIALOG,
                              IDD_PLUS4DRIVEROM_SETTINGS_DIALOG,
                              romset_dialog_resources, uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_TED, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidriveplus4_settings_dialog(hwnd);
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog(hwnd, 0, NULL, 0);
        break;
    }
}

int plus4ui_init(void)
{
    uiplus4cart_init();

    ui_register_machine_specific(plus4_ui_specific);
    ui_register_menu_toggles(plus4_ui_menu_toggles);
    ui_register_res_values(plus4_ui_res_values);
    return 0;
}

void plus4ui_shutdown(void)
{
}

