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

#include "vice.h"

#define UI_PLUS4
#define UI_MENU_NAME plus4_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME plus4_ui_menu

#include "private.h"
#include "plus4cart.h"
#include "plus4ui.h"
#include "plus4uires.h"
#include "uicart.h"
#include "lib.h"
#include "intl.h"
#include "translate.h"

#include "mui/uiacia.h"
#include "mui/uidriveplus4.h"
#include "mui/uiplus4settings.h"
#include "mui/uiromplus4settings.h"
#include "mui/uisidcart.h"
#include "mui/uivideoc64plus4vic20.h"

static const ui_menu_toggle_t plus4_ui_menu_toggles[] = {
    { "TEDDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "TEDDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "TEDVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "TEDScale2x", IDM_TOGGLE_SCALE2X },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { NULL, 0 }
};

static const uicart_params_t plus4_ui_cartridges[] = {
    {
        IDM_CART_ATTACH_FUNCLO,
        0,
        IDS_ATTACH_FUNCTION_LOW_CART,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_ATTACH_FUNCHI,
        0,
        IDS_ATTACH_FUNCTION_HIGH_CART,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_ATTACH_C1LO,
        0,
        IDS_ATTACH_CART1_LOW,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_ATTACH_C1HI,
        0,
        IDS_ATTACH_CART1_HIGH,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_ATTACH_C2LO,
        0,
        IDS_ATTACH_CART2_LOW,
        UILIB_FILTER_ALL
    },
    {
        IDM_CART_ATTACH_C2HI,
        0,
        IDS_ATTACH_CART2_HIGH,
        UILIB_FILTER_ALL
    },
    {
        0, 0, 0, 0
    }
};

static int uiplus4cart_attach_image(int type, char *s)
{
    switch (type) {
      case IDM_CART_ATTACH_FUNCLO:
        resources_set_string("FunctionLowName", s);
        return plus4cart_load_func_lo(s);
      case IDM_CART_ATTACH_FUNCHI:
        resources_set_string("FunctionHighName", s);
        return plus4cart_load_func_lo(s);
      case IDM_CART_ATTACH_C1LO:
        resources_set_string("c1loName", s);
        return plus4cart_load_c1lo(s);
      case IDM_CART_ATTACH_C1HI:
        resources_set_string("c1hiName", s);
        return plus4cart_load_c1hi(s);
      case IDM_CART_ATTACH_C2LO:
        resources_set_string("c2loName", s);
        return plus4cart_load_c2lo(s);
      case IDM_CART_ATTACH_C2HI:
        resources_set_string("c2hiName", s);
        return plus4cart_load_c2hi(s);
    }
    return -1;
}

static void uiplus4cart_attach(video_canvas_t *canvas, int idm,
                                const uicart_params_t *cartridges)
{
    int i;
    char *name;

    i = 0;

    while ((cartridges[i].idm != idm) && (cartridges[i].idm != 0))
        i++;

    if (cartridges[i].idm == 0) {
        ui_error(translate_text(IDMES_BAD_CART_CONFIG_IN_UI));
        return;
    }

    if ((name = uilib_select_file(translate_text(cartridges[i].title),
        cartridges[i].filter, UILIB_SELECTOR_TYPE_FILE_LOAD,
        UILIB_SELECTOR_STYLE_CART)) != NULL) {

        if (uiplus4cart_attach_image(cartridges[i].idm, name) < 0)
            ui_error(translate_text(IDMES_INVALID_CART_IMAGE));
        lib_free(name);
    }
}

static int plus4_ui_specific(video_canvas_t *canvas, int idm)
{
    switch (idm) {
      case IDM_CART_ATTACH_FUNCLO:
      case IDM_CART_ATTACH_FUNCHI:
      case IDM_CART_ATTACH_C1LO:
      case IDM_CART_ATTACH_C1HI:
      case IDM_CART_ATTACH_C2LO:
      case IDM_CART_ATTACH_C2HI:
        uiplus4cart_attach(canvas, idm, plus4_ui_cartridges);
        break;
      case IDM_CART_DETACH:
        plus4cart_detach_cartridges();
      case IDM_PLUS4_SETTINGS:
        ui_plus4_settings_dialog();
        break;
      case IDM_COMPUTER_ROM_SETTINGS:
        ui_plus4_computer_rom_settings_dialog(canvas);
        break;
      case IDM_DRIVE_ROM_SETTINGS:
        ui_plus4_drive_rom_settings_dialog(canvas);
        break;
      case IDM_VIDEO_SETTINGS:
        ui_video_c64plus4vic20_settings_dialog(canvas, "TEDExternalPalette", "TEDPaletteFile");
        break;
      case IDM_DRIVE_SETTINGS:
        uidriveplus4_settings_dialog();
        break;
      case IDM_ACIA_SETTINGS:
        ui_acia_settings_dialog();
        break;
      case IDM_SIDCART_SETTINGS:
        ui_sidcart_settings_dialog("$FD40", "$FE80", "PLUS4");
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
