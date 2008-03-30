/*
 * dlg-autostart.c - The autostart-dialog.
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
#include "vice.h"

#include <os2.h>

#include <string.h>            // strrchr
#include <direct.h>            // chdir

#include "autostart.h"         // autostart_autodetect
#include "snippets\filedlg.h"  // FILEDLG2

#define nTypes 15

char *autoName[nTypes]=
{
    "All Disk Images",
    "All Tape Images",
    "Raw Program File",
    "Vice/2 Snapshot File",
    "All 1541 Images",
    "1541",
    "1541",
    "1541",
    "1581",
    "8050",
    "1581",
    "8250",
    "T64",
    "Raw 1531 Tape File",
    "All Files"
};

char *autoExt[nTypes]=
{
    "*.d64*; *.d71*; *.d80*; *.d81.*; *.d82*; *.g64*; *.x64*",
    "*.t64*; *.tap*",
    "*.p??",
    "*.vsf",
    "*.d64; *.g64*; *.x64*",
    "*.d64*",
    "*.g64*",
    "*.x64*",
    "*.d71*",
    "*.d80*",
    "*.d81*",
    "*.d82*",
    "*.t64*",
    "*.tap*",
    ""
};

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void autostart_dialog(HWND hwnd)
{
    static char drive[3]="f:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource
    char   result [CCHMAXPATH];
    char   dirname[CCHMAXPATH];
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

    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG);               // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;  // FDS_CUSTOM
    filedlg.pszTitle    = "Autostart image";
    filedlg.pszOKButton = "Autostart";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = WinFileDlgProc;
    filedlg.ulUser      = (ULONG)&filedlg2;

    strcpy(filedlg.szFullFile, path);

    filedlg2.fN    = nTypes;
    filedlg2.fExt  = autoExt;
    filedlg2.fName = autoName;

    // Display the dialog and get the file
    if (!WinFileDialog(hwnd, &filedlg))
        return;

    if (filedlg.lReturn!=DID_OK)
        return;

    if (autostart_autodetect(filedlg.szFullFile, NULL, 0) < 0)
    {
        WinMessageBox(HWND_DESKTOP, hwnd,
                      "Cannot autostart specified file.", "VICE/2 Error",
                      0, MB_OK);
        return;
    }

    drive[0]=filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\')='\0';
    strcpy(path, filedlg.szFullFile+2);
}

