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

void vicemenu_free_tune_menu(void)
{
    BMenuItem *mi;

    while (mi = vsid_tune_menu->FindItem(MENU_VSID_TUNE)) {
        vsid_tune_menu->RemoveItem(mi);
        delete mi;
    }
}

void vicemenu_tune_menu_add(int tune)
{
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

    if (tune < 10) {
        vsid_tune_menu->AddItem(new BMenuItem(menustr, msg, shortcut));
    } else {
        vsid_tune_menu->AddItem(new BMenuItem(menustr, msg));
    }
}

BMenuBar *menu_create(int machine_class)
{
    BMenuBar *menubar;
    BMenu *uppermenu, *menu, *submenu, *extsubmenu;
    BMenuItem *item;

    menubar = new BMenuBar(BRect(0, 0, 10, 10), "Menubar");

    /* create the FILE menu */
    uppermenu = new BMenu("File");
    menubar->AddItem(uppermenu);

    if (!vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Autostart Settings"));
            menu->AddItem(new BMenuItem("Autostart warp", new BMessage(MENU_AUTOSTART_WARP)));
            menu->AddItem(new BMenuItem("Use ':' with run", new BMessage(MENU_USE_COLON_WITH_RUN)));
            menu->AddItem(submenu = new BMenu("PRG autostart mode"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("Virtual FS", new BMessage(MENU_AUYOSTART_PRG_VIRTUAL_FS)));
                submenu->AddItem(new BMenuItem("Inject", new BMessage(MENU_AUYOSTART_PRG_INJECT)));
                submenu->AddItem(new BMenuItem("Disk image", new BMessage(MENU_AUYOSTART_PRG_DISK_IMAGE)));
            menu->AddItem(new BMenuItem("Select file for PRG autostart disk", new BMessage(MENU_AUYOSTART_PRG_DISK_IMAGE_SELECT)));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(new BMenuItem("Autostart", new BMessage(MENU_AUTOSTART), 'A'));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(menu = new BMenu("Attach Disk"));
            menu->AddItem(new BMenuItem("Drive 8", new BMessage(MENU_ATTACH_DISK8), '8'));
            menu->AddItem(new BMenuItem("Drive 9", new BMessage(MENU_ATTACH_DISK9), '9'));
            menu->AddItem(new BMenuItem("Drive 10", new BMessage(MENU_ATTACH_DISK10)));
            menu->AddItem(new BMenuItem("Drive 11", new BMessage(MENU_ATTACH_DISK11)));
        uppermenu->AddItem(menu = new BMenu("Detach Disk"));
            menu->AddItem(new BMenuItem("Drive 8", new BMessage(MENU_DETACH_DISK8)));
            menu->AddItem(new BMenuItem("Drive 9", new BMessage(MENU_DETACH_DISK9)));
            menu->AddItem(new BMenuItem("Drive 10", new BMessage(MENU_DETACH_DISK10)));
            menu->AddItem(new BMenuItem("Drive 11", new BMessage(MENU_DETACH_DISK11)));
        uppermenu->AddItem(menu = new BMenu("Flip List"));
            menu->AddItem(new BMenuItem("Add current image", new BMessage(MENU_FLIP_ADD), 'I'));
            menu->AddItem(new BMenuItem("Remove Current Image", new BMessage(MENU_FLIP_REMOVE), 'K'));
            menu->AddItem(new BMenuItem("Attach next image", new BMessage(MENU_FLIP_NEXT), 'N'));
            menu->AddItem(new BMenuItem("Attach previous image", new BMessage(MENU_FLIP_PREVIOUS), 'N', B_CONTROL_KEY));
        uppermenu->AddSeparatorItem();
    }

    if (machine_class != VICE_MACHINE_C64DTV && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Attach Tape", new BMessage(MENU_ATTACH_TAPE), 'T'));
        uppermenu->AddItem(new BMenuItem("Detach Tape", new BMessage(MENU_DETACH_TAPE)));
        uppermenu->AddItem(menu = new BMenu("Datasette Control"));
            menu->AddItem(new BMenuItem("Start", new BMessage(MENU_DATASETTE_START)));
            menu->AddItem(new BMenuItem("Stop", new BMessage(MENU_DATASETTE_STOP)));
            menu->AddItem(new BMenuItem("Forward", new BMessage(MENU_DATASETTE_FORWARD)));
            menu->AddItem(new BMenuItem("Rewind", new BMessage(MENU_DATASETTE_REWIND)));
            menu->AddItem(new BMenuItem("Record", new BMessage(MENU_DATASETTE_RECORD)));
            menu->AddItem(new BMenuItem("Reset", new BMessage(MENU_DATASETTE_RESET)));
            menu->AddItem(new BMenuItem("Reset Counter", new BMessage(MENU_DATASETTE_COUNTER)));
        uppermenu->AddSeparatorItem();
    }

    if (machine_class == VICE_MACHINE_C64 && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Attach cartridge image"));
            menu->AddItem(new BMenuItem("CRT", new BMessage(MENU_CART_ATTACH_CRT)));
            menu->AddItem(new BMenuItem("Generic 8KB", new BMessage(MENU_CART_ATTACH_8KB)));
            menu->AddItem(new BMenuItem("Generic 16KB", new BMessage(MENU_CART_ATTACH_16KB)));
            menu->AddItem(new BMenuItem("Action Replay", new BMessage(MENU_CART_ATTACH_AR)));
            menu->AddItem(new BMenuItem("Action Replay 3", new BMessage(MENU_CART_ATTACH_AR3)));
            menu->AddItem(new BMenuItem("Action Replay 4", new BMessage(MENU_CART_ATTACH_AR4)));
            menu->AddItem(new BMenuItem("StarDOS", new BMessage(MENU_CART_ATTACH_STARDOS)));
            menu->AddItem(new BMenuItem("Atomic Power", new BMessage(MENU_CART_ATTACH_AT)));
            menu->AddItem(new BMenuItem("Epyx fastload", new BMessage(MENU_CART_ATTACH_EPYX)));
            menu->AddItem(new BMenuItem("IEEE488 interface", new BMessage(MENU_CART_ATTACH_IEEE488)));
            menu->AddItem(new BMenuItem("IDE64 interface", new BMessage(MENU_CART_ATTACH_IDE64)));
            menu->AddItem(new BMenuItem("Super Snapshot 4", new BMessage(MENU_CART_ATTACH_SS4)));
            menu->AddItem(new BMenuItem("Super Snapshot 5", new BMessage(MENU_CART_ATTACH_SS5)));
            menu->AddSeparatorItem();
            menu->AddItem(new BMenuItem("Set cartridge as default", new BMessage(MENU_CART_SET_DEFAULT)));
        uppermenu->AddItem(new BMenuItem("Detach cartridge image", new BMessage(MENU_CART_DETACH)));
        uppermenu->AddItem(new BMenuItem("Cartridge freeze", new BMessage(MENU_CART_FREEZE), 'Z'));
        uppermenu->AddSeparatorItem();
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Attach cartridge image"));
            menu->AddItem(new BMenuItem("Generic cartridge image", new BMessage(MENU_CART_VIC20_GENERIC)));
            menu->AddItem(new BMenuItem("Mega-Cart image", new BMessage(MENU_CART_VIC20_MEGACART)));
            menu->AddItem(new BMenuItem("Final Expansion image", new BMessage(MENU_CART_VIC20_FINAL_EXPANSION)));
            menu->AddItem(new BMenuItem("Vic Flash Plugin image", new BMessage(MENU_CART_VIC20_FP)));
            menu->AddItem(submenu = new BMenu("Add to generic"));
                submenu->AddItem(new BMenuItem("Smart Attach", new BMessage(MENU_CART_VIC20_SMART_ATTACH)));
                submenu->AddItem(new BMenuItem("4/8/16KB image at $2000", new BMessage(MENU_CART_VIC20_16KB_2000)));
                submenu->AddItem(new BMenuItem("4/8/16KB image at $4000", new BMessage(MENU_CART_VIC20_16KB_4000)));
                submenu->AddItem(new BMenuItem("4/8/16KB image at $6000", new BMessage(MENU_CART_VIC20_16KB_6000)));
                submenu->AddItem(new BMenuItem("4/8KB image at $A000", new BMessage(MENU_CART_VIC20_8KB_A000)));
                submenu->AddItem(new BMenuItem("4KB image at $B000", new BMessage(MENU_CART_VIC20_4KB_B000)));
        uppermenu->AddItem(new BMenuItem("Detach cartridge image", new BMessage(MENU_CART_DETACH)));
        uppermenu->AddSeparatorItem();
    }
                        
    if (!vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Snapshot"));
            menu->AddItem(new BMenuItem("Load snapshot", new BMessage(MENU_SNAPSHOT_LOAD)));
            menu->AddItem(new BMenuItem("Save snapshot", new BMessage(MENU_SNAPSHOT_SAVE)));
            menu->AddItem(new BMenuItem("Load quicksnapshot", new BMessage(MENU_LOADQUICK), 'L', B_CONTROL_KEY));
            menu->AddItem(new BMenuItem("Save quicksnapshot", new BMessage(MENU_SAVEQUICK), 'S', B_CONTROL_KEY));
        uppermenu->AddItem(menu = new BMenu("Netplay"));
            menu->AddItem(new BMenuItem("Start Server", new BMessage(MENU_NETPLAY_SERVER)));
            menu->AddItem(new BMenuItem("Connect Client", new BMessage(MENU_NETPLAY_CLIENT)));
            menu->AddItem(new BMenuItem("Disconnect", new BMessage(MENU_NETPLAY_DISCONNECT)));
            menu->AddItem(new BMenuItem("Settings...", new BMessage(MENU_NETPLAY_SETTINGS)));

        uppermenu->AddItem(menu = new BMenu("Event History"));
            menu->AddItem(new BMenuItem("Start/Stop recording", new BMessage(MENU_EVENT_TOGGLE_RECORD)));
            menu->AddItem(new BMenuItem("Start/Stop playback", new BMessage(MENU_EVENT_TOGGLE_PLAYBACK)));
            menu->AddItem(new BMenuItem("Set Milestone", new BMessage(MENU_EVENT_SETMILESTONE), 'G'));
            menu->AddItem(new BMenuItem("Return to Milestone", new BMessage(MENU_EVENT_RESETMILESTONE), 'H'));
            menu->AddItem(new BMenuItem("Select start snapshot", new BMessage(MENU_EVENT_SNAPSHOT_START)));
            menu->AddItem(new BMenuItem("Select end snapshot", new BMessage(MENU_EVENT_SNAPSHOT_END)));
        uppermenu->AddItem(menu = new BMenu("Recording start mode"));
            menu->AddItem(new BMenuItem("Save new snapshot", new BMessage(MENU_EVENT_START_MODE_SAVE)));
            menu->AddItem(new BMenuItem("Load existing snapshot", new BMessage(MENU_EVENT_START_MODE_LOAD)));
            menu->AddItem(new BMenuItem("Start with Reset", new BMessage(MENU_EVENT_START_MODE_RESET)));
            menu->AddItem(new BMenuItem("Overwrite Playback", new BMessage(MENU_EVENT_START_MODE_PLAYBACK)));
    }

    if (vsid_mode) {
        /* vsid */
        uppermenu->AddItem(new BMenuItem("Load psid file", new BMessage(MENU_VSID_LOAD)));
        uppermenu->AddItem(vsid_tune_menu = new BMenu("Tune"));
    }

    uppermenu->AddSeparatorItem();
    uppermenu->AddItem(item = new BMenuItem("Pause", new BMessage(MENU_PAUSE), 'P'));
    uppermenu->AddItem(item = new BMenuItem("Monitor", new BMessage(MENU_MONITOR), 'M'));
    uppermenu->AddItem(item = new BMenuItem("Soft Reset", new BMessage(MENU_RESET_SOFT), 'R'));
    uppermenu->AddItem(new BMenuItem("Hard Reset", new BMessage(MENU_RESET_HARD), 'R', B_CONTROL_KEY));
    uppermenu->AddSeparatorItem();
    uppermenu->AddItem(new BMenuItem("Quit", new BMessage(MENU_EXIT_REQUESTED), 'Q'));

    /* create the EDIT menu */
    uppermenu = new BMenu("Edit");
    menubar->AddItem(uppermenu);

    if (!vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Copy", new BMessage(MENU_COPY)));
        uppermenu->AddItem(new BMenuItem("Paste", new BMessage(MENU_PASTE)));
    }

    /* create the OPTIONS menu */
    uppermenu = new BMenu("Options");
    menubar->AddItem(uppermenu);

    if (!vsid_mode) {
        /* refresh rate */
        uppermenu->AddItem(menu = new BMenu("Refresh Rate"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("Auto", new BMessage(MENU_REFRESH_RATE_AUTO)));
            menu->AddItem(new BMenuItem("1/1", new BMessage(MENU_REFRESH_RATE_1)));
            menu->AddItem(new BMenuItem("1/2", new BMessage(MENU_REFRESH_RATE_2)));
            menu->AddItem(new BMenuItem("1/3", new BMessage(MENU_REFRESH_RATE_3)));
            menu->AddItem(new BMenuItem("1/4", new BMessage(MENU_REFRESH_RATE_4)));
            menu->AddItem(new BMenuItem("1/5", new BMessage(MENU_REFRESH_RATE_5)));
            menu->AddItem(new BMenuItem("1/6", new BMessage(MENU_REFRESH_RATE_6)));
            menu->AddItem(new BMenuItem("1/7", new BMessage(MENU_REFRESH_RATE_7)));
            menu->AddItem(new BMenuItem("1/8", new BMessage(MENU_REFRESH_RATE_8)));
            menu->AddItem(new BMenuItem("1/9", new BMessage(MENU_REFRESH_RATE_9)));
            menu->AddItem(new BMenuItem("1/10", new BMessage(MENU_REFRESH_RATE_10)));

        /* maximum speed */
        uppermenu->AddItem(menu = new BMenu("Maximum Speed"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("No Limit", new BMessage(MENU_MAXIMUM_SPEED_NO_LIMIT)));
            menu->AddItem(new BMenuItem("200%", new BMessage(MENU_MAXIMUM_SPEED_200)));
            menu->AddItem(new BMenuItem("100%", new BMessage(MENU_MAXIMUM_SPEED_100)));
            menu->AddItem(new BMenuItem("50%", new BMessage(MENU_MAXIMUM_SPEED_50)));
            menu->AddItem(new BMenuItem("20%", new BMessage(MENU_MAXIMUM_SPEED_20)));
            menu->AddItem(new BMenuItem("10%", new BMessage(MENU_MAXIMUM_SPEED_10)));
        uppermenu->AddItem(new BMenuItem("Warp Mode", new BMessage(MENU_TOGGLE_WARP_MODE),'W'));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(new BMenuItem("DirectWindow", new BMessage(MENU_TOGGLE_DIRECTWINDOW)));
        uppermenu->AddItem(new BMenuItem("Video Cache", new BMessage(MENU_TOGGLE_VIDEOCACHE)));
        uppermenu->AddItem(new BMenuItem("Double Size", new BMessage(MENU_TOGGLE_DOUBLESIZE),'D'));
        uppermenu->AddItem(new BMenuItem("Double Scan", new BMessage(MENU_TOGGLE_DOUBLESCAN)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C128 ||
         machine_class == VICE_MACHINE_VIC20) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("PAL emulation", new BMessage(MENU_TOGGLE_FASTPAL), 'P', B_CONTROL_KEY));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64DTV ||
         machine_class == VICE_MACHINE_C128 ||
         machine_class == VICE_MACHINE_PLUS4 ||
         machine_class == VICE_MACHINE_VIC20) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Scale2x", new BMessage(MENU_TOGGLE_SCALE2X), 'S'));
    }

    if (machine_class == VICE_MACHINE_C128 && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("VDC"));
            menu->AddItem(new BMenuItem("Double Size", new BMessage(MENU_TOGGLE_VDC_DOUBLESIZE)));
            menu->AddItem(new BMenuItem("Double Scan", new BMessage(MENU_TOGGLE_VDC_DOUBLESCAN)));
            menu->AddSeparatorItem();
            menu->AddItem(new BMenuItem("64KB video memory", new BMessage(MENU_TOGGLE_VDC64KB)));
        uppermenu->AddItem(menu = new BMenu("VDC revision"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("Rev 0", new BMessage(MENU_VDC_REV_0)));
            menu->AddItem(new BMenuItem("Rev 1", new BMessage(MENU_VDC_REV_1)));
            menu->AddItem(new BMenuItem("Rev 2", new BMessage(MENU_VDC_REV_2)));
    }

    if (!vsid_mode) {
        uppermenu->AddSeparatorItem();
    }

    if ((machine_class == VICE_MACHINE_C128 ||
         machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64DTV) && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("VIC-II Border mode"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("Normal", new BMessage(MENU_VICII_BORDERS_NORMAL)));
            menu->AddItem(new BMenuItem("Full", new BMessage(MENU_VICII_BORDERS_FULL)));
            menu->AddItem(new BMenuItem("Debug", new BMessage(MENU_VICII_BORDERS_DEBUG)));
    }

    /* sound options */
    uppermenu->AddItem(new BMenuItem("Sound", new BMessage(MENU_TOGGLE_SOUND)));
    uppermenu->AddItem(menu = new BMenu("Sound Recording"));
        menu->AddItem(new BMenuItem("Sound Record AIFF", new BMessage(MENU_SOUND_RECORD_AIFF)));
        menu->AddItem(new BMenuItem("Sound Record IFF", new BMessage(MENU_SOUND_RECORD_IFF)));
#ifdef USE_LAMEMP3
        menu->AddItem(new BMenuItem("Sound Record MP3", new BMessage(MENU_SOUND_RECORD_MP3)));
#endif
        menu->AddItem(new BMenuItem("Sound Record VOC", new BMessage(MENU_SOUND_RECORD_VOC)));
        menu->AddItem(new BMenuItem("Sound Record WAV", new BMessage(MENU_SOUND_RECORD_WAV)));
        menu->AddItem(new BMenuItem("Stop Sound Record", new BMessage(MENU_SOUND_RECORD_STOP)));
        menu->AddSeparatorItem();

    if (!vsid_mode) {
        uppermenu->AddItem(new BMenuItem("True Drive Emulation", new BMessage(MENU_TOGGLE_DRIVE_TRUE_EMULATION)));
        uppermenu->AddItem(new BMenuItem("Handle TDE for autostart", new BMessage(MENU_TOGGLE_HANDLE_TDE_AUTOSTART)));
        uppermenu->AddItem(new BMenuItem("Virtual Devices", new BMessage(MENU_TOGGLE_VIRTUAL_DEVICES)));
        uppermenu->AddSeparatorItem();
    }

    if (machine_class == VICE_MACHINE_C64 ||
        machine_class == VICE_MACHINE_C128 ||
        machine_class == VICE_MACHINE_VIC20 ||
        machine_class == VICE_MACHINE_C64DTV) {
        uppermenu->AddItem(menu = new BMenu("Video Standard"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("PAL-G", new BMessage(MENU_SYNC_FACTOR_PAL)));
            menu->AddItem(new BMenuItem("NTSC-M", new BMessage(MENU_SYNC_FACTOR_NTSC)));
    }

    if (machine_class == VICE_MACHINE_C64) {
            menu->AddItem(new BMenuItem("Old NTSC-M", new BMessage(MENU_SYNC_FACTOR_NTSCOLD)));
            menu->AddItem(new BMenuItem("PAL-N", new BMessage(MENU_SYNC_FACTOR_PALN)));
    }

    if (machine_class == VICE_MACHINE_C64SC) {
        uppermenu->AddItem(menu = new BMenu("C64 model"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("C64 PAL", new BMessage(MENU_C64_MODEL_C64_PAL)));
            menu->AddItem(new BMenuItem("C64C PAL", new BMessage(MENU_C64_MODEL_C64C_PAL)));
            menu->AddItem(new BMenuItem("C64 old PAL", new BMessage(MENU_C64_MODEL_C64_OLD_PAL)));
            menu->AddItem(new BMenuItem("C64 NTSC", new BMessage(MENU_C64_MODEL_C64_NTSC)));
            menu->AddItem(new BMenuItem("C64C NTSC", new BMessage(MENU_C64_MODEL_C64C_NTSC)));
            menu->AddItem(new BMenuItem("C64 old NTSC", new BMessage(MENU_C64_MODEL_C64_OLD_NTSC)));
            menu->AddItem(new BMenuItem("Drean", new BMessage(MENU_C64_MODEL_DREAN)));
            menu->AddItem(submenu = new BMenu("Custom"));
                submenu->AddItem(extsubmenu = new BMenu("VICII model"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("6569 (PAL)", new BMessage(MENU_VICII_MODEL_6569_PAL)));
                    extsubmenu->AddItem(new BMenuItem("8565 (PAL)", new BMessage(MENU_VICII_MODEL_8565_PAL)));
                    extsubmenu->AddItem(new BMenuItem("6569R1 (old PAL)", new BMessage(MENU_VICII_MODEL_6569R1_OLD_PAL)));
                    extsubmenu->AddItem(new BMenuItem("6567 (NTSC)", new BMessage(MENU_VICII_MODEL_6567_NTSC)));
                    extsubmenu->AddItem(new BMenuItem("8562 (NTSC)", new BMessage(MENU_VICII_MODEL_8562_NTSC)));
                    extsubmenu->AddItem(new BMenuItem("6567R56A (old NTSC)", new BMessage(MENU_VICII_MODEL_6567R56A_OLD_NTSC)));
                    extsubmenu->AddItem(new BMenuItem("6572 (PAL-N)", new BMessage(MENU_VICII_MODEL_6572_PAL_N)));
                submenu->AddItem(new BMenuItem("New luminances", new BMessage(MENU_VICII_NEW_LUMINANCES)));
                submenu->AddItem(extsubmenu = new BMenu("CIA-1 model"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("6526 (old)", new BMessage(MENU_CIA1_MODEL_6526_OLD)));
                    extsubmenu->AddItem(new BMenuItem("6526A (new)", new BMessage(MENU_CIA1_MODEL_6526A_NEW)));
                submenu->AddItem(extsubmenu = new BMenu("CIA-2 model"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("6526 (old)", new BMessage(MENU_CIA2_MODEL_6526_OLD)));
                    extsubmenu->AddItem(new BMenuItem("6526A (new)", new BMessage(MENU_CIA2_MODEL_6526A_NEW)));
                submenu->AddItem(extsubmenu = new BMenu("Glue logic"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("discrete", new BMessage(MENU_GLUE_LOGIC_DISCRETE)));
                    extsubmenu->AddItem(new BMenuItem("6526A (new)", new BMessage(MENU_GLUE_LOGIC_CUSTOM_IC)));
    }

    if (machine_class == VICE_MACHINE_PET ||
        machine_class == VICE_MACHINE_PLUS4 ||
        machine_class == VICE_MACHINE_CBM5x0 ||
        machine_class == VICE_MACHINE_CBM6x0) {
        uppermenu->AddItem(menu = new BMenu("Drive sync factor"));
            menu->SetRadioMode(true);
            menu->AddItem(new BMenuItem("PAL", new BMessage(MENU_SYNC_FACTOR_PAL)));
            menu->AddItem(new BMenuItem("NTSC", new BMessage(MENU_SYNC_FACTOR_NTSC)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64DTV ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        uppermenu->AddSeparatorItem();
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Emulator ID", new BMessage(MENU_TOGGLE_EMUID)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64DTV ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Grab mouse events", new BMessage(MENU_TOGGLE_MOUSE)));
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(new BMenuItem("Enable Paddles", new BMessage(MENU_TOGGLE_MOUSE)));
    }

    if (machine_class == VICE_MACHINE_C64DTV && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("PS/2 mouse", new BMessage(MENU_TOGGLE_PS2MOUSE)));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(menu = new BMenu("C64DTV Options"));
            menu->AddItem(new BMenuItem("C64DTV ROM File", new BMessage(MENU_C64DTV_ROM_FILE)));
            menu->AddItem(submenu = new BMenu("C64DTV revision"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("DTV2", new BMessage(MENU_C64DTV_REVISION_2)));
                submenu->AddItem(new BMenuItem("DTV3", new BMessage(MENU_C64DTV_REVISION_3)));
            menu->AddItem(new BMenuItem("C64DTV ROM writable", new BMessage(MENU_TOGGLE_C64DTV_WRITE_ENABLE)));
            menu->AddItem(new BMenuItem("Enable Hummer ADC", new BMessage(MENU_HUMMER_USERPORT_ADC)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Mouse Options"));
            menu->AddItem(submenu = new BMenu("Mouse Type"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("1351", new BMessage(MENU_MOUSE_TYPE_1351)));
                submenu->AddItem(new BMenuItem("NEOS", new BMessage(MENU_MOUSE_TYPE_NEOS)));
                submenu->AddItem(new BMenuItem("AMIGA", new BMessage(MENU_MOUSE_TYPE_AMIGA)));
                submenu->AddItem(new BMenuItem("PADDLE", new BMessage(MENU_MOUSE_TYPE_PADDLE)));
            menu->AddItem(submenu = new BMenu("Mouse Port"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("Joy1", new BMessage(MENU_MOUSE_PORT_JOY1)));
                submenu->AddItem(new BMenuItem("Joy2", new BMessage(MENU_MOUSE_PORT_JOY2)));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(menu = new BMenu("Expansion Carts"));
            menu->AddItem(submenu = new BMenu("REU Options"));
                submenu->AddItem(new BMenuItem("REU emulation", new BMessage(MENU_TOGGLE_REU)));
                submenu->AddItem(extsubmenu = new BMenu("REU size"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("128 kB", new BMessage(MENU_REU_SIZE_128)));
                    extsubmenu->AddItem(new BMenuItem("256 kB", new BMessage(MENU_REU_SIZE_256)));
                    extsubmenu->AddItem(new BMenuItem("512 kB", new BMessage(MENU_REU_SIZE_512)));
                    extsubmenu->AddItem(new BMenuItem("1024 kB", new BMessage(MENU_REU_SIZE_1024)));
                    extsubmenu->AddItem(new BMenuItem("2048 kB", new BMessage(MENU_REU_SIZE_2048)));
                    extsubmenu->AddItem(new BMenuItem("4096 kB", new BMessage(MENU_REU_SIZE_4096)));
                    extsubmenu->AddItem(new BMenuItem("8192 kB", new BMessage(MENU_REU_SIZE_8192)));
                    extsubmenu->AddItem(new BMenuItem("16384 kB", new BMessage(MENU_REU_SIZE_16384)));
                submenu->AddItem(new BMenuItem("REU File", new BMessage(MENU_REU_FILE)));
            menu->AddItem(submenu = new BMenu("GEORAM Options"));
                submenu->AddItem(new BMenuItem("GEORAM emulation", new BMessage(MENU_TOGGLE_GEORAM)));
                submenu->AddItem(extsubmenu = new BMenu("GEORAM size"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("64 kB", new BMessage(MENU_GEORAM_SIZE_64)));
                    extsubmenu->AddItem(new BMenuItem("128 kB", new BMessage(MENU_GEORAM_SIZE_128)));
                    extsubmenu->AddItem(new BMenuItem("256 kB", new BMessage(MENU_GEORAM_SIZE_256)));
                    extsubmenu->AddItem(new BMenuItem("512 kB", new BMessage(MENU_GEORAM_SIZE_512)));
                    extsubmenu->AddItem(new BMenuItem("1024 kB", new BMessage(MENU_GEORAM_SIZE_1024)));
                    extsubmenu->AddItem(new BMenuItem("2048 kB", new BMessage(MENU_GEORAM_SIZE_2048)));
                    extsubmenu->AddItem(new BMenuItem("4096 kB", new BMessage(MENU_GEORAM_SIZE_4096)));
                submenu->AddItem(new BMenuItem("GEORAM File", new BMessage(MENU_GEORAM_FILE)));
            menu->AddItem(submenu = new BMenu("RAMCART Options"));
                submenu->AddItem(new BMenuItem("RAMCART emulation", new BMessage(MENU_TOGGLE_RAMCART)));
                submenu->AddItem(extsubmenu = new BMenu("RAMCART size"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("64 kB", new BMessage(MENU_RAMCART_SIZE_64)));
                    extsubmenu->AddItem(new BMenuItem("128 kB", new BMessage(MENU_RAMCART_SIZE_128)));
                submenu->AddItem(new BMenuItem("RAMCART File", new BMessage(MENU_RAMCART_FILE)));
            menu->AddItem(submenu = new BMenu("Magic Voice Options"));
                submenu->AddItem(new BMenuItem("Magic Voice emulation", new BMessage(MENU_TOGGLE_MAGICVOICE)));
                submenu->AddItem(new BMenuItem("Magic Voice File", new BMessage(MENU_MAGICVOICE_FILE)));
            menu->AddItem(submenu = new BMenu("Digimax Options"));
                submenu->AddItem(new BMenuItem("Digimax emulation", new BMessage(MENU_TOGGLE_DIGIMAX)));
                submenu->AddItem(extsubmenu = new BMenu("Digimax base"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("Userport Interface", new BMessage(MENU_DIGIMAX_BASE_DD00)));
                    extsubmenu->AddItem(new BMenuItem("$DE00", new BMessage(MENU_DIGIMAX_BASE_DE00)));
                    extsubmenu->AddItem(new BMenuItem("$DE20", new BMessage(MENU_DIGIMAX_BASE_DE20)));
                    extsubmenu->AddItem(new BMenuItem("$DE40", new BMessage(MENU_DIGIMAX_BASE_DE40)));
                    extsubmenu->AddItem(new BMenuItem("$DE60", new BMessage(MENU_DIGIMAX_BASE_DE60)));
                    extsubmenu->AddItem(new BMenuItem("$DE80", new BMessage(MENU_DIGIMAX_BASE_DE80)));
                    extsubmenu->AddItem(new BMenuItem("$DEA0", new BMessage(MENU_DIGIMAX_BASE_DEA0)));
                    extsubmenu->AddItem(new BMenuItem("$DEC0", new BMessage(MENU_DIGIMAX_BASE_DEC0)));
                    extsubmenu->AddItem(new BMenuItem("$DEE0", new BMessage(MENU_DIGIMAX_BASE_DEE0)));
                    extsubmenu->AddItem(new BMenuItem("$DF00", new BMessage(MENU_DIGIMAX_BASE_DF00)));
                    extsubmenu->AddItem(new BMenuItem("$DF20", new BMessage(MENU_DIGIMAX_BASE_DF20)));
                    extsubmenu->AddItem(new BMenuItem("$DF40", new BMessage(MENU_DIGIMAX_BASE_DF40)));
                    extsubmenu->AddItem(new BMenuItem("$DF60", new BMessage(MENU_DIGIMAX_BASE_DF60)));
                    extsubmenu->AddItem(new BMenuItem("$DF80", new BMessage(MENU_DIGIMAX_BASE_DF80)));
                    extsubmenu->AddItem(new BMenuItem("$DFA0", new BMessage(MENU_DIGIMAX_BASE_DFA0)));
                    extsubmenu->AddItem(new BMenuItem("$DFC0", new BMessage(MENU_DIGIMAX_BASE_DFC0)));
                    extsubmenu->AddItem(new BMenuItem("$DFE0", new BMessage(MENU_DIGIMAX_BASE_DFE0)));
            menu->AddItem(submenu = new BMenu("SFX Sound Expander Options"));
                submenu->AddItem(new BMenuItem("SFX Sound Expander emulation", new BMessage(MENU_TOGGLE_SFX_SE)));
                submenu->AddItem(extsubmenu = new BMenu("SFX Sound Expander YM chip"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("3526", new BMessage(MENU_SFX_SE_3526)));
                    extsubmenu->AddItem(new BMenuItem("3812", new BMessage(MENU_SFX_SE_3526)));
            menu->AddItem(new BMenuItem("SFX Sound Sampler emulation", new BMessage(MENU_TOGGLE_SFX_SS)));
            menu->AddItem(submenu = new BMenu("EasyFlash Options"));
                submenu->AddItem(new BMenuItem("Jumper", new BMessage(MENU_TOGGLE_EASYFLASH_JUMPER)));
                submenu->AddItem(new BMenuItem("Save to .crt file on detach", new BMessage(MENU_TOGGLE_EASYFLASH_AUTOSAVE)));
                submenu->AddItem(new BMenuItem("Save .crt file now", new BMessage(MENU_EASYFLASH_SAVE_NOW)));
    }

    if (machine_class == VICE_MACHINE_C64 && !vsid_mode) {
            menu->AddItem(submenu = new BMenu("Double Quick Brown Box Options"));
                submenu->AddItem(new BMenuItem("DQBB emulation", new BMessage(MENU_TOGGLE_DQBB)));
                submenu->AddItem(new BMenuItem("DQBB File", new BMessage(MENU_DQBB_FILE)));
            menu->AddItem(submenu = new BMenu("Isepic Options"));
                submenu->AddItem(new BMenuItem("Isepic emulation", new BMessage(MENU_TOGGLE_ISEPIC)));
                submenu->AddItem(new BMenuItem("Isepic switch", new BMessage(MENU_TOGGLE_ISEPIC_SWITCH)));
            menu->AddItem(submenu = new BMenu("PLUS60K Options"));
                submenu->AddItem(new BMenuItem("PLUS60K emulation", new BMessage(MENU_TOGGLE_PLUS60K)));
                submenu->AddItem(extsubmenu = new BMenu("PLUS60K base"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("$D040", new BMessage(MENU_PLUS60K_BASE_D040)));
                    extsubmenu->AddItem(new BMenuItem("$D100", new BMessage(MENU_PLUS60K_BASE_D100)));
                submenu->AddItem(new BMenuItem("PLUS60K File", new BMessage(MENU_PLUS60K_FILE)));
            menu->AddItem(submenu = new BMenu("PLUS256K Options"));
                submenu->AddItem(new BMenuItem("PLUS256K emulation", new BMessage(MENU_TOGGLE_PLUS256K)));
                submenu->AddItem(new BMenuItem("PLUS256K File", new BMessage(MENU_PLUS256K_FILE)));
            menu->AddItem(submenu = new BMenu("C64_256K Options"));
                submenu->AddItem(new BMenuItem("C64_256K emulation", new BMessage(MENU_TOGGLE_C64_256K)));
                submenu->AddItem(extsubmenu = new BMenu("C64_256K base"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("$DE00-$DE7F", new BMessage(MENU_C64_256K_BASE_DE00)));
                    extsubmenu->AddItem(new BMenuItem("$DE80-$DEFF", new BMessage(MENU_C64_256K_BASE_DE80)));
                    extsubmenu->AddItem(new BMenuItem("$DF00-$DF7F", new BMessage(MENU_C64_256K_BASE_DF00)));
                    extsubmenu->AddItem(new BMenuItem("$DF80-$DFFF", new BMessage(MENU_C64_256K_BASE_DF80)));
                submenu->AddItem(new BMenuItem("C64_256K File", new BMessage(MENU_C64_256K_FILE)));
            menu->AddItem(submenu = new BMenu("MMC64 Options"));
                submenu->AddItem(new BMenuItem("MMC64 emulation", new BMessage(MENU_TOGGLE_MMC64)));
                submenu->AddItem(extsubmenu = new BMenu("MMC64 revision"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("Revision A", new BMessage(MENU_MMC64_REVISION_A)));
                    extsubmenu->AddItem(new BMenuItem("Revision B", new BMessage(MENU_MMC64_REVISION_B)));
                submenu->AddItem(new BMenuItem("MMC64 BIOS jumper", new BMessage(MENU_TOGGLE_MMC64_FLASHJUMPER)));
                submenu->AddItem(new BMenuItem("MMC64 BIOS save when changed", new BMessage(MENU_TOGGLE_MMC64_SAVE)));
                submenu->AddItem(new BMenuItem("MMC64 BIOS File", new BMessage(MENU_MMC64_BIOS_FILE)));
                submenu->AddItem(extsubmenu = new BMenu("MMC64 card type"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("Auto", new BMessage(MENU_MMC64_CARD_TYPE_AUTO)));
                    extsubmenu->AddItem(new BMenuItem("MMC", new BMessage(MENU_MMC64_CARD_TYPE_MMC)));
                    extsubmenu->AddItem(new BMenuItem("SD", new BMessage(MENU_MMC64_CARD_TYPE_SD)));
                    extsubmenu->AddItem(new BMenuItem("SDHC", new BMessage(MENU_MMC64_CARD_TYPE_SDHC)));
                submenu->AddItem(new BMenuItem("MMC64 Image read-only", new BMessage(MENU_TOGGLE_MMC64_READ_ONLY)));
                submenu->AddItem(new BMenuItem("MMC64 Image File", new BMessage(MENU_MMC64_IMAGE_FILE)));
            menu->AddItem(submenu = new BMenu("MMC Replay Options"));
                submenu->AddItem(new BMenuItem("EEPROM read/write", new BMessage(MENU_TOGGLE_MMCR_EEPROM_READ_WRITE)));
                submenu->AddItem(new BMenuItem("EEPROM File", new BMessage(MENU_MMCR_EEPROM_FILE)));
                submenu->AddItem(new BMenuItem("Rescue mode", new BMessage(MENU_TOGGLE_MMCR_RESCUE_MODE)));
                submenu->AddItem(extsubmenu = new BMenu("MMC Replay card type"));
                    extsubmenu->SetRadioMode(true);
                    extsubmenu->AddItem(new BMenuItem("Auto", new BMessage(MENU_MMCR_CARD_TYPE_AUTO)));
                    extsubmenu->AddItem(new BMenuItem("MMC", new BMessage(MENU_MMCR_CARD_TYPE_MMC)));
                    extsubmenu->AddItem(new BMenuItem("SD", new BMessage(MENU_MMCR_CARD_TYPE_SD)));
                    extsubmenu->AddItem(new BMenuItem("SDHC", new BMessage(MENU_MMCR_CARD_TYPE_SDHC)));
                submenu->AddItem(new BMenuItem("MMC Replay Image read/write", new BMessage(MENU_TOGGLE_MMCR_READ_WRITE)));
                submenu->AddItem(new BMenuItem("MMC Replay Image File", new BMessage(MENU_MMCR_IMAGE_FILE)));
    }

    if ((machine_class == VICE_MACHINE_VIC20 ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        menu->AddItem(new BMenuItem("IEEE488 Interface", new BMessage(MENU_TOGGLE_IEEE488)));
    }

    /* create the SETTINGS menu */
    uppermenu = new BMenu("Settings");
    menubar->AddItem(uppermenu);

    if ((machine_class == VICE_MACHINE_CBM5x0 ||
         machine_class == VICE_MACHINE_CBM6x0) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("CBM 2 ...", new BMessage(MENU_CBM2_SETTINGS)));
    }

    if (machine_class == VICE_MACHINE_PET && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("PET ...", new BMessage(MENU_PET_SETTINGS)));
        uppermenu->AddItem(menu = new BMenu("PET REU Options"));
            menu->AddItem(new BMenuItem("PET REU emulation", new BMessage(MENU_TOGGLE_PETREU)));
            menu->AddItem(submenu = new BMenu("PET REU size"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("128 kB", new BMessage(MENU_PETREU_SIZE_128)));
                submenu->AddItem(new BMenuItem("512 kB", new BMessage(MENU_PETREU_SIZE_512)));
                submenu->AddItem(new BMenuItem("1024 kB", new BMessage(MENU_PETREU_SIZE_1024)));
                submenu->AddItem(new BMenuItem("2048 kB", new BMessage(MENU_PETREU_SIZE_2048)));
            menu->AddItem(new BMenuItem("PET REU File", new BMessage(MENU_PETREU_FILE)));
        uppermenu->AddItem(menu = new BMenu("PET DWW Options"));
            menu->AddItem(new BMenuItem("PET DWW emulation", new BMessage(MENU_TOGGLE_PETDWW)));
            menu->AddItem(new BMenuItem("PET DWW File", new BMessage(MENU_PETDWW_FILE)));
        uppermenu->AddItem(new BMenuItem("PET Userport DAC emulation", new BMessage(MENU_TOGGLE_PET_USERPORT_DAC)));
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("VIC20 ...", new BMessage(MENU_VIC20_SETTINGS)));
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Final Expansion Options"));
            menu->AddItem(new BMenuItem("Write back to cart image", new BMessage(MENU_TOGGLE_FE_WRITE_BACK)));
        uppermenu->AddItem(menu = new BMenu("Mega-Cart Options"));
            menu->AddItem(new BMenuItem("Write back to nvram file", new BMessage(MENU_TOGGLE_MC_NVRAM_WRITE_BACK)));
            menu->AddItem(new BMenuItem("Select nvram file", new BMessage(MENU_MC_NVRAM_FILE)));
    }

    if ((machine_class == VICE_MACHINE_VIC20 ||
         machine_class == VICE_MACHINE_PLUS4 ||
         machine_class == VICE_MACHINE_PET) && !vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("SIDCART Options"));
            menu->AddItem(new BMenuItem("SIDCART emulation", new BMessage(MENU_TOGGLE_SIDCART)));
            menu->AddItem(submenu = new BMenu("SIDCART model"));
                submenu->SetRadioMode(true);
                submenu->AddItem(new BMenuItem("6581", new BMessage(MENU_SIDCART_MODEL_6581)));
                submenu->AddItem(new BMenuItem("8580", new BMessage(MENU_SIDCART_MODEL_8580)));
            menu->AddItem(new BMenuItem("SIDCART filters", new BMessage(MENU_TOGGLE_SIDCART_FILTERS)));
            menu->AddItem(submenu = new BMenu("SIDCART address"));
                submenu->SetRadioMode(true);
    }

    if (machine_class == VICE_MACHINE_PET && !vsid_mode) {
                submenu->AddItem(new BMenuItem("$8F00", new BMessage(MENU_SIDCART_ADDRESS_1)));
                submenu->AddItem(new BMenuItem("$E900", new BMessage(MENU_SIDCART_ADDRESS_2)));
    }

    if (machine_class == VICE_MACHINE_PLUS4 && !vsid_mode) {
                submenu->AddItem(new BMenuItem("$FD40", new BMessage(MENU_SIDCART_ADDRESS_1)));
                submenu->AddItem(new BMenuItem("$FE80", new BMessage(MENU_SIDCART_ADDRESS_2)));
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
                submenu->AddItem(new BMenuItem("$9800", new BMessage(MENU_SIDCART_ADDRESS_1)));
                submenu->AddItem(new BMenuItem("$9C00", new BMessage(MENU_SIDCART_ADDRESS_2)));
    }

    if ((machine_class == VICE_MACHINE_VIC20 ||
         machine_class == VICE_MACHINE_PLUS4 ||
         machine_class == VICE_MACHINE_PET) && !vsid_mode) {
        menu->AddItem(submenu = new BMenu("SIDCART clock"));
            submenu->SetRadioMode(true);
            submenu->AddItem(new BMenuItem("C64", new BMessage(MENU_SIDCART_CLOCK_C64)));
    }

    if (machine_class == VICE_MACHINE_PET&& !vsid_mode) {
            submenu->AddItem(new BMenuItem("PET", new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
    }

    if (machine_class == VICE_MACHINE_PLUS4 && !vsid_mode) {
            submenu->AddItem(new BMenuItem("PLUS4", new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
    }

    if (machine_class == VICE_MACHINE_VIC20 && !vsid_mode) {
            submenu->AddItem(new BMenuItem("PLUS4", new BMessage(MENU_SIDCART_CLOCK_NATIVE)));
    }

    if (!vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Video ...", new BMessage(MENU_VIDEO_SETTINGS)));
        uppermenu->AddItem(new BMenuItem("Device ...", new BMessage(MENU_DEVICE_SETTINGS)));
        uppermenu->AddItem(new BMenuItem("Drive ...", new BMessage(MENU_DRIVE_SETTINGS)));
    }

    if (machine_class != VICE_MACHINE_C64DTV && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("Datasette ...", new BMessage(MENU_DATASETTE_SETTINGS)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64DTV ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("VIC-II ...", new BMessage(MENU_VICII_SETTINGS)));
    }

    if (!vsid_mode) {
        uppermenu->AddItem(menu = new BMenu("Joystick..."));
            menu->AddItem(new BMenuItem("Joystick/Keyset settings...", new BMessage(MENU_JOYSTICK_SETTINGS)));
            menu->AddItem(new BMenuItem("Allow opposite joystick directions", new BMessage(MENU_ALLOW_OPPOSITE_JOY)));
    }

    if (machine_class == VICE_MACHINE_PLUS4 && !vsid_mode) {
            menu->AddItem(new BMenuItem("SIDcart joystick emulation", new BMessage(MENU_TOGGLE_USERPORT_JOY)));
    }

    if (machine_class != VICE_MACHINE_CBM5x0 && machine_class != VICE_MACHINE_PLUS4 && !vsid_mode) {
            menu->AddItem(new BMenuItem("Userport joystick emulation", new BMessage(MENU_TOGGLE_USERPORT_JOY)));
            menu->AddItem(submenu = new BMenu("Userport joystick type"));
                submenu->AddItem(new BMenuItem("CGA", new BMessage(MENU_USERPORT_JOY_CGA)));
                submenu->AddItem(new BMenuItem("PET", new BMessage(MENU_USERPORT_JOY_PET)));
                submenu->AddItem(new BMenuItem("Hummer", new BMessage(MENU_USERPORT_JOY_HUMMER)));
                submenu->AddItem(new BMenuItem("OEM", new BMessage(MENU_USERPORT_JOY_OEM)));
    }

    if ((machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C128) && !vsid_mode) {
                submenu->AddItem(new BMenuItem("HIT", new BMessage(MENU_USERPORT_JOY_HIT)));
    }
        
    uppermenu->AddItem(new BMenuItem("Sound ...", new BMessage(MENU_SOUND_SETTINGS)));

    if (machine_class == VICE_MACHINE_C64 ||
        machine_class == VICE_MACHINE_C128 ||
        machine_class == VICE_MACHINE_CBM5x0 ||
        machine_class == VICE_MACHINE_CBM6x0) {
        uppermenu->AddItem(new BMenuItem("SID ...", new BMessage(MENU_SID_SETTINGS)));
    }

    if (machine_class == VICE_MACHINE_C64DTV) {
        uppermenu->AddItem(new BMenuItem("SID ...", new BMessage(MENU_SIDDTV_SETTINGS)));
    }

    uppermenu->AddItem(new BMenuItem("RAM ...", new BMessage(MENU_RAM_SETTINGS)));

    if (machine_class == VICE_MACHINE_C128 && !vsid_mode) {
        uppermenu->AddItem(new BMenuItem("RAM banks 2 & 3", new BMessage(MENU_TOGGLE_C128FULLBANKS)));
    }

    if (!vsid_mode) {
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(new BMenuItem("Load Settings", new BMessage(MENU_SETTINGS_LOAD)));
        uppermenu->AddItem(new BMenuItem("Save Settings", new BMessage(MENU_SETTINGS_SAVE)));
        uppermenu->AddItem(new BMenuItem("Default Settings", new BMessage(MENU_SETTINGS_DEFAULT)));
        uppermenu->AddSeparatorItem();
        uppermenu->AddItem(new BMenuItem("Save settings on exit", new BMessage(MENU_TOGGLE_SAVE_SETTINGS_ON_EXIT)));
        uppermenu->AddItem(new BMenuItem("Confirm on exit", new BMessage(MENU_TOGGLE_CONFIRM_ON_EXIT)));
    }

    /* create the HELP menu */
    uppermenu = new BMenu("Help");
    menubar->AddItem(uppermenu);

    uppermenu->AddItem(new BMenuItem("About BeVICE...", new BMessage(MENU_ABOUT)));
    uppermenu->AddSeparatorItem();
    uppermenu->AddItem(new BMenuItem("Commandline Options", new BMessage(MENU_CMDLINE)));
    uppermenu->AddSeparatorItem();
    uppermenu->AddItem(new BMenuItem("Contributors", new BMessage(MENU_CONTRIBUTORS)));
    uppermenu->AddItem(new BMenuItem("License", new BMessage(MENU_LICENSE)));
    uppermenu->AddItem(new BMenuItem("No Warranty", new BMessage(MENU_WARRANTY)));

    return menubar;
}
