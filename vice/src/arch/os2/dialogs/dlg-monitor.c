/*
 * dlg-monitor.c - The monitor-dialog.
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

#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include "vice.h"
#include "dialogs.h"

#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "utils.h"

int trigger_console_exit;

static MRESULT EXPENTRY pm_monitor(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int  *wait_for_input;
    static char **input;

    switch (msg)
    {
    case WM_INITDLG:
        {
            CHAR achFont[] = "11.System VIO";

            trigger_console_exit=FALSE;

            WinSetDlgFont(hwnd, LB_MONOUT, achFont);
            WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, EF_MONIN));
        }
        break;

    case WM_CLOSE:
        if (!wait_for_input)
            break;

        trigger_console_exit=TRUE;
        input=NULL;
        *wait_for_input=FALSE;
        wait_for_input=NULL;
        break;

    case WM_CHAR:
        if (SHORT1FROMMP(mp1)&KC_CHAR)
        {
            char txt[80]="";
            WinQueryDlgText(hwnd, EF_MONIN, txt, 80);
            if (strlen(txt))
            {
                if (input)
                    *input=stralloc(txt);
                WinSetDlgItemText(hwnd, EF_MONIN,"");
                input=NULL;
                *wait_for_input=FALSE;
                wait_for_input=NULL;
            }
        }
        break;

    case WM_INSERT:
        WinLboxInsertItem(hwnd, LB_MONOUT, (char*)mp1);
        free(mp1);
        WinSendDlgMsg(hwnd, LB_MONOUT, LM_SETTOPINDEX,
                      WinLboxQueryCount(hwnd, LB_MONOUT), 0);
        return FALSE;

    case WM_ADJUSTWINDOWPOS:
        {
            SWP *swp=(SWP*)mp1;

            if (!(swp->fl&SWP_SIZE))
                break;

            if (swp->cx<320) swp->cx=320;
            if (swp->cy<200) swp->cy=200;
            // SWP wpos;
            // WinQueryWindowPos(WinWindowFromID(hwnd, LB_MONOUT), &wpos);
            WinSetWindowPos(WinWindowFromID(hwnd, LB_MONOUT), 0, 0, 0,
                            swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                            swp->cy-2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)
                            -WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)-22,
                            SWP_SIZE);
            // WinQueryWindowPos(WinWindowFromID(hwnd, EF_MONIN), &wpos);
            WinSetWindowPos(WinWindowFromID(hwnd, EF_MONIN), 0, 0, 0,
                            swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME)-4,
                            16, SWP_SIZE);
        }
        break;
    case WM_PROMPT:
        {
            char *out, tmp[90];
            int pos=WinLboxQueryCount(hwnd, LB_MONOUT)-1;
            WinLboxQueryItem(hwnd, LB_MONOUT, pos, tmp, 90);
            WinLboxDeleteItem(hwnd, LB_MONOUT, pos);
            out = concat(tmp, mp1, NULL);
            WinLboxInsertItem(hwnd, LB_MONOUT, out);
            WinSendDlgMsg(hwnd, LB_MONOUT, LM_SETTOPINDEX, pos, 0);
            free(out);
        }
        return FALSE;

    case WM_INPUT:
        input          = mp1;
        wait_for_input = mp2;
        return FALSE;

    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */
HWND hwndMonitor=NULLHANDLE;

void monitor_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndMonitor))
        return;

    hwndMonitor=WinLoadDlg(HWND_DESKTOP, hwnd, pm_monitor, NULLHANDLE,
                           DLG_MONITOR, NULL);
}

