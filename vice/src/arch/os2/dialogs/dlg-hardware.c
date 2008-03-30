/*
 * dlg-hardware.c - The hardware-dialog.
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

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "drive.h"
#include "resources.h"

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static MRESULT EXPENTRY pm_hardware(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_HARDWARE);
        first = TRUE;
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_HARDWARE);
            break;
        }
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_HARDWARE);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int val;
                first=FALSE;
#ifdef __X128__
                resources_get_value("VDC_64KB", (resource_value_t*) &val);
                WinCheckButton(hwnd, val?RB_VDC64K:RB_VDC16K, 1);
#endif
#if defined __X64__ || defined __X128__
                resources_get_value("REU", (resource_value_t*) &val);
                WinCheckButton(hwnd, CB_REU, val?1:0);
                resources_get_value("VideoStandard", (resource_value_t*) &val);
                switch (val)
                {
                case DRIVE_SYNC_PAL:
                    WinCheckButton(hwnd, RB_PAL, 1);
                    break;
                case DRIVE_SYNC_NTSC:
                    WinCheckButton(hwnd, RB_NTSC, 1);
                    break;
#ifdef __X64__
                case DRIVE_SYNC_NTSCOLD:
                    WinCheckButton(hwnd, RB_NTSCOLD, 1);
                    break;
#endif // __X64__
                }
#endif // __X64__ || __X128__
            }
        }
        break;
    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
            switch (ctrl)
            {
#ifdef __X128__
            case RB_VDC16K:
                resources_set_value("VDC_64KB", (resource_value_t*)0);
                break;
            case RB_VDC64K:
                resources_set_value("VDC_64KB", (resource_value_t*)1);
                break;
#endif
#if defined __X64__ || defined __X128__
            case RB_PAL:
                resources_set_value("VideoStandard",
                                    (resource_value_t*) DRIVE_SYNC_PAL);
                break;
            case RB_NTSC:
                resources_set_value("VideoStandard",
                                    (resource_value_t*) DRIVE_SYNC_NTSC);
                break;
#ifdef __X64__
            case RB_NTSCOLD:
                resources_set_value("VideoStandard",
                                    (resource_value_t*) DRIVE_SYNC_NTSCOLD);
                break;
#endif // __X64__
            case CB_REU:
                toggle("REU");
                break;
#endif // __X64__ || __X128__
            }
            break;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void hardware_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_HARDWARE)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_hardware, NULLHANDLE,
               DLG_HARDWARE, NULL);
}

