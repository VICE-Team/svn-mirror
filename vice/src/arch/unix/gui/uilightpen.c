/*
 * uilightpen.c
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
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

#ifdef HAVE_MOUSE

#include <stdio.h>

#include "uimenu.h"
#include "uilightpen.h"
#include "lightpen.h"

UI_MENU_DEFINE_RADIO(Lightpentype)

static ui_menu_entry_t lightpentype_submenu[] = {
    { N_("Pen with button Up"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_PEN_U, NULL },
    { N_("Pen with button Left"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_PEN_L, NULL },
    { N_("Datel Pen"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_PEN_DATEL, NULL },
    { N_("Magnum Light Phaser"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_GUN_Y, NULL },
    { N_("Stack Light Rifle"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_GUN_L, NULL },
    { N_("Inkwell Pen"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Lightpentype,
      (ui_callback_data_t) LIGHTPEN_TYPE_INKWELL, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Lightpen)

static UI_CALLBACK(intercept_toggle_Lightpen)
{
#ifdef USE_GNOMEUI
    toggle_Lightpen(w, event_data);
#else
    toggle_Lightpen(w, client_data, call_data);
    if (!CHECK_MENUS) {
        ui_check_mouse_cursor();
    }
#endif
}

ui_menu_entry_t lightpen_submenu[] = {
    { N_("Enable lightpen emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)intercept_toggle_Lightpen, NULL, NULL, KEYSYM_g, UI_HOTMOD_META },
    { N_("Lightpen type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, lightpentype_submenu },
    { NULL }
};

#endif /* HAVE_MOUSE */
