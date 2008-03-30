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

#define INCL_WINSYS // font
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES

#include "vice.h"
#include "dialogs.h"

#include <stdio.h>
#include <string.h>

HWND hwndCmdopt;

static MRESULT EXPENTRY pm_cmdopt(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_CMDOPT);
        first = TRUE;
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_CMDOPT);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                CHAR achFont[] = "11.System VIO";
                char text[1024];

                first=FALSE;

                WinSetDlgFont(hwnd, LB_CMDOPT, achFont);
            }
        }
        break;
    case WM_INSERT:
        WinLboxInsertItem(hwnd, LB_CMDOPT, (char*)mp1);
        WinSendDlgMsg(hwnd, LB_CMDOPT, LM_SETTOPINDEX,
                      WinLboxQueryCount(hwnd, LB_CMDOPT),0);
        return FALSE;
    case WM_ADJUSTWINDOWPOS:
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

void cmdopt_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_CMDOPT)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_cmdopt, NULLHANDLE,
               DLG_CMDOPT, NULL);
}

