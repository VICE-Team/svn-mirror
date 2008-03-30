/*
 * dlg-snapshot.c - The snapshot-dialog.
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
#define INCL_WINBUTTONS

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "log.h"
#include "dialogs.h"
#include "resources.h"

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

char *snapshot_dialog(HWND hwnd)
{
    static char drive[3]="g:";
    static char path[CCHMAXPATH-2]="\\c64\\images\\vice2.vsf";
    char dirname[CCHMAXPATH];
    static FILEDLG filedlg;                     // File dialog info structure

    _getcwd(dirname, CCHMAXPATH);        // store working dir

    strcat(strcpy(filedlg.szFullFile, drive),path);
    *strrchr(filedlg.szFullFile,'\\')='\0';
    if (chdir(filedlg.szFullFile))       // try if actual image dir exist
    {                                    // if it doesn't exist, set
        drive[0]=dirname[0];             // imagedir to working dir
        drive[1]=':';                    // maybe drive is empty at first call
        strcpy(path, dirname+2);
    }
    chdir(dirname);                      // change back to working dir

    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG);               // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_SAVEAS_DIALOG | FDS_ENABLEFILELB;  // FDS_CUSTOM
    filedlg.pszTitle    = "Save Snapshot as";
    filedlg.pszOKButton = "Apply";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = NULL;

    sprintf(filedlg.szFullFile, "%s", path);

    // Display the dialog and get the file
    if (!WinFileDlg(HWND_DESKTOP, hwnd, &filedlg))
        return NULL;
    if (filedlg.lReturn!=DID_OK)
        return NULL;

    drive[0]=filedlg.szFullFile[0];
    strcpy(path, filedlg.szFullFile+2);

    return filedlg.szFullFile;
}

