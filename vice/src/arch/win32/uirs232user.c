/*
 * uirs232user.c - Implementation of the RS232 userport settings dialog box.
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
#include <windows.h>
#include <tchar.h>

#include "res.h"
#include "resources.h"
#include "uirs232user.h"
#include "winmain.h"


#define MAXRS232 4


#define NUM_OF_BAUDRATES 6
static const int baudrates[NUM_OF_BAUDRATES] = {
    300, 600, 1200, 2400, 4800, 9600
};


static void enable_rs232user_controls(HWND hwnd)
{
    int rs232user_enable;

    rs232user_enable = 0;

    if (IsDlgButtonChecked(hwnd, IDC_RS232USER_ENABLE) == BST_CHECKED)
        rs232user_enable = 1;

    EnableWindow(GetDlgItem(hwnd, IDC_RS232USER_DEVICE), rs232user_enable);
    EnableWindow(GetDlgItem(hwnd, IDC_RS232USER_BAUDRATE), rs232user_enable);
} 

static void init_rs232user_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    int res_value_loop;
    int active_value;

    resources_get_value("RsUserEnable", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_RS232USER_ENABLE,
                   res_value ? BST_CHECKED : BST_UNCHECKED);

    temp_hwnd = GetDlgItem(hwnd, IDC_RS232USER_DEVICE);
    for (res_value_loop = 0; res_value_loop < MAXRS232; res_value_loop++) {
        TCHAR st[20];
        _stprintf(st, TEXT("RS232 device %i"), res_value_loop + 1);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_value("RsUserDev", (void *)&res_value);
    active_value = 0;
    for (res_value_loop = 0; res_value_loop < MAXRS232; res_value_loop++) {
        if (res_value_loop == res_value)
            active_value = res_value_loop;
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    temp_hwnd = GetDlgItem(hwnd, IDC_RS232USER_BAUDRATE);
    for (res_value_loop = 0; res_value_loop < NUM_OF_BAUDRATES;
        res_value_loop++) {
        TCHAR st[10];
        _itot(baudrates[res_value_loop], st, 10);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }

    resources_get_value("RsUserBaud", (void *)&res_value);
    active_value = 0;
    for (res_value_loop = 0; res_value_loop < MAXRS232; res_value_loop++) {
        if (baudrates[res_value_loop] == res_value)
            active_value = res_value_loop;
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    enable_rs232user_controls(hwnd);
}

static void end_rs232user_dialog(HWND hwnd)
{
    resources_set_value("RsUserEnable", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_RS232USER_ENABLE) == BST_CHECKED ? 1 : 0 ));

    resources_set_value("RsUserDev",(resource_value_t)
                        SendMessage(GetDlgItem(hwnd, IDC_RS232USER_DEVICE),
                        CB_GETCURSEL, 0, 0));

    resources_set_value("RsUserBaud",(resource_value_t)
                        baudrates[SendMessage(GetDlgItem(
                        hwnd, IDC_RS232USER_BAUDRATE), CB_GETCURSEL, 0, 0)]);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_RS232USER_ENABLE:
            enable_rs232user_controls(hwnd);
            break;
          case IDOK:
            end_rs232user_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_rs232user_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_rs232user_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_RS232USER_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}

