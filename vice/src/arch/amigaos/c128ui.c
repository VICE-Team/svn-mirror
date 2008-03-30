/*
 * c128ui.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#define UI_C128
#define UI_MENU_NAME c128_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME c128_ui_menu

#include "private.h"
#include "c128ui.h"
#include "c128uires.h"
#include "uic64cart.h"

#include "mui/uiacia.h"
#include "mui/uidrivec128.h"
#include "mui/uigeoram.h"
#include "mui/uiramcart.h"
#include "mui/uireu.h"
#include "mui/uirs232user.h"
#include "mui/uisid.h"
#include "mui/uivicii.h"

static const ui_menu_toggle_t c128_ui_menu_toggles[] = {
    { "VICIIDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "VICIIScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "IEEE488", IDM_IEEE488 },
    { "Mouse", IDM_MOUSE },
    { "CartridgeReset", IDM_TOGGLE_CART_RESET },
    { "VDCDoubleSize", IDM_TOGGLE_VDC_DOUBLESIZE },
    { "VDCDoubleScan", IDM_TOGGLE_VDC_DOUBLESCAN },
    { "VDC64KB", IDM_TOGGLE_VDC64KB },
    { "EmuID", IDM_TOGGLE_EMUID },   
//    { "InternalFunctionROM", IDM_TOGGLE_IFUNCTIONROM },
//    { "ExternalFunctionROM", IDM_TOGGLE_EFUNCTIONROM },
    { NULL, 0 }
};

static int c128_ui_specific(video_canvas_t *canvas, int idm)
{
    uic64cart_proc(canvas, idm);

    switch (idm) {
      case IDM_VICII_SETTINGS:
        ui_vicii_settings_dialog();
        break;
      case IDM_SID_SETTINGS:
        ui_sid_settings_dialog();
        break;
      case IDM_REU_SETTINGS:
        ui_reu_settings_dialog();
        break;
      case IDM_GEORAM_SETTINGS:
        ui_georam_settings_dialog();
        break;
      case IDM_RAMCART_SETTINGS:
        ui_ramcart_settings_dialog();
        break;
      case IDM_IDE64_SETTINGS:
//        uiide64_settings_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
//        uirom_settings_dialog(hwnd, IDD_C128ROM_SETTINGS_DIALOG,
//                              IDD_C128DRIVEROM_SETTINGS_DIALOG,
//                              romset_dialog_resources, uirom_settings);
        break;
#ifdef HAVE_TFE
      case IDM_TFE_SETTINGS:
//        ui_tfe_settings_dialog(hwnd);
        break;
#endif
      case IDM_C128_SETTINGS:
//        ui_c128_dialog(hwnd);
        break;
      case IDM_VIDEO_SETTINGS:
//        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VICII, UI_VIDEO_CHIP_VDC);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec128_settings_dialog();
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia64128_settings_dialog();
        break;
      case IDM_RS232USER_SETTINGS:
        ui_rs232user_settings_dialog();
        break;
      case IDM_KEYBOARD_SETTINGS:
//        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
        break;
    }

    return 0;
}

int c128ui_init(void)
{
  ui_register_menu_translation_layout(c128_ui_translation_menu);
  ui_register_menu_layout(c128_ui_menu);
  ui_register_machine_specific(c128_ui_specific);
  ui_register_menu_toggles(c128_ui_menu_toggles);
//  ui_register_res_values(const ui_res_value_list_t *valuelist);

  return 0;
}

void c128ui_shutdown(void)
{
}

