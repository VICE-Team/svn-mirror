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

#include "vice.h"
#include "dialogs.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "vsync.h"
#include "resources.h"


#if defined __X64__ || defined __X128__ || defined __XVIC__
#define VIDEO_CACHE "VideoCache"
#else
//#if defined __XPET__ || defined __XCBM__
#define VIDEO_CACHE "CrtcVideoCache"
#endif

/* Needed prototype funtions                                        */
/*----------------------------------------------------------------- */

extern void emulator_pause(void);
extern void emulator_resume(void);
extern int isEmulatorPaused(void);

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static MRESULT EXPENTRY pm_emulator(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;
    const char psz[11][6] =
    {
        " Auto", " 1/1", " 1/2", " 1/3", " 1/4", " 1/5",
        " 1/6",  " 1/7", " 1/8", " 1/9", " 1/10"
    };

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_EMULATOR);
        first = TRUE;
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case PB_SPEED100:
            WinSetSpinVal(hwnd, SPB_SPEED, 100);
            suspend_speed_eval();
            resources_set_value("Speed", (resource_value_t)100);
            WinEnableControl(hwnd, PB_SPEED100, FALSE);
            return FALSE;
        case DID_CLOSE:
            delDlgOpen(DLGO_EMULATOR);
            break;
        }
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_EMULATOR);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int val;
                WinCheckButton(hwnd, CB_PAUSE, isEmulatorPaused());
                resources_get_value(VIDEO_CACHE,  (resource_value_t *) &val);
                WinCheckButton(hwnd, CB_VCACHE, val);
#if defined __X64__ || defined __X128__
                resources_get_value("CheckSbColl", (resource_value_t *) &val);
                WinCheckButton(hwnd, CB_SBCOLL, val);
                resources_get_value("CheckSsColl", (resource_value_t *) &val);
                WinCheckButton(hwnd, CB_SSCOLL, val);
                resources_get_value("EmuID", (resource_value_t *) &val);
                WinCheckButton(hwnd, CB_EMUID, val);
#endif
                for (val=0; val<11; val++)
                    WinLboxInsertItem(hwnd, CBS_REFRATE, psz[val]);
                resources_get_value("Speed", (resource_value_t *) &val);
                WinSetSpinVal(hwnd, SPB_SPEED, val);
                WinEnableControl(hwnd, PB_SPEED100, (val!=100));
                resources_get_value("RefreshRate", (resource_value_t *) &val);
                WinLboxSelectItem(hwnd, CBS_REFRATE, val);
                first=FALSE;
            }
        }
        break;
    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
            switch (ctrl)
            {
            case SPB_SPEED:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
                {
                    ULONG val;
                    WinGetSpinVal(hwnd, ctrl, &val);
                    suspend_speed_eval();
                    resources_set_value("Speed", (resource_value_t)val);
                    WinEnableControl(hwnd, PB_SPEED100, (val!=100));
                }
                break;
            case CBS_REFRATE:
                if (SHORT2FROMMP(mp1)==/*LN_ENTER*/LN_SELECT)
                {
                    resources_set_value("RefreshRate",
                                        (resource_value_t)WinLboxQuerySelectedItem (hwnd, CBS_REFRATE));
                }
                break;
            case CB_PAUSE:
                if (isEmulatorPaused())
                    emulator_resume();
                else
                    emulator_pause();
                break;
            case CB_VCACHE:
                toggle(VIDEO_CACHE);
                break;
#if defined __X64__ || defined __X128__
            case CB_SBCOLL:
                toggle("CheckSbColl");
                break;
            case CB_SSCOLL:
                toggle("CheckSsColl");
                break;
            case CB_EMUID:
                toggle("EmuID");
                break;
#endif
            }
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void emulator_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_EMULATOR)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_emulator, NULLHANDLE,
               DLG_EMULATOR, NULL);
}

