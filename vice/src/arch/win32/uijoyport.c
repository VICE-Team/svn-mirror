/*
 * uijoyport.c - Implementation of the joyport system settings dialog box.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <windows.h>
#include <tchar.h>

#include "joyport.h"
#include "lib.h"
#if 1
#include "log.h"
#endif
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "types.h"
#include "uilib.h"
#include "uimouse.h"
#include "winmain.h"

static int ports[4] = { 0, 0, 0, 0};
static joyport_desc_t *devices_port_1 = NULL;
static joyport_desc_t *devices_port_2 = NULL;
static joyport_desc_t *devices_port_3 = NULL;
static joyport_desc_t *devices_port_4 = NULL;

static void enable_joyport_controls(HWND hwnd)
{
    if (ports[JOYPORT_1]) {
        EnableWindow(GetDlgItem(hwnd, IDC_JOYPORT1), 1);
    }
    if (ports[JOYPORT_2]) {
        EnableWindow(GetDlgItem(hwnd, IDC_JOYPORT2), 1);
    }
    if (ports[JOYPORT_3]) {
        EnableWindow(GetDlgItem(hwnd, IDC_JOYPORT3), 1);
    }
    if (ports[JOYPORT_4]) {
        EnableWindow(GetDlgItem(hwnd, IDC_JOYPORT4), 1);
    }
}

static uilib_localize_dialog_param joyport_dialog_trans[] = {
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group joyport_left_group[JOYPORT_MAX_PORTS + 1];
static uilib_dialog_group joyport_right_group[JOYPORT_MAX_PORTS + 1];

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static void init_joyport_dialog(HWND hwnd)
{
    HWND temp_hwnd1;
    HWND temp_hwnd2 = 0;
    HWND temp_hwnd3 = 0;
    HWND temp_hwnd4 = 0;
    HWND element;
    int res_value1 = 0;
    int res_value2 = 0;
    int res_value3 = 0;
    int res_value4 = 0;
    int xpos;
    RECT rect;
    int i = 0;
    int joy1 = 0;
    int joy2 = 0;
    int joy3 = 0;
    int joy4 = 0;
    char *tmp_text = NULL;

    devices_port_1 = joyport_get_valid_devices(JOYPORT_1);
    devices_port_2 = joyport_get_valid_devices(JOYPORT_2);
    devices_port_3 = joyport_get_valid_devices(JOYPORT_3);
    devices_port_4 = joyport_get_valid_devices(JOYPORT_4);

    /* localize possible port items, and build left and right groups */
    if (ports[JOYPORT_1]) {
        tmp_text = lib_msprintf(translate_text(IDS_JOYPORT_S_DEVICE), translate_text(joyport_get_port_trans_name(JOYPORT_1)));
        element = GetDlgItem(hwnd, IDC_JOYPORT1_LABEL);
        SetWindowText(element, tmp_text);
        lib_free(tmp_text);
        joyport_left_group[i].idc = IDC_JOYPORT1_LABEL;
        joyport_left_group[i].element_type = 0;
        joyport_right_group[i].idc = IDC_JOYPORT1;
        joyport_right_group[i].element_type = 0;
        ++i;
    }

    if (ports[JOYPORT_2]) {
        tmp_text = lib_msprintf(translate_text(IDS_JOYPORT_S_DEVICE), translate_text(joyport_get_port_trans_name(JOYPORT_2)));
        element = GetDlgItem(hwnd, IDC_JOYPORT2_LABEL);
        SetWindowText(element, tmp_text);
        lib_free(tmp_text);
        joyport_left_group[i].idc = IDC_JOYPORT2_LABEL;
        joyport_left_group[i].element_type = 0;
        joyport_right_group[i].idc = IDC_JOYPORT2;
        joyport_right_group[i].element_type = 0;
        ++i;
    }  

    if (ports[JOYPORT_3]) {
        tmp_text = lib_msprintf(translate_text(IDS_JOYPORT_S_DEVICE), translate_text(joyport_get_port_trans_name(JOYPORT_3)));
        element = GetDlgItem(hwnd, IDC_JOYPORT3_LABEL);
        SetWindowText(element, tmp_text);
        lib_free(tmp_text);
        joyport_left_group[i].idc = IDC_JOYPORT3_LABEL;
        joyport_left_group[i].element_type = 0;
        joyport_right_group[i].idc = IDC_JOYPORT3;
        joyport_right_group[i].element_type = 0;
        ++i;
    }  

    if (ports[JOYPORT_4]) {
        tmp_text = lib_msprintf(translate_text(IDS_JOYPORT_S_DEVICE), translate_text(joyport_get_port_trans_name(JOYPORT_4)));
        element = GetDlgItem(hwnd, IDC_JOYPORT4_LABEL);
        SetWindowText(element, tmp_text);
        lib_free(tmp_text);
        joyport_left_group[i].idc = IDC_JOYPORT4_LABEL;
        joyport_left_group[i].element_type = 0;
        joyport_right_group[i].idc = IDC_JOYPORT4;
        joyport_right_group[i].element_type = 0;
        ++i;
    }  

    joyport_left_group[i].idc = 0;
    joyport_left_group[i].element_type = 0;
    joyport_right_group[i].idc = 0;
    joyport_right_group[i].element_type = 0;

    /* translate other dialog items */
    uilib_localize_dialog(hwnd, joyport_dialog_trans);

    /* adjust the size of the elements in the left group */
    uilib_adjust_group_width(hwnd, joyport_left_group);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, joyport_left_group, &xpos);

    /* move the right group to the correct position */
    uilib_move_group(hwnd, joyport_right_group, xpos + 10);

    /* get the max x of the right group */
    uilib_get_group_max_x(hwnd, joyport_right_group, &xpos);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xpos + 20, rect.bottom - rect.top, TRUE);

    /* recenter the buttons in the newly resized dialog window */
    uilib_center_buttons(hwnd, move_buttons_group, 0);

    /* handle items of the ports */
    if (ports[JOYPORT_1]) {
        temp_hwnd1 = GetDlgItem(hwnd, IDC_JOYPORT1);
        resources_get_int("JoyPort1Device", &joy1);
        for (i = 0; devices_port_1[i].name; ++i) {
            SendMessage(temp_hwnd1, CB_ADDSTRING, 0, (LPARAM)translate_text(devices_port_1[i].trans_name));
            if (devices_port_1[i].id == joy1) {
                res_value1 = i;
            }
        }
        SendMessage(temp_hwnd1, CB_SETCURSEL, (WPARAM)res_value1, 0);
    }

    if (ports[JOYPORT_2]) {
        temp_hwnd2 = GetDlgItem(hwnd, IDC_JOYPORT2);
        resources_get_int("JoyPort2Device", &joy2);
        for (i = 0; devices_port_2[i].name; ++i) {
            SendMessage(temp_hwnd2, CB_ADDSTRING, 0, (LPARAM)translate_text(devices_port_2[i].trans_name));
            if (devices_port_2[i].id == joy2) {
                res_value2 = i;
            }
        }
        SendMessage(temp_hwnd2, CB_SETCURSEL, (WPARAM)res_value2, 0);
    }

    if (ports[JOYPORT_3]) {
        temp_hwnd3 = GetDlgItem(hwnd, IDC_JOYPORT3);
        resources_get_int("JoyPort3Device", &joy3);
        for (i = 0; devices_port_3[i].name; ++i) {
            SendMessage(temp_hwnd3, CB_ADDSTRING, 0, (LPARAM)translate_text(devices_port_3[i].trans_name));
            if (devices_port_3[i].id == joy3) {
                res_value3 = i;
            }
        }
        SendMessage(temp_hwnd3, CB_SETCURSEL, (WPARAM)res_value3, 0);
    }

    if (ports[JOYPORT_4]) {
        temp_hwnd4 = GetDlgItem(hwnd, IDC_JOYPORT4);
        resources_get_int("JoyPort4Device", &joy4);
        for (i = 0; devices_port_4[i].name; ++i) {
            SendMessage(temp_hwnd4, CB_ADDSTRING, 0, (LPARAM)translate_text(devices_port_4[i].trans_name));
            if (devices_port_4[i].id == joy4) {
                res_value4 = i;
            }
        }
        SendMessage(temp_hwnd4, CB_SETCURSEL, (WPARAM)res_value4, 0);
    }

    enable_joyport_controls(hwnd);
}

static void free_device_ports(void)
{
    if (devices_port_1) {
        lib_free(devices_port_1);
        devices_port_1 = NULL;
    }
    if (devices_port_2) {
        lib_free(devices_port_2);
        devices_port_2 = NULL;
    }
    if (devices_port_3) {
        lib_free(devices_port_3);
        devices_port_3 = NULL;
    }
    if (devices_port_4) {
        lib_free(devices_port_4);
        devices_port_4 = NULL;
    }
}

static void end_joyport_dialog(HWND hwnd)
{
    int joy1 = 0;
    int joy2 = 0;
    int joy3 = 0;
    int joy4 = 0;
    int id1 = 0;
    int id2 = 0;
    int id3 = 0;
    int id4 = 0;

    /* Because all ports need to be set at the same time,
       and actually they are set one at a time,
       set all ports to 'NONE' so there are 'fake' conflicts.
     */
    if (ports[JOYPORT_1]) {
        resources_set_int("JoyPort1Device", JOYPORT_ID_NONE);
    }
    if (ports[JOYPORT_2]) {
        resources_set_int("JoyPort2Device", JOYPORT_ID_NONE);
    }
    if (ports[JOYPORT_3]) {
        resources_set_int("JoyPort3Device", JOYPORT_ID_NONE);
    }
    if (ports[JOYPORT_4]) {
        resources_set_int("JoyPort4Device", JOYPORT_ID_NONE);
    }

    if (ports[JOYPORT_1]) {
        joy1 = (int)SendMessage(GetDlgItem(hwnd, IDC_JOYPORT1), CB_GETCURSEL, 0, 0);
        id1 = devices_port_1[joy1].id;
        resources_set_int("JoyPort1Device", id1);
    }

    if (ports[JOYPORT_2]) {
        joy2 = (int)SendMessage(GetDlgItem(hwnd, IDC_JOYPORT2), CB_GETCURSEL, 0, 0);
        id2 = devices_port_2[joy2].id;
        resources_set_int("JoyPort2Device", id2);
    }
    if (ports[JOYPORT_3]) {
        joy3 = (int)SendMessage(GetDlgItem(hwnd, IDC_JOYPORT3), CB_GETCURSEL, 0, 0);
        id3 = devices_port_3[joy3].id;
        resources_set_int("JoyPort3Device", id3);
    }
    if (ports[JOYPORT_4]) {
        joy4 = (int)SendMessage(GetDlgItem(hwnd, IDC_JOYPORT4), CB_GETCURSEL, 0, 0);
        id4 = devices_port_4[joy4].id;
        resources_set_int("JoyPort4Device", id4);
    }

    free_device_ports();
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
        case WM_COMMAND:
            command = LOWORD(wparam);
            switch (command) {
                case IDOK:
                    end_joyport_dialog(hwnd);
                case IDCANCEL:
                    free_device_ports();
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            return FALSE;
        case WM_CLOSE:
            free_device_ports();
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_joyport_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

void ui_joyport_settings_dialog(HWND hwnd, int port1, int port2, int port3, int port4)
{
    int total = (port1 << 3) | (port2 << 2) | (port3 << 1) | port4;
    ports[0] = port1;
    ports[1] = port2;
    ports[2] = port3;
    ports[3] = port4;

    switch (total) {
        case 15:
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOYPORT1234_SETTINGS_DIALOG, hwnd, dialog_proc);
            break;
        case 14:
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOYPORT123_SETTINGS_DIALOG, hwnd, dialog_proc);
            break;
        case 12:
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOYPORT12_SETTINGS_DIALOG, hwnd, dialog_proc);
            break;
        case 11:
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOYPORT134_SETTINGS_DIALOG, hwnd, dialog_proc);
            break;
        case 3:
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOYPORT34_SETTINGS_DIALOG, hwnd, dialog_proc);
            break;
    }
}
