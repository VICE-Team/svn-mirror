/*
 * uimouse.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "mouse.h"
#include "uimenu.h"
#include "uimouse.h"

UI_MENU_DEFINE_RADIO(Mouseport)
UI_MENU_DEFINE_RADIO(Mousetype)

static ui_menu_entry_t mousetype_submenu[] = {
    { "1351", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_1351, NULL },
    { "NEOS", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_NEOS, NULL },
    { "Amiga", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_AMIGA, NULL },
    { N_("Paddles"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_PADDLE, NULL },
    { "Atari CX-22", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_CX22, NULL },
    { "Atari ST", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_ST, NULL },
    { "Smart", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_SMART, NULL },
    { "MicroMys", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_MICROMYS, NULL },
    { "Koalapad", UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mousetype,
      (ui_callback_data_t)MOUSE_TYPE_KOALAPAD, NULL },
    { NULL }
};

static ui_menu_entry_t mouseport_submenu[] = {
    { N_("Port 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mouseport,
      (ui_callback_data_t)1, NULL },
    { N_("Port 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_Mouseport,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Mouse)
UI_MENU_DEFINE_TOGGLE(SmartMouseRTCSave)

ui_menu_entry_t mouse_submenu[] = {
    { N_("Enable mouse grab"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Mouse, NULL, NULL, KEYSYM_m, UI_HOTMOD_META },
    { N_("Mouse type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mousetype_submenu },
    { N_("Mouse port"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, mouseport_submenu },
    { N_("Enable SmartMouse RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SmartMouseRTCSave, NULL },
    { NULL }
};

ui_menu_entry_t mouse_vic20_submenu[] = {
    { N_("Enable paddles"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Mouse, NULL, NULL, KEYSYM_m, UI_HOTMOD_META },
    { NULL }
};

#endif
