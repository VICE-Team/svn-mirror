/*
 * cbm2ui.c
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

#define UI_CBM2
#define UI_MENU_NAME cbm2_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME cbm2_ui_menu

#include "private.h"
#include "cbm2ui.h"
#include "cbm2uires.h"

#include "mui/uiacia.h"
#include "mui/uicbm2settings.h"
#include "mui/uidrivepetcbm2.h"
#include "mui/uisid.h"

static const ui_menu_toggle_t cbm2_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "EmuID", IDM_TOGGLE_EMUID },   
    { NULL, 0 }
};

static int cbm2_ui_specific(video_canvas_t *canvas, int idm)
{
    switch (idm) {
      case IDM_CBM2_SETTINGS:
        ui_cbm2_settings_dialog();
        break;
      case IDM_SID_SETTINGS:
        ui_sid_settings_dialog();
        break;
      case IDM_ROM_SETTINGS:
//        uirom_settings_dialog(hwnd, IDD_CBM2ROM_SETTINGS_DIALOG,
//                              IDD_CBM2DRIVEROM_SETTINGS_DIALOG,
//                              romset_dialog_resources, uirom_settings);
        break;
      case IDM_VIDEO_SETTINGS:
//        ui_video_settings_dialog(hwnd, UI_VIDEO_CHIP_CRTC, UI_VIDEO_CHIP_NONE);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog();
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

int cbm2ui_init(void)
{
  ui_register_menu_translation_layout(cbm2_ui_translation_menu);
  ui_register_menu_layout(cbm2_ui_menu);
  ui_register_machine_specific(cbm2_ui_specific);
  ui_register_menu_toggles(cbm2_ui_menu_toggles);
//  ui_register_res_values(const ui_res_value_list_t *valuelist);

  return 0;
}

void cbm2ui_shutdown(void)
{
}

