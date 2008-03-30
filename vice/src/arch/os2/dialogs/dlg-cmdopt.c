/*
 * dlg-cmdopt.c - The commandline options dialog.
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

#define INCL_WINSYS        // font
#define INCL_WINPOINTERS   // WinLoadPointer
#define INCL_WINFRAMEMGR   // WM_SETICON
#define INCL_WINLISTBOXES  // WinLbox*
#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "dialogs.h"

static MRESULT EXPENTRY pm_cmdopt(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            HPOINTER hicon=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, IDM_VICE2);
            if (hicon)
                WinSendMsg(hwnd, WM_SETICON, MPFROMLONG(hicon), MPVOID);
        }
        return FALSE;

    case WM_INSERT:
        //
        // insert a new line to the text
        //
        WinLboxInsertItem(hwnd, LB_CMDOPT, (char*)mp1);
        WinSendDlgMsg(hwnd, LB_CMDOPT, LM_SETTOPINDEX,
                      WinLboxQueryCount(hwnd, LB_CMDOPT),0);
        return FALSE;

    case WM_ADJUSTWINDOWPOS:
        //
        // resize dialog
        //
        {
            SWP *swp=(SWP*)mp1;
            if (swp->fl&SWP_SIZE)
            {
                if (swp->cx<320) swp->cx=320;
                if (swp->cy<200) swp->cy=200;
                WinSetWindowPos(WinWindowFromID(hwnd, LB_CMDOPT), 0, 0, 0,
                                swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                                swp->cy-2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)
                                -WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)-2,
                                SWP_SIZE);
            }
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/*----------------------------------------------------------------- */

HWND cmdopt_dialog(HWND hwnd)
{
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2))
        return NULLHANDLE;

    hwnd2 = WinLoadDlg(HWND_DESKTOP, hwnd, pm_cmdopt, NULLHANDLE,
                       DLG_CMDOPT, NULL);
    return hwnd2;
}

