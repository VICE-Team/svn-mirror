/*
 * dlg-attach.c - The attach-dialog.
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

#define INCL_WINSTDSPIN // WinSetSpinVal
#define INCL_WINDIALOGS // WinSendDlgItemMsg
#include "vice.h"
#include "dialogs.h"

#include "psid.h"

static MRESULT EXPENTRY pm_vsid(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first=TRUE;
    extern int trigger_shutdown;

    switch (msg)
    {
    case WM_PAINT:
        if (first)
        {
            int tunes;
            int default_tune;
            tunes = psid_tunes(&default_tune);
            WinSetSpinVal(hwnd, SPB_TUNENO, default_tune);
            WinSetSpinVal(hwnd, SPB_TUNES, tunes);
            first = FALSE;
        }
        break;
    case WM_CLOSE:
        trigger_shutdown = 1;
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            trigger_shutdown = 1;
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

HWND hwndVsid;

void vsid_dialog()
{
    hwndVsid = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, pm_vsid, NULLHANDLE,
                          DLG_VSID, NULL);
}
