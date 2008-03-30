/*
 * uisharedef.h - shared RISC OS specific defines
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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



#ifndef _UISHAREDEF_RO_H
#define _UISHAREDEF_RO_H

#include "uiconfig.h"


/* Misc icons */
#define Icon_Pane_LED0		1
#define Icon_Pane_LED1		3
#define Icon_Pane_LED2		5
#define Icon_Pane_LED3		7
#define Icon_Pane_Drive0	0
#define Icon_Pane_Drive1	2
#define Icon_Pane_Drive2	4
#define Icon_Pane_Drive3	6
#define Icon_Pane_Reset		8
#define Icon_Pane_Pause		9
#define Icon_Pane_Speed		10
#define Icon_Pane_Toggle	11
#define Icon_Pane_TrkSec	12

#define Icon_Sound_Volume	0
#define Icon_Sound_Notes	1

#define Icon_Save_Sprite	0
#define Icon_Save_Path		1
#define Icon_Save_OK		2

#define Icon_Snap_Sprite	0
#define Icon_Snap_Path		1
#define Icon_Snap_OK		2
#define Icon_Snap_ROM		3
#define Icon_Snap_Disk		4

#define Icon_Info_Name		5
#define Icon_Info_Purpose	6
#define Icon_Info_Version	9

#define Icon_Jam_Message	0
#define Icon_Jam_Reset		1
#define Icon_Jam_ResetH		2
#define Icon_Jam_Monitor	3

#define Icon_Create_Type	0
#define Icon_Create_TypeT	1
#define Icon_Create_Name	5
#define Icon_Create_Sprite	3
#define Icon_Create_OK		4
#define Icon_Create_File	2


/* Icon lists for machine-specific greying out */

#define ICON_LIST_PET \
  {CONF_WIN_PET, Icon_ConfPET_PetMem}, {CONF_WIN_PET, Icon_ConfPET_PetMemT},\
  {CONF_WIN_PET, Icon_ConfPET_PetIO}, {CONF_WIN_PET, Icon_ConfPET_PetIOT},\
  {CONF_WIN_PET, Icon_ConfPET_PetVideo}, {CONF_WIN_PET, Icon_ConfPET_PetVideoT},\
  {CONF_WIN_PET, Icon_ConfPET_PetModel}, {CONF_WIN_PET, Icon_ConfPET_PetModelT},\
  {CONF_WIN_PET, Icon_ConfPET_PetKbd}, {CONF_WIN_PET, Icon_ConfPET_PetCrt},\
  {CONF_WIN_PET, Icon_ConfPET_PetRAM9}, {CONF_WIN_PET, Icon_ConfPET_PetRAMA}, \
  {CONF_WIN_PET, Icon_ConfPET_PetDiagPin},

#define ICON_LIST_VIC \
  {CONF_WIN_VIC, Icon_ConfVIC_VICCart}, {CONF_WIN_VIC, Icon_ConfVIC_VICCartT},\
  {CONF_WIN_VIC, Icon_ConfVIC_VICCartF}, {CONF_WIN_VIC, Icon_ConfVIC_VICMem},

#define ICON_LIST_CART64 \
  {CONF_WIN_SYSTEM, Icon_ConfSys_CartType}, {CONF_WIN_SYSTEM, Icon_ConfSys_CartTypeT},\
  {CONF_WIN_SYSTEM, Icon_ConfSys_CartFile},

#define ICON_LIST_DEVRSUSR \
  {CONF_WIN_DEVICES, Icon_ConfDev_RsUsr}, {CONF_WIN_DEVICES, Icon_ConfDev_RsUsrDev}, \
  {CONF_WIN_DEVICES, Icon_ConfDev_RsUsrDevT}, {CONF_WIN_DEVICES, Icon_ConfDev_FileRsOK}, \
  {CONF_WIN_DEVICES, Icon_ConfDev_FileRsPath}, {CONF_WIN_DEVICES, Icon_ConfDev_FileRsIcon},

#define ICON_LIST_DEVICES \
  {CONF_WIN_DEVICES, Icon_ConfDev_ACIADE}, {CONF_WIN_DEVICES, Icon_ConfDev_ACIAD67},\
  {CONF_WIN_DEVICES, Icon_ConfDev_ACIAIrq}, {CONF_WIN_DEVICES, Icon_ConfDev_ACIADev},\
  {CONF_WIN_DEVICES, Icon_ConfDev_ACIADevT},   {CONF_WIN_DEVICES, Icon_ConfDev_Serial}, \
  {CONF_WIN_DEVICES, Icon_ConfDev_SerialT}, \
  ICON_LIST_DEVRSUSR

#define ICON_LIST_SID \
  {CONF_WIN_SOUND, Icon_ConfSnd_SidFilter}, {CONF_WIN_SOUND, Icon_ConfSnd_SidModel},\
  {CONF_WIN_SOUND, Icon_ConfSnd_SidModelT}, {CONF_WIN_SOUND, Icon_ConfSnd_SidEngine},\
  {CONF_WIN_SOUND, Icon_ConfSnd_SidEngineT}, {CONF_WIN_SOUND, Icon_ConfSnd_ResidSamp},\
  {CONF_WIN_SOUND, Icon_ConfSnd_ResidSampT}, {CONF_WIN_SOUND, Icon_ConfSnd_ResidPass},\
  {CONF_WIN_SOUND, Icon_ConfSnd_SidStereo},

#define ICON_LIST_SYSTEM \
  {CONF_WIN_SYSTEM, Icon_ConfSys_REU}, {CONF_WIN_SYSTEM, Icon_ConfSys_IEEE488},\
  {CONF_WIN_SYSTEM, Icon_ConfSys_CheckSScoll}, {CONF_WIN_SYSTEM, Icon_ConfSys_CheckSBcoll},\

#define ICON_LIST_SYS64 \
  {CONF_WIN_SYSTEM, Icon_ConfSys_CharGen}, {CONF_WIN_SYSTEM, Icon_ConfSys_Kernal},\
  {CONF_WIN_SYSTEM, Icon_ConfSys_Basic},

#define ICON_LIST_TRUE \
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrv}, \
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt8}, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt8T}, \
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle8}, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle8T},\
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar8}, \
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt9}, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt9T}, \
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle9}, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle9T},\
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar9},



/* Emulator menu entries */
#define Menu_EmuWin_Items	11
#define Menu_EmuWin_Width	200
#define Menu_EmuWin_Configure	0
#define Menu_EmuWin_Fliplist	1
#define Menu_EmuWin_Snapshot	2
#define Menu_EmuWin_Screenshot	3
#define Menu_EmuWin_Freeze	4
#define Menu_EmuWin_Pane	5
#define Menu_EmuWin_Active	6
#define Menu_EmuWin_TrueDrvEmu	7
#define Menu_EmuWin_Datasette	8
#define Menu_EmuWin_Sound	9
#define Menu_EmuWin_Monitor	10



struct wimp_msg_desc_s;
struct help_icon_s;

struct ui_machine_callback_s {
  int (*setup_config_window)(int);
  int (*mouse_click_event)(int *);
  int (*mouse_click_ibar)(int *);
  int (*key_pressed_event)(int *);
  int (*key_pressed_config)(int *, int, const char *);
  int (*menu_select_emuwin)(int *);
  int (*menu_select_config_pre)(int *, int);
  int (*menu_select_config_main)(int *, int);
  int (*usr_msg_data_load)(int *);
  int (*usr_msg_data_open)(int *);
  int (*load_prg_file)(const char *);
  int (*display_speed)(int, int, int);
  struct help_icon_s *(*help_for_window_icon)(int, int);
};

extern struct ui_machine_callback_s ViceMachineCallbacks;

#endif
