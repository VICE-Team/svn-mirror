/*
 * uijoystick.c - Joystick user interface for Win32 platforms.
 *
 * Written by
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

#include <windows.h>
#include "res.h"
#include "resources.h"
#include "joy.h"
#include "winmain.h"

/*  These are in joystick.c . */
extern void joystick_calibrate(HWND hwnd);
extern void joystick_ui_get_device_list(HWND joy_hwnd);
extern void joystick_ui_get_autofire_axes(HWND joy_hwnd, int device);
extern void joystick_ui_get_autofire_buttons(HWND joy_hwnd, int device);

static int joy1;
static int joy2;
static int current_keyset_index;
static int current_key_index;

static int keyset[9];

static char *keydefine_texts[]={
    "Press key for NorthWest",
    "Press key for North",
    "Press key for NorthEast",
    "Press key for East",
    "Press key for SouthEast",
    "Press key for South",
    "Press key for SouthWest",
    "Press key for West",
    "Press key for Fire"
};

static long CALLBACK real_callback(HWND hwnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam)
{
    int kcode;

    switch (msg) {
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return 0;
      case WM_GETDLGCODE:
        return DLGC_WANTALLKEYS;
      case WM_KEYDOWN:
        kcode = (lparam >> 16) & 0xff;

        /*  Translate Extended scancodes */
        if (lparam & (1 << 24)) {
            kcode = _kbd_extended_key_tab[kcode];
        }
        if (kcode == K_ESC) {
            kcode = 0;
        }
        keyset[current_key_index] = kcode;
        EndDialog(hwnd, 0);
        return 0;

    }
    return DefDlgProc(hwnd, msg, wparam, lparam);
}

static BOOL CALLBACK key_dialog(HWND hwnd, UINT msg, WPARAM wparam,
                                LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        SetWindowText(hwnd, keydefine_texts[current_key_index]);
        SetWindowLong(hwnd, GWL_WNDPROC, (LONG)real_callback);
        return FALSE;
    }
    return FALSE;
}


static void init_keyset_dialog(HWND hwnd)
{
    if (current_keyset_index==0) {
        resources_get_value("KeySet1NorthWest", (void *)&keyset[KEYSET_NW]);
        resources_get_value("KeySet1North", (void *)&keyset[KEYSET_N]);
        resources_get_value("KeySet1NorthEast", (void *)&keyset[KEYSET_NE]);
        resources_get_value("KeySet1East", (void *)&keyset[KEYSET_E]);
        resources_get_value("KeySet1SouthEast", (void *)&keyset[KEYSET_SE]);
        resources_get_value("KeySet1South", (void *)&keyset[KEYSET_S]);
        resources_get_value("KeySet1SouthWest", (void *)&keyset[KEYSET_SW]);
        resources_get_value("KeySet1West", (void *)&keyset[KEYSET_W]);
        resources_get_value("KeySet1Fire", (void *)&keyset[KEYSET_FIRE]);
        SetWindowText(hwnd,"Configure Keyset A");
    } else {
        resources_get_value("KeySet2NorthWest", (void *)&keyset[KEYSET_NW]);
        resources_get_value("KeySet2North", (void *)&keyset[KEYSET_N]);
        resources_get_value("KeySet2NorthEast", (void *)&keyset[KEYSET_NE]);
        resources_get_value("KeySet2East", (void *)&keyset[KEYSET_E]);
        resources_get_value("KeySet2SouthEast", (void *)&keyset[KEYSET_SE]);
        resources_get_value("KeySet2South", (void *)&keyset[KEYSET_S]);
        resources_get_value("KeySet2SouthWest", (void *)&keyset[KEYSET_SW]);
        resources_get_value("KeySet2West", (void *)&keyset[KEYSET_W]);
        resources_get_value("KeySet2Fire", (void *)&keyset[KEYSET_FIRE]);
        SetWindowText(hwnd,"Configure Keyset B");
    }
    SetDlgItemText(hwnd, IDC_KEY_NW,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_NW]));
    SetDlgItemText(hwnd, IDC_KEY_N,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_N]));
    SetDlgItemText(hwnd, IDC_KEY_NE,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_NE]));
    SetDlgItemText(hwnd, IDC_KEY_E,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_E]));
    SetDlgItemText(hwnd, IDC_KEY_SE,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_SE]));
    SetDlgItemText(hwnd, IDC_KEY_S,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_S]));
    SetDlgItemText(hwnd, IDC_KEY_SW,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_SW]));
    SetDlgItemText(hwnd, IDC_KEY_W,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_W]));
    SetDlgItemText(hwnd, IDC_KEY_FIRE,
                   kbd_code_to_string((kbd_code_t)keyset[KEYSET_FIRE]));
}

static void set_keyset(void)
{
    if (current_keyset_index == 0) {
        resources_set_value("KeySet1NorthWest",
                            (resource_value_t)keyset[KEYSET_NW]);
        resources_set_value("KeySet1North",
                            (resource_value_t)keyset[KEYSET_N]);
        resources_set_value("KeySet1NorthEast",
                            (resource_value_t)keyset[KEYSET_NE]);
        resources_set_value("KeySet1East",
                            (resource_value_t)keyset[KEYSET_E]);
        resources_set_value("KeySet1SouthEast",
                            (resource_value_t)keyset[KEYSET_SE]);
        resources_set_value("KeySet1South",
                            (resource_value_t)keyset[KEYSET_S]);
        resources_set_value("KeySet1SouthWest",
                            (resource_value_t)keyset[KEYSET_SW]);
        resources_set_value("KeySet1West",
                            (resource_value_t)keyset[KEYSET_W]);
        resources_set_value("KeySet1Fire",
                            (resource_value_t)keyset[KEYSET_FIRE]);
    } else {
        resources_set_value("KeySet2NorthWest",
                            (resource_value_t)keyset[KEYSET_NW]);
        resources_set_value("KeySet2North",
                            (resource_value_t)keyset[KEYSET_N]);
        resources_set_value("KeySet2NorthEast",
                            (resource_value_t)keyset[KEYSET_NE]);
        resources_set_value("KeySet2East",
                            (resource_value_t)keyset[KEYSET_E]);
        resources_set_value("KeySet2SouthEast",
                            (resource_value_t)keyset[KEYSET_SE]);
        resources_set_value("KeySet2South",
                            (resource_value_t)keyset[KEYSET_S]);
        resources_set_value("KeySet2SouthWest",
                            (resource_value_t)keyset[KEYSET_SW]);
        resources_set_value("KeySet2West",
                            (resource_value_t)keyset[KEYSET_W]);
        resources_set_value("KeySet2Fire",
                            (resource_value_t)keyset[KEYSET_FIRE]);
    }
}

static BOOL CALLBACK keyset_dialog(HWND hwnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        init_keyset_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_KEYSET_NW:
            current_key_index = KEYSET_NW;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_NW,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_NW]));
            return TRUE;
          case IDC_KEYSET_N:
            current_key_index = KEYSET_N;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_N,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_N]));
            return TRUE;
          case IDC_KEYSET_NE:
            current_key_index = KEYSET_NE;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_NE,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_NE]));
            return TRUE;
          case IDC_KEYSET_E:
            current_key_index = KEYSET_E;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_E,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_E]));
            return TRUE;
          case IDC_KEYSET_SE:
            current_key_index = KEYSET_SE;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_SE,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_SE]));
            return TRUE;
          case IDC_KEYSET_S:
            current_key_index = KEYSET_S;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_S,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_S]));
            return TRUE;
          case IDC_KEYSET_SW:
            current_key_index = KEYSET_SW;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_SW,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_SW]));
            return TRUE;
          case IDC_KEYSET_W:
            current_key_index = KEYSET_W;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_W,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_W]));
            return TRUE;
          case IDC_KEYSET_FIRE:
            current_key_index = KEYSET_FIRE;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEY_DIALOG, hwnd,
                      key_dialog);
            SetDlgItemText(hwnd, IDC_KEY_FIRE,
                           kbd_code_to_string((kbd_code_t)keyset[KEYSET_FIRE]));
            return TRUE;
          case IDOK:
            set_keyset();
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void init_joystick_dialog(HWND hwnd)
{
    HWND joy_hwnd;
    int res_value;
    int device;

    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_DEV1);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"None");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Numpad + RCtrl");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Keyset A");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Keyset B");
    joystick_ui_get_device_list(joy_hwnd);
    resources_get_value("JoyDevice1", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value,0);
    joy1 = device = res_value;

    resources_get_value("JoyAutofire1Speed", (void *)&res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"numeric (see above)");
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_value("JoyAutofire1Axis", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"No button - Autofire disabled");
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_value("JoyAutofire1Button", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                            (device >= JOYDEV_HW1) && (res_value == 0));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON),
                            (device >= JOYDEV_HW1));

    joy_hwnd = GetDlgItem(hwnd,IDC_JOY_DEV2);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"None");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Numpad + RCtrl");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Keyset A");
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Keyset B");
    joystick_ui_get_device_list(joy_hwnd);
    resources_get_value("JoyDevice2", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value,0);
    joy2 = device = res_value;

    resources_get_value("JoyAutofire2Speed", (void *)&res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"numeric (see above)");
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_value("JoyAutofire2Axis", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"No button - Autofire disabled");
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_value("JoyAutofire2Button", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                            (device >= JOYDEV_HW1) && (res_value == 0));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                            (device >= JOYDEV_HW1));

    EnableWindow(GetDlgItem(hwnd, IDC_JOY_CALIBRATE), joystick_inited);
}

static void rebuild_axis_list_1(HWND hwnd, int device)
{
HWND    joy_hwnd;
int     res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"numeric (see above)");
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_value("JoyAutofire1Axis", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_axis_list_2(HWND hwnd, int device)
{
HWND    joy_hwnd;
int     res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"numeric (see above)");
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_value("JoyAutofire2Axis", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_button_list_1(HWND hwnd, int device)
{
HWND    joy_hwnd;
int     res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"No button - Autofire disabled");
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_value("JoyAutofire1Button", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_button_list_2(HWND hwnd, int device)
{
HWND    joy_hwnd;
int     res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"No button - Autofire disabled");
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_value("JoyAutofire2Button", (void *)&res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;
    int res_value;
    int axis;

    switch (msg) {
      case WM_INITDIALOG:
        init_joystick_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_JOY_CALIBRATE:
            joystick_calibrate(hwnd);
            return TRUE;
          case IDC_JOY_CONFIG_A:
            current_keyset_index = 0;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEYSET_DIALOG,
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_CONFIG_B:
            current_keyset_index = 1;
            DialogBox(winmain_instance, (LPCTSTR)IDD_CONFIG_KEYSET_DIALOG,
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_DEV1:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_value("JoyDevice1",
                                    (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                    IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_DEV1,
                                               CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_1(hwnd, res_value);
                    rebuild_button_list_1(hwnd, res_value);
                }
                axis = SendDlgItemMessage(hwnd,IDC_JOY_FIRE1_AXIS,CB_GETCURSEL,0,0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                             (res_value >= JOYDEV_HW1) && (axis == 0));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON),
                             (res_value >= JOYDEV_HW1));
            }
            return TRUE;
          case IDC_JOY_DEV2:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_value("JoyDevice2",
                                    (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                    IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_DEV2,
                                               CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_2(hwnd, res_value);
                    rebuild_button_list_2(hwnd, res_value);
                }
                axis = SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                             (res_value >= JOYDEV_HW1) && (axis == 0));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                             (res_value >= JOYDEV_HW1));
            }
            return TRUE;
          case IDC_JOY_FIRE1_AXIS:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS,
                                               CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                             (res_value == 0));
            }
            return TRUE;
          case IDC_JOY_FIRE1_SPEED:
            if (HIWORD(wparam) == EN_KILLFOCUS) {
                res_value = GetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED,NULL,
                                          FALSE);
                if (res_value > 32)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 32, FALSE);
                if (res_value < 1)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 1, FALSE);
            }
            return TRUE;
          case IDC_JOY_FIRE2_AXIS:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS,
                                               CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                             (res_value == 0));
            }
            return TRUE;
          case IDC_JOY_FIRE2_SPEED:
            if (HIWORD(wparam) == EN_KILLFOCUS) {
                res_value = GetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, NULL,
                                          FALSE);
                if (res_value > 32)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, 32, FALSE);
                if (res_value < 1)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, 1, FALSE);
            }
            return TRUE;
          case IDOK:
            resources_set_value("JoyDevice1",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
            resources_set_value("JoyDevice2",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
            resources_set_value("JoyAutofire1Speed",
                                (resource_value_t)GetDlgItemInt(hwnd,
                                IDC_JOY_FIRE1_SPEED, NULL, FALSE));
            resources_set_value("JoyAutofire1Axis",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_FIRE1_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_value("JoyAutofire1Button",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_FIRE1_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_value("JoyAutofire2Speed",
                                (resource_value_t)GetDlgItemInt(hwnd,
                                IDC_JOY_FIRE2_SPEED, NULL, FALSE));
            resources_set_value("JoyAutofire2Axis",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_FIRE2_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_value("JoyAutofire2Button",
                                (resource_value_t)SendMessage(GetDlgItem(hwnd,
                                IDC_JOY_FIRE2_BUTTON), CB_GETCURSEL, 0, 0));
            EndDialog(hwnd,0);
            return TRUE;
          case IDCANCEL:
            resources_set_value("JoyDevice1",
                                (resource_value_t)joy1);
            resources_set_value("JoyDevice2",
                                (resource_value_t)joy2);
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ui_joystick_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_JOY_SETTINGS_DIALOG,
              hwnd,dialog_proc);
}

void ui_joystick_swap_joystick(void)
{
    int device1;
    int device2;

    resources_get_value("JoyDevice1",(void *)&device1);
    resources_get_value("JoyDevice2",(void *)&device2);
    resources_set_value("JoyDevice1",(resource_value_t)device2);
    resources_set_value("JoyDevice2",(resource_value_t)device1);
    resources_get_value("JoyAutofire1Speed",(void *)&device1);
    resources_get_value("JoyAutofire2Speed",(void *)&device2);
    resources_set_value("JoyAutofire1Speed",(resource_value_t)device2);
    resources_set_value("JoyAutofire2Speed",(resource_value_t)device1);
    resources_get_value("JoyAutofire1Axis",(void *)&device1);
    resources_get_value("JoyAutofire2Axis",(void *)&device2);
    resources_set_value("JoyAutofire1Axis",(resource_value_t)device2);
    resources_set_value("JoyAutofire2Axis",(resource_value_t)device1);
    resources_get_value("JoyAutofire1Button",(void *)&device1);
    resources_get_value("JoyAutofire2Button",(void *)&device2);
    resources_set_value("JoyAutofire1Button",(resource_value_t)device2);
    resources_set_value("JoyAutofire2Button",(resource_value_t)device1);

    resources_get_value("KeySet1NorthWest",(void *)&device1);
    resources_get_value("KeySet2NorthWest",(void *)&device2);
    resources_set_value("KeySet1NorthWest",(resource_value_t)device2);
    resources_set_value("KeySet2NorthWest",(resource_value_t)device1);
    resources_get_value("KeySet1North",(void *)&device1);
    resources_get_value("KeySet2North",(void *)&device2);
    resources_set_value("KeySet1North",(resource_value_t)device2);
    resources_set_value("KeySet2North",(resource_value_t)device1);
    resources_get_value("KeySet1NorthEast",(void *)&device1);
    resources_get_value("KeySet2NorthEast",(void *)&device2);
    resources_set_value("KeySet1NorthEast",(resource_value_t)device2);
    resources_set_value("KeySet2NorthEast",(resource_value_t)device1);
    resources_get_value("KeySet1East",(void *)&device1);
    resources_get_value("KeySet2East",(void *)&device2);
    resources_set_value("KeySet1East",(resource_value_t)device2);
    resources_set_value("KeySet2East",(resource_value_t)device1);
    resources_get_value("KeySet1SouthEast",(void *)&device1);
    resources_get_value("KeySet2SouthEast",(void *)&device2);
    resources_set_value("KeySet1SouthEast",(resource_value_t)device2);
    resources_set_value("KeySet2SouthEast",(resource_value_t)device1);
    resources_get_value("KeySet1South",(void *)&device1);
    resources_get_value("KeySet2South",(void *)&device2);
    resources_set_value("KeySet1South",(resource_value_t)device2);
    resources_set_value("KeySet2South",(resource_value_t)device1);
    resources_get_value("KeySet1SouthWest",(void *)&device1);
    resources_get_value("KeySet2SouthWest",(void *)&device2);
    resources_set_value("KeySet1SouthWest",(resource_value_t)device2);
    resources_set_value("KeySet2SouthWest",(resource_value_t)device1);
    resources_get_value("KeySet1West",(void *)&device1);
    resources_get_value("KeySet2West",(void *)&device2);
    resources_set_value("KeySet1West",(resource_value_t)device2);
    resources_set_value("KeySet2West",(resource_value_t)device1);
    resources_get_value("KeySet1Fire",(void *)&device1);
    resources_get_value("KeySet2Fire",(void *)&device2);
    resources_set_value("KeySet1Fire",(resource_value_t)device2);
    resources_set_value("KeySet2Fire",(resource_value_t)device1);
}

