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
#include "dialogs.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "vsync.h"
#include "drive.h"
#include "resources.h"

#if defined __X64__ || defined __X128__ || defined __XVIC__
const char *VIDEO_CACHE="VideoCache";
#else
//#if defined __XPET__ || defined __XCBM__
const char *VIDEO_CACHE="CrtcVideoCache";
#endif

CHAR  screenshotHist[10][CCHMAXPATH];

/* Needed prototype funtions                                        */
/*----------------------------------------------------------------- */

extern void emulator_pause(void);
extern void emulator_resume(void);
extern int isEmulatorPaused(void);

/* The Screenshot saveas name                                       */
/*------------------------------------------------------------------*/

#define ID_NAME (void*)0
#define ID_TYPE (void*)1

static char pszScreenshotName[CCHMAXPATH]="vice2.bmp";
static int iSsType=0;

char *screenshot_name()
{
    return pszScreenshotName;
}

char *screenshot_type()
{
    return iSsType?"PNG":"BMP";
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
                first=FALSE;
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
#ifdef HAVE_MOUSE
                resources_get_value("Mouse", (resource_value_t *) &val);
                WinCheckButton(hwnd, CB_MOUSE, val);
                if (val)
                {
                    WinEnableControl(hwnd, CB_HIDEMOUSE, 1);
                    resources_get_value("HideMousePtr", (resource_value_t *) &val);
                    WinCheckButton(hwnd, CB_HIDEMOUSE, val);
                }
                else
                {
                    WinCheckButton(hwnd, CB_HIDEMOUSE, 0);
                    WinEnableControl(hwnd, CB_HIDEMOUSE, 0);
                }
#endif // HAVE_MOUSE
                val=0;
                while (val<10 && screenshotHist[val][0])
                    WinLboxInsertItem(hwnd, CBS_SSNAME, screenshotHist[val++]);
                WinCheckButton(hwnd, iSsType?RB_PNG:RB_BMP, 1);
                for (val=0; val<11; val++)
                    WinLboxInsertItem(hwnd, CBS_REFRATE, psz[val]);
                resources_get_value("Speed", (resource_value_t *) &val);
                WinSetSpinVal(hwnd, SPB_SPEED, val);
                WinEnableControl(hwnd, PB_SPEED100, (val!=100));
                resources_get_value("RefreshRate", (resource_value_t *) &val);
                WinLboxSelectItem(hwnd, CBS_REFRATE, val);
            }
        }
        break;
    case WM_DISPLAY:
        WinSetSpinVal(hwnd, SPB_SPEEDDISP,   mp1);
        WinSetSpinVal(hwnd, SPB_REFRATEDISP, mp2);
        return FALSE;
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
#if defined HAVE_MOUSE
            case CB_MOUSE:
                {
                    int val=toggle("Mouse");
                    if (val)
                    {
                        WinEnableControl(hwnd, CB_HIDEMOUSE, 1);
                        resources_get_value("HideMousePtr", (resource_value_t *) &val);
                        WinCheckButton(hwnd, CB_HIDEMOUSE, val);
                    }
                    else
                    {
                        WinCheckButton(hwnd, CB_HIDEMOUSE, 0);
                        WinEnableControl(hwnd, CB_HIDEMOUSE, 0);
                    }
                    break;
                }
            case CB_HIDEMOUSE:
                toggle("HideMousePtr");
                break;
#endif // HAVE_MOUSE
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
            case CB_SBCOLL:
                toggle("CheckSbColl");
                break;
            case CB_SSCOLL:
                toggle("CheckSsColl");
                break;
            case CB_EMUID:
                toggle("EmuID");
                break;
#endif // __X64__ || __X128__
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
            case RB_BMP:
                screenshot_check_type(hwnd, 0);
                break;
            case RB_PNG:
                screenshot_check_type(hwnd, 1);
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
    if (dlgOpen(DLGO_EMULATOR)) return;
    hwndEmulator=WinLoadDlg(HWND_DESKTOP, hwnd, pm_emulator, NULLHANDLE,
                            DLG_EMULATOR, NULL);
}

