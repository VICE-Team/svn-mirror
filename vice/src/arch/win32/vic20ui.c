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
#include <windowsx.h>

#include "ui.h"

#include "cartridge.h"
#include "res.h"
#include "uilib.h"
#include "uivideo.h"
#include "vic20ui.h"
#include "winmain.h"
#include "uivicset.h"

ui_menu_toggle  vic20_ui_menu_toggles[]={
    { "DoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "DoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "PALEmulation", IDM_TOGGLE_FASTPAL },
    { "VICVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "IEEE488", IDM_IEEE488 },
    { NULL, 0 }
};

static ui_cartridge_params vic20_ui_cartridges[]={
    {
        IDM_CART_VIC20_8KB_2000,
        CARTRIDGE_VIC20_16KB_2000,
        "Attach 4/8/16KB cartridge image at $2000",
        UI_LIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_16KB_4000,
        CARTRIDGE_VIC20_16KB_4000,
        "Attach 4/8/16KB cartridge image at $4000",
        UI_LIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_6000,
        CARTRIDGE_VIC20_16KB_6000,
        "Attach 4/8/16KB cartridge image at $6000",
        UI_LIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_8KB_A000,
        CARTRIDGE_VIC20_8KB_A000,
        "Attach 8KB cartridge image at $A000",
        UI_LIB_FILTER_ALL
    },
    {
        IDM_CART_VIC20_4KB_B000,
        CARTRIDGE_VIC20_4KB_B000,
        "Attach 4KB cartridge image at $B000",
        UI_LIB_FILTER_ALL
    },
    {
        0, 0, NULL, 0
    }
};

static void vic20_ui_attach_cartridge(WPARAM wparam, HWND hwnd,
                                      ui_cartridge_params *cartridges)
{
    int i;
    char *s;

    i = 0;
    while ((cartridges[i].wparam != wparam) && (cartridges[i].wparam != 0))
        i++;
    if (cartridges[i].wparam==0) {
        ui_error("Bad cartridge config in UI!");
        return;
    }
    if ((s = ui_select_file(hwnd,cartridges[i].title,
        cartridges[i].filter, FILE_SELECTOR_DEFAULT_STYLE, NULL)) != NULL) {
        if (cartridge_attach_image(cartridges[i].type, s) < 0)
            ui_error("Invalid cartridge image");
        free(s);
    }
}

/* Probably one should simply remove the size numbers from the IDM_* stuff */
static void vic20_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CART_VIC20_8KB_2000:
      case IDM_CART_VIC20_16KB_4000:
      case IDM_CART_VIC20_8KB_6000:
      case IDM_CART_VIC20_8KB_A000:
      case IDM_CART_VIC20_4KB_B000:
        vic20_ui_attach_cartridge(wparam, hwnd, vic20_ui_cartridges);
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
      case IDM_VIDEO_SETTINGS:
        ui_video_settings_dialog(hwnd, UI_VIDEO_PAL);
        break;
    }
}

int vic20_ui_init(void)
{
    ui_register_machine_specific(vic20_ui_specific);
    ui_register_menu_toggles(vic20_ui_menu_toggles);
    return 0;
}

