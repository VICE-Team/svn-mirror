/*
 * uiacia.c - Implementation of the ACIA settings dialog box.
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
#include <windows.h>
#include <tchar.h>

#include "res.h"
#include "resources.h"
#include "translate.h"
#include "uiacia.h"
#include "winmain.h"
#include "intl.h"
#include "uilib.h"


#define MAXRS232 4


static const int interrupt_names[] = {
    IDS_NONE, IDS_IRQ, IDS_NMI, 0
};

static const int mode_names[] = {
    IDS_NORMAL, IDS_SWIFTLINK, IDS_TURBO232, 0
};

static unsigned int support_enable;
static int *support_location;
static unsigned int support_interrupt;
static unsigned int support_mode;

static void enable_acia_controls(HWND hwnd)
{
    int acia_enable, acia_location, acia_device, acia_interrupt, acia_mode;

    acia_enable = 0;
    acia_location = 0;
    acia_device = 1;
    acia_interrupt = 0;
    acia_mode = 0;
    
    if (support_mode !=0)
        acia_mode = 1;

    if (support_enable != 0)
        acia_enable = 1;

    if (support_location != NULL)
        acia_location = 1;

    if (support_interrupt != 0)
        acia_interrupt = 1;

    if (support_enable != 0) {
        if (IsDlgButtonChecked(hwnd, IDC_ACIA_ENABLE) != BST_CHECKED) {
            acia_location = 0;
            acia_device = 0;
            acia_interrupt = 0;
            acia_mode = 0;
        }
    }

    EnableWindow(GetDlgItem(hwnd, IDC_ACIA_ENABLE), acia_enable);
    EnableWindow(GetDlgItem(hwnd, IDC_ACIA_LOCATION), acia_location);
    EnableWindow(GetDlgItem(hwnd, IDC_ACIA_DEVICE), acia_device);
    EnableWindow(GetDlgItem(hwnd, IDC_ACIA_INTERRUPT), acia_interrupt);
    EnableWindow(GetDlgItem(hwnd, IDC_ACIA_MODE), acia_mode);
}

static void init_acia_dialog(HWND hwnd)
{
HWND temp_hwnd;
int res_value;
unsigned int i;
RECT rect;
RECT child_rect;
int min_width;
int xpos;
int xsize, ysize;

    SetWindowText(hwnd, intl_translate_text_new(IDS_ACIA_CAPTION));
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_ENABLE);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_ACIA_ENABLE));
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_DEVICE_LABEL);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_ACIA_DEVICE));
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_LOCATION_LABEL);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_ACIA_LOCATION));
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_INTERRUPT_LABEL);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_ACIA_INTERRUPT));
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_MODE_LABEL);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_ACIA_MODE));
    temp_hwnd = GetDlgItem(hwnd, IDOK);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_OK));
    temp_hwnd = GetDlgItem(hwnd, IDCANCEL);
    SetWindowText(temp_hwnd, intl_translate_text_new(IDS_CANCEL));

    GetClientRect(hwnd, &rect);

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_ENABLE);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    xsize += 20;
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    min_width = child_rect.left + xsize + 10;

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_DEVICE_LABEL);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    xpos = child_rect.left + xsize + 10;

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_LOCATION_LABEL);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    if (xpos < child_rect.left + xsize + 10) {
        xpos = child_rect.left + xsize + 10;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_INTERRUPT_LABEL);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    if (xpos < child_rect.left + xsize + 10) {
        xpos = child_rect.left + xsize + 10;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_MODE_LABEL);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    MoveWindow(temp_hwnd, child_rect.left, child_rect.top, xsize, child_rect.bottom - child_rect.top, TRUE);
    if (xpos < child_rect.left + xsize + 10) {
        xpos = child_rect.left + xsize + 10;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_DEVICE);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    MoveWindow(temp_hwnd, xpos, child_rect.top, child_rect.right - child_rect.left, child_rect.bottom - child_rect.top, TRUE);
    if (min_width < xpos + child_rect.right - child_rect.left) {
        min_width = xpos + child_rect.right - child_rect.left;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_LOCATION);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    MoveWindow(temp_hwnd, xpos, child_rect.top, child_rect.right - child_rect.left, child_rect.bottom - child_rect.top, TRUE);
    if (min_width < xpos + child_rect.right - child_rect.left) {
        min_width = xpos + child_rect.right - child_rect.left;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_INTERRUPT);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    MoveWindow(temp_hwnd, xpos, child_rect.top, child_rect.right - child_rect.left, child_rect.bottom - child_rect.top, TRUE);
    if (min_width < xpos + child_rect.right - child_rect.left) {
        min_width = xpos + child_rect.right - child_rect.left;
    }

    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_MODE);
    GetClientRect(temp_hwnd, &child_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&child_rect, 2);
    MoveWindow(temp_hwnd, xpos, child_rect.top, child_rect.right - child_rect.left, child_rect.bottom - child_rect.top, TRUE);
    if (min_width < xpos + child_rect.right - child_rect.left) {
        min_width = xpos + child_rect.right - child_rect.left;
    }

    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, min_width + 20, rect.bottom - rect.top, TRUE);

    if (support_enable != 0) {
        resources_get_value("Acia1Enable", (void *)&res_value);
        CheckDlgButton(hwnd, IDC_ACIA_ENABLE,
                       res_value ? BST_CHECKED : BST_UNCHECKED);
    } else {
        CheckDlgButton(hwnd, IDC_ACIA_ENABLE, BST_CHECKED);
    }

    resources_get_value("Acia1Dev", (void *)&res_value);
    temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_DEVICE);
    for (i = 0; i < MAXRS232; i++) {
        TCHAR st[20];
        _stprintf(st, translate_text(IDS_RS232_DEVICE_I), i + 1);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    if (support_interrupt != 0) {
        int res_value_loop;

        resources_get_value("Acia1Irq", (void *)&res_value);
        temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_INTERRUPT);
        for (res_value_loop = 0; interrupt_names[res_value_loop];
            res_value_loop++) {
            SendMessage(temp_hwnd, CB_ADDSTRING, 0,
                        (LPARAM)(TCHAR *)intl_translate_text_new(interrupt_names[res_value_loop]));
        }
        SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    }

    if (support_mode != 0) {
        int res_value_loop;

        resources_get_value("Acia1Mode", (void *)&res_value);
        temp_hwnd = GetDlgItem(hwnd, IDC_ACIA_MODE);
        for (res_value_loop = 0; mode_names[res_value_loop];
            res_value_loop++) {
            SendMessage(temp_hwnd, CB_ADDSTRING, 0,
                        (LPARAM)(TCHAR *)intl_translate_text_new(mode_names[res_value_loop]));
        }
        SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    }

    enable_acia_controls(hwnd);
}

static void end_acia_dialog(HWND hwnd)
{
    if (support_enable != 0) {
        resources_set_value("Acia1Enable", (resource_value_t)
                            (IsDlgButtonChecked
                            (hwnd, IDC_ACIA_ENABLE) == BST_CHECKED ? 1 : 0 ));
    }

    resources_set_value("Acia1Dev",(resource_value_t)
                        SendMessage(GetDlgItem(hwnd, IDC_ACIA_DEVICE),
                        CB_GETCURSEL, 0, 0));

    if (support_interrupt != 0) {
        resources_set_value("Acia1Irq",(resource_value_t)
                            SendMessage(GetDlgItem(hwnd,
                            IDC_ACIA_INTERRUPT), CB_GETCURSEL, 0, 0));
    }

    if (support_mode != 0) {
        resources_set_value("Acia1Mode",(resource_value_t)
                            SendMessage(GetDlgItem(hwnd,
                            IDC_ACIA_MODE), CB_GETCURSEL, 0, 0));
    }
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
        case WM_COMMAND:
            command = LOWORD(wparam);
            switch (command) {
                case IDC_ACIA_ENABLE:
                    enable_acia_controls(hwnd);
                    break;
                case IDOK:
                    end_acia_dialog(hwnd);
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_acia_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

void ui_acia_settings_dialog(HWND hwnd, unsigned int enable, int *location,
                             unsigned int interrupt, unsigned int mode)
{
    support_enable = enable;
    support_location = location;
    support_interrupt = interrupt;
    support_mode = mode;

    DialogBox(winmain_instance, (LPCTSTR)IDD_ACIA_SETTINGS_DIALOG, hwnd,
              dialog_proc);
}
