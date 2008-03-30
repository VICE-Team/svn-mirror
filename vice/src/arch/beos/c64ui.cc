/*
 * c64ui.c - C64-specific user interface.
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
#include "ui.h"
#include "c64ui.h"
#include "cartridge.h"
#include "constants.h"
#include "kbd.h"
#include "keyboard.h"
}

ui_menu_toggle  c64_ui_menu_toggles[]={
    { "DoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "DoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "REU", MENU_TOGGLE_REU },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { "SidFilters", MENU_TOGGLE_SIDFILTERS },
#ifdef HAVE_RESID
    { "SidUseResid", MENU_TOGGLE_SOUND_RESID },
#endif
    { NULL, 0 }
};

static ui_res_possible_values SidType[] = {
    {0, MENU_SIDTYPE_6581},
    {1, MENU_SIDTYPE_8580},
    {-1,0}
};

ui_res_value_list c64_ui_res_values[] = {
    {"SidModel", SidType},
    {NULL,NULL}
};


void c64_ui_specific(void *msg)
{
    switch (((BMessage*)msg)->what) {
    	default: ;
    }
}

int c64_ui_init(void)
{
    ui_register_machine_specific(c64_ui_specific);
    ui_register_menu_toggles(c64_ui_menu_toggles);
    ui_register_res_values(c64_ui_res_values);
    ui_update_menus();
    return 0;
}

