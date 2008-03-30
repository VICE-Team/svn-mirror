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

#define INCL_WINSTDFILE        // FILEDLG
#define INCL_WINBUTTONS        // WC_BUTTON
#define INCL_WINWINDOWMGR      // QWL_USER
#include "vice.h"

#include <string.h>            // strrchr
#include <stdlib.h>            // free
#include <direct.h>            // chdir

#include "tape.h"              // tape_attach_image
#include "utils.h"             // xmsprintf
#include "attach.h"            // file_system_attach_disk
#include "dialogs.h"           // WinQueryDlgPos
#include "snippets\filedlg.h"  // FILEDLG2

#define nTypesDsk 10

char *imgNameDsk[nTypesDsk]=
{
    "All Disk Images",
    "All 1541 Images",
    "1541",
    "1541",
    "1541",
    "1581",
    "8050",
    "1581",
    "8250",
    "All Files"
};

char *imgExtDsk[nTypesDsk]=
{
    "*.d64*; *.d71*; *.d80*; *.d81.*; *.d82*; *.g64*; *.x64*",
    "*.d64; *.g64*; *.x64*",
    "*.d64*",
    "*.g64*",
    "*.x64*",
    "*.d71*",
    "*.d80*",
    "*.d81*",
    "*.d82*",
    ""
};

#define nTypesTap 4

char *imgNameTap[nTypesTap]=
{
    "All Tape Images",
    "T64",
    "Raw 1531 Tape File",
    "All Files"
};

char *imgExtTap[nTypesTap]=
{
    "*.t64*; *.tap*",
    "*.t64*",
    "*.tap*",
    ""
};

#define ID_LIST 0x1000

MRESULT EXPENTRY fnwpAttach(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int type=0;
    static int suspend=FALSE;
    static char szFullFile[CCHMAXPATH];

    switch (msg)
    {
    case WM_INITDLG:
        {
            SWP swp1, swp2;

            const FILEDLG  *dlg  = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);
            const FILEDLG2 *dlg2 = (FILEDLG2*)dlg->ulUser;

            if (!dlg2->fUser)
                break;

            WinQueryDlgPos(hwnd, DID_OK,     &swp1);
            WinQueryDlgPos(hwnd, DID_CANCEL, &swp2);

            WinCreateStdDlg(hwnd, ID_LIST, WC_BUTTON, WS_VISIBLE|BS_PUSHBUTTON,
                            "Contents",
                            2*swp2.x-swp1.x, swp1.y,
                            swp1.cx, swp1.cy);
        }
        break;

    case WM_COMMAND: // 32 0x20
        if ((int)mp1==ID_LIST)
        {
            //
            // Send a dummy Apply to get filename from validation msg
            //
            suspend=TRUE;
            WinDefFileDlgProc(hwnd, WM_COMMAND, (MPARAM)DID_OK, mp2);
            suspend=FALSE;

            //
            // now we have the file name: call the contents dialog.
            //
            contents_dialog(hwnd, szFullFile);
            return FALSE;
        }
        break;

    case FDM_VALIDATE: // 4137=0x1000+41=WM_USER+41
        //
        // not a dummy: file is valid // FIXME? check for REAL validity?
        //
        if (!suspend)
            break;

        //
        // Dummy apply: don't close the dialog
        //
        strcpy(szFullFile, mp1);
        return FALSE;
    }
//    return WinDefFileDlgProc (hwnd, msg, mp1, mp2);
    return WinFileDlgProc(hwnd, msg, mp1, mp2);
}

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void attach_dialog(HWND hwnd, int number)
{
    static char drive[3]="f:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource
    char   result [CCHMAXPATH];
    char   dirname[CCHMAXPATH];
    char  *text;
    char  *title;
    int    rc;
    FILEDLG  filedlg;                     // File dialog info structure
    FILEDLG2 filedlg2;

    _getcwd(dirname, CCHMAXPATH);        // store working dir

    strcat(strcpy(result, drive),path);
    if (chdir(result))                   // try if actual image dir exist
    {                                    // if it doesn't exist, set
        drive[0]=dirname[0];             // imagedir to working dir
        drive[1]=':';                    // maybe drive is empty at first call
        strcpy(path, dirname+2);
    }
    chdir(dirname);                      // change back to working dir

    text = xmsprintf("Attach %s image to ", number?"disk":"tape");
    free(text);

    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG);               // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;  // FDS_CUSTOM
    filedlg.pszOKButton = "Attach";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = fnwpAttach;
    filedlg.ulUser      = (ULONG)&filedlg2;
    filedlg.pszTitle    = number?
        xmsprintf("%sdrive #%d", text, number):
        xmsprintf("%sdatasette", text);

    strcpy(filedlg.szFullFile, path);

    filedlg2.fUser = (void*)number;
    filedlg2.fN    = number?nTypesDsk:nTypesTap;
    filedlg2.fExt  = number?imgExtDsk:imgExtTap;
    filedlg2.fName = number?imgNameDsk:imgNameTap;

    // Display the dialog and get the file
    rc=WinFileDialog(hwnd, &filedlg);
    free(filedlg.pszTitle);

    if (!rc || filedlg.lReturn!=DID_OK)
        return;

    if ((number?file_system_attach_disk(number, filedlg.szFullFile):tape_attach_image(filedlg.szFullFile)) < 0)
    {
        WinMessageBox(HWND_DESKTOP, hwnd,
                      "Cannot attach specified file.", "VICE/2 Error",
                      0, MB_OK);
        return;
    }

    drive[0]=filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\')='\0';
    strcpy(path, filedlg.szFullFile+2);
}

