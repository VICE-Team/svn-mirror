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
#define CONF_WIN_VIDEO		5
#define CONF_WIN_JOY		6
#define CONF_WIN_PET		7
#define CONF_WIN_VIC		8
#define CONF_WIN_CBM2		9
#define CONF_WIN_C128		10
#define CONF_WIN_NUMBER		11

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
#define CONF_MENU_SID2ADDR	43
#define CONF_MENU_PALDEPTH	44
#define CONF_MENU_PALMODE	45
#define CONF_MENU_VIDCACHE	46
#define CONF_MENU_SIDENGINE	47

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
typedef struct conf_iconid_s {
  unsigned char win;
  unsigned char icon;
} conf_iconid_t;

typedef struct conf_item_s {
  const char *resource;
  unsigned char ctype;
  conf_iconid_t id;
} config_item_t;

typedef struct disp_strshow_s {
  int icon;
  int item;
} disp_strshow_t;

typedef struct disp_desc_s {
  const char *resource;
  conf_iconid_t id;
  RO_MenuHead *menu;
  int items;
  unsigned int flags;
  unsigned int writable;
} disp_desc_t;

typedef struct menu_icon_s {
  RO_MenuHead *menu;
  const char *resource;
  disp_desc_t *desc;
  conf_iconid_t id;
} menu_icon_t;





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
#define Icon_ConfDrv_TrueDrv		16
#define Icon_ConfDrv_TrueDrvPar8	17
#define Icon_ConfDrv_TrueDrvExt8	19
#define Icon_ConfDrv_TrueDrvExt8T	20
#define Icon_ConfDrv_TrueDrvIdle8	21
#define Icon_ConfDrv_TrueDrvIdle8T	22
#define Icon_ConfDrv_TrueDrvType8	27
#define Icon_ConfDrv_TrueDrvType8T	28
#define Icon_ConfDrv_TrueDrvPar9	29
#define Icon_ConfDrv_TrueDrvExt9	30
#define Icon_ConfDrv_TrueDrvExt9T	31
#define Icon_ConfDrv_TrueDrvIdle9	32
#define Icon_ConfDrv_TrueDrvIdle9T	33
#define Icon_ConfDrv_TrueDrvType9	34
#define Icon_ConfDrv_TrueDrvType9T	35
#define Icon_ConfDrv_DriveType8		2
#define Icon_ConfDrv_DriveType9		5
#define Icon_ConfDrv_DriveType10	8
#define Icon_ConfDrv_DriveType11	11
#define Icon_ConfDrv_DriveFile8		3
#define Icon_ConfDrv_DriveFile9		6
#define Icon_ConfDrv_DriveFile10	9
#define Icon_ConfDrv_DriveFile11	12
#define Icon_ConfDrv_DriveRdOnly8	24
#define Icon_ConfDrv_DriveRdOnly9	25
#define Icon_ConfDrv_DriveRdOnly10	26
#define Icon_ConfDrv_DriveRdOnly11	36

/* Tape config */
#define Icon_ConfTap_TapeFile		2
#define Icon_ConfTap_TapeDetach		3
#define Icon_ConfTap_DataReset		4
#define Icon_ConfTap_DataCounter	6
#define Icon_ConfTap_DataStop		8
#define Icon_ConfTap_DataRewind		9
#define Icon_ConfTap_DataPlay		10
#define Icon_ConfTap_DataForward	11
#define Icon_ConfTap_DataRecord		12
#define Icon_ConfTap_DataDoReset	13

/* Device config */
#define Icon_ConfDev_ACIAIrq		2
#define Icon_ConfDev_ACIADev		3
#define Icon_ConfDev_ACIADevT		4
#define Icon_ConfDev_ACIADE		8
#define Icon_ConfDev_ACIAD67		9
#define Icon_ConfDev_RsUsr		5
#define Icon_ConfDev_RsUsrDev		6
#define Icon_ConfDev_RsUsrDevT		7
#define Icon_ConfDev_Serial		11
#define Icon_ConfDev_SerialT		12
#define Icon_ConfDev_FileRsOK		13
#define Icon_ConfDev_FileRsPath		14
#define Icon_ConfDev_FileRsIcon		15
#define Icon_ConfDev_PrntOn		19
#define Icon_ConfDev_PrntDev		20
#define Icon_ConfDev_PrntDevT		21
#define Icon_ConfDev_PrntUsrOn		22
#define Icon_ConfDev_PrntUsrDev		23
#define Icon_ConfDev_PrntUsrDevT	24
#define Icon_ConfDev_FilePrOK		25
#define Icon_ConfDev_FilePrPath		26
#define Icon_ConfDev_FilePrIcon		27

/* Sound config */
#define Icon_ConfSnd_SoundOn		2
#define Icon_ConfSnd_SampleRate		3
#define Icon_ConfSnd_SampleRateT	4
#define Icon_ConfSnd_SoundDev		5
#define Icon_ConfSnd_SoundDevT		6
#define Icon_ConfSnd_Oversample		7
#define Icon_ConfSnd_OversampleT	8
#define Icon_ConfSnd_SidModel		9
#define Icon_ConfSnd_SidModelT		10
#define Icon_ConfSnd_SidFilter		11
#define Icon_ConfSnd_FileSndOK		12
#define Icon_ConfSnd_FileSndPath	13
#define Icon_ConfSnd_FileSndIcon	14
#define Icon_ConfSnd_Volume		17
#define Icon_ConfSnd_SoundBuff		18
#define Icon_ConfSnd_SoundBuffT		19
#define Icon_ConfSnd_SidEngine		20
#define Icon_ConfSnd_SidEngineT		34
#define Icon_ConfSnd_SpeedAdjust	21
#define Icon_ConfSnd_SpeedAdjustT	22
#define Icon_ConfSnd_ResidSamp		25
#define Icon_ConfSnd_ResidSampT		26
#define Icon_ConfSnd_ResidPass		27
#define Icon_ConfSnd_Sound16Bit		29
#define Icon_ConfSnd_SidStereo		30
#define Icon_ConfSnd_Sid2Addr		31
#define Icon_ConfSnd_Sid2AddrT		32

/* System config */
#define Icon_ConfSys_CharGen		3
#define Icon_ConfSys_Kernal		5
#define Icon_ConfSys_Basic		7
#define Icon_ConfSys_REU		10
#define Icon_ConfSys_IEEE488		11
#define Icon_ConfSys_EmuID		12
#define Icon_ConfSys_NoTraps		13
#define Icon_ConfSys_PollEvery		21
#define Icon_ConfSys_SpeedEvery		23
#define Icon_ConfSys_SoundEvery		25
#define Icon_ConfSys_SpeedLmt		29
#define Icon_ConfSys_SpeedLmtT		30
#define Icon_ConfSys_WarpMode		28
#define Icon_ConfSys_CartType		14
#define Icon_ConfSys_CartTypeT		15
#define Icon_ConfSys_CartFile		16
#define Icon_ConfSys_CheckSScoll	35
#define Icon_ConfSys_CheckSBcoll	34
#define Icon_ConfSys_DosName		36
#define Icon_ConfSys_DosNameT		37
#define Icon_ConfSys_DosNameF		38
#define Icon_ConfSys_AutoPause		40
#define Icon_ConfSys_ROMSet		39
#define Icon_ConfSys_ROMSetT		43
#define Icon_ConfSys_ROMAction		45
#define Icon_ConfSys_VideoSync		48
#define Icon_ConfSys_VideoSyncT		49
#define Icon_ConfSys_Keyboard		46
#define Icon_ConfSys_KeyboardT		47

/* Video config */
#define Icon_ConfVid_UseBPlot		2
#define Icon_ConfVid_PALDepth		6
#define Icon_ConfVid_PALDepthT		7
#define Icon_ConfVid_PALDouble		8
#define Icon_ConfVid_SetPalette		9
#define Icon_ConfVid_Refresh		10
#define Icon_ConfVid_RefreshT		11
#define Icon_ConfVid_FullScrNorm	16
#define Icon_ConfVid_FullScrPAL		35
#define Icon_ConfVid_FullScrDbl		37
#define Icon_ConfVid_MaxSkipFrms	13
#define Icon_ConfVid_Palette		32
#define Icon_ConfVid_ExtPal		33
#define Icon_ConfVid_ColourSat		18
#define Icon_ConfVid_Contrast		20
#define Icon_ConfVid_Brightness		22
#define Icon_ConfVid_Gamma		24
#define Icon_ConfVid_LineShade		26
#define Icon_ConfVid_PalMode		28
#define Icon_ConfVid_PalModeT		29
#define Icon_ConfVid_VCache		38

/* Joystick conf */
#define Icon_ConfJoy_JoyPort1		2
#define Icon_ConfJoy_JoyPort1T		3
#define Icon_ConfJoy_JoyPort2		4
#define Icon_ConfJoy_JoyPort2T		5
#define Icon_ConfJoy_JoyKey1U		8
#define Icon_ConfJoy_JoyKey1D		10
#define Icon_ConfJoy_JoyKey1L		12
#define Icon_ConfJoy_JoyKey1R		14
#define Icon_ConfJoy_JoyKey1F		16
#define Icon_ConfJoy_JoyKey2U		20
#define Icon_ConfJoy_JoyKey2D		22
#define Icon_ConfJoy_JoyKey2L		24
#define Icon_ConfJoy_JoyKey2R		26
#define Icon_ConfJoy_JoyKey2F		28

/* PET config */
#define Icon_ConfPET_PetMem		2
#define Icon_ConfPET_PetMemT		3
#define Icon_ConfPET_PetIO		4
#define Icon_ConfPET_PetIOT		5
#define Icon_ConfPET_PetVideo		6
#define Icon_ConfPET_PetVideoT		7
#define Icon_ConfPET_PetModel		8
#define Icon_ConfPET_PetModelT		9
#define Icon_ConfPET_PetKbd		10
#define Icon_ConfPET_PetCrt		12
#define Icon_ConfPET_PetRAM9		13
#define Icon_ConfPET_PetRAMA		14
#define Icon_ConfPET_PetDiagPin		15
#define Icon_ConfPET_PetSuper		16

/* VIC config */
#define Icon_ConfVIC_VICCart		2
#define Icon_ConfVIC_VICCartT		3
#define Icon_ConfVIC_VICCartF		4
#define Icon_ConfVIC_VICMem		6

/* CBM 2 config */
#define Icon_ConfCBM_CBM2Line		2
#define Icon_ConfCBM_CBM2LineT		3
#define Icon_ConfCBM_CBM2Mem		4
#define Icon_ConfCBM_CBM2MemT		5
#define Icon_ConfCBM_CBM2Model		6
#define Icon_ConfCBM_CBM2ModelT		7
#define Icon_ConfCBM_CBM2RAM		8
#define Icon_ConfCBM_CBM2Kbd		10
#define Icon_ConfCBM_CBM2Cart		12
#define Icon_ConfCBM_CBM2CartT		13
#define Icon_ConfCBM_CBM2CartF		14

/* C128 config */
#define Icon_Conf128_C128Palette	3
#define Icon_Conf128_C128Size		5
#define Icon_Conf128_C1284080		6
#define Icon_Conf128_C128z80bios	8
#define Icon_Conf128_C128dblsize	9
#define Icon_Conf128_C128dblscan	10



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

extern menu_icon_t ConfigMenus[];
extern config_item_t Configurations[];

extern RO_Window *ConfWindows[CONF_WIN_NUMBER];

#endif
