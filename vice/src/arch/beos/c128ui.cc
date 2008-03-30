/*
 * c128ui.cc - C128-specific user interface.
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
#include "c128ui.h"
#include "constants.h" 
#include "resources.h"
#include "ui.h"
#include "ui_vicii.h"
}

ui_menu_toggle  c128_ui_menu_toggles[]={
    { "VICIIDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "REU", MENU_TOGGLE_REU },
    { "IEEE488", MENU_TOGGLE_IEEE488 },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { "VDCDoubleSize", MENU_TOGGLE_VDC_DOUBLESIZE },
    { "VDCDoubleScan", MENU_TOGGLE_VDC_DOUBLESCAN },
    { "VDC64KB", MENU_TOGGLE_VDC64KB },
    { "PALEmulation", MENU_TOGGLE_FASTPAL },
    { "VICIIScale2x", MENU_TOGGLE_SCALE2X },
    { NULL, 0 }
};

ui_res_possible_values ReuSize[] = {
        {128, MENU_REU_SIZE_128},
        {256, MENU_REU_SIZE_256},
        {512, MENU_REU_SIZE_512},
        {1024, MENU_REU_SIZE_1024},
        {2048, MENU_REU_SIZE_2048},
        {4096, MENU_REU_SIZE_4096},
        {8192, MENU_REU_SIZE_8192},
        {16384, MENU_REU_SIZE_16384},
        {-1, 0}
};


ui_res_value_list c128_ui_res_values[] = {
    {"REUsize", ReuSize},
    {NULL,NULL}
};


void c128_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
		case MENU_VICII_SETTINGS:
        	ui_vicii();
        	break;

    	default: ;
    }
}

int c128ui_init(void)
{
    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);
    ui_register_res_values(c128_ui_res_values);
    ui_update_menus();
    return 0;
}

void c128ui_shutdown(void)
{
}

