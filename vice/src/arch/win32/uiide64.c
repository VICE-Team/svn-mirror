/*
 * uiide64.c - Implementation of the IDE64 settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <tchar.h>
#include <windows.h>

#include "lib.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uiide64.h"
#include "winmain.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif


static void enable_ide64_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_TOGGLE_IDE64_SIZEAUTODETECT)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_IDE64_CYLINDERS), !is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_IDE64_HEADS), !is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_IDE64_SECTORS), !is_enabled);
}

static void update_text(HWND hwnd)
{
    char *str;
    HWND ide64_hwnd;
    int cylinders_idx, heads_idx, sectors_idx, total;

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_CYLINDERS);
    cylinders_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_HEADS);
    heads_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_SECTORS);
    sectors_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    total = (cylinders_idx + 1) * (heads_idx + 1) * sectors_idx / 2;

    str = lib_msprintf("Total size: %iKB", total);
    SetDlgItemText(hwnd, IDC_IDE64_SIZE, str);
    lib_free(str);
}

static void init_ide64_dialog(HWND hwnd)
{
    int res_value, index;
    const char *ide64file;
    TCHAR memb[20];
    HWND ide64_hwnd;

    resources_get_value("IDE64Image", (void *)&ide64file);
    SetDlgItemText(hwnd, IDC_IDE64_HDIMAGE_FILE,
                   ide64file != NULL ? ide64file : "");

    resources_get_value("IDE64AutodetectSize", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_TOGGLE_IDE64_SIZEAUTODETECT, res_value
                   ? BST_CHECKED : BST_UNCHECKED);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_CYLINDERS);
    for (index = 1; index <= 1024; index++) {
        _stprintf(memb, TEXT("%d"), index);
        SendMessage(ide64_hwnd, CB_ADDSTRING, 0, (LPARAM)memb);
    }
    resources_get_value("IDE64Cylinders", (void *)&res_value);
    SendMessage(ide64_hwnd, CB_SETCURSEL, (WPARAM)(res_value - 1), 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_HEADS);
    for (index = 1; index <= 16; index++) {
        _stprintf(memb, TEXT("%d"), index);
        SendMessage(ide64_hwnd, CB_ADDSTRING, 0, (LPARAM)memb);
    }
    resources_get_value("IDE64Heads", (void *)&res_value);
    SendMessage(ide64_hwnd, CB_SETCURSEL, (WPARAM)(res_value - 1), 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_SECTORS);
    for (index = 0; index < 64; index++) {
        _stprintf(memb, TEXT("%d"), index);
        SendMessage(ide64_hwnd, CB_ADDSTRING, 0, (LPARAM)memb);
    }
    resources_get_value("IDE64Sectors", (void *)&res_value);
    SendMessage(ide64_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    update_text(hwnd);
    enable_ide64_controls(hwnd);
}

static void end_ide64_dialog(HWND hwnd)
{
    char s[MAX_PATH];
    HWND ide64_hwnd;
    int res_value;

    GetDlgItemText(hwnd, IDC_IDE64_HDIMAGE_FILE, s, MAX_PATH);
    resources_set_value("IDE64Image", (resource_value_t)s);

    resources_set_value("IDE64AutodetectSize",
                        (resource_value_t)(IsDlgButtonChecked
                        (hwnd, IDC_TOGGLE_IDE64_SIZEAUTODETECT)
                        == BST_CHECKED ? 1 : 0));

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_CYLINDERS);
    res_value = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);
    resources_set_value("IDE64Cylinders", (resource_value_t)(res_value + 1));

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_HEADS);
    res_value = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);
    resources_set_value("IDE64Heads", (resource_value_t)(res_value + 1));

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_SECTORS);
    res_value = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);
    resources_set_value("IDE64Sectors", (resource_value_t)res_value);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_IDE64_HDIMAGE_BROWSE:
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
                ofn.lpstrTitle = "Select HD image file";
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
                    SetDlgItemText(hwnd, IDC_IDE64_HDIMAGE_FILE, name);
            }
            break;
          case IDC_TOGGLE_IDE64_SIZEAUTODETECT:
            enable_ide64_controls(hwnd);
            break;
          case IDC_IDE64_CYLINDERS:
          case IDC_IDE64_HEADS:
          case IDC_IDE64_SECTORS:
            update_text(hwnd);
            break;
          case IDOK:
            end_ide64_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_ide64_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void uiide64_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_IDE64_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}

