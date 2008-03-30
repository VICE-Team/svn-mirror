/*
 * uiplus60k.c - Implementation of the +60K EXPANSION HACK settings dialog box.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "translate.h"
#include "uilib.h"
#include "uiplus60k.h"
#include "winmain.h"

static void enable_plus60k_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_PLUS60K_ENABLE)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_PLUS60K_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PLUS60K_FILE), is_enabled);
}

static void init_plus60k_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    const char *plus60kfile;
    TCHAR *st_plus60kfile;
    int res_value_loop;
    int active_value;

    resources_get_value("PLUS60K", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_PLUS60K_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    resources_get_value("PLUS60Kfilename", (void *)&plus60kfile);
    st_plus60kfile = system_mbstowcs_alloc(plus60kfile);
    SetDlgItemText(hwnd, IDC_PLUS60K_FILE,
                   plus60kfile != NULL ? st_plus60kfile : TEXT(""));
    system_mbstowcs_free(st_plus60kfile);

    enable_plus60k_controls(hwnd);
}

static void end_plus60k_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    resources_set_value("PLUS60K", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_PLUS60K_ENABLE) == BST_CHECKED ?
                        1 : 0 ));

    GetDlgItemText(hwnd, IDC_PLUS60K_FILE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("PLUS60Kfilename", (resource_value_t)s);
}

static void browse_plus60k_file(HWND hwnd)
{
    uilib_select_browse(hwnd, translate_text(IDS_SELECT_FILE_PLUS60K),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE,
                        IDC_PLUS60K_FILE);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PLUS60K_BROWSE:
            browse_plus60k_file(hwnd);
            break;
          case IDC_PLUS60K_ENABLE:
            enable_plus60k_controls(hwnd);
            break;
          case IDOK:
            end_plus60k_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_plus60k_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_plus60k_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_PLUS60K_SETTINGS_DIALOG), hwnd,
              dialog_proc);
}
