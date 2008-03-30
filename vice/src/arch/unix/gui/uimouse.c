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

#include "uimenu.h"
#include "uimouse.h"


UI_MENU_DEFINE_RADIO(Mouseport)

static ui_menu_entry_t mouseport_submenu[] = {
    { "*Port 1", (ui_callback_t)radio_Mouseport,
      (ui_callback_data_t)1, NULL },
    { "*Port 2", (ui_callback_t)radio_Mouseport,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Mouse)

ui_menu_entry_t mouse_submenu[] = {
    { N_("*Enable mouse"),
      (ui_callback_t)toggle_Mouse, NULL, NULL, XK_m, UI_HOTMOD_META },
    { N_("Mouse port"),
      NULL, NULL, mouseport_submenu },
    { NULL }
};

#endif

