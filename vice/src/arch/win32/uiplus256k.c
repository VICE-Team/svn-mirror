/*
 * uiplus256k.c - Implementation of the +256K EXPANSION settings dialog box.
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
#include "uiplus256k.h"
#include "winmain.h"
#include "intl.h"

static void enable_plus256k_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_PLUS256K_ENABLE)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_PLUS256K_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PLUS256K_FILE), is_enabled);
}

static uilib_localize_dialog_param plus256k_dialog[] = {
    {0, IDS_PLUS256K_CAPTION, -1},
    {IDC_PLUS256K_ENABLE, IDS_PLUS256K_ENABLE, 0},
    {IDC_PLUS256K_FILE_LABEL, IDS_PLUS256K_FILE, 0},
    {IDC_PLUS256K_BROWSE, IDS_BROWSE, 0},
    {IDOK, IDS_OK, 0},
    {IDCANCEL, IDS_CANCEL, 0},
    {0, 0, 0}
};

static uilib_dialog_group plus256k_group1[] = {
    {IDC_PLUS256K_ENABLE, 1},
    {0, 0}
};

static uilib_dialog_group plus256k_leftgroup[] = {
    {IDC_PLUS256K_FILE_LABEL, 0},
    {0, 0}
};

static uilib_dialog_group plus256k_rightgroup[] = {
    {IDC_PLUS256K_BROWSE, 0},
    {0, 0}
};

static void init_plus256k_dialog(HWND hwnd)
{
    int res_value;
    const char *plus256kfile;
    TCHAR *st_plus256kfile;
    int xsize, ysize;

    uilib_localize_dialog(hwnd, plus256k_dialog);
    uilib_adjust_group_width(hwnd, plus256k_group1);
    uilib_get_group_extent(hwnd, plus256k_leftgroup, &xsize, &ysize);
    uilib_adjust_group_width(hwnd, plus256k_leftgroup);
    uilib_move_group(hwnd, plus256k_rightgroup, xsize + 30);

    resources_get_value("PLUS256K", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_PLUS256K_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    resources_get_value("PLUS256Kfilename", (void *)&plus256kfile);
    st_plus256kfile = system_mbstowcs_alloc(plus256kfile);
    SetDlgItemText(hwnd, IDC_PLUS256K_FILE,
                   plus256kfile != NULL ? st_plus256kfile : TEXT(""));
    system_mbstowcs_free(st_plus256kfile);

    enable_plus256k_controls(hwnd);
}

static void end_plus256k_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    resources_set_value("PLUS256K", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_PLUS256K_ENABLE) == BST_CHECKED ?
                        1 : 0 ));

    GetDlgItemText(hwnd, IDC_PLUS256K_FILE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("PLUS256Kfilename", (resource_value_t)s);
}

static void browse_plus256k_file(HWND hwnd)
{
    uilib_select_browse(hwnd, intl_translate_text_new(IDS_PLUS256K_SELECT_FILE),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE,
                        IDC_PLUS256K_FILE);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PLUS256K_BROWSE:
            browse_plus256k_file(hwnd);
            break;
          case IDC_PLUS256K_ENABLE:
            enable_plus256k_controls(hwnd);
            break;
          case IDOK:
            end_plus256k_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_plus256k_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_plus256k_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_PLUS256K_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}
