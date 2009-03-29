/*
 * uiisepic.c - Implementation of the Isepic settings dialog box.
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

#include "intl.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "types.h"
#include "uilib.h"
#include "uiisepic.h"
#include "winmain.h"


static void enable_isepic_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_ISEPIC_ENABLE)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_ISEPIC_SWITCH), is_enabled);
}

static void init_isepic_dialog(HWND hwnd)
{
    int res_value;

    resources_get_int("Isepic", &res_value);
    CheckDlgButton(hwnd, IDC_ISEPIC_ENABLE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    resources_get_int("IsepicSwitch", &res_value);
    CheckDlgButton(hwnd, IDC_ISEPIC_SWITCH, 
        res_value ? BST_CHECKED : BST_UNCHECKED);

    enable_isepic_controls(hwnd);
}

static void end_isepic_dialog(HWND hwnd)
{
    resources_set_int("Isepic", (IsDlgButtonChecked(hwnd,
                      IDC_ISEPIC_ENABLE) == BST_CHECKED ? 1 : 0 ));
    resources_set_int("IsepicSwitch", (IsDlgButtonChecked(hwnd,
                      IDC_ISEPIC_SWITCH) == BST_CHECKED ? 1 : 0 ));
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                    LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_ISEPIC_ENABLE:
            enable_isepic_controls(hwnd);
            break;
          case IDOK:
            end_isepic_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_isepic_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_isepic_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_ISEPIC_SETTINGS_DIALOG), hwnd,
              dialog_proc);
}
