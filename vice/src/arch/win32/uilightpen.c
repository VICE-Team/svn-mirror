/*
 * uilightpen.c - Implementation of the lightpen settings dialog box.
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
#include "ui.h"
#include "uilib.h"
#include "uilightpen.h"
#include "winmain.h"
#include "intl.h"

static void enable_lightpen_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_LIGHTPEN_ENABLE) == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_LIGHTPEN_TYPE), is_enabled);
}

static uilib_localize_dialog_param lightpen_dialog[] = {
    { 0, IDS_LIGHTPEN_CAPTION, -1 },
    { IDC_LIGHTPEN_ENABLE, IDS_LIGHTPEN_ENABLE, 0 },
    { IDC_LIGHTPEN_TYPE_LABEL, IDS_LIGHTPEN_TYPE, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group lightpen_leftgroup[] = {
    { IDC_LIGHTPEN_TYPE_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group lightpen_rightgroup[] = {
    { IDC_LIGHTPEN_TYPE, 0 },
    { 0, 0 }
};

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static void init_lightpen_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    int xsize, ysize;
    RECT rect;

    uilib_localize_dialog(hwnd, lightpen_dialog);
    uilib_get_group_extent(hwnd, lightpen_leftgroup, &xsize, &ysize);
    uilib_adjust_group_width(hwnd, lightpen_leftgroup);
    uilib_move_group(hwnd, lightpen_rightgroup, xsize + 30);

    /* get the max x of the right group */
    uilib_get_group_max_x(hwnd, lightpen_rightgroup, &xsize);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xsize + 20, rect.bottom - rect.top, TRUE);

    /* recenter the buttons in the newly resized dialog window */
    uilib_center_buttons(hwnd, move_buttons_group, 0);

    resources_get_int("Lightpen", &res_value);
    CheckDlgButton(hwnd, IDC_LIGHTPEN_ENABLE, res_value ? BST_CHECKED : BST_UNCHECKED);
    
    temp_hwnd = GetDlgItem(hwnd, IDC_LIGHTPEN_TYPE);

    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Pen with button Up");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Pen with button Left");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Datel Pen");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Magnum Light Phaser");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Stack Light Rifle");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Inkwell Pen");
    resources_get_int("LightpenType", &res_value);
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    enable_lightpen_controls(hwnd);
}

static void end_lightpen_dialog(HWND hwnd)
{
    resources_set_int("Lightpen", (IsDlgButtonChecked(hwnd, IDC_LIGHTPEN_ENABLE) == BST_CHECKED ? 1 : 0 ));

    resources_set_int("LightpenType", (int)SendMessage(GetDlgItem(hwnd, IDC_LIGHTPEN_TYPE), CB_GETCURSEL, 0, 0));
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
        case WM_COMMAND:
            command = LOWORD(wparam);
            switch (command) {
                case IDC_LIGHTPEN_ENABLE:
                    enable_lightpen_controls(hwnd);
                    break;
                case IDOK:
                    end_lightpen_dialog(hwnd);
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_lightpen_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}


void ui_lightpen_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_LIGHTPEN_SETTINGS_DIALOG, hwnd, dialog_proc);
}
