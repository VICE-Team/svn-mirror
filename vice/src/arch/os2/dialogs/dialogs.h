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

#ifndef DIALOGS_H
#define DIALOGS_H

#ifdef WATCOM_COMPILE
#include "vice.h" // HAVE_*
#endif

//
//  --------------- Resource Definitions ----------------
//
#ifdef WATCOM_COMPILE
#define IDM_VICE2          0x0fff
#else
#define IDM_VICE2          0x0001
#endif
//#define PTR_VICE2        0x0002
#define DLG_DRIVE          0x1019
#define DLG_ABOUT          0x1050
#define DLG_COLOR          0x1051
#define DLG_CRT            0x2051
#define DLG_DATASETTE      0x1060
#define DLG_IDE64          0x2060
#define DLG_EMULATOR       0x1090
#define DLG_MONITOR        0x10b0
#define DLG_MONREG         0x10b1
#define DLG_MONDIS         0x10b2
#define DLG_CONTENTS       0x10c0
#define DLG_CMDOPT         0x10d0
#define DLG_LOGGING        0x10e0
#define DLG_VSID           0x10f0
#define DLG_FSMODES        0x1100
#define DLG_JOYSTICK       0x1400
#define DLG_CALIBRATE      0x1405
#define DLG_KEYSET         0x140f
#define DLG_EXTRA_JOYSTICK 0x1500
#define DLG_FILEIO         0x2000

#define ID_NONE        -1
#define DID_CLOSE      DID_OK

#define PTR_DRAGOK     0x100
#define PTR_INFO       0x101
#define PTR_SKULL      0x102
#define PTR_NOTE       0x103

// Monitor Dialog
#define WM_INSERT      WM_USER+0x1

// Contents Dialog
#define LB_CONTENTS    0x10

// Commandline option Dialog
#define LB_CMDOPT      0x10

// Video modes Dialog
#define LB_FSMODES     0x10

// Logging Dialog
#define LB_LOG         0x10

//
//  ------------- My Styles -------------
//
#define STY_STD             WS_TABSTOP | WS_VISIBLE

#define STY_CLOSEBUTTON     WC_BUTTON, BS_PUSHBUTTON | STY_STD | BS_DEFAULT
#define STY_GROUPBOX        WC_STATIC, SS_GROUPBOX  | WS_GROUP | WS_VISIBLE
#define STY_PUSHBUTTON      WC_BUTTON, BS_PUSHBUTTON | STY_STD
#define STY_AUTORADIOBUTTON WC_BUTTON, BS_AUTORADIOBUTTON | STY_STD
#define STY_AUTOCHECKBOX    WC_BUTTON, BS_AUTOCHECKBOX | STY_STD
#define STY_SPINBUTTON      WC_SPINBUTTON, SPBS_MASTER | SPBS_JUSTRIGHT | SPBS_NUMERICONLY | SPBS_FASTSPIN | SPBS_PADWITHZEROS | STY_STD
#define STY_DIALOG          FS_NOBYTEALIGN | FS_SCREENALIGN | FS_DLGBORDER | WS_CLIPSIBLINGS | WS_SAVEBITS | WS_ANIMATE | WS_VISIBLE, FCF_TITLEBAR | FCF_SYSMENU
#define STY_STATUSDLG       WC_SPINBUTTON, SPBS_JUSTCENTER  | SPBS_NOBORDER | SPBS_NUMERICONLY | SPBS_READONLY | SPBS_PADWITHZEROS  | WS_VISIBLE
#define STY_DISPVAL         WC_SPINBUTTON, SPBS_JUSTCENTER  | SPBS_PADWITHZEROS  | WS_VISIBLE
#define STY_TEXT            WC_STATIC, SS_TEXT | SS_AUTOSIZE | DT_LEFT | DT_TOP | WS_VISIBLE
#define TEXT(txt,x,y,w,h)   CONTROL txt, ID_NONE, x, y, 0xFFFF, 0xFFFF, STY_TEXT

//
// ----------------- Win*-Macros -----------------------                                                    */
//

#define WinIsDlgEnabled(hwnd, id)                           WinIsWindowEnabled(WinWindowFromID(hwnd, id))
#define WinSendDlgMsg(hwnd, id, msg, mp1, mp2)              WinSendMsg(WinWindowFromID(hwnd, id), msg, (MPARAM)(mp1), (MPARAM)(mp2))
#define WinSetDlgPos(hwnd, id, d, x, y, cx, cy, swp)        WinSetWindowPos(WinWindowFromID(hwnd, id), d, x, y, cx, cy, swp)
#define WinCreateStdDlg(hwnd, id, d0, d1, d2, x, y, cx, cy) WinCreateWindow(hwnd, d0, d2, d1, x, y, cx, cy, NULLHANDLE, HWND_TOP, id, NULL, NULL)

//
// ---------------- List Box Macros ------------------
//
#define WinSetDlgLboxItemText(hwnd, id, index, psz)      WinSendDlgMsg(hwnd, id, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz))
#define WinLboxDeselectItem(hwnd, id, index)             WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, FALSE);
#define WinLboxInsertItemAt(hwnd, id, psz, pos)          WinInsertLboxItem(WinWindowFromID(hwnd, id), pos, psz)
#define WinLboxQueryItem(hwnd, id, pos, psz, max)        WinQueryLboxItemText(WinWindowFromID(hwnd, id), pos, psz, max)
#define WinLboxQuerySelectedItemText(hwnd, id, psz, max) WinLboxQueryItem(hwnd, id, WinQueryLboxSelectedItem(WinWindowFromID(hwnd, id)), psz, max)
#define WinQueryDlgText(hwnd, id, psz, max)              WinQueryWindowText(WinWindowFromID(hwnd, id), max, psz)

//
// ---------------- Resource funtions ------------------
//

extern HWND hwndLog;

extern int toggle(const char *resource_name);
extern void ViceErrorDlg(HWND hwnd, int id, char *text);

//
// ---------------- dialog definitions ------------------
//
extern void about_dialog(HWND hwnd);
extern void contents_dialog(HWND hwnd, char *szFullFile);
extern void create_dialog(HWND hwnd);
extern HWND cmdopt_dialog(HWND hwnd);
extern HWND fsmodes_dialog(HWND hwnd);
extern void log_dialog(int state);

extern void hardreset_dialog (HWND hwnd);
extern void softreset_dialog (HWND hwnd);

extern int  isEmulatorPaused(void);
extern void emulator_pause(void);
extern void emulator_resume(void);

#endif
