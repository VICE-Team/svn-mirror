/*
 * vic20ui.cc - VIC20-specific user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <Message.h>
#include <stdio.h>

extern "C" {
#include "constants.h"
#include "ui.h"
#include "vic20ui.h"
}


ui_menu_toggle  vic20_ui_menu_toggles[]={
    { "DoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "DoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "IEEE488", MENU_TOGGLE_IEEE488 },
    { NULL, 0 }
};


int vic20_ui_init(void)
{
    ui_register_menu_toggles(vic20_ui_menu_toggles);
    ui_update_menus();
    return 0;
}

