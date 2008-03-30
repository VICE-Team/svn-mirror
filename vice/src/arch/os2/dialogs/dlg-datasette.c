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
#define INCL_WINSTDSPIN   // SPBN_*
#define INCL_WINDIALOGS
#include "vice.h"

#include "ui_status.h"

#include "dialogs.h"
#include "dlg-datasette.h"

#include "tape.h"              // tape_*
#include "datasette.h"         // datasette_*
#include "resources.h"         // resources_*
#include "dlg-fileio.h"        // ViceFileDialog
#include "snippets\pmwin2.h"   // WinShowDlg


static MRESULT EXPENTRY pm_datasette(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            int val;

            WinSendMsg(hwnd, WM_COUNTER,  (void*)ui_status.lastTapeCounter, 0);
            WinSendMsg(hwnd, WM_TAPESTAT,
                       (void*)ui_status.lastTapeCtrlStat,
                       (void*)ui_status.lastTapeStatus);
            WinShowDlg(hwnd, SS_SPIN,
                       (ui_status.lastTapeMotor && ui_status.lastTapeStatus)
                       ?1:0);

            resources_get_value("DatasetteResetWithCPU", (resource_value_t *) &val);
            WinCheckButton(hwnd, CB_RESETWCPU, val);
            resources_get_value("DatasetteZeroGapDelay", (resource_value_t *) &val);
            WinSetDlgSpinVal(hwnd, SPB_DELAY, (val/100));
            resources_get_value("DatasetteSpeedTuning", (resource_value_t *) &val);
            WinSetDlgSpinVal(hwnd, SPB_GAP, val);
        }
        break;

    case WM_COUNTER:
        WinSetDlgSpinVal(hwnd, SPB_COUNT, (ULONG)mp1);
        return FALSE;

    case WM_TAPESTAT:
        WinEnableControl(hwnd, PB_RECORD,   (int)mp2?((int)mp1!=DATASETTE_CONTROL_RECORD):0);
        WinEnableControl(hwnd, PB_REWIND,   (int)mp2?((int)mp1!=DATASETTE_CONTROL_REWIND):0);
        WinEnableControl(hwnd, PB_STOP,     (int)mp2?((int)mp1!=DATASETTE_CONTROL_STOP):0);
        WinEnableControl(hwnd, PB_START,    (int)mp2?((int)mp1!=DATASETTE_CONTROL_START):0);
        WinEnableControl(hwnd, PB_FORWARD,  (int)mp2?((int)mp1!=DATASETTE_CONTROL_FORWARD):0);
        WinEnableControl(hwnd, PB_RESET,    (int)mp2?1:0);
        WinEnableControl(hwnd, PB_RESETCNT, (int)mp2?1:0);
        WinEnableControl(hwnd, SPB_COUNT,   (int)mp2?1:0);

        if (!mp2)
            WinShowDlg(hwnd, SS_SPIN, 0);

        return FALSE;

    case WM_SPINNING:
        WinShowDlg(hwnd, SS_SPIN, (int)(mp2 && mp1)?1:0);
        return FALSE;
 
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case PB_STOP:
        case PB_START:
        case PB_FORWARD:
        case PB_REWIND:
        case PB_RECORD:
        case PB_RESET:
        case PB_RESETCNT:
            datasette_control(LONGFROMMP(mp1)&0xf);
            return FALSE;

        case PB_TATTACH:
            ViceFileDialog(hwnd, 0x0201, FDS_OPEN_DIALOG);
            return FALSE;

        case PB_TDETACH:
            tape_detach_image();
            return FALSE;
        }
        break;
    case WM_CONTROL:
        switch (SHORT1FROMMP(mp1))
        {
        case CB_RESETWCPU:
            toggle("DatasetteResetWithCPU");
            break;
        case SPB_DELAY:
            if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
            {
                const ULONG val = WinGetSpinVal((HWND)mp2);
                resources_set_value("DatasetteZeroGapDelay", (resource_value_t)(val*100));
            }
            break;
        case SPB_GAP:
            if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
            {
                const ULONG val = WinGetSpinVal((HWND)mp2);
                resources_set_value("DatasetteSpeedTuning", (resource_value_t)val);
            }
            break;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */
HWND hwndDatasette=NULLHANDLE;

void datasette_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndDatasette))
        return;

    hwndDatasette=WinLoadDlg(HWND_DESKTOP, hwnd, pm_datasette, NULLHANDLE,
                             DLG_DATASETTE, NULL);
}

