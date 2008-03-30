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

#define INCL_WINSTATICS
#define INCL_WINSTDFILE
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#include "vice.h"

#include <direct.h>
#include <string.h>

#include "ui.h"
#include "drive.h"
#include "vdrive.h"
#include "dialogs.h"
#include "charsets.h"
#include "diskimage.h"

#define nTYPES 7

const char imgType[nTYPES][5]=
{
    ".d64",
    ".g64",
    ".x64",
    ".d71",
    ".d80",
    ".d81",
    ".d82",
};

const int imgRes[nTYPES]=
{
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_X64,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D82
};

#define CBS_IMGTYPE  0x1001
#define EF_NAME      0x1002

MRESULT EXPENTRY fnwpCreate(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    switch (msg)
    {
    case WM_DESTROY:
        {
            int  *type = (int*)((FILEDLG*)WinQueryWindowPtr(hwnd,QWL_USER))->ulUser;
            char *name = (char*)type+sizeof(int);
            WinQueryWindowText(WinWindowFromID(hwnd, EF_NAME), 17, name);
            *type = WinLboxQuerySelectedItem(hwnd, CBS_IMGTYPE);
        }
        first=TRUE;
        break;
    case WM_PAINT:
        if (first)
        {
            int i;
            first = FALSE;
            WinCreateWindow(hwnd,                     /* Parent window       */
                            WC_STATIC,                /* Class name          */
                            "Name:",                  /* Window text         */
                            SS_TEXT | WS_VISIBLE,     /* Window style        */
                            17, 48,                   /* Position (x,y)      */
                            50, 14,                   /* Size (width,height) */
                            NULLHANDLE,               /* Owner window        */
                            HWND_TOP,                 /* Sibling window      */
                            ID_NONE,                  /* Window id           */
                            NULL,                     /* Control data        */
                            NULL);                    /* Pres parameters     */
            WinCreateWindow(hwnd,                     /* Parent window       */
                            WC_ENTRYFIELD,            /* Class name          */
                            "",                       /* Window text         */
                            ES_MARGIN | WS_VISIBLE,   /* Window style        */
                            70, 46,                   /* Position (x,y)      */
                            155, 16,                  /* Size (width,height) */
                            NULLHANDLE,               /* Owner window        */
                            HWND_TOP,                 /* Sibling window      */
                            EF_NAME,                  /* Window id           */
                            NULL,                     /* Control data        */
                            NULL);                    /* Pres parameters     */
            WinCreateWindow(hwnd,                     /* Parent window       */
                            WC_STATIC,                /* Class name          */
                            "Type:",                  /* Window text         */
                            SS_TEXT | WS_VISIBLE,     /* Window style        */
                            255, 46,                  /* Position (x,y)      */
                            45, 16,                   /* Size (width,height) */
                            NULLHANDLE,               /* Owner window        */
                            HWND_TOP,                 /* Sibling window      */
                            -1,                       /* Window id           */
                            NULL,                     /* Control data        */
                            NULL);                    /* Pres parameters     */
            WinCreateWindow(hwnd,                     /* Parent window       */
                            WC_COMBOBOX,              /* Class name          */
                            "",                       /* Window text         */
                            CBS_DROPDOWNLIST |
                            //                            WS_GROUP | WS_TABSTOP |
                            WS_VISIBLE,               /* Window style        */
                            300, 2,                   /* Position (x,y)      */
                            50, 62,                   /* Size (width,height) */
                            NULLHANDLE,               /* Owner window        */
                            HWND_TOP,                 /* Sibling window      */
                            CBS_IMGTYPE,              /* Window id           */
                            NULL,                     /* Control data        */
                            NULL);                    /* Pres parameters     */
            for (i=0; i<nTYPES; i++)
                WinLboxInsertItem(hwnd, CBS_IMGTYPE, imgType[i]+1);
            WinLboxSelectItem(hwnd, CBS_IMGTYPE, 0);
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
    static char drive[3]="g:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource
    char   result [21];
    char   dirname[CCHMAXPATH];
    FILEDLG filedlg;                     // File dialog info structure

    _getcwd(dirname, CCHMAXPATH);        // store working dir

    strcat(strcpy(result, drive),path);
    if (chdir(result))                   // try if actual image dir exist
    {                                    // if it doesn't exist, set
        drive[0]=dirname[0];             // imagedir to working dir
        drive[1]=':';                    // maybe drive is empty at first call
        strcpy(path, dirname+2);
    }
    chdir(dirname);                      // change back to working dir

    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG);                 // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_SAVEAS_DIALOG;  // FDS_CUSTOM
    filedlg.pszTitle    = "Create new disk image";
    filedlg.pszOKButton = "Create";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = fnwpCreate;
    filedlg.ulUser      = (ULONG)result;

    strcat(strcpy(filedlg.szFullFile, path), "\\");

    // Display the dialog and get the file
    if (!WinFileDlg(HWND_DESKTOP, hwnd, &filedlg))
        return;
    if (filedlg.lReturn!=DID_OK)
        return;

    {
        int type  = *((int*)filedlg.ulUser);
        char *ext = filedlg.szFullFile+strlen(filedlg.szFullFile)-4;
        if (!(ext<filedlg.szFullFile))
            if (strcmpi(ext, imgType[type]))
                if (strlen(filedlg.szFullFile)<CCHMAXPATH-5)
                    strcat(filedlg.szFullFile, imgType[type]);

        if (vdrive_internal_create_format_disk_image(filedlg.szFullFile,
                                                     a2p((char*)filedlg.ulUser+sizeof(int)),
                                                     imgRes[type]))
        {
            WinError(hwnd, "Unable to create new diskimage.");
            return;
        }
    }
    drive[0]=filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\')='\0';
    strcpy(path, filedlg.szFullFile+2);
}

