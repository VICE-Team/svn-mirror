/*
 * uiuserportrtc58321a.c
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "uilib.h"
#include "uimenu.h"
#include "uiuserportrtc58321a.h"

UI_MENU_DEFINE_TOGGLE(UserportRTC58321a)
UI_MENU_DEFINE_TOGGLE(UserportRTC58321aSave)

ui_menu_entry_t userportrtc58321a_submenu[] = {
    { N_("Enable"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportRTC58321a, NULL, NULL },
    { N_("Enable RTC (58321a) saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportRTC58321aSave, NULL, NULL },
    { NULL }
};
