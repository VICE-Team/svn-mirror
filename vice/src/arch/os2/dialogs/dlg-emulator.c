/*
 * dlg-emulator.c - The emulator-dialog.
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

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "dlg-emulator.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "vsync.h"
#include "resources.h"
#include "snippets\pmwin2.h"

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static MRESULT EXPENTRY pm_emulator(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    // FIXME: Speed=0 'No Limit' is missing
    static int first = TRUE;
    const char psz[11][6] = {
        " Auto",
        " 1/1",
        " 1/2",
        " 1/3",
        " 1/4",
        " 1/5",
        " 1/6",
        " 1/7",
        " 1/8",
        " 1/9",
        " 1/10"
    };

    switch (msg) {
        case WM_INITDLG:
            {
            int val=0;

                for (val = 0; val < 11; val++) {
                    WinDlgLboxInsertItem(hwnd, CBS_REFRATE, psz[val]);
                }
                resources_get_int("Speed", &val);
                WinSetDlgSpinVal(hwnd, SPB_SPEED, val);
                WinEnableControl(hwnd, PB_SPEED100, (val!=100));
                resources_get_int("RefreshRate", &val);
                WinDlgLboxSelectItem(hwnd, CBS_REFRATE, val);
            }
            break;
        case WM_COMMAND:
            switch (LONGFROMMP(mp1)) {
                case PB_SPEED100:
                WinSetDlgSpinVal(hwnd, SPB_SPEED, 100);
                vsync_suspend_speed_eval();
                resources_set_int("Speed", 100);
                WinEnableControl(hwnd, PB_SPEED100, FALSE);
                return FALSE;
            }
            break;
        case WM_DISPLAY:
            {
                char txt1[8] = "---%";
                char txt2[8] = "--fps";

                if ((int)mp1 < 100000) {
                    sprintf(txt1, "%5d%%", mp1);
                }
                if ((int)mp2 < 10000) {
                    sprintf(txt2, "%4dfps", mp2);
                }
                WinSetDlgItemText(hwnd, ID_SPEEDDISP, txt1);
                WinSetDlgItemText(hwnd, ID_REFRATEDISP, txt2);
            }
            return FALSE;
        case WM_CONTROL:
            {
                const int ctrl = SHORT1FROMMP(mp1);

                switch (ctrl) {
                    case SPB_SPEED:
                        if (SHORT2FROMMP(mp1) == SPBN_ENDSPIN) {
                            const ULONG val = WinGetSpinVal((HWND)mp2);

                            vsync_suspend_speed_eval();
                            resources_set_int("Speed", val);
                            WinEnableControl(hwnd, PB_SPEED100, (val != 100));
                        }
                        break;
                    case CBS_REFRATE:
                        if (SHORT2FROMMP(mp1) == LN_SELECT) {
                            const int val = WinQueryLboxSelectedItem((HWND)mp2);

                            resources_set_int("RefreshRate", val);
                        }
                        break;
                }
                break;
            }
            break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

HWND hwndEmulator=NULLHANDLE;

void emulator_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndEmulator)) {
        return;
    }

    hwndEmulator = WinLoadStdDlg(hwnd, pm_emulator, DLG_EMULATOR, NULL);
}
