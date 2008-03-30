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

#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include "vice.h"
#include "dialogs.h"

#include "log.h"

static MRESULT EXPENTRY pm_monitor(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_MONITOR);
        first = TRUE;
        break;
    case WM_CHAR:
        if (SHORT1FROMMP(mp1)&KC_CHAR)
            log_debug("wmchar %x ",  SHORT1FROMMP(mp2));
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_MONITOR);
            break;
        }
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_MONITOR);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                WinEnableControl(hwnd, LB_MONOUT, 0);
                first=FALSE;
            }
        }
        break;
    case WM_BUTTON1DOWN:
        log_debug("button1");
        return FALSE;

    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
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
            log_debug("entry: %x %x", mp1, mp2);
/*            switch (ctrl)
            {
            }*/
/*            BEI WM_CHAR 0xd oder 0x9 (return and tab)

 An application can use the WinQueryWindowTextLength and WinQueryWindowText functions to retrieve the text from an entry field.  WinQueryWindowTextLength
 returns the length of the text; WinQueryWindowText copies the window text to a buffer.

 Typically, an application needs to retrieve the text from an entry field only if the user changes the text.  An entry field sends an EN_CHANGE notification code
 in the low word of the first message parameter of the WM_CONTROL message whenever the text changes.  The following code fragment sets a flag when it
 receives the EN_CHANGE code, checks the flag during the WM_COMMAND message and, if it is set, retrieves the text of the entry field:


     HWND hwnd;
     ULONG msg;
     MPARAM mp1;
     CHAR chBuf[64];
     HWND hwndEntryField;
     LONG cbTextLen;
     LONG cbTextRead;
     static BOOL fFieldChanged = FALSE;

     switch (msg) {
         case WM_CONTROL:
             switch (SHORT1FROMMP(mp1)) {
                 case IDD_ENTRYFIELD:

                 /* Check if t
                  he user changed the entry-field text.
                     if ((USHORT) SHORT2FROMMP(mp1) == EN_CHANGE)
                         fFieldChanged = TRUE;
                     return 0;
             }

         case WM_COMMAND:
             switch (SHORT1FROMMP(mp1)) {
                 case DID_OK:

                     /* If the user changed the entry-field text,
                     /* obtain the text and store it in a buffer.
                     if (fFieldChanged) {
                         hwndEntryField = WinWindowFromID(hwnd,
                             IDD_ENTRYFIELD);
                         cbTextLen = WinQueryWindowTextLength(hwndEntryField);
                         cbTextRead = WinQueryWindowText(hwndEntryField,
                             sizeof(chBuf), chBuf);
                         .
                         . /* Do something with the text.
                         .
                     }
                     WinDismissDlg(hwnd, 1);
                     return 0;
             }
     }

*/

            break;
        }
    case WM_INSERT:
        WinLboxInsertItem(hwnd, LB_MONOUT, (char*)mp1);
        break;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

HWND hwndMonitor;

void monitor_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_MONITOR)) return;
    hwndMonitor=WinLoadDlg(HWND_DESKTOP, hwnd, pm_monitor, NULLHANDLE,
                           DLG_MONITOR, NULL);
}

