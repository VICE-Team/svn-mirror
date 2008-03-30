/*
 * cbm2uires.h
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#ifndef _CBM2UIRES_H_
#define _CBM2UIRES_H_

#include "uires.h"

static struct NewMenu UI_MENU_NAME[] = {
  TITLE("File", NULL)
    ITEM("Autostart disk/tape image...", NULL, IDM_AUTOSTART)
    ITEMSEPARATOR()
    ITEM("Attach disk image", NULL, NULL)
      SUB("Drive 8", "8", IDM_ATTACH_8)
      SUB("Drive 9", "9",  IDM_ATTACH_9)
      SUB("Drive 10", "0", IDM_ATTACH_10)
      SUB("Drive 11", "1", IDM_ATTACH_11)
    ITEM("Detach disk image", NULL, NULL)
      SUB("Drive 8", NULL, IDM_DETACH_8)
      SUB("Drive 9", NULL, IDM_DETACH_9)
      SUB("Drive 10", NULL, IDM_DETACH_10)
      SUB("Drive 11", NULL, IDM_DETACH_11)
      SUBSEPARATOR()
      SUB("All", "A", IDM_DETACH_ALL)
    ITEM("Flip list", NULL, NULL)
      SUB("Add current image", "I",     IDM_FLIP_ADD)
      SUB("Remove current image", "K",  IDM_FLIP_REMOVE)
      SUB("Attach next image", "N",     IDM_FLIP_NEXT)
      SUB("Attach previous image", "B", IDM_FLIP_PREVIOUS)
      SUBSEPARATOR()
      SUB("Load flip list", NULL, IDM_FLIP_LOAD)
      SUB("Save flip list", NULL, IDM_FLIP_SAVE)
    ITEMSEPARATOR()
    ITEM("Attach tape image...", "T", IDM_ATTACH_TAPE)
    ITEM("Detach tape image", NULL,           IDM_DETACH_TAPE)
    ITEM("Datassette control", NULL, NULL)
      SUB("Stop", NULL,          IDM_DATASETTE_CONTROL_STOP)
      SUB("Start", NULL,         IDM_DATASETTE_CONTROL_START)
      SUB("Forward", NULL,       IDM_DATASETTE_CONTROL_FORWARD)
      SUB("Rewind", NULL,        IDM_DATASETTE_CONTROL_REWIND)
      SUB("Record", NULL,        IDM_DATASETTE_CONTROL_RECORD)
      SUB("Reset", NULL,         IDM_DATASETTE_CONTROL_RESET)
      SUB("Reset Counter", NULL, IDM_DATASETTE_RESET_COUNTER)
    ITEMSEPARATOR()
#if defined(UI_VIC20)
    ITEM("Attach cartridge image...", NULL, NULL)
      SUB("4/8/16KB image at $2000...", NULL, IDM_CART_VIC20_8KB_2000)
      SUB("4/8/16KB image at $4000...", NULL, IDM_CART_VIC20_16KB_4000)
      SUB("4/8/16KB image at $6000...", NULL, IDM_CART_VIC20_8KB_6000)
      SUB("4/8KB image at $A000...",    NULL, IDM_CART_VIC20_8KB_A000)
      SUB("4KB image at $B000...",      NULL, IDM_CART_VIC20_4KB_B000)
    ITEM("Detach cartridge image", NULL, IDM_CART_DETACH)
    ITEMSEPARATOR()
#endif
#if defined(UI_PLUS4)
    ITEM("Attach cartridge image...", NULL, NULL)
      SUB("C1 low  image...", NULL, IDM_CART_ATTACH_C1LO)
      SUB("C1 high image...", NULL, IDM_CART_ATTACH_C1HI)
      SUB("C2 low  image...", NULL, IDM_CART_ATTACH_C2LO)
      SUB("C2 high image...", NULL, IDM_CART_ATTACH_C2HI)
      SUBSEPARATOR()
      SUB("Function low  image (3plus1)...", NULL, IDM_CART_ATTACH_FUNCLO)
      SUB("Function high image (3plus1)...", NULL, IDM_CART_ATTACH_FUNCHI)
    ITEM("Detach cartridge image", NULL, IDM_CART_DETACH)
    ITEMSEPARATOR()
#endif
#if !defined(UI_CBM2) && !defined(UI_PET) && !defined(UI_PLUS4) && !defined(UI_VIC20)
    ITEM("Attach cartridge image...", NULL, NULL)
      SUB("CRT image...", NULL,               IDM_CART_ATTACH_CRT)
      SUBSEPARATOR()
      SUB("Generic 8KB image...", NULL,       IDM_CART_ATTACH_8KB)
      SUB("Generic 16KB image...", NULL,      IDM_CART_ATTACH_16KB)
      SUB("Action Replay image...", NULL,     IDM_CART_ATTACH_AR)
      SUB("Atomic Power image...", NULL,      IDM_CART_ATTACH_AT)
      SUB("Epyx fastload image...", NULL,     IDM_CART_ATTACH_EPYX)
      SUB("IEEE488 interface image...", NULL, IDM_CART_ATTACH_IEEE488)
      SUB("Retro Replay image...", NULL,      IDM_CART_ATTACH_RR)
      SUB("IDE64 interface image...", NULL,   IDM_CART_ATTACH_IDE64)
      SUB("Super Snapshot 4 image...", NULL,  IDM_CART_ATTACH_SS4)
      SUB("Super Snapshot 5 image...", NULL,  IDM_CART_ATTACH_SS5)
/* AmigaOS only support one submenu level */
    ITEM("Expert Cartridge", NULL, NULL)
      SUB("Enable", NULL, IDM_CART_ENABLE_EXPERT)
      SUBSEPARATOR()
      SUBTOGGLE("Off", NULL,    IDM_CART_MODE_OFF)
      SUBTOGGLE("Prg", NULL,    IDM_CART_MODE_PRG)
      SUBTOGGLE("On", NULL,     IDM_CART_MODE_ON)
    ITEM("Set cartridge as default", NULL, IDM_CART_SET_DEFAULT)
    ITEMTOGGLE("Reset on cart change", NULL, IDM_TOGGLE_CART_RESET)
    ITEMSEPARATOR()
    ITEM("Detach cartridge image", NULL,  IDM_CART_DETACH)
    ITEM("Cartridge freeze", "Z", IDM_CART_FREEZE)
    ITEMSEPARATOR()
#endif
    ITEMTOGGLE("Pause", NULL, IDM_PAUSE)
    ITEM("Monitor", "M", IDM_MONITOR)
    ITEM("Reset", NULL, NULL)
      SUB("Hard", NULL, IDM_RESET_HARD)
      SUB("Soft", "R", IDM_RESET_SOFT)
      SUBSEPARATOR()
      SUB("Drive 8", NULL, IDM_RESET_DRIVE8)
      SUB("Drive 9", NULL, IDM_RESET_DRIVE9)
      SUB("Drive 10", NULL, IDM_RESET_DRIVE10)
      SUB("Drive 11", NULL, IDM_RESET_DRIVE11)
    ITEMSEPARATOR()
    ITEM("Exit", "X", IDM_EXIT)
  TITLE("Snapshot", NULL)
    ITEM("Load snapshot image...", NULL, IDM_SNAPSHOT_LOAD)
    ITEM("Save snapshot image...", NULL, IDM_SNAPSHOT_SAVE)
    ITEM("Load quicksnapshot image", "L", IDM_LOADQUICK)
    ITEM("Save quicksnapshot image", "S", IDM_SAVEQUICK)
    ITEMSEPARATOR()
    ITEM("Start/Stop Recording History", NULL,       IDM_EVENT_TOGGLE_RECORD)
    ITEM("Start/Stop Playback History", NULL,        IDM_EVENT_TOGGLE_PLAYBACK)
    ITEM("Set Recording Milestone", "G",  IDM_EVENT_SETMILESTONE)
    ITEM("Return to Milestone", "H",      IDM_EVENT_RESETMILESTONE)
    ITEM("Recording start mode", NULL, NULL)
      SUBTOGGLE("Save new snapshot", NULL,      IDM_EVENT_START_MODE_SAVE)
      SUBTOGGLE("Load existing snapshot", NULL, IDM_EVENT_START_MODE_LOAD)
      SUBTOGGLE("Start with reset", NULL,       IDM_EVENT_START_MODE_RESET)
      SUBTOGGLE("Overwrite Playback", NULL,       IDM_EVENT_START_MODE_PLAYBACK)
    ITEM("Select History files/directory", NULL,    IDM_EVENT_DIRECTORY)
    ITEMSEPARATOR()
    ITEM("Save/stop media file...", "C", IDM_MEDIAFILE)
  TITLE("Options", NULL)
    ITEM("Refresh rate", NULL, NULL)
      SUBTOGGLE("Auto", NULL, IDM_REFRESH_RATE_AUTO)
      SUBTOGGLE("1/1", NULL, IDM_REFRESH_RATE_1)
      SUBTOGGLE("1/2", NULL, IDM_REFRESH_RATE_2)
      SUBTOGGLE("1/3", NULL, IDM_REFRESH_RATE_3)
      SUBTOGGLE("1/4", NULL, IDM_REFRESH_RATE_4)
      SUBTOGGLE("1/5", NULL, IDM_REFRESH_RATE_5)
      SUBTOGGLE("1/6", NULL, IDM_REFRESH_RATE_6)
      SUBTOGGLE("1/7", NULL, IDM_REFRESH_RATE_7)
      SUBTOGGLE("1/8", NULL, IDM_REFRESH_RATE_8)
      SUBTOGGLE("1/9", NULL, IDM_REFRESH_RATE_9)
      SUBTOGGLE("1/10", NULL, IDM_REFRESH_RATE_10)
  ITEM("Maximum Speed", NULL, NULL)
      SUBTOGGLE("200%", NULL, IDM_MAXIMUM_SPEED_200)
      SUBTOGGLE("100%", NULL, IDM_MAXIMUM_SPEED_100)
      SUBTOGGLE("50%", NULL, IDM_MAXIMUM_SPEED_50)
      SUBTOGGLE("20%", NULL, IDM_MAXIMUM_SPEED_20)
      SUBTOGGLE("10%", NULL, IDM_MAXIMUM_SPEED_10)
      SUBTOGGLE("No limit", NULL, IDM_MAXIMUM_SPEED_NO_LIMIT)
      SUBSEPARATOR()
      SUBTOGGLE("Custom", NULL, IDM_MAXIMUM_SPEED_CUSTOM)
    ITEMTOGGLE("Warp mode", "W", IDM_TOGGLE_WARP_MODE)
    ITEMSEPARATOR()
#if !defined(UI_C128)
    ITEMTOGGLE("Fullscreen", "D", IDM_TOGGLE_FULLSCREEN)
    ITEMSEPARATOR()
#endif
    ITEMTOGGLE("Video cache", NULL, IDM_TOGGLE_VIDEOCACHE)
    ITEMTOGGLE("Double size", NULL, IDM_TOGGLE_DOUBLESIZE)
    ITEMTOGGLE("Double scan", NULL, IDM_TOGGLE_DOUBLESCAN)
#if !defined(UI_CBM2) && !defined(UI_PET)
    ITEMTOGGLE("PAL Emulation", NULL, IDM_TOGGLE_FASTPAL)
    ITEMTOGGLE("Scale2x", NULL, IDM_TOGGLE_SCALE2X)
#endif
#if defined(UI_C128)
    ITEM("VDC settings", NULL, NULL)
      SUBTOGGLE("Double size", NULL, IDM_TOGGLE_VDC_DOUBLESIZE)
      SUBTOGGLE("Double scan", NULL, IDM_TOGGLE_VDC_DOUBLESCAN)
      SUBSEPARATOR()
      SUBTOGGLE("64KB video memory", NULL, IDM_TOGGLE_VDC64KB)
#endif
    ITEMSEPARATOR()
    ITEM("Swap Joystick", "J", IDM_SWAP_JOYSTICK)
    ITEMSEPARATOR()
    ITEMTOGGLE("Sound playback", NULL, IDM_TOGGLE_SOUND)
    ITEMSEPARATOR()
    ITEMTOGGLE("True drive emulation", NULL, IDM_TOGGLE_DRIVE_TRUE_EMULATION)
    ITEMTOGGLE("Virtual device traps", NULL, IDM_TOGGLE_VIRTUAL_DEVICES)
#if defined(UI_CBM2) || defined(UI_PET)
    ITEM("Drive sync factor", NULL, NULL)
      SUBTOGGLE("PAL",  NULL, IDM_SYNC_FACTOR_PAL)
      SUBTOGGLE("NTSC", NULL, IDM_SYNC_FACTOR_NTSC)
#endif
    ITEMSEPARATOR()
#if !defined(UI_CBM2) && !defined(UI_PET)
    ITEM("Video standard", NULL, NULL)
      SUBTOGGLE("PAL-G", NULL, IDM_SYNC_FACTOR_PAL)
      SUBTOGGLE("NTSC-M", NULL, IDM_SYNC_FACTOR_NTSC)
#if !defined(UI_C128) && !defined(UI_PLUS4) && !defined(UI_VIC20)
      SUBTOGGLE("Old NTSC-M", NULL, IDM_SYNC_FACTOR_NTSCOLD)
#endif
#endif
#if !defined(UI_PLUS4)
    ITEMSEPARATOR()
    ITEMTOGGLE("Emulator Identification", NULL, IDM_TOGGLE_EMUID)
#if defined(UI_PLUS4)
	MENUITEM "VIC-1112 IEEE 488 module", IDM_IEEE488
#endif
#if defined(UI_C128)
    ITEMTOGGLE("IEEE 488 Interface emulation", NULL, IDM_IEEE488)
#endif
#if !defined(UI_CBM2) && !defined(UI_PET) && !defined(UI_VIC20)
    ITEMTOGGLE("1351 mouse emulation", "Q", IDM_MOUSE)
#endif
#endif
  TITLE("Settings", NULL)
    ITEM("Video settings...", NULL,     IDM_VIDEO_SETTINGS)
#if defined(UI_VIC20)
    ITEM("VIC settings...", NULL, IDM_VIC_SETTINGS)
#endif
#if defined(UI_CBM2)
    ITEM("CBM2 settings...", NULL, IDM_CBM2_SETTINGS)
#endif
#if defined(UI_PET)
    ITEM("PET settings...", NULL, IDM_PET_SETTINGS)
#endif
    ITEM("Peripheral settings...", NULL,IDM_DEVICEMANAGER)
    ITEM("Drive settings...", NULL,     IDM_DRIVE_SETTINGS)
    ITEM("Datasette settings...", NULL, IDM_DATASETTE_SETTINGS)
#if defined(UI_PLUS4)
    ITEM("Plus4 settings...", NULL, IDM_PLUS4_SETTINGS)
#endif
#if !defined(UI_PET) && !defined(UI_PLUS4) && !defined(UI_VIC20)
    ITEM("VIC-II settings...", NULL,    IDM_VICII_SETTINGS)
#endif
    ITEM("Joystick settings...", NULL,  IDM_JOY_SETTINGS)
    ITEM("Keyboard settings...", NULL,  IDM_KEYBOARD_SETTINGS)
    ITEM("Sound settings...", NULL,     IDM_SOUND_SETTINGS)
#if !defined(UI_PET) && !defined(UI_PLUS4) && !defined(UI_VIC20)
    ITEM("SID settings...", NULL,       IDM_SID_SETTINGS)
#endif
    ITEM("ROM settings...", NULL,       IDM_ROM_SETTINGS)
    ITEM("RAM settings...", NULL,       IDM_RAM_SETTINGS)
    ITEM("RS232 settings...", NULL,       IDM_RS232_SETTINGS)
#if defined(UI_C128)
    ITEM("C128 settings...", NULL,      IDM_C128_SETTINGS)
#endif
#if !defined(UI_CBM2) && !defined(UI_PET) && !defined(UI_PLUS4) && !defined(UI_VIC20)
    ITEM("Cartridge/IO settings", NULL, NULL)
      SUB("REU settings...", NULL,      IDM_REU_SETTINGS)
      SUB("IDE64 settings...", NULL,    IDM_IDE64_SETTINGS)
#ifdef HAVE_TFE
      SUB("Ethernet settings...", NULL, IDM_TFE_SETTINGS)
#endif
      SUB("ACIA settings...", NULL,    IDM_ACIA_SETTINGS)
      SUB("RS232 userport settings...", NULL,    IDM_RS232USER_SETTINGS)
#endif
    ITEMSEPARATOR()
    ITEM("Save current settings", NULL, IDM_SETTINGS_SAVE)
    ITEM("Load saved settings", NULL,   IDM_SETTINGS_LOAD)
    ITEM("Set default settings", NULL,  IDM_SETTINGS_DEFAULT)
    ITEMSEPARATOR()
    ITEMTOGGLE("Save settings on exit", NULL, IDM_TOGGLE_SAVE_SETTINGS_ON_EXIT)
    ITEMTOGGLE("Confirm on exit", NULL,       IDM_TOGGLE_CONFIRM_ON_EXIT)
  TITLE("Help", NULL)
    ITEM("About...", NULL,             IDM_ABOUT)
    ITEMSEPARATOR()
    ITEM("Command line options", NULL, IDM_CMDLINE)
    ITEMSEPARATOR()
    ITEM("Contributors", NULL,         IDM_CONTRIBUTORS)
    ITEM("License", NULL,              IDM_LICENSE)
    ITEM("No warranty", NULL,          IDM_WARRANTY)
  END()
};

#endif /* _CBM2UIRES_H_ */

