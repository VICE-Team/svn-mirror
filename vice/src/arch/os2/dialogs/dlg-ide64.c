/*
 * dlg-ide64.c - The IDE64 geometry dialog.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#define INCL_WINSTDSPIN   // SPBN_*
#define INCL_WINDIALOGS

#include <os2.h>

#include "vice.h"

#include "ui.h"
#include "ui_status.h"

#include "dialogs.h"
#include "dlg-ide64.h"

#include "resources.h"         // resources_*
#include "snippets\pmwin2.h"   // WinShowDlg

static int ide64_nr = 0;

static MRESULT EXPENTRY pm_ide64(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int val;

                resources_get_int_sprintf("IDE64Cylinders%i", &val, ide64_nr);
                WinSetDlgSpinVal(hwnd, IDE64_CYLS, val);
                resources_get_int_sprintf("IDE64Heads%i", &val, ide64_nr);
                WinSetDlgSpinVal(hwnd, IDE64_HEADS, val);
                resources_get_int_sprintf("IDE64Sectors%i", &val, ide64_nr);
                WinSetDlgSpinVal(hwnd, IDE64_SECTORS, val);
            }
            break;
        case WM_CONTROL:
            switch (SHORT1FROMMP(mp1)) {
                case IDE64_CYLS:
                    if (SHORT2FROMMP(mp1) == SPBN_ENDSPIN) {
                        const ULONG val = WinGetSpinVal((HWND)mp2);
                        resources_set_int_sprintf("IDE64Cylinders%i", val, ide64_nr);
                    }
                    break;
                case IDE64_HEADS:
                    if (SHORT2FROMMP(mp1) == SPBN_ENDSPIN) {
                        const ULONG val = WinGetSpinVal((HWND)mp2);
                        resources_set_int_sprintf("IDE64Heads%i", val, ide64_nr);
                    }
                    break;
                case IDE64_SECTORS:
                    if (SHORT2FROMMP(mp1) == SPBN_ENDSPIN) {
                        const ULONG val = WinGetSpinVal((HWND)mp2);
                        resources_set_int_sprintf("IDE64Sectors%i", val, ide64_nr);
                    }
                    break;
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

HWND hwndIDE64 = NULLHANDLE;

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void ide64_dialog(HWND hwnd, int nr)
{
    if (WinIsWindowVisible(hwndIDE64)) {
        return;
    }

    ide64_nr = nr;

    hwndIDE64 = WinLoadStdDlg(hwnd, pm_ide64, DLG_IDE64, NULL);
}
