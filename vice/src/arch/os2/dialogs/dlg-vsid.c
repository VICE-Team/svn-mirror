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
#include "resources.h"

static MRESULT EXPENTRY pm_vsid(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first=TRUE;
    extern int trigger_shutdown;

    switch (msg)
    {
    case WM_PAINT:
        if (first)
            first = FALSE;
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
/*        case PB_DEFTUNE:
            psid_set_tune(0);
            return FALSE;*/
        }
        break;

    case WM_CONTROL:
        {
            int ctrl=SHORT1FROMMP(mp1);
            switch (ctrl)
            {
            case SPB_SETTUNE:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
                {
                    ULONG val;
                    WinGetSpinVal(hwnd, ctrl, &val);
                    resources_set_value("PSIDTune", (resource_value_t)val);
                }
                break;
            }
        }
        break;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

HWND hwndVsid;

void vsid_dialog()
{
    hwndVsid = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, pm_vsid, NULLHANDLE,
                          DLG_VSID, NULL);
}
