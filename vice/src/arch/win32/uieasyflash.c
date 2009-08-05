/*
 * uieasyflash.c - Implementation of the EasyFlash settings dialog box.
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

#include "cart/easyflash.h"
#include "intl.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"
#include "uieasyflash.h"
#include "uilib.h"
#include "winmain.h"

static void init_easyflash_dialog(HWND hwnd)
{
    int res_value;

    resources_get_int("EasyFlashJumper", &res_value);
    CheckDlgButton(hwnd, IDC_EASYFLASH_JUMPER, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
    
    resources_get_int("EasyFlashWriteCRT", &res_value);
    CheckDlgButton(hwnd, IDC_EASYFLASH_AUTOSAVE, 
        res_value ? BST_CHECKED : BST_UNCHECKED);
}

static void end_easyflash_dialog(HWND hwnd)
{
    resources_set_int("EasyFlashJumper", (IsDlgButtonChecked(hwnd,
                      IDC_EASYFLASH_JUMPER) == BST_CHECKED ? 1 : 0 
));
    resources_set_int("EasyFlashWriteCRT", (IsDlgButtonChecked(hwnd,
                      IDC_EASYFLASH_AUTOSAVE) == BST_CHECKED ? 1 : 0 ));
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                    LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_EASYFLASH_SAVE_NOW:
            if (easyflash_save_crt() < 0) {
              ui_error(translate_text(IDS_ERROR_SAVING_EASYFLASH_CRT));
            }
            break;
          case IDOK:
            end_easyflash_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_easyflash_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_easyflash_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_EASYFLASH_SETTINGS_DIALOG), hwnd,
              dialog_proc);
}
