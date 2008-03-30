/*
 * dlg-drive.c - The drive-dialog.
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
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include "ui_status.h"

#include "vice.h"
#include "dialogs.h"

#include <string.h>

#include "log.h"
#include "drive.h"
#include "attach.h"
#include "fliplist.h"
#include "resources.h"

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static int toggle_drive_res(char *format, int drive)
{
    char tmp[80];
    sprintf(tmp, format, drive+8);
    return toggle(tmp);
}

static int get_drive_res(char *format, int drive)
{
    char tmp[80];
    int val;
    sprintf(tmp, format, drive+8);
    resources_get_value(tmp, (resource_value_t*) &val);
    return val;
}

static void set_drive_res(char *format, int drive, int val)
{
    char tmp[80];
    sprintf(tmp, format, drive+8);
    resources_set_value(tmp, (resource_value_t) val);
}


extern void ui_attach(HWND hwnd, int number);

static MRESULT EXPENTRY pm_drive(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int drive = 0;
    static int first = TRUE;

    static char image_hist[10][CCHMAXPATH];

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_DRIVE);
        first = TRUE;
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_DRIVE);
        break;
    case WM_PAINT:
        {
            if (first) {
                int val, i;
                for (i=0; i<10; i++)
                    if (image_hist[i][0])
                        WinLboxInsertItem(hwnd, CBS_IMAGE, image_hist[i]);
                WinLboxInsertItem(hwnd, CBS_IMAGE, "");
                resources_get_value("VideoStandard", (resource_value_t*) &val);
                WinCheckButton(hwnd, val==DRIVE_SYNC_PAL?RB_PAL:RB_NTSC, 1);
                resources_get_value("DriveTrueEmulation", (resource_value_t*) &val);
                WinCheckButton(hwnd, CB_TRUEDRIVE, val);
                WinCheckButton(hwnd, RB_DRIVE8|drive, 1);
                WinSendMsg(hwnd, WM_SWITCH, (void*)drive, 0);
                first=FALSE;
            }
        }
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_DRIVE);
            break;
        case PB_ATTACH:
            ui_attach(hwnd, drive+8);
            return FALSE;
        case PB_DETACH:
            file_system_detach_disk(drive+8);
            return FALSE;
        }
        break;
    case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
            case RB_DRIVE8:
            case RB_DRIVE9:
            case RB_DRIVE10:
            case RB_DRIVE11:
                log_debug("Radio %x (%d)", SHORT1FROMMP(mp1),SHORT1FROMMP(mp1)&0x3);
                WinSendMsg(hwnd, WM_SWITCH, (void*)(SHORT1FROMMP(mp1)&0x3), 0);
                break;
            case CB_TRUEDRIVE:
                toggle("DriveTrueEmulation");
                break;
            case CB_CONVERTP00:
                WinEnableControl(hwnd, CB_HIDENONP00,
                                 toggle_drive_res("FSDevice%dConvertP00", drive));
                break;
            case CB_SAVEP00:
                toggle_drive_res("FileDevice%dSaveP00", drive);
                break;
            case CB_ALLOWACCESS:
                toggle_drive_res("FileSystemDevice%d", drive);
                break;
            case CB_HIDENONP00:
                toggle_drive_res("FSDevice%dHideCBMFiles", drive);
                break;
            case CB_PARALLEL:
                if (drive==0 || drive==1)
                    toggle_drive_res("Drive%dParallelCable", drive);
                break;
            case RB_NEVER:
            case RB_ASK:
            case RB_ALWAYS:
                if (drive==0 || drive==1)
                    set_drive_res("Drive%dExtendImagePolicy", drive,
                                  SHORT1FROMMP(mp1)&0x3);
                break;
            case RB_NONE:
            case RB_TRAP:
            case RB_SKIP:
                if (drive==0 || drive==1)
                    set_drive_res("Drive%dIdleMethod", drive,
                                  SHORT1FROMMP(mp1)&0x3);
                break;
            case RB_PAL:
                resources_set_value("VideoStandard", (resource_value_t*) DRIVE_SYNC_PAL);
                break;
            case RB_NTSC:
                resources_set_value("VideoStandard", (resource_value_t*) DRIVE_SYNC_NTSC);
                break;
            case CBS_IMAGE:
                {
                    if (SHORT2FROMMP(mp1)==CBN_ENTER)
                    {
                        char psz[CCHMAXPATH];
                        WinLboxQuerySelectedItemText(hwnd, CBS_IMAGE, psz, CCHMAXPATH);

                        if (strlen(psz))
                        {
                            if (file_system_attach_disk(drive+8, psz))
                                WinError(hwnd, "Cannot attach specified file.");
                        }
                        else
                            file_system_detach_disk(drive+8);
                    }
                }
                return FALSE;
            }
        }
        break;
    case WM_DRIVEIMAGE:
        {
            int pos;
            if (!strlen(mp1))
                pos=WinLboxQueryCount(hwnd, CBS_IMAGE)-1;
            else
            {
                char tmp[CCHMAXPATH];
                int max=WinLboxQueryCount(hwnd, CBS_IMAGE);
                int i;
                pos=-1;
                for (i=0; i<max; i++)
                {
                    int j=WinLboxQueryItem(hwnd, CBS_IMAGE, i, tmp, CCHMAXPATH);
                    if (!strcmp(tmp, mp1)) pos=i;
                }
                if (pos<0)
                {
                    for (i=10; i>0; i--)
                        strcpy(image_hist[i], image_hist[i-1]);
                    strcpy(image_hist[0], mp1);
                    if (max==11) WinLboxDeleteItem(hwnd, CBS_IMAGE, 9);
                    WinLboxInsertItemAt(hwnd, CBS_IMAGE, mp1, 0);
                    pos=0;
                }
            }
            if (drive==(int)mp2)
                WinLboxSelectItem(hwnd, CBS_IMAGE, pos);
        }
        return FALSE;
    case WM_SWITCH:
        drive = (int)mp1;
        {
            int drive89=(drive==0 || drive==1);
            if (drive89)
            {
                WinCheckButton(hwnd, RB_NEVER |
                               get_drive_res("Drive%dExtendImagePolicy",
                                             drive), 1);
                WinCheckButton(hwnd, RB_NONE |
                               get_drive_res("Drive%dIdleMethod",
                                             drive), 1);
            }
            else
            {
                WinCheckButton(hwnd, RB_NEVER,  0);
                WinCheckButton(hwnd, RB_ASK,    0);
                WinCheckButton(hwnd, RB_ALWAYS, 0);
                WinCheckButton(hwnd, RB_NONE,   0);
                WinCheckButton(hwnd, RB_SKIP,   0);
                WinCheckButton(hwnd, RB_TRAP,   0);
            }
            WinCheckButton(hwnd, CB_PARALLEL, drive89 ?
                           (get_drive_res("Drive%dParallelCable", drive)!=0):0);
            WinEnableControl(hwnd, CB_PARALLEL, drive89);
            WinEnableControl(hwnd, RB_NEVER,    drive89);
            WinEnableControl(hwnd, RB_ASK,      drive89);
            WinEnableControl(hwnd, RB_ALWAYS,   drive89);
            WinEnableControl(hwnd, RB_NONE,     drive89);
            WinEnableControl(hwnd, RB_SKIP,     drive89);
            WinEnableControl(hwnd, RB_TRAP,     drive89);
            WinCheckButton(hwnd, CB_SAVEP00,
                           get_drive_res("FSDevice%dSaveP00", drive)!=0);
            WinCheckButton(hwnd, CB_ALLOWACCESS,
                           get_drive_res("FileSystemDevice%d", drive)!=0);
            WinCheckButton(hwnd, CB_HIDENONP00,
                           get_drive_res("FSDevice%dHideCBMFiles", drive)!=0);
            {
                int conv=(get_drive_res("FSDevice%dConvertP00", drive)!=0);
                WinCheckButton  (hwnd, CB_CONVERTP00, conv);
                WinEnableControl(hwnd, CB_HIDENONP00, conv);
            }
            {
                char tmp[CCHMAXPATH];
                int max=WinLboxQueryCount(hwnd, CBS_IMAGE);
                int pos=-1;
                int i;
                for (i=0; i<max; i++)
                {
                    WinLboxQueryItem(hwnd, CBS_IMAGE, i, tmp, CCHMAXPATH);
                    if (!strcmp(ui_status.lastImage[drive], tmp)) pos=i;
                }
                WinLboxSelectItem(hwnd, CBS_IMAGE, pos);
            }
        }
        return FALSE;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

HWND hwndDrive;

void drive_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DRIVE)) return;
    hwndDrive=WinLoadDlg(HWND_DESKTOP, hwnd, pm_drive, NULLHANDLE,
                         DLG_DRIVE, NULL);
}


