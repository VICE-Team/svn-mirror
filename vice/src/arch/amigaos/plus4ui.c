/*
 * plus4ui.c
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

#define UI_PLUS4
#define UI_MENU_NAME plus4_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME plus4_ui_menu

#include "private.h"
#include "plus4ui.h"
#include "plus4uires.h"

#include "mui/uiacia.h"
#include "mui/uidriveplus4.h"

static const ui_menu_toggle_t plus4_ui_menu_toggles[] = {
    { "TEDDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "TEDDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "TEDVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "TEDScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { NULL, 0 }
};

static int plus4_ui_specific(video_canvas_t *canvas, int idm)
{
//    uiplus4cart_proc(canvas, idm)

    switch (idm) {
      case IDM_PLUS4_SETTINGS:
//        ui_plus4_memory_dialog(hwnd);
        break;
      case IDM_ROM_SETTINGS:
//        uirom_settings_dialog(hwnd, IDD_PLUS4ROM_SETTINGS_DIALOG,
//                              IDD_PLUS4DRIVEROM_SETTINGS_DIALOG,
//                              romset_dialog_resources, uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
//        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_TED, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidriveplus4_settings_dialog();
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog();
        break;
      case IDM_KEYBOARD_SETTINGS:
//        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
        break;
    }

    return 0;
}

int plus4ui_init(void)
{
  ui_register_menu_translation_layout(plus4_ui_translation_menu);
  ui_register_menu_layout(plus4_ui_menu);
  ui_register_machine_specific(plus4_ui_specific);
  ui_register_menu_toggles(plus4_ui_menu_toggles);
//  ui_register_res_values(const ui_res_value_list_t *valuelist);

  return 0;
}

void plus4ui_shutdown(void)
{
}

