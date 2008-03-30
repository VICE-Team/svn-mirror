/*
 * dialogs.h - The dialog windows.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#ifndef _DIALOGS_H
#define _DIALOGS_H

#include <os2.h>
#include "config.h"

#define ID_NONE        -1
#define DID_CLOSE      DID_OK

// Sound Dialog
#define DLG_SOUND      0x1010
#define CB_SOUND       0x1011
#define CS_VOLUME      0x1012
#define SPB_BUFFER     0x1013
#define CB_SIDFILTER   0x1014
#ifdef HAVE_RESID
  #define CB_RESID     0x1015
#endif
#define RB_8000HZ      0x1F40
#define RB_11025HZ     0x2B11
#define RB_22050HZ     0x5622
#define RB_44100HZ     0xAC44
#define RB_OFF         0x1030
#define RB_2X          0x1031
#define RB_4X          0x1032
#define RB_8X          0x1033
#define RB_6581        0x1040
#define RB_8580        0x1041

// Drive Dialog
#define DLG_DRIVE      0x1019
#define RB_DRIVE8      0x1020
#define RB_DRIVE9      0x1021
#define RB_DRIVE10     0x1022
#define RB_DRIVE11     0x1023
#define CB_TRUEDRIVE   0x1024
#define RB_PAL         0x1025
#define RB_NTSC        0x1026

#define CBS_IMAGE      0x1033
#define CBS_TYPE       0x1034
#define CB_PARALLEL    0x1035

#define RB_NONE        0x1030
#define RB_SKIP        0x1031
#define RB_TRAP        0x1032

#define RB_NEVER       0x1040
#define RB_ASK         0x1041
#define RB_ALWAYS      0x1042

#define CB_ALLOWACCESS 0x1036
#define CB_CONVERTP00  0x1037
#define CB_SAVEP00     0x1038
#define CB_HIDENONP00  0x1039
#define PB_ATTACH      0x103a
#define PB_DETACH      0x103b
#define PB_FLIP        0x103c
#define WM_SWITCH      WM_USER+0x1
#define WM_DRIVEIMAGE  WM_USER+0x2

// About Dialog
#define DLG_ABOUT      0x1050

// Datasette Dialog
#define DLG_DATASETTE  0x1060
#define PB_STOP        0x1070
#define PB_START       0x1071
#define PB_FORWARD     0x1072
#define PB_REWIND      0x1073
#define PB_RECORD      0x1074
#define PB_RESET       0x1075

#ifdef HAS_JOYSTICK

// Joystick Dialog
#define DLG_JOYSTICK   0x1000
#define CB_JOY11       0x111
#define CB_JOY12       0x211
#define CB_JOY21       0x112
#define CB_JOY22       0x212
#define ID_SWAP        0x110
#define ID_CALIBRATE   0x113

// Calibrate Dialog
#define DLG_CALIBRATE  0x1080
#define RB_JOY1        0x1081
#define RB_JOY2        0x1082
#define SPB_UP         0x1083
#define SPB_DOWN       0x1084
#define SPB_LEFT       0x1085
#define SPB_RIGHT      0x1086
#define ID_START       0x1087
#define ID_STOP        0x1088
#define ID_RESET       0x1089
#define WM_SETJOY      WM_USER+0x1
#define WM_PROCESS     WM_USER+0x2
#define WM_FILLSPB     WM_USER+0x3
#define WM_ENABLECTRL  WM_USER+0x4
#endif

// Emulator Dialog
#define DLG_EMULATOR   0x1090
#define SPB_SPEED      0x1091
#define PB_SPEED100    0x1092
#define CBS_REFRATE    0x1093
#define CB_PAUSE       0x1094
#define CB_VCACHE      0x1095
#define CB_SBCOLL      0x1096
#define CB_SSCOLL      0x1097

// Monitor Dialog
#define DLG_MONITOR    0x10a0
#define LB_MONOUT      0x10a1
#define EF_MONIN       0x10a2
#define WM_INSERT      WM_USER+0x1
#define WM_INPUT       WM_USER+0x2

// Contents Dialog
#define DLG_CONTENTS   0x10b0
#define LB_CONTENTS    0x10b1

/* WinPM-Macros                                                     */
/*----------------------------------------------------------------- */

#define WinIsDlgEnabled(hwnd, id) \
    WinIsWindowEnabled(WinWindowFromID(hwnd, id))
#define WinSendDlgMsg(hwnd, id, msg, mp1, mp2) \
    WinSendMsg(WinWindowFromID(hwnd, id), msg, (MPARAM)mp1, (MPARAM)mp2)
#define WinSetDlgLboxItemText(hwnd, id, index, psz) \
    WinSendDlgMsg(hwnd, id, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz))
#define WinSetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_SETCURRENTVALUE, (MPARAM)val,(MPARAM)0)
#define WinGetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_QUERYVALUE, (MPARAM)val, (MPARAM)0)
#define WinLboxSelectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, TRUE);
#define WinLboxDeselectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, FALSE);
#define WinLboxInsertItem(hwnd, id, psz) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), LIT_END, psz)
#define WinLboxInsertItemAt(hwnd, id, psz, pos) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), pos, psz)
#define WinLboxQuerySelectedItem(hwnd, id) \
    WinQueryLboxSelectedItem (WinWindowFromID(hwnd, id))
#define WinLboxQueryCount(hwnd, id) \
    WinQueryLboxCount(WinWindowFromID(hwnd, id))
#define WinLboxQueryItem(hwnd, id, pos, psz, max) \
    WinQueryLboxItemText(WinWindowFromID(hwnd, id), pos, psz, max)
#define WinLboxDeleteItem(hwnd, id, pos) \
    WinDeleteLboxItem(WinWindowFromID(hwnd, id), pos)
#define WinSetDlgFont(hwnd, id, font) \
    WinSetPresParam(WinWindowFromID(hwnd, id), PP_FONTNAMESIZE, strlen(font)+1,font);
#define WinLboxQuerySelectedItemText(hwnd, id, psz, max) \
    WinLboxQueryItem(hwnd, id, WinLboxQuerySelectedItem(hwnd, id), psz, max)

/* Is-this-dialog-open handling                                     */
/*----------------------------------------------------------------- */

#define DLGO_SOUND      0x001
#define DLGO_JOYSTICK   0x002
#define DLGO_DRIVE      0x004
#define DLGO_ABOUT      0x008
#define DLGO_DATASETTE  0x010
#define DLGO_CALIBRATE  0x020
#define DLGO_EMULATOR   0x040
#define DLGO_MONITOR    0x080
#define DLGO_CONTENTS   0x100

extern int dlgOpen(int dlg);
extern void setDlgOpen(int dlg);
extern void delDlgOpen(int dlg);

/* Resource funtions                                                */
/*----------------------------------------------------------------- */

extern int toggle(char *resource_name);

/* Dialog-Function Prototypes                                       */
/*----------------------------------------------------------------- */

extern void drive_dialog     (HWND hwnd);
extern void sound_dialog     (HWND hwnd);
extern void about_dialog     (HWND hwnd);
extern void datasette_dialog (HWND hwnd);
extern void emulator_dialog  (HWND hwnd);
extern HWND monitor_dialog   (HWND hwnd);
extern void contents_dialog  (HWND hwnd, char *szFullFile);

#ifdef HAS_JOYSTICK
extern void joystick_dialog  (HWND hwnd);
extern void calibrate_dialog (HWND hwnd);
#endif

#endif

