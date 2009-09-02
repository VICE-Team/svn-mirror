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

#define INCL_WINSYS         // font
#define INCL_DOSPROCESS     // DosSleep
#define INCL_WINDIALOGS     // WinProcessDlg
#define INCL_WINPOINTERS    // WinLoadPointer
#define INCL_WINFRAMEMGR    // WM_SETICON
#define INCL_WINLISTBOXES   // WinLbox*
#define INCL_WINSWITCHLIST  // HSWITCH

#include "vice.h"

#include <os2.h>

#include <stdlib.h>        // _beginthread
#include <stdio.h>
#include <string.h>

#ifdef WATCOM_COMPILE
#include <process.h>
#endif

#include "dialogs.h"
#include "menubar.h"

#include "snippets/pmwin2.h"

#include "log.h"

static MRESULT EXPENTRY pm_cmdopt(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                HPOINTER hicon=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, IDM_VICE2);
                if (hicon) {
                    WinSendMsg(hwnd, WM_SETICON, MPFROMLONG(hicon), MPVOID);
                }
            }
            return FALSE;
        case WM_INSERT:
            //
            // insert a new line to the text
            //
            if (mp2) {
                WinDlgLboxInsertItem(hwnd, LB_CMDOPT, mp1);
            } else {
                WinDlgLboxInsertItem(hwnd, LB_CMDOPT, (char*)mp1);
                WinDlgLboxSettop(hwnd, LB_CMDOPT);
            }
            return FALSE;
        case WM_MINMAXFRAME:
        case WM_ADJUSTWINDOWPOS:
            {
                //
                // resize dialog
                //
                SWP *swp=(SWP*)mp1;

                if (!(swp->fl & SWP_SIZE)) {
                    break;
                }

                if (swp->cx < 320) {
                    swp->cx = 320;
                }
                if (swp->cy < 100) {
                    swp->cy = 100;
                }
                WinSetWindowPos(WinWindowFromID(hwnd, LB_CMDOPT), 0, 0, 0, swp->cx - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                                swp->cy - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME) - WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - 2, SWP_SIZE);
            }
            break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/*----------------------------------------------------------------- */

HWND cmdopt_dialog(HWND hwnd)
{
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2)) {
        return NULLHANDLE;
    }

    hwnd2 = WinLoadStdDlg(hwnd?hwnd:HWND_DESKTOP, pm_cmdopt, DLG_CMDOPT, NULL);

    if (hwnd) {
        //
        // if the dialog is opened from the menubar while the
        // emulator is still running, remove the entry for
        // the dialog from the switch list
        //
        HSWITCH hswitch = WinQuerySwitchHandle(hwnd2, 0);
        WinRemoveSwitchEntry(hswitch);
    }

    return hwnd2;
}

/*----------------------------------------------------------------- */

HWND fsmodes_dialog(HWND hwnd)
{
    static HWND hwnd3 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd3)) {
        return NULLHANDLE;
    }

    hwnd3 = WinLoadStdDlg(hwnd?hwnd:HWND_DESKTOP, pm_cmdopt, DLG_FSMODES, NULL);

    if (hwnd) {
        //
        // if the dialog is opened from the menubar while the
        // emulator is still running, remove the entry for
        // the dialog from the switch list
        //
        HSWITCH hswitch = WinQuerySwitchHandle(hwnd3, 0);
        WinRemoveSwitchEntry(hswitch);
    }

    return hwnd3;
}

// -----------------------------------------------------------------

HWND hwndLog = NULLHANDLE;

void LogThread(void *state)
{
    QMSG qmsg;

    HAB hab = WinInitialize(0);            // Initialize PM
    HMQ hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    hwndLog = WinLoadStdDlg(HWND_DESKTOP, pm_cmdopt, DLG_LOGGING, NULL);

    if (state) {
        WinActivateWindow(hwndLog, 1);
    }

    //
    // MAINLOOP
    // (don't use WinProcessDlg it ignores the missing WM_VISIBLE flag)
    // returns when a WM_QUIT Msg goes through the queue
    //
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0)) {
        WinDispatchMsg(hab, &qmsg);
    }

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}

void log_dialog(int state)
{
    _beginthread(LogThread, NULL, 0x4000, (void*)state);

    while (!hwndLog) {
        DosSleep(1);
    }
}
