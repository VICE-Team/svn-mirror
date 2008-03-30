/*
 * uiconfig.h - RISC OS configuration data structures.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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



#ifndef _UICONFIG_RO_H
#define _UICONFIG_RO_H


#include "wimp.h"


/* Configuration windows */
#define CONF_WIN_DRIVES		0
#define CONF_WIN_TAPE		1
#define CONF_WIN_DEVICES	2
#define CONF_WIN_SOUND		3
#define CONF_WIN_SYSTEM		4
#define CONF_WIN_JOY		5
#define CONF_WIN_PET		6
#define CONF_WIN_VIC		7
#define CONF_WIN_CBM2		8
#define CONF_WIN_C128		9
#define CONF_WIN_NUMBER		10

/* Configuration menus */
#define CONF_MENU_PRNTDEV	0
#define CONF_MENU_PRUSER	1
#define CONF_MENU_SAMPRATE	2
#define CONF_MENU_SOUNDDEV	3
#define CONF_MENU_SOUNDOVER	4
#define CONF_MENU_TRUESYNC	5
#define CONF_MENU_TRUEIDLE8	6
#define CONF_MENU_TRUEEXT8	7
#define CONF_MENU_TRUETYPE8	8
#define CONF_MENU_TRUEIDLE9	9
#define CONF_MENU_TRUEEXT9	10
#define CONF_MENU_TRUETYPE9	11
#define CONF_MENU_DRIVE8	12
#define CONF_MENU_DRIVE9	13
#define CONF_MENU_DRIVE10	14
#define CONF_MENU_DRIVE11	15
#define CONF_MENU_CARTTYPE	16
#define CONF_MENU_RSUSRDEV	17
#define CONF_MENU_ACIADEV	18
#define CONF_MENU_SERIAL	19
#define CONF_MENU_SIDMODEL	20
#define CONF_MENU_SPEED		21
#define CONF_MENU_REFRESH	22
#define CONF_MENU_PETMEM	23
#define CONF_MENU_PETIO		24
#define CONF_MENU_PETVIDEO	25
#define CONF_MENU_PETMODEL	26
#define CONF_MENU_VICRAM	27
#define CONF_MENU_VICCART	28
#define CONF_MENU_DOSNAME	29
#define CONF_MENU_C2LINE	30
#define CONF_MENU_C2MEM		31
#define CONF_MENU_C2MODEL	32
#define CONF_MENU_C2RAM		33
#define CONF_MENU_C2CART	34
#define CONF_MENU_SNDBUFF	35
#define CONF_MENU_JOYDEV1	36
#define CONF_MENU_JOYDEV2	37
#define CONF_MENU_ROMSET	38
#define CONF_MENU_ROMACT	39
#define CONF_MENU_SYSKBD	40
#define CONF_MENU_SPDADJUST	41
#define CONF_MENU_RESIDSAMP	42

/* Configuration issues... */
#define CONFIG_INT	1
#define CONFIG_SELECT	2
#define CONFIG_STRING	3
#define CONFIG_DRAG	4

/* Menu item display initializers */
#define DISP_DESC_STRING	1
#define DISP_DESC_BITFIELD	2
#define DISP_DESC_STRSHOW	4



/*
 *  Data structures
 */

/* Config icon identifier: pair of {config window number, icon number} */
typedef struct {
  unsigned char win;
  unsigned char icon;
} conf_icon_id;

typedef struct {
  const char *resource;
  unsigned char ctype;
  conf_icon_id id;
} config_item;

typedef struct {
  RO_MenuHead *menu;
  const char *resource;
  conf_icon_id id;
} menu_icon;

typedef struct {
  int icon;
  int item;
} disp_strshow_t;

typedef struct {
  const char *resource;
  conf_icon_id id;
  RO_MenuHead *menu;
  int items;
  unsigned int flags;
  unsigned int writable;
} disp_desc_t;




#define Menu_Height		44
#define Menu_Flags		0x07003011

/* The menus */
#define MENU_HEADER(name,w) \
  {name, 7,2,7,0, w, Menu_Height, 0}
#define MENU_ITEM(name) \
  {0, (RO_MenuHead*)-1, Menu_Flags, {name}}
#define MENU_ITEM_LAST(name) \
  {MFlg_LastItem, (RO_MenuHead*)-1, Menu_Flags, {name}}
#define MENU_ITEM_SUB(name,sub) \
  {0, (RO_MenuHead*)sub, Menu_Flags, {name}}
#define MENU_ITEM_SUBLAST(name, sub) \
  {MFlg_LastItem, (RO_MenuHead*)sub, Menu_Flags, {name}}




/*
 *  Configuration icons
 */

/* Drive config */
#define Icon_Conf_TrueDrv	16
#define Icon_Conf_TrueDrvPar8	17
#define Icon_Conf_TrueDrvExt8	19
#define Icon_Conf_TrueDrvExt8T	20
#define Icon_Conf_TrueDrvIdle8	21
#define Icon_Conf_TrueDrvIdle8T	22
#define Icon_Conf_TrueDrvType8	27
#define Icon_Conf_TrueDrvType8T	28
#define Icon_Conf_TrueDrvPar9	29
#define Icon_Conf_TrueDrvExt9	30
#define Icon_Conf_TrueDrvExt9T	31
#define Icon_Conf_TrueDrvIdle9	32
#define Icon_Conf_TrueDrvIdle9T	33
#define Icon_Conf_TrueDrvType9	34
#define Icon_Conf_TrueDrvType9T	35
#define Icon_Conf_DriveType8	2
#define Icon_Conf_DriveType9	5
#define Icon_Conf_DriveType10	8
#define Icon_Conf_DriveType11	11
#define Icon_Conf_DriveFile8	3
#define Icon_Conf_DriveFile9	6
#define Icon_Conf_DriveFile10	9
#define Icon_Conf_DriveFile11	12
#define Icon_Conf_DriveRdOnly8	24
#define Icon_Conf_DriveRdOnly9	25
#define Icon_Conf_DriveRdOnly10	26
#define Icon_Conf_DriveRdOnly11	36

/* Tape config */
#define Icon_Conf_TapeFile	2
#define Icon_Conf_TapeDetach	3
#define Icon_Conf_DataReset	4
#define Icon_Conf_DataCounter	6
#define Icon_Conf_DataStop	8
#define Icon_Conf_DataRewind	9
#define Icon_Conf_DataPlay	10
#define Icon_Conf_DataForward	11
#define Icon_Conf_DataRecord	12
#define Icon_Conf_DataDoReset	13

/* Device config */
#define Icon_Conf_ACIAIrq	2
#define Icon_Conf_ACIADev	3
#define Icon_Conf_ACIADevT	4
#define Icon_Conf_ACIADE	8
#define Icon_Conf_ACIAD67	9
#define Icon_Conf_RsUsr		5
#define Icon_Conf_RsUsrDev	6
#define Icon_Conf_RsUsrDevT	7
#define Icon_Conf_Serial	11
#define Icon_Conf_SerialT	12
#define Icon_Conf_FileRsOK	13
#define Icon_Conf_FileRsPath	14
#define Icon_Conf_FileRsIcon	15
#define Icon_Conf_PrntOn	19
#define Icon_Conf_PrntDev	20
#define Icon_Conf_PrntDevT	21
#define Icon_Conf_PrntUsrOn	22
#define Icon_Conf_PrntUsrDev	23
#define Icon_Conf_PrntUsrDevT	24
#define Icon_Conf_FilePrOK	25
#define Icon_Conf_FilePrPath	26
#define Icon_Conf_FilePrIcon	27

/* Sound config */
#define Icon_Conf_SoundOn	2
#define Icon_Conf_SampleRate	3
#define Icon_Conf_SampleRateT	4
#define Icon_Conf_SoundDev	5
#define Icon_Conf_SoundDevT	6
#define Icon_Conf_Oversample	7
#define Icon_Conf_OversampleT	8
#define Icon_Conf_SidModel	9
#define Icon_Conf_SidModelT	10
#define Icon_Conf_SidFilter	11
#define Icon_Conf_FileSndOK	12
#define Icon_Conf_FileSndPath	13
#define Icon_Conf_FileSndIcon	14
#define Icon_Conf_Volume	17
#define Icon_Conf_SoundBuff	18
#define Icon_Conf_SoundBuffT	19
#define Icon_Conf_UseResid	20
#define Icon_Conf_SpeedAdjust	21
#define Icon_Conf_SpeedAdjustT	22
#define Icon_Conf_ResidSamp	25
#define Icon_Conf_ResidSampT	26
#define Icon_Conf_ResidPass	27
#define Icon_Conf_Sound16Bit	29
#define Icon_Conf_SidStereo	30
#define Icon_Conf_Sid2Addr	31
#define Icon_Conf_Sid2AddrT	32

/* System config */
#define Icon_Conf_CharGen	3
#define Icon_Conf_Kernal	5
#define Icon_Conf_Basic		7
#define Icon_Conf_Palette	9
#define Icon_Conf_REU		10
#define Icon_Conf_IEEE488	11
#define Icon_Conf_EmuID		12
#define Icon_Conf_NoTraps	13
#define Icon_Conf_VideoCache	17
#define Icon_Conf_PollEvery	21
#define Icon_Conf_SpeedEvery	23
#define Icon_Conf_SoundEvery	25
#define Icon_Conf_SpeedLmt	31
#define Icon_Conf_SpeedLmtT	32
#define Icon_Conf_Refresh	29
#define Icon_Conf_RefreshT	30
#define Icon_Conf_WarpMode	28
#define Icon_Conf_CartType	14
#define Icon_Conf_CartTypeT	15
#define Icon_Conf_CartFile	16
#define Icon_Conf_CheckSScoll	35
#define Icon_Conf_CheckSBcoll	36
#define Icon_Conf_DosName	37
#define Icon_Conf_DosNameT	38
#define Icon_Conf_DosNameF	39
#define Icon_Conf_AutoPause	40
#define Icon_Conf_ROMSet	43
#define Icon_Conf_ROMSetT	44
#define Icon_Conf_ROMAction	46
#define Icon_Conf_FullScreen	48
#define Icon_Conf_SetPalette	49
#define Icon_Conf_Keyboard	50
#define Icon_Conf_KeyboardT	51
#define Icon_Conf_VideoSync	52
#define Icon_Conf_VideoSyncT	53
#define Icon_Conf_MaxSkipFrms	54
#define Icon_Conf_UseBPlot	58
#define Icon_Conf_PALDepth	61
#define Icon_Conf_PALDepthT	62
#define Icon_Conf_PALDouble	64

/* Joystick conf */
#define Icon_Conf_JoyPort1	2
#define Icon_Conf_JoyPort1T	3
#define Icon_Conf_JoyPort2	4
#define Icon_Conf_JoyPort2T	5
#define Icon_Conf_JoyKey1U	8
#define Icon_Conf_JoyKey1D	10
#define Icon_Conf_JoyKey1L	12
#define Icon_Conf_JoyKey1R	14
#define Icon_Conf_JoyKey1F	16
#define Icon_Conf_JoyKey2U	20
#define Icon_Conf_JoyKey2D	22
#define Icon_Conf_JoyKey2L	24
#define Icon_Conf_JoyKey2R	26
#define Icon_Conf_JoyKey2F	28

/* PET config */
#define Icon_Conf_PetMem	2
#define Icon_Conf_PetMemT	3
#define Icon_Conf_PetIO		4
#define Icon_Conf_PetIOT	5
#define Icon_Conf_PetVideo	6
#define Icon_Conf_PetVideoT	7
#define Icon_Conf_PetModel	8
#define Icon_Conf_PetModelT	9
#define Icon_Conf_PetKbd	10
#define Icon_Conf_PetCrt	12
#define Icon_Conf_PetRAM9	13
#define Icon_Conf_PetRAMA	14
#define Icon_Conf_PetDiagPin	15
#define Icon_Conf_PetSuper	16

/* VIC config */
#define Icon_Conf_VICCart	2
#define Icon_Conf_VICCartT	3
#define Icon_Conf_VICCartF	4
#define Icon_Conf_VICMem	6

/* CBM 2 config */
#define Icon_Conf_CBM2Line	2
#define Icon_Conf_CBM2LineT	3
#define Icon_Conf_CBM2Mem	4
#define Icon_Conf_CBM2MemT	5
#define Icon_Conf_CBM2Model	6
#define Icon_Conf_CBM2ModelT	7
#define Icon_Conf_CBM2RAM	8
#define Icon_Conf_CBM2Kbd	10
#define Icon_Conf_CBM2Cart	12
#define Icon_Conf_CBM2CartT	13
#define Icon_Conf_CBM2CartF	14

/* C128 config */
#define Icon_Conf_C128Palette	3
#define Icon_Conf_C128Cache	4
#define Icon_Conf_C128Size	5
#define Icon_Conf_C1284080	6
#define Icon_Conf_C128z80bios	8
#define Icon_Conf_C128dblsize	9
#define Icon_Conf_C128dblscan	10



/* some special menu entries */
#define Menu_DriveDisk_Detach	0

#define Menu_DriveFS_ConvP00	0
#define Menu_DriveFS_SaveP00	1
#define Menu_DriveFS_HideCBM	2

#define Menu_VideoSync_Custom	2

#define Menu_DriveType_Disk	0
#define Menu_DriveType_FS	1

#define Menu_RomAct_Create	0
#define Menu_RomAct_Delete	1
#define Menu_RomAct_Save	2
#define Menu_RomAct_Dump	3
#define Menu_RomAct_Clear	4
#define Menu_RomAct_Restore	5

#define Menu_SysKbd_Save	0
#define Menu_SysKbd_LoadDef	1




/*
 *  Exported symbols
 */

extern menu_icon ConfigMenus[];
extern disp_desc_t *ConfigDispDescs[];
extern config_item Configurations[];

#endif
