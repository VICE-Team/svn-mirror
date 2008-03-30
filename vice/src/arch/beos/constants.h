/*
 * constants.h - BeOS constans for menus, messages, ...
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

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#ifndef uint32
#define uint32 unsigned int
#endif

// Messages for window interaction with application

const uint32 WINDOW_CLOSED						= 'WRcl';
const uint32 MESSAGE_ATTACH_READONLY            = 'MARO';
const uint32 MESSAGE_SET_RESOURCE	            = 'MASR';

// Messages for menu commands

const uint32 MENU_AUTOSTART						= 'M001';
const uint32 MENU_ATTACH_DISK8					= 'M002';
const uint32 MENU_ATTACH_DISK9					= 'M003';
const uint32 MENU_ATTACH_DISK10					= 'M004';
const uint32 MENU_ATTACH_DISK11					= 'M005';
const uint32 MENU_DETACH_DISK8					= 'M006';
const uint32 MENU_DETACH_DISK9					= 'M007';
const uint32 MENU_DETACH_DISK10					= 'M008';
const uint32 MENU_DETACH_DISK11					= 'M009';
const uint32 MENU_ATTACH_TAPE					= 'M010';
const uint32 MENU_DETACH_TAPE					= 'M011';
const uint32 MENU_DATASETTE_START				= 'M013';
const uint32 MENU_DATASETTE_STOP				= 'M014';
const uint32 MENU_DATASETTE_REWIND				= 'M015';
const uint32 MENU_DATASETTE_FORWARD				= 'M016';
const uint32 MENU_DATASETTE_RECORD				= 'M017';
const uint32 MENU_DATASETTE_RESET				= 'M018';
const uint32 MENU_DATASETTE_COUNTER				= 'M019';
const uint32 MENU_RESET_SOFT					= 'M020';
const uint32 MENU_RESET_HARD					= 'M021';
const uint32 MENU_EXIT_REQUESTED				= 'M022';
const uint32 MENU_FLIP_ADD						= 'M024';
const uint32 MENU_FLIP_REMOVE					= 'M025';
const uint32 MENU_FLIP_NEXT						= 'M026';
const uint32 MENU_FLIP_PREVIOUS					= 'M027';
const uint32 MENU_SNAPSHOT_LOAD					= 'M028';
const uint32 MENU_SNAPSHOT_SAVE					= 'M029';
const uint32 MENU_LOADQUICK						= 'M030';
const uint32 MENU_SAVEQUICK						= 'M031';
const uint32 MENU_MONITOR						= 'M032';
const uint32 MENU_PAUSE							= 'M033';
const uint32 MENU_CART_ATTACH_CRT				= 'M040';
const uint32 MENU_CART_ATTACH_8KB				= 'M041';
const uint32 MENU_CART_ATTACH_16KB				= 'M042';
const uint32 MENU_CART_ATTACH_AR				= 'M043';
const uint32 MENU_CART_ATTACH_AT				= 'M044';
const uint32 MENU_CART_ATTACH_EPYX				= 'M045';
const uint32 MENU_CART_ATTACH_IEEE488			= 'M046';
const uint32 MENU_CART_ATTACH_SS4				= 'M047';
const uint32 MENU_CART_ATTACH_SS5				= 'M048';
const uint32 MENU_CART_ATTACH_IDE64				= 'M049';
const uint32 MENU_CART_SET_DEFAULT				= 'M050';
const uint32 MENU_CART_DETACH					= 'M051';
const uint32 MENU_CART_FREEZE					= 'M052';

const uint32 MENU_CART_VIC20_16KB_2000			= 'M060';
const uint32 MENU_CART_VIC20_16KB_4000			= 'M061';
const uint32 MENU_CART_VIC20_16KB_6000			= 'M062';
const uint32 MENU_CART_VIC20_8KB_A000			= 'M063';
const uint32 MENU_CART_VIC20_4KB_B000			= 'M064';

const uint32 ATTACH_C64_CART					= 'MA01';
const uint32 ATTACH_VIC20_CART					= 'MA02';

const uint32 MENU_TOGGLE_SOUND					= 'M100';
const uint32 MENU_TOGGLE_WARP_MODE				= 'M101';
const uint32 MENU_TOGGLE_DRIVE_TRUE_EMULATION	= 'M102';
const uint32 MENU_TOGGLE_VIRTUAL_DEVICES		= 'M103';
const uint32 MENU_TOGGLE_EMUID					= 'M104';
const uint32 MENU_TOGGLE_DOUBLESIZE				= 'M106';
const uint32 MENU_TOGGLE_DOUBLESCAN				= 'M107';
const uint32 MENU_TOGGLE_VIDEOCACHE				= 'M108';
const uint32 MENU_TOGGLE_REU					= 'M109';
const uint32 MENU_TOGGLE_MOUSE					= 'M110';
const uint32 MENU_TOGGLE_DIRECTWINDOW			= 'M111';
const uint32 MENU_TOGGLE_IEEE488				= 'M112';
const uint32 MENU_TOGGLE_VDC64KB				= 'M113';
const uint32 MENU_TOGGLE_VDC_DOUBLESIZE			= 'M114';
const uint32 MENU_TOGGLE_VDC_DOUBLESCAN			= 'M115';
const uint32 MENU_TOGGLE_SAVE_SETTINGS_ON_EXIT	= 'M116';
const uint32 MENU_TOGGLE_CONFIRM_ON_EXIT		= 'M117';
const uint32 MENU_TOGGLE_FASTPAL				= 'M118';
const uint32 MENU_TOGGLE_SCALE2X				= 'M119';

const uint32 MENU_REFRESH_RATE_AUTO				= 'M120';
const uint32 MENU_REFRESH_RATE_1				= 'M121';
const uint32 MENU_REFRESH_RATE_2				= 'M122';
const uint32 MENU_REFRESH_RATE_3				= 'M123';
const uint32 MENU_REFRESH_RATE_4				= 'M124';
const uint32 MENU_REFRESH_RATE_5				= 'M125';
const uint32 MENU_REFRESH_RATE_6				= 'M126';
const uint32 MENU_REFRESH_RATE_7				= 'M127';
const uint32 MENU_REFRESH_RATE_8				= 'M128';
const uint32 MENU_REFRESH_RATE_9				= 'M129';
const uint32 MENU_REFRESH_RATE_10				= 'M130';
const uint32 MENU_MAXIMUM_SPEED_NO_LIMIT		= 'M131';
const uint32 MENU_MAXIMUM_SPEED_10				= 'M132';
const uint32 MENU_MAXIMUM_SPEED_20				= 'M133';
const uint32 MENU_MAXIMUM_SPEED_50				= 'M134';
const uint32 MENU_MAXIMUM_SPEED_100				= 'M135';
const uint32 MENU_MAXIMUM_SPEED_200				= 'M136';

const uint32 MENU_SYNC_FACTOR_PAL				= 'M140';
const uint32 MENU_SYNC_FACTOR_NTSC				= 'M141';
const uint32 MENU_SYNC_FACTOR_NTSCOLD			= 'M142';

const uint32 MENU_DRIVE_SETTINGS				= 'M200';
const uint32 MENU_DEVICE_SETTINGS				= 'M201';
const uint32 MENU_SOUND_SETTINGS				= 'M202';
const uint32 MENU_JOYSTICK_SETTINGS				= 'M203';
const uint32 MENU_DATASETTE_SETTINGS			= 'M204';
const uint32 MENU_CBM2_SETTINGS					= 'M205';
const uint32 MENU_PET_SETTINGS					= 'M206';
const uint32 MENU_VICII_SETTINGS				= 'M207';
const uint32 MENU_VIC20_SETTINGS				= 'M208';
const uint32 MENU_VIDEO_SETTINGS				= 'M209';
const uint32 MENU_SID_SETTINGS					= 'M210';
const uint32 MENU_RAM_SETTINGS					= 'M211';

const uint32 MENU_SETTINGS_LOAD					= 'M220';
const uint32 MENU_SETTINGS_SAVE					= 'M221';
const uint32 MENU_SETTINGS_DEFAULT				= 'M222';

const uint32 MENU_VSID_LOAD						= 'M300';
const uint32 PLAY_VSID							= 'M301';
const uint32 MENU_VSID_TUNE						= 'M302';

const uint32 MENU_REU_SIZE_128					= 'M400';
const uint32 MENU_REU_SIZE_256					= 'M401';
const uint32 MENU_REU_SIZE_512					= 'M402';
const uint32 MENU_REU_SIZE_1024					= 'M403';
const uint32 MENU_REU_SIZE_2048					= 'M404';
const uint32 MENU_REU_SIZE_4096					= 'M405';
const uint32 MENU_REU_SIZE_8192					= 'M406';
const uint32 MENU_REU_SIZE_16384				= 'M407';

const uint32 MENU_ABOUT							= 'M901';
const uint32 MENU_CONTRIBUTORS					= 'M902';
const uint32 MENU_LICENSE						= 'M904';
const uint32 MENU_WARRANTY						= 'M905';
const uint32 MENU_CMDLINE						= 'M906';

#endif
