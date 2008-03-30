/*
 * uidrivepetcbm2.c - Implementation of the drive settings dialog box.
 *
 * Written by
 *  Tibor Biczo <viceteam@t-online.de>
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
#include <prsht.h>

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "drive.h"
#include "res.h"
#include "resources.h"
#include "serial.h"
#include "system.h"
#include "ui.h"
#include "uidrivepetcbm2.h"
#include "uilib.h"
#include "winmain.h"

static void enable_controls_for_drive_settings(HWND hwnd, int type)
{
    int drive_type = 0;

    switch (type) {
      case IDC_SELECT_DRIVE_TYPE_2031:
        drive_type = DRIVE_TYPE_2031;
        break;
      case IDC_SELECT_DRIVE_TYPE_2040:
        drive_type = DRIVE_TYPE_2040;
        break;
      case IDC_SELECT_DRIVE_TYPE_3040:
        drive_type = DRIVE_TYPE_3040;
        break;
      case IDC_SELECT_DRIVE_TYPE_4040:
        drive_type = DRIVE_TYPE_4040;
        break;
      case IDC_SELECT_DRIVE_TYPE_1001:
        drive_type = DRIVE_TYPE_1001;
        break;
      case IDC_SELECT_DRIVE_TYPE_8050:
        drive_type = DRIVE_TYPE_8050;
        break;
      case IDC_SELECT_DRIVE_TYPE_8250:
        drive_type = DRIVE_TYPE_8250;
        break;
    }

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER),
                 drive_check_extend_policy(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ASK),
                 drive_check_extend_policy(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ACCESS),
                 drive_check_extend_policy(drive_type));
}

static void init_dialog(HWND hwnd, int num)
{
    int drive_type, drive_extend_image_policy, n;

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_2031),
                 drive_check_type(DRIVE_TYPE_2031, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_2040),
                 drive_check_type(DRIVE_TYPE_2040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_3040),
                 drive_check_type(DRIVE_TYPE_3040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_4040),
                 drive_check_type(DRIVE_TYPE_4040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1001),
                 drive_check_type(DRIVE_TYPE_1001, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8050),
                 drive_check_type(DRIVE_TYPE_8050, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8250),
                 drive_check_type(DRIVE_TYPE_8250, num - 8));

    resources_get_sprintf("Drive%dType",
                          (resource_value_t *) &drive_type, num);
    resources_get_sprintf("Drive%dExtendImagePolicy",
                          (resource_value_t *) &drive_extend_image_policy, num);

    switch (drive_type) {
      case DRIVE_TYPE_NONE:
        n = IDC_SELECT_DRIVE_TYPE_NONE;
        break;
      case DRIVE_TYPE_2031:
        n = IDC_SELECT_DRIVE_TYPE_2031;
        break;
      case DRIVE_TYPE_2040:
        n = IDC_SELECT_DRIVE_TYPE_2040;
        break;
      case DRIVE_TYPE_3040:
        n = IDC_SELECT_DRIVE_TYPE_3040;
        break;
      case DRIVE_TYPE_4040:
        n = IDC_SELECT_DRIVE_TYPE_4040;
        break;
      case DRIVE_TYPE_1001:
        n = IDC_SELECT_DRIVE_TYPE_1001;
        break;
      case DRIVE_TYPE_8050:
        n = IDC_SELECT_DRIVE_TYPE_8050;
        break;
      case DRIVE_TYPE_8250:
        n = IDC_SELECT_DRIVE_TYPE_8250;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_TYPE_2031,
                     IDC_SELECT_DRIVE_TYPE_NONE, n);

    enable_controls_for_drive_settings(hwnd, n);

    switch (drive_extend_image_policy) {
      case DRIVE_EXTEND_NEVER:
        n = IDC_SELECT_DRIVE_EXTEND_NEVER;
        break;
      case DRIVE_EXTEND_ASK:
        n = IDC_SELECT_DRIVE_EXTEND_ASK;
        break;
      case DRIVE_EXTEND_ACCESS:
        n = IDC_SELECT_DRIVE_EXTEND_ACCESS;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER,
                     IDC_SELECT_DRIVE_EXTEND_ACCESS, n);
}

static BOOL CALLBACK dialog_proc(int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SELECT_DRIVE_TYPE_NONE:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_NONE, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_2031:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_2031, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_2040:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_2040, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_3040:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_3040, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_4040:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_4040, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1001:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_1001, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_8050:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_8050, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_8250:
            resources_set_sprintf("Drive%dType",
                                  (resource_value_t *)DRIVE_TYPE_8250, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_EXTEND_NEVER:
            resources_set_sprintf("Drive%dExtendImagePolicy",
                                  (resource_value_t *)DRIVE_EXTEND_NEVER, num);
            break;
          case IDC_SELECT_DRIVE_EXTEND_ASK:
            resources_set_sprintf("Drive%dExtendImagePolicy",
                                  (resource_value_t *)DRIVE_EXTEND_ASK, num);
            break;
          case IDC_SELECT_DRIVE_EXTEND_ACCESS:
            resources_set_sprintf("Drive%dExtendImagePolicy",
                                  (resource_value_t *)DRIVE_EXTEND_ACCESS, num);
            break;
          default:
            return FALSE;
        }
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

void uidrivepetcbm2_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 2; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG_PETCBM2);
        psp[i].pszIcon = NULL;
#else
        psp[i].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG_PETCBM2);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_8;
    psp[0].pszTitle = TEXT("Drive 8");
    psp[1].pfnDlgProc = callback_9;
    psp[1].pszTitle = TEXT("Drive 9");

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = TEXT("Drive Settings");
    psh.nPages = 2;
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    system_psh_settings(&psh);
    PropertySheet(&psh);
}

