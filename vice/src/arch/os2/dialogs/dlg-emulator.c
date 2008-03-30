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

/*
CHAR  screenshotHist[10][CCHMAXPATH];
CHAR  snapshotHist[10][CCHMAXPATH];

#define ID_NAME (void*)0
#define ID_TYPE (void*)1

static char pszScreenshotName[CCHMAXPATH]="vice2.bmp";
static char pszSnapshotName[CCHMAXPATH]="vice2.vsf";
static int save_roms = 0;
static int save_disks = 0;
static int iSsType=0;

char *screenshot_name()
{
    return pszScreenshotName;
}

char *screenshot_type()
{
    return iSsType?"PNG":"BMP";
}

char *get_snapshot(int *sr, int *sd)
{
    *sr = save_roms;
    *sd = save_disks;
    return pszSnapshotName;
};


void snapshot_check_name(const char *psz)
{
    if (strlen(psz)>4)
    {
        if (!stricmp(psz+strlen(psz)-4, ".vsf"))
            strcpy(pszScreenshotName, psz);
            return;
    }
    strcat(strcpy(pszScreenshotName, psz), ".vsf");
}

void screenshot_check_name(HWND hwnd, const char *psz)
{
    if (strlen(psz)>4)
    {
        if (!stricmp(psz+strlen(psz)-4, ".bmp"))
        {
            strcpy(pszScreenshotName, psz);
            iSsType = 0;
            WinCheckButton(hwnd, RB_BMP, 1);
            return;
        }
#ifdef HAVE_PNG
        if (!stricmp(psz+strlen(psz)-4, ".png"))
        {
            strcpy(pszScreenshotName, psz);
            iSsType = 1;
            WinCheckButton(hwnd, RB_PNG, 1);
            return;
        }
#endif // HAVE_PNG
    }
    strcat(strcpy(pszScreenshotName, psz), iSsType?".png":".bmp");
}

void screenshot_check_type(HWND hwnd, int mp)
{
    if (mp==iSsType)
        return;

    iSsType = mp;

    *(pszScreenshotName+strlen(pszScreenshotName)-4)='\0';

    strcat(pszScreenshotName, iSsType?".png":".bmp");

    WinLboxDeleteItem(hwnd, CBS_SSNAME, 0);
    WinLboxInsertItemAt(hwnd, CBS_SSNAME, pszScreenshotName, 0);
    WinLboxSelectItem(hwnd, CBS_SSNAME, 0);
}
*/
/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static MRESULT EXPENTRY pm_emulator(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    // FIXME: Speed=0 'No Limit' is missing
    static int first = TRUE;
    const char psz[11][6] =
    {
        " Auto", " 1/1", " 1/2", " 1/3", " 1/4", " 1/5",
        " 1/6",  " 1/7", " 1/8", " 1/9", " 1/10"
    };

    switch (msg)
    {
    case WM_INITDLG:
        {
            long val=0;
/*
            while (val<10 && screenshotHist[val][0])
                WinLboxInsertItem(hwnd, CBS_SSNAME, screenshotHist[val++]);
            WinCheckButton(hwnd, iSsType?RB_PNG:RB_BMP, 1);
            val=0;
            while (val<10 && snapshotHist[val][0])
                WinLboxInsertItem(hwnd, CBS_SSNAME, snapshotHist[val++]);
*/
            for (val=0; val<11; val++)
                WinDlgLboxInsertItem(hwnd, CBS_REFRATE, psz[val]);
            resources_get_value("Speed", (void *)&val);
            WinSetDlgSpinVal(hwnd, SPB_SPEED, val);
            WinEnableControl(hwnd, PB_SPEED100, (val!=100));
            resources_get_value("RefreshRate", (void *)&val);
            WinDlgLboxSelectItem(hwnd, CBS_REFRATE, val);
        }
        break;

    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case PB_SPEED100:
            WinSetDlgSpinVal(hwnd, SPB_SPEED, 100);
            vsync_suspend_speed_eval();
            resources_set_value("Speed", (resource_value_t)100);
            WinEnableControl(hwnd, PB_SPEED100, FALSE);
            return FALSE;
/*
        case PB_SSCHANGE:
            {
                char *c = screenshot_dialog(hwnd);

                if (!c || !(*c))
                    return FALSE;

                screenshot_check_name(hwnd, c);

                WinLboxInsertItemAt(hwnd, CBS_SSNAME, pszScreenshotName, 0);
                WinLboxSelectItem(hwnd, CBS_SSNAME, 0);
            }
            return FALSE;
        case PB_SPSCHANGE:
            {
                char *c = snapshot_dialog(hwnd);

                if (!c || !(*c))
                    return FALSE;

                snapshot_check_name(c);

                WinLboxInsertItemAt(hwnd, CBS_SPSNAME, pszSnapshotName, 0);
                WinLboxSelectItem(hwnd, CBS_SPSNAME, 0);
            }
            return FALSE;
*/
        }
        break;

    case WM_DISPLAY:
        {
            char txt1[8]="---%";
            char txt2[8]="--fps";
            if ((int)mp1<100000)
                sprintf(txt1, "%5d%%", mp1);
            if ((int)mp2<10000)
                sprintf(txt2, "%4dfps", mp2);
            WinSetDlgItemText(hwnd, ID_SPEEDDISP,   txt1);
            WinSetDlgItemText(hwnd, ID_REFRATEDISP, txt2);
        }
        return FALSE;
    case WM_CONTROL:
        {
            const int ctrl = SHORT1FROMMP(mp1);
            switch (ctrl)
            {
            case SPB_SPEED:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
                {
                    const ULONG val = WinGetSpinVal((HWND)mp2);
                    vsync_suspend_speed_eval();
                    resources_set_value("Speed", (resource_value_t)val);
                    WinEnableControl(hwnd, PB_SPEED100, (val!=100));
                }
                break;
            case CBS_REFRATE:
                if (SHORT2FROMMP(mp1)==/*LN_ENTER*/LN_SELECT)
                {
                    const int val = WinQueryLboxSelectedItem((HWND)mp2);
                    resources_set_value("RefreshRate", (resource_value_t)val);
                }
                break;
/*
            case CBS_SSNAME:
                {
                    char psz[CCHMAXPATH]="";
                    if (SHORT2FROMMP(mp1)==CBN_ENTER)
                        WinLboxQuerySelectedItemText(hwnd, CBS_SSNAME, psz, CCHMAXPATH);

                    if (!(*psz))
                        return FALSE;

                    screenshot_check_name(hwnd, psz);
                }
                return FALSE;
            case CBS_SPSNAME:
                {
                    char psz[CCHMAXPATH]="";
                    if (SHORT2FROMMP(mp1)==CBN_ENTER)
                        WinLboxQuerySelectedItemText(hwnd, CBS_SPSNAME, psz, CCHMAXPATH);

                    if (!(*psz))
                        return FALSE;

                    snapshot_check_name(psz);

                }
                return FALSE;
            case RB_BMP:
                screenshot_check_type(hwnd, 0);
                break;
            case RB_PNG:
                screenshot_check_type(hwnd, 1);
                break;
*/
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
    if (WinIsWindowVisible(hwndEmulator))
        return;

    hwndEmulator = WinLoadStdDlg(hwnd, pm_emulator, DLG_EMULATOR, NULL);
}

