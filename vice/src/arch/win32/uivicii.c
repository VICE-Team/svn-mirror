/*
 * uivicii.c - Implementation of VIC-II settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include "intl.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "uilib.h"
#include "winmain.h"


static void enable_controls_for_vicii_settings(HWND hwnd, int type)
{
}

static void init_vicii_dialog(HWND hwnd)
{
    int n;

    resources_get_value("VICIICheckSsColl", (void *)&n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_SSC,
                   n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_value("VICIICheckSbColl", (void *)&n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_SBC,
                   n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_value("VICIINewLuminances", (void *)&n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_NEWLUM,
                   n ? BST_CHECKED : BST_UNCHECKED);
}

static void end_vicii_dialog(HWND hwnd)
{
    resources_set_value("VICIICheckSsColl", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_TOGGLE_VICII_SSC) == BST_CHECKED ? 1 : 0 ));

    resources_set_value("VICIICheckSbColl", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_TOGGLE_VICII_SBC) == BST_CHECKED ? 1 : 0 ));

    resources_set_value("VICIINewLuminances", (resource_value_t)
                        (IsDlgButtonChecked
                        (hwnd, IDC_TOGGLE_VICII_NEWLUM) == BST_CHECKED ? 1 : 0 ));
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_CLOSE:
        EndDialog(hwnd,0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_vicii_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_TOGGLE_VICII_SSC:
            break;
          case IDC_TOGGLE_VICII_SBC:
            break;
          case IDC_TOGGLE_VICII_NEWLUM:
            break;
          case IDOK:
            end_vicii_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

void ui_vicii_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(intl_translate(IDD_VICII_DIALOG)), hwnd,
              dialog_proc);
}

