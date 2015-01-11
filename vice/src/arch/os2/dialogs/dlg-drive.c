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
#define INCL_WINDIALOGS        // WinSendDlgItemMsg
#define INCL_WINSTDSPIN
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include <os2.h>

#include "vice.h"

#include "ui.h"
#include "ui_status.h"
#include "dialogs.h"           // WinLbox*
#include "dlg-drive.h"

#include <string.h>            // strlen
#include <stdlib.h>            // free
#include <direct.h>            // chdir

#include "lib.h"               // lib_msprintf
#include "drive.h"             // DRIVE_TYPE_*
#include "attach.h"            // file_system_*
#include "fliplist.h"          // FLIP_NEXT
#include "resources.h"         // resources_
#include "dlg-fileio.h"        // ViceFileDialog
#include "snippets\pmwin2.h"   // WinShowDlg

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static int toggle_drive_res(char *format, int drive)
{
    int res;
    char *tmp;

    tmp = lib_msprintf(format, drive + 8);

    resources_toggle(tmp, &res);

    lib_free(tmp);

    return res;
}

static int get_drive_res(char *format, int drive)
{
    int val;

    resources_get_int_sprintf(format, &val, drive + 8);

    return val;
}

#define nDRIVES 15

const char driveName[nDRIVES][28] = {
    "None",
    "1540, 5\"1/4, SS",
    "1541, 5\"1/4, SS",
    "1541-II, 5\"1/4 SS",
    "1551",
    "1571, 5\"1/4, DS",
    "1581, 3\"1/2, DS",
    "2000, 3\"1/2, HD/DS",
    "4000, 3\"1/2, ED/DS",
    "2031, 5\"1/4, SS, IEEE488",
    "2040, dual drive, 170k/disk",
    "3040, dual drive, 170k/disk",
    "4040, dual drive, 170k/disk",
    "1001, single drive, 1M/disk",
    "8050, dual drive, 0.5M/disk",
    "8250, dual drive, 1M/disk"
};

const int driveRes[nDRIVES] = {
    DRIVE_TYPE_NONE,
    DRIVE_TYPE_1540,
    DRIVE_TYPE_1541,
    DRIVE_TYPE_1541II,
    DRIVE_TYPE_1551,
    DRIVE_TYPE_1571,
    DRIVE_TYPE_1581,
    DRIVE_TYPE_2000,
    DRIVE_TYPE_4000,
    DRIVE_TYPE_2031,
    DRIVE_TYPE_2040,
    DRIVE_TYPE_3040,
    DRIVE_TYPE_4040,
    DRIVE_TYPE_1001,
    DRIVE_TYPE_8050,
    DRIVE_TYPE_8250
};

static MRESULT EXPENTRY pm_drive(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int drive = 0;

    switch (msg) {
        case WM_INITDLG:
            {
                int val, i = 0;
                HWND ihwnd = WinWindowFromID(hwnd, CBS_IMAGE);

                while (i < 10 && ui_status.imageHist[i][0]) {
                    WinLboxInsertItem(ihwnd, ui_status.imageHist[i++]);
                }
                WinLboxInsertItem(ihwnd, "");
                resources_get_int("DriveTrueEmulation", &val);
                WinCheckButton(hwnd, CB_TRUEDRIVE, val);
                WinCheckButton(hwnd, RB_DRIVE8|drive, 1);
                WinSendMsg(hwnd, WM_SWITCH, (void*)drive, 0);
                WinSendMsg(hwnd, WM_DRIVESTATE, (void*)ui_status.lastDriveState, NULL);
                for (i = 0; i < 3; i++) {
                    WinSendMsg(hwnd, WM_TRACK, (void*)i, (void*)(int)(ui_status.lastTrack[i] * 2));
                }
        }
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1)) {
            case PB_CREATE:
                create_dialog(hwnd);
                return FALSE;
            case PB_ATTACH:
                ViceFileDialog(hwnd, 0x0100 | (drive + 1), FDS_OPEN_DIALOG);
                return FALSE;
            case PB_DETACH:
                file_system_detach_disk(drive + 8);
                return FALSE;
            case PB_FLIPADD:
                fliplist_add_image(drive + 8);
                return FALSE;
            case PB_FLIPREMOVE:
                fliplist_remove(drive + 8, NULL);
                return FALSE;
            case PB_FLIP:
                fliplist_attach_head(drive + 8, FLIP_NEXT);
                return FALSE;
        }
        break;
    case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1)) {
                case RB_DRIVE8:
                case RB_DRIVE9:
                case RB_DRIVE10:
                case RB_DRIVE11:
                    WinSendMsg(hwnd, WM_SWITCH, (void*)(SHORT1FROMMP(mp1) & 0x3), 0);
                    break;
                case CB_TRUEDRIVE:
                    toggle("DriveTrueEmulation");
                    WinSendMsg(hwnd, WM_SWITCH, (void*)drive, 0);
                    break;
                case CB_CONVERTP00:
                    toggle_drive_res("FSDevice%dConvertP00", drive);//);
                    WinSendMsg(hwnd, WM_SWITCH, (void*)drive, 0);
                    break;
                case CB_SAVEP00:
                    toggle_drive_res("FileDevice%dSaveP00", drive);
                    break;
                case CB_ALLOWACCESS:
                    toggle_drive_res("FileSystemDevice%d", drive);
                    WinSendMsg(hwnd, WM_SWITCH, (void*)drive, 0);
                    break;
                case CB_HIDENONP00:
                    toggle_drive_res("FSDevice%dHideCBMFiles", drive);
                    break;
                case CB_MEM2000:
                    toggle_drive_res("Drive%dRAM2000", drive);
                    break;
                case CB_MEM4000:
                    toggle_drive_res("Drive%dRAM4000", drive);
                    break;
                case CB_MEM6000:
                    toggle_drive_res("Drive%dRAM6000", drive);
                    break;
                case CB_MEM8000:
                    toggle_drive_res("Drive%dRAM8000", drive);
                    break;
                case CB_MEMA000:
                    toggle_drive_res("Drive%dRAMA000", drive);
                    break;
                case CB_READONLY:
                    toggle_drive_res("AttachDevice%dReadonly", drive);
                    break;
                case CB_PARALLEL:
                    if (drive == 0 || drive == 1) {
                        toggle_drive_res("Drive%dParallelCable", drive);
                    }
                    break;
                case RB_NEVER:
                case RB_ASK:
                case RB_ALWAYS:
                    if (drive == 0 || drive == 1) {
                        resources_set_int_sprintf("Drive%dExtendImagePolicy", (SHORT1FROMMP(mp1) & 0x3), drive + 8);
                    }
                    break;
                case RB_NONE:
                case RB_TRAP:
                case RB_SKIP:
                    if (drive == 0 || drive == 1) {
                        resources_set_int_sprintf("Drive%dIdleMethod", (SHORT1FROMMP(mp1) & 0x3), drive + 8);
                    }
                    break;
               case CBS_IMAGE:
                   if (SHORT2FROMMP(mp1) == CBN_ENTER) {
                       char psz[CCHMAXPATH];

                       WinLboxQuerySelectedItemText(hwnd, CBS_IMAGE, psz, CCHMAXPATH);

                       if (!strlen(psz)) {
                           file_system_detach_disk(drive + 8);
                           return FALSE;
                       }

                       if (file_system_attach_disk(drive + 8, psz)) {
                           WinMessageBox(HWND_DESKTOP, hwnd, "Cannot attach specified file.", "VICE/2 Error", 0, MB_OK);
                       }
                   }
                   return FALSE;
                case CBS_PATH:
                    switch (SHORT2FROMMP(mp1)) {
                        case SPBN_CHANGE:
                            {
                                char path[255];

                                WinSendDlgMsg(hwnd, CBS_PATH, SPBM_QUERYVALUE, &path, 255);
                                if (!chdir(path)) {
                                    resources_set_string_sprintf("FSDevice%dDir", path, drive + 8);
                                }
                            }
                            break;
                        case SPBN_KILLFOCUS:
                            {
                                const char *path;

                                resources_get_string_sprintf("FSDevice%dDir", &path, drive + 8);
                                WinSendDlgMsg(hwnd, CBS_PATH, SPBM_SETARRAY, &path, 1);
                                WinSetDlgSpinVal(hwnd, CBS_PATH, 0);
                            }
                            break;
                    }
                    return FALSE;
                case CBS_TYPE:
                    if (SHORT2FROMMP(mp1) == CBN_ENTER && (drive == 0 || drive == 1)) {
                        const int nr  = WinQueryLboxSelectedItem((HWND)mp2);
                        const int val = WinLboxItemHandle((HWND)mp2, nr);

                        resources_set_int_sprintf("Drive%dType", val, drive + 8);
                    }
                    return FALSE;
            }
        }
        break;
    case WM_DRIVEIMAGE:
        {
            HWND ihwnd = WinWindowFromID(hwnd, CBS_IMAGE);
            const char *name = (char *)mp1;

            int pos;
            for (pos = 0; pos < 9; pos++) {
                WinDeleteLboxItem(ihwnd, 0);
            }

            pos = 0;
            while (pos < 10 && ui_status.imageHist[pos][0]) {
                WinLboxInsertItem(ihwnd, ui_status.imageHist[pos++]);
            }

            WinLboxInsertItem(ihwnd, "");

            if (drive == (int)mp2) {
                WinLboxSelectItem(ihwnd, name[0] ? 0 : pos);
            }
        }
        return FALSE;
    case WM_TRACK:
        if (!(ui_status.lastDriveState & (1 << (int)mp1))) {
            break;
        }

        WinSetDlgSpinVal(hwnd, SPB_TRACK8+(int)mp1, (int)((int)mp2 / 2));
        WinShowDlg(hwnd, SS_HALFTRACK8 + (int)mp1, ((int)mp2 % 2));
        break;
    case WM_DRIVELEDS:
        WinShowDlg(hwnd, SS_LED8+(int)mp1, (int)mp2);
        break;
    case WM_DRIVESTATE:
        WinShowDlg(hwnd, SPB_TRACK8, (int)mp1 & 1 == 1);
        WinShowDlg(hwnd, SS_HALFTRACK8, (int)mp1 & 1 == 1);
        WinShowDlg(hwnd, SPB_TRACK9, (int)mp1 & 2 == 1);
        WinShowDlg(hwnd, SS_HALFTRACK9, (int)mp1 & 2 == 1);
        WinShowDlg(hwnd, SS_LED8, 0);
        WinShowDlg(hwnd, SS_LED9, 0);
        break;
    case WM_SWITCH:
        drive = (int)mp1;
            {
                const HWND lbox = WinWindowFromID(hwnd, CBS_TYPE);
                int type = 0;
                int val;
                int res;
                int drive89 = (drive == 0 || drive == 1);

                resources_get_int("DriveTrueEmulation", &val);

                WinLboxEmpty(lbox);

                if (drive89) {
                    int i, nr;

                    WinCheckButton(hwnd, RB_NEVER | get_drive_res("Drive%dExtendImagePolicy", drive), 1);
                    WinCheckButton(hwnd, RB_NONE | get_drive_res("Drive%dIdleMethod", drive), 1);

                    nr = 0;
                    res = get_drive_res("Drive%dType", drive);
                    for (i = 0; i < nDRIVES; i++) {
                        if (!drive_check_type(driveRes[i], drive)) {
                            continue;
                        }

                        WinLboxInsertItem(lbox, driveName[i]);
                        WinLboxSetItemHandle(lbox, nr, driveRes[i]);

                        if (res == driveRes[i]) {
                            type = nr;
                        }

                        nr++;
                    }
                } else {
                    WinCheckButton(hwnd, RB_NEVER, 0);
                    WinCheckButton(hwnd, RB_ASK, 0);
                    WinCheckButton(hwnd, RB_ALWAYS, 0);
                    WinCheckButton(hwnd, RB_NONE, 0);
                    WinCheckButton(hwnd, RB_SKIP, 0);
                    WinCheckButton(hwnd, RB_TRAP, 0);
                }
                WinLboxSelectItem(lbox, type);

                WinCheckButton(hwnd, CB_PARALLEL, drive89 && get_drive_res("Drive%dParallelCable", drive) != 0);
                WinCheckButton(hwnd, CB_MEM2000, drive89 && get_drive_res("Drive%dRAM2000", drive) != 0);
                WinCheckButton(hwnd, CB_MEM4000, drive89 && get_drive_res("Drive%dRAM4000", drive) != 0);
                WinCheckButton(hwnd, CB_MEM6000, drive89 && get_drive_res("Drive%dRAM6000", drive) != 0);
                WinCheckButton(hwnd, CB_MEM8000, drive89 && get_drive_res("Drive%dRAM8000", drive) != 0);
                WinCheckButton(hwnd, CB_MEMA000, drive89 && get_drive_res("Drive%dRAMA000", drive) != 0);
                WinEnableControl(hwnd, CB_PARALLEL, drive89 && val);
                WinEnableControl(hwnd, RB_NEVER, drive89 && val);
                WinEnableControl(hwnd, RB_ASK, drive89 && val);
                WinEnableControl(hwnd, RB_ALWAYS, drive89 && val);
                WinEnableControl(hwnd, RB_NONE, drive89 && val);
                WinEnableControl(hwnd, RB_SKIP, drive89 && val);
                WinEnableControl(hwnd, RB_TRAP, drive89 && val);
                WinEnableControl(hwnd, CBS_TYPE, drive89 && val);
                WinEnableControl(hwnd, CB_MEM2000, drive89 && val);
                WinEnableControl(hwnd, CB_MEM4000, drive89 && val);
                WinEnableControl(hwnd, CB_MEM6000, drive89 && val);
                WinEnableControl(hwnd, CB_MEM8000, drive89 && val);
                WinEnableControl(hwnd, CB_MEMA000, drive89 && val);
                {
                    int acc  = get_drive_res("FileSystemDevice%d", drive) != 0;
                    int conv = get_drive_res("FSDevice%dConvertP00", drive) != 0;

                    if (!conv) {
                        resources_set_int_sprintf("FSDevice%dHideCBMFiles", 0, drive + 8);
                    }

                    WinCheckButton(hwnd, CB_ALLOWACCESS, acc);
                    WinCheckButton(hwnd, CB_CONVERTP00, conv);
                    WinCheckButton(hwnd, CB_SAVEP00, get_drive_res("FSDevice%dSaveP00", drive) != 0);
                    WinCheckButton(hwnd, CB_HIDENONP00, get_drive_res("FSDevice%dHideCBMFiles", drive) != 0);
                    WinEnableControl(hwnd, CB_ALLOWACCESS, !(drive89 && val));
                    WinEnableControl(hwnd, CB_SAVEP00, !(drive89 && val) && acc);
                    WinEnableControl(hwnd, CB_CONVERTP00, !(drive89 && val) && acc);
                    WinEnableControl(hwnd, CBS_PATH, !(drive89 && val) && acc);
                    WinEnableControl(hwnd, CB_HIDENONP00, !(drive89 && val) && acc && conv);

                    WinCheckButton(hwnd, CB_READONLY, get_drive_res("AttachDevice%dReadonly", drive) != 0);
                }
                {
                    char tmp[CCHMAXPATH];
                    int max = WinDlgLboxQueryCount(hwnd, CBS_IMAGE);
                    int pos = -1;

                    do {
                        WinLboxQueryItem(hwnd, CBS_IMAGE, ++pos, tmp, CCHMAXPATH);
                    }
                    while (pos < max && strcmp(ui_status.lastImage[drive], tmp));
                    WinDlgLboxSelectItem(hwnd, CBS_IMAGE, pos);
                }
                {
                    const char *path;
                    resources_get_string_sprintf("FSDevice%dDir", &path, drive + 8);
                    WinSendDlgMsg(hwnd, CBS_PATH, SPBM_SETARRAY, &path, 1);
                    WinSetDlgSpinVal(hwnd, CBS_PATH, 0);
                }
            }
            return FALSE;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

HWND hwndDrive=NULLHANDLE;

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void drive_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndDrive)) {
        return;
    }

    hwndDrive = WinLoadStdDlg(hwnd, pm_drive, DLG_DRIVE, NULL);
}
