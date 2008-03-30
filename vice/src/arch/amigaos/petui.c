/*
 * petui.c
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

#define UI_PET
#define UI_MENU_NAME pet_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME pet_ui_menu

#include "private.h"
#include "petui.h"
#include "petuires.h"

#include "mui/uiacia.h"
#include "mui/uidrivepetcbm2.h"
#include "mui/uipetreu.h"
#include "mui/uipetsettings.h"
#include "mui/uirompetsettings.h"
#include "mui/uisidcart.h"
#include "mui/uivideocbm2pet.h"

static const ui_menu_toggle_t pet_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "EmuID", IDM_TOGGLE_EMUID },   
    { NULL, 0 }
};

static int pet_ui_specific(video_canvas_t *canvas, int idm)
{
    switch (idm) {
      case IDM_PETREU_SETTINGS:
        ui_petreu_settings_dialog(canvas);
        break;
      case IDM_PET_SETTINGS:
        ui_pet_settings_dialog();
        break;
      case IDM_PET_MODEL:
        ui_pet_model_dialog();
        break;
      case IDM_COMPUTER_ROM_SETTINGS:
        ui_pet_computer_rom_settings_dialog(canvas);
        break;
      case IDM_DRIVE_ROM_SETTINGS:
        ui_pet_drive_rom_settings_dialog(canvas);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_cbm2pet_settings_dialog(canvas);
        break;
      case IDM_DRIVE_SETTINGS:
        uidrivepetcbm2_settings_dialog();
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog();
        break;
      case IDM_SIDCART_SETTINGS:
        ui_sidcart_settings_dialog("$8F00", "$E900", "PET");
        break;
      case IDM_KEYBOARD_SETTINGS:
//        uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
        break;
    }

    return 0;
}

int petui_init(void)
{
  ui_register_menu_translation_layout(pet_ui_translation_menu);
  ui_register_menu_layout(pet_ui_menu);
  ui_register_machine_specific(pet_ui_specific);
  ui_register_menu_toggles(pet_ui_menu_toggles);
//  ui_register_res_values(const ui_res_value_list_t *valuelist);

  return 0;
}

void petui_shutdown(void)
{
}

