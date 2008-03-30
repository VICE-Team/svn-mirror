/*
 * ui.c - RISC OS GUI.
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ROlib.h"
#include "wimp.h"

#include "vice.h"

#include "attach.h"
#include "c610ui.h"
#include "c64mem.h"
#include "cartridge.h"
#include "drive.h"
#include "fsdevice.h"
#include "joystick.h"
#include "kbd.h"
#include "log.h"
#include "machine.h"
#include "petui.h"
#include "resources.h"
#include "romset.h"
#include "sound.h"
#include "sysfile.h"
#include "tape.h"
#include "ui.h"
#include "uiimage.h"
#include "utils.h"
#include "vicii.h"
#include "vsync.h"





extern int pet_set_model(const char *name, void *extra);
extern int cbm2_set_model(const char *name, void *extra);

/* Defined in soundacorn.c. Important for timer handling! */
extern int  sound_wimp_poll_prologue(void);
extern int  sound_wimp_poll_epilogue(int install);
extern void sound_wimp_safe_exit(void);

/* Defined in raster.c, but raster.h is a mess, so delare it here... */
extern int handle_mode_change(void);



/* Declare some static functions */
static int  ui_poll_core(int *block);
static void ui_poll_prologue(void);
static void ui_poll_epilogue(void);
static void ui_temp_suspend_sound(void);
static void ui_temp_resume_sound(void);





/* Misc text */
static char CustomSprites[] = "Vice:Sprites";
static char TemplatesFile[] = "Vice:Templates";
static char MessagesFile[] = "<Vice$Messages>";
static char WimpScrapFile[] = "<Wimp$ScrapDir>.ViceSnap";
static char VicePathVariable[] = "Vice$Path";
static char ResourceDriveDir[] = "DRIVES";


#define RSETARCH_EXT	"vra"



/* Error messages */
#define Error_IconSprite	0x100
#define Error_TemplatesFile	0x101
#define Error_Template		0x102


#define FileType_Text		0xfff
#define FileType_C64File	0x064


/* Start scanning for internal keynumbers from here */
#define IntKey_MinCode		3
#define IntKey_Shift		0

/* Return key in icons */
#define Key_Return		13


/* For sound volume icons */
#define Maximum_Volume		127
#define Well_Border		12

/* Maximum sound latency (ms) */
#define Maximum_Latency		500


/* Menu definitions */
#define Menu_IBar		1
#define Menu_Emulator		2

#define Menu_Height		44
#define Menu_Flags		0x07003011


/* Icons */
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

#define Icon_Jam_Message	0
#define Icon_Jam_Reset		1
#define Icon_Jam_ResetH		2
#define Icon_Jam_Monitor	3
#define Icon_Jam_Debug		4



/* Drive config */
#define Icon_Conf_TrueDrv	16
#define Icon_Conf_TrueDrvSync	18
#define Icon_Conf_TrueDrvSyncT	23
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
#define Icon_Conf_TapeFile	26
#define Icon_Conf_TapeDetach	36

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





/* Drag types */
#define DRAG_TYPE_NONE		0
#define DRAG_TYPE_SOUND		1
#define DRAG_TYPE_SERIAL	2
#define DRAG_TYPE_PRINTER	3
#define DRAG_TYPE_SNAPSHOT	4
#define DRAG_TYPE_VOLUME	5
#define DRAG_TYPE_SAVEBOX	6







/* Configuration windows */
#define CONF_WIN_DRIVES		0
#define CONF_WIN_DEVICES	1
#define CONF_WIN_SOUND		2
#define CONF_WIN_SYSTEM		3
#define CONF_WIN_JOY		4
#define CONF_WIN_PET		5
#define CONF_WIN_VIC		6
#define CONF_WIN_CBM2		7
#define CONF_WIN_NUMBER		8








/* Function type for setting a value */
typedef int (*set_var_function)(const char *name, resource_value_t val);


/* Config icon identifier: pair of {config window number, icon number} */
typedef struct {
  unsigned char win;
  unsigned char icon;
} conf_icon_id;





/* Variables */

unsigned int TaskHandle;

static int WimpMessages[] = {
  Message_DataSave, Message_DataSaveAck, Message_DataLoad, Message_DataLoadAck,
  Message_DataOpen, Message_PaletteChange, Message_ModeChange, Message_MenuWarning, 0
};

/* General wimp variable */
static int PollMask;
static int LastMenu;
static int LastClick;
static int LastDrag;
static int MenuType;
static int DragType;
static int LastSpeedLimit;
static int CMOS_DragType;
static int ShowPane = 1;
static int TrueDriveEmulation = 0;
static int SoundEnabled = 0;
static int SoundSuspended = 0;
static int WasAutoPaused = 0;
static int SoundVolume;
static int DisplayFPS = 0;
static int DisplayDriveTrack = 0;
static int tracknums[2] = {36, 36};
static int WimpScrapUsed = 0;
static int JoystickWindowOpen = 0;
static int WithinUiPoll = 0;
static int WimpBlock[64];

static int SnapshotPending = 0;
static int SnapshotMessage[64];


/* Custom sprites area */
static int *SpriteArea;

/* Icon translation tables */
static char LEDtoIcon[4] = {Icon_Pane_LED0, Icon_Pane_LED1, Icon_Pane_LED2, Icon_Pane_LED3};
static char DriveToFile[4] = {Icon_Conf_DriveFile8, Icon_Conf_DriveFile9, Icon_Conf_DriveFile10, Icon_Conf_DriveFile11};

/* Config icons affected by True Drive Emulation state */
static conf_icon_id TrueDependentIcons[] = {
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSync},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSyncT},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar8},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt8},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt8T},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle8},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle8T},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar9},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt9},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt9T},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle9},
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle9T},
  {0xff, 0xff}
};

/* Config icons affected by Sound enable state */
static conf_icon_id SoundDependentIcons[] = {
  {CONF_WIN_SOUND, Icon_Conf_SampleRate},
  {CONF_WIN_SOUND, Icon_Conf_SampleRateT},
  {CONF_WIN_SOUND, Icon_Conf_SoundDev},
  {CONF_WIN_SOUND, Icon_Conf_SoundDevT},
  {CONF_WIN_SOUND, Icon_Conf_Oversample},
  {CONF_WIN_SOUND, Icon_Conf_OversampleT},
  {CONF_WIN_SOUND, Icon_Conf_Volume},
  {0xff, 0xff}
};

static conf_icon_id SidDependentIcons[] = {
  {CONF_WIN_SOUND, Icon_Conf_SidFilter},
  {CONF_WIN_SOUND, Icon_Conf_UseResid},
  {CONF_WIN_SOUND, Icon_Conf_SidModel},
  {CONF_WIN_SOUND, Icon_Conf_SidModelT},
  {0xff, 0xff}
};


#define ICON_LIST_PET \
  {CONF_WIN_PET, Icon_Conf_PetMem}, {CONF_WIN_PET, Icon_Conf_PetMemT},\
  {CONF_WIN_PET, Icon_Conf_PetIO}, {CONF_WIN_PET, Icon_Conf_PetIOT},\
  {CONF_WIN_PET, Icon_Conf_PetVideo}, {CONF_WIN_PET, Icon_Conf_PetVideoT},\
  {CONF_WIN_PET, Icon_Conf_PetModel}, {CONF_WIN_PET, Icon_Conf_PetModelT},\
  {CONF_WIN_PET, Icon_Conf_PetKbd}, {CONF_WIN_PET, Icon_Conf_PetCrt},\
  {CONF_WIN_PET, Icon_Conf_PetRAM9}, {CONF_WIN_PET, Icon_Conf_PetRAMA}, \
  {CONF_WIN_PET, Icon_Conf_PetDiagPin},

#define ICON_LIST_VIC \
  {CONF_WIN_VIC, Icon_Conf_VICCart}, {CONF_WIN_VIC, Icon_Conf_VICCartT},\
  {CONF_WIN_VIC, Icon_Conf_VICCartF}, {CONF_WIN_VIC, Icon_Conf_VICMem},

#define ICON_LIST_CART64 \
  {CONF_WIN_SYSTEM, Icon_Conf_CartType}, {CONF_WIN_SYSTEM, Icon_Conf_CartTypeT},\
  {CONF_WIN_SYSTEM, Icon_Conf_CartFile},

#define ICON_LIST_DEVRSUSR \
  {CONF_WIN_DEVICES, Icon_Conf_RsUsr}, {CONF_WIN_DEVICES, Icon_Conf_RsUsrDev}, \
  {CONF_WIN_DEVICES, Icon_Conf_RsUsrDevT}, {CONF_WIN_DEVICES, Icon_Conf_FileRsOK}, \
  {CONF_WIN_DEVICES, Icon_Conf_FileRsPath}, {CONF_WIN_DEVICES, Icon_Conf_FileRsIcon},

#define ICON_LIST_DEVICES \
  {CONF_WIN_DEVICES, Icon_Conf_ACIADE}, {CONF_WIN_DEVICES, Icon_Conf_ACIAD67},\
  {CONF_WIN_DEVICES, Icon_Conf_ACIAIrq}, {CONF_WIN_DEVICES, Icon_Conf_ACIADev},\
  {CONF_WIN_DEVICES, Icon_Conf_ACIADevT},   {CONF_WIN_DEVICES, Icon_Conf_Serial}, \
  {CONF_WIN_DEVICES, Icon_Conf_SerialT}, \
  ICON_LIST_DEVRSUSR

#define ICON_LIST_SID \
  {CONF_WIN_SOUND, Icon_Conf_SidFilter}, {CONF_WIN_SOUND, Icon_Conf_SidModel},\
  {CONF_WIN_SOUND, Icon_Conf_SidModelT}, {CONF_WIN_SOUND, Icon_Conf_UseResid},

#define ICON_LIST_SYSTEM \
  {CONF_WIN_SYSTEM, Icon_Conf_REU}, {CONF_WIN_SYSTEM, Icon_Conf_IEEE488},\
  {CONF_WIN_SYSTEM, Icon_Conf_CheckSScoll}, {CONF_WIN_SYSTEM, Icon_Conf_CheckSBcoll},\

#define ICON_LIST_SYS64 \
  {CONF_WIN_SYSTEM, Icon_Conf_CharGen}, {CONF_WIN_SYSTEM, Icon_Conf_Kernal},\
  {CONF_WIN_SYSTEM, Icon_Conf_Basic},

#define ICON_LIST_TRUE \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrv}, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSync}, \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSyncT}, \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt8}, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt8T}, \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle8}, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle8T},\
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar8}, \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt9}, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt9T}, \
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle9}, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle9T},\
  {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar9},


/* Config icons that are greyed out in some CBM machines */
static conf_icon_id conf_grey_x64[] = {
  ICON_LIST_PET
  ICON_LIST_VIC
  {CONF_WIN_DEVICES, Icon_Conf_ACIAD67},
  {0xff, 0xff}
};

static conf_icon_id conf_grey_x128[] = {
  ICON_LIST_CART64
  ICON_LIST_PET
  ICON_LIST_VIC
  {0xff, 0xff}
};

static conf_icon_id conf_grey_xvic[] = {
  ICON_LIST_CART64
  ICON_LIST_SYSTEM
  ICON_LIST_SID
  ICON_LIST_DEVICES
  ICON_LIST_PET
  {0xff, 0xff}
};

static conf_icon_id conf_grey_xpet[] = {
  ICON_LIST_CART64
  ICON_LIST_VIC
  ICON_LIST_SYS64
  ICON_LIST_SYSTEM
  ICON_LIST_DEVICES
  ICON_LIST_SID
  ICON_LIST_TRUE
  {0xff, 0xff}
};

static conf_icon_id conf_grey_xcbm2[] = {
  ICON_LIST_CART64
  ICON_LIST_VIC
  ICON_LIST_SYSTEM
  ICON_LIST_PET
  ICON_LIST_VIC
  ICON_LIST_DEVRSUSR
  {0xff, 0xff}
};



/* Configuration options */
int PollEvery;
int SpeedEvery;
int SpeedLimit;
int AutoPauseEmu;
int DriveType8;
int DriveType9;
int DriveType10;
int DriveType11;
char *DriveFile8 = NULL;
char *DriveFile9 = NULL;
char *DriveFile10 = NULL;
char *DriveFile11 = NULL;
char *TapeFile = NULL;

static int *DriveTypes[] = {
  &DriveType8, &DriveType9, &DriveType10, &DriveType11
};

static char **DriveFiles[] = {
  &DriveFile8, &DriveFile9, &DriveFile10, &DriveFile11
};




/* Logging */
log_t roui_log = LOG_ERR;

int EmuZoom;
int LastPoll;
int LastSpeed;
int LastFrame;
int FrameCS;
int NumberOfFrames = 0;
int RelativeSpeed = 100;
int EmuPaused;
int SingleTasking = 0;
char *PetModelName = NULL;
char *CBM2ModelName = NULL;
char *ROMSetName = NULL;
char *ROMSetArchiveFile = NULL;

char ROMSetItemFile[256];


/* The screen */
RO_Screen ScreenMode;
int UseEigen;


/* Previous owner of caret */
RO_Caret LastCaret;


/* The windows */
RO_Window *EmuWindow;
RO_Window *EmuPane;
RO_Window *InfoWindow;
RO_Window *SnapshotWindow;
RO_Window *CpuJamWindow;
RO_Window *SaveBox;
RO_Window *ImgContWindow;
RO_Window *ConfWindows[CONF_WIN_NUMBER];

#define TitleBarOffset	40
RO_Window *ConfWinPositions[CONF_WIN_NUMBER];




/* Symbols and error messages for translation */
enum SymbolInstances {
  Symbol_Version,
  Symbol_Date,
  Symbol_Zoom1,
  Symbol_Zoom2,
  Symbol_Pause,
  Symbol_Resume,
  Symbol_ACIAD7,
  Symbol_PaneSpd,
  Symbol_PaneFPS,
  Symbol_Purpose,
  Symbol_MachDown,
  Symbol_DlgExtend,
  Symbol_ErrTemp,
  Symbol_ErrSMem,
  Symbol_ErrSpr,
  Symbol_ErrIcon,
  Symbol_ErrTFile,
  Symbol_ErrSave,
  Symbol_ErrLoad,
  Symbol_NumSymbols
};

static char *SymbolStrings[] = {
  "\\ViceVersion",
  "\\ViceDate",
  "\\PaneZoom1",
  "\\PaneZoom2",
  "\\PanePause",
  "\\PaneResume",
  "\\DevACIAD7",
  "\\PaneFmtSpeed",
  "\\PaneFmtFps",
  "\\MsgPurpFmt",
  "\\MsgMachDown",
  "\\DlgExtendImage",
  "\\ErrTemplateFmt",
  "\\ErrSpriteMem",
  "\\ErrSpriteFmt",
  "\\ErrIconFmt",
  "\\ErrTempFileFmt",
  "\\ErrSaveFmt",
  "\\ErrLoadFmt",
  NULL
};





/* Lookup table internal keynumbers to descriptive strings */
static char *IntKeyToString[128] = {
  "Shft", "Ctrl", "Alt", "ShftL",
  "CtrlL", "AltL", "ShftR", "CtrlR",
  "AltR", "Slct", "Menu", "Adjst",
  NULL, NULL, NULL, NULL,
  "q", "3", "4", "5",
  "F4", "8", "F7", "-",
  "6", "Left", "num6", "num7",
  "F11", "F12", "F10", "ScLck",
  "Prnt", "w", "e", "t",
  "7", "i", "9", "0",
  "-", "Down", "num8", "num9",
  "Brk", "`", "£", "Del",
  "1", "2", "d", "r",
  "6", "u", "o", "p",
  "[", "Up", "num+", "num-",
  "nmEnt", "Isrt", "Home", "PgUp",
  "CpLck", "a", "x", "f",
  "y", "j", "k", "2",
  ";", "Ret", "num/", NULL,
  "num.", "nmLck", "PgDwn", "\'",
  NULL, "s", "c", "g",
  "h", "n", "l", ";",
  "]", "Del", "num#", "num*",
  NULL, "=", "Extra", NULL,
  "Tab", "z", "Space", "v",
  "b", "m", ",", ".",
  "/", "Copy", "num0", "num1",
  "num3", NULL, NULL, NULL,
  "Esc", "F1", "F2", "F3",
  "F5", "F6", "F8", "F9",
  "\\", "Right", "num4", "num5",
  "num2", NULL, NULL, NULL
};

static Joy_Keys JoyToIcon[2] = {
  {Icon_Conf_JoyKey1U, Icon_Conf_JoyKey1D, Icon_Conf_JoyKey1L, Icon_Conf_JoyKey1R, Icon_Conf_JoyKey1F},
  {Icon_Conf_JoyKey2U, Icon_Conf_JoyKey2D, Icon_Conf_JoyKey2L, Icon_Conf_JoyKey2R, Icon_Conf_JoyKey2F}
};




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



/* Configuration menu */
#define Menu_Config_Items	8
#define Menu_Config_Width	200
#define Menu_Config_Drives	0
#define Menu_Config_Devices	1
#define Menu_Config_Sound	2
#define Menu_Config_System	3
#define Menu_Config_Joystick	4
#define Menu_Config_Machine	5
#define Menu_Config_Save	6
#define Menu_Config_Reload	7
static struct MenuConfigure {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Config_Items];
} MenuConfigure = {
  MENU_HEADER("\\MenConfTit", Menu_Config_Width),
  {
    MENU_ITEM("\\MenConfDrv"),
    MENU_ITEM("\\MenConfDev"),
    MENU_ITEM("\\MenConfSnd"),
    MENU_ITEM("\\MenConfSys"),
    MENU_ITEM("\\MenConfJoy"),
    {MFlg_Dotted, (RO_MenuHead*)-1, Menu_Flags, {"\\MenConfMch"}},
    MENU_ITEM("\\MenConfSav"),
    MENU_ITEM_LAST("\\MenConfRel")
  }
};

/* Icon bar menu */
#define Menu_IBar_Items		3
#define Menu_IBar_Width		200
#define Menu_IBar_Info		0
#define Menu_IBar_Configure	1
#define Menu_IBar_Quit		2
static struct MenuIconBar {
  RO_MenuHead head;
  RO_MenuItem item[Menu_IBar_Items];
} MenuIconBar = {
  MENU_HEADER("foo", Menu_IBar_Width),
  {
    MENU_ITEM("\\MenIBInfo"),
    MENU_ITEM_SUB("\\MenIBConf", &MenuConfigure),
    MENU_ITEM_LAST("\\MenIBQuit")
  }
};

/* Emu window menu */
#define Menu_EmuWin_Items	7
#define Menu_EmuWin_Width	200
#define Menu_EmuWin_Configure	0
#define Menu_EmuWin_Snapshot	1
#define Menu_EmuWin_Freeze	2
#define Menu_EmuWin_Pane	3
#define Menu_EmuWin_TrueDrvEmu	4
#define Menu_EmuWin_Sound	5
#define Menu_EmuWin_Monitor	6
static struct MenuEmuWindow {
  RO_MenuHead head;
  RO_MenuItem item[Menu_EmuWin_Items];
} MenuEmuWindow = {
  MENU_HEADER("foo", Menu_EmuWin_Width),
  {
    MENU_ITEM_SUB("\\MenEmuConf", &MenuConfigure),
    MENU_ITEM("\\MenEmuSnap"),
    MENU_ITEM("\\MenEmuFrz"),
    MENU_ITEM("\\MenEmuPane"),
    MENU_ITEM("\\MenEmuTrue"),
    MENU_ITEM("\\MenEmuSnd"),
    MENU_ITEM_LAST("\\MenEmuMon"),
  }
};



/* Sprite name copied in by the ui init function of the machine */
static RO_IconDesc IBarIcon = {
  -1, 0, 0, 68, 68, 0x301a,
  {""},
  0
};




/* Configuration issues... */
#define CONFIG_INT	1
#define CONFIG_SELECT	2
#define CONFIG_STRING	3
#define CONFIG_DRAG	4


/* Drive types */
#define DRIVE_TYPE_DISK	0
#define DRIVE_TYPE_FS	1


/* Resource names */
static char Rsrc_Prnt4[] = "Printer4";
static char Rsrc_Prnt4Dev[] = "Printer4Dev";
static char Rsrc_PrUsr[] = "PrUser";
static char Rsrc_PrUsrDev[] = "PrUserDev";
static char Rsrc_Sound[] = "Sound";
static char Rsrc_SndRate[] = "SoundSampleRate";
static char Rsrc_SndDev[] = "SoundDeviceName";
static char Rsrc_SndOver[] = "SoundOversample";
static char Rsrc_SndBuff[] = "SoundBufferSize";
static char Rsrc_JoyDev1[] = "JoyDevice1";
static char Rsrc_JoyDev2[] = "JoyDevice2";
static char Rsrc_True[] = "DriveTrueEmulation";
static char Rsrc_TruePar8[] = "Drive8ParallelCable";
static char Rsrc_TruePar9[] = "Drive9ParallelCable";
static char Rsrc_TrueExImg8[] = "Drive8ExtendImagePolicy";
static char Rsrc_TrueExImg9[] = "Drive9ExtendImagePolicy";
static char Rsrc_TrueIdle8[] = "Drive8IdleMethod";
static char Rsrc_TrueIdle9[] = "Drive9IdleMethod";
static char Rsrc_TrueType8[] = "Drive8Type";
static char Rsrc_TrueType9[] = "Drive9Type";
static char Rsrc_TrueSync[] = "DriveSyncFactor";
static char Rsrc_Dos1541[] = "DosName1541";
static char Rsrc_Dos1571[] = "DosName1571";
static char Rsrc_Dos1581[] = "DosName1581";
static char Rsrc_Dos2031[] = "DosName2031";
static char Rsrc_Poll[] = "PollEvery";
static char Rsrc_Speed[] = "SpeedEvery";
static char Rsrc_SndEvery[] = "SoundEvery";
static char Rsrc_AutoPause[] = "AutoPause";
static char Rsrc_SpeedLimit[] = "SpeedLimit";
static char Rsrc_Refresh[] = "RefreshRate";
static char Rsrc_WarpMode[] = "WarpMode";
static char Rsrc_DriveT8[] = "DriveType8";
static char Rsrc_DriveT9[] = "DriveType9";
static char Rsrc_DriveT10[] = "DriveType10";
static char Rsrc_DriveT11[] = "DriveType11";
static char Rsrc_DriveF8[] = "DriveFile8";
static char Rsrc_DriveF9[] = "DriveFile9";
static char Rsrc_DriveF10[] = "DriveFile10";
static char Rsrc_DriveF11[] = "DriveFile11";
static char Rsrc_TapeFile[] = "TapeFile";
static char Rsrc_Conv8P00[] = "FSDevice8ConvertP00";
static char Rsrc_Conv9P00[] = "FSDevice9ConvertP00";
static char Rsrc_Conv10P00[] = "FSDevice10ConvertP00";
static char Rsrc_Conv11P00[] = "FSDevice11ConvertP00";
static char Rsrc_Save8P00[] = "FSDevice8SaveP00";
static char Rsrc_Save9P00[] = "FSDevice9SaveP00";
static char Rsrc_Save10P00[] = "FSDevice10SaveP00";
static char Rsrc_Save11P00[] = "FSDevice11SaveP00";
static char Rsrc_Hide8CBM[] = "FSDevice8HideCBMFiles";
static char Rsrc_Hide9CBM[] = "FSDevice9HideCBMFiles";
static char Rsrc_Hide10CBM[] = "FSDevice10HideCBMFiles";
static char Rsrc_Hide11CBM[] = "FSDevice11HideCBMFiles";
static char Rsrc_AciaDE[] = "AciaDE";
static char Rsrc_ACIAD6[] = "AciaD6";
static char Rsrc_ACIAD7[] = "AciaD7";
static char Rsrc_Serial[] = "SerialBaud";
static char Rsrc_RsUsr[] = "RsUser";
static char Rsrc_RsUsrDev[] = "RsUserDev";
static char Rsrc_AciaIrq[] = "Acia1Irq";
static char Rsrc_AciaDev[] = "Acia1Dev";
static char Rsrc_SidFilt[] = "SidFilters";
static char Rsrc_ReSid[] = "SidUseResid";
static char Rsrc_SidMod[] = "SidModel";
static char Rsrc_CharGen[] = "CharGenName";
static char Rsrc_Kernal[] = "KernalName";
static char Rsrc_Basic[] = "BasicName";
static char Rsrc_REU[] = "REU";
static char Rsrc_IEEE[] = "IEEE488";
static char Rsrc_EmuID[] = "EmuID";
static char Rsrc_CartT[] = "CartridgeType";
static char Rsrc_CartF[] = "CartridgeFile";
static char Rsrc_SScoll[] = "CheckSsColl";
static char Rsrc_SBcoll[] = "CheckSbColl";
static char Rsrc_Palette[] = "PaletteFile";
static char Rsrc_NoTraps[] = "NoTraps";
static char Rsrc_VideoCache[] = "VideoCache";
static char Rsrc_SoundFile[] = "SoundDeviceArg";
static char Rsrc_SerialFile[] = "SerialFile";
static char Rsrc_PrinterFile[] = "PrinterFile";
static char Rsrc_PetMem[] = "RamSize";
static char Rsrc_PetIO[] = "IOSize";
static char Rsrc_PetVideo[] = "VideoSize";
static char Rsrc_PetModel[] = "Model";
static char Rsrc_PetCrt[] = "Crtc";
static char Rsrc_PetRAM9[] = "Ram9";
static char Rsrc_PetRAMA[] = "RamA";
static char Rsrc_PetDiag[] = "DiagPin";
static char Rsrc_PetSuper[] = "SuperPET";
static char Rsrc_VicCart2[] = "CartridgeFile2000";
static char Rsrc_VicCart6[] = "CartridgeFile6000";
static char Rsrc_VicCartA[] = "CartridgeFileA000";
static char Rsrc_VicCartB[] = "CartridgeFileB000";
static char Rsrc_VicRam0[] = "RamBlock0";
static char Rsrc_VicRam1[] = "RamBlock1";
static char Rsrc_VicRam2[] = "RamBlock2";
static char Rsrc_VicRam3[] = "RamBlock3";
static char Rsrc_VicRam5[] = "RamBlock5";
static char Rsrc_C2Cart1[] = "Cart1Name";
static char Rsrc_C2Cart2[] = "Cart2Name";
static char Rsrc_C2Cart4[] = "Cart4Name";
static char Rsrc_C2Cart6[] = "Cart6Name";
static char Rsrc_C2RAM08[] = "Ram08";
static char Rsrc_C2RAM1[] = "Ram1";
static char Rsrc_C2RAM2[] = "Ram2";
static char Rsrc_C2RAM4[] = "Ram4";
static char Rsrc_C2RAM6[] = "Ram6";
static char Rsrc_C2RAMC[] = "RamC";
static char Rsrc_C2Line[] = "ModelLine";
static char Rsrc_C2Mem[] = "RamSize";

/*static char Rsrc_PetKeymap[] = "KeymapIndex";*/

/*static char *Rsrc_DriveTypes[4] = {
  Rsrc_DriveT8, Rsrc_DriveT9, Rsrc_DriveT10, Rsrc_DriveT11
};
static char *Rsrc_DriveFiles[4] = {
  Rsrc_DriveF8, Rsrc_DriveF9, Rsrc_DriveF10, Rsrc_DriveF11
};
static char *Rsrc_VicCartridge[4] = {
  Rsrc_VicCart2, Rsrc_VicCart6, Rsrc_VicCartA, Rsrc_VicCartB
};*/
static char *Rsrc_ConvP00[4] = {
  Rsrc_Conv8P00, Rsrc_Conv9P00, Rsrc_Conv10P00, Rsrc_Conv11P00
};
static char *Rsrc_SaveP00[4] = {
  Rsrc_Save8P00, Rsrc_Save9P00, Rsrc_Save10P00, Rsrc_Save11P00
};
static char *Rsrc_HideCBM[4] = {
  Rsrc_Hide8CBM, Rsrc_Hide9CBM, Rsrc_Hide10CBM, Rsrc_Hide11CBM
};




typedef struct {
  char *resource;
  unsigned char ctype;
  conf_icon_id id;
} config_item;

typedef struct {
  RO_MenuHead *menu;
  char *resource;
  conf_icon_id id;
} menu_icon;



#define Menu_PrntDev_Items	3
#define Menu_PrntDev_Width	200
static struct MenuPrintDev {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PrntDev_Items];
} MenuPrintDev = {
  MENU_HEADER("\\MenPrtTit", Menu_PrntDev_Width),
  {
    MENU_ITEM("\\MenPrtFile"),
    MENU_ITEM("\\MenPrtPar"),
    MENU_ITEM_LAST("\\MenPrtSer")
  }
};

#define Menu_UserDev_Items	3
#define Menu_UserDev_Width	200
static struct MenuUserDev {
  RO_MenuHead head;
  RO_MenuItem item[Menu_UserDev_Items];
} MenuUserDev = {
  MENU_HEADER("\\MenUsrTit", Menu_UserDev_Width),
  {
    MENU_ITEM("\\MenUsrFile"),
    MENU_ITEM("\\MenUsrPar"),
    MENU_ITEM_LAST("\\MenUsrSer")
  }
};

#define Menu_SampRate_Items	5
#define Menu_SampRate_Width	200
#define Menu_SampRate_8k	0
#define Menu_SampRate_11k	1
#define Menu_SampRate_22k	2
#define Menu_SampRate_44k	3
#define Menu_SampRate_48k	4
static struct MenuSampleRate {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SampRate_Items];
} MenuSampleRate = {
  MENU_HEADER("\\MenSampTit", Menu_SampRate_Width),
  {
    MENU_ITEM("\\MenSamp8"),
    MENU_ITEM("\\MenSamp11"),
    MENU_ITEM("\\MenSamp22"),
    MENU_ITEM("\\MenSamp44"),
    MENU_ITEM_LAST("\\MenSamp48")
  }
};

#define Menu_SoundDev_Items	5
#define Menu_SoundDev_Width	200
#define Menu_SoundDev_VIDC	0
#define Menu_SoundDev_Dummy	1
#define Menu_SoundDev_FS	2
#define Menu_SoundDev_Speed	3
#define Menu_SoundDev_Dump	4
static struct MenuSoundDevice {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SoundDev_Items];
} MenuSoundDevice = {
  MENU_HEADER("\\MenSndTit", Menu_SoundDev_Width),
  {
    MENU_ITEM("\\MenSndVidc"),
    MENU_ITEM("\\MenSndDmy"),
    MENU_ITEM("\\MenSndFS"),
    MENU_ITEM("\\MenSndSpd"),
    MENU_ITEM_LAST("\\MenSndDmp")
  }
};

#define Menu_SoundOver_Items	4
#define Menu_SoundOver_Width	200
#define Menu_SoundOver_1	0
#define Menu_SoundOver_2	1
#define Menu_SoundOver_4	2
#define Menu_SoundOver_8	3
static struct MenuSoundOver {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SoundOver_Items];
} MenuSoundOver = {
  MENU_HEADER("\\MenOverTit", Menu_SoundOver_Width),
  {
    MENU_ITEM("\\MenOver1"),
    MENU_ITEM("\\MenOver2"),
    MENU_ITEM("\\MenOver4"),
    MENU_ITEM_LAST("\\MenOver8")
  }
};

#define Menu_SoundBuffer_Items	8
#define Menu_SoundBuffer_Width	200
static struct MenuSoundBuffer {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SoundBuffer_Items];
} MenuSoundBuffer = {
  MENU_HEADER("\\MenSBfTit", Menu_SoundBuffer_Width),
  {
    MENU_ITEM("\\MenSBf2"),
    MENU_ITEM("\\MenSBf4"),
    MENU_ITEM("\\MenSBf6"),
    MENU_ITEM("\\MenSBf8"),
    MENU_ITEM("\\MenSBf10"),
    MENU_ITEM("\\MenSBf20"),
    MENU_ITEM("\\MenSBf35"),
    MENU_ITEM_LAST("\\MenSBf50")
  }
};

#define Menu_TrueExtend_Items	3
#define Menu_TrueExtend_Width	200
#define Menu_TrueExtend_Never	0
#define Menu_TrueExtend_Ask	1
#define Menu_TrueExtend_Access	2
#define Menu_TrueIdle_Items	3
#define Menu_TrueIdle_Width	200
#define Menu_TrueIdle_NoTraps	0
#define Menu_TrueIdle_SkipC	1
#define Menu_TrueIdle_Trap	2
#define Menu_TrueType_Items	6
#define Menu_TrueType_Width	200
#define Menu_TrueType_None	0
#define Menu_TrueType_1541	1
#define Menu_TrueType_1541II	2
#define Menu_TrueType_1571	3
#define Menu_TrueType_1581	4
#define Menu_TrueType_2031	5

#define TRUE_DRIVE_EXTEND_MENU(name, title) \
  static struct name { \
    RO_MenuHead head; \
    RO_MenuItem item[Menu_TrueExtend_Items]; \
  } name = { \
    MENU_HEADER(title, Menu_TrueExtend_Width), \
    { \
      MENU_ITEM("\\MenExtNvr"), \
      MENU_ITEM("\\MenExtAsk"), \
      MENU_ITEM_LAST("\\MenExtXss") \
    } \
  };

#define TRUE_DRIVE_IDLE_MENU(name, title) \
  static struct name { \
    RO_MenuHead head; \
    RO_MenuItem item[Menu_TrueIdle_Items]; \
  } name = { \
    MENU_HEADER(title, Menu_TrueIdle_Width), \
    { \
      MENU_ITEM("\\MenIdlNoT"), \
      MENU_ITEM("\\MenIdlSkp"), \
      MENU_ITEM_LAST("\\MenIdlTrp") \
    } \
  };

#define TRUE_DRIVE_TYPE_MENU(name, title) \
  static struct name { \
    RO_MenuHead head; \
    RO_MenuItem item[Menu_TrueType_Items]; \
  } name = { \
    MENU_HEADER(title, Menu_TrueType_Width), \
    { \
      MENU_ITEM("\\MenDtpNone"), \
      MENU_ITEM("\\MenDtp1541"), \
      MENU_ITEM("\\MenDtp15412"), \
      MENU_ITEM("\\MenDtp1571"), \
      MENU_ITEM("\\MenDtp1581"), \
      MENU_ITEM_LAST("\\MenDtp2031") \
    } \
  };

TRUE_DRIVE_EXTEND_MENU(MenuTrueExtend8, "\\MenExt8Tit")
TRUE_DRIVE_IDLE_MENU(MenuTrueIdle8, "\\MenIdl8Tit")
TRUE_DRIVE_TYPE_MENU(MenuTrueType8, "\\MenDtp8Tit")
TRUE_DRIVE_EXTEND_MENU(MenuTrueExtend9, "\\MenExt9Tit")
TRUE_DRIVE_IDLE_MENU(MenuTrueIdle9, "\\MenIdl9Tit")
TRUE_DRIVE_TYPE_MENU(MenuTrueType9, "\\MenDtp9Tit")


#define Menu_TrueSync_Items	3
#define Menu_TrueSync_Width	200
#define Menu_TrueSync_PAL	0
#define Menu_TrueSync_NTSC	1
#define Menu_TrueSync_Custom	2

static char TrueSyncCustomField[16];

static struct MenuTrueSync {
  RO_MenuHead head;
  RO_MenuItem item[Menu_TrueSync_Items];
} MenuTrueSync = {
  MENU_HEADER("\\MenSncTit", Menu_TrueSync_Width),
  {
    MENU_ITEM("\\MenSncPAL"),
    MENU_ITEM("\\MenSncNTSC"),
    /* This menu item will be made indirected */
    {MFlg_LastItem | 4, (RO_MenuHead*)-1, Menu_Flags, {""}}
  }
};

/* drive type submenus */

#define Menu_DriveDisk_Items	1
#define Menu_DriveDisk_Width	200
#define Menu_DriveDisk_Detach	0
static struct MenuDriveDisk {
  RO_MenuHead head;
  RO_MenuItem item[Menu_DriveDisk_Items];
} MenuDriveDisk = {
  MENU_HEADER("\\MenDskTit", Menu_DriveDisk_Width),
  {
    MENU_ITEM_LAST("\\MenDskDet")
  }
};

#define Menu_DriveFS_Items	3
#define Menu_DriveFS_Width	200
#define Menu_DriveFS_ConvP00	0
#define Menu_DriveFS_SaveP00	1
#define Menu_DriveFS_HideCBM	2
static struct MenuDriveFS {
  RO_MenuHead head;
  RO_MenuItem item[Menu_DriveFS_Items];
} MenuDriveFS = {
  MENU_HEADER("\\MenFSTit", Menu_DriveFS_Width),
  {
    MENU_ITEM("\\MenFSCP00"),
    MENU_ITEM("\\MenFSSP00"),
    MENU_ITEM_LAST("\\MenFSHide")
  }
};

#define Menu_DriveType_Items	2
#define Menu_DriveType_Width	200
#define Menu_DriveType_Disk	0
#define Menu_DriveType_FS	1
static struct MenuDriveType {
  RO_MenuHead head;
  RO_MenuItem items[Menu_DriveType_Items];
} MenuDriveType = {
  MENU_HEADER("\\MenTypTit", Menu_DriveType_Width),
  {
    MENU_ITEM_SUB("\\MenTypDsk", &MenuDriveDisk),
    MENU_ITEM_SUBLAST("\\MenTypFS", &MenuDriveFS)
  }
};

#define Menu_RsUsrDev_Items	3
#define Menu_RsUsrDev_Width	200
static struct MenuRsUserDevice {
  RO_MenuHead head;
  RO_MenuItem item[Menu_RsUsrDev_Items];
} MenuRsUserDevice = {
  MENU_HEADER("\\MenRsUTit", Menu_RsUsrDev_Width),
  {
    MENU_ITEM("\\MenRsUFile"),
    MENU_ITEM("\\MenRsUPar"),
    MENU_ITEM_LAST("\\MenRsUSer")
  }
};

#define Menu_AciaDev_Items	3
#define Menu_AciaDev_Width	200
static struct MenuAciaDevice {
  RO_MenuHead head;
  RO_MenuItem item[Menu_AciaDev_Items];
} MenuAciaDevice = {
  MENU_HEADER("\\MenAciaTit", Menu_AciaDev_Width),
  {
    MENU_ITEM("\\MenAciaFile"),
    MENU_ITEM("\\MenAciaPar"),
    MENU_ITEM_LAST("\\MenAciaSer")
  }
};

#define Menu_SidModel_Items	2
#define Menu_SidModel_Width	200
#define Menu_SidModel_6581	0
#define Menu_SidModel_8500	1
static struct MenuSidModel {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SidModel_Items];
} MenuSidModel = {
  MENU_HEADER("\\MenSidTit", Menu_SidModel_Width),
  {
    MENU_ITEM("\\MenSid6581"),
    MENU_ITEM_LAST("\\MenSid8500")
  }
};

#define Menu_SpeedLimit_Items	6
#define Menu_SpeedLimit_Width	200
static struct MenuSpeedLimit {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SpeedLimit_Items];
} MenuSpeedLimit = {
  MENU_HEADER("\\MenSLmTit", Menu_SpeedLimit_Width),
  {
    MENU_ITEM("\\MenSLm200"),
    MENU_ITEM("\\MenSLm100"),
    MENU_ITEM("\\MenSLm50"),
    MENU_ITEM("\\MenSLm20"),
    MENU_ITEM("\\MenSLm10"),
    MENU_ITEM_LAST("\\MenSLm0")
  }
};

#define Menu_Refresh_Items	11
#define Menu_Refresh_Width	200
static struct MenuRefresh {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Refresh_Items];
} MenuRefresh = {
  MENU_HEADER("\\MenRefTit", Menu_Refresh_Width),
  {
    MENU_ITEM("\\MenRefAuto"),
    MENU_ITEM("\\MenRef1"),
    MENU_ITEM("\\MenRef2"),
    MENU_ITEM("\\MenRef3"),
    MENU_ITEM("\\MenRef4"),
    MENU_ITEM("\\MenRef5"),
    MENU_ITEM("\\MenRef6"),
    MENU_ITEM("\\MenRef7"),
    MENU_ITEM("\\MenRef8"),
    MENU_ITEM("\\MenRef9"),
    MENU_ITEM_LAST("\\MenRef10")
  }
};

#define Menu_Serial_Items	15
#define Menu_Serial_Width	100
static struct MenuSerialBaud {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Serial_Items];
} MenuSerialBaud = {
  MENU_HEADER("\\MenSerTit", Menu_Serial_Width),
  {
    MENU_ITEM("50"),
    MENU_ITEM("75"),
    MENU_ITEM("110"),
    MENU_ITEM("134.5"),
    MENU_ITEM("150"),
    MENU_ITEM("300"),
    MENU_ITEM("600"),
    MENU_ITEM("1200"),
    MENU_ITEM("1800"),
    MENU_ITEM("2400"),
    MENU_ITEM("3600"),
    MENU_ITEM("4800"),
    MENU_ITEM("7200"),
    MENU_ITEM("9600"),
    MENU_ITEM_LAST("19200")
  }
};

#define Menu_Cartridge_Items	13
#define Menu_Cartridge_Width	200
static struct MenuCartridgeType {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Cartridge_Items];
} MenuCartridgeType = {
  MENU_HEADER("\\MenCrtTit", Menu_Cartridge_Width),
  {
    MENU_ITEM("\\MenCrtNone"),
    MENU_ITEM("\\MenCrtG8"),
    MENU_ITEM("\\MenCrtG16"),
    MENU_ITEM("\\MenCrtCRT"),
    MENU_ITEM("\\MenCrtAct"),
    MENU_ITEM("\\MenCrtKCS"),
    MENU_ITEM("\\MenCrtSim"),
    MENU_ITEM("\\MenCrtUlt"),
    MENU_ITEM("\\MenCrtSSn"),
    MENU_ITEM("\\MenCrtFin"),
    MENU_ITEM("\\MenCrtOcn"),
    MENU_ITEM("\\MenCrtFun"),
    MENU_ITEM_LAST("\\MenCrtSGm")
  }
};

#define Menu_PetMemory_Items	6
#define Menu_PetMemory_Width	200
static struct MenuPetMemory {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PetMemory_Items];
} MenuPetMemory = {
  MENU_HEADER("\\MenPMyTit", Menu_PetMemory_Width),
  {
    MENU_ITEM("\\MenPMy4"),
    MENU_ITEM("\\MenPMy8"),
    MENU_ITEM("\\MenPMy16"),
    MENU_ITEM("\\MenPMy32"),
    MENU_ITEM("\\MenPMy96"),
    MENU_ITEM_LAST("\\MenPMy128")
  }
};

#define Menu_PetIO_Items	2
#define Menu_PetIO_Width	200
static struct MenuPetIO {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PetIO_Items];
} MenuPetIO = {
  MENU_HEADER("\\MenPIOTit", Menu_PetIO_Width),
  {
    MENU_ITEM("\\MenPIO2"),
    MENU_ITEM_LAST("\\MenPIO256")
  }
};

#define Menu_PetVideo_Items	3
#define Menu_PetVideo_Width	200
static struct MenuPetVideo {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PetVideo_Items];
} MenuPetVideo = {
  MENU_HEADER("\\MenPVdTit", Menu_PetVideo_Width),
  {
    MENU_ITEM("\\MenPVdAuto"),
    MENU_ITEM("\\MenPVd40"),
    MENU_ITEM_LAST("\\MenPVd80")
  }
};

#define Menu_PetModel_Items	12
#define Menu_PetModel_Width	200
static struct MenuPetModel {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PetModel_Items];
} MenuPetModel = {
  MENU_HEADER("\\MenPMdTit", Menu_PetModel_Width),
  {
    MENU_ITEM("\\MenPMd201"),
    MENU_ITEM("\\MenPMd308"),
    MENU_ITEM("\\MenPMd316"),
    MENU_ITEM("\\MenPMd332"),
    MENU_ITEM("\\MenPMd332B"),
    MENU_ITEM("\\MenPMd416"),
    MENU_ITEM("\\MenPMd432"),
    MENU_ITEM("\\MenPMd432B"),
    MENU_ITEM("\\MenPMd832"),
    MENU_ITEM("\\MenPMd896"),
    MENU_ITEM("\\MenPMd8296"),
    MENU_ITEM_LAST("\\MenPMdSup")
  }
};

#define Menu_VicRam_Items	5
#define Menu_VicRam_Width	200
static struct MenuVicRam {
  RO_MenuHead head;
  RO_MenuItem item[Menu_VicRam_Items];
} MenuVicRam = {
  MENU_HEADER("\\MenVRmTit", Menu_VicRam_Width),
  {
    MENU_ITEM("\\MenVRm04"),
    MENU_ITEM("\\MenVRm20"),
    MENU_ITEM("\\MenVRm40"),
    MENU_ITEM("\\MenVRm60"),
    MENU_ITEM_LAST("\\MenVRmA0")
  }
};

#define Menu_VicCart_Items	4
#define Menu_VicCart_Width	200
static struct MenuVicCartridge {
  RO_MenuHead head;
  RO_MenuItem item[Menu_VicCart_Items];
} MenuVicCartridge = {
  MENU_HEADER("\\MenVCtTit", Menu_VicCart_Width),
  {
    MENU_ITEM("\\MenVCt2"),
    MENU_ITEM("\\MenVCt6"),
    MENU_ITEM("\\MenVCtA"),
    MENU_ITEM_LAST("\\MenVCtB")
  }
};

#define Menu_DosName_Items	4
#define Menu_DosName_Width	200
static struct MenuDosName {
  RO_MenuHead head;
  RO_MenuItem item[Menu_DosName_Items];
} MenuDosName = {
  MENU_HEADER("\\MenDOSTit", Menu_DosName_Width),
  {
    MENU_ITEM("\\MenDOS1541"),
    MENU_ITEM("\\MenDOS1571"),
    MENU_ITEM("\\MenDOS1581"),
    MENU_ITEM_LAST("\\MenDOS2031")
  }
};

#define Menu_CBM2Line_Items	3
#define Menu_CBM2Line_Width	200
static struct MenuCBM2Line {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2Line_Items];
} MenuCBM2Line = {
  MENU_HEADER("\\MenC2MlTit", Menu_CBM2Line_Width),
  {
    MENU_ITEM("\\MenC2Ml75"),
    MENU_ITEM("\\MenC2Ml66"),
    MENU_ITEM_LAST("\\MenC2Ml65")
  }
};

#define Menu_CBM2Mem_Items	4
#define Menu_CBM2Mem_Width	200
static struct MenuCBM2Memory {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2Mem_Items];
} MenuCBM2Memory = {
  MENU_HEADER("\\MenC2MyTit", Menu_CBM2Mem_Width),
  {
    MENU_ITEM("\\MenC2My128"),
    MENU_ITEM("\\MenC2My256"),
    MENU_ITEM("\\MenC2My512"),
    MENU_ITEM_LAST("\\MenC2My1M")
  }
};

#define Menu_CBM2Model_Items	6
#define Menu_CBM2Model_Width	200
static struct MenuCBM2Model {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2Model_Items];
} MenuCBM2Model = {
  MENU_HEADER("\\MenC2MdTit", Menu_CBM2Model_Width),
  {
    MENU_ITEM("\\MenC2Md61"),
    MENU_ITEM("\\MenC2Md62"),
    MENU_ITEM("\\MenC2Md62P"),
    MENU_ITEM("\\MenC2Md71"),
    MENU_ITEM("\\MenC2Md72"),
    MENU_ITEM_LAST("\\MenC2Md72P")
  }
};

#define Menu_CBM2RAM_Items	6
#define Menu_CBM2RAM_Width	200
static struct MenuCBM2RAM {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2RAM_Items];
} MenuCBM2RAM = {
  MENU_HEADER("\\MenC2RTit", Menu_CBM2RAM_Width),
  {
    MENU_ITEM("\\MenC2R08"),
    MENU_ITEM("\\MenC2R1"),
    MENU_ITEM("\\MenC2R2"),
    MENU_ITEM("\\MenC2R4"),
    MENU_ITEM("\\MenC2R6"),
    MENU_ITEM_LAST("\\MenC2RC")
  }
};

#define Menu_CBM2Cart_Items	4
#define Menu_CBM2Cart_Width	200
static struct MenuCBM2Cartridge {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2Cart_Items];
} MenuCBM2Cartridge = {
  MENU_HEADER("\\MenC2CTit", Menu_CBM2Cart_Width),
  {
    MENU_ITEM("\\MenC2C1"),
    MENU_ITEM("\\MenC2C2"),
    MENU_ITEM("\\MenC2C4"),
    MENU_ITEM_LAST("\\MenC2C6")
  }
};


#define Menu_JoyDevice_Items	JOYDEV_NUMBER
#define Menu_JoyDevice_Width	200

#define JOYSTICK_DEVICE_MENU(name, title) \
  static struct name { \
    RO_MenuHead head; \
    RO_MenuItem item[Menu_JoyDevice_Items]; \
  } name = { \
    MENU_HEADER(title, Menu_JoyDevice_Width), \
    { \
      MENU_ITEM("\\MenJoyNone"), \
      MENU_ITEM("\\MenJoyKey1"), \
      MENU_ITEM("\\MenJoyKey2"), \
      MENU_ITEM("\\MenJoyJoy1"), \
      MENU_ITEM_LAST("\\MenJoyJoy2") \
    } \
  };

JOYSTICK_DEVICE_MENU(MenuJoyDevice1, "\\MenJoyTit1")
JOYSTICK_DEVICE_MENU(MenuJoyDevice2, "\\MenJoyTit2")


#define Menu_ROMSet_Width	200
RO_MenuHead *MenuROMSet = NULL;

static struct MenuROMSetTmpl {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuROMSetTmpl = {
  MENU_HEADER("\\MenRSTit", Menu_ROMSet_Width),
  {
    MENU_ITEM_LAST("\\MenRSDef")
  }
};

/* ROMset actions */
static char NewRomSetName[32];

static struct MenuRomActName {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuRomActName = {
  MENU_HEADER("\\MenRANT", 200),
  {
    /* Item will be made indirected in init routine */
    {MFlg_Writable | MFlg_LastItem, (RO_MenuHead*)-1, Menu_Flags, {""}}
  }
};

#define Menu_RomAct_Items	6
#define Menu_RomAct_Width	200
#define Menu_RomAct_Create	0
#define Menu_RomAct_Delete	1
#define Menu_RomAct_Save	2
#define Menu_RomAct_Dump	3
#define Menu_RomAct_Clear	4
#define Menu_RomAct_Restore	5
static struct MenuRomAction {
  RO_MenuHead head;
  RO_MenuItem item[Menu_RomAct_Items];
} MenuRomAction = {
  MENU_HEADER("\\MenRActT", Menu_RomAct_Width),
  {
    MENU_ITEM_SUB("\\MenRActCrt", &MenuRomActName),
    MENU_ITEM("\\MenRActDel"),
    {MFlg_Warning, (RO_MenuHead*)-1, Menu_Flags, {"\\MenRActSav"}},
    MENU_ITEM("\\MenRActDmp"),
    MENU_ITEM("\\MenRActClr"),
    MENU_ITEM_LAST("\\MenRActRst")
  }
};



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

/* Config Menus */
static menu_icon ConfigMenus[] = {
  {(RO_MenuHead*)&MenuPrintDev, Rsrc_Prnt4Dev,
    {CONF_WIN_DEVICES, Icon_Conf_PrntDev}},		/* 0 (prdevice.c) */
  {(RO_MenuHead*)&MenuUserDev, Rsrc_PrUsrDev,
    {CONF_WIN_DEVICES, Icon_Conf_PrntUsrDev}},		/* 1 (pruser.c) */
  {(RO_MenuHead*)&MenuSampleRate, Rsrc_SndRate,
    {CONF_WIN_SOUND, Icon_Conf_SampleRate}},		/* 2 (sound.c) */
  {(RO_MenuHead*)&MenuSoundDevice, Rsrc_SndDev,
    {CONF_WIN_SOUND, Icon_Conf_SoundDev}},		/* 3 (sound.c) */
  {(RO_MenuHead*)&MenuSoundOver, Rsrc_SndOver,
    {CONF_WIN_SOUND, Icon_Conf_Oversample}},		/* 4 (sound.c) */
  {(RO_MenuHead*)&MenuTrueSync, Rsrc_TrueSync,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSync}},		/* 5 (drive.c) */
  {(RO_MenuHead*)&MenuTrueIdle8, Rsrc_TrueIdle8,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle8}},		/* 6 */
  {(RO_MenuHead*)&MenuTrueExtend8, Rsrc_TrueExImg8,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt8}},		/* 7 */
  {(RO_MenuHead*)&MenuTrueType8, Rsrc_TrueType8,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvType8}},		/* 8 */
  {(RO_MenuHead*)&MenuTrueIdle9, Rsrc_TrueIdle9,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle9}},		/* 9 */
  {(RO_MenuHead*)&MenuTrueExtend9, Rsrc_TrueExImg9,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt9}},		/* 10 */
  {(RO_MenuHead*)&MenuTrueType9, Rsrc_TrueType9,
    {CONF_WIN_DRIVES, Icon_Conf_TrueDrvType9}},		/* 11 */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT8,
    {CONF_WIN_DRIVES, Icon_Conf_DriveType8}},		/* 12 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT9,
    {CONF_WIN_DRIVES, Icon_Conf_DriveType9}},		/* 13 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT10,
    {CONF_WIN_DRIVES, Icon_Conf_DriveType10}},		/* 14 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT11,
    {CONF_WIN_DRIVES, Icon_Conf_DriveType11}},		/* 15 (here) */
  {(RO_MenuHead*)&MenuCartridgeType, Rsrc_CartT,
    {CONF_WIN_SYSTEM, Icon_Conf_CartType}},		/* 16 (cartridge.c) */
  {(RO_MenuHead*)&MenuRsUserDevice, Rsrc_RsUsrDev,
    {CONF_WIN_DEVICES, Icon_Conf_RsUsrDev}},		/* 17 (rsuser.c) */
  {(RO_MenuHead*)&MenuAciaDevice, Rsrc_AciaDev,
    {CONF_WIN_DEVICES, Icon_Conf_ACIADev}},		/* 18 (c64acia.c) */
  {(RO_MenuHead*)&MenuSerialBaud, Rsrc_Serial,
    {CONF_WIN_DEVICES, Icon_Conf_Serial}},		/* 19 (serial.c) */
  {(RO_MenuHead*)&MenuSidModel, Rsrc_SidMod,
    {CONF_WIN_SOUND, Icon_Conf_SidModel}},		/* 20 (sid.c) */
  {(RO_MenuHead*)&MenuSpeedLimit, Rsrc_SpeedLimit,
    {CONF_WIN_SYSTEM, Icon_Conf_SpeedLmt}},		/* 21 (here) */
  {(RO_MenuHead*)&MenuRefresh, Rsrc_Refresh,
    {CONF_WIN_SYSTEM, Icon_Conf_Refresh}},		/* 22 (here) */
  {(RO_MenuHead*)&MenuPetMemory, Rsrc_PetMem,
    {CONF_WIN_PET, Icon_Conf_PetMem}},			/* 23 (pets.c) */
  {(RO_MenuHead*)&MenuPetIO, Rsrc_PetIO,
    {CONF_WIN_PET, Icon_Conf_PetIO}},			/* 24 */
  {(RO_MenuHead*)&MenuPetVideo, Rsrc_PetVideo,
    {CONF_WIN_PET, Icon_Conf_PetVideo}},		/* 25 */
  {(RO_MenuHead*)&MenuPetModel, Rsrc_PetModel,
    {CONF_WIN_PET, Icon_Conf_PetModel}},		/* 26 */
  {(RO_MenuHead*)&MenuVicRam, NULL,
    {CONF_WIN_VIC, Icon_Conf_VICMem}},			/* 27 */
  {(RO_MenuHead*)&MenuVicCartridge, NULL,
    {CONF_WIN_VIC, Icon_Conf_VICCart}},			/* 28 */
  {(RO_MenuHead*)&MenuDosName, NULL,
    {CONF_WIN_SYSTEM, Icon_Conf_DosName}},		/* 29 */
  {(RO_MenuHead*)&MenuCBM2Line, Rsrc_C2Line,
    {CONF_WIN_CBM2, Icon_Conf_CBM2Line}},		/* 30 */
  {(RO_MenuHead*)&MenuCBM2Memory, Rsrc_C2Mem,
    {CONF_WIN_CBM2, Icon_Conf_CBM2Mem}},		/* 31 */
  {(RO_MenuHead*)&MenuCBM2Model, NULL,
    {CONF_WIN_CBM2, Icon_Conf_CBM2Model}},		/* 32 */
  {(RO_MenuHead*)&MenuCBM2RAM, NULL,
    {CONF_WIN_CBM2, Icon_Conf_CBM2RAM}},		/* 33 */
  {(RO_MenuHead*)&MenuCBM2Cartridge, NULL,
    {CONF_WIN_CBM2, Icon_Conf_CBM2Cart}},		/* 34 */
  {(RO_MenuHead*)&MenuSoundBuffer, NULL,
    {CONF_WIN_SOUND, Icon_Conf_SoundBuff}},		/* 35 */
  {(RO_MenuHead*)&MenuJoyDevice1, NULL,
    {CONF_WIN_JOY, Icon_Conf_JoyPort1}},		/* 36 */
  {(RO_MenuHead*)&MenuJoyDevice2, NULL,
    {CONF_WIN_JOY, Icon_Conf_JoyPort2}},		/* 37 */
  {(RO_MenuHead*)&MenuROMSetTmpl, NULL,
    {CONF_WIN_SYSTEM, Icon_Conf_ROMSet}},		/* 38 */
  {(RO_MenuHead*)&MenuRomAction, NULL,
    {CONF_WIN_SYSTEM, Icon_Conf_ROMAction}},		/* 39 */
  {NULL, NULL, {0, 0}}
};

/* Config Icons */
static config_item Configurations[] = {
  {Rsrc_Prnt4, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_PrntOn}},	/* prdevice.c */
  {Rsrc_PrUsr, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_PrntUsrOn}},	/* pruser */
  {Rsrc_Sound, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_Conf_SoundOn}},	/* sound.c */
  {Rsrc_NoTraps, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_NoTraps}},	/* traps.c */
  {Rsrc_True, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_Conf_TrueDrv}},	/* drive.c */
  {Rsrc_TruePar8, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar8}},
  {Rsrc_TruePar9, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvPar9}},
  {Rsrc_Poll, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_Conf_PollEvery}},	/* right here */
  {Rsrc_Speed, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_Conf_SpeedEvery}},
  {Rsrc_SndEvery, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_Conf_SoundEvery}},
  {Rsrc_AutoPause, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_AutoPause}},
  {Rsrc_DriveF8, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_Conf_DriveFile8}},
  {Rsrc_DriveF9, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_Conf_DriveFile9}},
  {Rsrc_DriveF10, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_Conf_DriveFile10}},
  {Rsrc_DriveF11, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_Conf_DriveFile11}},
  {Rsrc_WarpMode, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_WarpMode}},
  {Rsrc_VideoCache, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_VideoCache}},
  {Rsrc_CharGen, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_CharGen}},	/* c64mem.c */
  {Rsrc_Kernal, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Kernal}},
  {Rsrc_Basic, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Basic}},
  {Rsrc_REU, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_REU}},
  {Rsrc_IEEE, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_IEEE488}},
  {Rsrc_EmuID, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_EmuID}},
  {Rsrc_AciaDE, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_ACIADE}},
  {Rsrc_ACIAD6, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_ACIAD67}},
  {Rsrc_CartF, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_CartFile}},
  {Rsrc_RsUsr, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_RsUsr}},	/* rsuser.c */
  {Rsrc_AciaIrq, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_Conf_ACIAIrq}},	/* c64acia.c */
  {Rsrc_SidFilt, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_Conf_SidFilter}},	/* sid.c */
  {Rsrc_ReSid, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_Conf_UseResid}},
  {Rsrc_SScoll, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_CheckSScoll}},/* vicii.c */
  {Rsrc_SBcoll, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_Conf_CheckSBcoll}},
  {Rsrc_Palette, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Palette}},
  {Rsrc_SoundFile, CONFIG_STRING, {CONF_WIN_SOUND, Icon_Conf_FileSndPath}},
  {Rsrc_SerialFile, CONFIG_STRING, {CONF_WIN_DEVICES, Icon_Conf_FileRsPath}},
  {Rsrc_PrinterFile, CONFIG_STRING, {CONF_WIN_DEVICES, Icon_Conf_FilePrPath}},
  {Rsrc_PetCrt, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetCrt}},
  {Rsrc_PetRAM9, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetRAM9}},
  {Rsrc_PetRAMA, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetRAMA}},
  {Rsrc_PetDiag, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetDiagPin}},
  {Rsrc_PetSuper, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetSuper}},
  {NULL, 0, {0, 0}}
};



/* Menu item display initializers */
#define DISP_DESC_STRING	1
#define DISP_DESC_BITFIELD	2
#define DISP_DESC_STRSHOW	4

typedef struct {
  int icon;
  int item;
} disp_strshow_t;

typedef struct {
  char *resource;
  conf_icon_id id;
  RO_MenuHead *menu;
  int items;
  unsigned int flags;
  unsigned int writable;
} disp_desc_t;

static struct MenuDisplayTrueSync {
  disp_desc_t dd;
  int values[Menu_TrueSync_Items];
} MenuDisplayTrueSync = {
  {Rsrc_TrueSync, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvSyncT},
    (RO_MenuHead*)&MenuTrueSync, Menu_TrueSync_Items, 0, 1<<Menu_TrueSync_Custom},
  {DRIVE_SYNC_PAL, DRIVE_SYNC_NTSC, 0}
};

#define DISP_TRUE_DRIVE_EXTEND_MENU(n) \
  static struct MenuDisplayTrueExtend##n { \
    disp_desc_t dd; \
    int values[Menu_TrueExtend_Items]; \
  } MenuDisplayTrueExtend##n = { \
    {Rsrc_TrueExImg##n, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvExt##n##T}, \
      (RO_MenuHead*)&MenuTrueExtend##n, Menu_TrueExtend_Items, 0, 0}, \
    {DRIVE_EXTEND_NEVER, DRIVE_EXTEND_ASK, DRIVE_EXTEND_ACCESS} \
  };

#define DISP_TRUE_DRIVE_IDLE_MENU(n) \
  static struct MenuDisplayTrueIdle##n { \
    disp_desc_t dd; \
    int values[Menu_TrueIdle_Items]; \
  } MenuDisplayTrueIdle##n = { \
    {Rsrc_TrueIdle##n, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvIdle##n##T}, \
      (RO_MenuHead*)&MenuTrueIdle##n, Menu_TrueIdle_Items, 0, 0}, \
    {DRIVE_IDLE_NO_IDLE, DRIVE_IDLE_SKIP_CYCLES, DRIVE_IDLE_TRAP_IDLE} \
  };

#define DISP_TRUE_DRIVE_TYPE_MENU(n) \
  static struct MenuDisplayTrueType##n { \
    disp_desc_t dd; \
    int values[Menu_TrueType_Items]; \
  } MenuDisplayTrueType##n = { \
    {Rsrc_TrueType##n, {CONF_WIN_DRIVES, Icon_Conf_TrueDrvType##n##T}, \
      (RO_MenuHead*)&MenuTrueType##n, Menu_TrueType_Items, 0, 0}, \
    {DRIVE_TYPE_NONE, DRIVE_TYPE_1541, DRIVE_TYPE_1541II, DRIVE_TYPE_1571, DRIVE_TYPE_1581, DRIVE_TYPE_2031} \
  };

DISP_TRUE_DRIVE_EXTEND_MENU(8)
DISP_TRUE_DRIVE_IDLE_MENU(8)
DISP_TRUE_DRIVE_TYPE_MENU(8)
DISP_TRUE_DRIVE_EXTEND_MENU(9)
DISP_TRUE_DRIVE_IDLE_MENU(9)
DISP_TRUE_DRIVE_TYPE_MENU(9)


static struct MenuDisplaySampleRate {
  disp_desc_t dd;
  int values[Menu_SampRate_Items];
} MenuDisplaySampleRate = {
  {Rsrc_SndRate, {CONF_WIN_SOUND, Icon_Conf_SampleRateT},
    (RO_MenuHead*)&MenuSampleRate, Menu_SampRate_Items, 0, 0},
  {8000, 11025, 22050, 44100, 48000}
};

static char SoundDevice0[] = "vidc";
static char SoundDevice1[] = "dummy";
static char SoundDevice2[] = "fs";
static char SoundDevice3[] = "speed";
static char SoundDevice4[] = "dump";

static struct MenuDisplaySoundDevice {
  disp_desc_t dd;
  int values[Menu_SoundDev_Items];
} MenuDisplaySoundDevice = {
  {Rsrc_SndDev, {CONF_WIN_SOUND, Icon_Conf_SoundDevT},
    (RO_MenuHead*)&MenuSoundDevice, Menu_SoundDev_Items, DISP_DESC_STRING, 0},
  {(int)SoundDevice0, (int)SoundDevice1, (int)SoundDevice2, (int)SoundDevice3, (int)SoundDevice4}
};

static struct MenuDisplaySoundOver {
  disp_desc_t dd;
  int values[Menu_SoundOver_Items];
} MenuDisplaySoundOver = {
  {Rsrc_SndOver, {CONF_WIN_SOUND, Icon_Conf_OversampleT},
    (RO_MenuHead*)&MenuSoundOver, Menu_SoundOver_Items, 0, 0},
  {0, 1, 2, 3}
};

static struct MenuDisplaySidModel {
  disp_desc_t dd;
  int values[Menu_SidModel_Items];
} MenuDisplaySidModel = {
  {Rsrc_SidMod, {CONF_WIN_SOUND, Icon_Conf_SidModelT},
    (RO_MenuHead*)&MenuSidModel, Menu_SidModel_Items, 0, 0},
  {0, 1}
};

static struct MenuDisplaySpeedLimit {
  disp_desc_t dd;
  int values[Menu_SpeedLimit_Items];
} MenuDisplaySpeedLimit = {
  {Rsrc_SpeedLimit, {CONF_WIN_SYSTEM, Icon_Conf_SpeedLmtT},
    (RO_MenuHead*)&MenuSpeedLimit, Menu_SpeedLimit_Items, 0, 0},
  {200, 100, 50, 20, 10, 0}
};

static struct MenuDisplayRefresh {
  disp_desc_t dd;
  int values[Menu_Refresh_Items];
} MenuDisplayRefresh = {
  {Rsrc_Refresh, {CONF_WIN_SYSTEM, Icon_Conf_RefreshT},
    (RO_MenuHead*)&MenuRefresh, Menu_Refresh_Items, 0, 0},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
};

static struct MenuDisplayPrintDev {
  disp_desc_t dd;
  int values[Menu_PrntDev_Items];
} MenuDisplayPrintDev = {
  {Rsrc_Prnt4Dev, {CONF_WIN_DEVICES, Icon_Conf_PrntDevT},
    (RO_MenuHead*)&MenuPrintDev, Menu_PrntDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayUserDev {
  disp_desc_t dd;
  int values[Menu_UserDev_Items];
} MenuDisplayUserDev = {
  {Rsrc_PrUsrDev, {CONF_WIN_DEVICES, Icon_Conf_PrntUsrDevT},
    (RO_MenuHead*)&MenuUserDev, Menu_UserDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayAciaDevice {
  disp_desc_t dd;
  int values[Menu_AciaDev_Items];
} MenuDisplayAciaDevice = {
  {Rsrc_AciaDev, {CONF_WIN_DEVICES, Icon_Conf_ACIADevT},
    (RO_MenuHead*)&MenuAciaDevice, Menu_AciaDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayRsUserDevice {
  disp_desc_t dd;
  int values[Menu_RsUsrDev_Items];
} MenuDisplayRsUserDevice = {
  {Rsrc_RsUsrDev, {CONF_WIN_DEVICES, Icon_Conf_RsUsrDevT},
    (RO_MenuHead*)&MenuRsUserDevice, Menu_RsUsrDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplaySerialBaud {
  disp_desc_t dd;
  int values[Menu_Serial_Items];
} MenuDisplaySerialBaud = {
  {Rsrc_Serial, {CONF_WIN_DEVICES, Icon_Conf_SerialT},
    (RO_MenuHead*)&MenuSerialBaud, Menu_Serial_Items, 0, 0},
  {9, 1, 10, 11, 2, 3, 12, 4, 13, 5, 14, 6, 15, 7, 8}
};


static struct MenuDisplayCartridgeType {
  disp_desc_t dd;
  int values[Menu_Cartridge_Items];
} MenuDisplayCartridgeType = {
  {Rsrc_CartT, {CONF_WIN_SYSTEM, Icon_Conf_CartTypeT},
    (RO_MenuHead*)&MenuCartridgeType, Menu_Cartridge_Items, 0, 0},
  {CARTRIDGE_NONE, CARTRIDGE_GENERIC_8KB, CARTRIDGE_GENERIC_16KB, CARTRIDGE_CRT,
   CARTRIDGE_ACTION_REPLAY, CARTRIDGE_KCS_POWER, CARTRIDGE_SIMONS_BASIC,
   CARTRIDGE_ULTIMAX, CARTRIDGE_SUPER_SNAPSHOT, CARTRIDGE_FINAL_III, CARTRIDGE_OCEAN,
   CARTRIDGE_FUNPLAY, CARTRIDGE_SUPER_GAMES}
};

static struct MenuDisplayPetMemory {
  disp_desc_t dd;
  int values[Menu_PetMemory_Items];
} MenuDisplayPetMemory = {
  {Rsrc_PetMem, {CONF_WIN_PET, Icon_Conf_PetMemT},
    (RO_MenuHead*)&MenuPetMemory, Menu_PetMemory_Items, 0, 0},
  {4, 8, 16, 32, 96, 128}
};

static struct MenuDisplayPetIO {
  disp_desc_t dd;
  int values[Menu_PetIO_Items];
} MenuDisplayPetIO = {
  {Rsrc_PetIO, {CONF_WIN_PET, Icon_Conf_PetIOT},
    (RO_MenuHead*)&MenuPetIO, Menu_PetIO_Items, 0, 0},
  {0x800, 0x100}
};

static struct MenuDisplayPetVideo {
  disp_desc_t dd;
  int values[Menu_PetVideo_Items];
} MenuDisplayPetVideo = {
  {Rsrc_PetVideo, {CONF_WIN_PET, Icon_Conf_PetVideoT},
    (RO_MenuHead*)&MenuPetVideo, Menu_PetVideo_Items, 0, 0},
  {0, 40, 80}
};

static char PetModel0[] = "2001";
static char PetModel1[] = "3008";
static char PetModel2[] = "3016";
static char PetModel3[] = "3032";
static char PetModel4[] = "3032B";
static char PetModel5[] = "4016";
static char PetModel6[] = "4032";
static char PetModel7[] = "4032B";
static char PetModel8[] = "8032";
static char PetModel9[] = "8096";
static char PetModel10[] = "8296";
static char PetModel11[] = "SuperPET";

static struct MenuDisplayPetModel {
  disp_desc_t dd;
  int values[Menu_PetModel_Items];
} MenuDisplayPetModel = {
  {Rsrc_PetModel, {CONF_WIN_PET, Icon_Conf_PetModelT},
    (RO_MenuHead*)&MenuPetModel, Menu_PetModel_Items, DISP_DESC_STRING, 0},
  {(int)PetModel0, (int)PetModel1, (int)PetModel2, (int)PetModel3, (int)PetModel4,
   (int)PetModel5, (int)PetModel6, (int)PetModel7, (int)PetModel8, (int)PetModel9,
   (int)PetModel10, (int)PetModel11}
};

static struct MenuDisplayVicRam {
  disp_desc_t dd;
  int values[Menu_VicRam_Items];
} MenuDisplayVicRam = {
  {NULL, {CONF_WIN_VIC, 0},
    (RO_MenuHead*)&MenuVicRam, Menu_VicRam_Items, DISP_DESC_BITFIELD, 0},
  {(int)Rsrc_VicRam0, (int)Rsrc_VicRam1, (int)Rsrc_VicRam2, (int)Rsrc_VicRam3, (int)Rsrc_VicRam5}
};

static disp_strshow_t VicCartridgeDesc = {
  Icon_Conf_VICCartF, 0
};

static struct MenuDisplayVicCartridge {
  disp_desc_t dd;
  int values[Menu_VicCart_Items];
} MenuDisplayVicCartridge = {
  {(char*)&VicCartridgeDesc, {CONF_WIN_VIC, Icon_Conf_VICCartT},
    (RO_MenuHead*)&MenuVicCartridge, Menu_VicCart_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_VicCart2, (int)Rsrc_VicCart6, (int)Rsrc_VicCartA, (int)Rsrc_VicCartB}
};

static disp_strshow_t DosNameDesc = {
  Icon_Conf_DosNameF, 0
};

static struct MenuDisplayDosName {
  disp_desc_t dd;
  int values[Menu_DosName_Items];
} MenuDisplayDosName = {
  {(char*)&DosNameDesc, {CONF_WIN_SYSTEM, Icon_Conf_DosNameT},
    (RO_MenuHead*)&MenuDosName, Menu_DosName_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_Dos1541, (int)Rsrc_Dos1571, (int)Rsrc_Dos1581, (int)Rsrc_Dos2031}
};

static struct MenuDisplayCBM2Line {
  disp_desc_t dd;
  int values[Menu_CBM2Line_Items];
} MenuDisplayCBM2Line = {
  {Rsrc_C2Line, {CONF_WIN_CBM2, Icon_Conf_CBM2LineT},
    (RO_MenuHead*)&MenuCBM2Line, Menu_CBM2Line_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayCBM2Memory {
  disp_desc_t dd;
  int values[Menu_CBM2Mem_Items];
} MenuDisplayCBM2Memory = {
  {Rsrc_C2Mem, {CONF_WIN_CBM2, Icon_Conf_CBM2MemT},
    (RO_MenuHead*)&MenuCBM2Memory, Menu_CBM2Mem_Items, 0, 0},
  {128, 256, 512, 1024}
};

static char CBM2Model0[] = "610";
static char CBM2Model1[] = "620";
static char CBM2Model2[] = "620+";
static char CBM2Model3[] = "710";
static char CBM2Model4[] = "720";
static char CBM2Model5[] = "720+";

static struct MenuDisplayCBM2Model {
  disp_desc_t dd;
  int values[Menu_CBM2Model_Items];
} MenuDisplayCBM2Model = {
  {NULL, {CONF_WIN_CBM2, Icon_Conf_CBM2ModelT},
    (RO_MenuHead*)&MenuCBM2Model, Menu_CBM2Model_Items, DISP_DESC_STRING, 0},
  {(int)CBM2Model0, (int)CBM2Model1, (int)CBM2Model2, (int)CBM2Model3, (int)CBM2Model4, (int)CBM2Model5}
};

static struct MenuDisplayCBM2RAM {
  disp_desc_t dd;
  int values[Menu_CBM2RAM_Items];
} MenuDisplayCBM2RAM = {
  {NULL, {CONF_WIN_CBM2, 0},
    (RO_MenuHead*)&MenuCBM2RAM, Menu_CBM2RAM_Items, DISP_DESC_BITFIELD, 0},
  {(int)Rsrc_C2RAM08, (int)Rsrc_C2RAM1, (int)Rsrc_C2RAM2, (int)Rsrc_C2RAM4, (int)Rsrc_C2RAM6, (int)Rsrc_C2RAMC}
};

static disp_strshow_t CBM2CartridgeDesc = {
  Icon_Conf_CBM2CartF, 0
};

static struct MenuDisplayCBM2Cartridge {
  disp_desc_t dd;
  int values[Menu_CBM2Cart_Items];
} MenuDisplayCBM2Cartridge = {
  {(char*)&CBM2CartridgeDesc, {CONF_WIN_CBM2, Icon_Conf_CBM2CartT},
    (RO_MenuHead*)&MenuCBM2Cartridge, Menu_CBM2Cart_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_C2Cart1, (int)Rsrc_C2Cart2, (int)Rsrc_C2Cart4, (int)Rsrc_C2Cart6}
};

static struct MenuDisplaySoundBuffer {
  disp_desc_t dd;
  int values[Menu_SoundBuffer_Items];
} MenuDisplaySoundBuffer = {
  {Rsrc_SndBuff, {CONF_WIN_SOUND, Icon_Conf_SoundBuffT},
    (RO_MenuHead*)&MenuSoundBuffer, Menu_SoundBuffer_Items, 0, 0},
  {20, 40, 60, 80, 100, 200, 350, 500}
};

#define DISP_JOYSTICK_DEVICE_MENU(n) \
  static struct MenuDisplayJoyDevice##n { \
    disp_desc_t dd; \
    int values[Menu_JoyDevice_Items]; \
  } MenuDisplayJoyDevice##n = { \
    {Rsrc_JoyDev##n, {CONF_WIN_JOY, Icon_Conf_JoyPort##n##T}, \
      (RO_MenuHead*)&MenuJoyDevice##n, Menu_JoyDevice_Items, 0, 0}, \
    {JOYDEV_NONE, JOYDEV_KBD1, JOYDEV_KBD2, JOYDEV_JOY1, JOYDEV_JOY2} \
  };

DISP_JOYSTICK_DEVICE_MENU(1)
DISP_JOYSTICK_DEVICE_MENU(2)

static disp_desc_t *MenuDisplayROMSet = NULL;

static struct MenuDisplayROMSetTmpl {
  disp_desc_t dd;
  int values[1];
} MenuDisplayROMSetTmpl = {
  {NULL, {CONF_WIN_SYSTEM, Icon_Conf_ROMSetT},
    (RO_MenuHead*)&MenuROMSetTmpl, 1, DISP_DESC_STRING, 0},
  {0}
};





/* Mode changes */
int FrameBufferUpdate = 0;
int ModeChanging = 0;
static PIXEL oldColours[16];

/* LED states */
static int ledstates[4] = {0, 0, 0, 0};

/* Window title */
static char EmuTitle[256];




static int set_poll_every(resource_value_t v)
{
  PollEvery = (int)v;
  return 0;
}

static int set_speed_every(resource_value_t v)
{
  SpeedEvery = (int)v;
  return 0;
}

static int set_sound_every(resource_value_t v)
{
  SoundPollEvery = (int)v;	/* actually defined in soundacorn */
  return 0;
}

static int set_drive_type8(resource_value_t v)
{
  DriveType8 = (int)v;
  return 0;
}

static int set_drive_type9(resource_value_t v)
{
  DriveType9 = (int)v;
  return 0;
}

static int set_drive_type10(resource_value_t v)
{
  DriveType10 = (int)v;
  return 0;
}

static int set_drive_type11(resource_value_t v)
{
  DriveType11 = (int)v;
  return 0;
}

static int set_drive_file8(resource_value_t v)
{
  const char *name = (const char *)v;

  if ((DriveFile8 != NULL) && (name != NULL) && (strcmp(name, DriveFile8) == 0))
    return 0;

  string_set(&DriveFile8, name);
  return 0;
}

static int set_drive_file9(resource_value_t v)
{
  const char *name = (const char*)v;

  if ((DriveFile9 != NULL) && (name != NULL) && (strcmp(name, DriveFile9) == 0))
    return 0;

  string_set(&DriveFile9, name);
  return 0;
}

static int set_drive_file10(resource_value_t v)
{
  const char *name = (const char*)v;

  if ((DriveFile10 != NULL) && (name != NULL) && (strcmp(name, DriveFile10) == 0))
    return 0;

  string_set(&DriveFile10, name);
  return 0;
}

static int set_drive_file11(resource_value_t v)
{
  const char *name = (const char*)v;

  if ((DriveFile11 != NULL) && (name != NULL) && (strcmp(name, DriveFile11) == 0))
    return 0;

  string_set(&DriveFile11, name);
  return 0;
}

static int set_tape_file(resource_value_t v)
{
  const char *name = (const char*)v;

  if ((TapeFile != NULL) && (name != NULL) && (strcmp(name, TapeFile) == 0))
    return 0;

  string_set(&TapeFile, name);
  return 0;
}

static int set_speed_limit(resource_value_t v)
{
  SpeedLimit = (int)v;
  FrameCS = (SpeedLimit == 0) ? 0 : 200/SpeedLimit;

  return 0;
}

static int set_auto_pause(resource_value_t v)
{
  AutoPauseEmu = (int)v;
  return 0;
}



static resource_t resources[] = {
  {Rsrc_Poll, RES_INTEGER, (resource_value_t)20,
    (resource_value_t*)&PollEvery, set_poll_every},
  {Rsrc_Speed, RES_INTEGER, (resource_value_t)100,
    (resource_value_t*)&SpeedEvery, set_speed_every},
  {Rsrc_SndEvery, RES_INTEGER, (resource_value_t)0,
    (resource_value_t*)&SoundPollEvery, set_sound_every},
  {Rsrc_AutoPause, RES_INTEGER, (resource_value_t)0,
    (resource_value_t*)&AutoPauseEmu, set_auto_pause},
  {Rsrc_DriveT8, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType8, set_drive_type8},
  {Rsrc_DriveT9, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType9, set_drive_type9},
  {Rsrc_DriveT10, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType10, set_drive_type10},
  {Rsrc_DriveT11, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType11, set_drive_type11},
  {Rsrc_DriveF8, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile8, set_drive_file8},
  {Rsrc_DriveF9, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile9, set_drive_file9},
  {Rsrc_DriveF10, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile10, set_drive_file10},
  {Rsrc_DriveF11, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile11, set_drive_file11},
  {Rsrc_TapeFile, RES_STRING, (resource_value_t)"",
    (resource_value_t*)&TapeFile, set_tape_file},
  {Rsrc_SpeedLimit, RES_INTEGER, (resource_value_t)100,
    (resource_value_t*)&SpeedLimit, set_speed_limit},
  {NULL}
};



static int ui_load_template(const char *tempname, RO_Window **wptr, wimp_msg_desc *msg)
{
  if ((*wptr = wimp_load_template(tempname)) == NULL)
  {
    log_error(roui_log, SymbolStrings[Symbol_ErrTemp], tempname);
    exit(-1);
  }
  else
  {
    RO_Window *w;
    RO_Icon *icons;
    char *title = NULL;

    w = (*wptr);
    icons = (RO_Icon*)(((char*)w) + sizeof(RO_Window));

    if (wptr == &EmuWindow)
    {
      int dx, dy;

      UseEigen = (ScreenMode.eigx < ScreenMode.eigy) ? ScreenMode.eigx : ScreenMode.eigy;
      dx = (VIC_II_SCREEN_WIDTH << UseEigen) * EmuZoom;
      dy = (VIC_II_SCREEN_HEIGHT << UseEigen) * EmuZoom;
      w->vminx = (ScreenMode.resx - dx) / 2; w->vmaxx = w->vminx + dx;
      w->vminy = (ScreenMode.resy - dy) / 2; w->vmaxy = w->vminy + dy;
      w->wmaxx = dx; w->wminy = -dy;
      title = EmuTitle;
    }
    else if (wptr == &EmuPane)
    {
      w->SpriteAreaPtr = (int)SpriteArea;
      icons[Icon_Pane_LED0].dat.ind.val = (int*)SpriteArea;
      icons[Icon_Pane_LED1].dat.ind.val = (int*)SpriteArea;
      icons[Icon_Pane_LED2].dat.ind.val = (int*)SpriteArea;
      icons[Icon_Pane_LED3].dat.ind.val = (int*)SpriteArea;
      sprintf((char*)(icons[Icon_Pane_Pause].dat.ind.tit), SymbolStrings[Symbol_Pause]);
      sprintf((char*)(icons[Icon_Pane_Toggle].dat.ind.tit), SymbolStrings[(EmuZoom == 1) ? Symbol_Zoom2 : Symbol_Zoom1]);
    }
    else if (wptr == &InfoWindow)
    {
      w->SpriteAreaPtr = (int)SpriteArea;
    }

    /* Message handling if necessary */
    if (msg != NULL) wimp_message_translate_window(msg, w);

    wimp_window_create((int*)w, title);

    return 0;
  }
}


int ui_init_named_app(const char *appname, const char *iconname)
{
  return 0;
}


int ui_init_resources(void)
{
  return resources_register(resources);
}


int ui_init_cmdline_options(void)
{
  return 0;
}


static void ui_temp_suspend_sound(void)
{
  if (SoundEnabled != 0)
  {
    sound_suspend();
  }
}

static void ui_temp_resume_sound(void)
{
  if (SoundEnabled != 0)
  {
    /* Don't resume sound when in the ui_poll loop. Just mark that this should be done
       on exit */
    if (WithinUiPoll == 0)
    {
      sound_resume();
    }
    else
    {
      SoundSuspended = 1;
    }
  }
}


/* If w != NULL it overrides the window information in the descriptor */
static void ui_set_icons_grey(RO_Window *w, conf_icon_id *desc, int state)
{
  int i;
  unsigned eor;

  eor = (state == 0) ? IFlg_Grey : 0;
  for (i=0; desc[i].win != 0xff; i++)
  {
    wimp_window_set_icon_state((w == NULL) ? ConfWindows[desc[i].win] : w, desc[i].icon, eor, IFlg_Grey);
  }
}


static void ui_display_truedrv_emulation(int state)
{
  int i;
  unsigned int eor;

  TrueDriveEmulation = state;

  wimp_window_set_icon_state(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TrueDrv, (state == 0) ? 0 : IFlg_Slct, IFlg_Slct);

  eor = (state == 0) ? 0 : IFlg_Grey;
  for (i=2; i<4; i++)
  {
    wimp_window_set_icon_state(EmuPane, LEDtoIcon[i], eor, IFlg_Grey);
  }

  ui_set_icons_grey(NULL, TrueDependentIcons, state);

  if (state == 0)
  {
    wimp_window_write_icon_text_u(EmuPane, Icon_Pane_TrkSec, "");
  }

  wimp_menu_tick_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_TrueDrvEmu, state);
}

static void ui_set_truedrv_emulation(int state)
{
  if (machine_class == VICE_MACHINE_PET) return;

  /*
   *  In case sound is on and true drives are switched on we have to suspend sound
   *  because the sound thread sitting on the timer screws up the drive init phase
   *  royally.
   */
  if (state != 0) ui_temp_suspend_sound();

  if (resources_set_value(Rsrc_True, (resource_value_t)state) == 0)
  {
    ui_display_truedrv_emulation(state);
  }

  if (state != 0) ui_temp_resume_sound();
}


static void ui_display_sound_enable(int state)
{
  SoundEnabled = state;

  wimp_window_set_icon_state(ConfWindows[CONF_WIN_SOUND], Icon_Conf_SoundOn, (state == 0) ? 0 : IFlg_Slct, IFlg_Slct);

  ui_set_icons_grey(NULL, SoundDependentIcons, state);
  if ((machine_class == VICE_MACHINE_C64) || (machine_class == VICE_MACHINE_C128) || (machine_class == VICE_MACHINE_CBM2))
  {
    ui_set_icons_grey(NULL, SidDependentIcons, state);
  }

  wimp_menu_tick_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Sound, state);

  wimp_window_redraw_icon(ConfWindows[CONF_WIN_SOUND], Icon_Conf_Volume);
}

static void ui_set_sound_enable(int state)
{
  if (resources_set_value(Rsrc_Sound, (resource_value_t)state) != 0) return;

  ui_display_sound_enable(state);
}


static void ui_set_pane_state(int state)
{
  wimp_menu_tick_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Pane, state);

  if (state == 0)
  {
    Wimp_CloseWindow((int*)EmuPane);
  }
  else
  {
    int block[WindowB_WFlags+1];

    block[WindowB_Handle] = EmuWindow->Handle;
    Wimp_GetWindowState(block);
    if ((block[WindowB_WFlags] & (1<<16)) != 0)
    {
      ui_open_emu_window(block);
    }
  }
}




static void ui_set_menu_disp_strshow(const disp_desc_t *dd)
{
  disp_strshow_t *ds;
  char **resources;
  resource_value_t val;

  ds = ((disp_strshow_t*)(dd->resource));
  resources = (char**)(dd + 1);
  if (resources_get_value(resources[ds->item], &val) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[dd->id.win], ds->icon, (char*)val);
  }
}


static void ui_update_menu_disp_strshow(const disp_desc_t *dd, resource_value_t val)
{
  disp_strshow_t *ds;
  char **resources;

  ds = ((disp_strshow_t*)(dd->resource));
  resources = (char**)(dd + 1);
  if (resources_set_value(resources[ds->item], val) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[dd->id.win], ds->icon, (char*)val);
  }
}


static void ui_setup_menu_disp_core(const disp_desc_t *dd, resource_value_t val)
{
  RO_MenuHead *menu;
  RO_MenuItem *item;
  int *values;
  int i=-1;

  menu = dd->menu;
  item = (RO_MenuItem*)(menu + 1); values = (int*)(dd + 1);

  if ((dd->flags & DISP_DESC_STRSHOW) != 0)
  {
    i = ((disp_strshow_t*)(dd->resource))->item;
  }
  else
  {
    for (i=0; i<dd->items; i++)
    {
      if ((dd->flags & DISP_DESC_STRING) != 0)
      {
        if (val == (resource_value_t)0) continue;
        if (strcmp((char*)(values[i]), (char*)val) == 0) break;
      }
      else
      {
        if (values[i] == (int)val) break;
      }
    }
    if (i >= dd->items) i = -1;
  }

  if (i >= 0)
  {
    RO_Icon *icon;

    wimp_menu_tick_exclusive(menu, i);
    if ((icon = wimp_window_get_icon(ConfWindows[dd->id.win], dd->id.icon)) == NULL) return;
    if ((icon->iflags & IFlg_Indir) == 0) return;
    ((char*)(icon->dat.ind.tit))[12] = 0;
    strncpy((char*)(icon->dat.ind.tit), item[i].dat.strg, 12);
    wimp_window_redraw_icon(ConfWindows[dd->id.win], dd->id.icon);
  }
}

static void ui_setup_menu_display(const disp_desc_t *dd)
{
  resource_value_t val;

  if ((dd->flags & DISP_DESC_BITFIELD) != 0)
  {
    unsigned int bits = 0;
    char **values;
    int i;

    values = (char**)(dd + 1);
    for (i=0; i<dd->items; i++)
    {
      if (resources_get_value(values[i], &val) == 0)
      {
        if (val != 0) bits |= (1<<i);
      }
    }
    wimp_menu_tick_slct(dd->menu, bits);
  }
  else if ((dd->flags & DISP_DESC_STRSHOW) != 0)
  {
    ui_setup_menu_disp_core(dd, 0);
    ui_set_menu_disp_strshow(dd);
  }
  else if (dd->resource != NULL)
  {
    if (resources_get_value(dd->resource, &val) != 0) return;

    ui_setup_menu_disp_core(dd, val);
  }
}


/* Special set-functions */
int set_pet_model_by_name(const char *name, resource_value_t val)
{
  PetModelName = (char*)val;
  return pet_set_model((const char*)val, NULL);
}

int set_cbm2_model_by_name(const char *name, resource_value_t val)
{
  CBM2ModelName = (char*)val;
  return cbm2_set_model((const char*)val, NULL);
}

int set_romset_by_name(const char *name, resource_value_t val)
{
  if (val == (resource_value_t)0) return -1;
  if (ROMSetName != NULL) free(ROMSetName);
  ROMSetName = stralloc((char*)val);
  return romset_select_item((char*)val);
}


static void ui_set_menu_display_core(const disp_desc_t *dd, set_var_function func, int number)
{
  RO_MenuHead *menu;
  RO_MenuItem *item;
  int *values;
  int state=0;

  if (number >= dd->items) return;

  menu = dd->menu;
  item = (RO_MenuItem*)(menu + 1); item += number;
  values = (int*)(dd + 1); values += number;

  if ((dd->writable & (1<<number)) != 0)
  {
    if ((item->iflags & IFlg_Indir) == 0)
      *values = atoi(item->dat.strg);
    else
      *values = atoi((char*)(item->dat.ind.tit));
  }
  if (func != (set_var_function)NULL)
  {
    state = func(dd->resource, (resource_value_t)(*values));
  }
  if (state == 0)
  {
    RO_Icon *icon;

    wimp_menu_tick_exclusive(menu, number);
    if ((icon = wimp_window_get_icon(ConfWindows[dd->id.win], dd->id.icon)) == NULL) return;
    if ((icon->iflags & IFlg_Indir) == 0) return;
    ((char*)(icon->dat.ind.tit))[12] = 0;
    strncpy((char*)(icon->dat.ind.tit), item->dat.strg, 12);
    wimp_window_redraw_icon(ConfWindows[dd->id.win], dd->id.icon);
  }
}


static void ui_set_menu_display_value(const disp_desc_t *dd, int number)
{
  if ((dd->flags & DISP_DESC_BITFIELD) != 0)
  {
    resource_value_t val;
    int state;
    char **values;

    values = (char**)(dd + 1);
    resources_get_value(values[number], &val);
    state = (int)val; state = !state;
    if (resources_set_value(values[number], (resource_value_t)state) == 0)
    {
      wimp_menu_tick_item(dd->menu, number, -1);
    }
  }
  else if ((dd->flags & DISP_DESC_STRSHOW) != 0)
  {
    ((disp_strshow_t*)(dd->resource))->item = number;
    ui_set_menu_display_core(dd, (set_var_function)NULL, number);
    ui_set_menu_disp_strshow(dd);
  }
  else if (dd->resource != NULL)
  {
    ui_set_menu_display_core(dd, resources_set_value, number);
  }
}



static int ui_set_drive_image(int number, const char *file)
{
  int info[4];

  if ((ReadCatalogueInfo(file, info) & 1) == 0) return -1;
  file_system_detach_disk(8 + number);
  if (file_system_attach_disk(8 + number, file) == 0)
  {
    string_set(DriveFiles[number], file);
    *(DriveTypes[number]) = DRIVE_TYPE_DISK;
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number], file);
    return 0;
  }
  return -1;
}


static int ui_set_drive_dir(int number, const char *dir)
{
  int info[4];

  if ((ReadCatalogueInfo(dir, info) & 2) == 0) return -1;
  file_system_detach_disk(8 + number);
  fsdevice_set_directory((char*)dir, 8 + number);
  string_set(DriveFiles[number], dir);
  *(DriveTypes[number]) = DRIVE_TYPE_FS;
  wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number], dir);
  return 0;
}


static int ui_set_sound_file(const char *name)
{
  if (resources_set_value(Rsrc_SoundFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_SOUND], Icon_Conf_FileSndPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_serial_file(const char *name)
{
  if (resources_set_value(Rsrc_SerialFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_Conf_FileRsPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_printer_file(const char *name)
{
  if (resources_set_value(Rsrc_PrinterFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_Conf_FilePrPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_cartridge_file(const char *name)
{
  if (resources_set_value(Rsrc_CartF, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_SYSTEM], Icon_Conf_CartFile, name);
    return 0;
  }
  return -1;
}


static int ui_make_snapshot(const char *name)
{
  int save_roms, save_disks, status;
  int block[10];

  wimp_window_get_icon_state(SnapshotWindow, Icon_Snap_ROM, block);
  save_roms = ((block[6] & IFlg_Slct) == 0) ? 0 : 1;
  wimp_window_get_icon_state(SnapshotWindow, Icon_Snap_Disk, block);
  save_disks = ((block[6] & IFlg_Slct) == 0) ? 0 : 1;

  /* For reasons for this see true drive emulation */
  ui_temp_suspend_sound();

  if ((status = machine_write_snapshot(name, save_roms, save_disks)) == 0)
  {
    /* if successful, close the menu (true for all varieties this can be called with) */
    Wimp_CreateMenu((int*)-1, 0, 0);
  }
  else
  {
    /* else delete the file */
    remove(name);
  }

  SnapshotPending = 0;

  ui_temp_resume_sound();

  return status;
}


static void ui_save_snapshot_trap(ADDRESS unused_address, void *unused_data)
{
  ui_make_snapshot(((char*)SnapshotMessage)+44);
}

static int ui_check_save_snapshot(const char *name)
{
  if (wimp_check_for_path(name) == 0)
  {
    wimp_strcpy(((char*)SnapshotMessage)+44, name);
    maincpu_trigger_trap(ui_save_snapshot_trap, NULL);
    SnapshotPending = 1;
  }
  return -1;
}

static int ui_check_save_sbox(const char *name)
{
  if (wimp_check_for_path(name) == 0)
  {
    if (LastMenu == CONF_MENU_ROMACT + 0x100)
    {
      if (romset_save_item(ROMSetName, name) == 0)
      {
        wimp_strcpy(ROMSetItemFile, name);
        Wimp_CreateMenu((int*)-1, 0, 0);
      }
      return 0;
    }
  }
  return -1;
}


/* b = update / redraw block */
static void ui_draw_sound_volume(int *b)
{
  RO_Icon *icon;
  int minx, miny, maxx, maxy, thresh;

  icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_Conf_Volume);
  /* Transform to screen coordinates */
  minx = b[RedrawB_VMinX] - b[RedrawB_ScrollX] + icon->minx + Well_Border;
  miny = b[RedrawB_VMaxY] - b[RedrawB_ScrollY] + icon->miny + Well_Border;
  maxx = minx + (icon->maxx - icon->minx - 2*Well_Border);
  maxy = miny + (icon->maxy - icon->miny - 2*Well_Border);
  /* Clip */
  if ((b[RedrawB_CMinX] > maxx) || (b[RedrawB_CMaxX] < minx) ||
      (b[RedrawB_CMinY] > maxy) || (b[RedrawB_CMaxY] < miny))
    return;

  thresh = minx + ((maxx - minx) * SoundVolume) / Maximum_Volume;
  if (b[RedrawB_CMinX] < thresh)
  {
    ColourTrans_SetGCOL((SoundEnabled == 0) ? 0x88888800 : 0x00ff0000, 0, 0);	/* grey|green */
    OS_Plot(0x04, minx, miny); OS_Plot(0x65, thresh, maxy);
  }
  if (b[RedrawB_CMaxX] > thresh)
  {
    ColourTrans_SetGCOL(0xffffff00, 0, 0);	/* white */
    OS_Plot(0x04, thresh, miny); OS_Plot(0x65, maxx, maxy);
  }
}

/* b = mouse-pos block */
static void ui_drag_sound_volume(int *b)
{
  RO_Icon *icon;
  int wstate[RedrawB_CMaxY+1];
  int minx, miny, maxx, maxy, thresh, more;

  wstate[WindowB_Handle] = ConfWindows[CONF_WIN_SOUND]->Handle;
  Wimp_GetWindowState(wstate);

  icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_Conf_Volume);
  minx = wstate[RedrawB_VMinX] - wstate[RedrawB_ScrollX] + icon->minx + Well_Border;
  miny = wstate[RedrawB_VMaxY] - wstate[RedrawB_ScrollY] + icon->miny + Well_Border;
  maxx = minx + (icon->maxx - icon->minx - 2*Well_Border);
  maxy = miny + (icon->maxy - icon->miny - 2*Well_Border);

  thresh = b[MouseB_PosX];
  if (thresh < minx) thresh = minx;
  if (thresh > maxx) thresh = maxx;
  SoundVolume = ((thresh - minx) * Maximum_Volume) / (maxx - minx);

  wstate[1] = icon->minx; wstate[2] = icon->miny;
  wstate[3] = icon->maxx; wstate[4] = icon->maxy;
  more = Wimp_UpdateWindow(wstate);
  while (more != 0)
  {
    ui_draw_sound_volume(wstate);
    more = Wimp_GetRectangle(wstate);
  }
}


void ui_set_sound_volume(void)
{
  Sound_Volume(SoundVolume);
  wimp_window_redraw_icon(ConfWindows[CONF_WIN_SOUND], Icon_Conf_Volume);
}


static char *ui_check_for_syspath(const char *path)
{
  char *vicepath;
  int len;

  if ((vicepath = getenv(VicePathVariable)) == NULL) return (char*)path;
  len = strlen(vicepath);
  if (strncasecmp(path, vicepath, len) == 0)
  {
    vicepath = (char*)path + len;
    len = strlen(machine_name);
    if ((strncasecmp(vicepath, machine_name, len) == 0) && (vicepath[len] == '.'))
      return (char*)vicepath + len + 1;
    len = strlen(ResourceDriveDir);
    if ((strncasecmp(vicepath, ResourceDriveDir, len) == 0) && (vicepath[len] == '.'))
      return (char*)vicepath + len + 1;
  }
  return (char*)path;
}


static int ui_build_romset_menu(void)
{
  int number;

  if (MenuROMSet != NULL) free(MenuROMSet);
  if (MenuDisplayROMSet != NULL) free(MenuDisplayROMSet);
  MenuROMSet = NULL; MenuDisplayROMSet = NULL;
  ConfigMenus[CONF_MENU_ROMSET].menu = (RO_MenuHead*)&MenuROMSetTmpl;

  number = romset_get_number();
  if (number <= 0) return -1;
  MenuROMSet = (RO_MenuHead*)malloc(sizeof(RO_MenuHead) + number * sizeof(RO_MenuItem));
  MenuDisplayROMSet = (disp_desc_t*)malloc(sizeof(disp_desc_t) + number * sizeof(int));

  if ((MenuROMSet != NULL) && (MenuDisplayROMSet != NULL))
  {
    RO_MenuItem *item;
    int *values;
    int i;

    memcpy(MenuROMSet, &MenuROMSetTmpl, sizeof(RO_MenuHead));
    memcpy(MenuDisplayROMSet, &MenuDisplayROMSetTmpl, sizeof(disp_desc_t));
    item = (RO_MenuItem*)(MenuROMSet + 1);
    values = (int*)(MenuDisplayROMSet + 1);

    for (i=0; i<number; i++)
    {
      char *name;

      if ((name = romset_get_item(i)) == NULL) name = "";
      item[i].mflags = 0; item[i].submenu = (RO_MenuHead*)-1; item[i].iflags = Menu_Flags;
      strncpy(item[i].dat.strg, name, 12);
      values[i] = (int)(name);
    }
    item[number-1].mflags = MFlg_LastItem;
    ConfigMenus[CONF_MENU_ROMSET].menu = MenuROMSet;
    MenuDisplayROMSet->menu = MenuROMSet;
    MenuDisplayROMSet->items = number;
    return 0;
  }
  return -1;
}



/* Make absolutely sure the sound timer is killed when the app terminates */
static void ui_safe_exit(void)
{
  sound_wimp_safe_exit();
}


/* Shared by all uis for installing the icon bar icon */
int ui_init(int *argc, char *argv[])
{
  int block[4];
  int x, y, mode;
  char buffer[64];
  char *iname;
  wimp_msg_desc *msg;
  WIdatI *dat;

  PollMask = 0x01000830;	/* save/restore FP regs */
  LastMenu = 0; LastClick = 0; LastDrag = 0; MenuType = 0; DragType = 0;
  EmuZoom = 1;

  if ((msg = wimp_message_init(MessagesFile)) == NULL)
  {
    log_error(roui_log, "Unable to open messages file!\n");
    exit(-1);
  }

  /* Init internal messages of wimp.c */
  wimp_init_messages(msg);

  /* Translate message pool in two passes: 1) determine lenght, 2) actually do it */
  x = wimp_message_translate_symbols(msg, SymbolStrings, NULL);
  if ((iname = (char*)malloc(x)) == NULL)
  {
    log_error(roui_log, "Unable to claim memory for symbol pool!\n");
    exit(-1);
  }
  wimp_message_translate_symbols(msg, SymbolStrings, iname);

  if ((ReadCatalogueInfo(CustomSprites, block) & 1) != 0)
  {
    FILE *fp;

    if ((SpriteArea = (int*)malloc(block[2] + 16)) == NULL)
    {
      log_error(roui_log, "%s\n", SymbolStrings[Symbol_ErrSMem]);
      exit(-1);
    }
    fp = fopen(CustomSprites, "rb");
    fread(SpriteArea + 1, 1, block[2], fp);
    fclose(fp);
    SpriteArea[0] = block[2] + 4;
  }
  else
  {
    log_error(roui_log, SymbolStrings[Symbol_ErrSpr], CustomSprites); log_error(roui_log, "\n");
    exit(-1);
  }

  wimp_read_screen_mode(&ScreenMode);

  LastPoll = OS_ReadMonotonicTime(); LastSpeed = LastPoll; LastFrame = LastPoll;

  TaskHandle = Wimp_Initialise(310, TASK_WORD, WimpTaskName, (int*)WimpMessages);
  strncpy(MenuIconBar.head.title, WimpTaskName, 12);
  strncpy(MenuEmuWindow.head.title, WimpTaskName, 12);

  switch (machine_class)
  {
    case VICE_MACHINE_C64: iname = "!vice64"; break;
    case VICE_MACHINE_C128: iname = "!vice128"; break;
    case VICE_MACHINE_PET: iname = "!vicepet"; break;
    case VICE_MACHINE_VIC20: iname = "!vicevic"; break;
    case VICE_MACHINE_CBM2: iname = "!vicecbm2"; break;
    default: iname = "?"; break;
  }

  if (Wimp_SpriteInfo(iname, &x, &y, &mode) == NULL)
  {
    strncpy((char*)(&IBarIcon.dat), iname, 12);
    IBarIcon.maxx = x << OS_ReadModeVariable(mode, 4);
    IBarIcon.maxy = y << OS_ReadModeVariable(mode, 5);
  }
  else
  {
    _kernel_oserror err;

    err.errnum = Error_IconSprite;
    sprintf(err.errmess, SymbolStrings[Symbol_ErrIcon], iname);
    /*Wimp_ReportError(&err, 1, WimpTaskName);*/
    strncpy((char*)(&IBarIcon.dat), "file_fff", 12);
  }

  wimp_icon_create(0, &IBarIcon);

  for (x=0; x<CONF_WIN_NUMBER; x++)
  {
    ConfWindows[x] = NULL; ConfWinPositions[x] = NULL;
  }

  /* Load windows, translate messages and create windows */
  if (Wimp_OpenTemplate(TemplatesFile) == NULL)
  {
    ui_load_template("EmuWindow", &EmuWindow, msg);
    ui_load_template("EmuPane", &EmuPane, msg);
    ui_load_template("InfoWindow", &InfoWindow, msg);
    ui_load_template("DriveConfig", ConfWindows + CONF_WIN_DRIVES, msg);
    ui_load_template("DevConfig", ConfWindows + CONF_WIN_DEVICES, msg);
    ui_load_template("SoundConfig", ConfWindows + CONF_WIN_SOUND, msg);
    ui_load_template("SysConfig", ConfWindows + CONF_WIN_SYSTEM, msg);
    ui_load_template("JoyConfig", ConfWindows + CONF_WIN_JOY, msg);
    ui_load_template("PetConfig", ConfWindows + CONF_WIN_PET, msg);
    ui_load_template("VicConfig", ConfWindows + CONF_WIN_VIC, msg);
    ui_load_template("CBM2Config", ConfWindows + CONF_WIN_CBM2, msg);
    ui_load_template("Snapshot", &SnapshotWindow, msg);
    ui_load_template("CPUJamBox", &CpuJamWindow, msg);
    ui_load_template("SaveBox", &SaveBox, msg);
    ui_load_template("ImageCont", &ImgContWindow, msg);

    Wimp_CloseTemplate();
  }
  else
  {
    _kernel_oserror err;

    err.errnum = Error_TemplatesFile;
    sprintf(err.errmess, SymbolStrings[Symbol_ErrTFile], TemplatesFile);
    Wimp_ReportError(&err, 1, WimpTaskName);
    ui_exit();
    exit(-1);
  }

  /* Menus */
  wimp_message_translate_menu(msg, (RO_MenuHead*)&MenuIconBar);
  wimp_message_translate_menu(msg, (RO_MenuHead*)&MenuEmuWindow);
  for (x=0; ConfigMenus[x].menu != NULL; x++)
  {
    wimp_message_translate_menu(msg, ConfigMenus[x].menu);
  }

  /* Misc */

  sprintf(EmuTitle, "%s (%s)", WimpTaskName, SymbolStrings[Symbol_Version]);

  wimp_window_write_title(EmuWindow, EmuTitle);

  sprintf(buffer, SymbolStrings[Symbol_Purpose], machine_name);
  wimp_window_write_icon_text(InfoWindow, Icon_Info_Purpose, buffer);
  MenuIconBar.item[Menu_IBar_Info].submenu = (RO_MenuHead*)(InfoWindow->Handle);
  MenuEmuWindow.item[Menu_EmuWin_Snapshot].submenu = (RO_MenuHead*)(SnapshotWindow->Handle);
  wimp_window_write_icon_text(SnapshotWindow, Icon_Snap_Path, "ViceSnap");
  MenuRomAction.item[Menu_RomAct_Save].submenu = (RO_MenuHead*)(SaveBox->Handle);
  MenuRomActName.item[0].iflags |= IFlg_Indir;
  dat = &(MenuRomActName.item[0].dat.ind);
  dat->tit = (int*)NewRomSetName; dat->val = (int*)-1; dat->len = sizeof(NewRomSetName);
  NewRomSetName[0] = '\0';
  MenuTrueSync.item[Menu_TrueSync_Custom].iflags |= IFlg_Indir;
  dat = &(MenuTrueSync.item[Menu_TrueSync_Custom].dat.ind);
  dat->tit = (int*)TrueSyncCustomField; dat->val = (int*)-1; dat->len = sizeof(TrueSyncCustomField);
  TrueSyncCustomField[0] = '\0';
  sprintf(ROMSetItemFile, "rset/"RSETARCH_EXT);

  EmuPaused = 0; LastCaret.WHandle = -1;
  SoundVolume = Sound_Volume(0);

  if (machine_class == VICE_MACHINE_C128)
  {
    config_item *ci;

    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_Conf_ACIAD67, SymbolStrings[Symbol_ACIAD7]);
    for (ci=Configurations; ci->resource != NULL; ci++)
    {
      if ((ci->id.win == CONF_WIN_DEVICES) && (ci->id.icon == Icon_Conf_ACIAD67))
      {
        ci->resource = Rsrc_ACIAD7; break;
      }
    }
  }

  wimp_message_delete(msg);

  return 0;
}


int ui_init_finish(void)
{
  resource_value_t val;
  conf_icon_id *gi;
  int i;

  if (machine_class != VICE_MACHINE_PET)
  {
    if (resources_get_value(Rsrc_True, &val) == 0)
      TrueDriveEmulation = (int)val;
  }

  /*resources_set_value("SoundDeviceName", (resource_value_t)"vidc");*/

  /* Setup the drives */
  for (i=0; i<4; i++)
  {
    if (*(DriveTypes[i]) == DRIVE_TYPE_FS)
    {
      fsdevice_set_directory(*(DriveFiles[i]), 8 + i);
    }
    else if (DriveTypes[i] == DRIVE_TYPE_DISK)
    {
      file_system_attach_disk(8 + i, *(DriveFiles[i]));
    }
  }

  ui_set_truedrv_emulation((int)TrueDriveEmulation);

  if (resources_get_value(Rsrc_Sound, &val) == 0)
    ui_set_sound_enable((int)val);

  FrameCS = (SpeedLimit == 0) ? 0 : 200/SpeedLimit;
  LastSpeedLimit = SpeedLimit;

  CMOS_DragType = ReadDragType();

  switch (machine_class)
  {
    case VICE_MACHINE_C64: gi = conf_grey_x64; break;
    case VICE_MACHINE_C128: gi = conf_grey_x128; break;
    case VICE_MACHINE_VIC20: gi = conf_grey_xvic; break;
    case VICE_MACHINE_PET: gi = conf_grey_xpet; break;
    case VICE_MACHINE_CBM2: gi = conf_grey_xcbm2; break;
    default: gi = NULL;
  }

  if (gi != NULL)
  {
    ui_set_icons_grey(NULL, gi, 0);
  }

  if ((machine_class != VICE_MACHINE_PET) && (machine_class != VICE_MACHINE_VIC20) && (machine_class != VICE_MACHINE_CBM2))
  {
    wimp_menu_set_grey_item((RO_MenuHead*)&MenuConfigure, Menu_Config_Machine, 1);
  }
  if (machine_class == VICE_MACHINE_PET)
  {
    wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_TrueDrvEmu, 1);
  }
  if (machine_class != VICE_MACHINE_C64)
  {
    wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Freeze, 1);
  }

  ui_set_pane_state(ShowPane);

  i = ((TapeFile == NULL) || (strlen(TapeFile) == 0)) ? IFlg_Grey : 0;
  wimp_window_set_icon_state(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TapeDetach, i, IFlg_Grey);

  memset(SnapshotMessage, 0, 256);

  /* Sound buffer size sanity check */
  if (resources_get_value(Rsrc_SndBuff, &val) == 0)
  {
    if ((int)val > Maximum_Latency)
    {
      resources_set_value(Rsrc_SndBuff, (resource_value_t)Maximum_Latency);
    }
  }

  ROMSetName = stralloc("Default");

  if (sysfile_locate("romset/"RSETARCH_EXT, &ROMSetArchiveFile) == 0)
  {
    romset_load_archive(ROMSetArchiveFile, 0);
    ui_build_romset_menu();
  }

  atexit(ui_safe_exit);

  return 0;
}


static void ui_setup_config_item(config_item *ci)
{
  resource_value_t val;

  if (resources_get_value(ci->resource, &val) != 0) return;
  /* Development!
  if (ci->icon == 0) return;*/

  switch(ci->ctype)
  {
    case CONFIG_INT:
      wimp_window_write_icon_number(ConfWindows[ci->id.win], ci->id.icon, (int)val);
      break;
    case CONFIG_SELECT:
      wimp_window_set_icon_state(ConfWindows[ci->id.win], ci->id.icon, (val == 0) ? 0 : IFlg_Slct, IFlg_Slct);
      break;
    case CONFIG_STRING:
      wimp_window_write_icon_text(ConfWindows[ci->id.win], ci->id.icon, (char*)val);
      break;
    default: break;
  }
}


static void ui_setup_config_window(int wnum)
{
  int i;

  /* Setup window information */
  for (i=0; Configurations[i].resource != NULL; i++)
  {
    if (Configurations[i].id.win == wnum)
    {
      ui_setup_config_item(Configurations + i);
    }
  }

  /* Setup menus */
  switch (wnum)
  {
    case CONF_WIN_DRIVES:
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueType8);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueType9);
      if (machine_class != VICE_MACHINE_PET)
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueSync);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueIdle8);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueExtend8);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueIdle9);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayTrueExtend9);
      }
      break;
    case CONF_WIN_DEVICES:
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayPrintDev);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayUserDev);
      if ((machine_class == VICE_MACHINE_C64) || (machine_class == VICE_MACHINE_C128))
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayRsUserDevice);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayAciaDevice);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplaySerialBaud);
      }
      break;
    case CONF_WIN_SOUND:
      ui_setup_menu_display((disp_desc_t*)&MenuDisplaySampleRate);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplaySoundDevice);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplaySoundOver);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplaySoundBuffer);
      if ((machine_class == VICE_MACHINE_C64) || (machine_class == VICE_MACHINE_C128))
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplaySidModel);
      }
      break;
    case CONF_WIN_SYSTEM:
      ui_setup_menu_display((disp_desc_t*)&MenuDisplaySpeedLimit);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayRefresh);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayDosName);
      if (MenuDisplayROMSet != NULL)
      {
        ui_setup_menu_disp_core((disp_desc_t*)MenuDisplayROMSet, ROMSetName);
      }
      if (machine_class == VICE_MACHINE_C64)
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayCartridgeType);
      }
      break;
    case CONF_WIN_JOY:
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayJoyDevice1);
      ui_setup_menu_display((disp_desc_t*)&MenuDisplayJoyDevice2);
      {
        RO_Window *w = ConfWindows[CONF_WIN_JOY];
        Joy_Keys *jk;
        int i;

        for (i=0; i<2; i++)
        {
          jk = JoystickKeys + i;
          wimp_window_write_icon_text(w, JoyToIcon[i].up, IntKeyToString[jk->up]);
          wimp_window_write_icon_text(w, JoyToIcon[i].down, IntKeyToString[jk->down]);
          wimp_window_write_icon_text(w, JoyToIcon[i].left, IntKeyToString[jk->left]);
          wimp_window_write_icon_text(w, JoyToIcon[i].right, IntKeyToString[jk->right]);
          wimp_window_write_icon_text(w, JoyToIcon[i].fire, IntKeyToString[jk->fire]);
        }
      }
      break;
    case CONF_WIN_PET:
      if (machine_class == VICE_MACHINE_PET)
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetMemory);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetIO);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetVideo);
        ui_setup_menu_disp_core((disp_desc_t*)&MenuDisplayPetModel, PetModelName);
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_PET], Icon_Conf_PetKbd, pet_get_keyboard_name());
      }
      break;
    case CONF_WIN_VIC:
      if (machine_class == VICE_MACHINE_VIC20)
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayVicRam);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayVicCartridge);
      }
      break;
    case CONF_WIN_CBM2:
      if (machine_class == VICE_MACHINE_CBM2)
      {
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2Line);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2Memory);
        ui_setup_menu_disp_core((disp_desc_t*)&MenuDisplayCBM2Model, CBM2ModelName);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2RAM);
        ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2Cartridge);
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_Conf_CBM2Kbd, cbm2_get_keyboard_name());
      }
      break;
    default: break;
  }
}


static void ui_open_config_window(int wnum)
{
  int block[WindowB_WFlags+1];
  RO_Window *w;

  w = ConfWindows[wnum];
  block[WindowB_Handle] = w->Handle;
  Wimp_GetWindowState(block);
  block[WindowB_Stackpos] = -1;
  /* Already open? ==> just raise */
  if ((block[WindowB_WFlags] & (1<<16)) == 0)
  {
    int dx, dy, i;

    for (i=0; i<CONF_WIN_NUMBER; i++)
    {
      if (ConfWinPositions[i] == NULL)
      {
        ConfWinPositions[i] = w; break;
      }
    }
    /* Use the visible area stored in the template */
    dx = w->vmaxx - w->vminx; dy = w->vmaxy - w->vminy;
    block[WindowB_VMinX] = TitleBarOffset * i;
    block[WindowB_VMaxY] = ScreenMode.resy - TitleBarOffset * (i + 1);
    block[WindowB_VMaxX] = block[WindowB_VMinX] + dx;
    block[WindowB_VMinY] = block[WindowB_VMaxY] - dy;
    block[WindowB_ScrollX] = 0; block[WindowB_ScrollY] = 0;

    ui_setup_config_window(wnum);

    if (wnum == CONF_WIN_JOY) JoystickWindowOpen = 1;
  }
  Wimp_OpenWindow(block);
}


void ui_open_emu_window(int *b)
{
  int aux[WindowB_Stackpos+1];
  int paneblk[WindowB_Stackpos+1];
  int *block;
  int dx, dy, x;

  if (b == NULL)
  {
    int dx, dy;

    block = aux;
    aux[WindowB_Handle] = EmuWindow->Handle;
    dx = EmuWindow->wmaxx - EmuWindow->wminx;
    dy = EmuWindow->wmaxy - EmuWindow->wminy;
    aux[WindowB_VMinX] = (ScreenMode.resx - dx)/2; aux[WindowB_VMaxX] = aux[WindowB_VMinX]+dx;
    aux[WindowB_VMinY] = (ScreenMode.resy - dy)/2; aux[WindowB_VMaxY] = aux[WindowB_VMinY]+dy;
    aux[WindowB_ScrollX] = 0; aux[WindowB_ScrollY] = 0;
    aux[WindowB_Stackpos] = -1;
  }
  else
  {
    block = b;
  }

  /* Should the pane be displayed? */
  if (ShowPane != 0)
  {
    paneblk[WindowB_Handle] = EmuPane->Handle;
    dx = EmuPane->wmaxx - EmuPane->wminx;
    dy = EmuPane->wmaxy - EmuPane->wminy;
    x = block[WindowB_VMinX] - dx;
    if (x < 0)
    {
      if (block[WindowB_VMinX] < 0) x = block[WindowB_VMinX]; else x = 0;
    }
    paneblk[WindowB_VMinX] = x;
    paneblk[WindowB_VMaxX] = paneblk[WindowB_VMinX] + dx;
    paneblk[WindowB_VMaxY] = block[WindowB_VMaxY];
    paneblk[WindowB_VMinY] = paneblk[WindowB_VMaxY] - dy;
    paneblk[WindowB_Stackpos] = block[WindowB_Stackpos];

    /* Open pane */
    Wimp_OpenWindow(paneblk);

    /* Only open emu window behind pane when pane is actually displayed */
    block[WindowB_Stackpos] = EmuPane->Handle;
  }

  Wimp_OpenWindow(block);
}


void ui_close_emu_window(int *b)
{
  int aux[1];
  int *block;

  if (b == NULL)
  {
    block = aux;
    aux[0] = EmuWindow->Handle;
  }
  else
  {
    block = b;
  }
  Wimp_CloseWindow(block);

  aux[0] = EmuPane->Handle;
  Wimp_CloseWindow(aux);

  if (AutoPauseEmu != 0)
  {
    EmuPaused = 1; WasAutoPaused = 1;
    ui_display_paused(EmuPaused);
  }
}


void ui_toggle_truedrv_emulation(void)
{
  ui_set_truedrv_emulation(!TrueDriveEmulation);
}


void ui_toggle_sid_emulation(void)
{
  resource_value_t new_state;

  resources_toggle(Rsrc_Sound, &new_state);
  ui_set_sound_enable((int)new_state);
}




/*
 *  Wimp event handling code
 */
static void ui_redraw_window(int *b)
{
  int more;

  if (b[RedrawB_Handle] == EmuWindow->Handle)
  {
    graph_env ge;

    if (FrameBufferUpdate != 0)
    {
      int i, j;
      unsigned char map[256];
      unsigned char *f;
      unsigned int *out;

      memset(map, 0, 256);

      for (i=0; i<16; i++) map[oldColours[i]] = i;

      j = (FrameBuffer->tmpframebufferlinesize) * (FrameBuffer->height);
      f = (unsigned char*)(FrameBuffer->tmpframebuffer);
      out = (unsigned int*)f;

      for (i=0; i<256; i++)
      {
        map[i] = (unsigned char)((EmuCanvas->pixel_translation)[map[i]]);
      }

      for (i=0; i<j; i+=4)
      {
        *out++ = map[f[i]] | (map[f[i+1]] << 8) | (map[f[i+2]] << 16) | (map[f[i+3]] << 24);
      }

      FrameBufferUpdate = 0;
    }

    more = Wimp_RedrawWindow(b);
    while (more != 0)
    {
      ge.x = b[RedrawB_VMinX] - b[RedrawB_ScrollX] + (EmuCanvas->shiftx << UseEigen)*EmuZoom;
      ge.y = b[RedrawB_VMaxY] - b[RedrawB_ScrollY] + (EmuCanvas->shifty << UseEigen)*EmuZoom;
      ge.dimx = FrameBuffer->width; ge.dimy = FrameBuffer->height;

      if (EmuZoom == 1)
      {
        PlotZoom1(&ge, b + RedrawB_CMinX, FrameBuffer->tmpframebuffer, ColourTable);
      }
      else
      {
        PlotZoom2(&ge, b + RedrawB_CMinX, FrameBuffer->tmpframebuffer, ColourTable);
      }

      more = Wimp_GetRectangle(b);
    }
  }
  else if (b[RedrawB_Handle] == ImgContWindow->Handle)
  {
    ui_image_contents_redraw(b);
  }
  else
  {
    more = Wimp_RedrawWindow(b);
    while (more != 0)
    {
      if (b[RedrawB_Handle] == ConfWindows[CONF_WIN_SOUND]->Handle)
      {
        ui_draw_sound_volume(b);
      }
      more = Wimp_GetRectangle(b);
    }
  }
}


static void ui_open_window(int *b)
{
  if (b[WindowB_Handle] == EmuWindow->Handle)
  {
    ui_open_emu_window(b);
  }
  else
  {
    Wimp_OpenWindow(b);
  }
}


static void ui_close_window(int *b)
{
  RO_Caret currentCaret;

  Wimp_GetCaretPosition(&currentCaret);
  if ((currentCaret.WHandle == b[WindowB_Handle]) && (LastCaret.WHandle != -1))
  {
    Wimp_SetCaretPosition(LastCaret.WHandle, LastCaret.IHandle, LastCaret.offx, LastCaret.offy, LastCaret.height, LastCaret.index);
    LastCaret.WHandle = -1;
  }

  if (b[WindowB_Handle] == EmuWindow->Handle)
  {
    ui_close_emu_window(b);
  }
  else if (b[WindowB_Handle] == ImgContWindow->Handle)
  {
    ui_image_contents_close();
  }
  else
  {
    int i;

    for (i=0; i<CONF_WIN_NUMBER; i++)
    {
      if (ConfWinPositions[i] != NULL)
      {
        if (ConfWinPositions[i]->Handle == b[WindowB_Handle])
        {
          if (i == CONF_WIN_JOY) JoystickWindowOpen = 0;
          ConfWinPositions[i] = NULL; break;
        }
      }
    }
    Wimp_CloseWindow(b);
  }
}


static void ui_set_emu_window_size(void)
{
  int dx, dy;

  UseEigen = (ScreenMode.eigx < ScreenMode.eigy) ? ScreenMode.eigx : ScreenMode.eigy;
  if (EmuCanvas == NULL)
  {
    dx = (VIC_II_SCREEN_WIDTH << UseEigen) * EmuZoom;
    dy = (VIC_II_SCREEN_HEIGHT << UseEigen) * EmuZoom;
  }
  else
  {
    dx = (EmuCanvas->width << UseEigen) * EmuZoom;
    dy = (EmuCanvas->height << UseEigen) * EmuZoom;
  }
  wimp_window_set_extent(EmuWindow, 0, -dy, dx, 0);
}


/* Try setting a resource according to a selection box or update state if failed */
static int ui_set_resource_select(const char *name, conf_icon_id *id)
{
  int block[10];
  int selected;

  wimp_window_get_icon_state(ConfWindows[id->win], id->icon, block);
  selected = ((block[6] & IFlg_Slct) == 0) ? 0 : 1;

  if (resources_set_value(name, (resource_value_t)selected) != 0)
  {
    /* Revert to previous state */
    wimp_window_set_icon_state(ConfWindows[id->win], id->icon, (selected == 0) ? IFlg_Slct : 0, IFlg_Slct);
    selected ^= 1;
  }
  return selected;
}


static void ui_mouse_click(int *b)
{
  if (b[MouseB_Window] == EmuPane->Handle)
  {
    if (b[MouseB_Buttons] != 2)
    {
      switch (b[MouseB_Icon])
      {
        case Icon_Pane_Toggle:
          {
            int block[WindowB_WFlags+1];
            int dx, dy;

            if (EmuZoom == 1)
            {
              EmuZoom = 2;
              wimp_window_write_icon_text(EmuPane, Icon_Pane_Toggle, SymbolStrings[Symbol_Zoom1]);
            }
            else
            {
              EmuZoom = 1;
              wimp_window_write_icon_text(EmuPane, Icon_Pane_Toggle, SymbolStrings[Symbol_Zoom2]);
            }
            ui_set_emu_window_size();
            block[WindowB_Handle] = EmuWindow->Handle;
            Wimp_GetWindowState(block);
            dx = EmuWindow->wmaxx - EmuWindow->wminx;
            dy = EmuWindow->wmaxy - EmuWindow->wminy;
            block[WindowB_VMaxX] = block[WindowB_VMinX] + dx;
            block[WindowB_VMinY] = block[WindowB_VMaxY] - dy;
            Wimp_OpenWindow(block);
            Wimp_GetWindowState(block);
            ui_open_emu_window(block);
            Wimp_ForceRedraw(EmuWindow->Handle, 0, -dy, dx, 0);
          }
          break;
        case Icon_Pane_Reset:
          if (b[MouseB_Buttons] == 1) mem_powerup();	/* adjust ==> hard reset */
          maincpu_trigger_reset();
          break;
        case Icon_Pane_Pause:
          EmuPaused ^= 1;
          ui_display_paused(EmuPaused);
          break;
        case Icon_Pane_Speed:
          DisplayFPS ^= 1;
          break;
        case Icon_Pane_TrkSec:
          if (TrueDriveEmulation != 0)
          {
            DisplayDriveTrack ^= 1;
            ui_display_drive_track(DisplayDriveTrack, tracknums[DisplayDriveTrack]);
          }
          break;
        default:
          break;
      }
    }
  }
  else if (b[MouseB_Window] == EmuWindow->Handle)
  {
    if (b[MouseB_Buttons] == 2)
    {
      Wimp_CreateMenu((int*)&MenuEmuWindow, b[MouseB_PosX], b[MouseB_PosY]);
      LastMenu = Menu_Emulator;
    }
    else
    {
      Wimp_GetCaretPosition(&LastCaret);
      Wimp_SetCaretPosition(EmuWindow->Handle, -1, -100, 100, -1, -1);
    }
  }
  else if ((b[MouseB_Window] == -2) && (b[MouseB_Icon] == IBarIcon.IconHandle))
  {
    if (b[MouseB_Buttons] == 2)
    {
      Wimp_CreateMenu((int*)(&MenuIconBar), b[MouseB_PosX] - MenuIconBar.head.width / 2, 96 + Menu_Height*Menu_IBar_Items);
      LastMenu = Menu_IBar;
    }
    else if (b[MouseB_Buttons] == 4)
    {
      int block[WindowB_WFlags+1];
      int gainCaret;

      block[0] = EmuWindow->Handle; gainCaret = 0;
      Wimp_GetWindowState(block);
      /* Window was closed? Then open centered... */
      if ((block[WindowB_WFlags] & (1<<16)) == 0)
      {
        int dx, dy;

        dx = EmuWindow->wmaxx - EmuWindow->wminx;
        dy = EmuWindow->wmaxy - EmuWindow->wminy;
        block[WindowB_VMinX] = (ScreenMode.resx - dx) / 2;
        block[WindowB_VMaxX] = block[WindowB_VMinX] + dx;
        block[WindowB_VMinY] = (ScreenMode.resy - dy) / 2;
        block[WindowB_VMaxY] = block[WindowB_VMinY] + dy;
        gainCaret = 1;
      }
      block[WindowB_Stackpos] = -1;
      ui_open_emu_window(block);
      if (gainCaret != 0)
      {
        Wimp_GetCaretPosition(&LastCaret);
        Wimp_SetCaretPosition(EmuWindow->Handle, -1, -100, 100, -1, -1);
      }

      /* reverse autopause? */
      if ((AutoPauseEmu != 0) && (WasAutoPaused != 0))
      {
        WasAutoPaused = 0;
        if (EmuPaused != 0)
        {
          EmuPaused = 0;
          ui_display_paused(EmuPaused);
        }
      }
    }
    else if (b[MouseB_Buttons] == 1)
    {
      /* open config window */
      ui_open_config_window(CONF_WIN_DRIVES);
    }
  }
  else if (b[MouseB_Window] == SnapshotWindow->Handle)
  {
    /* Drag */
    if ((b[MouseB_Buttons] == 16) || (b[MouseB_Buttons] == 64))
    {
      if (b[MouseB_Icon] == Icon_Snap_Sprite)
      {
        LastDrag = DRAG_TYPE_SNAPSHOT;
        wimp_drag_icon_sprite(SnapshotWindow, b[MouseB_Icon], &ScreenMode, CMOS_DragType);
      }
    }
    else if ((b[MouseB_Buttons] == 1) || (b[MouseB_Buttons] == 4))
    {
      if (b[MouseB_Icon] == Icon_Snap_OK)
      {
        char *snapname;

        if ((snapname = wimp_window_read_icon_text(SnapshotWindow, Icon_Snap_Path)) != NULL)
          ui_check_save_snapshot(snapname);
      }
    }
  }
  else if (b[MouseB_Window] == SaveBox->Handle)
  {
    if ((b[MouseB_Buttons] == 16) || (b[MouseB_Buttons] == 64))
    {
      if (b[MouseB_Icon] == Icon_Save_Sprite)
      {
        LastDrag = DRAG_TYPE_SAVEBOX;
        wimp_drag_icon_sprite(SaveBox, b[MouseB_Icon], &ScreenMode, CMOS_DragType);
      }
    }
    else if ((b[MouseB_Buttons] == 1) || (b[MouseB_Buttons] == 4))
    {
      if (b[MouseB_Icon] == Icon_Save_OK)
      {
        char *savename;

        if ((savename = wimp_window_read_icon_text(SaveBox, Icon_Save_Path)) != NULL)
          ui_check_save_sbox(savename);
      }
    }
  }
  else
  {
    int wnum;
    RO_Window *win;

    for (wnum = 0; wnum < CONF_WIN_NUMBER; wnum++)
    {
      if ((win = ConfWindows[wnum]) == NULL) break;

      if (b[MouseB_Window] == win->Handle)
      {
        /* Select and adjust only */
        if ((b[MouseB_Buttons] == 1) || (b[MouseB_Buttons] == 4))
        {
          int i;

          for (i=0; ConfigMenus[i].menu != NULL; i++)
          {
            if ((ConfigMenus[i].id.win == wnum) && (ConfigMenus[i].id.icon == b[MouseB_Icon]))
            {
              RO_MenuHead *menu = ConfigMenus[i].menu;
              RO_Icon *icon;
              int wb[WindowB_WFlags+1];

              icon = wimp_window_get_icon(win, ConfigMenus[i].id.icon);
              wb[WindowB_Handle] = win->Handle;
              Wimp_GetWindowState(wb);	/* For absolute coordinates of menu item */

              switch (i)
              {
                case CONF_MENU_PRNTDEV:
                  break;
                case CONF_MENU_PRUSER:
                  break;
                case CONF_MENU_SAMPRATE:
                  break;
                case CONF_MENU_SOUNDDEV:
                  break;
                case CONF_MENU_SOUNDOVER:
                  break;
                case CONF_MENU_TRUESYNC:
                  break;
                case CONF_MENU_TRUEIDLE8:
                  break;
                case CONF_MENU_TRUEEXT8:
                  break;
                case CONF_MENU_TRUETYPE8:
                  break;
                case CONF_MENU_TRUEIDLE9:
                  break;
                case CONF_MENU_TRUEEXT9:
                  break;
                case CONF_MENU_TRUETYPE9:
                  break;
                case CONF_MENU_DRIVE8:
                case CONF_MENU_DRIVE9:
                case CONF_MENU_DRIVE10:
                case CONF_MENU_DRIVE11:
                  {
                    serial_t *sd;
                    int number = i-CONF_MENU_DRIVE8;
                    int j = -1;
                    unsigned int flags;
                    int state;
		    RO_MenuHead *submenu = (RO_MenuHead*)&MenuDriveFS;

                    sd = serial_get_device(number + 8);
                    if (strstr(sd->name, "Disk Drive") != NULL)
                    {
                      j = Menu_DriveType_Disk;
                      *(DriveTypes[number]) = DRIVE_TYPE_DISK;
                    }
                    else if (strstr(sd->name, "FS Drive") != NULL)
                    {
                      j = Menu_DriveType_FS;
                      *(DriveTypes[number]) = DRIVE_TYPE_FS;
                    }
		    /* Configure submenu */
		    flags = 0;
		    if (resources_get_value(Rsrc_ConvP00[number], (resource_value_t*)&state) == 0)
		    {
		      if (state != 0) flags |= (1<<Menu_DriveFS_ConvP00);
		    }
		    if (resources_get_value(Rsrc_SaveP00[number], (resource_value_t*)&state) == 0)
		    {
		      if (state != 0) flags |= (1<<Menu_DriveFS_SaveP00);
		    }
		    if (resources_get_value(Rsrc_HideCBM[number], (resource_value_t*)&state) == 0)
		    {
		      if (state != 0) flags |= (1<<Menu_DriveFS_HideCBM);
		    }
		    if (j >= 0)
		    {
		      wimp_menu_tick_exclusive(menu, j);
		      wimp_menu_set_grey_all(submenu, (j != Menu_DriveType_FS));
		      wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveDisk, (j != Menu_DriveType_Disk));
		      wimp_menu_tick_slct(submenu, flags);
		    }
                  }
                  break;
                case CONF_MENU_CARTTYPE:
                  break;
                case CONF_MENU_RSUSRDEV:
                  break;
                case CONF_MENU_ACIADEV:
                  break;
                case CONF_MENU_SERIAL:
                  break;
                case CONF_MENU_SIDMODEL:
                  break;
                case CONF_MENU_SPEED:
                  break;
                case CONF_MENU_REFRESH:
                  break;
                case CONF_MENU_PETMEM:
                  break;
                case CONF_MENU_PETIO:
                  break;
                case CONF_MENU_PETVIDEO:
                  break;
                case CONF_MENU_PETMODEL:
                  break;
                case CONF_MENU_VICRAM:
                  break;
                case CONF_MENU_VICCART:
                  break;
                case CONF_MENU_DOSNAME:
                  break;
                case CONF_MENU_C2LINE:
                  break;
                case CONF_MENU_C2MEM:
                  break;
                case CONF_MENU_C2MODEL:
                  break;
                case CONF_MENU_C2RAM:
                  break;
                case CONF_MENU_C2CART:
                  break;
                case CONF_MENU_SNDBUFF:
                  break;
                case CONF_MENU_JOYDEV1:
                  break;
                case CONF_MENU_JOYDEV2:
                  break;
                default:
                  break;
              }
              LastMenu = 256 + i;
              Wimp_CreateMenu((int*)menu, wb[WindowB_VMinX] - wb[WindowB_ScrollX] + icon->maxx, wb[WindowB_VMaxY] - wb[WindowB_ScrollY] + icon->maxy);
              break;
            }
          }
          /* Haven't found anything? */
          if (ConfigMenus[i].menu == NULL)
          {
            for (i=0; Configurations[i].resource != NULL; i++)
            {
              if ((Configurations[i].id.win == wnum) && (Configurations[i].id.icon == b[MouseB_Icon]))
              {
                if (Configurations[i].ctype == CONFIG_SELECT)
                {
                  int s;

                  s = ui_set_resource_select(Configurations[i].resource, &(Configurations[i].id));
                  /* Special cases */
                  if ((Configurations[i].id.win == CONF_WIN_DRIVES) && (Configurations[i].id.icon == Icon_Conf_TrueDrv))
                  {
                    ui_set_truedrv_emulation(s);
                  }
                  if ((Configurations[i].id.win == CONF_WIN_SOUND) && (Configurations[i].id.icon == Icon_Conf_SoundOn))
                  {
                    ui_set_sound_enable(s);
                  }
                }
                break;
              }
            }
            /* Haven't found anything there either? */
            if (Configurations[i].resource == NULL)
            {
              if (b[MouseB_Window] == ConfWindows[CONF_WIN_DEVICES]->Handle)
              {
                if (b[MouseB_Icon] == Icon_Conf_FileRsOK)
                {
                  char *fn;

                  if ((fn = wimp_window_read_icon_text(win, Icon_Conf_FileRsPath)) != NULL)
                    ui_set_serial_file(wimp_strterm(fn));
                }
                else if (b[MouseB_Icon] == Icon_Conf_FilePrOK)
                {
                  char *fn;

                  if ((fn = wimp_window_read_icon_text(win, Icon_Conf_FilePrPath)) != NULL)
                    ui_set_printer_file(wimp_strterm(fn));
                }
              }
              else if (b[MouseB_Window] == ConfWindows[CONF_WIN_SOUND]->Handle)
              {
                if (b[MouseB_Icon] == Icon_Conf_FileSndOK)
                {
                  char *fn;

                  if ((fn = wimp_window_read_icon_text(win, Icon_Conf_FileSndPath)) != NULL)
                  ui_set_sound_file(wimp_strterm(fn));
                }
                else if (b[MouseB_Icon] == Icon_Conf_Volume)
                {
                  ui_drag_sound_volume(b);
                  Sound_Volume(SoundVolume);
                }
              }
              else if (b[MouseB_Window] == ConfWindows[CONF_WIN_JOY]->Handle)
              {
                Wimp_GetCaretPosition(&LastCaret);
                Wimp_SetCaretPosition(ConfWindows[CONF_WIN_JOY]->Handle, -1, -100, 100, -1, -1);
              }
              else if (b[MouseB_Window] == ConfWindows[CONF_WIN_DRIVES]->Handle)
              {
                tape_detach_image();
                wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TapeFile, "");
                wimp_window_set_icon_state(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TapeDetach, IFlg_Grey, IFlg_Grey);
              }
            }
          }
        }
        /* Drag? */
        else if ((b[MouseB_Buttons] == 16) || (b[MouseB_Buttons] == 64))
        {
          int i = DRAG_TYPE_NONE;

          if (b[MouseB_Window] == ConfWindows[CONF_WIN_DEVICES]->Handle)
          {
            if (b[MouseB_Icon] == Icon_Conf_FileRsIcon)
              i = DRAG_TYPE_SERIAL;
            else if (b[MouseB_Icon] == Icon_Conf_FilePrIcon)
              i = DRAG_TYPE_PRINTER;
          }
          else if (b[MouseB_Window] == ConfWindows[CONF_WIN_SOUND]->Handle)
          {
            if (b[MouseB_Icon] == Icon_Conf_FileSndIcon)
              i = DRAG_TYPE_SOUND;
            else if (b[MouseB_Icon] == Icon_Conf_Volume)
            {
              int wstate[WindowB_WFlags+1];
              int dblk[DragB_BBMaxY+1];
              RO_Icon *icon;

              wstate[WindowB_Handle] = ConfWindows[CONF_WIN_SOUND]->Handle;
              Wimp_GetWindowState(wstate);
              icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_Conf_Volume);
              dblk[DragB_Handle] = ConfWindows[CONF_WIN_SOUND]->Handle;
              dblk[DragB_Type] = 7;
              dblk[DragB_IMinX] = b[MouseB_PosX];
              dblk[DragB_IMaxX] = b[MouseB_PosX];
              dblk[DragB_IMinY] = b[MouseB_PosY];
              dblk[DragB_IMaxY] = b[MouseB_PosY];
              dblk[DragB_BBMinX] = wstate[WindowB_VMinX] - wstate[WindowB_ScrollX] + icon->minx;
              dblk[DragB_BBMinY] = wstate[WindowB_VMaxY] - wstate[WindowB_ScrollY] + icon->miny;
              dblk[DragB_BBMaxX] = dblk[DragB_BBMinX] + (icon->maxx - icon->minx) - (1<<ScreenMode.eigx);
              dblk[DragB_BBMaxY] = dblk[DragB_BBMinY] + (icon->maxy - icon->miny) - (1<<ScreenMode.eigy);
              Wimp_DragBox(dblk);
              i = DRAG_TYPE_VOLUME;
            }
          }
          if (i != DRAG_TYPE_NONE)
          {
            LastDrag = i;
            if (i != DRAG_TYPE_VOLUME)
              wimp_drag_icon_sprite(win, b[MouseB_Icon], &ScreenMode, CMOS_DragType);
          }
        }
        break;
      }
    }
  }
}


/*
 *  Called by the emulator when everything is ready for a snapshot. It is
 *  of utmost importance that this function polls the WIMP immediately after
 *  having dispatched the message rather than wait for the next regular polling
 *  because the emulator must NOT be allowed to continue running once the machine
 *  is ready for the snapshot.
 */
static void ui_drag_snapshot_trap(ADDRESS unused_address, void *unused_data)
{
  int block[64];

  Wimp_SendMessage(18, SnapshotMessage, SnapshotMessage[5], SnapshotMessage[6]);

  WithinUiPoll++;

  /* Get ready for polling. Except for the loop this is identical to ui_poll */
  ui_poll_core(block);

  WithinUiPoll--;

  if ((WithinUiPoll == 0) && (SoundSuspended != 0)) sound_resume();

  ui_poll_epilogue();
}


static void ui_user_drag_box(int *b)
{
  int iconnum=-1, filetype=FileType_Data;
  RO_Window *win=NULL;
  int estsize=0;

  if (LastDrag == DRAG_TYPE_VOLUME)
  {
    Wimp_DragBox(NULL);
    Sound_Volume(SoundVolume);
    LastDrag = DRAG_TYPE_NONE;
    return;
  }

  if (CMOS_DragType == 0)
  {
    Wimp_DragBox(NULL);
  }
  else
  {
    DragASprite_Stop();
  }

  switch (LastDrag)
  {
    case DRAG_TYPE_SOUND:
      iconnum = Icon_Conf_FileSndPath; win = ConfWindows[CONF_WIN_SOUND]; break;
    case DRAG_TYPE_SERIAL:
      iconnum = Icon_Conf_FileRsPath; win = ConfWindows[CONF_WIN_DEVICES]; break;
    case DRAG_TYPE_PRINTER:
      iconnum = Icon_Conf_FilePrPath; win = ConfWindows[CONF_WIN_DEVICES]; break;
    case DRAG_TYPE_SNAPSHOT:
      iconnum = Icon_Snap_Path; win = SnapshotWindow; break;
    case DRAG_TYPE_SAVEBOX:
      iconnum = Icon_Save_Path; win = SaveBox; break;
    default: break;
  }

  if (win != NULL)
  {
    int block[64];

    Wimp_GetPointerInfo(block);
    if ((block[MouseB_Window] != -1) && ((block[MouseB_Window] != -2) || (block[MouseB_Icon] != IBarIcon.IconHandle)))
    {
      int h = block[MouseB_Window];
      int i;

      for (i=0; i<CONF_WIN_NUMBER; i++)
      {
        if (ConfWindows[i] == NULL) break;
        if (h == ConfWindows[i]->Handle) {h = 0; break;}
      }

      if ((h != EmuWindow->Handle) && (h != EmuPane->Handle) && (h != SaveBox->Handle) &&
          (h != 0))
      {
        char *name;

        if ((name = wimp_window_read_icon_text(win, iconnum)) == NULL)
        {
          LastDrag = DRAG_TYPE_NONE; return;
        }
        name = wimp_get_leaf_name(name);
        block[5] = block[MouseB_Window]; block[6] = block[MouseB_Icon];
        block[7] = block[MouseB_PosX]; block[8] = block[MouseB_PosY];
        block[9] = estsize; block[10] = filetype;
        wimp_strcpy(((char*)block)+44, name);
        block[MsgB_Size] = 44 + ((wimp_strlen(name) + 4) & ~3);
        block[MsgB_YourRef] = 0; block[MsgB_Action] = Message_DataSave;

        /* Have to install callback in this case */
        if (LastDrag == DRAG_TYPE_SNAPSHOT)
        {
          memcpy(SnapshotMessage, block, 256);
          maincpu_trigger_trap(ui_drag_snapshot_trap, NULL);
          SnapshotPending = 1;
        }
        else
        {
          Wimp_SendMessage(18, block, block[5], block[6]);
        }
      }
    }
  }
  else
  {
    LastDrag = DRAG_TYPE_NONE;
  }
}


static int ui_poll_joystick_window(int icon)
{
  if (wimp_window_has_input_focus(ConfWindows[CONF_WIN_JOY]) != 0)
  {
    unsigned char *dest = NULL;
    int i, code;

    for (i=0; i<2; i++)
    {
      if (icon == JoyToIcon[i].up) dest = &(JoystickKeys[i].up);
      else if (icon == JoyToIcon[i].down) dest = &(JoystickKeys[i].down);
      else if (icon == JoyToIcon[i].left) dest = &(JoystickKeys[i].left);
      else if (icon == JoyToIcon[i].right) dest = &(JoystickKeys[i].right);
      else if (icon == JoyToIcon[i].fire) dest = &(JoystickKeys[i].fire);
      if (dest != NULL) break;
    }
    if (dest == NULL) return 0;
    if ((code = ScanKeys(IntKey_MinCode)) != 0xff)
    {
      char *b;

      if (*dest != (unsigned char)code)
      {
        *dest = (unsigned char)code;
        if ((b = IntKeyToString[code]) != NULL)
        {
          wimp_window_write_icon_text(ConfWindows[CONF_WIN_JOY], icon, b);
        }
        kbd_update_joykeys(i);
      }
    }
    return 1;
  }
  return 0;
}


static void ui_key_press(int *b)
{
  int key;
  int wnum;

  key = b[KeyPB_Key];

  if (b[KeyPB_Window] == EmuWindow->Handle)
  {
    switch (key)
    {
      case 0x189:
        ShowPane ^= 1;
        ui_set_pane_state(ShowPane);
        break;
      case 0x18b:
        EmuPaused ^= 1;
        ui_display_paused(EmuPaused);
        break;
      case 0x1cc: /* pass on any variations of F12 */
      case 0x1dc:
      case 0x1ec:
      case 0x1fc: Wimp_ProcessKey(key); break;
      default: break;
    }
    return;
  }
  else if (b[KeyPB_Window] == SnapshotWindow->Handle)
  {
    if ((b[KeyPB_Icon] == Icon_Snap_Path) && (key == Key_Return))
    {
      char *fn;

      if ((fn = wimp_window_read_icon_text(SnapshotWindow, Icon_Snap_Path)) != NULL)
        ui_check_save_snapshot(fn);
    }
  }
  if (b[KeyPB_Window] == ConfWindows[CONF_WIN_JOY]->Handle)
  {
    int mpos[MouseB_Icon+1];

    Wimp_GetPointerInfo(mpos);
    if (ui_poll_joystick_window(mpos[MouseB_Icon]) != 0) return;
  }
  if (key == Key_Return)
  {
    if (b[KeyPB_Window] == SaveBox->Handle)
    {
      char *savename;

      if ((savename = wimp_window_read_icon_text(SaveBox, Icon_Save_Path)) != NULL)
        ui_check_save_sbox(savename);
      return;
    }

    for (wnum=0; wnum < CONF_WIN_NUMBER; wnum++)
    {
      char *data;
      int i=0;

      if (ConfWindows[wnum] == NULL) break;

      if (b[KeyPB_Window] != ConfWindows[wnum]->Handle) continue;

      if ((data = wimp_window_read_icon_text(ConfWindows[wnum], b[KeyPB_Icon])) == NULL)
        return;
      wimp_strterm(data);

      switch (wnum)
      {
        case CONF_WIN_DRIVES:
          switch (b[KeyPB_Icon])
          {
            case Icon_Conf_DriveFile11: i++;
            case Icon_Conf_DriveFile10: i++;
            case Icon_Conf_DriveFile9: i++;
            case Icon_Conf_DriveFile8:
              {
                int info[4];
                int status;

                status = ReadCatalogueInfo(data, info);
                if (status == 0) return;
                switch (status)
                {
                  case 1:
                    ui_set_drive_image(i, data); break;
                    break;
                  case 2:
                    ui_set_drive_dir(i, data); break;
                  case 3:
                    if (*(DriveTypes[i]) == DRIVE_TYPE_FS)
                    {
                      ui_set_drive_dir(i, data);
                    }
                    else
                    {
                      ui_set_drive_image(i, data);
                    }
                    break;
                  default: break;
                }
              }
            default: Wimp_ProcessKey(key); return;
          }
          break;
        case CONF_WIN_SYSTEM:
          switch (b[KeyPB_Icon])
          {
            case Icon_Conf_PollEvery: PollEvery = atoi(data); break;
            case Icon_Conf_SpeedEvery: SpeedEvery = atoi(data); break;
            case Icon_Conf_SoundEvery: SoundPollEvery = atoi(data); break;
            case Icon_Conf_CharGen:
              resources_set_value(Rsrc_CharGen, (resource_value_t)data); break;
            case Icon_Conf_Kernal:
              resources_set_value(Rsrc_Kernal, (resource_value_t)data); break;
            case Icon_Conf_Basic:
              resources_set_value(Rsrc_Basic, (resource_value_t)data); break;
            case Icon_Conf_Palette:
              resources_set_value(Rsrc_Palette, (resource_value_t)data); break;
            case Icon_Conf_CartFile: ui_set_cartridge_file(data); break;
            case Icon_Conf_DosName:
              ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayDosName, (resource_value_t)data);
              break;
            default: Wimp_ProcessKey(key); return;
          }
          break;
        case CONF_WIN_DEVICES:
          if (b[KeyPB_Icon] == Icon_Conf_FileRsPath)
            ui_set_serial_file(data);
          else if (b[KeyPB_Icon] == Icon_Conf_FilePrPath)
            ui_set_printer_file(data);
          else
          {
            Wimp_ProcessKey(key); return;
          }
          break;
        case CONF_WIN_SOUND:
          if (b[KeyPB_Icon] == Icon_Conf_FileSndPath)
            ui_set_sound_file(data);
          else
          {
            Wimp_ProcessKey(key); return;
          }
          break;
        case CONF_WIN_VIC:
          if (b[KeyPB_Icon] == Icon_Conf_VICCartF)
            ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayVicCartridge, (resource_value_t)data);
          else
          {
            Wimp_ProcessKey(key); return;
          }
          break;
        case CONF_WIN_CBM2:
          if (b[KeyPB_Icon] == Icon_Conf_CBM2CartF)
          {
            ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayCBM2Cartridge, (resource_value_t)data);
          }
          else
          {
            Wimp_ProcessKey(key);
          }
          break;
        default: Wimp_ProcessKey(key); return;
      }
    }
    return;
  }

  Wimp_ProcessKey(key);
}


/* In case a resource can't be set to a specific value revert the menu tick */
static void ui_toggle_resource_menu(const char *name, RO_MenuHead *menu, int number)
{
  int state;

  state = wimp_menu_tick_item(menu, number, -1);
  if (resources_set_value(name, (resource_value_t)state) != 0)
  {
    wimp_menu_tick_item(menu, number, -1);
  }
}


static config_item PETdependconf[] = {
  {Rsrc_PetCrt, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetCrt}},
  {Rsrc_PetRAM9, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetRAM9}},
  {Rsrc_PetRAMA, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetRAMA}},
  {Rsrc_PetDiag, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetDiagPin}},
  {Rsrc_PetSuper, CONFIG_SELECT, {CONF_WIN_PET, Icon_Conf_PetSuper}},
  {NULL, 0, {0, 0}}
};

static config_item SystemROMconf[] = {
  {Rsrc_CharGen, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_CharGen}},
  {Rsrc_Kernal, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Kernal}},
  {Rsrc_Basic, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Basic}},
  {Rsrc_Palette, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_Conf_Palette}},
  {NULL, 0, {0, 0}}
};

/* Update all open config windows */
static void ui_update_config_windows(void)
{
  int i;

  for (i=0; i<CONF_WIN_NUMBER; i++)
  {
    if (ConfWindows[i] == NULL) break;

    if (wimp_window_open_status(ConfWindows[i]) != 0)
    {
      ui_setup_config_window(i);
    }
  }
}


/* Update the system ROM names */
static void ui_update_rom_names(void)
{
  int i;

  for (i=0; SystemROMconf[i].resource != NULL; i++)
  {
    ui_setup_config_item(SystemROMconf + i);
  }
}


static void ui_menu_selection(int *b)
{
  int block[MouseB_Icon+1];
  int *menu = NULL;

  Wimp_GetPointerInfo(block);

  if ((LastMenu == Menu_IBar) || (LastMenu == Menu_Emulator))
  {
    int confWindow = -1;

    if (LastMenu == Menu_IBar)
    {
      menu = (int*)&MenuIconBar;
      switch (b[0])
      {
        case Menu_IBar_Configure:
          if (b[1] != -1) confWindow = CONF_WIN_NUMBER;
          break;
        case Menu_IBar_Quit: ui_exit(); break;
        default: break;
      }
    }
    else if (LastMenu == Menu_Emulator)
    {
      menu = (int*)&MenuEmuWindow;
      switch (b[0])
      {
        case Menu_EmuWin_Configure:
          if (b[1] != -1) confWindow = CONF_WIN_NUMBER;
          break;
        case Menu_EmuWin_Freeze:
          cartridge_trigger_freeze();
          break;
        case Menu_EmuWin_Pane:
          ShowPane ^= 1;
          ui_set_pane_state(ShowPane);
          break;
        case Menu_EmuWin_TrueDrvEmu:
          ui_set_truedrv_emulation(!wimp_menu_tick_read((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_TrueDrvEmu));
          break;
        case Menu_EmuWin_Sound:
          ui_set_sound_enable(!wimp_menu_tick_read((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Sound));
          break;
        case Menu_EmuWin_Monitor:
          ui_activate_monitor();
          break;
        default: break;
      }
    }
    if (confWindow >= 0)
    {
      confWindow = -1;
      switch (b[1])
      {
        case Menu_Config_Drives: confWindow = CONF_WIN_DRIVES; break;
        case Menu_Config_Devices: confWindow = CONF_WIN_DEVICES; break;
        case Menu_Config_Sound: confWindow = CONF_WIN_SOUND; break;
        case Menu_Config_System: confWindow = CONF_WIN_SYSTEM; break;
        case Menu_Config_Joystick: confWindow = CONF_WIN_JOY; break;
        case Menu_Config_Machine:
          if (machine_class == VICE_MACHINE_PET)
            confWindow = CONF_WIN_PET;
          else if (machine_class == VICE_MACHINE_VIC20)
            confWindow = CONF_WIN_VIC;
          else if (machine_class == VICE_MACHINE_CBM2)
            confWindow = CONF_WIN_CBM2;
          break;
        case Menu_Config_Save:
          resources_save(NULL); break;
        case Menu_Config_Reload:
          resources_load(NULL);
          ui_update_config_windows();
          break;
        default: break;
      }

      if (confWindow >= 0)
        ui_open_config_window(confWindow);
    }
  }

  /* Configuration menu? */
  if (LastMenu >= 0x100)
  {
    menu = (int*)(ConfigMenus[LastMenu - 0x100].menu);

    switch (LastMenu - 0x100)
    {
      case CONF_MENU_PRNTDEV:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayPrintDev, b[0]);
        break;
      case CONF_MENU_PRUSER:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayUserDev, b[0]);
        break;
      case CONF_MENU_SAMPRATE:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySampleRate, b[0]);
        break;
      case CONF_MENU_SOUNDDEV:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySoundDevice, b[0]);
        break;
      case CONF_MENU_SOUNDOVER:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySoundOver, b[0]);
        break;
      case CONF_MENU_TRUESYNC:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueSync, b[0]);
        break;
      case CONF_MENU_TRUEIDLE8:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueIdle8, b[0]);
        break;
      case CONF_MENU_TRUEEXT8:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueExtend8, b[0]);
        break;
      case CONF_MENU_TRUETYPE8:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueType8, b[0]);
        break;
      case CONF_MENU_TRUEIDLE9:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueIdle9, b[0]);
        break;
      case CONF_MENU_TRUEEXT9:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueExtend9, b[0]);
        break;
      case CONF_MENU_TRUETYPE9:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayTrueType9, b[0]);
        break;
      case CONF_MENU_DRIVE8:
      case CONF_MENU_DRIVE9:
      case CONF_MENU_DRIVE10:
      case CONF_MENU_DRIVE11:
        {
          int number = LastMenu - (0x100 + CONF_MENU_DRIVE8);
          if (b[0] == Menu_DriveType_Disk)
          {
            if (b[1] != -1)
            {
              if (b[1] == Menu_DriveDisk_Detach)
              {
                ui_set_drive_dir(number, "@");
                wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveDisk, 1);
                wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveFS, 0);
                wimp_menu_tick_exclusive((RO_MenuHead*)menu, Menu_DriveType_FS);
              }
            }
            else
            {
              if (*(DriveTypes[number]) != DRIVE_TYPE_DISK)
              {
                char *fn;

                if ((fn = wimp_window_read_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number])) != NULL)
                {
                  if (ui_set_drive_image(number, wimp_strterm(fn)) == 0)
                  {
                    wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveDisk, 0);
                    wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveFS, 1);
                    wimp_menu_tick_exclusive((RO_MenuHead*)menu, Menu_DriveType_Disk);
                  }
                }
              }
            }
          }
          else if (b[0] == Menu_DriveType_FS)
          {
            if (b[1] == -1)
            {
              if (*(DriveTypes[number]) != DRIVE_TYPE_FS)
              {
                char *fn;

                if ((fn = wimp_window_read_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number])) != NULL)
                {
                  /* Only allow FS mode in directories or image files */
                  if (ui_set_drive_dir(number, wimp_strterm(fn)) == 0)
                  {
                    wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveDisk, 1);
                    wimp_menu_set_grey_all((RO_MenuHead*)&MenuDriveFS, 0);
                    wimp_menu_tick_exclusive((RO_MenuHead*)menu, Menu_DriveType_FS);
                  }
                }
              }
              /* Change in submenus? */
            }
            else
            {
              RO_MenuHead *submenu;

              submenu = (RO_MenuHead*)&MenuDriveFS;
              switch (b[1])
              {
                case Menu_DriveFS_ConvP00:
                  ui_toggle_resource_menu(Rsrc_ConvP00[number], submenu, Menu_DriveFS_ConvP00);
                  break;
                case Menu_DriveFS_SaveP00:
                  ui_toggle_resource_menu(Rsrc_SaveP00[number], submenu, Menu_DriveFS_SaveP00);
                  break;
                case Menu_DriveFS_HideCBM:
                  ui_toggle_resource_menu(Rsrc_HideCBM[number], submenu, Menu_DriveFS_HideCBM);
                  break;
              }
            }
          }
        }
      case CONF_MENU_CARTTYPE:
        if (b[0] == 0)
          cartridge_detach_image();
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayCartridgeType, b[0]);
        break;
      case CONF_MENU_RSUSRDEV:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayRsUserDevice, b[0]);
        break;
      case CONF_MENU_ACIADEV:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayAciaDevice, b[0]);
        break;
      case CONF_MENU_SERIAL:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySerialBaud, b[0]);
        break;
      case CONF_MENU_SIDMODEL:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySidModel, b[0]);
        break;
      case CONF_MENU_SPEED:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySpeedLimit, b[0]);
        break;
      case CONF_MENU_REFRESH:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayRefresh, b[0]);
        break;
      case CONF_MENU_PETMEM:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayPetMemory, b[0]);
        break;
      case CONF_MENU_PETIO:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayPetIO, b[0]);
        break;
      case CONF_MENU_PETVIDEO:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayPetVideo, b[0]);
        break;
      case CONF_MENU_PETMODEL:
        {
          int i;

          ui_set_menu_display_core((disp_desc_t*)&MenuDisplayPetModel, set_pet_model_by_name, b[0]);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetMemory);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetIO);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayPetVideo);
          wimp_window_write_icon_text(ConfWindows[CONF_WIN_PET], Icon_Conf_PetKbd, pet_get_keyboard_name());
          ui_update_rom_names();
          for (i=0; PETdependconf[i].resource != NULL; i++)
          {
            ui_setup_config_item(PETdependconf + i);
          }
        }
        break;
      case CONF_MENU_VICRAM:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayVicRam, b[0]);
        break;
      case CONF_MENU_VICCART:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayVicCartridge, b[0]);
        break;
      case CONF_MENU_DOSNAME:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayDosName, b[0]);
        break;
      case CONF_MENU_C2LINE:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayCBM2Line, b[0]);
        break;
      case CONF_MENU_C2MEM:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayCBM2Memory, b[0]);
        break;
      case CONF_MENU_C2MODEL:
        {
          ui_set_menu_display_core((disp_desc_t*)&MenuDisplayCBM2Model, set_cbm2_model_by_name, b[0]);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2Memory);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2RAM);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayCBM2Line);
          wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_Conf_CBM2Kbd, cbm2_get_keyboard_name());
          ui_update_rom_names();
        }
        break;
      case CONF_MENU_C2RAM:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayCBM2RAM, b[0]);
        break;
      case CONF_MENU_C2CART:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayCBM2Cartridge, b[0]);
        break;
      case CONF_MENU_SNDBUFF:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplaySoundBuffer, b[0]);
        break;
      case CONF_MENU_JOYDEV1:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayJoyDevice1, b[0]);
        break;
      case CONF_MENU_JOYDEV2:
        ui_set_menu_display_value((disp_desc_t*)&MenuDisplayJoyDevice2, b[0]);
        break;
      case CONF_MENU_ROMSET:
        if (MenuDisplayROMSet != NULL)
        {
          ui_set_menu_display_core((disp_desc_t*)MenuDisplayROMSet, set_romset_by_name, b[0]);
          ui_setup_menu_display((disp_desc_t*)&MenuDisplayDosName);
          ui_update_rom_names();
          /*maincpu_trigger_reset();*/
        }
        break;
      case CONF_MENU_ROMACT:
        switch (b[0])
        {
          case Menu_RomAct_Create:
            if (b[1] != -1)
            {
              if (strlen(NewRomSetName) > 0)
              {
                romset_create_item(NewRomSetName, mem_romset_resources_list);
                ui_build_romset_menu();
                ui_setup_menu_display((disp_desc_t*)&MenuDisplayROMSet);
              }
            }
            break;
          case Menu_RomAct_Delete:
            romset_delete_item(ROMSetName);
            ui_build_romset_menu();
            ui_setup_menu_display((disp_desc_t*)&MenuDisplayROMSet);
            break;
          case Menu_RomAct_Dump:
            if (ROMSetArchiveFile != NULL)
            {
              romset_dump_archive(ROMSetArchiveFile);
            }
            break;
          case Menu_RomAct_Clear:
            romset_clear_archive();
            ui_build_romset_menu();
            ui_setup_menu_display((disp_desc_t*)&MenuDisplayROMSet);
            break;
          case Menu_RomAct_Restore:
            romset_clear_archive();
            if (ROMSetArchiveFile != NULL)
            {
              romset_load_archive(ROMSetArchiveFile, 0);
            }
            ui_build_romset_menu();
            ui_setup_menu_display((disp_desc_t*)&MenuDisplayROMSet);
            break;
          default: break;
        }
        break;
      default:
        break;
    }
  }

  /* Selection with adjust ==> re-open menu */
  if ((block[MouseB_Buttons] == 1) && (menu != NULL))
  {
    Wimp_CreateMenu(menu, block[MouseB_PosX], block[MouseB_PosY]);
  }
}


static void ui_new_drive_image(int number, int *b, int activate)
{
  int type;
  int aux[4];

  type = ReadCatalogueInfo(((char*)b)+44, aux);
  if (type == 2)
  {
    ui_set_drive_dir(number, ((char*)b)+44);
  }
  else
  {
    if (ScanKeys(IntKey_Shift) == 0xff)
    {
      if (ui_set_drive_image(number, ((char*)b)+44) != 0) return;
    }
    else
    {
      ui_image_contents_disk(((char*)b)+44); return;
    }
  }
  wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number], ((char*)b)+44);
}

static void ui_load_snapshot_trap(ADDRESS unused_address, void *unused_data)
{
  int status;

  /* See true drive emulation */
  ui_temp_suspend_sound();

  status = machine_read_snapshot(((char*)SnapshotMessage)+44);

  /* In this case the scrap removal had to be delayed */
  if (WimpScrapUsed != 0)
  {
    remove(WimpScrapFile);
    WimpScrapUsed = 0;
  }

  if (status == 0)
  {
    resource_value_t val;
    if (resources_get_value(Rsrc_True, &val) == 0)
    {
      ui_display_truedrv_emulation((int)val);
    }
    if (resources_get_value(Rsrc_Sound, &val) == 0)
    {
      ui_display_sound_enable((int)val);
    }
  }
  ui_temp_resume_sound();
}


static void ui_user_message(int *b)
{
  int i;
  int action=0;
  char *name = ((char*)b)+44;

  switch (b[MsgB_Action])
  {
    case Message_Quit: ui_exit(); break;
    case Message_ModeChange:
      {
        int block[WindowB_WFlags+1];

        wimp_read_screen_mode(&ScreenMode);
        /* Extremely annoying mode change code */
        ui_set_emu_window_size();	/* Change in eigen factors might make this necessary */
        block[WindowB_Handle] = EmuWindow->Handle;
        Wimp_GetWindowState(block);
        if ((block[WindowB_WFlags] & (1<<16)) != 0)	/* window open? */
        {
          int d;

          d = block[WindowB_VMaxY] - block[WindowB_VMinY];
          if (block[WindowB_VMaxY] > ScreenMode.resy - TitleBarHeight)
          {
            block[WindowB_VMaxY] = ScreenMode.resy - TitleBarHeight;
            if ((block[WindowB_VMinY] = block[WindowB_VMaxY] - d) < TitleBarHeight)
            {
              block[WindowB_VMinY] = TitleBarHeight;
            }
          }
          d = block[WindowB_VMaxX] - block[WindowB_VMinX];
          if (block[WindowB_VMaxX] > ScreenMode.resx - TitleBarHeight)
          {
            block[WindowB_VMaxX] = ScreenMode.resx - TitleBarHeight;
            if ((block[WindowB_VMinX] = block[WindowB_VMaxX] - d) < 0)
            {
              block[WindowB_VMinX] = 0;
            }
          }
          /* Send myself a message where to open the window */
          Wimp_SendMessage(2, block, TaskHandle, 0);
        }
      }
      break;
    case Message_PaletteChange:
      wimp_read_screen_mode(&ScreenMode);
      memcpy(oldColours, EmuCanvas->pixel_translation, 16*sizeof(PIXEL));
      FrameBufferUpdate = 1;
      ModeChanging = 1;
      handle_mode_change();
      ModeChanging = 0;
      break;
    case Message_DataLoad:
      if (b[5] == EmuWindow->Handle)
      {
        if ((b[10] == FileType_C64File) && (machine_class == VICE_MACHINE_C64))
        {
          FILE *fp;

          if ((fp = fopen(name, "rb")) != NULL)
          {
            BYTE lo, hi;
            int length;

            lo = fgetc(fp); hi = fgetc(fp); length = lo + (hi << 8);
            length += fread(ram + length, 1, ram_size - length, fp);
            fclose(fp);
            ram[0xc3] = lo; ram[0xc4] = hi;
            lo = length & 0xff; hi = (length >> 8) & 0xff;
            ram[0xae] = lo; ram[0x2d] = lo; ram[0x2f] = lo; ram[0x31] = lo; ram[0x33] = lo;
            ram[0xaf] = hi; ram[0x2e] = hi; ram[0x30] = hi; ram[0x32] = hi; ram[0x34] = hi;
            action = 1;
          }
        }
        else if (b[10] == FileType_Data)	/* Snapshot? */
        {
          wimp_strcpy(((char*)SnapshotMessage)+44, name);
          maincpu_trigger_trap(ui_load_snapshot_trap, NULL);
          action = 1; SnapshotPending = 1;
        }
      }
      else if (b[5] == EmuPane->Handle)
      {
        switch (b[6])
        {
          case Icon_Pane_Drive0:
          case Icon_Pane_LED0: i = 0; break;
          case Icon_Pane_Drive1:
          case Icon_Pane_LED1: i = 1; break;
          case Icon_Pane_Drive2:
          case Icon_Pane_LED2: i = 2; break;
          case Icon_Pane_Drive3:
          case Icon_Pane_LED3: i = 3; break;
          default: i = -1; break;
        }
        if (i >= 0)
        {
          ui_new_drive_image(i, b, 1); action = 1;
        }
      }
      else if (b[5] == ImgContWindow->Handle)
      {
        ui_image_contents_generic(name, b[10]); action = 1;
      }
      else if (b[5] == ConfWindows[CONF_WIN_DRIVES]->Handle)
      {
        for (i=0; i<4; i++)
        {
          if (b[6] == DriveToFile[i]) break;
        }
        if (i < 4)
        {
          ui_new_drive_image(i, b, 1); action = 1;
        }

        if (b[6] == Icon_Conf_TapeFile)
        {
          if (ScanKeys(IntKey_Shift) == 0xff)
          {
            if (tape_attach_image(name) == 0)
            {
              wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TapeFile, name);
              wimp_window_set_icon_state(ConfWindows[CONF_WIN_DRIVES], Icon_Conf_TapeDetach, 0, IFlg_Grey);
              action = 1;
            }
          }
          else
          {
            ui_image_contents_tape(name);
          }
        }
      }
      else if (b[5] == ConfWindows[CONF_WIN_SYSTEM]->Handle)
      {
        if (b[6] == Icon_Conf_CartFile)
        {
          ui_set_cartridge_file(name);
          action = 1;
        }
        else if (b[6] == Icon_Conf_DosNameF)
        {
          ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayDosName, (resource_value_t)ui_check_for_syspath(name));
          action = 1;
        }
        if ((b[10] == FileType_Data) || (b[10] == FileType_Text))
        {
          char *res = NULL;
          int rom_changed = 0;
          if (b[10] == FileType_Data)
          {
            if (b[6] == Icon_Conf_CharGen) res = Rsrc_CharGen;
            else if (b[6] == Icon_Conf_Kernal) res = Rsrc_Kernal;
            else if (b[6] == Icon_Conf_Basic) res = Rsrc_Basic;
            if (res != NULL) rom_changed = 1;
          }
          else if (b[10] == FileType_Text)
          {
            if (b[6] == Icon_Conf_Palette) res = Rsrc_Palette;
            /* Check extension */
            else
            {
              char *d, *ext;

              d = name; ext = d;
              while (*d > ' ') {if (*d =='/') ext = d+1; d++;}
              if (ext != name)
              {
                if (wimp_strcmp(ext, RSETARCH_EXT) == 0)
                {
                  romset_load_archive(name, 0);
                  ui_build_romset_menu();
                }
              }
            }
          }
          if (res != NULL)
          {
            char *filename;

            filename = ui_check_for_syspath(name);
            if (resources_set_value(res, (resource_value_t)filename) == 0)
            {
              wimp_window_write_icon_text(ConfWindows[CONF_WIN_SYSTEM], b[6], filename);
              if (rom_changed != 0)
              {
                mem_load(); maincpu_trigger_reset();
              }
              action = 1;
            }
          }
        }
      }
      else if (b[5] == ConfWindows[CONF_WIN_VIC]->Handle)
      {
        if (b[6] == Icon_Conf_VICCartF)
        {
          ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayVicCartridge, (resource_value_t)(b + 11));
          action = 1;
        }
      }
      else if (b[5] == ConfWindows[CONF_WIN_CBM2]->Handle)
      {
        if (b[6] == Icon_Conf_CBM2CartF)
        {
          ui_update_menu_disp_strshow((disp_desc_t*)&MenuDisplayCBM2Cartridge, (resource_value_t)(b + 11));
          action = 1;
        }
      }
      if (action != 0)
      {
        b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoadAck;
        Wimp_SendMessage(17, b, b[MsgB_Sender], b[6]);
      }
      if ((WimpScrapUsed != 0) && (SnapshotPending == 0))
      {
        remove(WimpScrapFile);
        WimpScrapUsed = 0;
      }
      break;
    case Message_DataSave:
      {
        if (b[5] == EmuWindow->Handle)
        {
          if (((b[10] == FileType_C64File) && (machine_class == VICE_MACHINE_C64)) || (b[10] == FileType_Data)) action = 1;
        }
        else if (b[5] == ConfWindows[CONF_WIN_SYSTEM]->Handle)
        {
          if ((b[10] == FileType_Data) || (b[10] == FileType_Text)) action = 1;
        }
        if (action != 0)
        {
          wimp_strcpy(name, WimpScrapFile);
          b[MsgB_Size] = 44 + ((wimp_strlen(name) + 4) & ~3);
          WimpScrapUsed = 1;
          b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataSaveAck;
          Wimp_SendMessage(17, b, b[MsgB_Sender], b[6]);
        }
      }
      break;
    case Message_DataSaveAck:
      {
        switch (LastDrag)
        {
          case DRAG_TYPE_SOUND:
            ui_set_sound_file(name); break;
          case DRAG_TYPE_SERIAL:
            ui_set_serial_file(name); break;
          case DRAG_TYPE_PRINTER:
            ui_set_printer_file(name); break;
          case DRAG_TYPE_SNAPSHOT:
            if (ui_make_snapshot(name) == 0)
            {
              wimp_window_write_icon_text(SnapshotWindow, Icon_Snap_Path, name);
              b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoad;
              Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
            }
            break;
          case DRAG_TYPE_SAVEBOX:
            if ((ROMSetName != NULL) && (romset_save_item(name, ROMSetName) == 0))
            {
              wimp_strcpy(ROMSetItemFile, name);
              b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoad;
              Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
              Wimp_CreateMenu((int*)-1, 0, 0);
            }
            break;
          default: break;
        }
        LastDrag = DRAG_TYPE_NONE;
      }
      break;
    case Message_MenuWarning:
      if (LastMenu == CONF_MENU_ROMACT + 0x100)
      {
        wimp_window_write_icon_text(SaveBox, Icon_Save_Path, ROMSetItemFile);
      }
      Wimp_CreateSubMenu((int*)(b[5]), b[6], b[7]);
      break;
    default: break;
  }
}


static void ui_user_message_ack(int *b)
{
  _kernel_oserror err;

  err.errnum = 0; err.errmess[0] = 0;

  /* Data save bounced? */
  if (b[MsgB_Action] == Message_DataSave)
  {
    sprintf(err.errmess, SymbolStrings[Symbol_ErrSave], ((char*)b)+44);
  }
  if (b[MsgB_Action] == Message_DataLoad)
  {
    sprintf(err.errmess, SymbolStrings[Symbol_ErrLoad], ((char*)b)+44);
  }

  if (err.errmess[0] != 0)
  {
    Wimp_ReportError(&err, 1, WimpTaskName);
  }
}


static void ui_null_event(int *b)
{
  int mpos[MouseB_Icon+1];

  Wimp_GetPointerInfo(mpos);

  if (mpos[MouseB_Window] == ConfWindows[CONF_WIN_SOUND]->Handle)
  {
    if ((LastDrag == DRAG_TYPE_VOLUME) && (mpos[MouseB_Icon] == Icon_Conf_Volume))
    {
      ui_drag_sound_volume(mpos);
    }
  }
  else if (mpos[MouseB_Window] == ConfWindows[CONF_WIN_JOY]->Handle)
  {
    ui_poll_joystick_window(mpos[MouseB_Icon]);
  }
}


/* Things to do before / after calling Wimp_Poll */
static void ui_poll_prologue(void)
{
  sound_wimp_poll_prologue();
}

static void ui_poll_epilogue(void)
{
  sound_wimp_poll_epilogue((WithinUiPoll == 0));
}

/* Core polling function */
static int ui_poll_core(int *block)
{
  int event;

  if ((EmuPaused == 0) || (LastDrag == DRAG_TYPE_VOLUME) || (JoystickWindowOpen != 0))
    PollMask &= ~1;
  else
    PollMask |= 1;

  ui_poll_prologue();
  event = Wimp_Poll(PollMask, block, NULL);

  switch (event)
  {
    case WimpEvt_Null: ui_null_event(block); break;
    case WimpEvt_RedrawWin: ui_redraw_window(block); break;
    case WimpEvt_OpenWin: ui_open_window(block); break;
    case WimpEvt_CloseWin: ui_close_window(block); break;
    case WimpEvt_MouseClick: ui_mouse_click(block); break;
    case WimpEvt_UserDragBox: ui_user_drag_box(block); break;
    case WimpEvt_KeyPress: ui_key_press(block); break;
    case WimpEvt_MenuSlct: ui_menu_selection(block); break;
    case WimpEvt_UserMsg:
    case WimpEvt_UserMsgRec: ui_user_message(block); break;
    case WimpEvt_UserMsgAck: ui_user_message_ack(block); break;
    default: break;
  }
  return event;
}

void ui_poll(void)
{
  int now;

  /* Just to be on the save side: snapshot unpauses for at most 1 frame! */
  SnapshotPending = 0;
  /* Was sound suspended while in this function? */
  SoundSuspended = 0;

  /* Must poll in every vblank! */
  kbd_poll();

  now = OS_ReadMonotonicTime();

  /* Speed limiter? Busy wait */
  if (FrameCS != 0)
  {
    while ((now - LastFrame) < FrameCS) now = OS_ReadMonotonicTime();
  }
  LastFrame = now;

  NumberOfFrames++;

  if ((now - LastSpeed) >= SpeedEvery)
  {
    RelativeSpeed = (200 * NumberOfFrames) / (now - LastSpeed);
    ui_display_speed(RelativeSpeed, (100 * NumberOfRefreshes) / (now - LastSpeed), 0);
    LastSpeed = now; NumberOfFrames = 0; NumberOfRefreshes = 0;
  }

  if (SingleTasking != 0) return;

  if ((now - LastPoll) >= PollEvery)
  {
    int event;

    WithinUiPoll++;	/* Allow for nested calls */

    LastPoll = now;

    do
    {
      event = ui_poll_core(WimpBlock);
    }
    /* A pending snapshot must unpause the emulator for a little */
    while ((EmuPaused != 0) && (SnapshotPending == 0) && (event != WimpEvt_Null));

    if (--WithinUiPoll == 0)
    {
      if (SoundSuspended != 0) sound_resume();
    }
    ui_poll_epilogue();
  }
}


ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
  char str[1024];
  va_list ap;
  int button, event;
  int dx, dy;
  int *block = (int*)str;
  RO_Window *w = CpuJamWindow;
  RO_Caret activeCaret;
  short mbBox[5];

  va_start(ap, format);

  vsprintf(str, format, ap);

  wimp_window_write_icon_text(CpuJamWindow, Icon_Jam_Message, str);
  dx = w->vmaxx - w->vminx; dy = w->vmaxy - w->vminy;
  w->vminx = (ScreenMode.resx - dx) / 2; w->vmaxx = w->vminx + dx;
  w->vminy = (ScreenMode.resy - dy) / 2; w->vmaxy = w->vminy + dy;
  w->stackpos = -1;
  Wimp_OpenWindow((int*)w);
  Wimp_GetCaretPosition(&activeCaret);
  Wimp_SetCaretPosition(CpuJamWindow->Handle, -1, -100, 100, -1, -1);
  mbBox[0] = 0x100;
  mbBox[1] = (short)(w->vminx); mbBox[2] = (short)(w->vminy);
  mbBox[3] = (short)(w->vmaxx); mbBox[4] = (short)(w->vmaxy);
  MouseBoundingBox(((unsigned char*)mbBox)+1);

  /* This shouldn't be necessary, but just to be on the safe side */
  WithinUiPoll++;

  button = -1;

  while (button < 0)
  {
    ui_poll_prologue();
    event = Wimp_Poll(PollMask & ~1, block, NULL);

    switch (event)
    {
      case WimpEvt_RedrawWin:
        ui_redraw_window(block);
        break;
      case WimpEvt_MouseClick:
        if (block[MouseB_Window] == CpuJamWindow->Handle)
        {
          switch (block[MouseB_Icon])
          {
            case Icon_Jam_Reset:
            case Icon_Jam_ResetH:
            case Icon_Jam_Monitor:
            case Icon_Jam_Debug: button = block[MouseB_Icon]; break;
            default: break;
          }
        }
        break;
      case WimpEvt_KeyPress:
        if ((block[KeyPB_Window] == CpuJamWindow->Handle) && (block[KeyPB_Key] == Key_Return))
        {
          button = Icon_Jam_Reset;
        }
        break;
      default:
        break;
    }
  }

  mbBox[1] = 0; mbBox[2] = 0; mbBox[3] = ScreenMode.resx; mbBox[4] = ScreenMode.resy;
  MouseBoundingBox(((unsigned char*)mbBox)+1);
  Wimp_SetCaretPosition(activeCaret.WHandle, activeCaret.IHandle, activeCaret.offx, activeCaret.offy, activeCaret.height, activeCaret.index);
  Wimp_CloseWindow((int*)CpuJamWindow);

  WithinUiPoll--;

  ui_poll_epilogue();

  switch (button)
  {
    case Icon_Jam_ResetH: return UI_JAM_HARD_RESET;
    case Icon_Jam_Monitor: return UI_JAM_MONITOR;
    case Icon_Jam_Debug: return UI_JAM_DEBUG;
    default: break;
  }
  return UI_JAM_RESET;
}


void ui_error(const char *format, ...)
{
  char str[1024];
  va_list ap;
  _kernel_oserror err;

  va_start(ap, format);

  vsprintf(str, format, ap);

  err.errnum = 0; strncpy(err.errmess, str, 256);
  log_error(roui_log, "%s\n", str);

  Wimp_ReportError(&err, 1, WimpTaskName);
}


void ui_message(const char *format, ...)
{
  char str[1024];
  va_list ap;
  _kernel_oserror err;

  va_start(ap, format);

  vsprintf(str, format, ap);

  err.errnum = 0; strncpy(err.errmess, str, 256);
  log_message(roui_log, "%s\n", str);

  Wimp_ReportError(&err, 17, WimpTaskName);
}


void ui_show_text(const char *title, const char *text, int width, int height)
{
  Wimp_CommandWindow((int)title);
  printf("%s", text);
  Wimp_CommandWindow(0);	/* Wait for space or click */
}


void ui_exit(void)
{
  machine_shutdown();
  video_free();
  sound_close();
  ui_image_contents_exit();
  log_message(roui_log, SymbolStrings[Symbol_MachDown]); log_message(roui_log, "\n");
  wimp_icon_delete(&IBarIcon);
  Wimp_CloseDown(TaskHandle, TASK_WORD);
  exit(0);
}


void ui_display_speed(int percent, int framerate, int warp_flag)
{
  char buffer[32];

  if (DisplayFPS == 0)
    sprintf(buffer, SymbolStrings[Symbol_PaneSpd], percent);
  else
    sprintf(buffer, SymbolStrings[Symbol_PaneFPS], framerate);

  wimp_window_write_icon_text_u(EmuPane, Icon_Pane_Speed, buffer);
}


void ui_toggle_drive_status(int state)
{
}


void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
  /*int i, EnabledDrives;

  EnabledDrives = (int)enable;
  for (i=0; i<4; i++)
  {
    wimp_window_set_icon_state(EmuPane, LEDtoIcon[i], ((EnabledDrives & (1<<i)) == 0) ? IFlg_Grey : 0, IFlg_Grey);
  }*/
}


void ui_display_drive_track(int drive_number, int track_number)
{
  RO_Icon *icon;
  int b[11];

  if (drive_number >= 2) return;

  tracknums[drive_number] = track_number;

  if (drive_number != DisplayDriveTrack) return;

  if ((icon = wimp_window_get_icon(EmuPane, Icon_Pane_TrkSec)) == NULL) return;
  sprintf((char*)b, "%d:%d.%d", DisplayDriveTrack+8, tracknums[DisplayDriveTrack] >> 1, 5 * (tracknums[DisplayDriveTrack] & 1));
  strncpy((char*)(icon->dat.ind.tit), (char*)b, icon->dat.ind.len-1);
  wimp_window_update_icon(EmuPane, Icon_Pane_TrkSec);
}


void ui_display_drive_led(int drive_number, int status)
{
  ui_set_drive_leds((unsigned int)drive_number, status);
}


void ui_set_drive_leds(unsigned int led, int status)
{
  RO_Icon *icon;
  char *name;

  if (led >= 4) return;

  if (ledstates[led] == status) return;

  ledstates[led] = status;

  icon = wimp_window_get_icon(EmuPane, LEDtoIcon[led]);
  name = (char*)(icon->dat.ind.tit);

  if (status == 0)
    sprintf(name, "led_off");
  else
    sprintf(name, "led_on");

  wimp_window_update_icon(EmuPane, LEDtoIcon[led]);
}


void ui_display_paused(int flag)
{
  char *t;

  if (flag == 0)
  {
    ui_temp_resume_sound(); t = SymbolStrings[Symbol_Pause];
  }
  else
  {
    ui_temp_suspend_sound(); t = SymbolStrings[Symbol_Resume];
  }
  wimp_window_write_icon_text_u(EmuPane, Icon_Pane_Pause, t);
}


int ui_extend_image_dialog(void)
{
  int button;
  _kernel_oserror err;

  err.errnum = 0; strcpy(err.errmess, SymbolStrings[Symbol_DlgExtend]);
  button = Wimp_ReportError(&err, 3, WimpTaskName);

  return (button == 1);
}


void ui_update_menus(void)
{
  resource_value_t val;

  if (resources_get_value(Rsrc_True, &val) == 0)
  {
    ui_display_truedrv_emulation((int)val);
  }
  if (resources_get_value(Rsrc_Sound, &val) == 0)
  {
    ui_display_sound_enable((int)val);
  }

  /* Update all open (!) config windows */
  ui_update_config_windows();
}


static void mon_trap(ADDRESS addr, void *unused_data)
{
  ui_temp_suspend_sound();
  mon(addr);
  ui_temp_resume_sound();
}


void ui_activate_monitor(void)
{
  maincpu_trigger_trap(mon_trap, (void*)0);
}
