/*
 * uiconfig.c - RISC OS configuration data structures.
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



#include "wimp.h"

#include "cartridge.h"
#include "joy.h"
#include "machine.h"
#include "sound.h"
#include "uiconfig.h"
#include "videoarch.h"
#include "drive/drive.h"
#include "video/video-resources.h"





/* Resource names */
static const char Rsrc_Prnt4[] = "Printer4";
static const char Rsrc_Prnt4Dev[] = "Printer4TextDevice";
static const char Rsrc_PrUsr[] = "PrinterUserport";
static const char Rsrc_PrUsrDev[] = "PrinterUserportTextDevice";
static const char Rsrc_Sound[] = "Sound";
static const char Rsrc_SndRate[] = "SoundSampleRate";
static const char Rsrc_SndDev[] = "SoundDeviceName";
static const char Rsrc_SndOver[] = "SoundOversample";
static const char Rsrc_SndBuff[] = "SoundBufferSize";
static const char Rsrc_SpdAdjust[] = "SoundSpeedAdjustment";
static const char Rsrc_Snd16Bit[] = "Use16BitSound";
static const char Rsrc_JoyDev1[] = "JoyDevice1";
static const char Rsrc_JoyDev2[] = "JoyDevice2";
static const char Rsrc_True[] = "DriveTrueEmulation";
static const char Rsrc_TruePar8[] = "Drive8ParallelCable";
static const char Rsrc_TruePar9[] = "Drive9ParallelCable";
static const char Rsrc_TrueExImg8[] = "Drive8ExtendImagePolicy";
static const char Rsrc_TrueExImg9[] = "Drive9ExtendImagePolicy";
static const char Rsrc_TrueIdle8[] = "Drive8IdleMethod";
static const char Rsrc_TrueIdle9[] = "Drive9IdleMethod";
static const char Rsrc_TrueType8[] = "Drive8Type";
static const char Rsrc_TrueType9[] = "Drive9Type";
static const char Rsrc_VideoSync[] = "MachineVideoStandard";
static const char Rsrc_Dos1541[] = "DosName1541";
static const char Rsrc_Dos15412[] = "DosName1541ii";
static const char Rsrc_Dos1571[] = "DosName1571";
static const char Rsrc_Dos1581[] = "DosName1581";
static const char Rsrc_Dos2031[] = "DosName2031";
static const char Rsrc_Dos2040[] = "DosName2040";
static const char Rsrc_Dos3040[] = "DosName3040";
static const char Rsrc_Dos4040[] = "DosName4040";
static const char Rsrc_Dos1001[] = "DosName1001";
static const char Rsrc_Dos8050[] = "DosName8050";
static const char Rsrc_Dos8250[] = "DosName8250";
static const char Rsrc_Poll[] = "PollEvery";
static const char Rsrc_Speed[] = "SpeedEvery";
static const char Rsrc_SndEvery[] = "SoundEvery";
static const char Rsrc_AutoPause[] = "AutoPause";
static const char Rsrc_SpeedLimit[] = "Speed";
static const char Rsrc_Refresh[] = "RefreshRate";
static const char Rsrc_WarpMode[] = "WarpMode";
static const char Rsrc_MaxSkipped[] = "MaxSkippedFrames";
static const char Rsrc_DriveT8[] = "DriveType8";
static const char Rsrc_DriveT9[] = "DriveType9";
static const char Rsrc_DriveT10[] = "DriveType10";
static const char Rsrc_DriveT11[] = "DriveType11";
static const char Rsrc_DriveF8[] = "DriveFile8";
static const char Rsrc_DriveF9[] = "DriveFile9";
static const char Rsrc_DriveF10[] = "DriveFile10";
static const char Rsrc_DriveF11[] = "DriveFile11";
static const char Rsrc_TapeFile[] = "TapeFile";
static const char Rsrc_DataReset[] = "DatasetteResetWithCPU";
static const char Rsrc_AciaDE[] = "AciaDE";
static const char Rsrc_ACIAD6[] = "AciaD6";
static const char Rsrc_Serial[] = "SerialBaud";
static const char Rsrc_RsUsr[] = "RsUser";
static const char Rsrc_RsUsrDev[] = "RsUserDev";
static const char Rsrc_AciaIrq[] = "Acia1Irq";
static const char Rsrc_AciaDev[] = "Acia1Dev";
static const char Rsrc_SidFilt[] = "SidFilters";
static const char Rsrc_ReSid[] = "SidUseResid";
static const char Rsrc_ReSidSamp[] = "SidResidSampling";
static const char Rsrc_ReSidPass[] = "SidResidPassband";
static const char Rsrc_SidMod[] = "SidModel";
static const char Rsrc_SidStereo[] = "SidStereo";
static const char Rsrc_Sid2Addr[] = "SidStereoAddressStart";
static const char Rsrc_CharGen[] = "CharGenName";
static const char Rsrc_Kernal[] = "KernalName";
static const char Rsrc_Basic[] = "BasicName";
static const char Rsrc_REU[] = "REU";
static const char Rsrc_IEEE[] = "IEEE488";
static const char Rsrc_EmuID[] = "EmuID";
static const char Rsrc_CartT[] = "CartridgeType";
static const char Rsrc_CartF[] = "CartridgeFile";
static const char Rsrc_SScoll[] = "VICIICheckSsColl";
static const char Rsrc_SBcoll[] = "VICIICheckSbColl";
static const char Rsrc_Palette[] = "PaletteFile";
static const char Rsrc_ExtPal[] = "ExternalPalette";
static const char Rsrc_NoTraps[] = "NoTraps";
static const char Rsrc_SoundFile[] = "SoundDeviceArg";
static const char Rsrc_SerialFile[] = "SerialFile";
static const char Rsrc_PrinterFile[] = "PrinterFile";
static const char Rsrc_PetMem[] = "RamSize";
static const char Rsrc_PetIO[] = "IOSize";
static const char Rsrc_PetVideo[] = "VideoSize";
static const char Rsrc_PetModel[] = "Model";
static const char Rsrc_PetCrt[] = "Crtc";
static const char Rsrc_PetRAM9[] = "Ram9";
static const char Rsrc_PetRAMA[] = "RamA";
static const char Rsrc_PetDiag[] = "DiagPin";
static const char Rsrc_PetSuper[] = "SuperPET";
static const char Rsrc_VicCart2[] = "CartridgeFile2000";
static const char Rsrc_VicCart6[] = "CartridgeFile6000";
static const char Rsrc_VicCartA[] = "CartridgeFileA000";
static const char Rsrc_VicCartB[] = "CartridgeFileB000";
static const char Rsrc_VicRam0[] = "RamBlock0";
static const char Rsrc_VicRam1[] = "RamBlock1";
static const char Rsrc_VicRam2[] = "RamBlock2";
static const char Rsrc_VicRam3[] = "RamBlock3";
static const char Rsrc_VicRam5[] = "RamBlock5";
static const char Rsrc_C2Cart1[] = "Cart1Name";
static const char Rsrc_C2Cart2[] = "Cart2Name";
static const char Rsrc_C2Cart4[] = "Cart4Name";
static const char Rsrc_C2Cart6[] = "Cart6Name";
static const char Rsrc_C2RAM08[] = "Ram08";
static const char Rsrc_C2RAM1[] = "Ram1";
static const char Rsrc_C2RAM2[] = "Ram2";
static const char Rsrc_C2RAM4[] = "Ram4";
static const char Rsrc_C2RAM6[] = "Ram6";
static const char Rsrc_C2RAMC[] = "RamC";
static const char Rsrc_C2Line[] = "ModelLine";
static const char Rsrc_C2Mem[] = "RamSize";
static const char Rsrc_FullScrNorm[] = "ScreenMode";
static const char Rsrc_FullScrPAL[] = "ScreenModePAL";
static const char Rsrc_FullScrDbl[] = "ScreenModeDouble";
static const char Rsrc_FullSetPal[] = "ScreenSetPalette";
static const char Rsrc_UseBPlot[] = "UseBPlot";
static const char Rsrc_Key8040[] = "40/80ColumnKey";
static const char Rsrc_VDCpalette[] = "VDC_PaletteFile";
static const char Rsrc_VDCsize[] = "VDC64KB";
static const char Rsrc_Z80Bios[] = "Z80BiosName";
static const char Rsrc_VDCdblsze[] = "VDC_DoubleSize";
static const char Rsrc_VDCdblscn[] = "VDCDoubleScan";
static const char Rsrc_ReadOnly8[] = "AttachDevice8Readonly";
static const char Rsrc_ReadOnly9[] = "AttachDevice9Readonly";
static const char Rsrc_ReadOnly10[] = "AttachDevice10Readonly";
static const char Rsrc_ReadOnly11[] = "AttachDevice11Readonly";
static const char Rsrc_PALDepth[] = "PALEmuDepth";
static const char Rsrc_PALDouble[] = "PALEmuDouble";
static const char Rsrc_ColourSat[] = "ColorSaturation";
static const char Rsrc_Contrast[] = "ColorContrast";
static const char Rsrc_Brightness[] = "ColorBrightness";
static const char Rsrc_Gamma[] = "ColorGamma";
static const char Rsrc_LineShade[] = "PALScanLineShade";
static const char Rsrc_PALMode[] = "PALMode";







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

#define Menu_SoundDev_Items	7
#define Menu_SoundDev_Width	200
#define Menu_SoundDev_VIDC	0
#define Menu_SoundDev_VIDCS	1
#define Menu_SoundDev_Dummy	2
#define Menu_SoundDev_FS	3
#define Menu_SoundDev_WAV	4
#define Menu_SoundDev_Speed	5
#define Menu_SoundDev_Dump	6
static struct MenuSoundDevice {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SoundDev_Items];
} MenuSoundDevice = {
  MENU_HEADER("\\MenSndTit", Menu_SoundDev_Width),
  {
    MENU_ITEM("\\MenSndVidc"),
    MENU_ITEM("\\MenSndVidcS"),
    MENU_ITEM("\\MenSndDmy"),
    MENU_ITEM("\\MenSndFS"),
    MENU_ITEM("\\MenSndWav"),
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

#define Menu_SoundBuffer_Items	9
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
    MENU_ITEM("\\MenSBf50"),
    MENU_ITEM_LAST("\\MenSBf100")
  }
};

#define Menu_SpeedAdjust_Items	3
#define Menu_SpeedAdjust_Width	200
#define Menu_SpeedAdjust_Flex	0
#define Menu_SpeedAdjust_Adjust	1
#define Menu_SpeedAdjust_Exact	2
static struct MenuSpeedAdjust {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SpeedAdjust_Items];
} MenuSpeedAdjust = {
  MENU_HEADER("\\MenSAdjTit", Menu_SpeedAdjust_Width),
  {
    MENU_ITEM("\\MenSAdjFlx"),
    MENU_ITEM("\\MenSAdjAdj"),
    MENU_ITEM_LAST("\\MenSAdjXct")
  }
};

#define Menu_ResidSamp_Items	3
#define Menu_ResidSamp_Width	200
#define Menu_ResidSamp_Fast	0
#define Menu_ResidSamp_Inter	1
#define Menu_ResidSamp_Resamp	2
static struct MenuResidSampling {
  RO_MenuHead head;
  RO_MenuItem item[Menu_ResidSamp_Items];
} MenuResidSampling = {
  MENU_HEADER("\\MenRSmpTit", Menu_ResidSamp_Width),
  {
    MENU_ITEM("\\MenRSmpFst"),
    MENU_ITEM("\\MenRSmpInt"),
    MENU_ITEM_LAST("\\MenRSmpRes")
  }
};

#define Menu_Sid2Addr_Items	7+5*8
#define Menu_Sid2Addr_Width	200
static struct MenuSid2Address {
  RO_MenuHead head;
  RO_MenuItem item[Menu_Sid2Addr_Items];
} MenuSid2Address = {
  MENU_HEADER("\\MenSid2ATit", Menu_Sid2Addr_Width),
  {
    MENU_ITEM("D420"),
    MENU_ITEM("D440"),
    MENU_ITEM("D460"),
    MENU_ITEM("D480"),
    MENU_ITEM("D4A0"),
    MENU_ITEM("D4C0"),
    MENU_ITEM("D4E0"),
    MENU_ITEM("D500"),
    MENU_ITEM("D520"),
    MENU_ITEM("D540"),
    MENU_ITEM("D560"),
    MENU_ITEM("D580"),
    MENU_ITEM("D5A0"),
    MENU_ITEM("D5C0"),
    MENU_ITEM("D5E0"),
    MENU_ITEM("D600"),
    MENU_ITEM("D620"),
    MENU_ITEM("D640"),
    MENU_ITEM("D660"),
    MENU_ITEM("D680"),
    MENU_ITEM("D6A0"),
    MENU_ITEM("D6C0"),
    MENU_ITEM("D6E0"),
    MENU_ITEM("D700"),
    MENU_ITEM("D720"),
    MENU_ITEM("D740"),
    MENU_ITEM("D760"),
    MENU_ITEM("D780"),
    MENU_ITEM("D7A0"),
    MENU_ITEM("D7C0"),
    MENU_ITEM("D4E0"),
    MENU_ITEM("DE00"),
    MENU_ITEM("DE20"),
    MENU_ITEM("DE40"),
    MENU_ITEM("DE60"),
    MENU_ITEM("DE80"),
    MENU_ITEM("DEA0"),
    MENU_ITEM("DEC0"),
    MENU_ITEM("DEE0"),
    MENU_ITEM("DF00"),
    MENU_ITEM("DF20"),
    MENU_ITEM("DF40"),
    MENU_ITEM("DF60"),
    MENU_ITEM("DF80"),
    MENU_ITEM("DFA0"),
    MENU_ITEM("DFC0"),
    MENU_ITEM_LAST("DFE0")
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
#define Menu_TrueType_Items	12
#define Menu_TrueType_Width	200
#define Menu_TrueType_None	0
#define Menu_TrueType_1541	1
#define Menu_TrueType_1541II	2
#define Menu_TrueType_1571	3
#define Menu_TrueType_1581	4
#define Menu_TrueType_2031	5
#define Menu_TrueType_2040	6
#define Menu_TrueType_3040	7
#define Menu_TrueType_4040	8
#define Menu_TrueType_1001	9
#define Menu_TrueType_8050	10
#define Menu_TrueType_8250	11

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
      MENU_ITEM("\\MenDtp2031"), \
      MENU_ITEM("\\MenDtp2040"), \
      MENU_ITEM("\\MenDtp3040"), \
      MENU_ITEM("\\MenDtp4040"), \
      MENU_ITEM("\\MenDtp1001"), \
      MENU_ITEM("\\MenDtp8050"), \
      MENU_ITEM_LAST("\\MenDtp8250") \
    } \
  };

TRUE_DRIVE_EXTEND_MENU(MenuTrueExtend8, "\\MenExt8Tit")
TRUE_DRIVE_IDLE_MENU(MenuTrueIdle8, "\\MenIdl8Tit")
TRUE_DRIVE_TYPE_MENU(MenuTrueType8, "\\MenDtp8Tit")
TRUE_DRIVE_EXTEND_MENU(MenuTrueExtend9, "\\MenExt9Tit")
TRUE_DRIVE_IDLE_MENU(MenuTrueIdle9, "\\MenIdl9Tit")
TRUE_DRIVE_TYPE_MENU(MenuTrueType9, "\\MenDtp9Tit")


#define Menu_VideoSync_Items	3
#define Menu_VideoSync_Width	200
#define Menu_VideoSync_PAL	0
#define Menu_VideoSync_NTSC	1

static struct MenuVideoSync {
  RO_MenuHead head;
  RO_MenuItem item[Menu_VideoSync_Items];
} MenuVideoSync = {
  MENU_HEADER("\\MenSncTit", Menu_VideoSync_Width),
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

#define Menu_Cartridge_Items	26
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
    MENU_ITEM("\\MenCrtSS5"),
    MENU_ITEM("\\MenCrtFin1"),
    MENU_ITEM("\\MenCrtFin3"),
    MENU_ITEM("\\MenCrtOcn"),
    /*MENU_ITEM("\\MenCrtOcHg"),*/
    MENU_ITEM("\\MenCrtFun"),
    MENU_ITEM("\\MenCrtSGm"),
    MENU_ITEM("\\MenCrtIEEE"),
    MENU_ITEM("\\MenCrtIDE"),
    MENU_ITEM("\\MenCrtAtom"),
    MENU_ITEM("\\MenCrtEpyx"),
    MENU_ITEM("\\MenCrtWest"),
    MENU_ITEM("\\MenCrtExpt"),
    MENU_ITEM("\\MenCrtRex"),
    MENU_ITEM("\\MenCrtGS"),
    MENU_ITEM("\\MenCrtWrp"),
    MENU_ITEM("\\MenCrtDin"),
    MENU_ITEM_LAST("\\MenCrtZax")
  }
};

#define Menu_PALDepth_Items	5
#define Menu_PALDepth_Width	200
static struct MenuPALDepth {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PALDepth_Items];
} MenuPALDepth = {
  MENU_HEADER("\\MenPALDepT", Menu_PALDepth_Width),
  {
    MENU_ITEM("\\MenPALDOff"),
    MENU_ITEM("\\MenPALDAut"),
    MENU_ITEM("\\MenPALD8"),
    MENU_ITEM("\\MenPALD16"),
    MENU_ITEM_LAST("\\MenPALD32")
  }
};

#define Menu_PALMode_Items	3
#define Menu_PALMode_Width	200
static struct MenuPALMode {
  RO_MenuHead head;
  RO_MenuItem item[Menu_PALMode_Items];
} MenuPALMode = {
  MENU_HEADER("\\MenPModeT", Menu_PALMode_Width),
  {
    MENU_ITEM("\\MenPMFast"),
    MENU_ITEM("\\MenPMSharp"),
    MENU_ITEM_LAST("\\MenPMBlur")
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

#define Menu_DosName_Items	11
#define Menu_DosName_Width	200
static struct MenuDosName {
  RO_MenuHead head;
  RO_MenuItem item[Menu_DosName_Items];
} MenuDosName = {
  MENU_HEADER("\\MenDOSTit", Menu_DosName_Width),
  {
    MENU_ITEM("\\MenDOS1541"),
    MENU_ITEM("\\MenDOS15412"),
    MENU_ITEM("\\MenDOS1571"),
    MENU_ITEM("\\MenDOS1581"),
    MENU_ITEM("\\MenDOS2031"),
    MENU_ITEM("\\MenDOS2040"),
    MENU_ITEM("\\MenDOS3040"),
    MENU_ITEM("\\MenDOS4040"),
    MENU_ITEM("\\MenDOS1001"),
    MENU_ITEM("\\MenDOS8050"),
    MENU_ITEM_LAST("\\MenDOS8250")
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

#define Menu_CBM2Model_Items	7
#define Menu_CBM2Model_Width	200
static struct MenuCBM2Model {
  RO_MenuHead head;
  RO_MenuItem item[Menu_CBM2Model_Items];
} MenuCBM2Model = {
  MENU_HEADER("\\MenC2MdTit", Menu_CBM2Model_Width),
  {
    MENU_ITEM("\\MenC2Md51"),
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

#define Menu_SysKbd_Items	2
#define Menu_SysKbd_Width	200
static struct MenuSysKeyboard {
  RO_MenuHead head;
  RO_MenuItem item[Menu_SysKbd_Items];
} MenuSysKeyboard = {
  MENU_HEADER("\\MenKbdT", Menu_SysKbd_Width),
  {
    {MFlg_Warning, (RO_MenuHead*)-1, Menu_Flags, {"\\MenKbdSav"}},
    MENU_ITEM_LAST("\\MenKbdLd")
  }
};





static struct MenuDisplayVideoSync {
  disp_desc_t dd;
  int values[Menu_VideoSync_Items];
} MenuDisplayVideoSync = {
  {Rsrc_VideoSync, {CONF_WIN_SYSTEM, Icon_ConfSys_VideoSyncT},
    (RO_MenuHead*)&MenuVideoSync, Menu_VideoSync_Items, 0, 1<<Menu_VideoSync_Custom},
  {MACHINE_SYNC_PAL, MACHINE_SYNC_NTSC, 0}
};

#define DISP_TRUE_DRIVE_EXTEND_MENU(n) \
  static struct MenuDisplayTrueExtend##n { \
    disp_desc_t dd; \
    int values[Menu_TrueExtend_Items]; \
  } MenuDisplayTrueExtend##n = { \
    {Rsrc_TrueExImg##n, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt##n##T}, \
      (RO_MenuHead*)&MenuTrueExtend##n, Menu_TrueExtend_Items, 0, 0}, \
    {DRIVE_EXTEND_NEVER, DRIVE_EXTEND_ASK, DRIVE_EXTEND_ACCESS} \
  };

#define DISP_TRUE_DRIVE_IDLE_MENU(n) \
  static struct MenuDisplayTrueIdle##n { \
    disp_desc_t dd; \
    int values[Menu_TrueIdle_Items]; \
  } MenuDisplayTrueIdle##n = { \
    {Rsrc_TrueIdle##n, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle##n##T}, \
      (RO_MenuHead*)&MenuTrueIdle##n, Menu_TrueIdle_Items, 0, 0}, \
    {DRIVE_IDLE_NO_IDLE, DRIVE_IDLE_SKIP_CYCLES, DRIVE_IDLE_TRAP_IDLE} \
  };

#define DISP_TRUE_DRIVE_TYPE_MENU(n) \
  static struct MenuDisplayTrueType##n { \
    disp_desc_t dd; \
    int values[Menu_TrueType_Items]; \
  } MenuDisplayTrueType##n = { \
    {Rsrc_TrueType##n, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvType##n##T}, \
      (RO_MenuHead*)&MenuTrueType##n, Menu_TrueType_Items, 0, 0}, \
    {DRIVE_TYPE_NONE, DRIVE_TYPE_1541, DRIVE_TYPE_1541II, DRIVE_TYPE_1571, DRIVE_TYPE_1581, \
     DRIVE_TYPE_2031, DRIVE_TYPE_2040, DRIVE_TYPE_3040, DRIVE_TYPE_4040, DRIVE_TYPE_1001, \
     DRIVE_TYPE_8050, DRIVE_TYPE_8250} \
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
  {Rsrc_SndRate, {CONF_WIN_SOUND, Icon_ConfSnd_SampleRateT},
    (RO_MenuHead*)&MenuSampleRate, Menu_SampRate_Items, 0, 0},
  {8000, 11025, 22050, 44100, 48000}
};

static const char SoundDevice0[] = "vidc";
static const char SoundDevice1[] = "vidcs";
static const char SoundDevice2[] = "dummy";
static const char SoundDevice3[] = "fs";
static const char SoundDevice4[] = "wav";
static const char SoundDevice5[] = "speed";
static const char SoundDevice6[] = "dump";

static struct MenuDisplaySoundDevice {
  disp_desc_t dd;
  int values[Menu_SoundDev_Items];
} MenuDisplaySoundDevice = {
  {Rsrc_SndDev, {CONF_WIN_SOUND, Icon_ConfSnd_SoundDevT},
    (RO_MenuHead*)&MenuSoundDevice, Menu_SoundDev_Items, DISP_DESC_STRING, 0},
  {(int)SoundDevice0, (int)SoundDevice1, (int)SoundDevice2, (int)SoundDevice3, (int)SoundDevice4,(int)SoundDevice5, (int)SoundDevice6}
};

static struct MenuDisplaySoundOver {
  disp_desc_t dd;
  int values[Menu_SoundOver_Items];
} MenuDisplaySoundOver = {
  {Rsrc_SndOver, {CONF_WIN_SOUND, Icon_ConfSnd_OversampleT},
    (RO_MenuHead*)&MenuSoundOver, Menu_SoundOver_Items, 0, 0},
  {0, 1, 2, 3}
};

static struct MenuDisplaySidModel {
  disp_desc_t dd;
  int values[Menu_SidModel_Items];
} MenuDisplaySidModel = {
  {Rsrc_SidMod, {CONF_WIN_SOUND, Icon_ConfSnd_SidModelT},
    (RO_MenuHead*)&MenuSidModel, Menu_SidModel_Items, 0, 0},
  {0, 1}
};

static struct MenuDisplaySpeedAdjust {
  disp_desc_t dd;
  int values[Menu_SpeedAdjust_Items];
} MenuDisplaySpeedAdjust = {
  {Rsrc_SpdAdjust, {CONF_WIN_SOUND, Icon_ConfSnd_SpeedAdjustT},
    (RO_MenuHead*)&MenuSpeedAdjust, Menu_SpeedAdjust_Items, 0, 0},
  {SOUND_ADJUST_FLEXIBLE, SOUND_ADJUST_ADJUSTING, SOUND_ADJUST_EXACT}
};

static struct MenuDisplayResidSampling {
  disp_desc_t dd;
  int values[Menu_ResidSamp_Items];
} MenuDisplayResidSampling = {
  {Rsrc_ReSidSamp, {CONF_WIN_SOUND, Icon_ConfSnd_ResidSampT},
    (RO_MenuHead*)&MenuResidSampling, Menu_ResidSamp_Items, 0, 0},
  { 0, 1, 2 }
};

static struct MenuDisplaySid2Address {
  disp_desc_t dd;
  int values[Menu_Sid2Addr_Items];
} MenuDisplaySid2Address = {
  {Rsrc_Sid2Addr, {CONF_WIN_SOUND, Icon_ConfSnd_Sid2AddrT},
    (RO_MenuHead*)&MenuSid2Address, Menu_Sid2Addr_Items, 0, 0},
  { 0xd420, 0xd440, 0xd460, 0xd480, 0xd4a0, 0xd4c0, 0xd4e0,
    0xd500, 0xd520, 0xd540, 0xd560, 0xd580, 0xd5a0, 0xd5c0, 0xd5e0,
    0xd600, 0xd620, 0xd640, 0xd660, 0xd680, 0xd6a0, 0xd6c0, 0xd6e0,
    0xd700, 0xd720, 0xd740, 0xd760, 0xd780, 0xd7a0, 0xd7c0, 0xd7e0,
    0xde00, 0xde20, 0xde40, 0xde60, 0xde80, 0xdea0, 0xdec0, 0xdee0,
    0xdf00, 0xdf20, 0xdf40, 0xdf60, 0xdf80, 0xdfa0, 0xdfc0, 0xdfe0 }
};

static struct MenuDisplaySpeedLimit {
  disp_desc_t dd;
  int values[Menu_SpeedLimit_Items];
} MenuDisplaySpeedLimit = {
  {Rsrc_SpeedLimit, {CONF_WIN_SYSTEM, Icon_ConfSys_SpeedLmtT},
    (RO_MenuHead*)&MenuSpeedLimit, Menu_SpeedLimit_Items, 0, 0},
  {200, 100, 50, 20, 10, 0}
};

static struct MenuDisplayRefresh {
  disp_desc_t dd;
  int values[Menu_Refresh_Items];
} MenuDisplayRefresh = {
  {Rsrc_Refresh, {CONF_WIN_VIDEO, Icon_ConfVid_RefreshT},
    (RO_MenuHead*)&MenuRefresh, Menu_Refresh_Items, 0, 0},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
};

static struct MenuDisplayPrintDev {
  disp_desc_t dd;
  int values[Menu_PrntDev_Items];
} MenuDisplayPrintDev = {
  {Rsrc_Prnt4Dev, {CONF_WIN_DEVICES, Icon_ConfDev_PrntDevT},
    (RO_MenuHead*)&MenuPrintDev, Menu_PrntDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayUserDev {
  disp_desc_t dd;
  int values[Menu_UserDev_Items];
} MenuDisplayUserDev = {
  {Rsrc_PrUsrDev, {CONF_WIN_DEVICES, Icon_ConfDev_PrntUsrDevT},
    (RO_MenuHead*)&MenuUserDev, Menu_UserDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayAciaDevice {
  disp_desc_t dd;
  int values[Menu_AciaDev_Items];
} MenuDisplayAciaDevice = {
  {Rsrc_AciaDev, {CONF_WIN_DEVICES, Icon_ConfDev_ACIADevT},
    (RO_MenuHead*)&MenuAciaDevice, Menu_AciaDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayRsUserDevice {
  disp_desc_t dd;
  int values[Menu_RsUsrDev_Items];
} MenuDisplayRsUserDevice = {
  {Rsrc_RsUsrDev, {CONF_WIN_DEVICES, Icon_ConfDev_RsUsrDevT},
    (RO_MenuHead*)&MenuRsUserDevice, Menu_RsUsrDev_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplaySerialBaud {
  disp_desc_t dd;
  int values[Menu_Serial_Items];
} MenuDisplaySerialBaud = {
  {Rsrc_Serial, {CONF_WIN_DEVICES, Icon_ConfDev_SerialT},
    (RO_MenuHead*)&MenuSerialBaud, Menu_Serial_Items, 0, 0},
  {9, 1, 10, 11, 2, 3, 12, 4, 13, 5, 14, 6, 15, 7, 8}
};


static struct MenuDisplayCartridgeType {
  disp_desc_t dd;
  int values[Menu_Cartridge_Items];
} MenuDisplayCartridgeType = {
  {Rsrc_CartT, {CONF_WIN_SYSTEM, Icon_ConfSys_CartTypeT},
    (RO_MenuHead*)&MenuCartridgeType, Menu_Cartridge_Items, 0, 0},
  {CARTRIDGE_NONE, CARTRIDGE_GENERIC_8KB, CARTRIDGE_GENERIC_16KB, CARTRIDGE_CRT,
   CARTRIDGE_ACTION_REPLAY, CARTRIDGE_KCS_POWER, CARTRIDGE_SIMONS_BASIC,
   CARTRIDGE_ULTIMAX, CARTRIDGE_SUPER_SNAPSHOT, CARTRIDGE_SUPER_SNAPSHOT_V5,
   CARTRIDGE_FINAL_I, CARTRIDGE_FINAL_III, CARTRIDGE_OCEAN, /*CARTRIDGE_OCEAN_HUGE,*/
   CARTRIDGE_FUNPLAY, CARTRIDGE_SUPER_GAMES, CARTRIDGE_IEEE488, CARTRIDGE_IDE64, CARTRIDGE_ATOMIC_POWER,
   CARTRIDGE_EPYX_FASTLOAD, CARTRIDGE_WESTERMANN, CARTRIDGE_WESTERMANN, CARTRIDGE_REX,
   CARTRIDGE_GS, CARTRIDGE_WARPSPEED, CARTRIDGE_DINAMIC, CARTRIDGE_ZAXXON}
};

static struct MenuDisplayPALDepth {
  disp_desc_t dd;
  int values[Menu_PALDepth_Items];
} MenuDisplayPALDepth = {
  {Rsrc_PALDepth, {CONF_WIN_VIDEO, Icon_ConfVid_PALDepthT},
    (RO_MenuHead*)&MenuPALDepth, Menu_PALDepth_Items, 0, 0},
  {PAL_EMU_DEPTH_NONE, PAL_EMU_DEPTH_AUTO, PAL_EMU_DEPTH_8, PAL_EMU_DEPTH_16, PAL_EMU_DEPTH_32}
};

static struct MenuDisplayPALMode {
  disp_desc_t dd;
  int values[Menu_PALMode_Items];
} MenuDisplayPALMode = {
  {Rsrc_PALMode, {CONF_WIN_VIDEO, Icon_ConfVid_PalModeT},
    (RO_MenuHead*)&MenuPALMode, Menu_PALMode_Items, 0, 0},
  {VIDEO_RESOURCE_PAL_MODE_FAST, VIDEO_RESOURCE_PAL_MODE_SHARP, VIDEO_RESOURCE_PAL_MODE_BLUR}
};

static struct MenuDisplayPetMemory {
  disp_desc_t dd;
  int values[Menu_PetMemory_Items];
} MenuDisplayPetMemory = {
  {Rsrc_PetMem, {CONF_WIN_PET, Icon_ConfPET_PetMemT},
    (RO_MenuHead*)&MenuPetMemory, Menu_PetMemory_Items, 0, 0},
  {4, 8, 16, 32, 96, 128}
};

static struct MenuDisplayPetIO {
  disp_desc_t dd;
  int values[Menu_PetIO_Items];
} MenuDisplayPetIO = {
  {Rsrc_PetIO, {CONF_WIN_PET, Icon_ConfPET_PetIOT},
    (RO_MenuHead*)&MenuPetIO, Menu_PetIO_Items, 0, 0},
  {0x800, 0x100}
};

static struct MenuDisplayPetVideo {
  disp_desc_t dd;
  int values[Menu_PetVideo_Items];
} MenuDisplayPetVideo = {
  {Rsrc_PetVideo, {CONF_WIN_PET, Icon_ConfPET_PetVideoT},
    (RO_MenuHead*)&MenuPetVideo, Menu_PetVideo_Items, 0, 0},
  {0, 40, 80}
};

static const char PetModel0[] = "2001";
static const char PetModel1[] = "3008";
static const char PetModel2[] = "3016";
static const char PetModel3[] = "3032";
static const char PetModel4[] = "3032B";
static const char PetModel5[] = "4016";
static const char PetModel6[] = "4032";
static const char PetModel7[] = "4032B";
static const char PetModel8[] = "8032";
static const char PetModel9[] = "8096";
static const char PetModel10[] = "8296";
static const char PetModel11[] = "SuperPET";

static struct MenuDisplayPetModel {
  disp_desc_t dd;
  int values[Menu_PetModel_Items];
} MenuDisplayPetModel = {
  {Rsrc_PetModel, {CONF_WIN_PET, Icon_ConfPET_PetModelT},
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
  Icon_ConfVIC_VICCartF, 0
};

static struct MenuDisplayVicCartridge {
  disp_desc_t dd;
  int values[Menu_VicCart_Items];
} MenuDisplayVicCartridge = {
  {(char*)&VicCartridgeDesc, {CONF_WIN_VIC, Icon_ConfVIC_VICCartT},
    (RO_MenuHead*)&MenuVicCartridge, Menu_VicCart_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_VicCart2, (int)Rsrc_VicCart6, (int)Rsrc_VicCartA, (int)Rsrc_VicCartB}
};

static disp_strshow_t DosNameDesc = {
  Icon_ConfSys_DosNameF, 0
};

static struct MenuDisplayDosName {
  disp_desc_t dd;
  int values[Menu_DosName_Items];
} MenuDisplayDosName = {
  {(char*)&DosNameDesc, {CONF_WIN_SYSTEM, Icon_ConfSys_DosNameT},
    (RO_MenuHead*)&MenuDosName, Menu_DosName_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_Dos1541, (int)Rsrc_Dos15412, (int)Rsrc_Dos1571, (int)Rsrc_Dos1581,
   (int)Rsrc_Dos2031, (int)Rsrc_Dos2040, (int)Rsrc_Dos3040, (int)Rsrc_Dos4040,
   (int)(int)Rsrc_Dos1001, (int)Rsrc_Dos8050, (int)Rsrc_Dos8250}
};

static struct MenuDisplayCBM2Line {
  disp_desc_t dd;
  int values[Menu_CBM2Line_Items];
} MenuDisplayCBM2Line = {
  {Rsrc_C2Line, {CONF_WIN_CBM2, Icon_ConfCBM_CBM2LineT},
    (RO_MenuHead*)&MenuCBM2Line, Menu_CBM2Line_Items, 0, 0},
  {0, 1, 2}
};

static struct MenuDisplayCBM2Memory {
  disp_desc_t dd;
  int values[Menu_CBM2Mem_Items];
} MenuDisplayCBM2Memory = {
  {Rsrc_C2Mem, {CONF_WIN_CBM2, Icon_ConfCBM_CBM2MemT},
    (RO_MenuHead*)&MenuCBM2Memory, Menu_CBM2Mem_Items, 0, 0},
  {128, 256, 512, 1024}
};

static const char CBM2Model0[] = "510";
static const char CBM2Model1[] = "610";
static const char CBM2Model2[] = "620";
static const char CBM2Model3[] = "620+";
static const char CBM2Model4[] = "710";
static const char CBM2Model5[] = "720";
static const char CBM2Model6[] = "720+";

static struct MenuDisplayCBM2Model {
  disp_desc_t dd;
  int values[Menu_CBM2Model_Items];
} MenuDisplayCBM2Model = {
  {NULL, {CONF_WIN_CBM2, Icon_ConfCBM_CBM2ModelT},
    (RO_MenuHead*)&MenuCBM2Model, Menu_CBM2Model_Items, DISP_DESC_STRING, 0},
  {(int)CBM2Model0, (int)CBM2Model1, (int)CBM2Model2, (int)CBM2Model3, (int)CBM2Model4, (int)CBM2Model5, (int)CBM2Model6}
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
  Icon_ConfCBM_CBM2CartF, 0
};

static struct MenuDisplayCBM2Cartridge {
  disp_desc_t dd;
  int values[Menu_CBM2Cart_Items];
} MenuDisplayCBM2Cartridge = {
  {(char*)&CBM2CartridgeDesc, {CONF_WIN_CBM2, Icon_ConfCBM_CBM2CartT},
    (RO_MenuHead*)&MenuCBM2Cartridge, Menu_CBM2Cart_Items, DISP_DESC_STRSHOW, 0},
  {(int)Rsrc_C2Cart1, (int)Rsrc_C2Cart2, (int)Rsrc_C2Cart4, (int)Rsrc_C2Cart6}
};

static struct MenuDisplaySoundBuffer {
  disp_desc_t dd;
  int values[Menu_SoundBuffer_Items];
} MenuDisplaySoundBuffer = {
  {Rsrc_SndBuff, {CONF_WIN_SOUND, Icon_ConfSnd_SoundBuffT},
    (RO_MenuHead*)&MenuSoundBuffer, Menu_SoundBuffer_Items, 0, 0},
  {20, 40, 60, 80, 100, 200, 350, 500, 1000}
};

#define DISP_JOYSTICK_DEVICE_MENU(n) \
  static struct MenuDisplayJoyDevice##n { \
    disp_desc_t dd; \
    int values[Menu_JoyDevice_Items]; \
  } MenuDisplayJoyDevice##n = { \
    {Rsrc_JoyDev##n, {CONF_WIN_JOY, Icon_ConfJoy_JoyPort##n##T}, \
      (RO_MenuHead*)&MenuJoyDevice##n, Menu_JoyDevice_Items, 0, 0}, \
    {JOYDEV_NONE, JOYDEV_KBD1, JOYDEV_KBD2, JOYDEV_JOY1, JOYDEV_JOY2} \
  };

DISP_JOYSTICK_DEVICE_MENU(1)
DISP_JOYSTICK_DEVICE_MENU(2)







/* Config Menus */
menu_icon_t ConfigMenus[] = {
  {(RO_MenuHead*)&MenuPrintDev, Rsrc_Prnt4Dev,
    {CONF_WIN_DEVICES, Icon_ConfDev_PrntDev}},		/* 0 (prdevice.c) */
  {(RO_MenuHead*)&MenuUserDev, Rsrc_PrUsrDev,
    {CONF_WIN_DEVICES, Icon_ConfDev_PrntUsrDev}},	/* 1 (pruser.c) */
  {(RO_MenuHead*)&MenuSampleRate, Rsrc_SndRate,
    {CONF_WIN_SOUND, Icon_ConfSnd_SampleRate}},		/* 2 (sound.c) */
  {(RO_MenuHead*)&MenuSoundDevice, Rsrc_SndDev,
    {CONF_WIN_SOUND, Icon_ConfSnd_SoundDev}},		/* 3 (sound.c) */
  {(RO_MenuHead*)&MenuSoundOver, Rsrc_SndOver,
    {CONF_WIN_SOUND, Icon_ConfSnd_Oversample}},		/* 4 (sound.c) */
  {(RO_MenuHead*)&MenuVideoSync, Rsrc_VideoSync,
    {CONF_WIN_SYSTEM, Icon_ConfSys_VideoSync}},		/* 5 (drive.c) */
  {(RO_MenuHead*)&MenuTrueIdle8, Rsrc_TrueIdle8,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle8}},	/* 6 */
  {(RO_MenuHead*)&MenuTrueExtend8, Rsrc_TrueExImg8,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt8}},	/* 7 */
  {(RO_MenuHead*)&MenuTrueType8, Rsrc_TrueType8,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvType8}},	/* 8 */
  {(RO_MenuHead*)&MenuTrueIdle9, Rsrc_TrueIdle9,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvIdle9}},	/* 9 */
  {(RO_MenuHead*)&MenuTrueExtend9, Rsrc_TrueExImg9,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvExt9}},	/* 10 */
  {(RO_MenuHead*)&MenuTrueType9, Rsrc_TrueType9,
    {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvType9}},	/* 11 */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT8,
    {CONF_WIN_DRIVES, Icon_ConfDrv_DriveType8}},	/* 12 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT9,
    {CONF_WIN_DRIVES, Icon_ConfDrv_DriveType9}},	/* 13 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT10,
    {CONF_WIN_DRIVES, Icon_ConfDrv_DriveType10}},	/* 14 (here) */
  {(RO_MenuHead*)&MenuDriveType, Rsrc_DriveT11,
    {CONF_WIN_DRIVES, Icon_ConfDrv_DriveType11}},	/* 15 (here) */
  {(RO_MenuHead*)&MenuCartridgeType, Rsrc_CartT,
    {CONF_WIN_SYSTEM, Icon_ConfSys_CartType}},		/* 16 (cartridge.c) */
  {(RO_MenuHead*)&MenuRsUserDevice, Rsrc_RsUsrDev,
    {CONF_WIN_DEVICES, Icon_ConfDev_RsUsrDev}},		/* 17 (rsuser.c) */
  {(RO_MenuHead*)&MenuAciaDevice, Rsrc_AciaDev,
    {CONF_WIN_DEVICES, Icon_ConfDev_ACIADev}},		/* 18 (c64acia.c) */
  {(RO_MenuHead*)&MenuSerialBaud, Rsrc_Serial,
    {CONF_WIN_DEVICES, Icon_ConfDev_Serial}},		/* 19 (serial.c) */
  {(RO_MenuHead*)&MenuSidModel, Rsrc_SidMod,
    {CONF_WIN_SOUND, Icon_ConfSnd_SidModel}},		/* 20 (sid.c) */
  {(RO_MenuHead*)&MenuSpeedLimit, Rsrc_SpeedLimit,
    {CONF_WIN_SYSTEM, Icon_ConfSys_SpeedLmt}},		/* 21 (here) */
  {(RO_MenuHead*)&MenuRefresh, Rsrc_Refresh,
    {CONF_WIN_VIDEO, Icon_ConfVid_Refresh}},		/* 22 (here) */
  {(RO_MenuHead*)&MenuPetMemory, Rsrc_PetMem,
    {CONF_WIN_PET, Icon_ConfPET_PetMem}},		/* 23 (pets.c) */
  {(RO_MenuHead*)&MenuPetIO, Rsrc_PetIO,
    {CONF_WIN_PET, Icon_ConfPET_PetIO}},		/* 24 */
  {(RO_MenuHead*)&MenuPetVideo, Rsrc_PetVideo,
    {CONF_WIN_PET, Icon_ConfPET_PetVideo}},		/* 25 */
  {(RO_MenuHead*)&MenuPetModel, Rsrc_PetModel,
    {CONF_WIN_PET, Icon_ConfPET_PetModel}},		/* 26 */
  {(RO_MenuHead*)&MenuVicRam, NULL,
    {CONF_WIN_VIC, Icon_ConfVIC_VICMem}},		/* 27 */
  {(RO_MenuHead*)&MenuVicCartridge, NULL,
    {CONF_WIN_VIC, Icon_ConfVIC_VICCart}},		/* 28 */
  {(RO_MenuHead*)&MenuDosName, NULL,
    {CONF_WIN_SYSTEM, Icon_ConfSys_DosName}},		/* 29 */
  {(RO_MenuHead*)&MenuCBM2Line, Rsrc_C2Line,
    {CONF_WIN_CBM2, Icon_ConfCBM_CBM2Line}},		/* 30 */
  {(RO_MenuHead*)&MenuCBM2Memory, Rsrc_C2Mem,
    {CONF_WIN_CBM2, Icon_ConfCBM_CBM2Mem}},		/* 31 */
  {(RO_MenuHead*)&MenuCBM2Model, NULL,
    {CONF_WIN_CBM2, Icon_ConfCBM_CBM2Model}},		/* 32 */
  {(RO_MenuHead*)&MenuCBM2RAM, NULL,
    {CONF_WIN_CBM2, Icon_ConfCBM_CBM2RAM}},		/* 33 */
  {(RO_MenuHead*)&MenuCBM2Cartridge, NULL,
    {CONF_WIN_CBM2, Icon_ConfCBM_CBM2Cart}},		/* 34 */
  {(RO_MenuHead*)&MenuSoundBuffer, NULL,
    {CONF_WIN_SOUND, Icon_ConfSnd_SoundBuff}},		/* 35 */
  {(RO_MenuHead*)&MenuJoyDevice1, NULL,
    {CONF_WIN_JOY, Icon_ConfJoy_JoyPort1}},		/* 36 */
  {(RO_MenuHead*)&MenuJoyDevice2, NULL,
    {CONF_WIN_JOY, Icon_ConfJoy_JoyPort2}},		/* 37 */
  {NULL, NULL,
    {CONF_WIN_SYSTEM, Icon_ConfSys_ROMSet}},		/* 38 */
  {(RO_MenuHead*)&MenuRomAction, NULL,
    {CONF_WIN_SYSTEM, Icon_ConfSys_ROMAction}},		/* 39 */
  {(RO_MenuHead*)&MenuSysKeyboard, NULL,
    {CONF_WIN_SYSTEM, Icon_ConfSys_Keyboard}},		/* 40 */
  {(RO_MenuHead*)&MenuSpeedAdjust, Rsrc_SpdAdjust,
    {CONF_WIN_SOUND, Icon_ConfSnd_SpeedAdjust}},	/* 41 */
  {(RO_MenuHead*)&MenuResidSampling, Rsrc_ReSidSamp,
    {CONF_WIN_SOUND, Icon_ConfSnd_ResidSamp}},		/* 42 */
  {(RO_MenuHead*)&MenuSid2Address, Rsrc_Sid2Addr,
    {CONF_WIN_SOUND, Icon_ConfSnd_Sid2Addr}},		/* 43 */
  {(RO_MenuHead*)&MenuPALDepth, Rsrc_PALDepth,
    {CONF_WIN_VIDEO, Icon_ConfVid_PALDepth}},		/* 44 */
  {(RO_MenuHead*)&MenuPALMode, Rsrc_PALMode,
    {CONF_WIN_VIDEO, Icon_ConfVid_PalMode}},		/* 45 */
  {NULL, NULL,
    {CONF_WIN_VIDEO, Icon_ConfVid_VCache}},		/* 46 */
  {NULL, NULL, {0, 0}}
};


disp_desc_t *ConfigDispDescs[] = {
  (disp_desc_t*)&MenuDisplayPrintDev,
  (disp_desc_t*)&MenuDisplayUserDev,
  (disp_desc_t*)&MenuDisplaySampleRate,
  (disp_desc_t*)&MenuDisplaySoundDevice,
  (disp_desc_t*)&MenuDisplaySoundOver,
  (disp_desc_t*)&MenuDisplayVideoSync,
  (disp_desc_t*)&MenuDisplayTrueIdle8,
  (disp_desc_t*)&MenuDisplayTrueExtend8,
  (disp_desc_t*)&MenuDisplayTrueType8,
  (disp_desc_t*)&MenuDisplayTrueIdle9,
  (disp_desc_t*)&MenuDisplayTrueExtend9,
  (disp_desc_t*)&MenuDisplayTrueType9,
  NULL,
  NULL,
  NULL,
  NULL,
  (disp_desc_t*)&MenuDisplayCartridgeType,
  (disp_desc_t*)&MenuDisplayRsUserDevice,
  (disp_desc_t*)&MenuDisplayAciaDevice,
  (disp_desc_t*)&MenuDisplaySerialBaud,
  (disp_desc_t*)&MenuDisplaySidModel,
  (disp_desc_t*)&MenuDisplaySpeedLimit,
  (disp_desc_t*)&MenuDisplayRefresh,
  (disp_desc_t*)&MenuDisplayPetMemory,
  (disp_desc_t*)&MenuDisplayPetIO,
  (disp_desc_t*)&MenuDisplayPetVideo,
  (disp_desc_t*)&MenuDisplayPetModel,
  (disp_desc_t*)&MenuDisplayVicRam,
  (disp_desc_t*)&MenuDisplayVicCartridge,
  (disp_desc_t*)&MenuDisplayDosName,
  (disp_desc_t*)&MenuDisplayCBM2Line,
  (disp_desc_t*)&MenuDisplayCBM2Memory,
  (disp_desc_t*)&MenuDisplayCBM2Model,
  (disp_desc_t*)&MenuDisplayCBM2RAM,
  (disp_desc_t*)&MenuDisplayCBM2Cartridge,
  (disp_desc_t*)&MenuDisplaySoundBuffer,
  (disp_desc_t*)&MenuDisplayJoyDevice1,
  (disp_desc_t*)&MenuDisplayJoyDevice2,
  NULL,
  NULL,
  NULL,
  (disp_desc_t*)&MenuDisplaySpeedAdjust,
  (disp_desc_t*)&MenuDisplayResidSampling,
  (disp_desc_t*)&MenuDisplaySid2Address,
  (disp_desc_t*)&MenuDisplayPALDepth,
  (disp_desc_t*)&MenuDisplayPALMode,
  NULL,
  NULL
};



/* Config Icons */
config_item_t Configurations[] = {
  {Rsrc_Prnt4, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_PrntOn}},
  {Rsrc_PrUsr, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_PrntUsrOn}},
  {Rsrc_Sound, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_ConfSnd_SoundOn}},
  {Rsrc_Snd16Bit, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_ConfSnd_Sound16Bit}},
  {Rsrc_NoTraps, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_NoTraps}},
  {Rsrc_True, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrv}},
  {Rsrc_TruePar8, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar8}},
  {Rsrc_TruePar9, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_TrueDrvPar9}},
  {Rsrc_Poll, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_ConfSys_PollEvery}},
  {Rsrc_Speed, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_ConfSys_SpeedEvery}},
  {Rsrc_SndEvery, CONFIG_INT, {CONF_WIN_SYSTEM, Icon_ConfSys_SoundEvery}},
  {Rsrc_AutoPause, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_AutoPause}},
  {Rsrc_FullScrNorm, CONFIG_STRING, {CONF_WIN_VIDEO, Icon_ConfVid_FullScrNorm}},
  {Rsrc_FullScrPAL, CONFIG_STRING, {CONF_WIN_VIDEO, Icon_ConfVid_FullScrPAL}},
  {Rsrc_FullScrDbl, CONFIG_STRING, {CONF_WIN_VIDEO, Icon_ConfVid_FullScrDbl}},
  {Rsrc_FullSetPal, CONFIG_SELECT, {CONF_WIN_VIDEO, Icon_ConfVid_SetPalette}},
  {Rsrc_UseBPlot, CONFIG_SELECT, {CONF_WIN_VIDEO, Icon_ConfVid_UseBPlot}},
  {Rsrc_MaxSkipped, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_MaxSkipFrms}},
  {Rsrc_DriveF8, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveFile8}},
  {Rsrc_DriveF9, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveFile9}},
  {Rsrc_DriveF10, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveFile10}},
  {Rsrc_DriveF11, CONFIG_STRING, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveFile11}},
  {Rsrc_TapeFile, CONFIG_STRING, {CONF_WIN_TAPE, Icon_ConfTap_TapeFile}},
  {Rsrc_DataReset, CONFIG_SELECT, {CONF_WIN_TAPE, Icon_ConfTap_DataReset}},
  {Rsrc_WarpMode, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_WarpMode}},
  {Rsrc_CharGen, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_CharGen}},
  {Rsrc_Kernal, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_Kernal}},
  {Rsrc_Basic, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_Basic}},
  {Rsrc_REU, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_REU}},
  {Rsrc_IEEE, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_IEEE488}},
  {Rsrc_EmuID, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_EmuID}},
  {Rsrc_AciaDE, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_ACIADE}},
  {Rsrc_ACIAD6, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_ACIAD67}},
  {Rsrc_CartF, CONFIG_STRING, {CONF_WIN_SYSTEM, Icon_ConfSys_CartFile}},
  {Rsrc_RsUsr, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_RsUsr}},
  {Rsrc_AciaIrq, CONFIG_SELECT, {CONF_WIN_DEVICES, Icon_ConfDev_ACIAIrq}},
  {Rsrc_SidFilt, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_ConfSnd_SidFilter}},
  {Rsrc_ReSid, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_ConfSnd_UseResid}},
  {Rsrc_ReSidPass, CONFIG_INT, {CONF_WIN_SOUND, Icon_ConfSnd_ResidPass}},
  {Rsrc_SidStereo, CONFIG_SELECT, {CONF_WIN_SOUND, Icon_ConfSnd_SidStereo}},
  {Rsrc_SScoll, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_CheckSScoll}},
  {Rsrc_SBcoll, CONFIG_SELECT, {CONF_WIN_SYSTEM, Icon_ConfSys_CheckSBcoll}},
  {Rsrc_Palette, CONFIG_STRING, {CONF_WIN_VIDEO, Icon_ConfVid_Palette}},
  {Rsrc_ExtPal, CONFIG_SELECT, {CONF_WIN_VIDEO, Icon_ConfVid_ExtPal}},
  {Rsrc_SoundFile, CONFIG_STRING, {CONF_WIN_SOUND, Icon_ConfSnd_FileSndPath}},
  {Rsrc_SerialFile, CONFIG_STRING, {CONF_WIN_DEVICES, Icon_ConfDev_FileRsPath}},
  {Rsrc_PrinterFile, CONFIG_STRING, {CONF_WIN_DEVICES, Icon_ConfDev_FilePrPath}},
  {Rsrc_PetCrt, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetCrt}},
  {Rsrc_PetRAM9, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetRAM9}},
  {Rsrc_PetRAMA, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetRAMA}},
  {Rsrc_PetDiag, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetDiagPin}},
  {Rsrc_PetSuper, CONFIG_SELECT, {CONF_WIN_PET, Icon_ConfPET_PetSuper}},
  {Rsrc_Key8040, CONFIG_SELECT, {CONF_WIN_C128, Icon_Conf128_C1284080}},
  {Rsrc_VDCpalette, CONFIG_STRING, {CONF_WIN_C128, Icon_Conf128_C128Palette}},
  {Rsrc_VDCsize, CONFIG_SELECT, {CONF_WIN_C128, Icon_Conf128_C128Size}},
  {Rsrc_Z80Bios, CONFIG_STRING, {CONF_WIN_C128, Icon_Conf128_C128z80bios}},
  {Rsrc_VDCdblsze, CONFIG_SELECT, {CONF_WIN_C128, Icon_Conf128_C128dblsize}},
  {Rsrc_VDCdblscn, CONFIG_SELECT, {CONF_WIN_C128, Icon_Conf128_C128dblscan}},
  {Rsrc_ReadOnly8, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveRdOnly8}},
  {Rsrc_ReadOnly9, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveRdOnly9}},
  {Rsrc_ReadOnly10, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveRdOnly10}},
  {Rsrc_ReadOnly11, CONFIG_SELECT, {CONF_WIN_DRIVES, Icon_ConfDrv_DriveRdOnly11}},
  {Rsrc_PALDouble, CONFIG_SELECT, {CONF_WIN_VIDEO, Icon_ConfVid_PALDouble}},
  {Rsrc_ColourSat, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_ColourSat}},
  {Rsrc_Contrast, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_Contrast}},
  {Rsrc_Brightness, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_Brightness}},
  {Rsrc_Gamma, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_Gamma}},
  {Rsrc_LineShade, CONFIG_INT, {CONF_WIN_VIDEO, Icon_ConfVid_LineShade}},
  {NULL, 0, {0, 0}}
};
