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

#define INCL_WINSYS         // PP_FONTNAMESIZE
#define INCL_WINSTDFILE
#define INCL_WINDIALOGS     // WinSendDlgItemMsg
#define INCL_WINWINDOWMGR   // QWL_USER
#define INCL_WINLISTBOXES   // Lbox
#define INCL_WINENTRYFIELDS // CBS_DROPDOWNLIST

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filedlg.h"
#include "lib.h"

MRESULT EXPENTRY WinFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int i;
                SWP swp;
                HWND lbox;
                char font[0x100] = "6.System VIO";
                const HWND filter = WinWindowFromID(hwnd, DID_FILTER_CB);
                const FILEDLG *fdlg = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);
                const FILEDLG2 *list = (FILEDLG2*)fdlg->ulUser;

                if (!list) {
                    break;
                }

                WinQueryWindowPos(filter, &swp);
                WinShowWindow(filter, FALSE);

                lbox = WinCreateWindow(hwnd, WC_COMBOBOX, "", CBS_DROPDOWNLIST | WS_TABSTOP | WS_VISIBLE, swp.x, swp.y, swp.cx, swp.cy, hwnd, HWND_TOP, DID_FILE_FILTER, NULL, NULL);

                WinQueryPresParam(filter, PP_FONTNAMESIZE, NULLHANDLE, NULL, 0x100, font, 0);
                WinSetPresParam(lbox, PP_FONTNAMESIZE, strlen(font) + 1, font);

                //
                // fill entries in combobox
                //
                for (i = 0; i < list->fN; i++) {
                    int len = strlen(list->fName[i]) + strlen(list->fExt[i]) + 5;
                    char *txt = malloc(len);

                    sprintf(txt, "<%s> %s", list->fName[i], list->fExt[i]);
                    WinInsertLboxItem(lbox, LIT_END, txt);
                    lib_free(txt);
                }

                //
                // select first entry
                //
                WinSendMsg(lbox, LM_SELECTITEM, 0, (void*)TRUE);
            }
            break;
        case WM_CONTROL:
            if (mp1 == MPFROM2SHORT(DID_FILE_FILTER, CBN_ENTER)) {
                const HWND name = WinWindowFromID(hwnd, DID_FILENAME_ED);
                //
                // Get new selection
                //
                const int item = WinQueryLboxSelectedItem((HWND)mp2);
                const int len = WinQueryLboxItemTextLength((HWND)mp2, item);

                //
                // set corresponding text in entry field
                //
                char *txt = malloc(len);

                WinQueryLboxItemText((HWND)mp2, item, txt, len);
                WinSetWindowText(name, strrchr(txt, '>') + 1);
                lib_free(txt);

                //
                // set focus to entry field and simulate an Apply
                //
                WinSetFocus(HWND_DESKTOP, name);
                WinDefFileDlgProc(hwnd, WM_COMMAND, (MPARAM)DID_OK, MPFROM2SHORT(CMDSRC_PUSHBUTTON, 0));
            }
            break;
    }
    return WinDefFileDlgProc (hwnd, msg, mp1, mp2);
}

HWND WinFileDialog(HWND hwnd, FILEDLG *filedlg)
{
    const FILEDLG2 *dlg2 = (FILEDLG2*)filedlg->ulUser;

    strcat(strcat(filedlg->szFullFile, "\\"), dlg2->fExt[0]);

    // Display the dialog and get the file
    return WinFileDlg(HWND_DESKTOP, hwnd, filedlg);
}
