/*
 * dlg-cartrisge.c - The cartridge-dialog.
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
#include <stdlib.h>            // free
#include <direct.h>            // chdir

#include "utils.h"             // xmsprintf
#include "cartridge.h"         // cartridge_attach_image
#include "snippets\filedlg.h"  // FILEDLG2

#define nTypesCrt 4

char *imgNameCrt[nTypesCrt]=
{
    "All Cardridge Files",
    "CRT",
    "BIN",
    "All Files"
};

char *imgExtCrt[nTypesCrt]=
{
    "*.crt; *.bin",
    "*.crt",
    "*.bin",
    ""
};

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void cartridge_dialog(HWND hwnd, const char *title, int type)
{
    static char drive[3]="f:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource

    char result [CCHMAXPATH];
    char dirname[CCHMAXPATH];
    int  rc;

    FILEDLG  filedlg;
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
    filedlg.pszTitle    = xmsprintf("Attach %s Cartridge Image", title);
    filedlg.pszOKButton = "Attach";
    filedlg.pszIDrive   = drive;
    filedlg.pfnDlgProc  = WinFileDlgProc;
    filedlg.ulUser      = (ULONG)&filedlg2;

    strcpy(filedlg.szFullFile, path);

    filedlg2.fN    = nTypesCrt;
    filedlg2.fExt  = imgExtCrt;
    filedlg2.fName = imgNameCrt;

    // Display the dialog and get the file
    rc=WinFileDialog(hwnd, &filedlg);
    free(filedlg.pszTitle);

    if (!rc || filedlg.lReturn!=DID_OK)
        return;

    if (cartridge_attach_image(type, filedlg.szFullFile) < 0)
    {
        WinMessageBox(HWND_DESKTOP, hwnd,
                      "Cannot attach cartridge image.", "VICE/2 Error",
                      0, MB_OK);
        return;
    }

    drive[0]=filedlg.szFullFile[0];
    *strrchr(filedlg.szFullFile,'\\')='\0';
    strcpy(path, filedlg.szFullFile+2);
}

char *crtsave_dialog(HWND hwnd)
{
    static char drive[3]="g:";
    static char path[CCHMAXPATH-2]="\\c64\\images\\vice2.crt";
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
    filedlg.pszTitle    = "Save Cartridge Image as";
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

