/*
 * uireu.c - Implementation of the REU settings dialog box.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <windows.h>
#include <tchar.h>

#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "uireu.h"
#include "winmain.h"


#define NUM_OF_REU_SIZE 8
static const int ui_reu_size[NUM_OF_REU_SIZE] = {
    128, 256, 512, 1024, 2048, 4096, 8192, 16384
};


static void enable_reu_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_REU_ENABLE)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_REU_SIZE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_REU_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_REU_FILE), is_enabled);
}

static void init_reu_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    const char *reufile;
    TCHAR *st_reufile;
    int res_value_loop;
    int active_value;

    resources_get_value("REU", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_REU_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    temp_hwnd = GetDlgItem(hwnd, IDC_REU_SIZE);
    for (res_value_loop = 0; res_value_loop < NUM_OF_REU_SIZE;
        res_value_loop++) {
        TCHAR st[10];
        _itot(ui_reu_size[res_value_loop], st, 10);
        _tcscat(st, TEXT(" kB"));
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
    st_reufile = system_mbstowcs_alloc(reufile);
    SetDlgItemText(hwnd, IDC_REU_FILE,
                   reufile != NULL ? st_reufile : TEXT(""));
    system_mbstowcs_free(st_reufile);

    enable_reu_controls(hwnd);
}

static void end_reu_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    resources_set_value("REU", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_REU_ENABLE) == BST_CHECKED ?
                        1 : 0 ));
    resources_set_value("REUsize",(resource_value_t)
                        ui_reu_size[SendMessage(GetDlgItem(
                        hwnd, IDC_REU_SIZE), CB_GETCURSEL, 0, 0)]);

    GetDlgItemText(hwnd, IDC_REU_FILE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("REUfilename", (resource_value_t)s);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_REU_BROWSE:
            uilib_select_browse(hwnd, TEXT("Select File for REU"),
                                UILIB_FILTER_ALL,
                                UILIB_SELECTOR_TYPE_FILE_SAVE, IDC_REU_FILE);
            break;
          case IDC_REU_ENABLE:
            enable_reu_controls(hwnd);
            break;
          case IDOK:
            end_reu_dialog(hwnd);
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

