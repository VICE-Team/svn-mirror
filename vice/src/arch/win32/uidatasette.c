/*
 * uidatasette.c - Implementation of the datasette settings dialog box.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#include <tchar.h>
#include <windows.h>

#include "datasette.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uidatasette.h"
#include "winmain.h"


static const int ui_datasette_zero_gap_delay[] = {
    1000,
    2000,
    5000,
    10000,
    20000,
    50000,
    100000
};


static void init_datasette_dialog(HWND hwnd)
{
    HWND snd_hwnd;
    HWND temp_hwnd;
    int res_value;
    int res_value_loop;
    int active_value;
    int min_width;
    int min_group_width;
    int xsize, ysize;
    RECT rect;
    RECT child_rect;

    SetWindowText(hwnd, translate_text(IDS_DATASETTE_CAPTION));
    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_RESET_WITH_CPU);
    SetWindowText(temp_hwnd, translate_text(IDS_DATASETTE_RESET_WITH_CPU));
    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_DELAY_TRIGGER);
    SetWindowText(temp_hwnd, translate_text(IDS_DATASETTE_DELAY_TRIGGER));
    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_DELAY_AT_ZERO);
    SetWindowText(temp_hwnd, translate_text(IDS_DATASETTE_DELAY_AT_ZERO));
    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_MOTOR_GROUP);
    SetWindowText(temp_hwnd, translate_text(IDS_DATASETTE_MOTOR_GROUP));
    temp_hwnd = GetDlgItem(hwnd, IDOK);
    SetWindowText(temp_hwnd, translate_text(IDS_OK));
    temp_hwnd = GetDlgItem(hwnd, IDCANCEL);
    SetWindowText(temp_hwnd, translate_text(IDS_CANCEL));

    GetClientRect(hwnd, &rect);

    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_RESET_WITH_CPU);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    xsize += 20;
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    min_width = child_rect.left + xsize + 10;

    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_DELAY_TRIGGER);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    xsize += 20;
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    min_group_width = child_rect.left + xsize + 10;

    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_DELAY_AT_ZERO);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    xsize += 20;
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    if (child_rect.left + xsize + 10 > min_group_width) {
        min_group_width = child_rect.left + xsize + 10;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_DATASETTE_MOTOR_GROUP);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    xsize += 20;
    if (child_rect.left + xsize + 10 > min_group_width) {
        min_group_width = child_rect.left + xsize + 10;
    }
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, min_group_width, child_rect.bottom - child_rect.top, TRUE);
    if (min_group_width + 20 > min_width) {
        min_width = min_group_width + 20;
    }
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, min_width, rect.bottom - rect.top, TRUE);

    resources_get_int("DatasetteResetWithCPU", &res_value);
    CheckDlgButton(hwnd, IDC_DATASETTE_RESET_WITH_CPU, res_value
                   ? BST_CHECKED : BST_UNCHECKED);
    
    snd_hwnd=GetDlgItem(hwnd, IDC_DATASETTE_ZERO_GAP_DELAY);
    for (res_value_loop = 0; res_value_loop < 7; res_value_loop++) {
        TCHAR st[10];
        _stprintf(st, TEXT("%d"), ui_datasette_zero_gap_delay[res_value_loop]);
        SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    resources_get_int("DatasetteZeroGapDelay", &res_value);
    active_value = 4; /* default */
    for (res_value_loop = 0; res_value_loop < 7; res_value_loop++) {
        if (ui_datasette_zero_gap_delay[res_value_loop] == res_value)
            active_value = res_value_loop;
    }
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    snd_hwnd=GetDlgItem(hwnd, IDC_DATASETTE_SPEED_TUNING);
    for (res_value_loop = 0; res_value_loop < 8; res_value_loop++) {
        TCHAR st[10];
        _stprintf(st,TEXT("%d"),res_value_loop);
        SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    resources_get_int("DatasetteSpeedTuning", &res_value);
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void end_datasette_dialog(HWND hwnd)
{
    resources_set_int("DatasetteResetWithCPU", (IsDlgButtonChecked(hwnd,
                      IDC_DATASETTE_RESET_WITH_CPU) == BST_CHECKED ? 1 : 0 ));
    resources_set_int("DatasetteSpeedTuning", (int)SendDlgItemMessage(hwnd,
                      IDC_DATASETTE_SPEED_TUNING, CB_GETCURSEL, 0, 0));
    resources_set_int("DatasetteZeroGapDelay",
                      ui_datasette_zero_gap_delay[SendDlgItemMessage(hwnd,
                      IDC_DATASETTE_ZERO_GAP_DELAY, CB_GETCURSEL, 0, 0)]);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                    LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command=LOWORD(wparam);
        switch (command) {
          case IDOK:
            end_datasette_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd,0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_datasette_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}


static void uidatasette_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_DATASETTE_SETTINGS_DIALOG),
              hwnd, dialog_proc);
}

void uidatasette_command(HWND hwnd, WPARAM wparam)
{
    switch (wparam) {
      case IDM_DATASETTE_SETTINGS:
        uidatasette_settings_dialog(hwnd);
        break;
      case IDM_DATASETTE_CONTROL_STOP:
        datasette_control(DATASETTE_CONTROL_STOP);
        break;
      case IDM_DATASETTE_CONTROL_START:
        datasette_control(DATASETTE_CONTROL_START);
        break;
      case IDM_DATASETTE_CONTROL_FORWARD:
        datasette_control(DATASETTE_CONTROL_FORWARD);
        break;
      case IDM_DATASETTE_CONTROL_REWIND:
        datasette_control(DATASETTE_CONTROL_REWIND);
        break;
      case IDM_DATASETTE_CONTROL_RECORD:
        datasette_control(DATASETTE_CONTROL_RECORD);
        break;
      case IDM_DATASETTE_CONTROL_RESET:
        datasette_control(DATASETTE_CONTROL_RESET);
        break;
      case IDM_DATASETTE_RESET_COUNTER:
        datasette_control(DATASETTE_CONTROL_RESET_COUNTER);
        break;
    }
}
