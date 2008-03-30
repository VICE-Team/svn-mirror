/*
 * uiplus4cart.c - PLUS4-specific cartridge user interface.
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

#include "cartridge.h"
#include "plus4cart.h"
#include "keyboard.h"
#include "lib.h"
#include "res.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uicart.h"
#include "uiplus4cart.h"
#include "uilib.h"
#include "resources.h"


static const uicart_params_t plus4_ui_cartridges[] = {
    {
        IDM_CART_ATTACH_FUNCLO,
        0,
        IDS_ATTACH_FUNCTION_LOW_CART,
        UILIB_FILTER_ALL | UILIB_FILTER_CRT
    },
    {
        IDM_CART_ATTACH_FUNCHI,
        0,
        IDS_ATTACH_FUNCTION_HIGH_CART,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_C1LO,
        0,
        IDS_ATTACH_CART1_LOW,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_C1HI,
        0,
        IDS_ATTACH_CART1_HIGH,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_C2LO,
        0,
        IDS_ATTACH_CART2_LOW,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_C2HI,
        0,
        IDS_ATTACH_CART2_HIGH,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        0, 0, 0, 0
    }
};

static int uiplus4cart_attach_image(int type, char *s)
{
    switch (type) {
      case IDM_CART_ATTACH_FUNCLO:
        resources_set_value("FunctionLowName", s);
        return plus4cart_load_func_lo(s);
      case IDM_CART_ATTACH_FUNCHI:
        resources_set_value("FunctionHighName", s);
        return plus4cart_load_func_lo(s);
      case IDM_CART_ATTACH_C1LO:
        resources_set_value("c1loName", s);
        return plus4cart_load_c1lo(s);
      case IDM_CART_ATTACH_C1HI:
        resources_set_value("c1hiName", s);
        return plus4cart_load_c1hi(s);
      case IDM_CART_ATTACH_C2LO:
        resources_set_value("c2loName", s);
        return plus4cart_load_c2lo(s);
      case IDM_CART_ATTACH_C2HI:
        resources_set_value("c2hiName", s);
        return plus4cart_load_c2hi(s);
    }
    return -1;
}

static void uiplus4cart_attach(WPARAM wparam, HWND hwnd,
                             const uicart_params_t *cartridges)
{
    int i;
    TCHAR *st_name;

    i = 0;

    while ((cartridges[i].wparam != wparam) && (cartridges[i].wparam != 0))
        i++;

    if (cartridges[i].wparam == 0) {
        ui_error(translate_text(IDS_BAD_CARTRIDGE_CONFIG));
        return;
    }

    if ((st_name = uilib_select_file(hwnd, translate_text(cartridges[i].title),
        cartridges[i].filter, UILIB_SELECTOR_TYPE_FILE_LOAD,
        UILIB_SELECTOR_STYLE_CART)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);
        if (uiplus4cart_attach_image(cartridges[i].wparam, name) < 0)
            ui_error(translate_text(IDS_INVALID_CARTRIDGE_IMAGE));
        system_wcstombs_free(name);
        lib_free(st_name);
    }
}

void uiplus4cart_proc(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CART_ATTACH_FUNCLO:
      case IDM_CART_ATTACH_FUNCHI:
      case IDM_CART_ATTACH_C1LO:
      case IDM_CART_ATTACH_C1HI:
      case IDM_CART_ATTACH_C2LO:
      case IDM_CART_ATTACH_C2HI:
        uiplus4cart_attach(wparam, hwnd, plus4_ui_cartridges);
        break;
      case IDM_CART_DETACH:
        plus4cart_detach_cartridges();
        break;
    }
}

void uiplus4cart_init(void)
{
}

