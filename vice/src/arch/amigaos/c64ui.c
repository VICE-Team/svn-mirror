/*
 * c64ui.c
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

#define UI_C64
#define UI_MENU_NAME c64_ui_menu

#include "private.h"
#include "c64ui.h"
#include "c64uires.h"
#include "uic64cart.h"

#include "mui/uidrivec64vic20.h"
#include "mui/uisid.h"

static const ui_menu_toggle_t c64_ui_menu_toggles[] = {
    { "VICIIDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VICIIScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "VICIIVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "Mouse", IDM_MOUSE },
    { "CartridgeReset", IDM_TOGGLE_CART_RESET },
    { NULL, 0 }
};

static int c64_ui_specific(video_canvas_t *canvas, int idm)
{
    uic64cart_proc(canvas, idm);

    switch (idm) {
      case IDM_VICII_SETTINGS:
//        ui_vicii_settings_dialog(hwnd);
        break;
      case IDM_SID_SETTINGS:
        ui_sid_settings_dialog();
        break;
      case IDM_REU_SETTINGS:
//        ui_reu_settings_dialog(hwnd);
        break;
      case IDM_IDE64_SETTINGS:
//        uiide64_settings_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
//        uirom_settings_dialog(hwnd, IDD_C64ROM_SETTINGS_DIALOG,
//                              IDD_C64DRIVEROM_SETTINGS_DIALOG,
//                              romset_dialog_resources, uirom_settings);
        break;
#ifdef HAVE_TFE
      case IDM_TFE_SETTINGS:
//        ui_tfe_settings_dialog(hwnd);
        break;
#endif
      case IDM_VIDEO_SETTINGS:
//        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_VICII, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivec64vic20_settings_dialog();
        break;
      case IDM_ACIA_SETTINGS:
//        ui_acia_settings_dialog(hwnd, 1, NULL, 1);
        break;
      case IDM_RS232USER_SETTINGS:
//        ui_rs232user_settings_dialog(hwnd);
        break;
      case IDM_KEYBOARD_SETTINGS:
//        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
        break;
    }

    return 0;
}

int c64ui_init(void)
{
  uic64cart_init();

  ui_register_menu_layout(c64_ui_menu);
  ui_register_machine_specific(c64_ui_specific);
  ui_register_menu_toggles(c64_ui_menu_toggles);
//  ui_register_res_values(const ui_res_value_list_t *valuelist);

  return 0;
}

void c64ui_shutdown(void)
{
}

