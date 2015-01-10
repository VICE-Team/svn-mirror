/*
 * dlg-create.c - The create-dialog.
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

#define INCL_WINSTATICS       // SS_TEXT
#define INCL_WINSTDFILE       // FILEDLG
#define INCL_WINLISTBOXES     // Lbox
#define INCL_WINWINDOWMGR     // QWL_USER
#define INCL_WINENTRYFIELDS   // WC_ENTRYFIELD

#include "vice.h"

#include <os2.h>

#include <direct.h>           // chdir
#include <string.h>           // strrchr

#include "vdrive-internal.h"
#include "dialogs.h"          // WinLbox*
#include "charset.h"          // a2p, p2a
#include "diskimage.h"        // DISK_IMAGE_TYPE_*
#include "snippets\pmwin2.h"  // WinQueryDlgPos

#define nTYPES 12

const char imgType[nTYPES][13] = {
    "<1541> *.d64",
    "<1541> *.g64",
    "<1541> *.p64",
    "<1541> *.x64",
    "<1571> *.d71",
    "<1581> *.d81",
    "<2040> *.d67",
    "<8050> *.d80",
    "<8250> *.d82",
    "<2000> *.d1m",
    "<2000> *.d2m",
    "<4000> *.d4m"
};

const int imgRes[nTYPES] = {
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_P64,
    DISK_IMAGE_TYPE_X64,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D67,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D82,
    DISK_IMAGE_TYPE_D1M,
    DISK_IMAGE_TYPE_D2M,
    DISK_IMAGE_TYPE_D4M
};

#define CBS_IMGTYPE 0x1001
#define EF_NAME     0x1002

MRESULT EXPENTRY fnwpCreate(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int i;
                SWP swp1, swp2, swp3, swp4, swp5, swp6, swp7;

                //
                // create entryfield and corresponding text
                //
                WinQueryDlgPos(hwnd, DID_FILTER_TXT, &swp1);
                WinShowDlg(hwnd, DID_FILTER_TXT, FALSE);
                WinCreateStdDlg(hwnd, ID_NONE, WC_STATIC, SS_TEXT | WS_VISIBLE, "Diskette Name (name[,ext]):", swp1.x, swp1.y, 250, swp1.cy);
                WinQueryDlgPos(hwnd, DID_FILTER_CB, &swp2);
                WinQueryDlgPos(hwnd, DID_FILENAME_ED, &swp3);
                WinShowDlg(hwnd, DID_FILTER_CB, FALSE);
                WinCreateStdDlg(hwnd, EF_NAME, WC_ENTRYFIELD, ES_MARGIN | ES_AUTOSCROLL, "", swp3.x, swp2.y+swp2.cy - swp3.cy, 0, 0);

                //
                // correct for ES_MARGIN
                //
                WinQueryDlgPos(hwnd, EF_NAME, &swp4);
                WinSetDlgPos(hwnd, EF_NAME, 0, swp3.x + swp4.cx / 2, swp2.y + swp2.cy - swp3.cy + swp4.cy / 2, 2 * (swp2.cx - swp4.cx) / 3, swp3.cy - swp4.cy, SWP_SIZE | SWP_MOVE | SWP_SHOW);

                //
                // create combobox and corresoponding text
                //
                WinQueryDlgPos(hwnd, EF_NAME, &swp5);
                WinQueryDlgPos(hwnd, DID_DIRECTORY_LB, &swp6);
                WinQueryDlgPos(hwnd, DID_FILES_LB, &swp7);
                swp5.x += swp5.cx;
                swp6.x += swp6.cx;
                WinCreateStdDlg(hwnd, ID_NONE, WC_STATIC, SS_TEXT | WS_VISIBLE, "Type:", swp5.x - swp6.x + swp7.x, swp1.y, 45, swp1.cy);

                WinCreateStdDlg(hwnd, CBS_IMGTYPE, WC_COMBOBOX, CBS_DROPDOWNLIST | WS_TABSTOP | WS_VISIBLE, "", swp5.x + swp7.x - swp6.x, swp2.y, swp3.cx - swp5.cx - swp7.x + swp6.x, swp2.cy);

                //
                // fill entries in combobox
                //
                for (i = 0; i < nTYPES; i++) {
                    WinDlgLboxInsertItem(hwnd, CBS_IMGTYPE, imgType[i]);
                }

                //
                // select first entry
                //
                WinDlgLboxSelectItem(hwnd, CBS_IMGTYPE, 0);
            }
            break;
        case WM_DESTROY:
            {
                int *type = (int*)((FILEDLG*)WinQueryWindowPtr(hwnd,QWL_USER))->ulUser;
                char *name = (char*)type + sizeof(int);

                *type = WinDlgLboxSelectedItem(hwnd, CBS_IMGTYPE);

                WinQueryDlgText(hwnd, EF_NAME, name, 20);
            }
            break;
    }
    return WinDefFileDlgProc (hwnd, msg, mp1, mp2);
}

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void create_dialog(HWND hwnd)
{
    static char drive[3] = "g:";
    static char path[CCHMAXPATH-2] = "\\c64\\images";
    char result[24];
    char dirname[CCHMAXPATH];
    FILEDLG filedlg;                     // File dialog info structure

    _getcwd(dirname, CCHMAXPATH);        // store working dir

    strcat(strcpy(result, drive),path);
    // try if actual image dir exist, if it doesn't exist, set
    // imagedir to working dir, maybe drive is empty at first call
    if (chdir(result)) {
        drive[0] = dirname[0];
        drive[1] = ':';
        strcpy(path, dirname + 2);
    }
    chdir(dirname);                      // change back to working dir

    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize = sizeof(FILEDLG);                 // Size of structure
    filedlg.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;      // FDS_CUSTOM
    filedlg.pszTitle = "Create new disk image";
    filedlg.pszOKButton = "Create";
    filedlg.pszIDrive = drive;
    filedlg.pfnDlgProc = fnwpCreate;
    filedlg.ulUser = (ULONG)result;

    strcat(strcpy(filedlg.szFullFile, path), "\\");

    // Display the dialog and get the file
    if (!WinFileDlg(HWND_DESKTOP, hwnd, &filedlg)) {
        return;
    }

    if (filedlg.lReturn != DID_OK) {
        return;
    }

    {
        int type  = *((int*)filedlg.ulUser);
        char *ext = filedlg.szFullFile + strlen(filedlg.szFullFile) - 4;

        if (!(ext<filedlg.szFullFile)) {
            if (strcmpi(ext, imgType[type] + 8)) {
                if (strlen(filedlg.szFullFile) < CCHMAXPATH - 5) {
                    strcat(filedlg.szFullFile, imgType[type] + 8);
                }
            }
        }

        if (vdrive_internal_create_format_disk_image(filedlg.szFullFile, a2p((char*)filedlg.ulUser + sizeof(int)), imgRes[type])) {
            ViceErrorDlg(hwnd, PTR_INFO, " Create Image:\n Cannot create a new disk image.");
            return;
        }
    }
    drive[0] = filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\') = '\0';
    strcpy(path, filedlg.szFullFile + 2);
}
