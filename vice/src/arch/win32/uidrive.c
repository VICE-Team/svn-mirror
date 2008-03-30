/*
 * uidrive.c - Implementation of the drive settings dialog box.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "drive.h"
#include "resc64.h"
#include "resources.h"
#include "serial.h"
#include "ui.h"
#include "uidrive.h"
#include "winmain.h"

static void init_dialog(HWND hwnd, int num)
{
    int drive_type, n;
    char tmp[256];

    sprintf(tmp, "Drive%dType", num);
    resources_get_value(tmp, (resource_value_t *) &drive_type);

    switch (drive_type) {
      case DRIVE_TYPE_NONE:
        n = IDC_SELECT_DRIVE_TYPE_NONE;
        break;
      case DRIVE_TYPE_1541:
        n = IDC_SELECT_DRIVE_TYPE_1541;
        break;
      case DRIVE_TYPE_1571:
        n = IDC_SELECT_DRIVE_TYPE_1571;
        break;
      case DRIVE_TYPE_1581:
        n = IDC_SELECT_DRIVE_TYPE_1581;
        break;
      case DRIVE_TYPE_2031:
        n = IDC_SELECT_DRIVE_TYPE_2031;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_TYPE_1541,
                     IDC_SELECT_DRIVE_TYPE_2031, n);
}

static BOOL CALLBACK dialog_proc(int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int drive_type;
    char tmp[256];

    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SELECT_DRIVE_TYPE_NONE:
            drive_type = DRIVE_TYPE_NONE;
            break;
          case IDC_SELECT_DRIVE_TYPE_1541:
            drive_type = DRIVE_TYPE_1541;
            break;
          case IDC_SELECT_DRIVE_TYPE_1571:
            drive_type = DRIVE_TYPE_1571;
            break;
          case IDC_SELECT_DRIVE_TYPE_1581:
            drive_type = DRIVE_TYPE_1581;
            break;
          case IDC_SELECT_DRIVE_TYPE_2031:
            drive_type = DRIVE_TYPE_2031;
            break;
          default:
            return FALSE;
        }
        sprintf(tmp, "Drive%dType", num);
        resources_set_value(tmp, (resource_value_t *) drive_type);
        return TRUE;
    }
    return FALSE;
}

#define _CALLBACK(num)                                            \
static BOOL CALLBACK callback_##num(HWND dialog, UINT msg,        \
                                    WPARAM wparam, LPARAM lparam) \
{                                                                 \
    return dialog_proc(num, dialog, msg, wparam, lparam);         \
}

_CALLBACK(8)
_CALLBACK(9)

void ui_drive_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 2; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
        psp[i].u1.pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG);
        psp[i].u2.pszIcon = NULL;
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_8;
    psp[0].pszTitle = "Drive 8";
    psp[1].pfnDlgProc = callback_9;
    psp[1].pszTitle = "Drive 9";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.u1.pszIcon = NULL;
    psh.pszCaption = "Drive Settings";
    psh.nPages = 2;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

