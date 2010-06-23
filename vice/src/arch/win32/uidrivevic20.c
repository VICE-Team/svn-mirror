/*
 * uidrivevic20.c - Implementation of the VIC20 drive settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <stdio.h>
#include <windows.h>
#include <prsht.h>

#include "drive.h"
#include "intl.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uidrivevic20.h"
#include "uilib.h"
#include "winmain.h"

static void enable_controls_for_drive_settings(HWND hwnd, int type)
{
    int drive_type = 0;

    switch (type) {
        case IDC_SELECT_DRIVE_TYPE_1541:
            drive_type = DRIVE_TYPE_1541;
            break;
        case IDC_SELECT_DRIVE_TYPE_1541II:
            drive_type = DRIVE_TYPE_1541II;
            break;
        case IDC_SELECT_DRIVE_TYPE_1570:
            drive_type = DRIVE_TYPE_1570;
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

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER), drive_check_extend_policy(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ASK), drive_check_extend_policy(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ACCESS), drive_check_extend_policy(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_NO_IDLE), drive_check_idle_method(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_TRAP_IDLE), drive_check_idle_method(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES), drive_check_idle_method(drive_type));

    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_2000), drive_check_expansion2000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_4000), drive_check_expansion4000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_6000), drive_check_expansion6000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_8000), drive_check_expansion8000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_A000), drive_check_expansionA000(drive_type));
}

static uilib_localize_dialog_param drive_dialog_trans[] = {
    { IDC_DRIVE_TYPE, IDS_DRIVE_TYPE, 0 },
    { IDC_SELECT_DRIVE_TYPE_NONE, IDS_SELECTNONE, 0 },
    { IDC_40_TRACK_HANDLING, IDS_40_TRACK_HANDLING, 0 },
    { IDC_SELECT_DRIVE_EXTEND_NEVER, IDS_SELECT_DRIVE_EXTEND_NEVER, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ASK, IDS_SELECT_DRIVE_EXTEND_ASK, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ACCESS, IDS_SELECT_DRIVE_EXTEND_ACCESS, 0 },
    { IDC_IDLE_METHOD, IDS_IDLE_METHOD, 0 },
    { IDC_SELECT_DRIVE_IDLE_NO_IDLE, IDS_NONE, 0 },
    { IDC_SELECT_DRIVE_IDLE_TRAP_IDLE, IDS_SELECT_DRIVE_IDLE_TRAP_IDLE, 0 },
    { IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, IDS_SELECT_DRIVE_IDLE_SKIP_CYC, 0 },
    { IDC_DRIVE_EXPANSION, IDS_DRIVE_EXPANSION, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_2000, IDS_TOGGLE_DRIVE_EXPANSION_2000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_4000, IDS_TOGGLE_DRIVE_EXPANSION_4000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_6000, IDS_TOGGLE_DRIVE_EXPANSION_6000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_8000, IDS_TOGGLE_DRIVE_EXPANSION_8000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_A000, IDS_TOGGLE_DRIVE_EXPANSION_A000, 0 },
    { 0, 0, 0 }
};

static uilib_localize_dialog_param parent_dialog_trans[] = {
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group drive_main_group[] = {
    { IDC_DRIVE_TYPE, 1 },
    { IDC_SELECT_DRIVE_TYPE_NONE, 1 },
    { IDC_40_TRACK_HANDLING, 1 },
    { IDC_SELECT_DRIVE_EXTEND_NEVER, 1 },
    { IDC_SELECT_DRIVE_EXTEND_ASK, 1 },
    { IDC_SELECT_DRIVE_EXTEND_ACCESS, 1 },
    { IDC_IDLE_METHOD, 1 },
    { IDC_SELECT_DRIVE_IDLE_NO_IDLE, 1 },
    { IDC_SELECT_DRIVE_IDLE_TRAP_IDLE, 1 },
    { IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, 1 },
    { IDC_DRIVE_EXPANSION, 1 },
    { IDC_TOGGLE_DRIVE_EXPANSION_2000, 1 },
    { IDC_TOGGLE_DRIVE_EXPANSION_4000, 1 },
    { IDC_TOGGLE_DRIVE_EXPANSION_6000, 1 },
    { IDC_TOGGLE_DRIVE_EXPANSION_8000, 1 },
    { IDC_TOGGLE_DRIVE_EXPANSION_A000, 1 },
    { 0, 0 }
};

static uilib_dialog_group drive_left_group[] = {
    { IDC_DRIVE_TYPE, 0 },
    { IDC_SELECT_DRIVE_TYPE_1541, 0 },
    { IDC_SELECT_DRIVE_TYPE_1541II, 0 },
    { IDC_SELECT_DRIVE_TYPE_1570, 0 },
    { IDC_SELECT_DRIVE_TYPE_1571, 0 },
    { IDC_SELECT_DRIVE_TYPE_1581, 0 },
    { IDC_SELECT_DRIVE_TYPE_2031, 0 },
    { IDC_SELECT_DRIVE_TYPE_2040, 0 },
    { IDC_SELECT_DRIVE_TYPE_3040, 0 },
    { IDC_SELECT_DRIVE_TYPE_4040, 0 },
    { IDC_SELECT_DRIVE_TYPE_1001, 0 },
    { IDC_SELECT_DRIVE_TYPE_8050, 0 },
    { IDC_SELECT_DRIVE_TYPE_8250, 0 },
    { IDC_SELECT_DRIVE_TYPE_NONE, 0 },
    { 0, 0 }
};

static uilib_dialog_group drive_middle_group[] = {
    { IDC_40_TRACK_HANDLING, 0 },
    { IDC_SELECT_DRIVE_EXTEND_NEVER, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ASK, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ACCESS, 0 },
    { IDC_DRIVE_EXPANSION, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_2000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_4000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_6000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_8000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_A000, 0 },
    { 0, 0 }
};

static uilib_dialog_group drive_middle_move_group[] = {
    { IDC_SELECT_DRIVE_EXTEND_NEVER, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ASK, 0 },
    { IDC_SELECT_DRIVE_EXTEND_ACCESS, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_2000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_4000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_6000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_8000, 0 },
    { IDC_TOGGLE_DRIVE_EXPANSION_A000, 0 },
    { 0, 0 }
};

static uilib_dialog_group drive_right_group[] = {
    { IDC_IDLE_METHOD, 0 },
    { IDC_SELECT_DRIVE_IDLE_NO_IDLE, 0 },
    { IDC_SELECT_DRIVE_IDLE_TRAP_IDLE, 0 },
    { IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, 0 },
    { 0, 0 }
};

static uilib_dialog_group drive_right_window_group[] = {
    { IDC_IDLE_METHOD, 0 },
    { 0, 0 }
};

static uilib_dialog_group drive_right_move_group[] = {
    { IDC_SELECT_DRIVE_IDLE_NO_IDLE, 0 },
    { IDC_SELECT_DRIVE_IDLE_TRAP_IDLE, 0 },
    { IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, 0 },
    { 0, 0 }
};

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static void init_dialog(HWND hwnd, int num)
{
    int drive_type, drive_extend_image_policy, drive_idle_method, n;
    int drive_true_emulation, iecdevice, enabled;
    int xpos;
    int xstart;
    HWND parent_hwnd;
    RECT rect;

    parent_hwnd = GetParent(hwnd);

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, drive_dialog_trans);

    /* translate the parent window items */
    uilib_localize_dialog(parent_hwnd, parent_dialog_trans);

    /* adjust the size of the elements in the main group */
    uilib_adjust_group_width(hwnd, drive_main_group);

    /* get the max x of the elements in the left group */
    uilib_get_group_max_x(hwnd, drive_left_group, &xpos);

    /* get the min x of the none element of the left group */
    uilib_get_element_min_x(hwnd, IDC_SELECT_DRIVE_TYPE_NONE, &xstart);

    /* resize and move the left group element to the correct position */
    uilib_move_and_set_element_width(hwnd, IDC_DRIVE_TYPE, xstart - 10, xpos - xstart + 20);

    /* get the max x of the left group element */
    uilib_get_element_max_x(hwnd, IDC_DRIVE_TYPE, &xpos);
    
    /* move the middle group elements to the correct position */
    uilib_move_group(hwnd, drive_middle_move_group, xpos + 20);
    uilib_move_element(hwnd, IDC_40_TRACK_HANDLING, xpos + 10);
    uilib_move_element(hwnd, IDC_DRIVE_EXPANSION, xpos + 10);

    xstart = xpos + 20;

    /* get the max x of the middle group */
    uilib_get_group_max_x(hwnd, drive_middle_group, &xpos);
    
    /* resize and move the middle group boxes to the correct position */
    uilib_move_and_set_element_width(hwnd, IDC_40_TRACK_HANDLING, xstart - 10, xpos - xstart + 20);
    uilib_move_and_set_element_width(hwnd, IDC_DRIVE_EXPANSION, xstart - 10, xpos - xstart + 20);

    /* get the max x of the middle group element */
    uilib_get_element_max_x(hwnd, IDC_DRIVE_EXPANSION, &xpos);

    /* move the right group elements to the correct position */
    uilib_move_group(hwnd, drive_right_move_group, xpos + 20);
    uilib_move_element(hwnd, IDC_IDLE_METHOD, xpos + 10);

    xstart = xpos + 20;

    /* get the max x of the right group */
    uilib_get_group_max_x(hwnd, drive_right_group, &xpos);

    /* resize and move the right group element to the correct position */
    uilib_move_and_set_element_width(hwnd, IDC_IDLE_METHOD, xstart - 10, xpos - xstart + 20);

    /* get the max x of the right window group */
    uilib_get_group_max_x(hwnd, drive_right_window_group, &xpos);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xpos + 10, rect.bottom - rect.top, TRUE);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(parent_hwnd, &rect);
    MoveWindow(parent_hwnd, rect.left, rect.top, xpos + 10, rect.bottom - rect.top, TRUE);

    /* recenter the buttons in the newly resized dialog window */
    uilib_center_buttons(parent_hwnd, move_buttons_group, 0);

    resources_get_int_sprintf("IECDevice%i", &iecdevice, num);
    resources_get_int("DriveTrueEmulation", &drive_true_emulation);
    enabled = drive_true_emulation && !iecdevice;

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1541), enabled && drive_check_type(DRIVE_TYPE_1541, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1541II), enabled && drive_check_type(DRIVE_TYPE_1541II, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1570), enabled && drive_check_type(DRIVE_TYPE_1570, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1571), enabled && drive_check_type(DRIVE_TYPE_1571, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1581), enabled && drive_check_type(DRIVE_TYPE_1581, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_2031), enabled && drive_check_type(DRIVE_TYPE_2031, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_2040), enabled && drive_check_type(DRIVE_TYPE_2040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_3040), enabled && drive_check_type(DRIVE_TYPE_3040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_4040), enabled && drive_check_type(DRIVE_TYPE_4040, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1001), enabled && drive_check_type(DRIVE_TYPE_1001, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8050), enabled && drive_check_type(DRIVE_TYPE_8050, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8250), enabled && drive_check_type(DRIVE_TYPE_8250, num - 8));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_NONE), enabled);

    resources_get_int_sprintf("Drive%dType", &drive_type, num);
    resources_get_int_sprintf("Drive%dExtendImagePolicy", &drive_extend_image_policy, num);
    resources_get_int_sprintf("Drive%dIdleMethod", &drive_idle_method, num);

    switch (drive_type) {
        case DRIVE_TYPE_NONE:
            n = IDC_SELECT_DRIVE_TYPE_NONE;
            break;
        case DRIVE_TYPE_1541:
            n = IDC_SELECT_DRIVE_TYPE_1541;
            break;
        case DRIVE_TYPE_1541II:
            n = IDC_SELECT_DRIVE_TYPE_1541II;
            break;
        case DRIVE_TYPE_1570:
            n = IDC_SELECT_DRIVE_TYPE_1570;
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

    if (!enabled) {
        n = IDC_SELECT_DRIVE_TYPE_NONE;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_TYPE_1541, IDC_SELECT_DRIVE_TYPE_NONE, n);

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

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER, IDC_SELECT_DRIVE_EXTEND_ACCESS, n);

    switch (drive_idle_method) {
        case DRIVE_IDLE_NO_IDLE:
            n = IDC_SELECT_DRIVE_IDLE_NO_IDLE;
            break;
        case DRIVE_IDLE_TRAP_IDLE:
            n = IDC_SELECT_DRIVE_IDLE_TRAP_IDLE;
            break;
        case DRIVE_IDLE_SKIP_CYCLES:
            n = IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES;
            break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_IDLE_NO_IDLE, IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, n);

    resources_get_int_sprintf("Drive%dRAM2000", &n, num);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_2000, n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int_sprintf("Drive%dRAM4000", &n, num);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_4000, n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int_sprintf("Drive%dRAM6000", &n, num);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_6000, n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int_sprintf("Drive%dRAM8000", &n, num);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_8000, n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int_sprintf("Drive%dRAMA000", &n, num);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_A000, n ? BST_CHECKED : BST_UNCHECKED);
}

static BOOL CALLBACK dialog_proc(int num, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    char tmp[256];

    switch (msg) {
        case WM_INITDIALOG:
            init_dialog(hwnd, num);
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDC_SELECT_DRIVE_TYPE_NONE:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_NONE, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1541:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1541, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1541II:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1541II, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1570:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1570, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1571:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1571, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1581:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1581, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_2031:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_2031, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_2040:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_2040, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_3040:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_3040, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_4040:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_4040, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_1001:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_1001, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_8050:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_8050, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_TYPE_8250:
                    resources_set_int_sprintf("Drive%dType", DRIVE_TYPE_8250, num);
                    enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
                    break;
                case IDC_SELECT_DRIVE_EXTEND_NEVER:
                    resources_set_int_sprintf("Drive%dExtendImagePolicy", DRIVE_EXTEND_NEVER, num);
                    break;
                case IDC_SELECT_DRIVE_EXTEND_ASK:
                    resources_set_int_sprintf("Drive%dExtendImagePolicy", DRIVE_EXTEND_ASK, num);
                    break;
                case IDC_SELECT_DRIVE_EXTEND_ACCESS:
                    resources_set_int_sprintf("Drive%dExtendImagePolicy", DRIVE_EXTEND_ACCESS, num);
                    break;
                case IDC_SELECT_DRIVE_IDLE_NO_IDLE:
                    resources_set_int_sprintf("Drive%dIdleMethod", DRIVE_IDLE_NO_IDLE, num);
                    break;
                case IDC_SELECT_DRIVE_IDLE_TRAP_IDLE:
                    resources_set_int_sprintf("Drive%dIdleMethod", DRIVE_IDLE_TRAP_IDLE, num);
                    break;
                case IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES:
                    resources_set_int_sprintf("Drive%dIdleMethod", DRIVE_IDLE_SKIP_CYCLES, num);
                    break;
                case IDC_TOGGLE_DRIVE_EXPANSION_2000:
                    sprintf(tmp, "Drive%dRAM2000", num);
                    resources_toggle(tmp, NULL);
                    break;
                case IDC_TOGGLE_DRIVE_EXPANSION_4000:
                    sprintf(tmp, "Drive%dRAM4000", num);
                    resources_toggle(tmp, NULL);
                    break;
                case IDC_TOGGLE_DRIVE_EXPANSION_6000:
                    sprintf(tmp, "Drive%dRAM6000", num);
                    resources_toggle(tmp, NULL);
                    break;
                case IDC_TOGGLE_DRIVE_EXPANSION_8000:
                    sprintf(tmp, "Drive%dRAM8000", num);
                    resources_toggle(tmp, NULL);
                    break;
                case IDC_TOGGLE_DRIVE_EXPANSION_A000:
                    sprintf(tmp, "Drive%dRAMA000", num);
                    resources_toggle(tmp, NULL);
                    break;
                default:
                    return FALSE;
            }
            return TRUE;
    }
    return FALSE;
}

#define _CALLBACK(num)                                                                      \
static INT_PTR CALLBACK callback_##num(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam) \
{                                                                                           \
    return dialog_proc(num, dialog, msg, wparam, lparam);                                   \
}

_CALLBACK(8)
_CALLBACK(9)
_CALLBACK(10)
_CALLBACK(11)

void uidrivevic20_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 4; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG_VIC20);
        psp[i].pszIcon = NULL;
#else
        psp[i].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG_VIC20);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_8;
    psp[0].pszTitle = translate_text(IDS_DRIVE_8);
    psp[1].pfnDlgProc = callback_9;
    psp[1].pszTitle = translate_text(IDS_DRIVE_9);
    psp[2].pfnDlgProc = callback_10;
    psp[2].pszTitle = translate_text(IDS_DRIVE_10);
    psp[3].pfnDlgProc = callback_11;
    psp[3].pszTitle = translate_text(IDS_DRIVE_11);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_DRIVE_SETTINGS);
    psh.nPages = 4;
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
