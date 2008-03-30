/*
 * c64ui.cc - C64-specific user interface.
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
#include "vicewindow.h"

extern "C" {
#include "c64ui.h"
#include "cartridge.h"
#include "constants.h"
#include "interrupt.h"
#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "psid.h"
#include "resources.h"
#include "ui.h"
#include "ui_file.h"
#include "ui_vicii.h"
#include "vicemenu.h"
#include "vsync.h"
}

ui_menu_toggle  c64_ui_menu_toggles[]={
    { "VICIIDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "REU", MENU_TOGGLE_REU },
    { "Mouse", MENU_TOGGLE_MOUSE },
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

ui_res_value_list c64_ui_res_values[] = {
    {"REUsize", ReuSize},
    {NULL,NULL}
};


static ui_cartridge_t c64_ui_cartridges[]={
    {
    MENU_CART_ATTACH_CRT,
    CARTRIDGE_CRT,
    "CRT"
    },
    {
    MENU_CART_ATTACH_8KB,
    CARTRIDGE_GENERIC_8KB,
    "Raw 8KB"
    },
    {
    MENU_CART_ATTACH_16KB,
    CARTRIDGE_GENERIC_16KB,
    "Raw 16KB"
    },
    {
    MENU_CART_ATTACH_AR,
    CARTRIDGE_ACTION_REPLAY,
    "Action Replay",
    },
    {
    MENU_CART_ATTACH_AT,
    CARTRIDGE_ATOMIC_POWER,
    "Atomic Power"
    },
    {
    MENU_CART_ATTACH_EPYX,
    CARTRIDGE_EPYX_FASTLOAD,
    "Epyx Fastload"
    },
    {
    MENU_CART_ATTACH_IEEE488,
    CARTRIDGE_IEEE488,
    "IEEE488 interface"
    },
    {
    MENU_CART_ATTACH_IDE64,
    CARTRIDGE_IDE64,
    "IDE64 interface"
    },
    {
    MENU_CART_ATTACH_SS4,
    CARTRIDGE_SUPER_SNAPSHOT,
    "Super Snapshot 4",
    },
    {
    MENU_CART_ATTACH_SS5,
    CARTRIDGE_SUPER_SNAPSHOT_V5,
    "Super Snapshot 5",
    },
    {
    0,0,NULL
    }
};


static void c64_ui_attach_cartridge(void *msg, void *window)
{
	int menu = ((BMessage*)msg)->what;
	ViceFilePanel *filepanel = ((ViceWindow*)window)->filepanel;
	int i = 0;
	
	while (menu != c64_ui_cartridges[i].menu_item 
		&& c64_ui_cartridges[i].menu_item)
		i++;
	
	if (!c64_ui_cartridges[i].menu_item) {
		ui_error("Bad cartridge config in UI");
		return;
	}

	ui_select_file(filepanel,C64_CARTRIDGE_FILE, &c64_ui_cartridges[i]);
}	


static void c64_play_vsid(const char *filename) {
	
	int tunes, default_tune, i;
    		
	if (machine_autodetect_psid(filename) < 0) {
    	ui_error("`%s' is not a valid PSID file.", filename);
      	return;
    }
    psid_init_driver();
    machine_play_psid(0);
    maincpu_trigger_reset();

	vicemenu_free_tune_menu();
	/* Get number of tunes in current PSID. */
	tunes = psid_tunes(&default_tune);

	/* Build tune menu. */
	vicemenu_tune_menu_add(-default_tune);
	for (i = 0; i < tunes; i++) {
		vicemenu_tune_menu_add(i+1);
	}
}


void c64_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_CART_ATTACH_CRT:    	
        case MENU_CART_ATTACH_8KB:
        case MENU_CART_ATTACH_16KB:
        case MENU_CART_ATTACH_AR:
        case MENU_CART_ATTACH_AT:
        case MENU_CART_ATTACH_EPYX:
        case MENU_CART_ATTACH_IEEE488:
        case MENU_CART_ATTACH_IDE64:
        case MENU_CART_ATTACH_SS4:
        case MENU_CART_ATTACH_SS5:
            c64_ui_attach_cartridge(msg, window);
            break;
        case MENU_CART_SET_DEFAULT:
            cartridge_set_default();
            break;
        case MENU_CART_DETACH:
            cartridge_detach_image();
            break;
        case MENU_CART_FREEZE:
            keyboard_clear_keymatrix();
            cartridge_trigger_freeze();
            break;
		case ATTACH_C64_CART:
		{	
			const char *filename;
			int32 type;
			
			((BMessage*)msg)->FindInt32("type", &type);
			((BMessage*)msg)->FindString("filename", &filename);
			if (cartridge_attach_image(type, filename) < 0)
				ui_error("Invalid cartridge image");
			break;
		}
		case MENU_VICII_SETTINGS:
        	ui_vicii();
        	break;
        case MENU_VSID_LOAD:
        	ui_select_file(((ViceWindow*)window)->filepanel, VSID_FILE, 0);
			break;
        case MENU_VSID_TUNE:
		{
			int32 tune;
			
			((BMessage*)msg)->FindInt32("nr", &tune);
			machine_play_psid(tune);
			vsync_suspend_speed_eval();
			maincpu_trigger_reset();
			break;
        }
		case PLAY_VSID:
		{
			const char *filename;
			
			((BMessage*)msg)->FindString("filename", &filename);
			c64_play_vsid(filename);
			break;
		}

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
