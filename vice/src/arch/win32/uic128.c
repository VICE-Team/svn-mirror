/*
 * uic128.c - Implementation of the C128 settings dialog box.
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
#include <string.h>
#include <tchar.h>
#include <windows.h>

#include "machine.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uic128.h"
#include "winmain.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif


static const TCHAR *ui_machine[] = 
{
    TEXT("International"),
    TEXT("Finnish"),
    TEXT("French"),
    TEXT("German"),
    TEXT("Italian"),
    TEXT("Norwegian"),
    TEXT("Swedish"),
    NULL
};


static void enable_machine_controls(HWND hwnd)
{
}

static void enable_functionrom_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_C128_FUNCTIONROM_INTERNAL)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_C128_FUNCTIONROM_INTERNAL_NAME),
                 is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_C128_FUNCTIONROM_INTERNAL_BROWSE),
                 is_enabled);

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL)
                 == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL_NAME),
                 is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL_BROWSE),
                 is_enabled);
}

static void init_machine_dialog(HWND hwnd)
{
    HWND machine_hwnd;
    int res_value;
    int res_value_loop;

    resources_get_value("MachineType", (void *)&res_value);
    machine_hwnd = GetDlgItem(hwnd, IDC_C128_MACHINE_TYPE);
    for (res_value_loop = 0; ui_machine[res_value_loop];
        res_value_loop++) {
        SendMessage(machine_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_machine[res_value_loop]);
    }
    SendMessage(machine_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    enable_machine_controls(hwnd);
}

static void init_functionrom_dialog(HWND hwnd)
{
    int res_value;
    const char *romfile;

    resources_get_value("InternalFunctionROM", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_C128_FUNCTIONROM_INTERNAL, res_value
                   ? BST_CHECKED : BST_UNCHECKED);

    resources_get_value("InternalFunctionName", (void *)&romfile);
    SetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_INTERNAL_NAME,
                   romfile != NULL ? romfile : "");

    resources_get_value("ExternalFunctionROM", (void *)&res_value);
    CheckDlgButton(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL, res_value
                   ? BST_CHECKED : BST_UNCHECKED);
 
    resources_get_value("ExternalFunctionName", (void *)&romfile);
    SetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL_NAME,
                   romfile != NULL ? romfile : "");

    enable_functionrom_controls(hwnd);
}

static BOOL CALLBACK machine_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                         LPARAM lparam)
{
    switch (msg) {
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            resources_set_value("MachineType", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_C128_MACHINE_TYPE),
                                CB_GETCURSEL, 0, 0));
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_machine_dialog(hwnd);
        return TRUE;
    }

    return FALSE;
}

static BOOL CALLBACK functionrom_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                             LPARAM lparam)
{
    int command;
    char name[MAX_PATH];

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_C128_FUNCTIONROM_INTERNAL:
          case IDC_C128_FUNCTIONROM_EXTERNAL:
            enable_functionrom_controls(hwnd);
            break;
          case IDC_C128_FUNCTIONROM_INTERNAL_BROWSE:
          case IDC_C128_FUNCTIONROM_EXTERNAL_BROWSE:
            memset(name, 0, sizeof(name));
            OPENFILENAME ofn;

            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.hInstance = winmain_instance;
            ofn.lpstrFilter = "All files (*.*)\0*.*\0";
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter = 0;
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = name;
            ofn.nMaxFile = sizeof(name);
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.lpstrTitle = "Select ROM image";
            ofn.Flags = (OFN_EXPLORER
                | OFN_HIDEREADONLY
                | OFN_NOTESTFILECREATE
                | OFN_FILEMUSTEXIST
                | OFN_SHAREAWARE
                | OFN_ENABLESIZING);
            ofn.nFileOffset = 0;
            ofn.nFileExtension = 0;
            ofn.lpstrDefExt = NULL;

            if (command == IDC_C128_FUNCTIONROM_INTERNAL_BROWSE) {
                if (GetSaveFileName(&ofn))
                    SetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_INTERNAL_NAME,
                    name);
            } else {
                if (GetSaveFileName(&ofn))
                    SetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL_NAME,
                    name);
            }
            break;
        }
        return FALSE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            resources_set_value("InternalFunctionROM", (resource_value_t)
                                (IsDlgButtonChecked(hwnd,
                                IDC_C128_FUNCTIONROM_INTERNAL)
                                == BST_CHECKED ? 1 : 0 ));
            GetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_INTERNAL_NAME, name,
                           MAX_PATH);
            resources_set_value("InternalFunctionName",
                                (resource_value_t)name);

            resources_set_value("ExternalFunctionROM", (resource_value_t)
                                (IsDlgButtonChecked(hwnd,
                                IDC_C128_FUNCTIONROM_EXTERNAL)
                                == BST_CHECKED ? 1 : 0 ));
            GetDlgItemText(hwnd, IDC_C128_FUNCTIONROM_EXTERNAL_NAME, name,
                           MAX_PATH);
            resources_set_value("ExternalFunctionName",
                                (resource_value_t)name);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_functionrom_dialog(hwnd);
        return TRUE;
    }

    return FALSE;
}

void ui_c128_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_C128_MACHINE_SETTINGS_DIALOG);
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_C128_MACHINE_SETTINGS_DIALOG);
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[1].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_C128_FUNCTIONROM_SETTINGS_DIALOG);
    psp[1].pszIcon = NULL;
#else
    psp[1].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_C128_FUNCTIONROM_SETTINGS_DIALOG);
    psp[1].u2.pszIcon = NULL;
#endif
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[0].pfnDlgProc = machine_dialog_proc;
    psp[0].pszTitle = "Machine type";
    psp[1].pfnDlgProc = functionrom_dialog_proc;
    psp[1].pszTitle = "Function ROM";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "C128 settings";
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

    PropertySheet(&psh);
}

