/*
 * dlg-datasette.c - The datasette-dialog.
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

#include "vice.h"

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_DOSSEMAPHORES // needed for ui_status

#include "dialogs.h"

#include "ui_status.h"

#include "tape.h"
#include "datasette.h"


static MRESULT EXPENTRY pm_datasette(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first=TRUE;
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_DATASETTE);
        first = TRUE;
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_DATASETTE);
        first = TRUE;
        break;
    case WM_PAINT:
        {
            if (first)
            {
                WinSendMsg(hwnd, WM_COUNTER,  (void*)ui_status.lastTapeCounter, 0);
                WinSendMsg(hwnd, WM_TAPESTAT,
                           (void*)ui_status.lastTapeCtrlStat,
                           (void*)ui_status.lastTapeStatus);
                WinShowDlg(hwnd, SS_SPIN,
                           (ui_status.lastTapeMotor && ui_status.lastTapeStatus)
                           ?1:0);
                first=FALSE;
            }
        }
        break;
    case WM_COUNTER:
        WinSetSpinVal(hwnd, SPB_COUNT, mp1);
        return FALSE;
    case WM_TAPESTAT:
        WinEnableControl(hwnd, PB_RECORD,   (int)mp2?((int)mp1!=DATASETTE_CONTROL_RECORD):0);
        WinEnableControl(hwnd, PB_REWIND,   (int)mp2?((int)mp1!=DATASETTE_CONTROL_REWIND):0);
        WinEnableControl(hwnd, PB_STOP,     (int)mp2?((int)mp1!=DATASETTE_CONTROL_STOP):0);
        WinEnableControl(hwnd, PB_START,    (int)mp2?((int)mp1!=DATASETTE_CONTROL_START):0);
        WinEnableControl(hwnd, PB_FORWARD,  (int)mp2?((int)mp1!=DATASETTE_CONTROL_FORWARD):0);
        WinEnableControl(hwnd, PB_RESET,    (int)mp2?1:0);
        WinEnableControl(hwnd, PB_RESETCNT, (int)mp2?1:0);
        WinEnableControl(hwnd, SPB_COUNT,   (int)mp2?1:0);
        if (!mp2) WinShowDlg(hwnd, SS_SPIN, 0);
        return FALSE;
    case WM_SPINNING:
        WinShowDlg(hwnd, SS_SPIN, (int)(mp2 && mp1)?1:0);
        return FALSE;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_DATASETTE);
            first = TRUE;
            break;
        case PB_STOP:
        case PB_START:
        case PB_FORWARD:
        case PB_REWIND:
        case PB_RECORD:
        case PB_RESET:
        case PB_RESETCNT:
            datasette_control(LONGFROMMP(mp1)&0xf);
            return FALSE;
        case PB_ATTACH:
            attach_dialog(hwnd, 0);
            return FALSE;
        case PB_DETACH:
            tape_detach_image();
            return FALSE;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */
HWND hwndDatasette;

void datasette_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DATASETTE)) return;
    hwndDatasette=WinLoadDlg(HWND_DESKTOP, hwnd, pm_datasette, NULLHANDLE,
                             DLG_DATASETTE, NULL);
}

