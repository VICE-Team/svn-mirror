/*
 * uic64cart.c
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

#include "private.h"
#include "vice.h"
#include "cartridge.h"
#include "keyboard.h"
#include "ui.h"
#include "uic64cart.h"
#include "uicart.h"
#include "uilib.h"
#include "uires.h"

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
        "Attach CRT cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_CRT
    },
    {
        IDM_CART_ATTACH_8KB,
        CARTRIDGE_GENERIC_8KB,
        "Attach raw 8KB cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_16KB,
        CARTRIDGE_GENERIC_16KB,
        "Attach raw 16KB cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR,
        CARTRIDGE_ACTION_REPLAY,
        "Attach Action Replay cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AT,
        CARTRIDGE_ATOMIC_POWER,
        "Attach Atomic Power cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_EPYX,
        CARTRIDGE_EPYX_FASTLOAD,
        "Attach Epyx fastload cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IEEE488,
        CARTRIDGE_IEEE488,
        "Attach IEEE interface cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_RR,
        CARTRIDGE_RETRO_REPLAY,
        "Attach Retro Replay cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IDE64,
        CARTRIDGE_IDE64,
        "Attach IDE64 interface cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS4,
        CARTRIDGE_SUPER_SNAPSHOT,
        "Attach Super Snapshot 4 cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS5,
        CARTRIDGE_SUPER_SNAPSHOT_V5,
        "Attach Super Snapshot 5 cartridge image",
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        0, 0, NULL, 0
    }
};

static void uic64cart_attach(video_canvas_t *canvas, int idm,
                             const uicart_params_t *cartridges)
{
    if (idm == IDM_CART_ENABLE_EXPERT) {
        if (cartridge_attach_image(CARTRIDGE_EXPERT, NULL) < 0)
            ui_error("Invalid cartridge");
        return;
    }

    uicart_attach(canvas, idm, cartridges);
}

void uic64cart_proc(video_canvas_t *canvas, int idm)
{
    switch (idm) {
      case IDM_CART_ATTACH_CRT:
      case IDM_CART_ATTACH_8KB:
      case IDM_CART_ATTACH_16KB:
      case IDM_CART_ATTACH_AR:
      case IDM_CART_ATTACH_AT:
      case IDM_CART_ATTACH_EPYX:
      case IDM_CART_ATTACH_IEEE488:
      case IDM_CART_ATTACH_RR:
      case IDM_CART_ATTACH_IDE64:
      case IDM_CART_ATTACH_SS4:
      case IDM_CART_ATTACH_SS5:
      case IDM_CART_ENABLE_EXPERT:
        uic64cart_attach(canvas, idm, c64_ui_cartridges);
        break;
      case IDM_CART_SET_DEFAULT:
        cartridge_set_default();
        break;
      case IDM_CART_DETACH:
        cartridge_detach_image();
        break;
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

