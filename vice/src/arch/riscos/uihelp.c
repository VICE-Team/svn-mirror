/*
 * uihelp.c - RISC OS interactive help data structures.
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


#include <stdlib.h>

#include <wimp.h>

#include "lib.h"
#include "machine.h"
#include "ui.h"
#include "uiconfig.h"
#include "uihelp.h"
#include "uimsgwin.h"
#include "uisharedef.h"
#include "videoarch.h"



/*
 *  Help text. sym is the symbol to look up; if sym is NULL, it's the same as the
 *  previous entry.
 */

/* Emu pane */
static help_icon_t Help_EmuPane[] = {
  {-1, "\\HelpPaneWindow"},
  {Icon_Pane_LED0, "\\HelpPaneDrives|M\\HelpPaneDrv0"},
  {Icon_Pane_Drive0, NULL},
  {Icon_Pane_LED1, "\\HelpPaneDrives"},
  {Icon_Pane_Drive1, NULL},
  {Icon_Pane_LED2, NULL},
  {Icon_Pane_Drive2, NULL},
  {Icon_Pane_LED3, NULL},
  {Icon_Pane_Drive3, NULL},
  {Icon_Pane_Reset, "\\HelpPaneReset"},
  {Icon_Pane_Pause, "\\HelpPanePause"},
  {Icon_Pane_Speed, "\\HelpPaneSpeed"},
  {Icon_Pane_Toggle, "\\HelpPaneToggle"},
  {Icon_Pane_TrkSec, "\\HelpPaneTrkSec"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_EmuWindow[] = {
  {-1, "\\HelpEmuWindow"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_InfoWindow[] = {
  {-1, "\\HelpInfoWindow"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ImgContWindow[] = {
  {-1, "\\HelpImageContents"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_CreateWindow[] = {
  {-1, "\\HelpCreateWindow"},
  {Icon_Create_Type, "\\HelpCreateType"},
  {Icon_Create_TypeT, "\\HelpCreateTypeT"},
  {Icon_Create_Name, "\\HelpCreateName"},
  {Icon_Create_Sprite, "\\HelpCreateSprite"},
  {Icon_Create_OK, "\\HelpCreateOK"},
  {Icon_Create_File, "\\HelpCreateFile"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_MonitorWindow[] = {
  {-1, "\\HelpMonitorWindow"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigDrives[] = {
  {-1, "\\HelpConfDrive"},
  {Icon_ConfDrv_DriveType8, "\\HelpConfDrvType"},
  {Icon_ConfDrv_DriveType9, NULL},
  {Icon_ConfDrv_DriveType10, NULL},
  {Icon_ConfDrv_DriveType11, NULL},
  {Icon_ConfDrv_DriveFile8, "\\HelpConfDrvFile"},
  {Icon_ConfDrv_DriveFile9, NULL},
  {Icon_ConfDrv_DriveFile10, NULL},
  {Icon_ConfDrv_DriveFile11, NULL},
  {Icon_ConfDrv_DriveRdOnly8, "\\HelpConfDrvRO"},
  {Icon_ConfDrv_DriveRdOnly9, NULL},
  {Icon_ConfDrv_DriveRdOnly10, NULL},
  {Icon_ConfDrv_DriveRdOnly11, NULL},
  {Icon_ConfDrv_TrueDrv, "\\HelpConfDrvTrue"},
  {Icon_ConfDrv_TrueDrvPar8, "\\HelpConfDrvPar"},
  {Icon_ConfDrv_TrueDrvPar9, NULL},
  {Icon_ConfDrv_TrueDrvExt8, "\\HelpConfDrvExt"},
  {Icon_ConfDrv_TrueDrvExt9, NULL},
  {Icon_ConfDrv_TrueDrvExt8T, "\\HelpConfDrvExtT"},
  {Icon_ConfDrv_TrueDrvExt9T, NULL},
  {Icon_ConfDrv_TrueDrvIdle8, "\\HelpConfDrvIdle"},
  {Icon_ConfDrv_TrueDrvIdle9, NULL},
  {Icon_ConfDrv_TrueDrvIdle8T, "\\HelpConfDrvIdleT"},
  {Icon_ConfDrv_TrueDrvIdle9T, NULL},
  {Icon_ConfDrv_TrueDrvType8, "\\HelpConfDrvTT"},
  {Icon_ConfDrv_TrueDrvType9, NULL},
  {Icon_ConfDrv_TrueDrvType8T, "\\HelpConfDrvTTT"},
  {Icon_ConfDrv_TrueDrvType9T, NULL},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigTape[] = {
  {-1, "\\HelpConfTape"},
  {Icon_ConfTap_TapeFile, "\\HelpConfTapeFile"},
  {Icon_ConfTap_TapeDetach, "\\HelpConfTapeDet"},
  {Icon_ConfTap_DataDoReset, "\\HelpConfTapeRst"},
  {Icon_ConfTap_DataCounter, "\\HelpConfTapeCtr"},
  {Icon_ConfTap_DataStop, "\\HelpConfTapeStop"},
  {Icon_ConfTap_DataRewind, "\\HelpConfTapeRwd"},
  {Icon_ConfTap_DataPlay, "\\HelpConfTapePlay"},
  {Icon_ConfTap_DataForward, "\\HelpConfTapeFwd"},
  {Icon_ConfTap_DataRecord, "\\HelpConfTapeRec"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigDevices[] = {
  {-1, "\\HelpConfDevice"},
  {Icon_ConfDev_ACIAIrq, "\\HelpConfDevACIAIrq"},
  {Icon_ConfDev_ACIADev, "\\HelpConfDevACIADev|M\\HelpConfDeviceDev"},
  {Icon_ConfDev_ACIADevT, "\\HelpConfDevACIADevT"},
  {Icon_ConfDev_ACIADE, "\\HelpConfDevACIADE"},
  {Icon_ConfDev_ACIAD67, "\\HelpConfDevACIA67"},
  {Icon_ConfDev_RsUsr, "\\HelpConfDevRsUsr"},
  {Icon_ConfDev_RsUsrDev, "\\HelpConfDevRsUsrDev|M\\HelpConfDeviceDev"},
  {Icon_ConfDev_RsUsrDevT, "\\HelpConfDevRsUsrDevT"},
  {Icon_ConfDev_Serial, "\\HelpConfDevSerial"},
  {Icon_ConfDev_SerialT, "\\HelpConfDevSerialT"},
  {Icon_ConfDev_FileRsOK, "\\HelpConfDevOK"},
  {Icon_ConfDev_FilePrOK, NULL},
  {Icon_ConfDev_FileRsPath, "\\HelpConfDevPath"},
  {Icon_ConfDev_FilePrPath, NULL},
  {Icon_ConfDev_FileRsIcon, "\\HelpConfDevIcon"},
  {Icon_ConfDev_FilePrIcon, NULL},
  {Icon_ConfDev_PrntOn, "\\HelpConfDevPrntOn"},
  {Icon_ConfDev_PrntDev, "\\HelpConfDevPrntDev|M\\HelpConfDeviceDev"},
  {Icon_ConfDev_PrntDevT, "\\HelpConfDevPrntDevT"},
  {Icon_ConfDev_PrntUsrOn, "\\HelpConfDevPrntUsrOn"},
  {Icon_ConfDev_PrntUsrDev, "\\HelpConfDevPrntUsrDev|M\\HelpConfDeviceDev"},
  {Icon_ConfDev_PrntUsrDevT, "\\HelpConfDevPrntUsrDevT"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigSound[] = {
  {-1, "\\HelpConfSound"},
  {Icon_ConfSnd_SoundOn, "\\HelpConfSndOn"},
  {Icon_ConfSnd_SampleRate, "\\HelpConfSndRate"},
  {Icon_ConfSnd_SampleRateT, "\\HelpConfSndRateT"},
  {Icon_ConfSnd_SoundDev, "\\HelpConfSndDev"},
  {Icon_ConfSnd_SoundDevT, "\\HelpConfSndDevT"},
  {Icon_ConfSnd_Oversample, "\\HelpConfSndOver"},
  {Icon_ConfSnd_OversampleT, "\\HelpConfSndOverT"},
  {Icon_ConfSnd_SidModel, "\\HelpConfSndMod"},
  {Icon_ConfSnd_SidModelT, "\\HelpConfSndModT"},
  {Icon_ConfSnd_SidFilter, "\\HelpConfSndFilter"},
  {Icon_ConfSnd_FileSndOK, "\\HelpConfSndOK"},
  {Icon_ConfSnd_FileSndPath, "\\HelpConfSndFile"},
  {Icon_ConfSnd_FileSndIcon, "\\HelpConfSndIcon"},
  {Icon_ConfSnd_Volume, "\\HelpConfSndVol"},
  {Icon_ConfSnd_SoundBuff, "\\HelpConfSndBuff"},
  {Icon_ConfSnd_SoundBuffT, "\\HelpConfSndBuffT"},
  {Icon_ConfSnd_SidEngine, "\\HelpConfSndSIDEngine"},
  {Icon_ConfSnd_SpeedAdjust, "\\HelpConfSndAdj"},
  {Icon_ConfSnd_SpeedAdjustT, "\\HelpConfSndAdjT"},
  {Icon_ConfSnd_ResidSamp, "\\HelpConfSndReSamp"},
  {Icon_ConfSnd_ResidSampT, "\\HelpConfSndReSampT"},
  {Icon_ConfSnd_ResidPass, "\\HelpConfSndRePass"},
  {Icon_ConfSnd_Sound16Bit, "\\HelpConfSnd16Bit"},
  {Icon_ConfSnd_SidStereo, "\\HelpConfSndStereo"},
  {Icon_ConfSnd_Sid2Addr, "\\HelpConfSndSid2Addr"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigSystem[] = {
  {-1, "\\HelpConfSystem"},
  {Icon_ConfSys_CharGen, "\\HelpConfSysCharGen|M\\HelpConfSystemPath"},
  {Icon_ConfSys_Kernal, "\\HelpConfSysKernal|M\\HelpConfSystemPath"},
  {Icon_ConfSys_Basic, "\\HelpConfSysBasic|M\\HelpConfSystemPath"},
  {Icon_ConfSys_REU, "\\HelpConfSysREU"},
  {Icon_ConfSys_IEEE488, "\\HelpConfSysIEEE"},
  {Icon_ConfSys_EmuID, "\\HelpConfSysID"},
  {Icon_ConfSys_NoTraps, "\\HelpConfSysTraps"},
  {Icon_ConfSys_PollEvery, "\\HelpConfSysPlEv"},
  {Icon_ConfSys_SpeedEvery, "\\HelpConfSysSpEv"},
  {Icon_ConfSys_SoundEvery, "\\HelpConfSysSndEv"},
  {Icon_ConfSys_SpeedLmt, "\\HelpConfSysSpdLmt"},
  {Icon_ConfSys_SpeedLmtT, "\\HelpConfSysSpdLmtT"},
  {Icon_ConfSys_WarpMode, "\\HelpConfSysWarpMode"},
  {Icon_ConfSys_CartType, "\\HelpConfSysCrtType|M\\HelpConfSystemCart"},
  {Icon_ConfSys_CartTypeT, "\\HelpConfSysCrtTypeT"},
  {Icon_ConfSys_CartFile, "\\HelpConfSysCrtFile|M\\HelpConfSystemCart"},
  {Icon_ConfSys_CheckSScoll, "\\HelpConfSysSScl"},
  {Icon_ConfSys_CheckSBcoll, "\\HelpConfSysSBcl"},
  {Icon_ConfSys_DosName, "\\HelpConfSysDosName"},
  {Icon_ConfSys_DosNameT, "\\HelpConfSysDosNameT"},
  {Icon_ConfSys_DosNameF, "\\HelpConfSysDosNameF|M\\HelpConfSystemPath"},
  {Icon_ConfSys_AutoPause, "\\HelpConfSysAutoPs"},
  {Icon_ConfSys_ROMSet, "\\HelpConfSysROMSet"},
  {Icon_ConfSys_ROMSetT, "\\HelpConfSysROMSetT"},
  {Icon_ConfSys_ROMAction, "\\HelpConfSysROMAct"},
  {Icon_ConfSys_Keyboard, "\\HelpConfSysKbd"},
  {Icon_ConfSys_KeyboardT, "\\HelpConfSysKbdT"},
  {Icon_ConfSys_VideoSync, "\\HelpConfSysVsync"},
  {Icon_ConfSys_VideoSyncT, "\\HelpConfSysVsyncT"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigVideo[] = {
  {-1, "\\HelpConfVideo"},
  {Icon_ConfVid_VCache, "\\HelpConfVidVCache"},
  {Icon_ConfVid_Refresh, "\\HelpConfVidRefresh"},
  {Icon_ConfVid_RefreshT, "\\HelpConfVidRefreshT"},
  {Icon_ConfVid_MaxSkipFrms, "\\HelpConfVidMaxSkip"},
  {Icon_ConfVid_FullScrNorm, "\\HelpConfVidFullScrNorm|M\\HelpConfVidFullScrFormat"},
  {Icon_ConfVid_FullScrPAL, "\\HelpConfVidFullScrPAL|M\\HelpConfVidFullScrFormat"},
  {Icon_ConfVid_FullScrDbl, "\\HelpConfVidFullScrDbl|M\\HelpConfVidFullScrFormat"},
  {Icon_ConfVid_SetPalette, "\\HelpConfVidSetPal"},
  {Icon_ConfVid_UseBPlot, "\\HelpConfVidUseBPlot"},
  {Icon_ConfVid_Palette, "\\HelpConfVidPal|M\\HelpConfSystemPath"},
  {Icon_ConfVid_ExtPal, "\\HelpConfVidExtPal"},
  {Icon_ConfVid_PALDepth, "\\HelpConfVidPALDepth"},
  {Icon_ConfVid_PALDepthT, "\\HelpConfVidPALDepthT"},
  {Icon_ConfVid_PALDouble, "\\HelpConfVidPALDouble"},
  {Icon_ConfVid_ColourSat, "\\HelpConfVidColourSat"},
  {Icon_ConfVid_Contrast, "\\HelpConfVidContrast"},
  {Icon_ConfVid_Brightness, "\\HelpConfVidBrightness"},
  {Icon_ConfVid_Gamma, "\\HelpConfVidGamma"},
  {Icon_ConfVid_LineShade, "\\HelpConfVidLineShade"},
  {Icon_ConfVid_PalMode, "\\HelpConfVidPalMode"},
  {Icon_ConfVid_PalModeT, "\\HelpConfVidPalModeT"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigJoystick[] = {
  {-1, "\\HelpConfJoystick"},
  {Icon_ConfJoy_JoyPort1, "\\HelpConfJoyPort"},
  {Icon_ConfJoy_JoyPort2, NULL},
  {Icon_ConfJoy_JoyPort1T, "\\HelpConfJoyPortT"},
  {Icon_ConfJoy_JoyPort2T, NULL},
  {Icon_ConfJoy_JoyKey1U, "\\HelpConfJoyKeymap"},
  {Icon_ConfJoy_JoyKey1D, NULL},
  {Icon_ConfJoy_JoyKey1L, NULL},
  {Icon_ConfJoy_JoyKey1R, NULL},
  {Icon_ConfJoy_JoyKey1F, NULL},
  {Icon_ConfJoy_JoyKey2U, NULL},
  {Icon_ConfJoy_JoyKey2D, NULL},
  {Icon_ConfJoy_JoyKey2L, NULL},
  {Icon_ConfJoy_JoyKey2R, NULL},
  {Icon_ConfJoy_JoyKey2F, NULL},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigMachinePET[] = {
  {-1, "\\HelpConfPET"},
  {Icon_ConfPET_PetMem, "\\HelpConfPETMem"},
  {Icon_ConfPET_PetMemT, "\\HelpConfPETMemT"},
  {Icon_ConfPET_PetIO, "\\HelpConfPETIO"},
  {Icon_ConfPET_PetIOT, "\\HelpConfPETIOT"},
  {Icon_ConfPET_PetVideo, "\\HelpConfPETVideo"},
  {Icon_ConfPET_PetVideoT, "\\HelpConfPETVideoT"},
  {Icon_ConfPET_PetModel, "\\HelpConfPETModel"},
  {Icon_ConfPET_PetModelT, "\\HelpConfPETModelT"},
  {Icon_ConfPET_PetKbd, "\\HelpConfPETKbd"},
  {Icon_ConfPET_PetCrt, "\\HelpConfPETCrt"},
  {Icon_ConfPET_PetRAM9, "\\HelpConfPETRAM9"},
  {Icon_ConfPET_PetRAMA, "\\HelpConfPETRAMA"},
  {Icon_ConfPET_PetDiagPin, "\\HelpConfPETDiag"},
  {Icon_ConfPET_PetSuper, "\\HelpConfPETSuper"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigMachineVIC[] = {
  {-1, "\\HelpConfVIC"},
  {Icon_ConfVIC_VICCart, "\\HelpConfVICCart"},
  {Icon_ConfVIC_VICCartT, "\\HelpConfVICCartT"},
  {Icon_ConfVIC_VICCartF, "\\HelpConfVICCartF"},
  {Icon_ConfVIC_VICMem, "\\HelpConfVICMem"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigMachineCBM2[] = {
  {-1, "\\HelpConfCBM2"},
  {Icon_ConfCBM_CBM2Line, "\\HelpConfCBMLine"},
  {Icon_ConfCBM_CBM2LineT, "\\HelpConfCBMLineT"},
  {Icon_ConfCBM_CBM2Mem, "\\HelpConfCBMMem"},
  {Icon_ConfCBM_CBM2MemT, "\\HelpConfCBMMemT"},
  {Icon_ConfCBM_CBM2Model, "\\HelpConfCBMModel"},
  {Icon_ConfCBM_CBM2ModelT, "\\HelpConfCBMModelT"},
  {Icon_ConfCBM_CBM2RAM, "\\HelpConfCBMRAM"},
  {Icon_ConfCBM_CBM2Kbd, "\\HelpConfCBMKbd"},
  {Icon_ConfCBM_CBM2Cart, "\\HelpConfCBMCart"},
  {Icon_ConfCBM_CBM2CartT, "\\HelpConfCBMCartT"},
  {Icon_ConfCBM_CBM2CartF, "\\HelpConfCBMCartF"},
  {Help_Icon_End, NULL}
};

static help_icon_t Help_ConfigMachineC128[] = {
  {-1, "\\HelpConfC128"},
  {Icon_Conf128_C128Palette, "\\HelpConf128Pal|M\\HelpConfC128Path"},
  {Icon_Conf128_C128Size, "\\HelpConf128Size"},
  {Icon_Conf128_C1284080, "\\HelpConf1284080"},
  {Icon_Conf128_C128z80bios, "\\HelpConf128z80bios|M\\HelpConfC128Path"},
  {Icon_Conf128_C128dblsize, "\\HelpConf128dsize"},
  {Icon_Conf128_C128dblscan, "\\HelpConf128dscan"},
  {Help_Icon_End, NULL}
};


/* must be kept in sync with config window numbers */
static help_icon_t *Help_ConfigWindows[CONF_WIN_NUMBER] = {
  Help_ConfigDrives,
  Help_ConfigTape,
  Help_ConfigDevices,
  Help_ConfigSound,
  Help_ConfigSystem,
  Help_ConfigVideo,
  Help_ConfigJoystick,
  Help_ConfigMachinePET,
  Help_ConfigMachineVIC,
  Help_ConfigMachineCBM2,
  Help_ConfigMachineC128,
};



/*
 *  Help text code
 */

#define HELPBUFFSIZE	512

void ui_translate_icon_help_msgs(const wimp_msg_desc *msg, help_icon_t *hi)
{
  char buffer[HELPBUFFSIZE];
  unsigned int i;

  /*log_message(LOG_DEFAULT, "Translate interactive help messages...\n");*/
  for (i=0; hi[i].icon != Help_Icon_End; i++)
  {
    hi[i].msg = NULL;
    if (hi[i].sym != NULL)
    {
      wimp_message_translate_string(msg, hi[i].sym, buffer, HELPBUFFSIZE);
      if ((hi[i].msg = (char*)lib_malloc(strlen(buffer)+1)) != NULL)
      {
        strcpy(hi[i].msg, buffer);
        /*log_message(LOG_DEFAULT, "%s = %s\n", hi[i].sym, hi[i].msg);*/
      }
    }
  }
}

void ui_translate_help_messages(const wimp_msg_desc *msg)
{
  unsigned int i;

  ui_translate_icon_help_msgs(msg, Help_EmuPane);
  ui_translate_icon_help_msgs(msg, Help_EmuWindow);
  ui_translate_icon_help_msgs(msg, Help_InfoWindow);
  ui_translate_icon_help_msgs(msg, Help_ImgContWindow);
  ui_translate_icon_help_msgs(msg, Help_CreateWindow);
  ui_translate_icon_help_msgs(msg, Help_MonitorWindow);

  for (i=0; i<CONF_WIN_NUMBER; i++)
  {
    if (Help_ConfigWindows[i] != NULL)
      ui_translate_icon_help_msgs(msg, Help_ConfigWindows[i]);
  }
}

const char *ui_get_help_for_window_icon(int handle, int icon)
{
  const help_icon_t *hi = NULL;

  if (canvas_for_handle(handle) != NULL)
  {
    hi = Help_EmuWindow;
  }
  else if (handle == EmuPane->Handle)
  {
    hi = Help_EmuPane;
  }
  else if (handle == InfoWindow->Handle)
  {
    hi = Help_InfoWindow;
  }
  else if (handle == ImgContWindow->Handle)
  {
    hi = Help_ImgContWindow;
  }
  else if (handle == CreateDiscWindow->Handle)
  {
    hi = Help_CreateWindow;
  }
  else if (ui_message_window_for_handle(handle) == msg_win_monitor)
  {
    hi = Help_MonitorWindow;
  }
  else if (ViceMachineCallbacks.help_for_window_icon != NULL)
  {
    hi = ViceMachineCallbacks.help_for_window_icon(handle, icon);
  }

  if (hi == NULL)
  {
    unsigned int i;

    for (i=0; i<CONF_WIN_NUMBER; i++)
    {
      if ((ConfWindows[i] != NULL) && (handle == ConfWindows[i]->Handle))
      {
        hi = Help_ConfigWindows[i];
      }
    }
  }

  if (hi != NULL)
  {
    unsigned int i;
    const char *msg=NULL;
    const char *winmsg=NULL;

    for (i=0; hi[i].icon != Help_Icon_End; i++)
    {
      if (hi[i].sym != NULL)
      {
        /* help for entire window? */
        if (hi[i].icon == -1)
          winmsg = hi[i].msg;

        msg = hi[i].msg;
      }

      if (hi[i].icon == icon)
        break;
    }

    /* no exact match found? */
    if (hi[i].icon == Help_Icon_End)
      msg = NULL;

    /* if no exact match then use the help text for the entire window */
    if (msg == NULL)
      msg = winmsg;

    return msg;
  }
  return NULL;
}
