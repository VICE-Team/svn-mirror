/*
 * uiprinter.c - Implementation of the printer settings dialog box.
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
#include <tchar.h>
#include <windows.h>

#include "printer.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "uiprinter.h"
#include "winmain.h"


static const TCHAR *ui_printer[] =
{
    TEXT("None"),
    TEXT("File system"),
#ifdef HAVE_OPENCBM
    TEXT("Real IEC device"),
#endif
    NULL
};

static const TCHAR *ui_printer_driver[] =
{
    TEXT("ASCII"),
    TEXT("MPS803"),
    NULL
};

static const char *ui_printer_driver_ascii[] =
{
    "ascii",
    "mps803",
    NULL
};

static const TCHAR *ui_printer_text_device[] =
{
    TEXT("0"),
    TEXT("1"),
    TEXT("2"),
    NULL
};

static void enable_printer4_controls(HWND hwnd)
{
    int res_value, is_enabled;

    res_value = SendMessage(GetDlgItem(hwnd, IDC_PRINTER4_TYPE),
                            CB_GETCURSEL, 0, 0);

    is_enabled = res_value == PRINTER_DEVICE_FS;

    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER4_DRIVER), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER4_TEXTOUT), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME), is_enabled);
}

static void enable_printer5_controls(HWND hwnd)
{
    int res_value, is_enabled;

    res_value = SendMessage(GetDlgItem(hwnd, IDC_PRINTER5_TYPE),
                            CB_GETCURSEL, 0, 0);

    is_enabled = res_value == PRINTER_DEVICE_FS;

    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER5_DRIVER), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER5_TEXTOUT), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME), is_enabled);
}

static void enable_printeruserport_controls(HWND hwnd)
{
    int res_value, is_enabled;

    res_value = SendMessage(GetDlgItem(hwnd, IDC_PRINTERUSERPORT_TYPE),
                            CB_GETCURSEL, 0, 0);

    is_enabled = res_value == PRINTER_DEVICE_FS;

    EnableWindow(GetDlgItem(hwnd, IDC_PRINTERUSERPORT_DRIVER), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTERUSERPORT_TEXTOUT), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME), is_enabled);
}

static void init_output_filename(HWND hwnd)
{
    const char *res_string;

    resources_get_value("PrinterTextDevice1", (void *)&res_string);
    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME,
                   res_string != NULL ? res_string : "");

    resources_get_value("PrinterTextDevice2", (void *)&res_string);
    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME,
                   res_string != NULL ? res_string : "");

    resources_get_value("PrinterTextDevice3", (void *)&res_string);
    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME,
                   res_string != NULL ? res_string : "");
}

static void init_printer4_dialog(HWND hwnd)
{
    HWND printer_hwnd;
    int res_value, res_value_loop;
    const char *res_string;
    int current = 0;

    resources_get_value("Printer4", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER4_TYPE);
    for (res_value_loop = 0; ui_printer[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_value("Printer4Driver", (void *)&res_string);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER4_DRIVER);
    for (res_value_loop = 0; ui_printer_driver[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_driver[res_value_loop]);
        if (!strcmp(ui_printer_driver_ascii[res_value_loop], res_string))
            current = res_value_loop;
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)current, 0);

    resources_get_value("Printer4TextDevice", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER4_TEXTOUT);
    for (res_value_loop = 0; ui_printer_text_device[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_text_device[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    init_output_filename(hwnd);

    enable_printer4_controls(hwnd);
}

static void init_printer5_dialog(HWND hwnd)
{
    HWND printer_hwnd;
    int res_value, res_value_loop;
    const char *res_string;
    int current = 0;

    resources_get_value("Printer5", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER5_TYPE);
    for (res_value_loop = 0; ui_printer[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_value("Printer5Driver", (void *)&res_string);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER5_DRIVER);
    for (res_value_loop = 0; ui_printer_driver[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_driver[res_value_loop]);
        if (!strcmp(ui_printer_driver_ascii[res_value_loop], res_string))
            current = res_value_loop;
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)current, 0);

    resources_get_value("Printer5TextDevice", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER5_TEXTOUT);
    for (res_value_loop = 0; ui_printer_text_device[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_text_device[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    init_output_filename(hwnd);

    enable_printer5_controls(hwnd);
}

static void init_printeruserport_dialog(HWND hwnd)
{
    HWND printer_hwnd;
    int res_value, res_value_loop;
    const char *res_string;
    int current = 0;

    resources_get_value("PrinterUserport", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTERUSERPORT_TYPE);
    for (res_value_loop = 0; ui_printer[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_value("PrinterUserportDriver", (void *)&res_string);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTERUSERPORT_DRIVER);
    for (res_value_loop = 0; ui_printer_driver[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_driver[res_value_loop]);
        if (!strcmp(ui_printer_driver_ascii[res_value_loop], res_string))
            current = res_value_loop;
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)current, 0);

    resources_get_value("PrinterUserportTextDevice", (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTERUSERPORT_TEXTOUT);
    for (res_value_loop = 0; ui_printer_text_device[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_text_device[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    init_output_filename(hwnd);

    enable_printeruserport_controls(hwnd);
}

static BOOL CALLBACK printer4_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                          LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PRINTER4_TYPE:
            enable_printer4_controls(hwnd);
            break;
        }
        return FALSE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_SETACTIVE:
            enable_printer4_controls(hwnd);
            break;
          case PSN_KILLACTIVE:
            resources_set_value("Printer4", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER4_TYPE),
                                CB_GETCURSEL, 0, 0));
            resources_set_value("Printer4Driver", (resource_value_t)
                                ui_printer_driver_ascii[
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER4_DRIVER),
                                CB_GETCURSEL, 0, 0)]);
            resources_set_value("Printer4TextDevice", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER4_TEXTOUT),
                                CB_GETCURSEL, 0, 0));
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_printer4_dialog(hwnd);
        return TRUE;
    }

    return FALSE;
}

static BOOL CALLBACK printer5_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                          LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PRINTER5_TYPE:
            enable_printer5_controls(hwnd);
            break;
        }
        return FALSE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_SETACTIVE:
            enable_printer5_controls(hwnd);
            break;
          case PSN_KILLACTIVE:
            resources_set_value("Printer5", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER5_TYPE),
                                CB_GETCURSEL, 0, 0));
            resources_set_value("Printer5Driver", (resource_value_t)
                                ui_printer_driver_ascii[
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER5_DRIVER),
                                CB_GETCURSEL, 0, 0)]);
            resources_set_value("Printer5TextDevice", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTER5_TEXTOUT),
                                CB_GETCURSEL, 0, 0));
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_printer5_dialog(hwnd);
        return TRUE;
    }

    return FALSE;
}

static BOOL CALLBACK printeruserport_dialog_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PRINTERUSERPORT_TYPE:
            enable_printeruserport_controls(hwnd);
            break;
        }
        return FALSE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_SETACTIVE:
            enable_printeruserport_controls(hwnd);
            break;
          case PSN_KILLACTIVE:
            resources_set_value("PrinterUserport", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTERUSERPORT_TYPE),
                                CB_GETCURSEL, 0, 0));
            resources_set_value("PrinterUserportDriver", (resource_value_t)
                                ui_printer_driver_ascii[
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTERUSERPORT_DRIVER),
                                CB_GETCURSEL, 0, 0)]);
            resources_set_value("PrinterUserportTextDevice", (resource_value_t)
                                SendMessage(GetDlgItem(hwnd,
                                IDC_PRINTERUSERPORT_TEXTOUT),
                                CB_GETCURSEL, 0, 0));
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_printeruserport_dialog(hwnd);
        return TRUE;
    }

    return FALSE;
}

void ui_printer_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PRINTER4_SETTINGS_DIALOG);
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_PRINTER4_SETTINGS_DIALOG);
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[1].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PRINTER5_SETTINGS_DIALOG);
    psp[1].pszIcon = NULL;
#else
    psp[1].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_PRINTER5_SETTINGS_DIALOG);
    psp[1].u2.pszIcon = NULL;
#endif
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[2].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_PRINTERUSERPORT_SETTINGS_DIALOG);
    psp[2].pszIcon = NULL;
#else
    psp[2].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_USERPORT_SETTINGS_DIALOG);
    psp[2].u2.pszIcon = NULL;
#endif
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psp[0].pfnDlgProc = printer4_dialog_proc;
    psp[0].pszTitle = "Printer #4";
    psp[1].pfnDlgProc = printer5_dialog_proc;
    psp[1].pszTitle = "Printer #5";
    psp[2].pfnDlgProc = printeruserport_dialog_proc;
    psp[2].pszTitle = "Userport Printer";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "Printer settings";
    psh.nPages = 3;
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

