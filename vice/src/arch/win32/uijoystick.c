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
#include <tchar.h>

#include "intl.h"
#include "joy.h"
#include "res.h"
#include "resources.h"
#include "translate.h"
#include "winmain.h"
#include "kbd.h"

/*  These are in joystick.c . */
extern void joystick_calibrate(HWND hwnd);
extern void joystick_ui_get_device_list(HWND joy_hwnd);
extern void joystick_ui_get_autofire_axes(HWND joy_hwnd, int device);
extern void joystick_ui_get_autofire_buttons(HWND joy_hwnd, int device);

extern const TCHAR *kbd_code_to_string(kbd_code_t kcode);

static int joy1;
static int joy2;
static int current_keyset_index;
static int current_key_index;

static int keyset[9];

static int keydefine_texts[] = {
    IDS_PRESS_KEY_NORTHWEST,
    IDS_PRESS_KEY_NORTH,
    IDS_PRESS_KEY_NORTHEAST,
    IDS_PRESS_KEY_EAST,
    IDS_PRESS_KEY_SOUTHEAST,
    IDS_PRESS_KEY_SOUTH,
    IDS_PRESS_KEY_SOUTHWEST,
    IDS_PRESS_KEY_WEST,
    IDS_PRESS_KEY_FIRE
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
        SetWindowText(hwnd, translate_text(keydefine_texts[current_key_index]));
        SetWindowLong(hwnd, GWL_WNDPROC, (LONG)real_callback);
        return FALSE;
    }
    return FALSE;
}

typedef enum {
    KEYSET_NW,
    KEYSET_N,
    KEYSET_NE,
    KEYSET_E,
    KEYSET_SE,
    KEYSET_S,
    KEYSET_SW,
    KEYSET_W,
    KEYSET_FIRE
} joystick_direction_t;

static void init_keyset_dialog(HWND hwnd)
{
    if (current_keyset_index == 0) {
        resources_get_int("KeySet1NorthWest", &keyset[KEYSET_NW]);
        resources_get_int("KeySet1North", &keyset[KEYSET_N]);
        resources_get_int("KeySet1NorthEast", &keyset[KEYSET_NE]);
        resources_get_int("KeySet1East", &keyset[KEYSET_E]);
        resources_get_int("KeySet1SouthEast", &keyset[KEYSET_SE]);
        resources_get_int("KeySet1South", &keyset[KEYSET_S]);
        resources_get_int("KeySet1SouthWest", &keyset[KEYSET_SW]);
        resources_get_int("KeySet1West", &keyset[KEYSET_W]);
        resources_get_int("KeySet1Fire", &keyset[KEYSET_FIRE]);
        SetWindowText(hwnd, translate_text(IDS_CONFIGURE_KEYSET_A));
    } else {
        resources_get_int("KeySet2NorthWest", &keyset[KEYSET_NW]);
        resources_get_int("KeySet2North", &keyset[KEYSET_N]);
        resources_get_int("KeySet2NorthEast", &keyset[KEYSET_NE]);
        resources_get_int("KeySet2East", &keyset[KEYSET_E]);
        resources_get_int("KeySet2SouthEast", &keyset[KEYSET_SE]);
        resources_get_int("KeySet2South", &keyset[KEYSET_S]);
        resources_get_int("KeySet2SouthWest", &keyset[KEYSET_SW]);
        resources_get_int("KeySet2West", &keyset[KEYSET_W]);
        resources_get_int("KeySet2Fire", &keyset[KEYSET_FIRE]);
        SetWindowText(hwnd, translate_text(IDS_CONFIGURE_KEYSET_B));
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
        resources_set_int("KeySet1NorthWest", keyset[KEYSET_NW]);
        resources_set_int("KeySet1North", keyset[KEYSET_N]);
        resources_set_int("KeySet1NorthEast",keyset[KEYSET_NE]);
        resources_set_int("KeySet1East", keyset[KEYSET_E]);
        resources_set_int("KeySet1SouthEast", keyset[KEYSET_SE]);
        resources_set_int("KeySet1South", keyset[KEYSET_S]);
        resources_set_int("KeySet1SouthWest", keyset[KEYSET_SW]);
        resources_set_int("KeySet1West", keyset[KEYSET_W]);
        resources_set_int("KeySet1Fire", keyset[KEYSET_FIRE]);
    } else {
        resources_set_int("KeySet2NorthWest", keyset[KEYSET_NW]);
        resources_set_int("KeySet2North", keyset[KEYSET_N]);
        resources_set_int("KeySet2NorthEast", keyset[KEYSET_NE]);
        resources_set_int("KeySet2East", keyset[KEYSET_E]);
        resources_set_int("KeySet2SouthEast", keyset[KEYSET_SE]);
        resources_set_int("KeySet2South", keyset[KEYSET_S]);
        resources_set_int("KeySet2SouthWest", keyset[KEYSET_SW]);
        resources_set_int("KeySet2West", keyset[KEYSET_W]);
        resources_set_int("KeySet2Fire", keyset[KEYSET_FIRE]);
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
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NONE));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMPAD_AND_RCTRL));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_KEYSET_A));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_KEYSET_B));
    joystick_ui_get_device_list(joy_hwnd);
    resources_get_int("JoyDevice1", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value,0);
    joy1 = device = res_value;

    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire1Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_int("JoyAutofire1Speed", &res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire1Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire1Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                            (device >= JOYDEV_HW1) && (res_value == 0));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON),
                            (device >= JOYDEV_HW1));

    joy_hwnd = GetDlgItem(hwnd,IDC_JOY_DEV2);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NONE));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMPAD_AND_RCTRL));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_KEYSET_A));
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_KEYSET_B));
    joystick_ui_get_device_list(joy_hwnd);
    resources_get_int("JoyDevice2", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value,0);
    joy2 = device = res_value;

    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire2Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_int("JoyAutofire2Speed", &res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire2Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire2Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                            (device >= JOYDEV_HW1) && (res_value == 0));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                            (device >= JOYDEV_HW1));
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON),
                            (device >= JOYDEV_HW1));

    EnableWindow(GetDlgItem(hwnd, IDC_JOY_CALIBRATE), joystick_uses_direct_input());
}

static void rebuild_axis_list_1(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire1Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_axis_list_2(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire2Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_button_list_1(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire1Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    SendDlgItemMessage(hwnd, IDC_JOY_AUTOFIRE1_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire1Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_button_list_2(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire2Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    SendDlgItemMessage(hwnd, IDC_JOY_AUTOFIRE2_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire2Button", &res_value);
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
#ifdef HAVE_DINPUT
          case IDC_JOY_CALIBRATE:
            joystick_calibrate(hwnd);
            return TRUE;
#endif
          case IDC_JOY_CONFIG_A:
            current_keyset_index = 0;
            DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_CONFIG_B:
            current_keyset_index = 1;
            DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_DEV1:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_int("JoyDevice1",
                                  (int)SendMessage(GetDlgItem(hwnd,
                                  IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_DEV1,
                                               CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_1(hwnd, res_value);
                    rebuild_button_list_1(hwnd, res_value);
                }
                axis = SendDlgItemMessage(hwnd,IDC_JOY_FIRE1_AXIS,CB_GETCURSEL,0,0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                             (res_value >= JOYDEV_HW1) && (axis == 0));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON),
                             (res_value >= JOYDEV_HW1));
            }
            return TRUE;
          case IDC_JOY_DEV2:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_int("JoyDevice2",
                                  (int)SendMessage(GetDlgItem(hwnd,
                                  IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
                res_value = SendDlgItemMessage(hwnd, IDC_JOY_DEV2,
                                               CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_2(hwnd, res_value);
                    rebuild_button_list_2(hwnd, res_value);
                }
                axis = SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                             (res_value >= JOYDEV_HW1) && (axis == 0));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                             (res_value >= JOYDEV_HW1));
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON),
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
            resources_set_int("JoyDevice1",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyDevice2",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyFire1Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE1_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire1Speed",
                              (int)GetDlgItemInt(hwnd,
                              IDC_JOY_FIRE1_SPEED, NULL, FALSE));
            resources_set_int("JoyAutofire1Axis",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE1_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire1Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_AUTOFIRE1_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyFire2Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE2_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire2Speed",
                              (int)GetDlgItemInt(hwnd,
                              IDC_JOY_FIRE2_SPEED, NULL, FALSE));
            resources_set_int("JoyAutofire2Axis",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE2_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire2Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_AUTOFIRE2_BUTTON), CB_GETCURSEL, 0, 0));
            EndDialog(hwnd,0);
            return TRUE;
          case IDCANCEL:
            resources_set_int("JoyDevice1", joy1);
            resources_set_int("JoyDevice2", joy2);
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ui_joystick_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_JOY_SETTINGS_DIALOG),
              hwnd,dialog_proc);
}

void ui_joystick_swap_joystick(void)
{
    int device1;
    int device2;

    resources_get_int("JoyDevice1", &device1);
    resources_get_int("JoyDevice2", &device2);
    resources_set_int("JoyDevice1", device2);
    resources_set_int("JoyDevice2", device1);
    resources_get_int("JoyFire1Button", &device1);
    resources_get_int("JoyFire2Button", &device2);
    resources_set_int("JoyFire1Button", device2);
    resources_set_int("JoyFire2Button", device1);
    resources_get_int("JoyAutofire1Speed", &device1);
    resources_get_int("JoyAutofire2Speed", &device2);
    resources_set_int("JoyAutofire1Speed", device2);
    resources_set_int("JoyAutofire2Speed", device1);
    resources_get_int("JoyAutofire1Axis", &device1);
    resources_get_int("JoyAutofire2Axis", &device2);
    resources_set_int("JoyAutofire1Axis", device2);
    resources_set_int("JoyAutofire2Axis", device1);
    resources_get_int("JoyAutofire1Button", &device1);
    resources_get_int("JoyAutofire2Button", &device2);
    resources_set_int("JoyAutofire1Button", device2);
    resources_set_int("JoyAutofire2Button", device1);

/* swapping the keyset definitions when swapping the joysticks is
   improper bahaviour in my opinion, if needed a swap keyset can be
   made later, for now the keyset swapping is commented out. */

#if 0
    resources_get_int("KeySet1NorthWest", &device1);
    resources_get_int("KeySet2NorthWest", &device2);
    resources_set_int("KeySet1NorthWest", device2);
    resources_set_int("KeySet2NorthWest", device1);
    resources_get_int("KeySet1North", &device1);
    resources_get_int("KeySet2North", &device2);
    resources_set_int("KeySet1North", device2);
    resources_set_int("KeySet2North", device1);
    resources_get_int("KeySet1NorthEast", device1);
    resources_get_int("KeySet2NorthEast", device2);
    resources_set_int("KeySet1NorthEast", device2);
    resources_set_int("KeySet2NorthEast", device1);
    resources_get_int("KeySet1East", &device1);
    resources_get_int("KeySet2East", &device2);
    resources_set_int("KeySet1East", device2);
    resources_set_int("KeySet2East", device1);
    resources_get_int("KeySet1SouthEast", &device1);
    resources_get_int("KeySet2SouthEast", &device2);
    resources_set_int("KeySet1SouthEast", device2);
    resources_set_int("KeySet2SouthEast", device1);
    resources_get_int("KeySet1South", &device1);
    resources_get_int("KeySet2South", &device2);
    resources_set_int("KeySet1South", device2);
    resources_set_int("KeySet2South", device1);
    resources_get_int("KeySet1SouthWest", &device1);
    resources_get_int("KeySet2SouthWest", &device2);
    resources_set_int("KeySet1SouthWest", device2);
    resources_set_int("KeySet2SouthWest", device1);
    resources_get_int("KeySet1West", &device1);
    resources_get_int("KeySet2West", &device2);
    resources_set_int("KeySet1West", device2);
    resources_set_int("KeySet2West", device1);
    resources_get_int("KeySet1Fire", &device1);
    resources_get_int("KeySet2Fire", &device2);
    resources_set_int("KeySet1Fire", device2);
    resources_set_int("KeySet2Fire", device1);
#endif
}
