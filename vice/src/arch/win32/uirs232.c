/*
 * uirs232.c - Implementation of the RS232 settings dialog box.
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
#include "rs232.h"
#include "system.h"
#include "translate.h"
#include "uirs232.h"
#include "winmain.h"



static void init_rs232_dialog(HWND hwnd)
{
    const char *device;
    TCHAR *st_device;

    resources_get_string("RsDevice1", &device);
    st_device = system_mbstowcs_alloc(device);
    SetDlgItemText(hwnd, IDC_RS232_DEVICE1,
                   device != NULL ? st_device : TEXT(""));
    system_mbstowcs_free(st_device);

    resources_get_string("RsDevice2", &device);
    st_device = system_mbstowcs_alloc(device);
    SetDlgItemText(hwnd, IDC_RS232_DEVICE2,
                   device != NULL ? st_device : TEXT(""));
    system_mbstowcs_free(st_device);

    resources_get_string("RsDevice3", &device);
    st_device = system_mbstowcs_alloc(device);
    SetDlgItemText(hwnd, IDC_RS232_DEVICE3,
                   device != NULL ? st_device : TEXT(""));
    system_mbstowcs_free(st_device);

    resources_get_string("RsDevice4", &device);
    st_device = system_mbstowcs_alloc(device);
    SetDlgItemText(hwnd, IDC_RS232_DEVICE4,
                   device != NULL ? st_device : TEXT(""));
    system_mbstowcs_free(st_device);
}

static void end_rs232_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_RS232_DEVICE1, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RsDevice1", s);

    GetDlgItemText(hwnd, IDC_RS232_DEVICE2, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RsDevice2", s);

    GetDlgItemText(hwnd, IDC_RS232_DEVICE3, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RsDevice3", s);

    GetDlgItemText(hwnd, IDC_RS232_DEVICE4, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RsDevice4", s);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                    LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDOK:
            end_rs232_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_rs232_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_rs232_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_RS232_SETTINGS_DIALOG), hwnd,
              dialog_proc);
}
