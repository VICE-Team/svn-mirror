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

#define INCL_WINSTDFILE
#define INCL_WINBUTTONS

#include "vice.h"

#include <direct.h>
#include <string.h>

#include "ui.h"
#include "tape.h"
#include "attach.h"
#include "dialogs.h"

#define ID_LIST     0x1001

MRESULT EXPENTRY fnwpAttach(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first  =TRUE;
    static int suspend=FALSE;
    static char szFullFile[CCHMAXPATH];

    switch (msg)
    {
    case WM_DESTROY:
        delDlgOpen(DLGO_CONTENTS);
        first=TRUE;
        break;
    case WM_COMMAND: // 32 0x20
        if ((int)mp1==ID_LIST)
        {
            suspend=TRUE;
            WinDefFileDlgProc (hwnd, msg, (MPARAM)DID_OK, mp2);
            suspend=FALSE;
            contents_dialog(hwnd, szFullFile);
            return FALSE;
        }
        break;
    case WM_PAINT:
        if (first)
        {
            first = FALSE;
            WinCreateWindow(hwnd,                     /* Parent window       */
                            WC_BUTTON,                /* Class name          */
                            "Contents",               /* Window text         */
                            WS_VISIBLE|BS_PUSHBUTTON, /* Window style        */
                            274, 8,                   /* Position (x,y)      */
                            93, 28,                   /* Size (width,height) */
                            NULLHANDLE,               /* Owner window        */
                            HWND_TOP,                 /* Sibling window      */
                            ID_LIST,                  /* Window id           */
                            NULL,                     /* Control data        */
                            NULL);                    /* Pres parameters     */
        }
        break;
    case 4137:
        strcpy(szFullFile, mp1);
        if (suspend) return FALSE; // file nicht uebernehmen!
        break;
    }
    return WinDefFileDlgProc (hwnd, msg, mp1, mp2);
}

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void attach_dialog(HWND hwnd, int number)
{
    static char drive[3]="g:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource
    char   result [CCHMAXPATH];
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
    filedlg.cbSize      = sizeof(FILEDLG);               // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;  // FDS_CUSTOM
    filedlg.pszTitle    = number?"Attach disk image":"Attach tape image";
    filedlg.pszOKButton = "Attach";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = fnwpAttach;

    // filedlg.usDlgId = 0x1000;   // custom dialog id
    // filedlg.hMod = NULLHANDLE;  // handle to module containing <usDlgId>

    sprintf(filedlg.szFullFile, "%s\\%s", path,
            number?"*.d64*; *.d71*; *.d81*; *.g64*; *.x64*":"*.t64*; *.tap*"); // Init Path, Filter (*.t64)

    // Display the dialog and get the file
    if (!WinFileDlg(HWND_DESKTOP, hwnd, &filedlg))
        return;
    if (filedlg.lReturn!=DID_OK)
        return;

    if ((number?file_system_attach_disk(number, filedlg.szFullFile):tape_attach_image(filedlg.szFullFile)) < 0)
    {
        WinError(hwnd, "Cannot attach specified file.");
        return;
    }

    drive[0]=filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\')='\0';
    strcpy(path, filedlg.szFullFile+2);
}

