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

#include "log.h"

static MRESULT EXPENTRY pm_monitor(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    static int  *wait_for_input;
    static char *input;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_MONITOR);
        first = TRUE;
        break;
    case WM_CHAR:
        if (SHORT1FROMMP(mp1)&KC_CHAR)
        {
            char txt[80];
            // cbTextLen  = WinQueryWindowTextLength(hwndEntryField);
            WinQueryDlgText(hwnd, EF_MONIN, txt, 70);
            if (strlen(txt))
            {
                if (input) strcpy(input, txt);
                WinSetDlgText(hwnd, EF_MONIN,"");
                *wait_for_input=FALSE;
                wait_for_input=NULL;
                input=NULL;
            }
        }
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_MONITOR);
            if (wait_for_input)
            {
                if (input) strcpy(input, "exit");
                *wait_for_input=FALSE;
                wait_for_input=NULL;
                input=NULL;
            }
            break;
        }
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_MONITOR);
        if (wait_for_input)
        {
            if (input) strcpy(input, "exit");
            *wait_for_input=FALSE;
            wait_for_input=NULL;
            input=NULL;
        }
        break;
    case WM_PAINT:
        {
            if (first)
            {
                CHAR achFont[] = "11.System VIO";
                WinSetDlgFont(hwnd, LB_MONOUT, achFont);
//                WinEnableControl(hwnd, LB_MONOUT, 0);
                first=FALSE;
            }
        }
        break;
/*    case WM_BUTTON1DOWN:
        log_debug("button1");
        return FALSE;

    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
            switch(ctrl)
            {
            case LB_MONOUT:
                if (SHORT2FROMMP(mp1)==LN_SELECT)
                {
                    log_debug("ln_select");
                    return FALSE;
                }
                if (SHORT2FROMMP(mp1)==LN_SETFOCUS)
                {
                    log_debug("ln_setfocus");
                    return FALSE;
                }
                break;
            case EF_MONIN:
                switch (SHORT2FROMMP(mp1))
                {
                case EN_CHANGE:
                    {
                    char txt[80];
                    WinQueryWindowText(WinWindowFromID(hwnd, EF_MONIN),
                                       70, txt);
                    log_debug("change: %s", txt);
                    }
                    break;
                }
                break;
            }
        }
        break;
*/    case WM_INSERT:
        WinLboxInsertItem(hwnd, LB_MONOUT, (char*)mp1);
        WinSendDlgMsg(hwnd, LB_MONOUT, LM_SETTOPINDEX,
                      WinLboxQueryCount(hwnd, LB_MONOUT),0);
        return FALSE;
    case WM_INPUT:
        input          = mp1;
        wait_for_input = mp2;

//        *wait_for_input = FALSE;
        return FALSE;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

HWND monitor_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_MONITOR)) return NULLHANDLE;
    return WinLoadDlg(HWND_DESKTOP, hwnd, pm_monitor, NULLHANDLE,
                      DLG_MONITOR, NULL);
}

