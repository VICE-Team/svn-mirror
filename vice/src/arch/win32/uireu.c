/*
 * uireu.c - Implementation of the REU settings dialog box.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include "vice.h"

#include <string.h>
#include <windows.h>

#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uireu.h"
#include "winmain.h"

/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

#define NUM_OF_REU_SIZE 8
static const int ui_reu_size[NUM_OF_REU_SIZE] = {
    128, 256, 512, 1024, 2048, 4096, 8192, 16384
};


static void init_reu_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    const char *reufile;
    int res_value_loop;
    int active_value;

    resources_get_value("REU", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_REU_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    temp_hwnd = GetDlgItem(hwnd,IDC_REU_SIZE);
    for (res_value_loop = 0; res_value_loop < NUM_OF_REU_SIZE;
        res_value_loop++) {
        char st[10];
        itoa(ui_reu_size[res_value_loop], st, 10);
        strcat(st, " kB");
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    resources_get_value("REUsize", (void *)&res_value);
    active_value = 0; /* default */
    for (res_value_loop = 0; res_value_loop < NUM_OF_REU_SIZE;
        res_value_loop++) {
        if (ui_reu_size[res_value_loop] == res_value)
            active_value = res_value_loop;
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    resources_get_value("REUfilename", (void *)&reufile);
    SetDlgItemText(hwnd, IDC_REU_FILE, reufile != NULL ? reufile : "");
}



static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;
    char s[MAX_PATH];

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_REU_BROWSE:
            {
                char name[1024] = "";
                OPENFILENAME ofn;

                memset(&ofn, 0, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.hInstance = winmain_instance;
                ofn.lpstrFilter = "All files (*.*)\0*.*\0";
                ofn.lpstrCustomFilter = NULL;
                ofn.nMaxCustFilter = 0;
                ofn.nFilterIndex = 1;
                ofn.lpstrFile = name;
                ofn.nMaxFile = sizeof(name);
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.lpstrTitle = "Select File for REU";
                ofn.Flags = (OFN_EXPLORER
                    | OFN_HIDEREADONLY
                    | OFN_NOTESTFILECREATE
                    | OFN_FILEMUSTEXIST
                    | OFN_SHAREAWARE
                    | OFN_ENABLESIZING);
                ofn.nFileOffset = 0;
                ofn.nFileExtension = 0;
                ofn.lpstrDefExt = NULL;

                if (GetSaveFileName(&ofn))
                    SetDlgItemText(hwnd, IDC_REU_FILE, name);
            }
            break;

          case IDOK:
            resources_set_value("REU", (resource_value_t)
                (IsDlgButtonChecked
                (hwnd, IDC_REU_ENABLE) == BST_CHECKED ?
                1 : 0 ));
            resources_set_value("REUsize",(resource_value_t)
                ui_reu_size[SendMessage(GetDlgItem(
                hwnd, IDC_REU_SIZE), CB_GETCURSEL, 0, 0)]);
                  
            GetDlgItemText(hwnd, IDC_REU_FILE, s, MAX_PATH);
            resources_set_value("REUfilename", (resource_value_t)s);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_reu_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}


void ui_reu_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_REU_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}

