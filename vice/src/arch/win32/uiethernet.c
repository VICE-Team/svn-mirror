/*
 * uiethernet.c - Implementation of the ethernet settings dialog box.
 *
 * Written by
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
 *  Marco van den heuvel <blackystardust68@yahoo.com>
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

#ifdef HAVE_PCAP

#include <stdio.h>
#include <windows.h>

#include "intl.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "rawnet.h"
#include "system.h"
#include "translate.h"
#include "uiethernet.h"
#include "winmain.h"
#include "uilib.h"
#include "util.h"

static BOOL get_ethernetname(int number, char **ppname, char **ppdescription)
{
    if (rawnet_enumadapter_open()) {
        char *pname = NULL;
        char *pdescription = NULL;

        while (number--) {
            if (!rawnet_enumadapter(&pname, &pdescription)) {
                break;
            }

            lib_free(pname);
            lib_free(pdescription);
        }

        if (rawnet_enumadapter(&pname, &pdescription)) {
            *ppname = pname;
            *ppdescription = pdescription;
            rawnet_enumadapter_close();
            return TRUE;
        }

        rawnet_enumadapter_close();
    }
    return FALSE;
}

static int gray_ungray_items(HWND hwnd)
{
    int enable;
    int number;

    int disabled = 0;

    resources_get_int("ETHERNET_DISABLED", &disabled);

    if (disabled) {
        EnableWindow(GetDlgItem(hwnd, IDOK), 0);
        SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_NAME), TEXT(""));
        SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_DESC), TEXT(""));
        enable = 0;
    }

    EnableWindow(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_T), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE), enable);

    if (enable) {
        char *pname = NULL;
        char *pdescription = NULL;
        TCHAR *st_name;
        TCHAR *st_description;

        number = (int)SendMessage(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE), CB_GETCURSEL, 0, 0);

        if (get_ethernetname(number, &pname, &pdescription)) {
            st_name = system_mbstowcs_alloc(pname);
            st_description = system_mbstowcs_alloc(pdescription);
            SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_NAME), st_name);
            SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_DESC), st_description);
            system_mbstowcs_free(st_name);
            system_mbstowcs_free(st_description);
            lib_free(pname);
            lib_free(pdescription);
        }
    } else {
        SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_NAME), TEXT(""));
        SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_DESC), TEXT(""));
    }

    return disabled ? 1 : 0;
}

static uilib_localize_dialog_param ethernet_dialog[] = {
    { 0, IDS_ETHERNET_CAPTION, -1 },
    { IDC_ETHERNET_SETTINGS_INTERFACE_T, IDS_ETHERNET_INTERFACE, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group ethernet_leftgroup[] = {
    { IDC_ETHERNET_SETTINGS_INTERFACE_T, 0 },
    { 0, 0 }
};

static uilib_dialog_group ethernet_rightgroup[] = {
    { IDC_ETHERNET_SETTINGS_INTERFACE, 0 },
    { 0, 0 }
};

static void init_ethernet_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int xsize, ysize;

    const char *interface_name;

    uilib_localize_dialog(hwnd, ethernet_dialog);
    uilib_get_group_extent(hwnd, ethernet_leftgroup, &xsize, &ysize);
    uilib_adjust_group_width(hwnd, ethernet_leftgroup);
    uilib_move_group(hwnd, ethernet_rightgroup, xsize + 30);

    resources_get_string("ETHERNET_INTERFACE", &interface_name);

    if (rawnet_enumadapter_open()) {
        int cnt = 0;

        char *pname;
        char *pdescription;
        char *combined;
        TCHAR *st_name;
        TCHAR *st_description;
        TCHAR *st_combined;

        temp_hwnd = GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE);

        for (cnt = 0; rawnet_enumadapter(&pname, &pdescription); cnt++) {
            BOOL this_entry = FALSE;

            if (strcmp(pname, interface_name) == 0) {
                this_entry = TRUE;
            }
            st_name = system_mbstowcs_alloc(pname);
            st_description = system_mbstowcs_alloc(pdescription);
            SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_NAME), st_name);
            SetWindowText(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE_DESC), st_description);
            system_mbstowcs_free(st_name);
            system_mbstowcs_free(st_description);

            combined = util_concat(pdescription, " (", pname, ")", NULL);
            st_combined = system_mbstowcs_alloc(combined);
            SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st_combined);
            system_mbstowcs_free(st_description);
            lib_free(combined);
            lib_free(pname);
            lib_free(pdescription);

            if (this_entry) {
                SendMessage(GetDlgItem(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE), CB_SETCURSEL, (WPARAM)cnt, 0);
            }
        }

        rawnet_enumadapter_close();
    }

    if (gray_ungray_items(hwnd)) {
        MessageBox(hwnd, intl_translate_tcs(IDS_ETHERNET_PROBLEM), intl_translate_tcs(IDS_ETHERNET_SUPPORT), MB_ICONINFORMATION | MB_OK);

        /* just quit the dialog before it is open */
        SendMessage( hwnd, WM_COMMAND, IDCANCEL, 0);
    }
}

static void save_ethernet_dialog(HWND hwnd)
{
    TCHAR st_buffer[256];
    char buffer[256];

    GetDlgItemText(hwnd, IDC_ETHERNET_SETTINGS_INTERFACE, st_buffer, 256);
    system_wcstombs(buffer, st_buffer, 256);
    resources_set_string("ETHERNET_INTERFACE", buffer);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDOK:
                    save_ethernet_dialog(hwnd);
                    /* FALL THROUGH */
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
                case IDC_ETHERNET_SETTINGS_INTERFACE:
                    gray_ungray_items(hwnd);
                    break;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_ethernet_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

void ui_ethernet_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_ETHERNET_SETTINGS_DIALOG, hwnd, dialog_proc);
}

#endif // #ifdef HAVE_PCAP
