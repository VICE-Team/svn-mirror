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

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS

#include "vice.h"
#include "dialogs.h"

#include "resources.h"
#include "datasette.h"

static MRESULT EXPENTRY pm_datasette(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_DATASETTE);
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_DATASETTE);
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_DATASETTE);
            break;
        case PB_STOP:
        case PB_START:
        case PB_FORWARD:
        case PB_REWIND:
        case PB_RECORD:
        case PB_RESET:
            datasette_control(LONGFROMMP(mp1)&0xf);
            return FALSE;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void datasette_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DATASETTE)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_datasette, NULLHANDLE,
               DLG_DATASETTE, NULL);
}

