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

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>

#include "intl.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "uiide64.h"
#include "uilib.h"
#include "winmain.h"


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
    TCHAR *st;
    HWND ide64_hwnd;
    int cylinders_idx, heads_idx, sectors_idx, total;

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_CYLINDERS);
    cylinders_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_HEADS);
    heads_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    ide64_hwnd = GetDlgItem(hwnd, IDC_IDE64_SECTORS);
    sectors_idx = SendMessage(ide64_hwnd, CB_GETCURSEL, 0, 0);

    total = (cylinders_idx + 1) * (heads_idx + 1) * sectors_idx / 2;

    str = lib_msprintf(intl_translate_text(IDS_TOTAL_SIZE_I_KB), total);
    st = system_mbstowcs_alloc(str);
    SetDlgItemText(hwnd, IDC_IDE64_SIZE, st);
    system_mbstowcs_free(st);
    lib_free(str);
}

static void init_ide64_dialog(HWND hwnd)
{
    int res_value, index;
    const char *ide64file;
    TCHAR *st_ide64file;
    TCHAR memb[20];
    HWND ide64_hwnd;

    resources_get_value("IDE64Image", (void *)&ide64file);
    st_ide64file = system_mbstowcs_alloc(ide64file);
    SetDlgItemText(hwnd, IDC_IDE64_HDIMAGE_FILE,
                   st_ide64file != NULL ? st_ide64file : TEXT(""));
    system_mbstowcs_free(st_ide64file);

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
    TCHAR st[MAX_PATH];
    HWND ide64_hwnd;
    int res_value;

    GetDlgItemText(hwnd, IDC_IDE64_HDIMAGE_FILE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
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
            uilib_select_browse(hwnd, TEXT(intl_translate_text(IDS_SELECT_HD_IMAGE)),
                                UILIB_FILTER_ALL,
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_IDE64_HDIMAGE_FILE);
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
    DialogBox(winmain_instance, (LPCTSTR)intl_translate(IDD_IDE64_SETTINGS_DIALOG), hwnd,
              dialog_proc);
}

