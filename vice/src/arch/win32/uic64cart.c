/*
 * uic64cart.c - C64-specific cartridge user interface.
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
#include "keyboard.h"
#include "lib.h"
#include "res.h"
#include "translate.h"
#include "ui.h"
#include "uic64cart.h"
#include "uicart.h"
#include "uilib.h"


static const ui_res_possible_values_t CartMode[] = {
    { CARTRIDGE_MODE_OFF, IDM_CART_MODE_OFF },
    { CARTRIDGE_MODE_PRG, IDM_CART_MODE_PRG },
    { CARTRIDGE_MODE_ON, IDM_CART_MODE_ON },
    { -1, 0 }
};

static const ui_res_value_list_t c64_ui_res_values[] = {
    { "CartridgeMode", CartMode, 0 },
    { NULL, NULL, 0 }
};

static const uicart_params_t c64_ui_cartridges[] = {
    {
        IDM_CART_ATTACH_CRT,
        CARTRIDGE_CRT,
        IDS_ATTACH_CRT_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_CRT
    },
    {
        IDM_CART_ATTACH_8KB,
        CARTRIDGE_GENERIC_8KB,
        IDS_ATTACH_RAW_8KB_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_16KB,
        CARTRIDGE_GENERIC_16KB,
        IDS_ATTACH_RAW_16KB_CART_IMG,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR,
        CARTRIDGE_ACTION_REPLAY,
        IDS_ATTACH_AR_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR3,
        CARTRIDGE_ACTION_REPLAY3,
        IDS_ATTACH_AR3_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AT,
        CARTRIDGE_ATOMIC_POWER,
        IDS_ATTACH_ATOMIC_P_CART_IMG,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_EPYX,
        CARTRIDGE_EPYX_FASTLOAD,
        IDS_ATTACH_EPYX_FL_CART_IMG,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IEEE488,
        CARTRIDGE_IEEE488,
        IDS_ATTACH_IEEE488_CART_IMG,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_RR,
        CARTRIDGE_RETRO_REPLAY,
        IDS_ATTACH_RETRO_R_CART_IMG,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IDE64,
        CARTRIDGE_IDE64,
        IDS_ATTACH_IDE64_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS4,
        CARTRIDGE_SUPER_SNAPSHOT,
        IDS_ATTACH_SS4_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS5,
        CARTRIDGE_SUPER_SNAPSHOT_V5,
        IDS_ATTACH_SS5_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_STB,
        CARTRIDGE_STRUCTURED_BASIC,
        IDS_ATTACH_STB_CART_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        0, 0, 0, 0
    }
};

static void uic64cart_attach(WPARAM wparam, HWND hwnd,
                             const uicart_params_t *cartridges)
{
    if (wparam == IDM_CART_ENABLE_EXPERT) {
        if (cartridge_attach_image(CARTRIDGE_EXPERT, NULL) < 0)
            ui_error(translate_text(IDS_INVALID_CARTRIDGE));
        return;
    }

    uicart_attach(wparam, hwnd, cartridges);
}

void uic64cart_proc(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CART_ATTACH_CRT:
      case IDM_CART_ATTACH_8KB:
      case IDM_CART_ATTACH_16KB:
      case IDM_CART_ATTACH_AR:
      case IDM_CART_ATTACH_AR3:
      case IDM_CART_ATTACH_AT:
      case IDM_CART_ATTACH_EPYX:
      case IDM_CART_ATTACH_IEEE488:
      case IDM_CART_ATTACH_RR:
      case IDM_CART_ATTACH_IDE64:
      case IDM_CART_ATTACH_SS4:
      case IDM_CART_ATTACH_SS5:
      case IDM_CART_ATTACH_STB:
      case IDM_CART_ENABLE_EXPERT:
        uic64cart_attach(wparam, hwnd, c64_ui_cartridges);
        break;
      case IDM_CART_SET_DEFAULT:
        cartridge_set_default();
        break;
      case IDM_CART_DETACH:
        cartridge_detach_image();
        break;
      case IDM_CART_FREEZE|0x00010000:
      case IDM_CART_FREEZE:
        keyboard_clear_keymatrix();
        cartridge_trigger_freeze();
        break;
    }
}

void uic64cart_init(void)
{
    ui_register_res_values(c64_ui_res_values);
}

