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
	BMenu *menu, *submenu, *uppermenu, *extsubmenu;
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
		menu->AddItem(submenu = new BMenu("Netplay"));
		submenu->AddItem(new BMenuItem("Start Server",
			new BMessage(MENU_NETPLAY_SERVER)));
		submenu->AddItem(new BMenuItem("Connect Client",
			new BMessage(MENU_NETPLAY_CLIENT)));
		submenu->AddItem(new BMenuItem("Disconnect",
			new BMessage(MENU_NETPLAY_DISCONNECT)));
		submenu->AddItem(new BMenuItem("Settings...",
			new BMessage(MENU_NETPLAY_SETTINGS)));

		menu->AddItem(submenu = new BMenu("Event History"));
		submenu->AddItem(new BMenuItem("Start/Stop recording",
			new BMessage(MENU_EVENT_TOGGLE_RECORD)));
		submenu->AddItem(new BMenuItem("Start/Stop playback",
			new BMessage(MENU_EVENT_TOGGLE_PLAYBACK)));
		submenu->AddItem(new BMenuItem("Set Milestone",
			new BMessage(MENU_EVENT_SETMILESTONE), 'G'));
		submenu->AddItem(new BMenuItem("Return to Milestone",
			new BMessage(MENU_EVENT_RESETMILESTONE), 'H'));
		submenu->AddItem(new BMenuItem("Select start snapshot",
			new BMessage(MENU_EVENT_SNAPSHOT_START)));
		submenu->AddItem(new BMenuItem("Select end snapshot",
			new BMessage(MENU_EVENT_SNAPSHOT_END)));
		menu->AddItem(submenu = new BMenu("Recording start mode"));
		submenu->AddItem(new BMenuItem("Save new snapshot",
			new BMessage(MENU_EVENT_START_MODE_SAVE)));
		submenu->AddItem(new BMenuItem("Load existing snapshot",
			new BMessage(MENU_EVENT_START_MODE_LOAD)));
		submenu->AddItem(new BMenuItem("Start with Reset",
			new BMessage(MENU_EVENT_START_MODE_RESET)));
		submenu->AddItem(new BMenuItem("Overwrite Playback",
			new BMessage(MENU_EVENT_START_MODE_PLAYBACK)));

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
        menu->AddItem(submenu = new BMenu("Sound Recording"));
	submenu->AddItem(new BMenuItem("Sound Record AIFF",
		new BMessage(MENU_SOUND_RECORD_AIFF)));
	submenu->AddItem(new BMenuItem("Sound Record IFF",
		new BMessage(MENU_SOUND_RECORD_AIFF)));
#ifdef USE_LAMEMP3
	submenu->AddItem(new BMenuItem("Sound Record MP3",
		new BMessage(MENU_SOUND_RECORD_MP3)));
#endif
	submenu->AddItem(new BMenuItem("Sound Record VOC",
		new BMessage(MENU_SOUND_RECORD_VOC)));
	submenu->AddItem(new BMenuItem("Sound Record WAV",
		new BMessage(MENU_SOUND_RECORD_WAV)));
	submenu->AddItem(new BMenuItem("Stop Sound Record",
		new BMessage(MENU_SOUND_RECORD_STOP)));
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
			menu->AddItem(new BMenuItem("Emulator ID",
				new BMessage(MENU_TOGGLE_EMUID)));
			menu->AddItem(new BMenuItem("1351 mouse",
				new BMessage(MENU_TOGGLE_MOUSE)));
			menu->AddSeparatorItem();

			menu->AddItem(submenu = new BMenu("Expansion Carts"));
			uppermenu = menu;
			menu = submenu;
                        menu->AddItem(submenu = new BMenu("REU Options"));
			submenu->AddItem(new BMenuItem("REU emulation",
				new BMessage(MENU_TOGGLE_REU)));
			submenu->AddItem(extsubmenu = new BMenu("REU size"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_REU_SIZE_128)));
			extsubmenu->AddItem(new BMenuItem("256 kB",
				new BMessage(MENU_REU_SIZE_256)));
			extsubmenu->AddItem(new BMenuItem("512 kB",
				new BMessage(MENU_REU_SIZE_512)));
			extsubmenu->AddItem(new BMenuItem("1024 kB",
				new BMessage(MENU_REU_SIZE_1024)));
			extsubmenu->AddItem(new BMenuItem("2048 kB",
				new BMessage(MENU_REU_SIZE_2048)));
			extsubmenu->AddItem(new BMenuItem("4096 kB",
				new BMessage(MENU_REU_SIZE_4096)));
			extsubmenu->AddItem(new BMenuItem("8192 kB",
				new BMessage(MENU_REU_SIZE_8192)));
			extsubmenu->AddItem(new BMenuItem("16384 kB",
				new BMessage(MENU_REU_SIZE_16384)));
			submenu->AddItem(new BMenuItem("REU File",
				new BMessage(MENU_REU_FILE)));

			menu->AddItem(submenu = new BMenu("GEORAM Options"));
			submenu->AddItem(new BMenuItem("GEORAM emulation",
				new BMessage(MENU_TOGGLE_GEORAM)));
			submenu->AddItem(extsubmenu = new BMenu("GEORAM size"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("64 kB",
				new BMessage(MENU_GEORAM_SIZE_64)));
			extsubmenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_GEORAM_SIZE_128)));
			extsubmenu->AddItem(new BMenuItem("256 kB",
				new BMessage(MENU_GEORAM_SIZE_256)));
			extsubmenu->AddItem(new BMenuItem("512 kB",
				new BMessage(MENU_GEORAM_SIZE_512)));
			extsubmenu->AddItem(new BMenuItem("1024 kB",
				new BMessage(MENU_GEORAM_SIZE_1024)));
			extsubmenu->AddItem(new BMenuItem("2048 kB",
				new BMessage(MENU_GEORAM_SIZE_2048)));
			extsubmenu->AddItem(new BMenuItem("4096 kB",
				new BMessage(MENU_GEORAM_SIZE_4096)));
			submenu->AddItem(new BMenuItem("GEORAM File",
				new BMessage(MENU_GEORAM_FILE)));

			menu->AddItem(submenu = new BMenu("RAMCART Options"));
			submenu->AddItem(new BMenuItem("RAMCART emulation",
				new BMessage(MENU_TOGGLE_RAMCART)));
			submenu->AddItem(extsubmenu = new BMenu("RAMCART size"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("64 kB",
				new BMessage(MENU_RAMCART_SIZE_64)));
			extsubmenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_RAMCART_SIZE_128)));
			submenu->AddItem(new BMenuItem("RAMCART File",
				new BMessage(MENU_RAMCART_FILE)));

			menu->AddItem(submenu = new BMenu("Digimax Options"));
			submenu->AddItem(new BMenuItem("Digimax emulation",
				new BMessage(MENU_TOGGLE_DIGIMAX)));
			submenu->AddItem(extsubmenu = new BMenu("Digimax base"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("Userport Interface",
				new BMessage(MENU_DIGIMAX_BASE_DD00)));
			extsubmenu->AddItem(new BMenuItem("$DE00",
				new BMessage(MENU_DIGIMAX_BASE_DE00)));
			extsubmenu->AddItem(new BMenuItem("$DE20",
				new BMessage(MENU_DIGIMAX_BASE_DE20)));
			extsubmenu->AddItem(new BMenuItem("$DE40",
				new BMessage(MENU_DIGIMAX_BASE_DE40)));
			extsubmenu->AddItem(new BMenuItem("$DE60",
				new BMessage(MENU_DIGIMAX_BASE_DE60)));
			extsubmenu->AddItem(new BMenuItem("$DE80",
				new BMessage(MENU_DIGIMAX_BASE_DE80)));
			extsubmenu->AddItem(new BMenuItem("$DEA0",
				new BMessage(MENU_DIGIMAX_BASE_DEA0)));
			extsubmenu->AddItem(new BMenuItem("$DEC0",
				new BMessage(MENU_DIGIMAX_BASE_DEC0)));
			extsubmenu->AddItem(new BMenuItem("$DEE0",
				new BMessage(MENU_DIGIMAX_BASE_DEE0)));
			extsubmenu->AddItem(new BMenuItem("$DF00",
				new BMessage(MENU_DIGIMAX_BASE_DF00)));
			extsubmenu->AddItem(new BMenuItem("$DF20",
				new BMessage(MENU_DIGIMAX_BASE_DF20)));
			extsubmenu->AddItem(new BMenuItem("$DF40",
				new BMessage(MENU_DIGIMAX_BASE_DF40)));
			extsubmenu->AddItem(new BMenuItem("$DF60",
				new BMessage(MENU_DIGIMAX_BASE_DF60)));
			extsubmenu->AddItem(new BMenuItem("$DF80",
				new BMessage(MENU_DIGIMAX_BASE_DF80)));
			extsubmenu->AddItem(new BMenuItem("$DFA0",
				new BMessage(MENU_DIGIMAX_BASE_DFA0)));
			extsubmenu->AddItem(new BMenuItem("$DFC0",
				new BMessage(MENU_DIGIMAX_BASE_DFC0)));
			extsubmenu->AddItem(new BMenuItem("$DFE0",
				new BMessage(MENU_DIGIMAX_BASE_DFE0)));

			if (machine_class == VICE_MACHINE_C64) {
				menu->AddItem(submenu = new BMenu("PLUS60K Options"));
				submenu->AddItem(new BMenuItem("PLUS60K emulation",
					new BMessage(MENU_TOGGLE_PLUS60K)));
				submenu->AddItem(extsubmenu = new BMenu("PLUS60K base"));
				extsubmenu->SetRadioMode(true);
				extsubmenu->AddItem(new BMenuItem("$D040",
					new BMessage(MENU_PLUS60K_BASE_D040)));
				extsubmenu->AddItem(new BMenuItem("$D100",
					new BMessage(MENU_PLUS60K_BASE_D100)));
				submenu->AddItem(new BMenuItem("PLUS60K File",
					new BMessage(MENU_PLUS60K_FILE)));

				menu->AddItem(submenu = new BMenu("PLUS256K Options"));
				submenu->AddItem(new BMenuItem("PLUS256K emulation",
					new BMessage(MENU_TOGGLE_PLUS256K)));
				submenu->AddItem(new BMenuItem("PLUS256K File",
					new BMessage(MENU_PLUS256K_FILE)));

				menu->AddItem(submenu = new BMenu("C64_256K Options"));
				submenu->AddItem(new BMenuItem("C64_256K emulation",
					new BMessage(MENU_TOGGLE_C64_256K)));
				submenu->AddItem(extsubmenu = new BMenu("C64_256K base"));
				extsubmenu->SetRadioMode(true);
				extsubmenu->AddItem(new BMenuItem("$DE00-$DE7F",
					new BMessage(MENU_C64_256K_BASE_DE00)));
				extsubmenu->AddItem(new BMenuItem("$DE80-$DEFF",
					new BMessage(MENU_C64_256K_BASE_DE80)));
				extsubmenu->AddItem(new BMenuItem("$DF00-$DF7F",
					new BMessage(MENU_C64_256K_BASE_DF00)));
				extsubmenu->AddItem(new BMenuItem("$DF80-$DFFF",
					new BMessage(MENU_C64_256K_BASE_DF80)));
				submenu->AddItem(new BMenuItem("C64_256K File",
					new BMessage(MENU_C64_256K_FILE)));

				menu->AddItem(submenu = new BMenu("MMC64 Options"));
				submenu->AddItem(new BMenuItem("MMC64 emulation",
					new BMessage(MENU_TOGGLE_MMC64)));
				submenu->AddItem(extsubmenu = new BMenu("MMC64 revision"));
				extsubmenu->SetRadioMode(true);
				extsubmenu->AddItem(new BMenuItem("Revision A",
					new BMessage(MENU_MMC64_REVISION_A)));
				extsubmenu->AddItem(new BMenuItem("Revision B",
					new BMessage(MENU_MMC64_REVISION_B)));
				submenu->AddItem(new BMenuItem("MMC64 BIOS jumper",
					new BMessage(MENU_TOGGLE_MMC64_FLASHJUMPER)));
				submenu->AddItem(new BMenuItem("MMC64 BIOS save when changed",
					new BMessage(MENU_TOGGLE_MMC64_SAVE)));
				submenu->AddItem(new BMenuItem("MMC64 BIOS File",
					new BMessage(MENU_MMC64_BIOS_FILE)));
				submenu->AddItem(new BMenuItem("MMC64 Image read-only",
					new BMessage(MENU_TOGGLE_MMC64_READ_ONLY)));
				submenu->AddItem(new BMenuItem("MMC64 Image File",
					new BMessage(MENU_MMC64_IMAGE_FILE)));
			}

			menu = uppermenu;
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

			menu->AddItem(submenu = new BMenu("PET REU Options"));
			submenu->AddItem(new BMenuItem("PET REU emulation",
				new BMessage(MENU_TOGGLE_PETREU)));
			submenu->AddItem(extsubmenu = new BMenu("PET REU size"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("128 kB",
				new BMessage(MENU_PETREU_SIZE_128)));
			extsubmenu->AddItem(new BMenuItem("512 kB",
				new BMessage(MENU_PETREU_SIZE_512)));
			extsubmenu->AddItem(new BMenuItem("1024 kB",
				new BMessage(MENU_PETREU_SIZE_1024)));
			extsubmenu->AddItem(new BMenuItem("2048 kB",
				new BMessage(MENU_PETREU_SIZE_2048)));
			menu->AddItem(new BMenuItem("PET REU File",
				new BMessage(MENU_PETREU_FILE)));
		}
		if (machine_class == VICE_MACHINE_VIC20) {
			menu->AddItem(new BMenuItem("VIC20 ...", 
				new BMessage(MENU_VIC20_SETTINGS)));
		}
		if (machine_class == VICE_MACHINE_VIC20 ||
		    machine_class == VICE_MACHINE_PLUS4 ||
                machine_class == VICE_MACHINE_PET) {

			menu->AddItem(submenu = new BMenu("SIDCART Options"));
			submenu->AddItem(new BMenuItem("SIDCART emulation",
				new BMessage(MENU_TOGGLE_SIDCART)));
			submenu->AddItem(extsubmenu = new BMenu("SIDCART model"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("6581",
				new BMessage(MENU_SIDCART_MODEL_6581)));
			extsubmenu->AddItem(new BMenuItem("8580",
				new BMessage(MENU_SIDCART_MODEL_8580)));
			submenu->AddItem(new BMenuItem("SIDCART filters",
				new BMessage(MENU_TOGGLE_SIDCART_FILTERS)));
			submenu->AddItem(extsubmenu = new BMenu("SIDCART address"));
			extsubmenu->SetRadioMode(true);
                  if (machine_class == VICE_MACHINE_PET) {
				extsubmenu->AddItem(new BMenuItem("$8F00",
					new BMessage(MENU_SIDCART_ADDRESS_1)));
				extsubmenu->AddItem(new BMenuItem("$E900",
					new BMessage(MENU_SIDCART_ADDRESS_2)));
			}
                  if (machine_class == VICE_MACHINE_PLUS4) {
				extsubmenu->AddItem(new BMenuItem("$FD40",
					new BMessage(MENU_SIDCART_ADDRESS_1)));
				extsubmenu->AddItem(new BMenuItem("$FE80",
					new BMessage(MENU_SIDCART_ADDRESS_2)));
			}
                  if (machine_class == VICE_MACHINE_VIC20) {
				extsubmenu->AddItem(new BMenuItem("$9800",
					new BMessage(MENU_SIDCART_ADDRESS_1)));
				extsubmenu->AddItem(new BMenuItem("$9C00",
					new BMessage(MENU_SIDCART_ADDRESS_2)));
			}
			submenu->AddItem(extsubmenu = new BMenu("SIDCART clock"));
			extsubmenu->SetRadioMode(true);
			extsubmenu->AddItem(new BMenuItem("C64",
				new BMessage(MENU_SIDCART_CLOCK_C64)));
                  if (machine_class == VICE_MACHINE_PET) {
				extsubmenu->AddItem(new BMenuItem("PET",
					new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
			}
                  if (machine_class == VICE_MACHINE_PLUS4) {
				extsubmenu->AddItem(new BMenuItem("PLUS4",
					new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
			}
                  if (machine_class == VICE_MACHINE_VIC20) {
				extsubmenu->AddItem(new BMenuItem("PLUS4",
					new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
			}
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
