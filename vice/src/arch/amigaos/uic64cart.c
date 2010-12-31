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
#include "c64cart.h"
#include "intl.h"
#include "keyboard.h"
#include "translate.h"
#include "ui.h"
#include "uic64cart.h"
#include "uicart.h"
#include "uilib.h"
#include "uires.h"

static const uicart_params_t c64_ui_cartridges[] = {
    {
        IDM_CART_ATTACH_CRT,
        CARTRIDGE_CRT,
        IDS_ATTACH_CRT_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_CRT
    },
    {
        IDM_CART_ATTACH_8KB,
        CARTRIDGE_GENERIC_8KB,
        IDS_ATTACH_RAW_8KB_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_16KB,
        CARTRIDGE_GENERIC_16KB,
        IDS_ATTACH_RAW_16KB_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR,
        CARTRIDGE_ACTION_REPLAY,
        IDS_ATTACH_ACTION_REPLAY_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR3,
        CARTRIDGE_ACTION_REPLAY3,
        IDS_ATTACH_ACTION_REPLAY3_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AR4,
        CARTRIDGE_ACTION_REPLAY4,
        IDS_ATTACH_ACTION_REPLAY4_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_STARDOS,
        CARTRIDGE_STARDOS,
        IDS_ATTACH_STARDOS_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_AT,
        CARTRIDGE_ATOMIC_POWER,
        IDS_ATTACH_ATOMIC_POWER_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_EPYX,
        CARTRIDGE_EPYX_FASTLOAD,
        IDS_ATTACH_EPYX_FASTLOAD_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IEEE488,
        CARTRIDGE_IEEE488,
        IDS_ATTACH_IEEE488_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_RR,
        CARTRIDGE_RETRO_REPLAY,
        IDS_ATTACH_RETRO_REPLAY_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_IDE64,
        CARTRIDGE_IDE64,
        IDS_ATTACH_IDE64_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS4,
        CARTRIDGE_SUPER_SNAPSHOT,
        IDS_ATTACH_SS4_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_SS5,
        CARTRIDGE_SUPER_SNAPSHOT_V5,
        IDS_ATTACH_SS5_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        IDM_CART_ATTACH_STB,
        CARTRIDGE_STRUCTURED_BASIC,
        IDS_ATTACH_SB_IMAGE,
        UILIB_FILTER_ALL | UILIB_FILTER_BIN
    },
    {
        0, 0, 0, 0
    }
};

static void uic64cart_attach(video_canvas_t *canvas, int idm, const uicart_params_t *cartridges)
{
    uicart_attach(canvas, idm, cartridges);
}

void uic64cart_proc(video_canvas_t *canvas, int idm)
{
    switch (idm) {
        case IDM_CART_ATTACH_CRT:
        case IDM_CART_ATTACH_8KB:
        case IDM_CART_ATTACH_16KB:
        case IDM_CART_ATTACH_AR:
        case IDM_CART_ATTACH_AR3:
        case IDM_CART_ATTACH_AR4:
        case IDM_CART_ATTACH_STARDOS:
        case IDM_CART_ATTACH_AT:
        case IDM_CART_ATTACH_EPYX:
        case IDM_CART_ATTACH_IEEE488:
        case IDM_CART_ATTACH_RR:
        case IDM_CART_ATTACH_IDE64:
        case IDM_CART_ATTACH_SS4:
        case IDM_CART_ATTACH_SS5:
        case IDM_CART_ATTACH_STB:
            uic64cart_attach(canvas, idm, c64_ui_cartridges);
            break;
        case IDM_CART_SET_DEFAULT:
            cartridge_set_default();
            break;
        case IDM_CART_DETACH:
            cartridge_detach_image(-1);
            break;
        case IDM_CART_FREEZE:
            keyboard_clear_keymatrix();
            cartridge_trigger_freeze();
            break;
    }
}

void uic64cart_init(void)
{
}
