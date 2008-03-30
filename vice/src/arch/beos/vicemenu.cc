/*
 * vicemenu.cc - Implementation of the BeVICE's menubar
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

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <stdio.h>

#include "constants.h"
#include "machine.h"
#include "vicemenu.h"

static BMenu *vsid_tune_menu;

void vicemenu_free_tune_menu(void) {
	BMenuItem *mi;
	
	while(mi = vsid_tune_menu->FindItem(MENU_VSID_TUNE)) {
		vsid_tune_menu->RemoveItem(mi);
		delete mi;
	}
}

void vicemenu_tune_menu_add(int tune) {

	char menustr[32];
	char shortcut;
	BMessage *msg;
	
	if (tune > 0) { 
		sprintf(menustr, "Tune %d", tune);
		shortcut = '0' + tune;
	} else {
		/*the default tune indicator */
		shortcut = '0';
		sprintf(menustr, "Default (%d)", -tune);
		tune = -tune;
	}
	
	msg = new BMessage(MENU_VSID_TUNE);
	msg->AddInt32("nr", tune);
	
	if (tune < 10)
		vsid_tune_menu->AddItem(new BMenuItem(menustr, msg, shortcut));
	else
		vsid_tune_menu->AddItem(new BMenuItem(menustr, msg));
		
}

BMenuBar *menu_create(int machine_class) {

	BMenuBar *menubar;
	BMenu *menu, *submenu;
	BMenuItem *item;
	
	menubar = new BMenuBar (BRect(0,0,10,10),"Menubar");
	
	/* create the FILE menu */
	menu = new BMenu("File");
	menubar->AddItem(menu);
	if (!vsid_mode) {
		menu->AddItem(new BMenuItem("Autostart",
			new BMessage(MENU_AUTOSTART), 'A'));
		menu->AddSeparatorItem();

		menu->AddItem(submenu = new BMenu("Attach Disk"));
		submenu->AddItem(new BMenuItem("Drive 8", 
			new BMessage(MENU_ATTACH_DISK8), '8'));
		submenu->AddItem(new BMenuItem("Drive 9", 
			new BMessage(MENU_ATTACH_DISK9), '9'));
		submenu->AddItem(new BMenuItem("Drive 10", 
			new BMessage(MENU_ATTACH_DISK10)));
		submenu->AddItem(new BMenuItem("Drive 11", 
			new BMessage(MENU_ATTACH_DISK11)));

		menu->AddItem(submenu = new BMenu("Detach Disk"));
		submenu->AddItem(new BMenuItem("Drive 8", 
			new BMessage(MENU_DETACH_DISK8)));
		submenu->AddItem(new BMenuItem("Drive 9", 
			new BMessage(MENU_DETACH_DISK9)));
		submenu->AddItem(new BMenuItem("Drive 10", 
			new BMessage(MENU_DETACH_DISK10)));
		submenu->AddItem(new BMenuItem("Drive 11", 
			new BMessage(MENU_DETACH_DISK11)));

		menu->AddItem(submenu = new BMenu("Flip List"));
		submenu->AddItem(new BMenuItem("Add current image", 
			new BMessage(MENU_FLIP_ADD), 'I'));
		submenu->AddItem(new BMenuItem("Remove Current Image", 
			new BMessage(MENU_FLIP_REMOVE), 'K'));
		submenu->AddItem(new BMenuItem("Attach next image", 
			new BMessage(MENU_FLIP_NEXT), 'N'));
		submenu->AddItem(new BMenuItem("Attach previous image", 
			new BMessage(MENU_FLIP_PREVIOUS), 'N', B_CONTROL_KEY));
		menu->AddSeparatorItem();

		menu->AddItem(new BMenuItem("Attach Tape", 
			new BMessage(MENU_ATTACH_TAPE), 'T'));
		menu->AddItem(new BMenuItem("Detach Tape", 
			new BMessage(MENU_DETACH_TAPE)));

		menu->AddItem(submenu = new BMenu("Datasette Control"));
		submenu->AddItem(new BMenuItem("Start", 
			new BMessage(MENU_DATASETTE_START)));
		submenu->AddItem(new BMenuItem("Stop", 
			new BMessage(MENU_DATASETTE_STOP)));
		submenu->AddItem(new BMenuItem("Forward", 
			new BMessage(MENU_DATASETTE_FORWARD)));
		submenu->AddItem(new BMenuItem("Rewind", 
			new BMessage(MENU_DATASETTE_REWIND)));
		submenu->AddItem(new BMenuItem("Record", 
			new BMessage(MENU_DATASETTE_RECORD)));
		submenu->AddItem(new BMenuItem("Reset", 
			new BMessage(MENU_DATASETTE_RESET)));
		submenu->AddItem(new BMenuItem("Reset Counter", 
			new BMessage(MENU_DATASETTE_COUNTER)));
		menu->AddSeparatorItem();
		if (machine_class == VICE_MACHINE_C64) {
			menu->AddItem(submenu = new BMenu("Attach cartridge image"));
			submenu->AddItem(new BMenuItem("CRT",
				new BMessage(MENU_CART_ATTACH_CRT)));
			submenu->AddItem(new BMenuItem("Generic 8KB",
				new BMessage(MENU_CART_ATTACH_8KB)));
			submenu->AddItem(new BMenuItem("Generic 16KB",
				new BMessage(MENU_CART_ATTACH_16KB)));
			submenu->AddItem(new BMenuItem("Action Replay",
				new BMessage(MENU_CART_ATTACH_AR)));
			submenu->AddItem(new BMenuItem("Atomic Power",
				new BMessage(MENU_CART_ATTACH_AT)));
			submenu->AddItem(new BMenuItem("Epyx fastload",
				new BMessage(MENU_CART_ATTACH_EPYX)));
			submenu->AddItem(new BMenuItem("IEEE488 interface",
				new BMessage(MENU_CART_ATTACH_IEEE488)));
			submenu->AddItem(new BMenuItem("IDE64 interface",
				new BMessage(MENU_CART_ATTACH_IDE64)));
			submenu->AddItem(new BMenuItem("Super Snapshot 4",
				new BMessage(MENU_CART_ATTACH_SS4)));
			submenu->AddItem(new BMenuItem("Super Snapshot 5",
				new BMessage(MENU_CART_ATTACH_SS5)));
			submenu->AddSeparatorItem();
			submenu->AddItem(new BMenuItem("Set cartridge as default",
				new BMessage(MENU_CART_SET_DEFAULT)));
			menu->AddItem(new BMenuItem("Detach cartridge image", 
				new BMessage(MENU_CART_DETACH)));
			menu->AddItem(new BMenuItem("Cartridge freeze", 
				new BMessage(MENU_CART_FREEZE), 'Z'));
			menu->AddSeparatorItem();
		}
		if (machine_class == VICE_MACHINE_VIC20) {
			menu->AddItem(submenu = new BMenu("Attach cartridge image"));
			submenu->AddItem(new BMenuItem("4/8/16KB image at $2000",
				new BMessage(MENU_CART_VIC20_16KB_2000)));
			submenu->AddItem(new BMenuItem("4/8/16KB image at $4000",
				new BMessage(MENU_CART_VIC20_16KB_4000)));
			submenu->AddItem(new BMenuItem("4/8/16KB image at $6000",
				new BMessage(MENU_CART_VIC20_16KB_6000)));
			submenu->AddItem(new BMenuItem("4/8KB image at $A000",
				new BMessage(MENU_CART_VIC20_8KB_A000)));
			submenu->AddItem(new BMenuItem("4KB image at $B000",
				new BMessage(MENU_CART_VIC20_4KB_B000)));
			menu->AddItem(new BMenuItem("Detach cartridge image", 
				new BMessage(MENU_CART_DETACH)));
			menu->AddSeparatorItem();
		}
			
		menu->AddItem(submenu = new BMenu("Snapshot"));
		submenu->AddItem(new BMenuItem("Load snapshot", 
			new BMessage(MENU_SNAPSHOT_LOAD)));
		submenu->AddItem(new BMenuItem("Save snapshot", 
			new BMessage(MENU_SNAPSHOT_SAVE)));
		submenu->AddItem(new BMenuItem("Load quicksnapshot", 
			new BMessage(MENU_LOADQUICK), 'L', B_CONTROL_KEY));
		submenu->AddItem(new BMenuItem("Save quicksnapshot", 
			new BMessage(MENU_SAVEQUICK), 'S', B_CONTROL_KEY));
	} else {
		/* vsid */
		menu->AddItem(new BMenuItem("Load psid file", 
			new BMessage(MENU_VSID_LOAD)));
		menu->AddItem(vsid_tune_menu = new BMenu("Tune")); 
	}
	
	menu->AddSeparatorItem();
	menu->AddItem(item = new BMenuItem("Pause", 
		new BMessage(MENU_PAUSE), 'P'));
	menu->AddItem(item = new BMenuItem("Monitor", 
		new BMessage(MENU_MONITOR), 'M'));
	menu->AddItem(item = new BMenuItem("Soft Reset", 
		new BMessage(MENU_RESET_SOFT), 'R'));
	menu->AddItem(new BMenuItem("Hard Reset", 
		new BMessage(MENU_RESET_HARD), 'R', B_CONTROL_KEY));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Quit", 
		new BMessage(MENU_EXIT_REQUESTED), 'Q'));

	/* create the OPTIONS menu */
	menu = new BMenu("Options");
	menubar->AddItem(menu);

	if (!vsid_mode) {
		/* refresh rate */
		menu->AddItem(submenu = new BMenu("Refresh Rate"));
		submenu->SetRadioMode(true);
		submenu->AddItem(new BMenuItem("Auto", 
			new BMessage(MENU_REFRESH_RATE_AUTO)));
		submenu->AddItem(new BMenuItem("1/1", 
			new BMessage(MENU_REFRESH_RATE_1)));
		submenu->AddItem(new BMenuItem("1/2", 
			new BMessage(MENU_REFRESH_RATE_2)));
		submenu->AddItem(new BMenuItem("1/3", 
			new BMessage(MENU_REFRESH_RATE_3)));
		submenu->AddItem(new BMenuItem("1/4", 
			new BMessage(MENU_REFRESH_RATE_4)));
		submenu->AddItem(new BMenuItem("1/5", 
			new BMessage(MENU_REFRESH_RATE_5)));
		submenu->AddItem(new BMenuItem("1/6", 
			new BMessage(MENU_REFRESH_RATE_6)));
		submenu->AddItem(new BMenuItem("1/7", 
			new BMessage(MENU_REFRESH_RATE_7)));
		submenu->AddItem(new BMenuItem("1/8", 
			new BMessage(MENU_REFRESH_RATE_8)));
		submenu->AddItem(new BMenuItem("1/9", 
			new BMessage(MENU_REFRESH_RATE_9)));
		submenu->AddItem(new BMenuItem("1/10", 
			new BMessage(MENU_REFRESH_RATE_10)));

		/* maximum speed */
		menu->AddItem(submenu = new BMenu("Maximum Speed"));
		submenu->SetRadioMode(true);
		submenu->AddItem(new BMenuItem("No Limit", 
			new BMessage(MENU_MAXIMUM_SPEED_NO_LIMIT)));
		submenu->AddItem(new BMenuItem("200%", 
			new BMessage(MENU_MAXIMUM_SPEED_200)));
		submenu->AddItem(new BMenuItem("100%", 
			new BMessage(MENU_MAXIMUM_SPEED_100)));
		submenu->AddItem(new BMenuItem("50%", 
			new BMessage(MENU_MAXIMUM_SPEED_50)));
		submenu->AddItem(new BMenuItem("20%", 
			new BMessage(MENU_MAXIMUM_SPEED_20)));
		submenu->AddItem(new BMenuItem("10%", 
			new BMessage(MENU_MAXIMUM_SPEED_10)));

		menu->AddItem(new BMenuItem("Warp Mode", 
			new BMessage(MENU_TOGGLE_WARP_MODE),'W'));
		menu->AddSeparatorItem();
		menu->AddItem(new BMenuItem("DirectWindow", 
			new BMessage(MENU_TOGGLE_DIRECTWINDOW)));
		menu->AddItem(new BMenuItem("Video Cache", 
			new BMessage(MENU_TOGGLE_VIDEOCACHE)));
		menu->AddItem(new BMenuItem("Double Size", 
			new BMessage(MENU_TOGGLE_DOUBLESIZE),'D'));
		menu->AddItem(new BMenuItem("Double Scan", 
			new BMessage(MENU_TOGGLE_DOUBLESCAN)));
		if (machine_class == VICE_MACHINE_C64
			|| machine_class == VICE_MACHINE_C128
			|| machine_class == VICE_MACHINE_VIC20) {
			menu->AddItem(new BMenuItem("PAL emulation", 
				new BMessage(MENU_TOGGLE_FASTPAL), 'P', B_CONTROL_KEY));
		}
		if (machine_class == VICE_MACHINE_C64
			|| machine_class == VICE_MACHINE_C128
			|| machine_class == VICE_MACHINE_PLUS4
			|| machine_class == VICE_MACHINE_VIC20) {
			menu->AddItem(new BMenuItem("Scale2x", 
				new BMessage(MENU_TOGGLE_SCALE2X), 'S'));
		}
		if (machine_class == VICE_MACHINE_C128) {
			menu->AddItem(submenu = new BMenu("VDC"));
			submenu->AddItem(new BMenuItem("Double Size",
				new BMessage(MENU_TOGGLE_VDC_DOUBLESIZE)));
			submenu->AddItem(new BMenuItem("Double Scan",
				new BMessage(MENU_TOGGLE_VDC_DOUBLESCAN)));
			submenu->AddSeparatorItem();
			submenu->AddItem(new BMenuItem("64KB video memory",
				new BMessage(MENU_TOGGLE_VDC64KB)));
		}
		menu->AddSeparatorItem();
	}
			
	/* sound options */
	menu->AddItem(new BMenuItem("Sound", 
		new BMessage(MENU_TOGGLE_SOUND)));
	menu->AddSeparatorItem();
	
	if (!vsid_mode) {
		menu->AddItem(new BMenuItem("True Drive Emulation", 
			new BMessage(MENU_TOGGLE_DRIVE_TRUE_EMULATION)));
		menu->AddItem(new BMenuItem("Virtual Devices", 
			new BMessage(MENU_TOGGLE_VIRTUAL_DEVICES)));
		menu->AddSeparatorItem();
	}
	if (machine_class == VICE_MACHINE_C64
		|| machine_class == VICE_MACHINE_C128
		|| machine_class == VICE_MACHINE_VIC20) {
		menu->AddItem(submenu = new BMenu("Video Standard"));
		submenu->SetRadioMode(true);
		submenu->AddItem(new BMenuItem("PAL-G", 
			new BMessage(MENU_SYNC_FACTOR_PAL)));
		submenu->AddItem(new BMenuItem("NTSC-M", 
			new BMessage(MENU_SYNC_FACTOR_NTSC)));
		if (machine_class == VICE_MACHINE_C64)
			submenu->AddItem(new BMenuItem("Old NTSC-M", 
				new BMessage(MENU_SYNC_FACTOR_NTSCOLD)));
	} else {
		menu->AddItem(submenu = new BMenu("Drive sync factor"));
		submenu->SetRadioMode(true);
		submenu->AddItem(new BMenuItem("PAL", 
			new BMessage(MENU_SYNC_FACTOR_PAL)));
		submenu->AddItem(new BMenuItem("NTSC", 
			new BMessage(MENU_SYNC_FACTOR_NTSC)));
	}

	if (!vsid_mode) {
		if (machine_class == VICE_MACHINE_C64
			|| machine_class == VICE_MACHINE_C128) {
			menu->AddSeparatorItem();
			menu->AddItem(new BMenuItem("REU emulation",
				new BMessage(MENU_TOGGLE_REU)));
			menu->AddItem(submenu = new BMenu("REU size"));
			submenu->SetRadioMode(true);
			submenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_REU_SIZE_128)));
			submenu->AddItem(new BMenuItem("256 kB",
				new BMessage(MENU_REU_SIZE_256)));
			submenu->AddItem(new BMenuItem("512 kB",
				new BMessage(MENU_REU_SIZE_512)));
			submenu->AddItem(new BMenuItem("1024 kB",
				new BMessage(MENU_REU_SIZE_1024)));
			submenu->AddItem(new BMenuItem("2048 kB",
				new BMessage(MENU_REU_SIZE_2048)));
			submenu->AddItem(new BMenuItem("4096 kB",
				new BMessage(MENU_REU_SIZE_4096)));
			submenu->AddItem(new BMenuItem("8192 kB",
				new BMessage(MENU_REU_SIZE_8192)));
			submenu->AddItem(new BMenuItem("16384 kB",
				new BMessage(MENU_REU_SIZE_16384)));

			menu->AddItem(new BMenuItem("GEORAM emulation",
				new BMessage(MENU_TOGGLE_GEORAM)));
			menu->AddItem(submenu = new BMenu("GEORAM size"));
			submenu->SetRadioMode(true);
			submenu->AddItem(new BMenuItem("64 kB",
				new BMessage(MENU_GEORAM_SIZE_64)));
			submenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_GEORAM_SIZE_128)));
			submenu->AddItem(new BMenuItem("256 kB",
				new BMessage(MENU_GEORAM_SIZE_256)));
			submenu->AddItem(new BMenuItem("512 kB",
				new BMessage(MENU_GEORAM_SIZE_512)));
			submenu->AddItem(new BMenuItem("1024 kB",
				new BMessage(MENU_GEORAM_SIZE_1024)));
			submenu->AddItem(new BMenuItem("2048 kB",
				new BMessage(MENU_GEORAM_SIZE_2048)));
			submenu->AddItem(new BMenuItem("4096 kB",
				new BMessage(MENU_GEORAM_SIZE_4096)));

			menu->AddItem(new BMenuItem("RAMCART emulation",
				new BMessage(MENU_TOGGLE_RAMCART)));
			menu->AddItem(submenu = new BMenu("RAMCART size"));
			submenu->SetRadioMode(true);
			submenu->AddItem(new BMenuItem("64 kB",
				new BMessage(MENU_RAMCART_SIZE_64)));
			submenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_RAMCART_SIZE_128)));

			menu->AddItem(new BMenuItem("+60K emulation",
				new BMessage(MENU_TOGGLE_PLUS60K)));
			menu->AddItem(submenu = new BMenu("PLUS60K base"));
			submenu->SetRadioMode(true);
			submenu->AddItem(new BMenuItem("$D040",
				new BMessage(MENU_PLUS60K_BASE_D040)));
			submenu->AddItem(new BMenuItem("$D100",
				new BMessage(MENU_PLUS60K_BASE_D100)));

			menu->AddItem(new BMenuItem("Emulator ID",
				new BMessage(MENU_TOGGLE_EMUID)));
			menu->AddItem(new BMenuItem("1351 mouse",
				new BMessage(MENU_TOGGLE_MOUSE)));
		}
		if (machine_class == VICE_MACHINE_VIC20
			|| machine_class == VICE_MACHINE_C128) {
			menu->AddItem(new BMenuItem("IEEE488 Interface",
				new BMessage(MENU_TOGGLE_IEEE488)));
		}
	}		
	
	/* create the SETTINGS menu */
	menu = new BMenu("Settings");
	menubar->AddItem(menu);

	if (!vsid_mode) {
		if (machine_class == VICE_MACHINE_CBM2) {
			menu->AddItem(new BMenuItem("CBM 2 ...", 
				new BMessage(MENU_CBM2_SETTINGS)));
		}
		if (machine_class == VICE_MACHINE_PET) {
			menu->AddItem(new BMenuItem("PET ...", 
				new BMessage(MENU_PET_SETTINGS)));
		}
		if (machine_class == VICE_MACHINE_VIC20) {
			menu->AddItem(new BMenuItem("VIC20 ...", 
				new BMessage(MENU_VIC20_SETTINGS)));
		}
	
		menu->AddItem(new BMenuItem("Video ...", 
			new BMessage(MENU_VIDEO_SETTINGS)));
		menu->AddItem(new BMenuItem("Device ...", 
			new BMessage(MENU_DEVICE_SETTINGS)));
		menu->AddItem(new BMenuItem("Drive ...", 
			new BMessage(MENU_DRIVE_SETTINGS)));
		menu->AddItem(new BMenuItem("Datasette ...",
			new BMessage(MENU_DATASETTE_SETTINGS)));
		if (machine_class == VICE_MACHINE_C64
			|| machine_class == VICE_MACHINE_C128) {
			menu->AddItem(new BMenuItem("VIC-II ...", 
				new BMessage(MENU_VICII_SETTINGS)));
		}
		menu->AddItem(new BMenuItem("Joystick ...", 
			new BMessage(MENU_JOYSTICK_SETTINGS)));
	}
	
	menu->AddItem(new BMenuItem("Sound ...", 
		new BMessage(MENU_SOUND_SETTINGS)));
	if (machine_class == VICE_MACHINE_C64
		|| machine_class == VICE_MACHINE_C128
		|| machine_class == VICE_MACHINE_CBM2) {
		menu->AddItem(new BMenuItem("SID ...", 
			new BMessage(MENU_SID_SETTINGS)));
	}
	menu->AddItem(new BMenuItem("RAM ...", 
		new BMessage(MENU_RAM_SETTINGS)));
		
	if (!vsid_mode) {
		menu->AddSeparatorItem();
		menu->AddItem(new BMenuItem("Load Settings", 
			new BMessage(MENU_SETTINGS_LOAD)));
		menu->AddItem(new BMenuItem("Save Settings", 
			new BMessage(MENU_SETTINGS_SAVE)));
		menu->AddItem(new BMenuItem("Default Settings", 
			new BMessage(MENU_SETTINGS_DEFAULT)));
		menu->AddSeparatorItem();
		menu->AddItem(new BMenuItem("Save settings on exit", 
			new BMessage(MENU_TOGGLE_SAVE_SETTINGS_ON_EXIT)));
		menu->AddItem(new BMenuItem("Confirm on exit", 
			new BMessage(MENU_TOGGLE_CONFIRM_ON_EXIT)));
	}
	
	/* create the HELP menu */
	menu = new BMenu("Help");
	menubar->AddItem(menu);

	menu->AddItem(new BMenuItem("About BeVICE...", 
		new BMessage(MENU_ABOUT)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Commandline Options", 
		new BMessage(MENU_CMDLINE)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Contributors", 
		new BMessage(MENU_CONTRIBUTORS)));
	menu->AddItem(new BMenuItem("License", 
		new BMessage(MENU_LICENSE)));
	menu->AddItem(new BMenuItem("No Warranty", 
		new BMessage(MENU_WARRANTY)));
	
	return menubar;
}
