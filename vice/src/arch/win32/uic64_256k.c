/*
 * uic64_256k.c - Implementation of the 256K EXPANSION HACK settings dialog box.
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
#include "uic64_256k.h"
#include "uilib.h"
#include "winmain.h"
#include "intl.h"

#define NUM_OF_C64_256K_BASE 4
static const int ui_c64_256k_base[NUM_OF_C64_256K_BASE] = {
    0xde00, 0xde80, 0xdf00, 0xdf80
};

static void enable_c64_256k_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_C64_256K_ENABLE)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_C64_256K_BASE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_C64_256K_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_C64_256K_FILE), is_enabled);
}

static uilib_localize_dialog_param c64_256k_dialog[] = {
    {0, IDS_C64_256K_CAPTION, -1},
    {IDC_C64_256K_ENABLE, IDS_C64_256K_ENABLE, 0},
    {IDC_C64_256K_BASE_LABEL, IDS_C64_256K_BASE, 0},
    {IDC_C64_256K_FILE_LABEL, IDS_C64_256K_FILE, 0},
    {IDC_C64_256K_BROWSE, IDS_BROWSE, 0},
    {IDOK, IDS_OK, 0},
    {IDCANCEL, IDS_CANCEL, 0},
    {0, 0, 0}
};

static uilib_dialog_group c64_256k_leftgroup[] = {
    {IDC_C64_256K_BASE_LABEL, 0},
    {IDC_C64_256K_FILE_LABEL, 0},
    {0, 0}
};

static uilib_dialog_group c64_256k_rightgroup[] = {
    {IDC_C64_256K_BASE, 0},
    {IDC_C64_256K_BROWSE, 0},
    {0, 0}
};

static void init_c64_256k_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    const char *c64_256kfile;
    TCHAR *st_c64_256kfile;
    int res_value_loop;
    int active_value;
    int xsize, ysize;

    uilib_localize_dialog(hwnd, c64_256k_dialog);
    uilib_get_group_extent(hwnd, c64_256k_leftgroup, &xsize, &ysize);
    uilib_adjust_group_width(hwnd, c64_256k_leftgroup);
    uilib_move_group(hwnd, c64_256k_rightgroup, xsize + 30);

    resources_get_value("C64_256K", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_C64_256K_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    temp_hwnd = GetDlgItem(hwnd, IDC_C64_256K_BASE);
    for (res_value_loop = 0; res_value_loop < NUM_OF_C64_256K_BASE;
        res_value_loop++) {
        TCHAR st[10];
        _stprintf(st, "$%X", ui_c64_256k_base[res_value_loop]);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    resources_get_value("C64_256Kbase", (void *)&res_value);
    active_value = 0;
    for (res_value_loop = 0; res_value_loop < NUM_OF_C64_256K_BASE;
        res_value_loop++) {
        if (ui_c64_256k_base[res_value_loop] == res_value)
            active_value = res_value_loop;
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    resources_get_value("C64_256Kfilename", (void *)&c64_256kfile);
    st_c64_256kfile = system_mbstowcs_alloc(c64_256kfile);
    SetDlgItemText(hwnd, IDC_C64_256K_FILE,
                   c64_256kfile != NULL ? st_c64_256kfile : TEXT(""));
    system_mbstowcs_free(st_c64_256kfile);

    enable_c64_256k_controls(hwnd);
}

static void end_c64_256k_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    resources_set_value("C64_256K", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_C64_256K_ENABLE) == BST_CHECKED ?
                        1 : 0 ));

    resources_set_value("C64_256Kbase",(resource_value_t)
                        ui_c64_256k_base[SendMessage(GetDlgItem(
                        hwnd, IDC_C64_256K_BASE), CB_GETCURSEL, 0, 0)]);

    GetDlgItemText(hwnd, IDC_C64_256K_FILE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("C64_256Kfilename", (resource_value_t)s);
}

static void browse_c64_256k_file(HWND hwnd)
{
    uilib_select_browse(hwnd, intl_translate_text_new(IDS_C64_256K_SELECT_FILE),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE,
                        IDC_C64_256K_FILE);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_C64_256K_BROWSE:
            browse_c64_256k_file(hwnd);
            break;
          case IDC_C64_256K_ENABLE:
            enable_c64_256k_controls(hwnd);
            break;
          case IDOK:
            end_c64_256k_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_c64_256k_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}


void ui_c64_256k_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_C64_256K_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}
