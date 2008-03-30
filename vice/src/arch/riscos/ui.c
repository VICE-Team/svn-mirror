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

#include "vice.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "wimp.h"

#include "archdep.h"
#include "attach.h"
#include "autostart.h"
#include "cbm2ui.h"
#include "cartridge.h"
#include "console.h"
#include "datasette.h"
#include "diskimage.h"
#include "fliplist.h"
#include "fsdevice.h"
#include "info.h"
#include "interrupt.h"
#include "joy.h"
#include "kbd.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "petui.h"
#include "resources.h"
#include "romset.h"
#include "serial.h"
#include "screenshot.h"
#include "sound.h"
#include "sysfile.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "uiconfig.h"
#include "uihelp.h"
#include "uiimage.h"
#include "uimsgwin.h"
#include "uisharedef.h"
#include "utils.h"
#include "version.h"
#include "videoarch.h"
#include "vsidarch.h"
#include "vsync.h"
#include "vsyncarch.h"

/* module includes */
#include "c64/c64mem.h"
#include "drive/drive.h"
#include "monitor/mon.h"
#include "raster/raster.h"
#include "vdrive/vdrive.h"
#include "vicii/vicii.h"





extern int  pet_set_model(const char *name, void *extra);
extern int  cbm2_set_model(const char *name, void *extra);
extern void screenshot_init_sprite(void);

/* Defined in soundacorn.c. Important for timer handling! */
extern int  sound_wimp_poll_prologue(void);
extern int  sound_wimp_poll_epilogue(int install);
extern void sound_wimp_safe_exit(void);
extern void sound_get_vidc_frequency(int *speed, int *period);



/* Declare some static functions */
static void ui_poll_prologue(void);
static void ui_poll_epilogue(void);
static void ui_temp_suspend_sound(void);
static void ui_temp_resume_sound(void);
static void ui_issue_reset(int doreset);
static int  ui_open_centered_or_raise_block(RO_Window *win, int *block);





/* Misc text */
static const char CustomSprites[] = "Vice:Sprites";
static const char TemplatesFile[] = "Vice:Templates";
static const char MessagesFile[] = "<Vice$Messages>";
static const char WimpScrapFile[] = "<Wimp$ScrapDir>.ViceScrap";
static const char VicePathVariable[] = "Vice$Path";
static const char ResourceDriveDir[] = "DRIVES";
static const char ViceSnapshotFile[] = "ViceSnap";
static const char ViceDfltFlipFile[] = "Fliplist";
static const char ViceNewDiscName[] = "NEWDISC,01";
static const char ViceNewDiscFile[] = "imagefile";
static const char PRGFileExtension[] = "prg";

#define RSETARCH_EXT	"vra"
#define KEYMAP_EXT	"vkm"



/* Error messages */
#define Error_IconSprite	0x100
#define Error_TemplatesFile	0x101
#define Error_Template		0x102


#define FileType_Text		0xfff
#define FileType_Data		0xffd
#define FileType_Sprite		0xff9
#define FileType_C64File	0x064
#define FileType_D64Image	0x164
#define FileType_SIDMusic	0x063


/* Start scanning for internal keynumbers from here */
#define IntKey_MinCode		3
#define IntKey_Shift		0

/* Return key in icons */
#define Key_Return		13


/* For sound volume icons */
#define Maximum_Volume		127
#define Well_Border		12

/* Maximum sound latency (ms) */
#define Maximum_Latency		1000


/* Menu definitions */
#define Menu_IBar		1
#define Menu_Emulator		2
#define Menu_Images		3
#define Menu_CreateDisc		4

#define Menu_Height		44
#define Menu_Flags		0x07003011







/* Drag types */
#define DRAG_TYPE_NONE		0
#define DRAG_TYPE_SOUND		1
#define DRAG_TYPE_SERIAL	2
#define DRAG_TYPE_PRINTER	3
#define DRAG_TYPE_SNAPSHOT	4
#define DRAG_TYPE_VOLUME	5
#define DRAG_TYPE_SAVEBOX	6
#define DRAG_TYPE_CREATEDISC	7

/* Savebox types */
#define SBOX_TYPE_NONE		0
#define SBOX_TYPE_ROMSET	1
#define SBOX_TYPE_KEYBOARD	2
#define SBOX_TYPE_SCRSHOT	3
#define SBOX_TYPE_FLIPLIST	4






/* Function type for setting a value */
typedef int (*set_var_function)(const char *name, resource_value_t val);




/* Variables */

unsigned int TaskHandle;

static int WimpMessages[] = {
  Message_DataSave, Message_DataSaveAck, Message_DataLoad, Message_DataLoadAck,
  Message_DataOpen, Message_PaletteChange, Message_ModeChange, Message_MenuWarning,
  Message_HelpRequest, Message_MenusDeleted, 0
};

/* General wimp variable */
static int PollMask;
static int LastMenu;
static int LastHandle;
static int LastClick;
static int LastDrag;
static int LastSubDrag;
static int MenuType;
static int DragType;
static int CMOS_DragType;
static int TrueDriveEmulation = 0;
static int SoundEnabled = 0;
static int SoundSuspended = 0;
static int WasAutoPaused = 0;
static int SoundVolume;
static int DisplayFPS = 0;
static int DisplayDriveTrack = 0;
static int WimpScrapUsed = 0;
static int JoystickWindowOpen = 0;
static int WithinUiPoll = 0;
static int DatasetteCounter = -1;
static int RegularProgramExit = 0;
static int WimpBlock[64];

static int SnapshotPending = 0;
static int SnapshotMessage[64];

static const int default_screen_width = 384;
static const int default_screen_height = 312;

/* length of indirected menu entries */
static const int MenuIndirectSize = 32;

/* flip lists */
static int FlipListIter = 0;
static int FlipListNumber = 0;
static const int FlipListDrive = 0;


/* Window title */
static char EmuTitle[256];

/* Custom sprites area */
static int *SpriteArea;

/* Icon translation tables */
static const char LEDtoIcon[4] = {Icon_Pane_LED0, Icon_Pane_LED1, Icon_Pane_LED2, Icon_Pane_LED3};
static const char DriveToFile[4] = {Icon_ConfDrv_DriveFile8, Icon_ConfDrv_DriveFile9, Icon_ConfDrv_DriveFile10, Icon_ConfDrv_DriveFile11};

/* Config icons affected by True Drive Emulation state */
static const conf_iconid_t TrueDependentIcons[] = {
  /*{CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvSync},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvSyncT},*/
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar8},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt8},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt8T},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle8},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle8T},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar9},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt9},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt9T},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle9},
  {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle9T},
  {0xff, 0xff}
};

/* Config icons affected by Tape file state */
static const conf_iconid_t TapeFileDependentIcons[] = {
  {CONF_WIN_TAPE, Icon_ConfTap_TapeDetach},
  {CONF_WIN_TAPE, Icon_ConfTap_DataCounter},
  {CONF_WIN_TAPE, Icon_ConfTap_DataStop},
  {CONF_WIN_TAPE, Icon_ConfTap_DataRewind},
  {CONF_WIN_TAPE, Icon_ConfTap_DataPlay},
  {CONF_WIN_TAPE, Icon_ConfTap_DataForward},
  {CONF_WIN_TAPE, Icon_ConfTap_DataRecord},
  {CONF_WIN_TAPE, Icon_ConfTap_DataDoReset},
  {0xff, 0xff}
};

/* Config icons affected by Sound enable state */
static const conf_iconid_t SoundDependentIcons[] = {
  {CONF_WIN_SOUND, Icon_ConfSnd_SampleRate},
  {CONF_WIN_SOUND, Icon_ConfSnd_SampleRateT},
  {CONF_WIN_SOUND, Icon_ConfSnd_SoundDev},
  {CONF_WIN_SOUND, Icon_ConfSnd_SoundDevT},
  {CONF_WIN_SOUND, Icon_ConfSnd_Oversample},
  {CONF_WIN_SOUND, Icon_ConfSnd_OversampleT},
  {CONF_WIN_SOUND, Icon_ConfSnd_SoundBuff},
  {CONF_WIN_SOUND, Icon_ConfSnd_SoundBuffT},
  {CONF_WIN_SOUND, Icon_ConfSnd_SpeedAdjust},
  {CONF_WIN_SOUND, Icon_ConfSnd_SpeedAdjustT},
  {CONF_WIN_SOUND, Icon_ConfSnd_Volume},
  {0xff, 0xff}
};

static const conf_iconid_t SidDependentIcons[] = {
  {CONF_WIN_SOUND, Icon_ConfSnd_SidFilter},
  {CONF_WIN_SOUND, Icon_ConfSnd_UseResid},
  {CONF_WIN_SOUND, Icon_ConfSnd_SidModel},
  {CONF_WIN_SOUND, Icon_ConfSnd_SidModelT},
  {CONF_WIN_SOUND, Icon_ConfSnd_ResidSamp},
  {CONF_WIN_SOUND, Icon_ConfSnd_ResidSampT},
  {CONF_WIN_SOUND, Icon_ConfSnd_ResidPass},
  {CONF_WIN_SOUND, Icon_ConfSnd_SidStereo},
  {CONF_WIN_SOUND, Icon_ConfSnd_Sid2Addr},
  {0xff, 0xff}
};





/* Configuration options */
static int AutoPauseEmu;
static int Use16BitSound;
static int DriveType8;
static int DriveType9;
static int DriveType10;
static int DriveType11;
static char *DriveFile8 = NULL;
static char *DriveFile9 = NULL;
static char *DriveFile10 = NULL;
static char *DriveFile11 = NULL;
static char *TapeFile = NULL;

static int *DriveTypes[] = {
  &DriveType8, &DriveType9, &DriveType10, &DriveType11
};

static char **DriveFiles[] = {
  &DriveFile8, &DriveFile9, &DriveFile10, &DriveFile11
};




/* Logging */
static log_t roui_log = LOG_ERR;

static int ShowEmuPane = 1;
static char *ROMSetName = NULL;
static char *ROMSetArchiveFile = NULL;

int EmuZoom;
int EmuPaused;
int SingleTasking = 0;
int CycleBasedSound;
char *PetModelName = NULL;
char *CBM2ModelName = NULL;

static char ROMSetItemFile[256];
static char SystemKeymapFile[256];
static char ViceScreenshotFile[256];
static char ViceFliplistFile[256];

/* Mode changes */
int FrameBufferUpdate = 0;
int ModeChanging = 0;
/*static PIXEL oldColours[16];*/

/* LED states */
int DriveLEDStates[4] = {0, 0, 0, 0};
int DriveTrackNumbers[2] = {36, 36};




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
RO_Window *MessageWindow;
RO_Window *CreateDiscWindow;
RO_Window *ConfWindows[CONF_WIN_NUMBER];
RO_Window *VSidWindow = NULL;

#define TitleBarOffset	40
RO_Window *ConfWinPositions[CONF_WIN_NUMBER];





/* Symbols and error messages for translation */
enum SymbolInstances {
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
  Symbol_TitLicense,
  Symbol_TitWarranty,
  Symbol_TitContrib,
  Symbol_DlgExtend,
  Symbol_ErrTemp,
  Symbol_ErrSMem,
  Symbol_ErrSpr,
  Symbol_ErrIcon,
  Symbol_ErrTFile,
  Symbol_ErrSave,
  Symbol_ErrLoad,
  Symbol_ErrSnapR,
  Symbol_ErrSnapW,
  Symbol_ErrFullScr,
  Symbol_NumSymbols
};

static char *SymbolStrings[] = {
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
  "\\TitLicense",
  "\\TitWarrant",
  "\\TitContrib",
  "\\DlgExtendImage",
  "\\ErrTemplateFmt",
  "\\ErrSpriteMem",
  "\\ErrSpriteFmt",
  "\\ErrIconFmt",
  "\\ErrTempFileFmt",
  "\\ErrSaveFmt",
  "\\ErrLoadFmt",
  "\\ErrSnapRead",
  "\\ErrSnapWrite",
  "\\ErrFullScreen",
  NULL
};





static Joy_Keys JoyToIcon[2] = {
  {Icon_ConfJoy_JoyKey1U, Icon_ConfJoy_JoyKey1D, Icon_ConfJoy_JoyKey1L, Icon_ConfJoy_JoyKey1R, Icon_ConfJoy_JoyKey1F},
  {Icon_ConfJoy_JoyKey2U, Icon_ConfJoy_JoyKey2D, Icon_ConfJoy_JoyKey2L, Icon_ConfJoy_JoyKey2R, Icon_ConfJoy_JoyKey2F}
};




/* Configuration menu */
#define Menu_Config_Items	10
#define Menu_Config_Width	200
#define Menu_Config_Drives	0
#define Menu_Config_Tape	1
#define Menu_Config_Devices	2
#define Menu_Config_Sound	3
#define Menu_Config_System	4
#define Menu_Config_Video	5
#define Menu_Config_Joystick	6
#define Menu_Config_Machine	7
#define Menu_Config_Save	8
#define Menu_Config_Reload	9
static struct MenuConfigure {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Config_Items];
} MenuConfigure = {
  MENU_HEADER("\\MenConfTit", Menu_Config_Width),
  {
    MENU_ITEM("\\MenConfDrv"),
    MENU_ITEM("\\MenConfTap"),
    MENU_ITEM("\\MenConfDev"),
    MENU_ITEM("\\MenConfSnd"),
    MENU_ITEM("\\MenConfSys"),
    MENU_ITEM("\\MenConfVid"),
    MENU_ITEM("\\MenConfJoy"),
    {MFlg_Dotted, (RO_MenuHead*)-1, Menu_Flags, {"\\MenConfMch"}},
    MENU_ITEM("\\MenConfSav"),
    MENU_ITEM_LAST("\\MenConfRel")
  }
};

/* Datasette control menu */
#define Menu_Datasette_Items	6
#define Menu_Datasette_Width	200
#define Menu_Datasette_Stop	0
#define Menu_Datasette_Start	1
#define Menu_Datasette_Forward	2
#define Menu_Datasette_Rewind	3
#define Menu_Datasette_Record	4
#define Menu_Datasette_Reset	5
static struct MenuDatasette {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Datasette_Items];
} MenuDatasette = {
  MENU_HEADER("\\MenDStTit", Menu_Datasette_Width),
  {
    MENU_ITEM("\\MenDStStp"),
    MENU_ITEM("\\MenDStStr"),
    MENU_ITEM("\\MenDStFwd"),
    MENU_ITEM("\\MenDStRwd"),
    MENU_ITEM("\\MenDStRec"),
    MENU_ITEM_LAST("\\MenDStRst")
  }
};

#define Menu_FlipImg_Width	200
static RO_MenuHead *MenuFlipImages = NULL;
static char *MenuFlipImgNames = NULL;

static struct MenuFlipImageTmpl {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuFlipImageTmpl = {
  MENU_HEADER("\\MenFlImTit", Menu_FlipImg_Width),
  {
    MENU_ITEM_LAST("\\MenFlImDet")
  }
};

/* Fliplist menu */
#define Menu_Fliplist_Items	7
#define Menu_Fliplist_Width	200
#define Menu_Fliplist_Attach	0
#define Menu_Fliplist_Detach	1
#define Menu_Fliplist_Next	2
#define Menu_Fliplist_Prev	3
#define Menu_Fliplist_Clear	4
#define Menu_Fliplist_Save	5
#define Menu_Fliplist_Images	6
static struct MenuFliplist {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Fliplist_Items];
} MenuFliplist = {
  MENU_HEADER("\\MenFlpTit", Menu_Fliplist_Width),
  {
    MENU_ITEM("\\MenFlpAtt"),
    MENU_ITEM("\\MenFlpDet"),
    MENU_ITEM("\\MenFlpNxt"),
    MENU_ITEM("\\MenFlpPrv"),
    MENU_ITEM("\\MenFlpClr"),
    {MFlg_Warning, (RO_MenuHead*)-1, Menu_Flags, {"\\MenFlpSav"}},
    MENU_ITEM_LAST("\\MenFlpImg")
  }
};

/* Icon bar menu */
#define Menu_IBar_Items		9
#define Menu_IBar_Width		200
#define Menu_IBar_Info		0
#define Menu_IBar_Configure	1
#define Menu_IBar_License	2
#define Menu_IBar_Warranty	3
#define Menu_IBar_Contrib	4
#define Menu_IBar_LogWin	5
#define Menu_IBar_CreateDisc	6
#define Menu_IBar_FullScreen	7
#define Menu_IBar_Quit		8
static struct MenuIconBar {
  RO_MenuHead head;
  RO_MenuItem item[Menu_IBar_Items];
} MenuIconBar = {
  MENU_HEADER("foo", Menu_IBar_Width),
  {
    MENU_ITEM("\\MenIBInfo"),
    MENU_ITEM_SUB("\\MenIBConf", &MenuConfigure),
    MENU_ITEM("\\MenIBLicns"),
    MENU_ITEM("\\MenIBWrnty"),
    MENU_ITEM("\\MenIBCntrb"),
    MENU_ITEM("\\MenIBLog"),
    MENU_ITEM("\\MenIBCreat"),
    MENU_ITEM("\\MenIBFull"),
    MENU_ITEM_LAST("\\MenIBQuit")
  }
};

/* Emu window menu */
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
static struct MenuEmuWindow {
  RO_MenuHead head;
  RO_MenuItem item[Menu_EmuWin_Items];
} MenuEmuWindow = {
  MENU_HEADER("foo", Menu_EmuWin_Width),
  {
    MENU_ITEM_SUB("\\MenEmuConf", &MenuConfigure),
    MENU_ITEM_SUB("\\MenEmuFlip", &MenuFliplist),
    MENU_ITEM("\\MenEmuSnap"),
    {MFlg_Warning, (RO_MenuHead*)-1, Menu_Flags, {"\\MenEmuScr"}},
    MENU_ITEM("\\MenEmuFrz"),
    MENU_ITEM("\\MenEmuPane"),
    MENU_ITEM("\\MenEmuActv"),
    MENU_ITEM("\\MenEmuTrue"),
    MENU_ITEM_SUB("\\MenEmuDSt", &MenuDatasette),
    MENU_ITEM("\\MenEmuSnd"),
    MENU_ITEM_LAST("\\MenEmuMon"),
  }
};

/* Create disc menu */
#define Menu_CrtDisc_Items	7
#define Menu_CrtDisc_Width	200
#define Menu_CrtDisc_X64	0
#define Menu_CrtDisc_G64	1
#define Menu_CrtDisc_D64	2
#define Menu_CrtDisc_D71	3
#define Menu_CrtDisc_D81	4
#define Menu_CrtDisc_D80	5
#define Menu_CrtDisc_D82	6
static struct MenuCreateDiscType {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CrtDisc_Items];
} MenuCreateDiscType = {
  MENU_HEADER("\\MenCrtTit", Menu_CrtDisc_Width),
  {
    MENU_ITEM("X64"),
    MENU_ITEM("G64"),
    MENU_ITEM("D64"),
    MENU_ITEM("D71"),
    MENU_ITEM("D81"),
    MENU_ITEM("D80"),
    MENU_ITEM_LAST("D82")
  }
};

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

static char VideoSyncCustomField[16];



/* Sprite name copied in by the ui init function of the machine */
static RO_IconDesc IBarIcon = {
  -1, 0, 0, 68, 68, 0x301a,
  {""},
  0
};



/* Drive types */
#define DRIVE_TYPE_DISK	0
#define DRIVE_TYPE_FS	1


/* Resource names */
static const char Rsrc_Sound[] = "Sound";
static const char Rsrc_SndRate[] = "SoundSampleRate";
static const char Rsrc_SndBuff[] = "SoundBufferSize";
static const char Rsrc_Snd16Bit[] = "Use16BitSound";
static const char Rsrc_ReSid[] = "SidUseResid";
static const char Rsrc_ReSidPass[] = "SidResidPassband";
static const char Rsrc_True[] = "DriveTrueEmulation";
static const char Rsrc_Poll[] = "PollEvery";
static const char Rsrc_Speed[] = "SpeedEvery";
static const char Rsrc_SndEvery[] = "SoundEvery";
static const char Rsrc_AutoPause[] = "AutoPause";
static const char Rsrc_SpeedLimit[] = "Speed";
static const char Rsrc_MaxSkipped[] = "MaxSkippedFrames";
static const char Rsrc_DriveT8[] = "DriveType8";
static const char Rsrc_DriveT9[] = "DriveType9";
static const char Rsrc_DriveT10[] = "DriveType10";
static const char Rsrc_DriveT11[] = "DriveType11";
static const char Rsrc_DriveF8[] = "DriveFile8";
static const char Rsrc_DriveF9[] = "DriveFile9";
static const char Rsrc_DriveF10[] = "DriveFile10";
static const char Rsrc_DriveF11[] = "DriveFile11";
static const char Rsrc_VideoSync[] = "MachineVideoStandard";
static const char Rsrc_TapeFile[] = "TapeFile";
static const char Rsrc_Conv8P00[] = "FSDevice8ConvertP00";
static const char Rsrc_Conv9P00[] = "FSDevice9ConvertP00";
static const char Rsrc_Conv10P00[] = "FSDevice10ConvertP00";
static const char Rsrc_Conv11P00[] = "FSDevice11ConvertP00";
static const char Rsrc_Save8P00[] = "FSDevice8SaveP00";
static const char Rsrc_Save9P00[] = "FSDevice9SaveP00";
static const char Rsrc_Save10P00[] = "FSDevice10SaveP00";
static const char Rsrc_Save11P00[] = "FSDevice11SaveP00";
static const char Rsrc_Hide8CBM[] = "FSDevice8HideCBMFiles";
static const char Rsrc_Hide9CBM[] = "FSDevice9HideCBMFiles";
static const char Rsrc_Hide10CBM[] = "FSDevice10HideCBMFiles";
static const char Rsrc_Hide11CBM[] = "FSDevice11HideCBMFiles";
static const char Rsrc_ACIAD7[] = "AciaD7";
static const char Rsrc_CharGen[] = "CharGenName";
static const char Rsrc_Kernal[] = "KernalName";
static const char Rsrc_Basic[] = "BasicName";
static const char Rsrc_CartF[] = "CartridgeFile";
static const char Rsrc_Palette[] = "PaletteFile";
static const char Rsrc_SoundFile[] = "SoundDeviceArg";
static const char Rsrc_SerialFile[] = "SerialFile";
static const char Rsrc_PrinterFile[] = "PrinterTextDevice1";
static const char Rsrc_PetCrt[] = "Crtc";
static const char Rsrc_PetRAM9[] = "Ram9";
static const char Rsrc_PetRAMA[] = "RamA";
static const char Rsrc_PetDiag[] = "DiagPin";
static const char Rsrc_PetSuper[] = "SuperPET";
static const char Rsrc_FullScrNorm[] = "ScreenMode";
static const char Rsrc_FullScrPal[] = "ScreenModePAL";
static const char Rsrc_FullScrPal2[] = "ScreenModeDouble";
static const char Rsrc_VDCpalette[] = "VDC_PaletteFile";
static const char Rsrc_CoreDump[] = "DoCoreDump";
static const char Rsrc_Z80Bios[] = "Z80BiosName";
static const char Rsrc_ColourSat[] = "ColorSaturation";
static const char Rsrc_Contrast[] = "ColorContrast";
static const char Rsrc_Brightness[] = "ColorBrightness";
static const char Rsrc_Gamma[] = "ColorGamma";
static const char Rsrc_DelLoop[] = "PALEmulation";
static const char Rsrc_LineShade[] = "PALScanLineShade";

static const char *Rsrc_ConvP00[4] = {
  Rsrc_Conv8P00, Rsrc_Conv9P00, Rsrc_Conv10P00, Rsrc_Conv11P00
};
static const char *Rsrc_SaveP00[4] = {
  Rsrc_Save8P00, Rsrc_Save9P00, Rsrc_Save10P00, Rsrc_Save11P00
};
static const char *Rsrc_HideCBM[4] = {
  Rsrc_Hide8CBM, Rsrc_Hide9CBM, Rsrc_Hide10CBM, Rsrc_Hide11CBM
};




static disp_desc_t *MenuDisplayROMSet = NULL;

static struct MenuDisplayROMSetTmpl {
  disp_desc_t dd;
  int values[1];
} MenuDisplayROMSetTmpl = {
  {NULL, {CONF_WIN_SYSTEM, Icon_ConfSys_ROMSetT},
    (RO_MenuHead*)&MenuROMSetTmpl, 1, DISP_DESC_STRING, 0},
  {0}
};





/*
 *  Resource functions
 */
static int set_sound_every(resource_value_t v, void *param)
{
  SoundPollEvery = (int)v;	/* actually defined in soundacorn */
  return 0;
}

static int set_drive_type8(resource_value_t v, void *param)
{
  DriveType8 = (int)v;
  return 0;
}

static int set_drive_type9(resource_value_t v, void *param)
{
  DriveType9 = (int)v;
  return 0;
}

static int set_drive_type10(resource_value_t v, void *param)
{
  DriveType10 = (int)v;
  return 0;
}

static int set_drive_type11(resource_value_t v, void *param)
{
  DriveType11 = (int)v;
  return 0;
}

static int set_drive_file8(resource_value_t v, void *param)
{
  util_string_set(&DriveFile8, (const char *)v);
  return 0;
}

static int set_drive_file9(resource_value_t v, void *param)
{
  util_string_set(&DriveFile9, (const char*)v);
  return 0;
}

static int set_drive_file10(resource_value_t v, void *param)
{
  util_string_set(&DriveFile10, (const char*)v);
  return 0;
}

static int set_drive_file11(resource_value_t v, void *param)
{
  util_string_set(&DriveFile11, (const char*)v);
  return 0;
}

static int set_tape_file(resource_value_t v, void *param)
{
  util_string_set(&TapeFile, (const char*)v);
  return 0;
}

static int set_auto_pause(resource_value_t v, void *param)
{
  AutoPauseEmu = (int)v;
  return 0;
}

static int set_16bit_sound(resource_value_t v, void *param)
{
  Use16BitSound = (int)v;
  return 0;
}



static resource_t resources[] = {
  {Rsrc_SndEvery, RES_INTEGER, (resource_value_t)0,
    (resource_value_t*)&SoundPollEvery, set_sound_every, NULL },
  {Rsrc_AutoPause, RES_INTEGER, (resource_value_t)0,
    (resource_value_t*)&AutoPauseEmu, set_auto_pause, NULL },
  {Rsrc_DriveT8, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType8, set_drive_type8, NULL },
  {Rsrc_DriveT9, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType9, set_drive_type9, NULL },
  {Rsrc_DriveT10, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType10, set_drive_type10, NULL },
  {Rsrc_DriveT11, RES_INTEGER, (resource_value_t)DRIVE_TYPE_FS,
    (resource_value_t*)&DriveType11, set_drive_type11, NULL },
  {Rsrc_DriveF8, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile8, set_drive_file8, NULL },
  {Rsrc_DriveF9, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile9, set_drive_file9, NULL },
  {Rsrc_DriveF10, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile10, set_drive_file10, NULL },
  {Rsrc_DriveF11, RES_STRING, (resource_value_t)"@",
    (resource_value_t*)&DriveFile11, set_drive_file11, NULL },
  {Rsrc_TapeFile, RES_STRING, (resource_value_t)"",
    (resource_value_t*)&TapeFile, set_tape_file, NULL },
  {Rsrc_Snd16Bit, RES_INTEGER, (resource_value_t)0,
    (resource_value_t*)&Use16BitSound, set_16bit_sound, NULL },
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
      dx = (default_screen_width << UseEigen) * EmuZoom;
      dy = (default_screen_height << UseEigen) * EmuZoom;
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


int ui_resources_init(void)
{
  return resources_register(resources);
}


int ui_cmdline_options_init(void)
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
void ui_set_icons_grey(RO_Window *w, const conf_iconid_t *desc, int state)
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

  wimp_window_set_icon_state(ConfWindows[CONF_WIN_DRIVES], Icon_ConfDrv_TrueDrv, (state == 0) ? 0 : IFlg_Slct, IFlg_Slct);

  eor = (state == 0) ? 0 : IFlg_Grey;
  for (i=2; i<4; i++)
  {
    wimp_window_set_icon_state(EmuPane, LEDtoIcon[i], eor, IFlg_Grey);
  }

  ui_set_icons_grey(NULL, TrueDependentIcons, state);

  if (state == 0)
  {
    wimp_window_write_icon_text_u(EmuPane, Icon_Pane_TrkSec, "");
    for (i=0; i<2; i++) DriveTrackNumbers[i] = 0;
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

  wimp_window_set_icon_state(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_SoundOn, (state == 0) ? 0 : IFlg_Slct, IFlg_Slct);

  ui_set_icons_grey(NULL, SoundDependentIcons, state);
  if ((machine_class == VICE_MACHINE_C64) || (machine_class == VICE_MACHINE_C128) || (machine_class == VICE_MACHINE_CBM2))
  {
    ui_set_icons_grey(NULL, SidDependentIcons, state);
  }

  wimp_menu_tick_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Sound, state);

  wimp_window_redraw_icon(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_Volume);
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
  else if (ActiveCanvas != NULL)
  {
    int block[WindowB_WFlags+1];
    RO_Window *win = ActiveCanvas->window;

    block[WindowB_Handle] = win->Handle;
    Wimp_GetWindowState(block);
    if ((block[WindowB_WFlags] & (1<<16)) != 0)
    {
      ui_open_emu_window(win, block);
    }
  }
}




static void ui_set_menu_disp_strshow(const disp_desc_t *dd)
{
  disp_strshow_t *ds;
  char **resources;
  resource_value_t val;
  int greyflag;

  ds = ((disp_strshow_t*)(dd->resource));
  resources = (char**)(dd + 1);
  if (resources_get_value(resources[ds->item], &val) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[dd->id.win], ds->icon, (char*)val);
    greyflag = 0;
  }
  else
  {
    greyflag = IFlg_Grey;
  }
  wimp_window_set_icon_state(ConfWindows[dd->id.win], dd->id.icon, greyflag, IFlg_Grey);
  wimp_window_set_icon_state(ConfWindows[dd->id.win], ds->icon, greyflag, IFlg_Grey);
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


static void ui_set_menu_display_text(const disp_desc_t *dd, int number, RO_MenuHead *menu)
{
  RO_MenuItem *item;
  RO_Icon *icon;
  int len;

  item = (RO_MenuItem*)(menu + 1);
  wimp_menu_tick_exclusive(menu, number);
  if ((icon = wimp_window_get_icon(ConfWindows[dd->id.win], dd->id.icon)) == NULL) return;
  if ((icon->iflags & IFlg_Indir) == 0) return;
  len = icon->dat.ind.len - 1;
  if ((item[number].iflags & IFlg_Indir) == 0)
  {
    if (len > 12)
      len = 12;
    strncpy((char*)(icon->dat.ind.tit), item[number].dat.strg, len);
  }
  else
  {
    if (len > item[number].dat.ind.len-1)
      len = item[number].dat.ind.len-1;
    strncpy((char*)(icon->dat.ind.tit), item[number].dat.ind.tit, len);
  }
  ((char*)(icon->dat.ind.tit))[len] = 0;
  wimp_window_redraw_icon(ConfWindows[dd->id.win], dd->id.icon);
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
    ui_set_menu_display_text(dd, i, menu);
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
      int greyflag;

      if (resources_get_value(values[i], &val) == 0)
      {
        if (val != 0) bits |= (1<<i);
        greyflag = 0;
      }
      else
      {
        greyflag = IFlg_Grey;
      }
      wimp_window_set_icon_state(ConfWindows[dd->id.win], dd->id.icon, greyflag, IFlg_Grey);
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
    int greyflag;
    if (resources_get_value(dd->resource, &val) == 0)
    {
      ui_setup_menu_disp_core(dd, val);
      greyflag = 0;
    }
    else
    {
      greyflag = IFlg_Grey;
    }
    wimp_window_set_icon_state(ConfWindows[dd->id.win], dd->id.icon, greyflag, IFlg_Grey);
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
    ui_set_menu_display_text(dd, number, menu);
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
  if (!vsid_mode)
  {
    int info[4];

    vsync_suspend_speed_eval();

    if ((ReadCatalogueInfo(file, info) & 1) == 0) return -1;
    file_system_detach_disk(8 + number);
    if (file_system_attach_disk(8 + number, file) == 0)
    {
      util_string_set(DriveFiles[number], file);
      *(DriveTypes[number]) = DRIVE_TYPE_DISK;
      wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number], file);
      return 0;
    }
    return -1;
  }
  return 0;	/* just ignore, no error */
}


void ui_display_drive_dir(int number, const char *dir)
{
  util_string_set(DriveFiles[number], dir);
  *(DriveTypes[number]) = DRIVE_TYPE_FS;
  wimp_window_write_icon_text(ConfWindows[CONF_WIN_DRIVES], DriveToFile[number], dir);
}


static int ui_set_drive_dir(int number, const char *dir)
{
  if (!vsid_mode)
  {
    int info[4];

    vsync_suspend_speed_eval();

    if ((ReadCatalogueInfo(dir, info) & 2) == 0) return -1;
    file_system_detach_disk(8 + number);
    fsdevice_set_directory((char*)dir, 8 + number);
    ui_display_drive_dir(number, dir);
  }
  return 0;
}


static void ui_detach_drive_image(int number)
{
  if (!vsid_mode)
  {
    RO_MenuHead *menu;
    RO_MenuItem *item;

    vsync_suspend_speed_eval();

    menu = ConfigMenus[CONF_MENU_DRIVE8 + number].menu;
    item = (RO_MenuItem*)(menu + 1);
    ui_set_drive_dir(number, "@");
    wimp_menu_set_grey_all(item[Menu_DriveType_Disk].submenu, 1);
    wimp_menu_set_grey_all(item[Menu_DriveType_FS].submenu, 0);
    wimp_menu_tick_exclusive(menu, Menu_DriveType_FS);
  }
}


static int ui_set_tape_image(const char *name)
{
  if (!vsid_mode)
  {
    RO_Window *win;
    int state;

    vsync_suspend_speed_eval();

    win = ConfWindows[CONF_WIN_TAPE];

    if ((name == NULL) || (wimp_strlen(name) == 0))
    {
      if (tape_image_detach(1) != 0)
        return -1;

      util_string_set(&TapeFile, "");
      wimp_window_write_icon_text(win, Icon_ConfTap_TapeFile, TapeFile);
      state = 0;
    }
    else
    {
      if (tape_image_attach(1, name) != 0)
        return -1;

      util_string_set(&TapeFile, name);
      wimp_window_write_icon_text(win, Icon_ConfTap_TapeFile, name);
      state = 1;
    }

    ui_set_icons_grey(NULL, TapeFileDependentIcons, state);
  }
  return 0;
}


static int ui_new_drive_image(int number, const char *name, int scankeys)
{
  int type;
  int aux[4];

  vsync_suspend_speed_eval();

  type = ReadCatalogueInfo(name, aux);
  /* directories or image files that aren't D64 images are attached as dirs */
  if ((type == 2) || ((type == 3) && (((aux[0]>>8) & 0xfff) != FileType_D64Image)))
  {
    if (!scankeys || (ScanKeys(IntKey_Shift) == 0xff))
    {
      if (ui_set_drive_dir(number, name) != 0)
        return -1;
    }
    else
    {
      if (ui_image_contents_dir(name) != 0)
        return -1;
    }
  }
  else
  {
    if (!scankeys || (ScanKeys(IntKey_Shift) == 0xff))
    {
      if (ui_set_drive_image(number, name) != 0)
        return -1;
    }
    else
    {
      if (ui_image_contents_disk(name) != 0);
        return -1;
    }
  }
  return 0;
}


static int ui_new_tape_image(const char *name, int scankeys)
{
  if (!scankeys || (ScanKeys(IntKey_Shift) == 0xff))
  {
    if (ui_set_tape_image(name) != 0)
      return -1;
  }
  else
  {
    if (ui_image_contents_tape(name) != 0)
      return -1;
  }
  return 0;
}


static int ui_flip_attach_image_no(int number)
{
  void *iter;
  int i;

  iter = flip_init_iterate(FlipListDrive + 8);
  for (i=0; i<number; i++) iter = flip_next_iterate(FlipListDrive + 8);
  if (iter != NULL)
  {
    const char *img;

    img = flip_get_image(iter);
    if (img != NULL)
      ui_new_drive_image(FlipListDrive, img, 0);

    if (MenuFlipImages != NULL)
      wimp_menu_tick_exclusive(MenuFlipImages, number+1);
  }
  return 0;
}


int ui_flip_iterate_and_attach(int dir)
{
  if (FlipListNumber > 0)
  {
    if (dir > 0)
    {
      FlipListIter++;
      if (FlipListIter >= FlipListNumber)
        FlipListIter = 0;
    }
    else
    {
      FlipListIter--;
      /* the list may have shrunk considerably in between! */
      if ((FlipListIter < 0) || (FlipListIter >= FlipListNumber))
        FlipListIter = FlipListNumber-1;
    }
    return ui_flip_attach_image_no(FlipListIter);
  }
  return -1;
}


static int ui_caret_to_last_focus(void)
{
  if (LastCaret.WHandle != -1)
  {
    Wimp_SetCaretPosition(LastCaret.WHandle, LastCaret.IHandle, LastCaret.offx, LastCaret.offy, LastCaret.height, LastCaret.index);
    LastCaret.WHandle = -1;
    return 1;
  }
  return 0;
}


static void ui_set_create_image_type(int number)
{
  wimp_menu_icon_set_number((RO_MenuHead*)&MenuCreateDiscType, CreateDiscWindow, Icon_Create_TypeT, number);
}


static int ui_set_sound_file(const char *name)
{
  if (resources_set_value(Rsrc_SoundFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_FileSndPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_serial_file(const char *name)
{
  if (resources_set_value(Rsrc_SerialFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_ConfDev_FileRsPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_printer_file(const char *name)
{
  if (resources_set_value(Rsrc_PrinterFile, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_ConfDev_FilePrPath, name);
    return 0;
  }
  return -1;
}


static int ui_set_cartridge_file(const char *name)
{
  vsync_suspend_speed_eval();

  if (resources_set_value(Rsrc_CartF, (resource_value_t)name) == 0)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_SYSTEM], Icon_ConfSys_CartFile, name);
    return 0;
  }
  return -1;
}


static int ui_make_snapshot(const char *name)
{
  int save_roms, save_disks, status;
  int block[10];

  vsync_suspend_speed_eval();

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
    SetFileType(name, FileType_Data);
  }
  else
  {
    _kernel_oserror err;

    /* else delete the file */
    err.errnum = 0; strcpy(err.errmess, SymbolStrings[Symbol_ErrSnapW]);
    Wimp_ReportError(&err, 1, WimpTaskName);
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
  /*log_message(LOG_DEFAULT, "Save snapshot %s", name);*/

  if (wimp_check_for_path(name) == 0)
  {
    wimp_strcpy(((char*)SnapshotMessage)+44, name);
    maincpu_trigger_trap(ui_save_snapshot_trap, NULL);
    SnapshotPending = 1;
  }
  return -1;
}

int ui_save_last_snapshot(void)
{
  return ui_check_save_snapshot(((char*)SnapshotMessage)+44);
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
    if (LastMenu == CONF_MENU_SYSKBD + 0x100)
    {
      if (kbd_dump_keymap(name, -1) == 0)
      {
        wimp_strcpy(SystemKeymapFile, name);
        Wimp_CreateMenu((int*)-1, 0, 0);
      }
    }
  }
  return -1;
}


/* b = update / redraw block */
static void ui_draw_sound_volume(int *b)
{
  RO_Icon *icon;
  int minx, miny, maxx, maxy, thresh;

  icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_Volume);
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

  icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_Volume);
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
  wimp_window_redraw_icon(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_Volume);
}


static const char *ui_check_for_syspath(const char *path)
{
  const char *vicepath;
  int len;

  if ((vicepath = getenv(VicePathVariable)) == NULL) return path;
  len = strlen(vicepath);
  if (strncasecmp(path, vicepath, len) == 0)
  {
    vicepath = path + len;
    len = strlen(machine_name);
    if ((strncasecmp(vicepath, machine_name, len) == 0) && (vicepath[len] == '.'))
      return vicepath + len + 1;
    len = strlen(ResourceDriveDir);
    if ((strncasecmp(vicepath, ResourceDriveDir, len) == 0) && (vicepath[len] == '.'))
      return vicepath + len + 1;
  }
  return path;
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
    if ((MenuROMSetTmpl.item[0].mflags & MFlg_FirstInd) != 0)
      item[0].mflags |= MFlg_FirstInd;
    item[0].mflags |= MFlg_Tick;
    ConfigMenus[CONF_MENU_ROMSET].menu = MenuROMSet;
    ConfigDispDescs[CONF_MENU_ROMSET] = MenuDisplayROMSet;
    MenuDisplayROMSet->menu = MenuROMSet;
    MenuDisplayROMSet->items = number;
    ui_set_menu_display_text(MenuDisplayROMSet, 0, MenuROMSet);
    return 0;
  }
  return -1;
}


static int ui_build_fliplist_menu(int doread)
{
  void *iter;
  int textsize = 0;
  const char *img;

  FlipListNumber = 0;
  if (doread)
  {
    if (MenuFlipImages != NULL) free(MenuFlipImages);
    if (MenuFlipImgNames != NULL) free(MenuFlipImgNames);
    MenuFlipImages = NULL; MenuFlipImgNames = NULL;
    iter = flip_init_iterate(FlipListDrive + 8);
    while (iter != NULL)
    {
      img = flip_get_image(iter);
      textsize += (strlen(archdep_extract_dir_and_leaf(img)) + 4) & ~3;
      FlipListNumber++;
      iter = flip_next_iterate(FlipListDrive + 8);
    }
  }
  if (FlipListNumber <= 0)
  {
    MenuFliplist.item[Menu_Fliplist_Images].submenu = (RO_MenuHead*)&MenuFlipImageTmpl;
    return 0;
  }
  MenuFlipImages = (RO_MenuHead*)malloc(sizeof(RO_MenuHead) + (FlipListNumber+1) * sizeof(RO_MenuHead));
  MenuFlipImgNames = (char*)malloc(textsize);
  if ((MenuFlipImages != NULL) && (MenuFlipImgNames != NULL))
  {
    RO_MenuItem *firstitem, *item;
    char *b = MenuFlipImgNames;

    memcpy(MenuFlipImages, &MenuFlipImageTmpl, sizeof(RO_MenuHead) + sizeof(RO_MenuItem));
    firstitem = (RO_MenuItem*)(MenuFlipImages + 1); item = firstitem;
    item->mflags |= MFlg_Dotted; item->mflags &= ~MFlg_LastItem;
    item++;
    iter = flip_init_iterate(FlipListDrive + 8);
    while (iter != NULL)
    {
      const char *use;
      int len;

      img = flip_get_image(iter);
      use = archdep_extract_dir_and_leaf(img);
      len = (strlen(use) + 4) & ~3;
      strcpy(b, use);
      item->mflags = 0; item->submenu = (RO_MenuHead*)-1;
      item->iflags = Menu_Flags | IFlg_Indir;
      item->dat.ind.tit = (int*)b; item->dat.ind.val = NULL; item->dat.ind.len = len;
      b += len;
      iter = flip_next_iterate(FlipListDrive + 8);
      item++;
    }
    item[-1].mflags = MFlg_LastItem;
    if ((MenuFlipImageTmpl.item[0].mflags & MFlg_FirstInd) != 0)
      firstitem->mflags |= MFlg_FirstInd;
    MenuFliplist.item[Menu_Fliplist_Images].submenu = MenuFlipImages;
    FlipListIter = 0;
    wimp_menu_tick_exclusive(MenuFlipImages, FlipListIter+1);
    return 0;
  }
  return -1;
}


/*
 *  The file system may be initialized after ui_init_finish, therefore the only
 *  way to attach images on startup is to delay it. So let's just use a fake
 *  Wimp message.
 */
static int ui_send_fake_data_load(RO_Window *win, int icon, const char *file)
{
  int type;
  int aux[4];

  type = ReadCatalogueInfo(file, aux);
  if (type != 0)
  {
    int len = wimp_strlen(file);

    WimpBlock[MsgB_Size] = (44 + len + 4) & ~3;
    WimpBlock[MsgB_YourRef] = 0;
    WimpBlock[MsgB_Action] = Message_DataLoad;
    WimpBlock[5] = win->Handle;
    WimpBlock[6] = icon;
    WimpBlock[7] = 0; WimpBlock[8] = 0;	/* x/y coordinate dummies */
    WimpBlock[9] = aux[2];
    if ((aux[0] & 0xfff00000) == 0xfff00000)
      WimpBlock[10] = (aux[0] >> 8) & 0xfff;
    else
      WimpBlock[10] = 0;
    wimp_strcpy(((char*)WimpBlock) + 44, file);
    /* hello me, ... */
    Wimp_SendMessage(WimpEvt_UserMsgRec, WimpBlock, TaskHandle, 0);
  }
  return -1;
}


void ui_issue_reset(int doreset)
{
  unsigned int i;

  for (i=0; i<4; i++) ui_set_drive_leds(i, 0);

  if (doreset != 0) maincpu_trigger_reset();
}



/* Make absolutely sure the sound timer is killed when the app terminates */
static void ui_safe_exit(void)
{
  int docoredump;

  sound_wimp_safe_exit();

  if (resources_get_value(Rsrc_CoreDump, (resource_value_t)&docoredump) != 0)
    docoredump = 0;

  if (docoredump != NULL)
  {
    int current, next, free;
    FILE *fp;

    next = -1; free = -1;
    Wimp_SlotSize(&current, &next, &free);
    if ((fp = fopen("core", "wb")) != NULL)
    {
      fwrite((void*)0x8000, 1, current, fp);
      fclose(fp);
    }
  }

  if (RegularProgramExit == 0)
    sound_close();

  archdep_closedown();
}


static void ui_open_log_window(void)
{
  if (archdep_get_default_log_file() != NULL)
  {
    ui_message_window_open(msg_win_log, "VICE log window", "\n", 100, 16);
  }
}


/* Shared by all uis for installing the icon bar icon */
int ui_init(int *argc, char *argv[])
{
  int block[4];
  int x, y, mode;
  char buffer[64];
  char *msgpool;
  const char *iname;
  wimp_msg_desc *msg;
  WIdatI *dat;
  RO_MenuItem *item;

  PollMask = 0x01000830;	/* save/restore FP regs */
  LastMenu = 0; LastClick = 0; LastDrag = 0; LastSubDrag = 0; MenuType = 0; DragType = 0;
  EmuZoom = 1;

  /* make sure all config menus are defined, if only temporarily */
  ConfigMenus[CONF_MENU_ROMSET].menu = (RO_MenuHead*)&MenuROMSetTmpl;
  ui_bind_video_cache_menu();

  if ((msg = wimp_message_init(MessagesFile)) == NULL)
  {
    log_error(roui_log, "Unable to open messages file!\n");
    exit(-1);
  }

  /* Init internal messages of wimp.c */
  wimp_init_messages(msg);
  ui_message_init_messages(msg);

  /* Translate message pool in two passes: 1) determine lenght, 2) actually do it */
  x = wimp_message_translate_symbols(msg, SymbolStrings, NULL);
  if ((msgpool = (char*)malloc(x)) == NULL)
  {
    log_error(roui_log, "Unable to claim memory for symbol pool!\n");
    exit(-1);
  }
  wimp_message_translate_symbols(msg, SymbolStrings, msgpool);

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

  if (vsid_mode)
    WimpTaskName = "Vice VSID";

  TaskHandle = Wimp_Initialise(310, TASK_WORD, WimpTaskName, (int*)WimpMessages);
  strncpy(MenuIconBar.head.title, WimpTaskName, 12);
  strncpy(MenuEmuWindow.head.title, WimpTaskName, 12);

  iname = ui_get_machine_ibar_icon();

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
    ui_load_template("TapeConfig", ConfWindows + CONF_WIN_TAPE, msg);
    ui_load_template("DevConfig", ConfWindows + CONF_WIN_DEVICES, msg);
    ui_load_template("SoundConfig", ConfWindows + CONF_WIN_SOUND, msg);
    ui_load_template("SysConfig", ConfWindows + CONF_WIN_SYSTEM, msg);
    ui_load_template("VideoConfig", ConfWindows + CONF_WIN_VIDEO, msg);
    ui_load_template("JoyConfig", ConfWindows + CONF_WIN_JOY, msg);
    ui_load_template("PetConfig", ConfWindows + CONF_WIN_PET, msg);
    ui_load_template("VicConfig", ConfWindows + CONF_WIN_VIC, msg);
    ui_load_template("CBM2Config", ConfWindows + CONF_WIN_CBM2, msg);
    ui_load_template("C128Config", ConfWindows + CONF_WIN_C128, msg);
    ui_load_template("Snapshot", &SnapshotWindow, msg);
    ui_load_template("CPUJamBox", &CpuJamWindow, msg);
    ui_load_template("SaveBox", &SaveBox, msg);
    ui_load_template("ImageCont", &ImgContWindow, msg);
    ui_load_template("MsgWindow", &MessageWindow, msg);
    ui_load_template("CreateDisc", &CreateDiscWindow, msg);

    if (vsid_mode)
      ui_load_template("VSidWindow", &VSidWindow, msg);

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
  wimp_message_translate_menu_indirect(msg, (RO_MenuHead*)&MenuIconBar, MenuIndirectSize);
  wimp_message_translate_menu_indirect(msg, (RO_MenuHead*)&MenuEmuWindow, MenuIndirectSize);
  wimp_message_translate_menu(msg, (RO_MenuHead*)&MenuFlipImageTmpl);
  wimp_message_translate_menu(msg, (RO_MenuHead*)&MenuCreateDiscType);
  for (x=0; ConfigMenus[x].menu != NULL; x++)
  {
    wimp_message_translate_menu_indirect(msg, ConfigMenus[x].menu, MenuIndirectSize);
  }
  if (vsid_mode)
    vsid_ui_message_hook(msg);

  /* Misc */

  sprintf(EmuTitle, "%s (%s)", WimpTaskName, VERSION);

  wimp_window_write_title(EmuWindow, EmuTitle);

  sprintf(buffer, SymbolStrings[Symbol_Purpose], (vsid_mode) ? "SID" : machine_name);
  wimp_window_write_icon_text(InfoWindow, Icon_Info_Purpose, buffer);
  sprintf(buffer, "%s (%s)", VERSION, SymbolStrings[Symbol_Date]);
  wimp_window_write_icon_text(InfoWindow, Icon_Info_Version, buffer);
  MenuIconBar.item[Menu_IBar_Info].submenu = (RO_MenuHead*)(InfoWindow->Handle);
  MenuEmuWindow.item[Menu_EmuWin_Snapshot].submenu = (RO_MenuHead*)(SnapshotWindow->Handle);
  MenuEmuWindow.item[Menu_EmuWin_Screenshot].submenu = (RO_MenuHead*)(SaveBox->Handle);
  MenuFliplist.item[Menu_Fliplist_Save].submenu = (RO_MenuHead*)(SaveBox->Handle);
  wimp_window_write_icon_text(SnapshotWindow, Icon_Snap_Path, ViceSnapshotFile);
  wimp_window_write_icon_text(CreateDiscWindow, Icon_Create_Name, ViceNewDiscName);
  wimp_window_write_icon_text(CreateDiscWindow, Icon_Create_File, ViceNewDiscFile);
  ui_set_create_image_type(0);
  item = (RO_MenuItem*)(ConfigMenus[CONF_MENU_ROMACT].menu + 1);
  item[Menu_RomAct_Save].submenu = (RO_MenuHead*)(SaveBox->Handle);
  item = (RO_MenuItem*)(item[Menu_RomAct_Create].submenu + 1);
  item->iflags |= IFlg_Indir;
  dat = &(item->dat.ind);
  dat->tit = (int*)NewRomSetName; dat->val = (int*)-1; dat->len = sizeof(NewRomSetName);
  NewRomSetName[0] = '\0';
  item = (RO_MenuItem*)(ConfigMenus[CONF_MENU_SYSKBD].menu + 1);
  item[Menu_SysKbd_Save].submenu = (RO_MenuHead*)(SaveBox->Handle);
  item = (RO_MenuItem*)(ConfigMenus[CONF_MENU_TRUESYNC].menu + 1);
  item[Menu_VideoSync_Custom].iflags |= IFlg_Indir;
  dat = &(item[Menu_VideoSync_Custom].dat.ind);
  dat->tit = (int*)VideoSyncCustomField; dat->val = (int*)-1; dat->len = sizeof(VideoSyncCustomField);

  VideoSyncCustomField[0] = '\0';
  sprintf(ROMSetItemFile, "rset/"RSETARCH_EXT);
  sprintf(SystemKeymapFile, "ROdflt/"KEYMAP_EXT);
  sprintf(ViceScreenshotFile, "scrshot");
  strcpy(ViceFliplistFile, ViceDfltFlipFile);

  EmuPaused = 0; LastCaret.WHandle = -1;
  SoundVolume = Sound_Volume(0);

  if (machine_class == VICE_MACHINE_C128)
  {
    config_item_t *ci;

    wimp_window_write_icon_text(ConfWindows[CONF_WIN_DEVICES], Icon_ConfDev_ACIAD67, SymbolStrings[Symbol_ACIAD7]);
    for (ci=Configurations; ci->resource != NULL; ci++)
    {
      if ((ci->id.win == CONF_WIN_DEVICES) && (ci->id.icon == Icon_ConfDev_ACIAD67))
      {
        ci->resource = Rsrc_ACIAD7; break;
      }
    }
  }

  ui_translate_help_messages(msg);

  wimp_message_delete(msg);

  ui_message_init();

  return 0;
}


int ui_init_finish(void)
{
  resource_value_t val;
  disp_desc_t *dd;
  int i;

  if (machine_class != VICE_MACHINE_PET)
  {
    if (resources_get_value(Rsrc_True, &val) == 0)
      TrueDriveEmulation = (int)val;
  }

  /* Setup the drives */
  for (i=0; i<4; i++)
  {
    if ((*(DriveFiles[i]) != NULL) && (strlen(*(DriveFiles[i])) > 0))
      ui_send_fake_data_load(ConfWindows[CONF_WIN_DRIVES], DriveToFile[i], *(DriveFiles[i]));
  }

  ui_set_truedrv_emulation((int)TrueDriveEmulation);

  if ((TapeFile != NULL) && (strlen(TapeFile) > 0))
    ui_send_fake_data_load(ConfWindows[CONF_WIN_TAPE], Icon_ConfTap_TapeFile, TapeFile);
  else
    ui_set_icons_grey(NULL, TapeFileDependentIcons, 0);

  if (resources_get_value(Rsrc_Sound, &val) == 0)
    ui_set_sound_enable((int)val);

  CMOS_DragType = ReadDragType();

  ui_grey_out_machine_icons();

  if ((machine_class != VICE_MACHINE_PET) && (machine_class != VICE_MACHINE_VIC20) && (machine_class != VICE_MACHINE_CBM2) && (machine_class != VICE_MACHINE_C128))
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

  if (vsid_mode)
  {
    RO_MenuHead *men;

    men = (RO_MenuHead*)&MenuEmuWindow;
    wimp_menu_set_grey_item(men, Menu_EmuWin_Fliplist, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Snapshot, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Screenshot, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Freeze, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Pane, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Active, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_TrueDrvEmu, 1);
    wimp_menu_set_grey_item(men, Menu_EmuWin_Datasette, 1);

    men = (RO_MenuHead*)&MenuIconBar;
    wimp_menu_set_grey_item(men, Menu_IBar_CreateDisc, 1);
    wimp_menu_set_grey_item(men, Menu_IBar_FullScreen, 1);

    men = (RO_MenuHead*)&MenuConfigure;
    wimp_menu_set_grey_item(men, Menu_Config_Drives, 1);
    wimp_menu_set_grey_item(men, Menu_Config_Tape, 1);
    wimp_menu_set_grey_item(men, Menu_Config_Devices, 1);
    wimp_menu_set_grey_item(men, Menu_Config_Joystick, 1);
    wimp_menu_set_grey_item(men, Menu_Config_Video, 1);
  }

  ui_set_pane_state(ShowEmuPane);

  memset(SnapshotMessage, 0, 256);

  /* adjust sample frequency approximations to actual VIDC frequencies */
  dd = ConfigDispDescs[CONF_MENU_SAMPRATE];
  for (i=0; i<dd->items; i++)
  {
    int *values = (int*)(dd + 1);

    sound_get_vidc_frequency(values + i, NULL);
  }
  if (resources_get_value(Rsrc_SndRate, &val) == 0)
  {
    int rate = (int)val;

    sound_get_vidc_frequency(&rate, NULL);
    resources_set_value(Rsrc_SndRate, (resource_value_t)rate);
  }
  /* Sound buffer size sanity check */
  if (resources_get_value(Rsrc_SndBuff, &val) == 0)
  {
    if ((int)val > Maximum_Latency)
    {
      resources_set_value(Rsrc_SndBuff, (resource_value_t)Maximum_Latency);
    }
  }
  /* resid active? */
  resources_get_value(Rsrc_ReSid, (resource_value_t*)&CycleBasedSound);

  ROMSetName = stralloc("Default");

  if (sysfile_locate("romset/"RSETARCH_EXT, &ROMSetArchiveFile) == 0)
  {
    romset_load_archive(ROMSetArchiveFile, 0);
    ui_build_romset_menu();
  }

  ui_build_fliplist_menu(0);

  /* must create log window, but may close it right afterwards! */
  ui_open_log_window();

  /* register callbacks */
  video_register_callbacks();

  atexit(ui_safe_exit);

  return 0;
}


int ui_init_finalize(void)
{
  /* register platform-specific drivers */
  if (!vsid_mode)
    screenshot_init_sprite();

  return 0;
}


static void ui_setup_config_item(config_item_t *ci)
{
  resource_value_t val;

  if (resources_get_value(ci->resource, &val) != 0)
  {
    wimp_window_set_icon_state(ConfWindows[ci->id.win], ci->id.icon, IFlg_Grey, IFlg_Grey);
    return;
  }
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
  for (i=0; ConfigMenus[i].menu != NULL; i++)
  {
    if (ConfigMenus[i].id.win == wnum)
    {
      if (ConfigDispDescs[i] != NULL)
        ui_setup_menu_display(ConfigDispDescs[i]);
    }
  }

  /* Setup menus */
  switch (wnum)
  {
    case CONF_WIN_JOY:
      {
        RO_Window *w = ConfWindows[CONF_WIN_JOY];
        Joy_Keys *jk;
        int i;

        for (i=0; i<2; i++)
        {
          jk = JoystickKeys + i;
          wimp_window_write_icon_text(w, JoyToIcon[i].up, kbd_intkey_to_string(jk->up));
          wimp_window_write_icon_text(w, JoyToIcon[i].down, kbd_intkey_to_string(jk->down));
          wimp_window_write_icon_text(w, JoyToIcon[i].left, kbd_intkey_to_string(jk->left));
          wimp_window_write_icon_text(w, JoyToIcon[i].right, kbd_intkey_to_string(jk->right));
          wimp_window_write_icon_text(w, JoyToIcon[i].fire, kbd_intkey_to_string(jk->fire));
        }
      }
      break;
    case CONF_WIN_PET:
      if (machine_class == VICE_MACHINE_PET)
      {
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_PET], Icon_ConfPET_PetKbd, pet_get_keyboard_name());
      }
      break;
    case CONF_WIN_CBM2:
      if (machine_class == VICE_MACHINE_CBM2)
      {
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_ConfCBM_CBM2Kbd, cbm2_get_keyboard_name());
      }
      break;
    case CONF_WIN_C128:
      /* no menus yet, nothing to do */
      break;
    default: break;
  }
}


static void ui_open_config_window(int wnum)
{
  int block[WindowB_WFlags+1];
  RO_Window *w;

  vsync_suspend_speed_eval();

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


void ui_open_emu_window(RO_Window *win, int *b)
{
  int aux[WindowB_Stackpos+1];
  int paneblk[WindowB_Stackpos+1];
  int *block;
  int dx, dy, x;

  if (vsid_mode)
    return;

  if (b == NULL)
  {
    int dx, dy;

    block = aux;
    aux[WindowB_Handle] = win->Handle;
    dx = win->wmaxx - win->wminx;
    dy = win->wmaxy - win->wminy;
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
  if ((ShowEmuPane != 0) && (ActiveCanvas != NULL) && (win == ActiveCanvas->window))
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


void ui_open_vsid_window(int *block)
{
  ui_open_centered_or_raise_block(VSidWindow, block);
  Wimp_OpenWindow(block);
}


void ui_close_emu_window(RO_Window *win, int *b)
{
  int aux[1];
  int *block;

  if (b == NULL)
  {
    block = aux;
    aux[0] = win->Handle;
  }
  else
  {
    block = b;
  }
  Wimp_CloseWindow(block);

  if ((ActiveCanvas != NULL) && (win == ActiveCanvas->window))
  {
    aux[0] = EmuPane->Handle;
    Wimp_CloseWindow(aux);
  }

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
  struct video_canvas_s *canvas;

  if ((canvas = canvas_for_handle(b[RedrawB_Handle])) != NULL)
  {
    video_redraw_desc_t vrd;
    video_frame_buffer_t *fb = &(canvas->fb);

    vrd.ge.dimx = fb->pitch; vrd.ge.dimy = fb->height;
    vrd.block = b;

    more = Wimp_RedrawWindow(b);
    while (more != 0)
    {
      /* transform WIMP coordinates back to canvas coordinates */
      video_pos_screen_to_canvas(canvas, b, b[RedrawB_CMinX], b[RedrawB_CMaxY], &vrd.xs, &vrd.ys);
      video_pos_screen_to_canvas(canvas, b, b[RedrawB_CMaxX], b[RedrawB_CMinY], &vrd.w, &vrd.h);
      vrd.w -= vrd.xs; vrd.h -= vrd.ys;
      video_canvas_redraw_core(canvas, &vrd);
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
  if ((ActiveCanvas != NULL) && (b[WindowB_Handle] == ActiveCanvas->window->Handle))
  {
    ui_open_emu_window(ActiveCanvas->window, b);
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
  if (currentCaret.WHandle == b[WindowB_Handle])
    ui_caret_to_last_focus();

  if ((ActiveCanvas != NULL) && (b[WindowB_Handle] == ActiveCanvas->window->Handle))
  {
    ui_close_emu_window(ActiveCanvas->window, b);
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


/* Try setting a resource according to a selection box or update state if failed */
static int ui_set_resource_select(const char *name, conf_iconid_t *id)
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


void ui_show_emu_scale(void)
{
  if (ActiveCanvas == NULL) return;

  if (ActiveCanvas->scale == 1)
    wimp_window_write_icon_text(EmuPane, Icon_Pane_Toggle, SymbolStrings[Symbol_Zoom2]);
  else
    wimp_window_write_icon_text(EmuPane, Icon_Pane_Toggle, SymbolStrings[Symbol_Zoom1]);
}


/* returns 0 if window was closed */
static int ui_open_centered_or_raise_block(RO_Window *win, int *block)
{
  int status;

  block[0] = win->Handle;
  Wimp_GetWindowState(block);
  /* Window was closed? Then open centered... */
  if ((block[WindowB_WFlags] & (1<<16)) == 0)
  {
    int dx, dy;

    dx = win->wmaxx - win->wminx;
    dy = win->wmaxy - win->wminy;
    block[WindowB_VMinX] = (ScreenMode.resx - dx) / 2;
    block[WindowB_VMaxX] = block[WindowB_VMinX] + dx;
    block[WindowB_VMinY] = (ScreenMode.resy - dy) / 2;
    block[WindowB_VMaxY] = block[WindowB_VMinY] + dy;
    status = 0;
  }
  else
    status = 1;

  block[WindowB_Stackpos] = -1;

  return status;
}


static int ui_create_new_disc_image(void)
{
  int number;
  unsigned int type;
  char *file, *name;

  vsync_suspend_speed_eval();

  number = wimp_menu_tick_read_first((RO_MenuHead*)&MenuCreateDiscType);

  switch(number)
  {
    case Menu_CrtDisc_X64:
      type = DISK_IMAGE_TYPE_X64; break;
    case Menu_CrtDisc_G64:
      type = DISK_IMAGE_TYPE_G64; break;
    case Menu_CrtDisc_D64:
      type = DISK_IMAGE_TYPE_D64; break;
    case Menu_CrtDisc_D71:
      type = DISK_IMAGE_TYPE_D71; break;
    case Menu_CrtDisc_D81:
      type = DISK_IMAGE_TYPE_D81; break;
    case Menu_CrtDisc_D80:
      type = DISK_IMAGE_TYPE_D80; break;
    case Menu_CrtDisc_D82:
      type = DISK_IMAGE_TYPE_D82; break;
    default:
      return -1;
  }

  file = wimp_window_read_icon_text(CreateDiscWindow, Icon_Create_File);

  if (wimp_check_for_path(file) == 0)
  {
    char *b;

    name = wimp_window_read_icon_text(CreateDiscWindow, Icon_Create_Name);
    b = name;
    while (*b != '\0')
    {
      *b = toupper((unsigned int)(*b));
      b++;
    }
    wimp_window_redraw_icon(CreateDiscWindow, Icon_Create_Name);

    if (vdrive_internal_create_format_disk_image(file, name, type) == 0)
    {
      SetFileType(file, FileType_Data);
      Wimp_CloseWindow((int*)CreateDiscWindow);
      ui_caret_to_last_focus();
      return 0;
    }
  }
  return -1;
}


/*
 *  Mouse click handlers
 */
static void ui_mouse_click_pane(int *b)
{
  if (b[MouseB_Buttons] != 2)
  {
    switch (b[MouseB_Icon])
    {
      case Icon_Pane_Toggle:
        {
          struct video_canvas_s *canvas;

          canvas = ActiveCanvas;
          canvas->scale = (canvas->scale == 1) ? 2 : 1;
          ui_show_emu_scale();
          video_canvas_update_size(canvas);
        }
        break;
      case Icon_Pane_Reset:
        if (b[MouseB_Buttons] == 1) mem_powerup();	/* adjust ==> hard reset */
        ui_issue_reset(1);
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
          ui_display_drive_track_int(DisplayDriveTrack, DriveTrackNumbers[DisplayDriveTrack]);
        }
        break;
      default:
        break;
    }
  }
  else
  {
    int wblock[WindowB_WFlags+1];
    int iblock[IconB_Data2+1];
    int posx, posy;

    wblock[WindowB_Handle] = EmuPane->Handle;
    Wimp_GetWindowState(wblock);
    iblock[IconB_Handle] = EmuPane->Handle;
    iblock[IconB_Number] = Icon_Pane_Drive0;
    Wimp_GetIconState(iblock);
    posx = b[MouseB_PosX] - wblock[WindowB_VMinX];
    posy = b[MouseB_PosY] - wblock[WindowB_VMaxY];
    if (((posx >= iblock[IconB_MinX]) && (posx <= iblock[IconB_MaxX])) &&
        ((posy >= iblock[IconB_MinY]) && (posy <= iblock[IconB_MaxY])))
    {
      RO_MenuHead *menu;

      menu = MenuFliplist.item[Menu_Fliplist_Images].submenu;
      Wimp_CreateMenu((int*)menu, b[MouseB_PosX], b[MouseB_PosY]);
      LastMenu = Menu_Images;
    }
  }
}

static void ui_mouse_click_vsid(int *b)
{
  if (b[MouseB_Buttons] == 2)
  {
    Wimp_CreateMenu((int*)&MenuEmuWindow, b[MouseB_PosX], b[MouseB_PosY]);
    LastHandle = VSidWindow->Handle;
    LastMenu = Menu_Emulator;
  }
  else
  {
    vsid_ui_mouse_click(b);
  }
}

static void ui_mouse_click_canvas(int *b)
{
  struct video_canvas_s *canvas;

  canvas = canvas_for_handle(b[MouseB_Window]);

  if (canvas != NULL)
  {
    if (b[MouseB_Buttons] == 2)
    {
       wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Active, (canvas_get_number() <= 1));
       wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Screenshot, 0);
       Wimp_CreateMenu((int*)&MenuEmuWindow, b[MouseB_PosX], b[MouseB_PosY]);
       LastHandle = canvas->window->Handle;
       LastMenu = Menu_Emulator;
    }
    else
    {
      Wimp_GetCaretPosition(&LastCaret);
      Wimp_SetCaretPosition(canvas->window->Handle, -1, -100, 100, -1, -1);
    }
    return;
  }
}

static void ui_mouse_click_ibar(int *b)
{
  if (b[MouseB_Buttons] == 2)
  {
    Wimp_CreateMenu((int*)(&MenuIconBar), b[MouseB_PosX] - MenuIconBar.head.width / 2, 96 + Menu_Height*Menu_IBar_Items);
    LastMenu = Menu_IBar;
  }
  else if (b[MouseB_Buttons] == 4)
  {
    int block[WindowB_WFlags+1];

    if (vsid_mode)
    {
      ui_open_vsid_window(block);
    }
    else
    {
      RO_Window *win;
      int gainCaret = 0;

      win = (ActiveCanvas == NULL) ? EmuWindow : ActiveCanvas->window;
      if (ui_open_centered_or_raise_block(win, block) == 0)
        gainCaret = 1;
      ui_open_emu_window(win, block);
      if (gainCaret != 0)
      {
        Wimp_GetCaretPosition(&LastCaret);
        Wimp_SetCaretPosition(win->Handle, -1, -100, 100, -1, -1);
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
  }
  else if (b[MouseB_Buttons] == 1)
  {
    /* open default config window */
    ui_open_config_window((vsid_mode) ? CONF_WIN_SOUND : CONF_WIN_DRIVES);
  }
}

static int ui_mouse_click_config(int *b, int wnum)
{
  RO_Window *win = ConfWindows[wnum];
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
            RO_MenuItem *item;
            RO_MenuHead *submenu;

            item = (RO_MenuItem*)(ConfigMenus[i].menu + 1);
            submenu = item[Menu_DriveType_FS].submenu;
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
              wimp_menu_set_grey_all(item[Menu_DriveType_Disk].submenu, (j != Menu_DriveType_Disk));
              wimp_menu_tick_slct(submenu, flags);
            }
          }
          break;
        default:
          break;
      }
      LastMenu = 256 + i;
      Wimp_CreateMenu((int*)menu, wb[WindowB_VMinX] - wb[WindowB_ScrollX] + icon->maxx, wb[WindowB_VMaxY] - wb[WindowB_ScrollY] + icon->maxy);
      break;
    }
  }
  return (ConfigMenus[i].menu != NULL);
}

static int ui_mouse_click_conf_misc(int *b, int wnum)
{
  RO_Window *win = ConfWindows[wnum];

  if (b[MouseB_Window] == ConfWindows[CONF_WIN_DEVICES]->Handle)
  {
    if (b[MouseB_Icon] == Icon_ConfDev_FileRsOK)
    {
      char *fn;

      if ((fn = wimp_window_read_icon_text(win, Icon_ConfDev_FileRsPath)) != NULL)
        ui_set_serial_file(wimp_strterm(fn));
    }
    else if (b[MouseB_Icon] == Icon_ConfDev_FilePrOK)
    {
      char *fn;

      if ((fn = wimp_window_read_icon_text(win, Icon_ConfDev_FilePrPath)) != NULL)
        ui_set_printer_file(wimp_strterm(fn));
    }
  }
  else if (b[MouseB_Window] == ConfWindows[CONF_WIN_SOUND]->Handle)
  {
    if (b[MouseB_Icon] == Icon_ConfSnd_FileSndOK)
    {
      char *fn;

      if ((fn = wimp_window_read_icon_text(win, Icon_ConfSnd_FileSndPath)) != NULL)
        ui_set_sound_file(wimp_strterm(fn));
    }
    else if (b[MouseB_Icon] == Icon_ConfSnd_Volume)
    {
      ui_drag_sound_volume(b);
      Sound_Volume(SoundVolume);
    }
    else if (b[MouseB_Icon] == Icon_ConfSnd_UseResid)
    {
      resources_get_value(Rsrc_ReSid, (resource_value_t*)&CycleBasedSound);
    }
  }
  else if (b[MouseB_Window] == ConfWindows[CONF_WIN_JOY]->Handle)
  {
    Wimp_GetCaretPosition(&LastCaret);
    Wimp_SetCaretPosition(ConfWindows[CONF_WIN_JOY]->Handle, -1, -100, 100, -1, -1);
  }
  else if (b[MouseB_Window] == ConfWindows[CONF_WIN_TAPE]->Handle)
  {
    switch (b[MouseB_Icon])
    {
      case Icon_ConfTap_TapeDetach:
        ui_set_tape_image(""); break;
      case Icon_ConfTap_DataStop:
        datasette_control(DATASETTE_CONTROL_STOP); break;
      case Icon_ConfTap_DataRewind:
        datasette_control(DATASETTE_CONTROL_REWIND); break;
      case Icon_ConfTap_DataPlay:
        datasette_control(DATASETTE_CONTROL_START); break;
      case Icon_ConfTap_DataForward:
        datasette_control(DATASETTE_CONTROL_FORWARD); break;
      case Icon_ConfTap_DataRecord:
        datasette_control(DATASETTE_CONTROL_RECORD); break;
      case Icon_ConfTap_DataDoReset:
        datasette_control(DATASETTE_CONTROL_RESET); break;
      default:
        break;
    }
  }
  else
    return 0;

  return 1;
}

static int ui_mouse_click_conf_drag(int *b, int wnum)
{
  RO_Window *win = ConfWindows[wnum];
  int i = DRAG_TYPE_NONE;

  if (b[MouseB_Window] == ConfWindows[CONF_WIN_DEVICES]->Handle)
  {
    if (b[MouseB_Icon] == Icon_ConfDev_FileRsIcon)
      i = DRAG_TYPE_SERIAL;
    else if (b[MouseB_Icon] == Icon_ConfDev_FilePrIcon)
      i = DRAG_TYPE_PRINTER;
  }
  else if (b[MouseB_Window] == ConfWindows[CONF_WIN_SOUND]->Handle)
  {
    if (b[MouseB_Icon] == Icon_ConfSnd_FileSndIcon)
      i = DRAG_TYPE_SOUND;
    else if (b[MouseB_Icon] == Icon_ConfSnd_Volume)
    {
      int wstate[WindowB_WFlags+1];
      int dblk[DragB_BBMaxY+1];
      RO_Icon *icon;

      wstate[WindowB_Handle] = ConfWindows[CONF_WIN_SOUND]->Handle;
      Wimp_GetWindowState(wstate);
      icon = wimp_window_get_icon(ConfWindows[CONF_WIN_SOUND], Icon_ConfSnd_Volume);
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

    return 1;
  }
  return 0;
}


static void ui_mouse_click(int *b)
{
  if (b[MouseB_Window] == EmuPane->Handle)
  {
    ui_mouse_click_pane(b);
  }
  else if ((vsid_mode) && (b[MouseB_Window] == VSidWindow->Handle))
  {
    ui_mouse_click_vsid(b);
  }
  else
  {
    ui_mouse_click_canvas(b);
  }

  if ((b[MouseB_Window] == -2) && (b[MouseB_Icon] == IBarIcon.IconHandle))
  {
    ui_mouse_click_ibar(b);
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
  else if (b[MouseB_Window] == ImgContWindow->Handle)
  {
    ui_image_contents_click(b);
  }
  else if (b[MouseB_Window] == CreateDiscWindow->Handle)
  {
    switch (b[MouseB_Icon])
    {
      case Icon_Create_OK:
        ui_create_new_disc_image();
        break;
      case Icon_Create_Type:
        Wimp_CreateMenu((int*)(&MenuCreateDiscType), b[MouseB_PosX], b[MouseB_PosY]);
        LastMenu = Menu_CreateDisc;
        break;
      case Icon_Create_Sprite:
        LastDrag = DRAG_TYPE_CREATEDISC;
        wimp_drag_icon_sprite(CreateDiscWindow, Icon_Create_Sprite, &ScreenMode, CMOS_DragType);
        break;
      default:
        break;
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
        /* menu ==> open emulator window menu */
        if (b[MouseB_Buttons] == 2)
        {
          wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Active, (canvas_get_number() <= 1));
          wimp_menu_set_grey_item((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Screenshot, 1);
          Wimp_CreateMenu((int*)&MenuEmuWindow, b[MouseB_PosX], b[MouseB_PosY]);
          LastHandle = EmuWindow->Handle;
          LastMenu = Menu_Emulator;
        }
        /* Select and adjust only */
        else if ((b[MouseB_Buttons] == 1) || (b[MouseB_Buttons] == 4))
        {
          /* Haven't found anything? */
          if (!ui_mouse_click_config(b, wnum))
          {
            int i;

            for (i=0; Configurations[i].resource != NULL; i++)
            {
              if ((Configurations[i].id.win == wnum) && (Configurations[i].id.icon == b[MouseB_Icon]))
              {
                if (Configurations[i].ctype == CONFIG_SELECT)
                {
                  int s;

                  s = ui_set_resource_select(Configurations[i].resource, &(Configurations[i].id));
                  /* Special cases */
                  if ((Configurations[i].id.win == CONF_WIN_DRIVES) && (Configurations[i].id.icon == Icon_ConfDrv_TrueDrv))
                  {
                    ui_set_truedrv_emulation(s);
                  }
                  if (Configurations[i].id.win == CONF_WIN_SOUND)
                  {
                    if (Configurations[i].id.icon == Icon_ConfSnd_SoundOn)
                    {
                      ui_set_sound_enable(s);
                    }
                    else if (Configurations[i].id.icon == Icon_ConfSnd_Sound16Bit)
                    {
                      int sndstate;
                      resources_get_value(Rsrc_Sound, (resource_value_t*)&sndstate);
                      if (sndstate != 0)
                      {
                        /* if sound enabled and 16bit state changed, close sound device
                           and immediately reopen it to use the new system */
                        /*resources_set_value(Rsrc_Sound, (resource_value_t)0);*/
                        sound_close();
                        resources_set_value(Rsrc_Sound, (resource_value_t)1);
                      }
                    }
                  }
                }
                break;
              }
            }
            /* Haven't found anything there either? */
            if (Configurations[i].resource == NULL)
            {
              ui_mouse_click_conf_misc(b, wnum);
            }
          }
        }
        /* Drag? */
        else if ((b[MouseB_Buttons] == 16) || (b[MouseB_Buttons] == 64))
        {
          ui_mouse_click_conf_drag(b, wnum);
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

  if ((WithinUiPoll == 0) && (SoundSuspended != 0) && (EmuPaused == 0)) sound_resume();

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
      iconnum = Icon_ConfSnd_FileSndPath; win = ConfWindows[CONF_WIN_SOUND]; break;
    case DRAG_TYPE_SERIAL:
      iconnum = Icon_ConfDev_FileRsPath; win = ConfWindows[CONF_WIN_DEVICES]; break;
    case DRAG_TYPE_PRINTER:
      iconnum = Icon_ConfDev_FilePrPath; win = ConfWindows[CONF_WIN_DEVICES]; break;
    case DRAG_TYPE_SNAPSHOT:
      iconnum = Icon_Snap_Path; win = SnapshotWindow; break;
    case DRAG_TYPE_SAVEBOX:
      iconnum = Icon_Save_Path; win = SaveBox;
      switch (LastSubDrag)
      {
        case SBOX_TYPE_ROMSET:
        case SBOX_TYPE_KEYBOARD:
        case SBOX_TYPE_FLIPLIST:
          filetype = FileType_Text; break;
        case SBOX_TYPE_SCRSHOT:
          filetype = FileType_Sprite; break;
        default:
          break;
      }
      break;
    case DRAG_TYPE_CREATEDISC:
      iconnum = Icon_Create_File; win = CreateDiscWindow; break;
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

      if ((h != 0) && (canvas_for_handle(h) == NULL) && (h != EmuPane->Handle) && (h != SaveBox->Handle) && (h != ImgContWindow->Handle) && (ui_message_window_for_handle(h) == msg_win_NUMBER))
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
      const char *b;

      if (*dest != (unsigned char)code)
      {
        *dest = (unsigned char)code;
        if ((b = kbd_intkey_to_string(code)) != NULL)
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


static void ui_key_press_config(int *b)
{
  int wnum;
  int key;

  key = b[KeyPB_Key];

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
          case Icon_ConfDrv_DriveFile11: i++;
          case Icon_ConfDrv_DriveFile10: i++;
          case Icon_ConfDrv_DriveFile9: i++;
          case Icon_ConfDrv_DriveFile8:
            ui_new_drive_image(i, data, 0);
            break;
          default: Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_TAPE:
        if (b[KeyPB_Icon] == Icon_ConfTap_TapeFile)
        {
          ui_set_tape_image(data);
        }
        else
        {
          Wimp_ProcessKey(key);
        }
        break;
      case CONF_WIN_SYSTEM:
        switch (b[KeyPB_Icon])
        {
          case Icon_ConfSys_PollEvery:
            resources_set_value(Rsrc_Poll, (resource_value_t)atoi(data)); break;
          case Icon_ConfSys_SpeedEvery:
            resources_set_value(Rsrc_Speed, (resource_value_t)atoi(data)); break;
          case Icon_ConfSys_SoundEvery:
            resources_set_value(Rsrc_SndEvery, (resource_value_t)atoi(data)); break;
          case Icon_ConfSys_CharGen:
            resources_set_value(Rsrc_CharGen, (resource_value_t)data); break;
          case Icon_ConfSys_Kernal:
            resources_set_value(Rsrc_Kernal, (resource_value_t)data); break;
          case Icon_ConfSys_Basic:
            resources_set_value(Rsrc_Basic, (resource_value_t)data); break;
          case Icon_ConfSys_CartFile:
            ui_set_cartridge_file(data); break;
          case Icon_ConfSys_DosName:
            ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_DOSNAME], (resource_value_t)data);
            break;
          default: Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_VIDEO:
        switch (b[KeyPB_Icon])
        {
          case Icon_ConfVid_MaxSkipFrms:
            resources_set_value(Rsrc_MaxSkipped, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_FullScrNorm:
            resources_set_value(Rsrc_FullScrNorm, (resource_value_t)data); break;
          case Icon_ConfVid_FullScrPAL:
            resources_set_value(Rsrc_FullScrPal, (resource_value_t)data); break;
          case Icon_ConfVid_FullScrDbl:
            resources_set_value(Rsrc_FullScrPal2, (resource_value_t)data); break;
          case Icon_ConfVid_ColourSat:
            resources_set_value(Rsrc_ColourSat, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_Contrast:
            resources_set_value(Rsrc_Contrast, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_Brightness:
            resources_set_value(Rsrc_Brightness, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_Gamma:
            resources_set_value(Rsrc_Gamma, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_LineShade:
            resources_set_value(Rsrc_LineShade, (resource_value_t)atoi(data)); break;
          case Icon_ConfVid_Palette:
            resources_set_value(Rsrc_Palette, (resource_value_t)data); break;
          default:
            Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_DEVICES:
        switch (b[KeyPB_Icon])
        {
          case Icon_ConfDev_FileRsPath:
            ui_set_serial_file(data); break;
          case Icon_ConfDev_FilePrPath:
            ui_set_printer_file(data); break;
          default:
            Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_SOUND:
        switch (b[KeyPB_Icon])
        {
          case Icon_ConfSnd_FileSndPath:
            ui_set_sound_file(data); break;
          case Icon_ConfSnd_ResidPass:
            resources_set_value(Rsrc_ReSidPass, (resource_value_t)atoi(data)); break;
          default:
            Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_VIC:
        if (b[KeyPB_Icon] == Icon_ConfVIC_VICCartF)
          ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_VICCART], (resource_value_t)data);
        else
        {
          Wimp_ProcessKey(key); return;
        }
        break;
      case CONF_WIN_CBM2:
        if (b[KeyPB_Icon] == Icon_ConfCBM_CBM2CartF)
        {
          ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_C2CART], (resource_value_t)data);
        }
        else
        {
          Wimp_ProcessKey(key);
        }
        break;
      case CONF_WIN_C128:
        if (b[KeyPB_Icon] == Icon_Conf128_C128Palette)
        {
          resources_set_value(Rsrc_VDCpalette, (resource_value_t)data);
        }
        else
        {
          Wimp_ProcessKey(key);
        }
        break;
      default: Wimp_ProcessKey(key); return;
    }
  }
}

static void ui_key_press(int *b)
{
  int key;

  key = b[KeyPB_Key];

  if (canvas_for_handle(b[KeyPB_Window]) != NULL)
  {
    switch (key)
    {
      case 0x189:	/* F9 */
        ShowEmuPane ^= 1;
        ui_set_pane_state(ShowEmuPane);
        break;
      case 0x1ca:	/* F10 */
        canvas_next_active(1);
        break;
      case 0x18b:	/* Copy */
        EmuPaused ^= 1;
        ui_display_paused(EmuPaused);
        break;
      case 0x180:	/* Print */
        ui_make_last_screenshot();
        break;
      case 0x1a9:	/* ^F9 */
        ui_save_last_snapshot();
        break;
      case 0x1ea:	/* ^F10 */
        ui_trigger_snapshot_load();
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
  else if (b[KeyPB_Window] == VSidWindow->Handle)
  {
    vsid_ui_key_press(b);
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
    else if (b[KeyPB_Window] == SnapshotWindow->Handle)
    {
      char *snapname;

      if ((snapname = wimp_window_read_icon_text(SnapshotWindow, Icon_Snap_Path)) != NULL)
        ui_check_save_snapshot(snapname);
    }
    else if (b[KeyPB_Window] == CreateDiscWindow->Handle)
    {
      ui_create_new_disc_image();
    }

    ui_key_press_config(b);

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


static config_item_t PETdependconf[] = {
  {Rsrc_PetCrt, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetCrt}},
  {Rsrc_PetRAM9, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetRAM9}},
  {Rsrc_PetRAMA, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetRAMA}},
  {Rsrc_PetDiag, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetDiagPin}},
  {Rsrc_PetSuper, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetSuper}},
  {NULL, 0, {0, 0}}
};

static config_item_t SystemROMconf[] = {
  {Rsrc_CharGen, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_CharGen}},
  {Rsrc_Kernal, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_Kernal}},
  {Rsrc_Basic, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_Basic}},
  {Rsrc_Palette, CONFIG_STRING, {CONF_WIN_VIDEO, Icon_ConfVid_Palette}},
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


static void ui_images_menu_selection(int *b)
{
  if (b[0] == 0)
  {
    ui_detach_drive_image(FlipListDrive);
  }
  else
  {
    FlipListIter = b[0] - 1;
    ui_flip_attach_image_no(FlipListIter);
  }
}


static int ui_menu_select_ibar(int *b, int **menu)
{
  int confWindow = -1;

  *menu = (int*)&MenuIconBar;
  switch (b[0])
  {
    int cols;

    case Menu_IBar_License:
      ui_message_get_dimensions(info_license_text, &cols, NULL);
      ui_message_window_open(msg_win_license, SymbolStrings[Symbol_TitLicense], info_license_text, cols, 0);
      break;
    case Menu_IBar_Warranty:
      ui_message_get_dimensions(info_warranty_text, &cols, NULL);
      ui_message_window_open(msg_win_warranty, SymbolStrings[Symbol_TitWarranty], info_warranty_text, cols, 0);
      break;
    case Menu_IBar_Contrib:
      ui_message_get_dimensions(info_contrib_text, &cols, NULL);
      ui_message_window_open(msg_win_contrib, SymbolStrings[Symbol_TitContrib], info_contrib_text, cols, 0);
      break;
    case Menu_IBar_LogWin:
      ui_open_log_window();
      break;
    case Menu_IBar_CreateDisc:
      {
        int block[WindowB_WFlags+1];
        int status;

        status = ui_open_centered_or_raise_block(CreateDiscWindow, block);
        Wimp_OpenWindow(block);
        if (status == 0)
        {
          Wimp_GetCaretPosition(&LastCaret);
          Wimp_SetCaretPosition(CreateDiscWindow->Handle, -1, -100, 100, -1, -1);
        }
      }
      break;
    case Menu_IBar_Configure:
      if (b[1] != -1) confWindow = CONF_WIN_NUMBER;
      break;
    case Menu_IBar_FullScreen:
      {
        if ((!vsid_mode) && (video_full_screen_on(SpriteArea) != 0))
        {
          _kernel_oserror err;

          err.errnum = 0; strcpy(err.errmess, SymbolStrings[Symbol_ErrFullScr]);
          Wimp_ReportError(&err, 1, WimpTaskName);
        }
      }
      break;
    case Menu_IBar_Quit: ui_exit(); break;
    default:
      break;
  }
  return confWindow;
}

static int ui_menu_select_emuwin(int *b, int **menu)
{
  int confWindow = -1;

  *menu = (int*)&MenuEmuWindow;
  switch (b[0])
  {
    case Menu_EmuWin_Configure:
      if (b[1] != -1) confWindow = CONF_WIN_NUMBER;
      break;
    case Menu_EmuWin_Fliplist:
      switch (b[1])
      {
        case Menu_Fliplist_Attach:
          flip_add_image(FlipListDrive + 8);
          ui_build_fliplist_menu(1);
          break;
        case Menu_Fliplist_Detach:
          if (DriveFile8 != NULL)
          {
            flip_remove(FlipListDrive + 8, DriveFile8);
            ui_build_fliplist_menu(1);
            /* don't tick an image (without attaching it) */
            wimp_menu_tick_all(MenuFlipImages, 0);
            FlipListIter = 0;
          }
          break;
        case Menu_Fliplist_Next:
          ui_flip_iterate_and_attach(+1);
          break;
        case Menu_Fliplist_Prev:
          ui_flip_iterate_and_attach(-1);
          break;
        case Menu_Fliplist_Clear:
          flip_clear_list(FlipListDrive + 8);
          ui_build_fliplist_menu(1);
          break;
        case Menu_Fliplist_Images:
          if (b[2] >= 0)
          {
            ui_images_menu_selection(b+2);
          }
          break;
        default:
          break;
      }
      break;
    case Menu_EmuWin_Freeze:
      cartridge_trigger_freeze();
      break;
    case Menu_EmuWin_Pane:
      ShowEmuPane ^= 1;
      ui_set_pane_state(ShowEmuPane);
      break;
    case Menu_EmuWin_Active:
      canvas_next_active(0);
      break;
    case Menu_EmuWin_TrueDrvEmu:
      ui_set_truedrv_emulation(!wimp_menu_tick_read((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_TrueDrvEmu));
      break;
    case Menu_EmuWin_Datasette:
      switch (b[1])
      {
        case Menu_Datasette_Stop: datasette_control(DATASETTE_CONTROL_STOP); break;
        case Menu_Datasette_Start: datasette_control(DATASETTE_CONTROL_START); break;
        case Menu_Datasette_Forward: datasette_control(DATASETTE_CONTROL_FORWARD); break;
        case Menu_Datasette_Rewind: datasette_control(DATASETTE_CONTROL_REWIND); break;
        case Menu_Datasette_Record: datasette_control(DATASETTE_CONTROL_RECORD); break;
        case Menu_Datasette_Reset: datasette_control(DATASETTE_CONTROL_RESET); break;
        default: break;
      }
      break;
    case Menu_EmuWin_Sound:
      ui_set_sound_enable(!wimp_menu_tick_read((RO_MenuHead*)&MenuEmuWindow, Menu_EmuWin_Sound));
      break;
    case Menu_EmuWin_Monitor:
      ui_activate_monitor();
      break;
    default:
      break;
  }
  return confWindow;
}

static int ui_menu_select_config(int *b, int **menu, int mnum)
{
  *menu = (int*)(ConfigMenus[mnum].menu);

  /* must execute cartridge detach before calling ui_set_menu_display_value() */
  if ((mnum == CONF_MENU_CARTTYPE) && (b[0] == 0) && !vsid_mode)
    cartridge_detach_image();

  if (ConfigDispDescs[mnum] != NULL)
    ui_set_menu_display_value(ConfigDispDescs[mnum], b[0]);

  switch (mnum)
  {
    case CONF_MENU_DRIVE8:
    case CONF_MENU_DRIVE9:
    case CONF_MENU_DRIVE10:
    case CONF_MENU_DRIVE11:
      {
        int number = mnum - CONF_MENU_DRIVE8;
        RO_MenuHead *dmen = ConfigMenus[CONF_MENU_DRIVE8 + number].menu;

        if (b[0] == Menu_DriveType_Disk)
        {
          if (b[1] != -1)
          {
            if (b[1] == Menu_DriveDisk_Detach)
            {
              ui_detach_drive_image(number);
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
                  RO_MenuItem *item = (RO_MenuItem*)(ConfigMenus[mnum].menu + 1);
                  wimp_menu_set_grey_all(item[Menu_DriveType_Disk].submenu, 0);
                  wimp_menu_set_grey_all(item[Menu_DriveType_FS].submenu, 1);
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
                  RO_MenuItem *item = (RO_MenuItem*)(ConfigMenus[mnum].menu + 1);
                  wimp_menu_set_grey_all(item[Menu_DriveType_Disk].submenu, 1);
                  wimp_menu_set_grey_all(item[Menu_DriveType_FS].submenu, 0);
                  wimp_menu_tick_exclusive((RO_MenuHead*)menu, Menu_DriveType_FS);
                }
              }
            }
            /* Change in submenus? */
          }
          else
          {
            RO_MenuItem *item;
            RO_MenuHead *submenu;

            item = (RO_MenuItem*)(dmen + 1);
            submenu = item[Menu_DriveType_FS].submenu;
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
    case CONF_MENU_PETMODEL:
      {
        int i;

        ui_set_menu_display_core(ConfigDispDescs[CONF_MENU_PETMODEL], set_pet_model_by_name, b[0]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_PETMEM]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_PETIO]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_PETVIDEO]);
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_PET], Icon_ConfPET_PetKbd, pet_get_keyboard_name());
        ui_update_rom_names();
        for (i=0; PETdependconf[i].resource != NULL; i++)
        {
          ui_setup_config_item(PETdependconf + i);
        }
      }
      break;
    case CONF_MENU_C2MODEL:
      {
        ui_set_menu_display_core(ConfigDispDescs[CONF_MENU_C2MODEL], set_cbm2_model_by_name, b[0]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_C2MEM]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_C2RAM]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_C2LINE]);
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_ConfCBM_CBM2Kbd, cbm2_get_keyboard_name());
        ui_update_rom_names();
      }
      break;
    case CONF_MENU_ROMSET:
      if (MenuDisplayROMSet != NULL)
      {
        ui_set_menu_display_core(ConfigDispDescs[CONF_MENU_ROMSET], set_romset_by_name, b[0]);
        ui_setup_menu_display(ConfigDispDescs[CONF_MENU_DOSNAME]);
        ui_update_rom_names();
        /*ui_issue_reset(1);*/
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
              ui_setup_menu_display(ConfigDispDescs[CONF_MENU_ROMSET]);
            }
          }
          break;
        case Menu_RomAct_Delete:
          romset_delete_item(ROMSetName);
          ui_build_romset_menu();
          ui_setup_menu_display(ConfigDispDescs[CONF_MENU_ROMSET]);
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
          ui_setup_menu_display(ConfigDispDescs[CONF_MENU_ROMSET]);
          break;
        case Menu_RomAct_Restore:
          romset_clear_archive();
          if (ROMSetArchiveFile != NULL)
          {
            romset_load_archive(ROMSetArchiveFile, 0);
          }
          ui_build_romset_menu();
          ui_setup_menu_display(ConfigDispDescs[CONF_MENU_ROMSET]);
          break;
        default: break;
      }
      break;
    case CONF_MENU_SYSKBD:
      switch (b[0])
      {
        case Menu_SysKbd_LoadDef:
          kbd_load_keymap(NULL, -1);
          break;
        default:
          break;
      }
      break;
    case CONF_MENU_SPEED:
      vsync_resync_speed();
      break;
    default:
      return 0;
  }
  return 1;
}


static void ui_menu_selection(int *b)
{
  int block[MouseB_Icon+1];
  int *menu = NULL;

  Wimp_GetPointerInfo(block);

  if ((LastMenu == Menu_IBar) || (LastMenu == Menu_Emulator) || (LastMenu == Menu_Images) || (LastMenu == Menu_CreateDisc))
  {
    int confWindow = -1;

    if (LastMenu == Menu_IBar)
    {
      confWindow = ui_menu_select_ibar(b, &menu);
    }
    else if (LastMenu == Menu_Emulator)
    {
      confWindow = ui_menu_select_emuwin(b, &menu);
    }
    else if (LastMenu == Menu_Images)
    {
      menu = (MenuFlipImages == NULL) ? (int*)&MenuFlipImageTmpl : (int*)MenuFlipImages;
      ui_images_menu_selection(b);
    }
    else if (LastMenu == Menu_CreateDisc)
    {
      menu = (int*)&MenuCreateDiscType;
      ui_set_create_image_type(b[0]);
    }

    if (confWindow >= 0)
    {
      confWindow = -1;
      switch (b[1])
      {
        case Menu_Config_Drives: confWindow = CONF_WIN_DRIVES; break;
        case Menu_Config_Tape: confWindow = CONF_WIN_TAPE; break;
        case Menu_Config_Devices: confWindow = CONF_WIN_DEVICES; break;
        case Menu_Config_Sound: confWindow = CONF_WIN_SOUND; break;
        case Menu_Config_System: confWindow = CONF_WIN_SYSTEM; break;
        case Menu_Config_Video: confWindow = CONF_WIN_VIDEO; break;
        case Menu_Config_Joystick: confWindow = CONF_WIN_JOY; break;
        case Menu_Config_Machine:
          if (machine_class == VICE_MACHINE_PET)
            confWindow = CONF_WIN_PET;
          else if (machine_class == VICE_MACHINE_VIC20)
            confWindow = CONF_WIN_VIC;
          else if (machine_class == VICE_MACHINE_CBM2)
            confWindow = CONF_WIN_CBM2;
          else if (machine_class == VICE_MACHINE_C128)
            confWindow = CONF_WIN_C128;
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
    ui_menu_select_config(b, &menu, LastMenu - 0x100);
  }

  /* Selection with adjust ==> re-open menu */
  if ((block[MouseB_Buttons] == 1) && (menu != NULL))
  {
    Wimp_CreateMenu(menu, block[MouseB_PosX], block[MouseB_PosY]);
  }
}


static void ui_load_snapshot_trap(ADDRESS unused_address, void *unused_data)
{
  int status;

  vsync_suspend_speed_eval();

  /* See true drive emulation */
  ui_temp_suspend_sound();

  /*log_message(LOG_DEFAULT, "Load snapshot %s", ((char*)SnapshotMessage)+44);*/

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
    wimp_window_write_icon_text(SnapshotWindow, Icon_Snap_Path, ((char*)SnapshotMessage)+44);
  }
  else
  {
    _kernel_oserror err;

    err.errnum = 0; strcpy(err.errmess, SymbolStrings[Symbol_ErrSnapR]);
    Wimp_ReportError(&err, 1, WimpTaskName);
    ui_issue_reset(1);
  }

  ui_temp_resume_sound();
}


void ui_trigger_snapshot_load(void)
{
  maincpu_trigger_trap(ui_load_snapshot_trap, NULL);
  SnapshotPending = 1;
}


static int ui_make_screenshot(const char *name)
{
  struct video_canvas_s *canvas = canvas_for_handle(LastHandle);
  if ((canvas != NULL) && (screenshot_canvas_save("Sprite", name, canvas) == 0))
  {
    SetFileType(name, FileType_Sprite);
    return 0;
  }
  return -1;
}


int ui_make_last_screenshot(void)
{
  if (wimp_check_for_path(ViceScreenshotFile) == 0)
    return ui_make_screenshot(ViceScreenshotFile);

  return -1;
}


static int ui_load_prg_file(const char *name)
{
  /* if shift is held down, then load only, otherwise autostart */
  if (ScanKeys(IntKey_Shift) == 0xff)
  {
    if (autostart_prg(name, AUTOSTART_MODE_RUN) == 0)
    {
      const char *b, *pend;
      char buffer[256];

      b = name; pend = b;
      while (*b != '\0')
      {
        if (*b == FSDEV_DIR_SEP_CHR) pend = b;
        b++;
      }
      memcpy(buffer, name, (pend-name));
      buffer[pend-name] = '\0';
      ui_display_drive_dir(0, buffer);

      return 0;
    }
    return -1;
  }
  else
  {
    if (machine_class == VICE_MACHINE_C64)
    {
      FILE *fp;

      vsync_suspend_speed_eval();

      if ((fp = fopen(name, "rb")) != NULL)
      {
        BYTE lo, hi;
        int length;

        lo = fgetc(fp); hi = fgetc(fp); length = lo + (hi << 8);
        length += fread(mem_ram + length, 1, C64_RAM_SIZE - length, fp);
        fclose(fp);
        mem_ram[0xc3] = lo; mem_ram[0xc4] = hi;
        lo = length & 0xff; hi = (length >> 8) & 0xff;
        mem_ram[0xae] = lo; mem_ram[0x2d] = lo; mem_ram[0x2f] = lo; mem_ram[0x31] = lo; mem_ram[0x33] = lo;
        mem_ram[0xaf] = hi; mem_ram[0x2e] = hi; mem_ram[0x30] = hi; mem_ram[0x32] = hi; mem_ram[0x34] = hi;
        return 0;
      }
    }
  }

  return -1;
}


static const char *ui_get_file_extension(const char *name)
{
  const char *d, *ext;

  d = name; ext = NULL;
  while (*d > ' ')
  {
    if (*d =='/') ext = d+1;
    d++;
  }

  return ext;
}


static void ui_user_msg_mode_change(int *b)
{
  canvas_list_t *clist = CanvasList;
  int block[WindowB_WFlags+1];
  RO_Window *win;

  vsync_suspend_speed_eval();

  wimp_read_screen_mode(&ScreenMode);
  /* Extremely annoying mode change code */
  /* Change in eigen factors might make this necessary */
  while (clist != NULL)
  {
    video_canvas_update_extent(clist->canvas);
    win = clist->canvas->window;
    clist = clist->next;

    block[WindowB_Handle] = win->Handle;
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
}

static void ui_user_msg_data_load(int *b)
{
  struct video_canvas_s *canvas;
  int i;
  int action=0;
  char *name = ((char*)b)+44;

  canvas = canvas_for_handle(b[5]);
  if (canvas != NULL)
  {
    int doprg = 0;

    if (b[10] == FileType_C64File)
      doprg = 1;
    else
    {
      const char *ext = ui_get_file_extension(name);

      if ((ext != NULL) && (wimp_strcasecmp(ext, PRGFileExtension) == 0))
        doprg = 1;
    }
    if (doprg)
    {
      if (ui_load_prg_file(name) == 0)
        action = 1;
    }
    else if (b[10] == FileType_Data)	/* Snapshot? */
    {
      wimp_strcpy(((char*)SnapshotMessage)+44, name);
      ui_trigger_snapshot_load();
      action = 1;
    }
  }
  else if (b[5] == EmuPane->Handle)
  {
    if (b[10] == FileType_Text)
    {
      /* Fliplist file? */
      if (flip_load_list(FlipListDrive + 8, name, 0) == 0)
      {
        ui_build_fliplist_menu(1);
        ui_flip_attach_image_no(0);
        action = 1;
      }
    }
    else
    {
      /* Otherwise disk image */
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
        if (ui_new_drive_image(i, name, 1) == 0)
          action = 1;
      }
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
      if (ui_new_drive_image(i, name, 1) == 0)
        action = 1;
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_TAPE]->Handle)
  {
    if (b[6] == Icon_ConfTap_TapeFile)
    {
      if (ui_new_tape_image(name, 1) == 0)
        action = 1;
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_SYSTEM]->Handle)
  {
    if (b[6] == Icon_ConfSys_CartFile)
    {
      ui_set_cartridge_file(name);
      action = 1;
    }
    else if (b[6] == Icon_ConfSys_DosNameF)
    {
      ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_DOSNAME], (resource_value_t)ui_check_for_syspath(name));
      action = 1;
    }
    if ((b[10] == FileType_Data) || (b[10] == FileType_Text))
    {
      const char *res = NULL;
      int rom_changed = 0;

      if (b[10] == FileType_Data)
      {
        if (b[6] == Icon_ConfSys_CharGen) res = Rsrc_CharGen;
        else if (b[6] == Icon_ConfSys_Kernal) res = Rsrc_Kernal;
        else if (b[6] == Icon_ConfSys_Basic) res = Rsrc_Basic;
        if (res != NULL) rom_changed = 1;
      }
      else if (b[10] == FileType_Text)
      {
        if ((b[6] == Icon_ConfSys_Keyboard) || (b[6] == Icon_ConfSys_KeyboardT))
        {
          kbd_load_keymap(name, -1);
          action = 1;
        }
        /* Check extension */
        else
        {
          const char *ext;

          if ((ext = ui_get_file_extension(name)) != NULL)
          {
            if (wimp_strcasecmp(ext, RSETARCH_EXT) == 0)
            {
              romset_load_archive(name, 0);
              ui_build_romset_menu();
              action = 1;
            }
            else if (wimp_strcasecmp(ext, KEYMAP_EXT) == 0)
            {
              kbd_load_keymap(name, -1);
              action = 1;
            }
          }
        }
      }
      if (res != NULL)
      {
        const char *filename;

        filename = ui_check_for_syspath(name);
        if (resources_set_value(res, (resource_value_t)filename) == 0)
        {
          wimp_window_write_icon_text(ConfWindows[CONF_WIN_SYSTEM], b[6], filename);
          if (rom_changed != 0)
          {
            mem_load(); /*ui_issue_reset(1);*/
          }
          action = 1;
        }
      }
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_VIDEO]->Handle)
  {
    if (b[10] == FileType_Text)
    {
      if (b[6] == Icon_ConfVid_Palette)
      {
        const char *filename;

        filename = ui_check_for_syspath(name);
        if (resources_set_value(Rsrc_Palette, (resource_value_t)filename) == 0)
        {
          wimp_window_write_icon_text(ConfWindows[CONF_WIN_VIDEO], b[6], filename);
          action = 1;
        }
      }
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_VIC]->Handle)
  {
    if (b[6] == Icon_ConfVIC_VICCartF)
    {
      ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_VICCART], (resource_value_t)name);
      action = 1;
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_CBM2]->Handle)
  {
    if (b[6] == Icon_ConfCBM_CBM2CartF)
    {
      ui_update_menu_disp_strshow(ConfigDispDescs[CONF_MENU_C2CART], (resource_value_t)name);
      action = 1;
    }
  }
  else if (b[5] == ConfWindows[CONF_WIN_C128]->Handle)
  {
    if ((b[6] == Icon_Conf128_C128Palette) || (b[6] == Icon_Conf128_C128z80bios))
    {
      const char *filename, *rsrc;

      if (b[6] == Icon_Conf128_C128Palette)
        rsrc = Rsrc_VDCpalette;
      else
        rsrc = Rsrc_Z80Bios;

      filename = ui_check_for_syspath(name);
      if (resources_set_value(rsrc, (resource_value_t)filename) == 0)
      {
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_C128], b[6], filename);
      }
      action = 1;
    }
  }
  else if (b[5] == VSidWindow->Handle)
  {
    if (vsid_ui_load_file(name) == 0)
      action = 1;
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
}

static void ui_user_msg_data_save(int *b)
{
  struct video_canvas_s *canvas;
  int action=0;
  char *name = ((char*)b)+44;

  canvas = canvas_for_handle(b[5]);

  if (canvas != NULL)
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


static void ui_user_msg_data_save_ack(int *b)
{
  char *name = ((char*)b) + 44;
  int status;

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
        wimp_strcpy(((char*)SnapshotMessage)+44, name);
        b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoad;
        Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
      }
      break;

    case DRAG_TYPE_SAVEBOX:
      status = -1;
      switch (LastSubDrag)
      {
        case SBOX_TYPE_ROMSET:
          if ((ROMSetName != NULL) && (romset_save_item(name, ROMSetName) == 0))
          {
            wimp_strcpy(ROMSetItemFile, name);
            status = 0;
          }
          break;
        case SBOX_TYPE_KEYBOARD:
          if (kbd_dump_keymap(name, -1) == 0)
          {
            wimp_strcpy(SystemKeymapFile, name);
            status = 0;
          }
          break;
        case SBOX_TYPE_SCRSHOT:
          {
            if (ui_make_screenshot(name) == 0)
            {
              wimp_strcpy(ViceScreenshotFile, name);
              status = 0;
            }
          }
          break;
        case SBOX_TYPE_FLIPLIST:
          if (flip_save_list(FlipListDrive + 8, name) == 0)
          {
            wimp_strcpy(ViceFliplistFile, name);
            status = 0;
          }
          break;
        default:
          break;
      }
      if (status == 0)
      {
        SetFileType(name, b[10]);
        b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoad;
        Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
        Wimp_CreateMenu((int*)-1, 0, 0);
      }
      break;

    case DRAG_TYPE_CREATEDISC:
      wimp_window_write_icon_text(CreateDiscWindow, Icon_Create_File, name);
      if (ui_create_new_disc_image() == 0)
      {
        b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoad;
        Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
      }
      break;
    default: break;
  }
  LastDrag = DRAG_TYPE_NONE;
}


static void ui_user_msg_data_open(int *b)
{
  if ((vsid_mode) && (b[10] == FileType_SIDMusic))
  {
    if (vsid_ui_load_file(((char*)b)+44) == 0)
    {
      b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_DataLoadAck;
      Wimp_SendMessage(18, b, b[MsgB_Sender], b[6]);
    }
  }
}


static void ui_user_msg_help_request(int *b)
{
  const char *msg = ui_get_help_for_window_icon(b[8], b[9]);

  if (msg != NULL)
  {
    unsigned int len;

    len = strlen(msg);
    if (len >= (256-20))
      len = 256-21;

    strncpy(((char*)b)+20, msg, len+1);
    /*log_message(LOG_DEFAULT, "Help (%d): %s", len, ((char*)b)+20);*/
    b[0] = (20 + len + 4) & ~3;
    b[MsgB_YourRef] = b[MsgB_MyRef]; b[MsgB_Action] = Message_HelpReply;
    Wimp_SendMessage(17, b, b[MsgB_Sender], b[6]);
  }
}


static void ui_user_message(int *b)
{
  switch (b[MsgB_Action])
  {
    case Message_Quit: ui_exit(); break;
    case Message_ModeChange:
      ui_user_msg_mode_change(b);
      canvas_mode_change();
      break;
    case Message_PaletteChange:
      wimp_read_screen_mode(&ScreenMode);
      FrameBufferUpdate = 1;
      ModeChanging = 1;
      raster_mode_change();
      canvas_mode_change();
      ModeChanging = 0;
      break;
    case Message_DataLoad:
      ui_user_msg_data_load(b);
      break;
    case Message_DataSave:
      ui_user_msg_data_save(b);
      break;
    case Message_DataSaveAck:
      ui_user_msg_data_save_ack(b);
      break;
    case Message_DataOpen:
      ui_user_msg_data_open(b);
      break;
    case Message_HelpRequest:
      ui_user_msg_help_request(b);
      break;
    case Message_MenuWarning:
      if (LastMenu == CONF_MENU_ROMACT + 0x100)
      {
        wimp_window_write_icon_text(SaveBox, Icon_Save_Path, ROMSetItemFile);
        wimp_set_icon_sprite_file(SaveBox, Icon_Save_Sprite, FileType_Text);
        LastSubDrag = SBOX_TYPE_ROMSET;
      }
      else if (LastMenu == CONF_MENU_SYSKBD + 0x100)
      {
        wimp_window_write_icon_text(SaveBox, Icon_Save_Path, SystemKeymapFile);
        wimp_set_icon_sprite_file(SaveBox, Icon_Save_Sprite, FileType_Text);
        LastSubDrag = SBOX_TYPE_KEYBOARD;
      }
      else if (LastMenu == Menu_Emulator)
      {
        if (b[8] == Menu_EmuWin_Screenshot)
        {
          wimp_window_write_icon_text(SaveBox, Icon_Save_Path, ViceScreenshotFile);
          wimp_set_icon_sprite_file(SaveBox, Icon_Save_Sprite, FileType_Sprite);
          LastSubDrag = SBOX_TYPE_SCRSHOT;
        }
        else if (b[8] == Menu_EmuWin_Fliplist)
        {
          wimp_window_write_icon_text(SaveBox, Icon_Save_Path, ViceFliplistFile);
          wimp_set_icon_sprite_file(SaveBox, Icon_Save_Sprite, FileType_Text);
          LastSubDrag = SBOX_TYPE_FLIPLIST;
        }
      }
      Wimp_CreateSubMenu((int*)(b[5]), b[6], b[7]);
      break;
    default: break;
  }
}


static void ui_user_message_ack(int *b)
{
  _kernel_oserror err;
  char *name = ((char*)b) + 44;

  err.errnum = 0; err.errmess[0] = 0;

  /* Data save bounced? */
  if (b[MsgB_Action] == Message_DataSave)
  {
    sprintf(err.errmess, SymbolStrings[Symbol_ErrSave], name);
  }
  if (b[MsgB_Action] == Message_DataLoad)
  {
    sprintf(err.errmess, SymbolStrings[Symbol_ErrLoad], name);
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
    if ((LastDrag == DRAG_TYPE_VOLUME) && (mpos[MouseB_Icon] == Icon_ConfSnd_Volume))
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
int ui_poll_core(int *block)
{
  int event;

  if ((EmuPaused == 0) || (LastDrag == DRAG_TYPE_VOLUME) || (JoystickWindowOpen != 0) || ui_message_window_is_busy(msg_win_monitor) || ui_message_need_null_event())
    PollMask &= ~1;
  else
    PollMask |= 1;

  ui_poll_prologue();
  event = Wimp_Poll(PollMask, block, NULL);

  if (ui_message_process_event(event, block) != 0)
    return event;

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

void ui_poll(int dopoll)
{
  int OldFullScreen = FullScreenMode;

  /* Just to be on the save side: snapshot unpauses for at most 1 frame! */
  SnapshotPending = 0;
  /* Was sound suspended while in this function? */
  SoundSuspended = 0;

  /* Must poll in every vblank! */
  kbd_poll();

  if (SingleTasking != 0) return;

  /* always pull when switching from full screen mode back to the WIMP */
  if ((OldFullScreen != 0) && (FullScreenMode == 0))
  {
    /* force poll and tell vsync about it */
    dopoll = 1;
    vsync_resync_poll();
  }

  if (dopoll != 0)
  {
    int event;

    WithinUiPoll++;	/* Allow for nested calls */

    do
    {
      event = ui_poll_core(WimpBlock);
      if ((SnapshotPending != 0) || (SingleTasking != 0)) break;
    }
    /* A pending snapshot must unpause the emulator for a little */
    while ((EmuPaused != 0) || (event != WimpEvt_Null));

    if (--WithinUiPoll == 0)
    {
      if ((SoundSuspended != 0) && (EmuPaused == 0)) sound_resume();
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

  vsync_suspend_speed_eval();

  va_start(ap, format);

  vsprintf(str, format, ap);

  /* Display a simple errorbox in single tasking (especially full screen mode) */
  if (SingleTasking != 0)
  {
    _kernel_oserror err;

    err.errnum = 0; strncpy(err.errmess, str, 256);
    button = Wimp_ReportError(&err, 3, WimpTaskName);

    video_full_screen_refresh();
    SetMousePointer(0);
    return (button == 1) ? UI_JAM_RESET : UI_JAM_HARD_RESET;
  }

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
              button = block[MouseB_Icon]; break;
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
    case Icon_Jam_ResetH: ui_issue_reset(0); return UI_JAM_HARD_RESET;
    case Icon_Jam_Monitor: return UI_JAM_MONITOR;
    default: break;
  }
  ui_issue_reset(0);
  return UI_JAM_RESET;
}


void ui_error(const char *format, ...)
{
  char str[1024];
  va_list ap;
  _kernel_oserror err;

  vsync_suspend_speed_eval();

  va_start(ap, format);

  vsprintf(str, format, ap);

  err.errnum = 0; strncpy(err.errmess, str, 256);
  log_error(roui_log, "%s\n", str);

  Wimp_ReportError(&err, 1, WimpTaskName);

  video_full_screen_refresh();
}


void ui_message(const char *format, ...)
{
  char str[1024];
  va_list ap;
  _kernel_oserror err;

  vsync_suspend_speed_eval();

  va_start(ap, format);

  vsprintf(str, format, ap);

  err.errnum = 0; strncpy(err.errmess, str, 256);
  log_message(roui_log, "%s\n", str);

  Wimp_ReportError(&err, 17, WimpTaskName);

  video_full_screen_refresh();
}


void ui_show_text(const char *title, const char *text, int width, int height)
{
  vsync_suspend_speed_eval();

  Wimp_CommandWindow((int)title);
  printf("%s", text);
  Wimp_CommandWindow(0);	/* Wait for space or click */

  video_full_screen_refresh();
}


void ui_exit(void)
{
  RegularProgramExit = 1;

  /* for some reason VSID won't shut down properly */
  if (vsid_mode)
    sound_close();

  machine_shutdown();
  sound_close();
  ui_image_contents_exit();
  ui_message_exit();
  log_message(roui_log, SymbolStrings[Symbol_MachDown]); log_message(roui_log, "\n");
  wimp_icon_delete(&IBarIcon);
  Wimp_CloseDown(TaskHandle, TASK_WORD);
  exit(0);
}


void ui_display_speed(int percent, int framerate, int warp_flag)
{
  if (FullScreenMode == 0)
  {
    if (vsid_mode == 0)
    {
      char buffer[32];

      if (DisplayFPS == 0)
        sprintf(buffer, SymbolStrings[Symbol_PaneSpd], percent);
      else
        sprintf(buffer, SymbolStrings[Symbol_PaneFPS], framerate);

      wimp_window_write_icon_text_u(EmuPane, Icon_Pane_Speed, buffer);
    }
    else
    {
      vsid_ui_display_speed(percent);
    }
  }
  else
  {
    video_full_screen_speed(percent, framerate, warp_flag);
  }
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


void ui_display_drive_track_int(int drive_number, int track_number)
{
  RO_Icon *icon;
  int b[11];

  if (drive_number >= 2) return;

  DriveTrackNumbers[drive_number] = track_number;

  if (drive_number != DisplayDriveTrack) return;

  if ((icon = wimp_window_get_icon(EmuPane, Icon_Pane_TrkSec)) == NULL) return;
  sprintf((char*)b, "%d:%d.%d", DisplayDriveTrack+8, DriveTrackNumbers[DisplayDriveTrack] >> 1, 5 * (DriveTrackNumbers[DisplayDriveTrack] & 1));
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

  if (DriveLEDStates[led] == status) return;

  DriveLEDStates[led] = status;

  icon = wimp_window_get_icon(EmuPane, LEDtoIcon[led]);
  name = (char*)(icon->dat.ind.tit);

  if (status == 0)
    sprintf(name, "led_off");
  else
    sprintf(name, "led_on");

  if (FullScreenMode == 0)
    wimp_window_update_icon(EmuPane, LEDtoIcon[led]);
  else
    video_full_screen_drive_leds(led);
}


/* tape-related ui, dummies so far */
void ui_set_tape_status(int tape_status)
{
}

void ui_display_tape_motor_status(int motor)
{
}

void ui_display_tape_control_status(int control)
{
}

void ui_display_tape_counter(int counter)
{
  if (counter != DatasetteCounter)
  {
    DatasetteCounter = counter;
    wimp_window_write_icon_number_u(ConfWindows[CONF_WIN_TAPE], Icon_ConfTap_DataCounter, counter);
  }
}


void ui_display_tape_current_image(const char *image)
{
  int state;

  wimp_window_write_icon_text(ConfWindows[CONF_WIN_TAPE], Icon_ConfTap_TapeFile, image);

  state = ((image == NULL) || (strlen(image) == 0));
  ui_set_icons_grey(NULL, TapeFileDependentIcons, 1);
}


void ui_display_paused(int flag)
{
  const char *t;

  if (flag == 0)
  {
    ui_temp_resume_sound(); t = SymbolStrings[Symbol_Pause];
    /* resync to avoid including the pause in the speed calculation */
    vsync_resync_speed();
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

  vsync_suspend_speed_eval();

  err.errnum = 0; strcpy(err.errmess, SymbolStrings[Symbol_DlgExtend]);
  button = Wimp_ReportError(&err, 3, WimpTaskName);

  video_full_screen_refresh();

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


static void mon_trap_full(ADDRESS addr, void *unused_data)
{
  ui_temp_suspend_sound();
  OS_FlushBuffer(0);
  mon(addr);
  video_full_screen_refresh();
  ui_temp_resume_sound();
}


static void mon_trap_wimp(ADDRESS addr, void *unused_data)
{
  /* no reentrancy! */
  if (!ui_message_window_is_open(msg_win_monitor))
  {
    EmuPaused = 1;
    ui_display_paused(EmuPaused);
    mon(addr);
    EmuPaused = 0;
    ui_display_paused(EmuPaused);
  }
}


void ui_activate_monitor(void)
{
  if (FullScreenMode != 0)
    maincpu_trigger_trap(mon_trap_full, (void*)0);
  else
    maincpu_trigger_trap(mon_trap_wimp, (void*)0);
}


void ui_display_drive_current_image(unsigned int drive_number,
                                    const char *image)
{
  RO_Window *win;
  const char *useimg = image;
  char buffer[256];
  char *title;

  if ((image == NULL) || (*image == '\0'))
  {
    title = EmuTitle;
  }
  else
  {
    useimg = archdep_extract_dir_and_leaf(image);
    sprintf(buffer, "%s   [%d: %s]", EmuTitle, drive_number + 8, useimg);
    title = buffer;
  }
  win = (ActiveCanvas == NULL) ? EmuWindow : ActiveCanvas->window;
  wimp_window_write_title(win, title);
  video_full_screen_display_image(drive_number, useimg);
}

