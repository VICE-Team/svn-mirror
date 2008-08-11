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
const uint32 MENU_CART_ATTACH_AR3				= 'M158';
const uint32 MENU_CART_ATTACH_AR4				= 'M159';
const uint32 MENU_CART_ATTACH_STARDOS			= 'M160';
const uint32 MENU_CART_ATTACH_AT				= 'M044';
const uint32 MENU_CART_ATTACH_EPYX				= 'M045';
const uint32 MENU_CART_ATTACH_IEEE488			= 'M046';
const uint32 MENU_CART_ATTACH_SS4				= 'M047';
const uint32 MENU_CART_ATTACH_SS5				= 'M048';
const uint32 MENU_CART_ATTACH_IDE64				= 'M049';
const uint32 MENU_CART_SET_DEFAULT				= 'M050';
const uint32 MENU_CART_DETACH					= 'M051';
const uint32 MENU_CART_FREEZE					= 'M052';
const uint32 MENU_NETPLAY_SERVER				= 'M053';
const uint32 MENU_NETPLAY_CLIENT				= 'M054';
const uint32 MENU_NETPLAY_DISCONNECT			= 'M055';
const uint32 MENU_NETPLAY_SETTINGS				= 'M056';

const uint32 MENU_EVENT_TOGGLE_RECORD			= 'M070';
const uint32 MENU_EVENT_TOGGLE_PLAYBACK			= 'M071';
const uint32 MENU_EVENT_SETMILESTONE			= 'M072';
const uint32 MENU_EVENT_RESETMILESTONE			= 'M073';
const uint32 MENU_EVENT_START_MODE_SAVE			= 'M074';
const uint32 MENU_EVENT_START_MODE_LOAD			= 'M075';
const uint32 MENU_EVENT_START_MODE_RESET		= 'M076';
const uint32 MENU_EVENT_START_MODE_PLAYBACK		= 'M077';
const uint32 MENU_EVENT_SNAPSHOT_START			= 'M078';
const uint32 MENU_EVENT_SNAPSHOT_END			= 'M079';

const uint32 MENU_CART_VIC20_16KB_2000			= 'M060';
const uint32 MENU_CART_VIC20_16KB_4000			= 'M061';
const uint32 MENU_CART_VIC20_16KB_6000			= 'M062';
const uint32 MENU_CART_VIC20_8KB_A000			= 'M063';
const uint32 MENU_CART_VIC20_4KB_B000			= 'M064';

const uint32 ATTACH_C64_CART					= 'MA01';
const uint32 ATTACH_VIC20_CART					= 'MA02';

const uint32 MENU_COPY						= 'M161';
const uint32 MENU_PASTE						= 'M162';

const uint32 MENU_TOGGLE_SOUND					= 'M100';
const uint32 MENU_TOGGLE_WARP_MODE				= 'M101';
const uint32 MENU_TOGGLE_DRIVE_TRUE_EMULATION	= 'M102';
const uint32 MENU_TOGGLE_VIRTUAL_DEVICES		= 'M103';
const uint32 MENU_TOGGLE_EMUID					= 'M104';
const uint32 MENU_TOGGLE_DOUBLESIZE				= 'M106';
const uint32 MENU_TOGGLE_DOUBLESCAN				= 'M107';
const uint32 MENU_TOGGLE_VIDEOCACHE				= 'M108';
const uint32 MENU_TOGGLE_REU					= 'M109';
const uint32 MENU_TOGGLE_GEORAM					= 'M143';
const uint32 MENU_TOGGLE_RAMCART					= 'M145';
const uint32 MENU_TOGGLE_PLUS60K					= 'M146';
const uint32 MENU_TOGGLE_PLUS256K					= 'M148';
const uint32 MENU_TOGGLE_C64_256K					= 'M144';
const uint32 MENU_TOGGLE_PETREU					= 'M147';
const uint32 MENU_TOGGLE_DIGIMAX					= 'M149';
const uint32 MENU_TOGGLE_MMC64					= 'M039';
const uint32 MENU_TOGGLE_MMC64_FLASHJUMPER			= 'M057';
const uint32 MENU_TOGGLE_MMC64_SAVE					= 'M068';
const uint32 MENU_TOGGLE_MMC64_READ_ONLY				= 'M058';
const uint32 MENU_TOGGLE_C64DTV_WRITE_ENABLE			= 'M099';
const uint32 MENU_TOGGLE_HUMMER_USERPORT_JOY			= 'M105';
const uint32 MENU_TOGGLE_SIDCART					= 'M085';
const uint32 MENU_TOGGLE_SIDCART_FILTERS				= 'M086';
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

const uint32 MENU_REU_FILE					= 'M012';

const uint32 MENU_GEORAM_SIZE_64					= 'M450';
const uint32 MENU_GEORAM_SIZE_128					= 'M451';
const uint32 MENU_GEORAM_SIZE_256					= 'M452';
const uint32 MENU_GEORAM_SIZE_512					= 'M453';
const uint32 MENU_GEORAM_SIZE_1024					= 'M454';
const uint32 MENU_GEORAM_SIZE_2048					= 'M455';
const uint32 MENU_GEORAM_SIZE_4096					= 'M456';

const uint32 MENU_GEORAM_FILE						= 'M023';

const uint32 MENU_RAMCART_SIZE_64					= 'M457';
const uint32 MENU_RAMCART_SIZE_128					= 'M458';

const uint32 MENU_RAMCART_FILE					= 'M034';

const uint32 MENU_PLUS60K_BASE_D040					= 'M459';
const uint32 MENU_PLUS60K_BASE_D100					= 'M460';

const uint32 MENU_PLUS60K_FILE					= 'M035';

const uint32 MENU_PLUS256K_FILE					= 'M036';

const uint32 MENU_C64_256K_BASE_DE00				= 'M459';
const uint32 MENU_C64_256K_BASE_DE80				= 'M460';
const uint32 MENU_C64_256K_BASE_DF00				= 'M461';
const uint32 MENU_C64_256K_BASE_DF80				= 'M462';

const uint32 MENU_C64_256K_FILE					= 'M037';

const uint32 MENU_PETREU_SIZE_128					= 'M463';
const uint32 MENU_PETREU_SIZE_512					= 'M464';
const uint32 MENU_PETREU_SIZE_1024					= 'M465';
const uint32 MENU_PETREU_SIZE_2048					= 'M466';

const uint32 MENU_PETREU_FILE						= 'M038';

const uint32 MENU_DIGIMAX_BASE_DD00					= 'M467';
const uint32 MENU_DIGIMAX_BASE_DE00					= 'M468';
const uint32 MENU_DIGIMAX_BASE_DE20					= 'M469';
const uint32 MENU_DIGIMAX_BASE_DE40					= 'M470';
const uint32 MENU_DIGIMAX_BASE_DE60					= 'M471';
const uint32 MENU_DIGIMAX_BASE_DE80					= 'M472';
const uint32 MENU_DIGIMAX_BASE_DEA0					= 'M473';
const uint32 MENU_DIGIMAX_BASE_DEC0					= 'M474';
const uint32 MENU_DIGIMAX_BASE_DEE0					= 'M475';
const uint32 MENU_DIGIMAX_BASE_DF00					= 'M476';
const uint32 MENU_DIGIMAX_BASE_DF20					= 'M477';
const uint32 MENU_DIGIMAX_BASE_DF40					= 'M478';
const uint32 MENU_DIGIMAX_BASE_DF60					= 'M479';
const uint32 MENU_DIGIMAX_BASE_DF80					= 'M480';
const uint32 MENU_DIGIMAX_BASE_DFA0					= 'M481';
const uint32 MENU_DIGIMAX_BASE_DFC0					= 'M482';
const uint32 MENU_DIGIMAX_BASE_DFE0					= 'M483';

const uint32 MENU_MMC64_REVISION_A					= 'M066';
const uint32 MENU_MMC64_REVISION_B					= 'M067';

const uint32 MENU_MMC64_BIOS_FILE					= 'M059';
const uint32 MENU_MMC64_IMAGE_FILE					= 'M065';

const uint32 MENU_C64DTV_REVISION_2					= 'M137';
const uint32 MENU_C64DTV_REVISION_3					= 'M138';

const uint32 MENU_HUMMER_JOY_PORT_1					= 'M139';
const uint32 MENU_HUMMER_JOY_PORT_2					= 'M150';

const uint32 MENU_C64DTV_ROM_FILE					= 'M151';

const uint32 MENU_SOUND_RECORD_AIFF					= 'M069';
const uint32 MENU_SOUND_RECORD_IFF					= 'M080';
const uint32 MENU_SOUND_RECORD_MP3					= 'M081';
const uint32 MENU_SOUND_RECORD_VOC					= 'M082';
const uint32 MENU_SOUND_RECORD_WAV					= 'M083';
const uint32 MENU_SOUND_RECORD_STOP					= 'M084';

const uint32 MENU_SIDCART_MODEL					= 'M087';
const uint32 MENU_SIDCART_ADDRESS					= 'M088';
const uint32 MENU_SIDCART_CLOCK					= 'M089';

const uint32 MENU_SIDCART_MODEL_6581				= 'M090';
const uint32 MENU_SIDCART_MODEL_8580				= 'M091';

const uint32 MENU_SIDCART_ADDRESS_1					= 'M092';
const uint32 MENU_SIDCART_ADDRESS_2					= 'M093';

const uint32 MENU_SIDCART_CLOCK_C64					= 'M094';
const uint32 MENU_SIDCART_CLOCK_NATIVE				= 'M095';

const uint32 MENU_VICII_BORDERS_NORMAL				= 'M096';
const uint32 MENU_VICII_BORDERS_FULL				= 'M097';
const uint32 MENU_VICII_BORDERS_DEBUG				= 'M098';

const uint32 MENU_TOGGLE_PS2MOUSE					= 'M152';

const uint32 MENU_MOUSE_TYPE_1351				= 'M153';
const uint32 MENU_MOUSE_TYPE_NEOS				= 'M154';
const uint32 MENU_MOUSE_TYPE_AMIGA				= 'M155';

const uint32 MENU_MOUSE_PORT_JOY1				= 'M156';
const uint32 MENU_MOUSE_PORT_JOY2				= 'M157';

const uint32 MENU_ABOUT							= 'M901';
const uint32 MENU_CONTRIBUTORS					= 'M902';
const uint32 MENU_LICENSE						= 'M904';
const uint32 MENU_WARRANTY						= 'M905';
const uint32 MENU_CMDLINE						= 'M906';

#endif
