/*
 * uiplus4mem.c - Implementation of PLUS4 memory settings dialog box.
 *
 * Written by
 *  Tibor Biczo <crown@axelero.hu>
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

#include <windows.h>

#include "res.h"
#include "resources.h"
#include "system.h"
#include "uilib.h"
#include "winmain.h"


static int orig_ramsize;
static int set_ramsize;

static void init_dialog(HWND hwnd)
{
    int n, res;

    resources_get_value("RamSize", (void *)&res);
    switch (res) {
      case 16:
        n = IDC_SELECT_PLUS4_MEM_16;
        break;
      case 32:
        n = IDC_SELECT_PLUS4_MEM_32;
        break;
      case 64:
      default:
        n = IDC_SELECT_PLUS4_MEM_64;
        break;
    }
    orig_ramsize = set_ramsize = res;
    CheckRadioButton(hwnd, IDC_SELECT_PLUS4_MEM_16, IDC_SELECT_PLUS4_MEM_64,
                     n);
}

static void end_dialog(void)
{
    if (orig_ramsize != set_ramsize) {
        resources_set_value("RamSize", (resource_value_t)set_ramsize);
    }
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_SELECT_PLUS4_MEM_16:
            set_ramsize=16;
            break;
          case IDC_SELECT_PLUS4_MEM_32:
            set_ramsize=32;
            break;
          case IDC_SELECT_PLUS4_MEM_64:
            set_ramsize=64;
            break;
          case IDOK:
            end_dialog();
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ui_plus4_memory_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_PLUS4_MEMORY_DIALOG),
              hwnd, dialog_proc);
}

