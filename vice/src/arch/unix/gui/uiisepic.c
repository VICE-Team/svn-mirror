/*
 * uiisepic.c
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "uiisepic.h"


UI_MENU_DEFINE_TOGGLE(Isepic)
UI_MENU_DEFINE_TOGGLE(IsepicSwitch)

ui_menu_entry_t georam_submenu[] = {
    { N_("*Enable Isepic"),
      (ui_callback_t)toggle_Isepic, NULL, NULL },
    { N_("*Enable Isepic Switch"),
      (ui_callback_t)toggle_IsepicSwitch, NULL, NULL },
    { NULL }
};
