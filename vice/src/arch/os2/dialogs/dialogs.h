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

#include "config.h" // HAS_*

//
//  ------------- My Styles -------------
//
#define STY_STD               WS_TABSTOP | WS_VISIBLE

#define STY_CLOSEBUTTON       WC_BUTTON, BS_PUSHBUTTON      | STY_STD | BS_DEFAULT
#define STY_GROUPBOX          WC_STATIC, SS_GROUPBOX        | WS_GROUP | WS_VISIBLE
#define STY_PUSHBUTTON        WC_BUTTON, BS_PUSHBUTTON      | STY_STD
#define STY_AUTORADIOBUTTON   WC_BUTTON, BS_AUTORADIOBUTTON | STY_STD
#define STY_AUTOCHECKBOX      WC_BUTTON, BS_AUTOCHECKBOX    | STY_STD
#define STY_SPINBUTTON        WC_SPINBUTTON, SPBS_MASTER | SPBS_JUSTRIGHT | \
                              SPBS_NUMERICONLY | SPBS_FASTSPIN | \
                              SPBS_PADWITHZEROS | STY_STD
#define STY_DIALOG            FS_NOBYTEALIGN | FS_SCREENALIGN | FS_DLGBORDER | \
                              WS_CLIPSIBLINGS | WS_SAVEBITS | WS_VISIBLE, \
                              FCF_TITLEBAR | FCF_SYSMENU
#define STY_STATUSDLG         WC_SPINBUTTON, SPBS_JUSTCENTER  | SPBS_NOBORDER | \
                              SPBS_NUMERICONLY | SPBS_READONLY | \
                              SPBS_PADWITHZEROS  | WS_VISIBLE
#define STY_DISPVAL           WC_SPINBUTTON, SPBS_JUSTCENTER  | \
                              SPBS_PADWITHZEROS  | WS_VISIBLE
#define STY_TEXT              WC_STATIC, SS_TEXT | DT_LEFT | DT_TOP | WS_VISIBLE
#define TEXT(txt,x,y,w,h) CONTROL txt, ID_NONE, x, y, w, h, WC_STATIC, \
                                       SS_AUTOSIZE | SS_TEXT | DT_LEFT | DT_TOP | WS_VISIBLE
                                       // DT_LEFT?0, specify DT_RIGHT, DT_CENTER

//
//  --------------- Resource Definitions ----------------
//
#define IDM_VICE2      0x001

#define ID_NONE        -1
#define DID_CLOSE      DID_OK

#define PTR_DRAGOK     0x100
#define PTR_INFO       0x101
#define PTR_SKULL      0x102
#define PTR_NOTE       0x103

// About Dialog
#define DLG_ABOUT      0x1050

// Monitor Dialog
#define DLG_MONITOR    0x10b0
#define LB_MONOUT      0x10b1
#define EF_MONIN       0x10b2
#define WM_INSERT      WM_USER+0x1
#define WM_INPUT       WM_USER+0x2
#define WM_PROMPT      WM_USER+0x3
//#define WM_DELETE      WM_USER+0x4

// Contents Dialog
#define DLG_CONTENTS   0x10c0
#define LB_CONTENTS    0x10c1

// Commandline option Dialog
#define DLG_CMDOPT     0x10d0
#define LB_CMDOPT      0x10d1

//
// ----------------- Win*-Macros -----------------------                                                    */
//

#define WinIsDlgEnabled(hwnd, id) \
    WinIsWindowEnabled(WinWindowFromID(hwnd, id))
#define WinSendDlgMsg(hwnd, id, msg, mp1, mp2) \
    WinSendMsg(WinWindowFromID(hwnd, id), msg, (MPARAM)(mp1), (MPARAM)(mp2))
#define WinSetDlgPos(hwnd, id, d, x, y, cx, cy, swp) \
    WinSetWindowPos(WinWindowFromID(hwnd, id), d, x, y, cx, cy, swp)
#define WinCreateStdDlg(hwnd, id, d0, d1, d2, x, y, cx, cy) \
    WinCreateWindow(hwnd, d0, d2, d1, x, y, cx, cy, NULLHANDLE, HWND_TOP, id, NULL, NULL)

//
// ---------------- List Box Macros ------------------
//
#define WinSetDlgLboxItemText(hwnd, id, index, psz) \
    WinSendDlgMsg(hwnd, id, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz))
#define WinLboxSelectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, (void*)(index), TRUE);
#define WinLboxDeselectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, FALSE);
#define WinLboxInsertItem(hwnd, id, psz) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), LIT_END, psz)
#define WinLboxInsertItemAt(hwnd, id, psz, pos) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), pos, psz)
#define WinLboxQueryCount(hwnd, id) \
    WinQueryLboxCount(WinWindowFromID(hwnd, id))
#define WinLboxQueryItem(hwnd, id, pos, psz, max) \
    WinQueryLboxItemText(WinWindowFromID(hwnd, id), pos, psz, max)
#define WinLboxDeleteItem(hwnd, id, pos) \
    WinDeleteLboxItem(WinWindowFromID(hwnd, id), pos)
#define WinLboxQuerySelectedItemText(hwnd, id, psz, max) \
    WinLboxQueryItem(hwnd, id, WinQueryLboxSelectedItem(WinWindowFromID(hwnd, id)), psz, max)
#define WinQueryDlgText(hwnd, id, psz, max) \
    WinQueryWindowText(WinWindowFromID(hwnd, id), max, psz)

//
// ---------------- Resource funtions ------------------
//

extern int toggle(const char *resource_name);
extern void ViceErrorDlg(HWND hwnd, int id, char *text);

//
// ---------------- dialog definitions ------------------
//
extern HWND hwndMonitor;

extern void  about_dialog     (HWND hwnd);
extern void  monitor_dialog   (HWND hwnd);
extern void  contents_dialog  (HWND hwnd, char *szFullFile);
extern void  create_dialog    (HWND hwnd);
extern HWND  cmdopt_dialog    (HWND hwnd);

extern void hardreset_dialog (HWND hwnd);
extern void softreset_dialog (HWND hwnd);

extern int isEmulatorPaused(void);
extern void emulator_pause(void);
extern void emulator_resume(void);

#endif
