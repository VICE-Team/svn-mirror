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

#include <Alert.h>
#include <Application.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__BEOS__) && defined(WORDS_BIGENDIAN)
#include <string.h>
#endif

extern "C" {
#include "archdep.h"
#include "cartridge.h"
#include "constants.h"
#include "resources.h"
#include "statusbar.h"
#include "types.h"
#include "ui.h"
#include "ui_file.h"
#include "ui_vic20.h"
#include "util.h"
#include "vic20ui.h"
#include "viceapp.h"
#include "vicewindow.h"
}

ui_menu_toggle  vic20_ui_menu_toggles[]={
    { "VICDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "PALEmulation", MENU_TOGGLE_FASTPAL },
    { "VICScale2x", MENU_TOGGLE_SCALE2X },
    { "IEEE488", MENU_TOGGLE_IEEE488 },
    { "SidCart", MENU_TOGGLE_SIDCART },
    { "SidFilters", MENU_TOGGLE_SIDCART_FILTERS },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { NULL, 0 }
};

static ui_cartridge_t vic20_ui_cartridges[]={
    {
    MENU_CART_VIC20_16KB_2000,
    CARTRIDGE_VIC20_16KB_2000,
    "4/8/16KB at $2000"
    },
    {
    MENU_CART_VIC20_16KB_4000,
    CARTRIDGE_VIC20_16KB_4000,
    "4/8/16KB at $4000"
    },
    {
    MENU_CART_VIC20_16KB_6000,
    CARTRIDGE_VIC20_16KB_6000,
    "4/8/16KB at $6000"
    },
    {
    MENU_CART_VIC20_8KB_A000,
    CARTRIDGE_VIC20_8KB_A000,
    "8KB at $A000"
    },
    {
    MENU_CART_VIC20_4KB_B000,
    CARTRIDGE_VIC20_4KB_B000,
    "4KB at $B000"
    },
    {
    0,0,NULL
    }
};


void vic20_ui_attach_cartridge(void *msg, void *window)
{
	int menu = ((BMessage*)msg)->what;
	ViceFilePanel *filepanel = ((ViceWindow*)window)->filepanel;
	int i = 0;
	
	while (menu != vic20_ui_cartridges[i].menu_item 
		&& vic20_ui_cartridges[i].menu_item)
		i++;
	
	if (!vic20_ui_cartridges[i].menu_item) {
		ui_error("Bad cartridge config in UI");
		return;
	}

	ui_select_file(filepanel,VIC20_CARTRIDGE_FILE, &vic20_ui_cartridges[i]);
}	



void vic20_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_CART_VIC20_16KB_2000:
        case MENU_CART_VIC20_16KB_4000:
        case MENU_CART_VIC20_16KB_6000:
        case MENU_CART_VIC20_8KB_A000:
        case MENU_CART_VIC20_4KB_B000:
            vic20_ui_attach_cartridge(msg, window);
            break;
        case MENU_CART_SET_DEFAULT:
            cartridge_set_default();
            break;
        case MENU_CART_DETACH:
            cartridge_detach_image();
            break;
		case ATTACH_VIC20_CART:
		{	
			const char *filename;
			int32 type;
			
			((BMessage*)msg)->FindInt32("type", &type);
			((BMessage*)msg)->FindString("filename", &filename);
			if (cartridge_attach_image(type, filename) < 0)
				ui_error("Invalid cartridge image");
			break;
		}
		case MENU_VIC20_SETTINGS:
        	ui_vic20();
        break;

    	default: ;
    }
}


ui_res_possible_values vic20_SIDCARTModel[] = {
        {0, MENU_SIDCART_MODEL_6581},
        {1, MENU_SIDCART_MODEL_8580},
        {-1, 0}
};

ui_res_possible_values vic20_SIDCARTAddress[] = {
        {0, MENU_SIDCART_ADDRESS_1},
        {1, MENU_SIDCART_ADDRESS_2},
        {-1, 0}
};

ui_res_possible_values vic20_SIDCARTClock[] = {
        {0, MENU_SIDCART_CLOCK_C64},
        {1, MENU_SIDCART_CLOCK_NATIVE},
        {-1, 0}
};

ui_res_value_list vic20_ui_res_values[] = {
    {"SidModel", vic20_SIDCARTModel},
    {"SidAddress", vic20_SIDCARTAddress},
    {"SidClock", vic20_SIDCARTClock},
    { NULL, NULL }
};

int vic20ui_init(void)
{
    ui_register_machine_specific(vic20_ui_specific);
    ui_register_menu_toggles(vic20_ui_menu_toggles);
    ui_register_res_values(vic20_ui_res_values);
    ui_update_menus();
    return 0;
}

void vic20ui_shutdown(void)
{
}

int vic20_cartridge_attach_image(int type, const char *filename)
{
	return cartridge_attach_image(type, filename);
}
