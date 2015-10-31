/*
 * uitfe.c - Implementation of the TFE/RR-Net settings dialog box.
 *
 * Written by
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
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

#ifdef HAVE_TFE

#include <stdio.h>
#include <windows.h>

#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "rawnet.h"
#include "translate.h"
#include "uitfe.h"
#include "winmain.h"
#include "uilib.h"
#include "util.h"

static BOOL get_tfename(int number, char **ppname, char **ppdescription)
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
        EnableWindow(GetDlgItem(hwnd, IDC_TFE_SETTINGS_ENABLE_T), 0);
        EnableWindow(GetDlgItem(hwnd, IDC_TFE_SETTINGS_ENABLE), 0);
        EnableWindow(GetDlgItem(hwnd, IDOK), 0);
        SetWindowText(GetDlgItem(hwnd,IDC_TFE_SETTINGS_INTERFACE_NAME), "");
        SetWindowText(GetDlgItem(hwnd,IDC_TFE_SETTINGS_INTERFACE_DESC), "");
        enable = 0;
    } else {
        enable = SendMessage(GetDlgItem(hwnd, IDC_TFE_SETTINGS_ENABLE), CB_GETCURSEL, 0, 0) ? 1 : 0;
    }

    EnableWindow(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_T), enable);
    EnableWindow(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE), enable);
    if (machine_class == VICE_MACHINE_VIC20) {
        EnableWindow(GetDlgItem(hwnd, IDC_TFE_SETTINGS_IO_SWAP), enable);
    }

    if (enable) {
        char *pname = NULL;
        char *pdescription = NULL;

        number = (int)SendMessage(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE), CB_GETCURSEL, 0, 0);

        if (get_tfename(number, &pname, &pdescription)) {
            SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_NAME), pname);
            SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_DESC), pdescription);
            lib_free(pname);
            lib_free(pdescription);
        }
    } else {
        SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_NAME), "");
        SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_DESC), "");
    }

    return disabled ? 1 : 0;
}

static uilib_localize_dialog_param tfe_dialog[] = {
    { 0, IDS_TFE_CAPTION, -1 },
    { IDC_TFE_SETTINGS_ENABLE_T, IDS_TFE_ETHERNET, 0 },
    { IDC_TFE_SETTINGS_INTERFACE_T, IDS_TFE_INTERFACE, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_localize_dialog_param tfe_mascuerade_dialog[] = {
    { IDC_TFE_SETTINGS_IO_SWAP, IDS_IO_SWAP, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group tfe_leftgroup[] = {
    { IDC_TFE_SETTINGS_ENABLE_T, 0 },
    { IDC_TFE_SETTINGS_INTERFACE_T, 0 },
    { 0, 0 }
};

static uilib_dialog_group tfe_rightgroup[] = {
    { IDC_TFE_SETTINGS_ENABLE, 0 },
    { IDC_TFE_SETTINGS_INTERFACE, 0 },
    { 0, 0 }
};

static void init_tfe_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int active_value;

    int tfe_enabled;
    int tfe_as_rr_net;
    int xsize, ysize;

    const char *interface_name;

    uilib_localize_dialog(hwnd, tfe_dialog);

    if (machine_class == VICE_MACHINE_VIC20) {
        uilib_localize_dialog(hwnd, tfe_mascuerade_dialog);
    }

    uilib_get_group_extent(hwnd, tfe_leftgroup, &xsize, &ysize);
    uilib_adjust_group_width(hwnd, tfe_leftgroup);
    uilib_move_group(hwnd, tfe_rightgroup, xsize + 30);

    resources_get_int("ETHERNET_ACTIVE", &tfe_enabled);
    resources_get_int("ETHERNET_AS_RR", &tfe_as_rr_net);
    active_value = tfe_as_rr_net ? 2 : (tfe_enabled ? 1 : 0);

    temp_hwnd=GetDlgItem(hwnd,IDC_TFE_SETTINGS_ENABLE);
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Disabled");
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"TFE");
    if (machine_class != VICE_MACHINE_VIC20) {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"RR Net");
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

    if (machine_class == VICE_MACHINE_VIC20) {
        resources_get_int("TFEIOSwap", &active_value);
        CheckDlgButton(hwnd, IDC_TFE_SETTINGS_IO_SWAP, active_value ? BST_CHECKED : BST_UNCHECKED);
    }

    resources_get_string("ETHERNET_INTERFACE", &interface_name);

    if (rawnet_enumadapter_open()) {
        int cnt = 0;

        char *pname;
        char *pdescription;
        char *combined;

        temp_hwnd = GetDlgItem(hwnd,IDC_TFE_SETTINGS_INTERFACE);

        for (cnt = 0; rawnet_enumadapter(&pname, &pdescription); cnt++) {
            BOOL this_entry = FALSE;

            if (strcmp(pname, interface_name) == 0) {
                this_entry = TRUE;
            }
            SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_NAME), pname);
            SetWindowText(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE_DESC), pdescription);
            combined = util_concat(pdescription, " (", pname, ")", NULL);
            SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)combined);
            lib_free(combined);
            lib_free(pname);
            lib_free(pdescription);

            if (this_entry) {
                SendMessage(GetDlgItem(hwnd, IDC_TFE_SETTINGS_INTERFACE), CB_SETCURSEL, (WPARAM)cnt, 0);
            }
        }

        rawnet_enumadapter_close();
    }

    if (gray_ungray_items(hwnd)) {
        /* we have a problem: TFE is disabled. Give a message to the user */
        MessageBox(hwnd, translate_text(IDS_TFE_PROBLEM), translate_text(IDS_TFE_RRNET_SUPPORT), MB_ICONINFORMATION | MB_OK);

        /* just quit the dialog before it is open */
        SendMessage( hwnd, WM_COMMAND, IDCANCEL, 0);
    }
}

static void save_tfe_dialog(HWND hwnd)
{
    int active_value;
    int tfe_enabled;
    int tfe_as_rr_net;
    char buffer[256];

    active_value = (int)SendMessage(GetDlgItem(hwnd, IDC_TFE_SETTINGS_ENABLE), CB_GETCURSEL, 0, 0);

    tfe_enabled = active_value >= 1 ? 1 : 0;
    tfe_as_rr_net = active_value == 2 ? 1 : 0;

    resources_set_int("ETHERNET_ACTIVE", tfe_enabled);
    resources_set_int("ETHERNET_AS_RR", tfe_as_rr_net);

    buffer[255] = '\0';
    GetDlgItemText(hwnd, IDC_TFE_SETTINGS_INTERFACE, buffer, 255);
    resources_set_string("ETHERNET_INTERFACE", buffer);

    if (machine_class == VICE_MACHINE_VIC20) {
        resources_set_int("TFEIOSwap", (IsDlgButtonChecked(hwnd, IDC_TFE_SETTINGS_IO_SWAP) == BST_CHECKED ? 1 : 0 ));
    }
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDOK:
                    save_tfe_dialog(hwnd);
                    /* FALL THROUGH */
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
                case IDC_TFE_SETTINGS_INTERFACE:
                    /* FALL THROUGH */
                case IDC_TFE_SETTINGS_ENABLE:
                    gray_ungray_items(hwnd);
                    break;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_tfe_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

void ui_tfe_settings_dialog(HWND hwnd)
{
    if (machine_class == VICE_MACHINE_VIC20) {
        DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_MASCUERADE_TFE_SETTINGS_DIALOG, hwnd, dialog_proc);
    } else {
        DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_TFE_SETTINGS_DIALOG, hwnd, dialog_proc);
    }
}

#endif // #ifdef HAVE_TFE
