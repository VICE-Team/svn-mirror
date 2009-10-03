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
#include "joystick.h"
#include "kbd.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "translate.h"
#include "uilib.h"
#include "winjoy.h"
#include "winkbd.h"
#include "winlong.h"
#include "winmain.h"


static int joy1;
static int joy2;
static int joy3;
static int joy4;
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

static INT_PTR CALLBACK real_callback(HWND hwnd, UINT msg, WPARAM wparam,
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
        kcode = (int)((lparam >> 16) & 0xff);

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

static INT_PTR CALLBACK key_dialog(HWND hwnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        SetWindowText(hwnd, translate_text(keydefine_texts[current_key_index]));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (INT_PTR)real_callback);
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

static INT_PTR CALLBACK keyset_dialog(HWND hwnd, UINT msg, WPARAM wparam,
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

static uilib_localize_dialog_param joystick_dialog[] = {
    {0, IDS_JOYSTICK_CAPTION, -1},
    {IDC_JOYSTICK_IN_PORT_1, IDS_JOYSTICK_IN_PORT_1, 0},
    {IDC_SELECT_FIRE_BUTTON_1, IDS_SELECT_FIRE_BUTTON, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_1, IDS_AUTO_FIRE_BUTTON_SETTINGS, 0},
    {IDC_AUTOFIRE_SPEED_1, IDS_AUTOFIRE_SPEED, 0},
    {IDC_JOYSTICK_IN_PORT_2, IDS_JOYSTICK_IN_PORT_2, 0},
    {IDC_SELECT_FIRE_BUTTON_2, IDS_SELECT_FIRE_BUTTON, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_2, IDS_AUTO_FIRE_BUTTON_SETTINGS, 0},
    {IDC_AUTOFIRE_SPEED_2, IDS_AUTOFIRE_SPEED, 0},
    {IDC_JOY_CONFIG_A, IDS_JOY_CONFIG_A, 0},
    {IDC_JOY_CONFIG_B, IDS_JOY_CONFIG_B, 0},
    {IDC_JOY_CALIBRATE, IDS_JOY_CALIBRATE, 0},
    {IDOK, IDS_OK, 0},
    {IDCANCEL, IDS_CANCEL, 0},
    {0, 0, 0}
};

static uilib_dialog_group joystick_left_group[] = {
    {IDC_JOYSTICK_IN_PORT_1, 0},
    {IDC_SELECT_FIRE_BUTTON_1, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_1, 0},
    {IDC_AUTOFIRE_SPEED_1, 0},
    {0, 0}
};

static uilib_dialog_group joystick_left_size_group[] = {
    {IDC_JOYSTICK_IN_PORT_1, 0},
    {IDC_JOY_DEV1, 0},
    {IDC_SELECT_FIRE_BUTTON_1, 0},
    {IDC_JOY_FIRE1_BUTTON, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_1, 0},
    {IDC_JOY_FIRE1_SPEED, 0},
    {IDC_JOY_FIRE1_AXIS, 0},
    {IDC_JOY_AUTOFIRE1_BUTTON, 0},
    {0, 0}
};

static uilib_dialog_group joystick_right_move_group[] = {
    {IDC_JOY_DEV2, 0},
    {IDC_SELECT_FIRE_BUTTON_2, 0},
    {IDC_JOY_FIRE2_BUTTON, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_2, 0},
    {IDC_AUTOFIRE_SPEED_2, 0},
    {IDC_JOY_FIRE2_AXIS, 0},
    {IDC_JOY_AUTOFIRE2_BUTTON, 0},
    {0, 0}
};

static uilib_dialog_group joystick_right_group[] = {
    {IDC_JOYSTICK_IN_PORT_2, 0},
    {IDC_SELECT_FIRE_BUTTON_2, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_2, 0},
    {IDC_AUTOFIRE_SPEED_2, 0},
    {0, 0}
};

static uilib_dialog_group joystick_right_size_group[] = {
    {IDC_JOYSTICK_IN_PORT_2, 0},
    {IDC_JOY_DEV2, 0},
    {IDC_SELECT_FIRE_BUTTON_2, 0},
    {IDC_JOY_FIRE2_BUTTON, 0},
    {IDC_AUTO_FIRE_BUTTON_SETTINGS_2, 0},
    {IDC_JOY_FIRE2_SPEED, 0},
    {IDC_JOY_FIRE2_AXIS, 0},
    {IDC_JOY_AUTOFIRE2_BUTTON, 0},
    {0, 0}
};

static uilib_dialog_group joystick_button_group[] = {
    {IDC_JOY_CONFIG_A, 1},
    {IDC_JOY_CONFIG_B, 1},
    {IDC_JOY_CALIBRATE, 1},
    {0, 0}
};

static void init_joystick_dialog(HWND hwnd)
{
    HWND joy_hwnd;
    int res_value;
    int device;
    int xpos;
    int xstart;
    int xpos1, xpos2, xpos3;
    int distance1, distance2;
    RECT rect;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, joystick_dialog);

    /* adjust the size of the left group elements */
    uilib_adjust_group_width(hwnd, joystick_left_group);

    /* get the max x of the autofire speed 1 settings element */
    uilib_get_element_max_x(hwnd, IDC_AUTOFIRE_SPEED_1, &xpos);

    /* move the fire 1 speed item to the correct position */
    uilib_move_element(hwnd, IDC_JOY_FIRE1_SPEED, xpos + 10);

    /* get the max x of the left size group elements */
    uilib_get_group_max_x(hwnd, joystick_left_size_group, &xpos);

    /* get the min x of the joystick in port 1 element */
    uilib_get_element_min_x(hwnd, IDC_JOYSTICK_IN_PORT_1, &xstart);

    /* set the size of the joystick in port 1 element */
    uilib_set_element_width(hwnd, IDC_JOYSTICK_IN_PORT_1, xpos - xstart + 10);

    /* set the position of the joystick in port 2 element */
    uilib_move_element(hwnd, IDC_JOYSTICK_IN_PORT_2, xpos + 10 + 15);

    /* set the position of the right group move elements */
    uilib_move_group(hwnd, joystick_right_move_group, xpos + 10 + 20);

    /* adjust the size of the right group elements */
    uilib_adjust_group_width(hwnd, joystick_right_group);

    /* get the max x of the autofire speed 2 settings element */
    uilib_get_element_max_x(hwnd, IDC_AUTOFIRE_SPEED_2, &xpos);

    /* move the fire 2 speed item to the correct position */
    uilib_move_element(hwnd, IDC_JOY_FIRE2_SPEED, xpos + 10);

    /* get the max x of the right size group elements */
    uilib_get_group_max_x(hwnd, joystick_right_size_group, &xpos);

    /* get the min x of the joystick in port 2 element */
    uilib_get_element_min_x(hwnd, IDC_JOYSTICK_IN_PORT_2, &xstart);

    /* set the size of the joystick in port 2 element */
    uilib_set_element_width(hwnd, IDC_JOYSTICK_IN_PORT_2, xpos - xstart + 10);

    /* get the max x of the config keyset a button */
    uilib_get_element_max_x(hwnd, IDC_JOY_CONFIG_A, &xpos1);

    /* get the min x of the joy calibrate button */
    uilib_get_element_min_x(hwnd, IDC_JOY_CALIBRATE, &xpos2);

    /* get the min x of the config keyset b button */
    uilib_get_element_min_x(hwnd, IDC_JOY_CONFIG_B, &xpos3);

    /* calculate distance between config keyset a button and joy calibrate button */
    distance1 = xpos2 - xpos1;

    /* get the max x of the joy calibrate button */
    uilib_get_element_min_x(hwnd, IDC_JOY_CALIBRATE, &xpos2);

    /* calculate distance between config keyset b button and joy calibrate button */
    distance2 = xpos3 - xpos2;

    /* adjust the size of the button group */
    uilib_adjust_group_width(hwnd, joystick_button_group);

    /* get the max x of the config keyset a button */
    uilib_get_element_max_x(hwnd, IDC_JOY_CONFIG_A, &xpos);
    
    /* move the joy calibrate button to the correct location */
    uilib_move_element(hwnd, IDC_JOY_CALIBRATE, xpos + distance1);

    /* get the max x of the joy calibrate botton */
    uilib_get_element_max_x(hwnd, IDC_JOY_CALIBRATE, &xpos);
    
    /* move the config keyset b button to the correct location */
    uilib_move_element(hwnd, IDC_JOY_CONFIG_B, xpos + distance1);

    /* get the max x of the joystick in port 2 element */
    uilib_get_element_max_x(hwnd, IDC_JOYSTICK_IN_PORT_2, &xpos1);

    /* get the max_x of the config keyset b button */
    uilib_get_element_max_x(hwnd, IDC_JOY_CONFIG_B, &xpos2);

    if (xpos2 > xpos1) {
        xpos = xpos2;
    } else {
        xpos = xpos1;
    }

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xpos + 10, rect.bottom - rect.top, TRUE);

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

    if (machine_class == VICE_MACHINE_VIC20) {
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_DEV2), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON), 0);
    } else {
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                              (device >= JOYDEV_HW1));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                              (device >= JOYDEV_HW1) && (res_value == 0));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                              (device >= JOYDEV_HW1));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON),
                              (device >= JOYDEV_HW1));
    }
    EnableWindow(GetDlgItem(hwnd, IDC_JOY_CALIBRATE), joystick_uses_direct_input());
}

static void enable_userport_joystick_controls(HWND hwnd, int joyamount)
{
    int device;
    int res_value;

    resources_get_int("JoyDevice3", &device);
    resources_get_int("JoyAutofire3Button", &res_value);

    if (joyamount == 0) {
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_DEV1), 0);
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON), 0);
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED), 0);
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS), 0);
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON), 0);
    } else {
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_DEV1), 1);
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON),
                                (device >= JOYDEV_HW1));
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                                (device >= JOYDEV_HW1) && (res_value == 0));
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS),
                                (device >= JOYDEV_HW1));
        EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON),
                                (device >= JOYDEV_HW1));
    }

    resources_get_int("JoyDevice4", &device);
    resources_get_int("JoyAutofire4Button", &res_value);

    if (joyamount < 2) {
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_DEV2), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS), 0);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON), 0);
    } else {
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_DEV2), 1);
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON),
                              (device >= JOYDEV_HW1));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_SPEED),
                              (device >= JOYDEV_HW1) && (res_value == 0));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS),
                              (device >= JOYDEV_HW1));
      EnableWindow(GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON),
                              (device >= JOYDEV_HW1));
    }
}

static void init_extra_joystick_dialog(HWND hwnd)
{
    HWND joy_hwnd;
    int res_value;
    int device;
    int joyamount;

    joy_hwnd = GetDlgItem(hwnd, IDC_EXTRA_JOY_ADAPTER);
    if (machine_class == VICE_MACHINE_PLUS4) {
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_NO_SIDCART_JOY));
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_SIDCART_JOY));
        resources_get_int("SIDCartJoy", &res_value);
        SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
        joyamount = res_value;
    } else {
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_NO_USERPORT_ADAPTER));
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_CGA_USERPORT_ADAPTER));
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_PET_USERPORT_ADAPTER));
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_HUMMER_USERPORT_ADAPTER));
        SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)translate_text(IDS_OEM_USERPORT_ADAPTER));
        if (machine_class == VICE_MACHINE_C64 || machine_class == VICE_MACHINE_C128) {
            SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                       (LPARAM)translate_text(IDS_HIT_USERPORT_ADAPTER));
        }
        resources_get_int("ExtraJoy", &res_value);
        if (res_value == 0) {
            SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
            joyamount = 0;
        } else {
            resources_get_int("ExtraJoyType", &res_value);
            if (res_value == EXTRA_JOYSTICK_HUMMER || res_value == EXTRA_JOYSTICK_OEM) {
                joyamount = 1;
            } else {
                joyamount = 2;
            }
            res_value++;
            SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
        }
    }

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
    resources_get_int("JoyDevice3", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy3 = device = res_value;

    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire3Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_int("JoyAutofire3Speed", &res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire3Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire3Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

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
    resources_get_int("JoyDevice4", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value,0);
    joy4 = device = res_value;

    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire4Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_int("JoyAutofire4Speed", &res_value);
    SetDlgItemInt(hwnd, IDC_JOY_FIRE2_SPEED, res_value, FALSE);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire4Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire4Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    enable_userport_joystick_controls(hwnd, joyamount);
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

static void rebuild_axis_list_3(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire3Axis", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_axis_list_4(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_AXIS);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NUMERIC_SEE_ABOVE));
    joystick_ui_get_autofire_axes(joy_hwnd, device);
    resources_get_int("JoyAutofire4Axis", &res_value);
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

static void rebuild_button_list_3(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire3Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    SendDlgItemMessage(hwnd, IDC_JOY_AUTOFIRE1_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE1_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire3Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void rebuild_button_list_4(HWND hwnd, int device)
{
    HWND joy_hwnd;
    int res_value;

    SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_FIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_ALL_BUTTONS_AS_FIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyFire4Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    SendDlgItemMessage(hwnd, IDC_JOY_AUTOFIRE2_BUTTON, CB_RESETCONTENT, 0, 0);
    joy_hwnd = GetDlgItem(hwnd, IDC_JOY_AUTOFIRE2_BUTTON);
    SendMessage(joy_hwnd, CB_ADDSTRING, 0,
                (LPARAM)translate_text(IDS_NO_BUTTON_NO_AUTOFIRE));
    joystick_ui_get_autofire_buttons(joy_hwnd, device);
    resources_get_int("JoyAutofire4Button", &res_value);
    SendMessage(joy_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
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
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_CONFIG_B:
            current_keyset_index = 1;
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_DEV1:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_int("JoyDevice1",
                                  (int)SendMessage(GetDlgItem(hwnd,
                                  IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_DEV1,
                                                    CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_1(hwnd, res_value);
                    rebuild_button_list_1(hwnd, res_value);
                }
                axis = (int)SendDlgItemMessage(hwnd,IDC_JOY_FIRE1_AXIS,CB_GETCURSEL,0,0);
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
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_DEV2,
                                                    CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_2(hwnd, res_value);
                    rebuild_button_list_2(hwnd, res_value);
                }
                axis = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_GETCURSEL, 0, 0);
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
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS,
                                                    CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                             (res_value == 0));
            }
            return TRUE;
          case IDC_JOY_FIRE1_SPEED:
            if (HIWORD(wparam) == EN_KILLFOCUS) {
                res_value = (int)GetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED,NULL,
                                               FALSE);
                if (res_value > 32)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 32, FALSE);
                if (res_value < 1)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 1, FALSE);
            }
            return TRUE;
          case IDC_JOY_FIRE2_AXIS:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS,
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

static INT_PTR CALLBACK dialog_proc_2(HWND hwnd, UINT msg, WPARAM wparam,
                                      LPARAM lparam)
{
    int command;
    int res_value;
    int axis;
    int joyamount;

     switch (msg) {
      case WM_INITDIALOG:
        init_extra_joystick_dialog(hwnd);
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
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_JOY_CONFIG_B:
            current_keyset_index = 1;
            DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_CONFIG_KEYSET_DIALOG),
                      hwnd, keyset_dialog);
            return TRUE;
          case IDC_EXTRA_JOY_ADAPTER:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                switch ((int)SendMessage(GetDlgItem(hwnd, IDC_EXTRA_JOY_ADAPTER), CB_GETCURSEL, 0, 0)) {
                    case 0:
                    default:
                        joyamount = 0;
                        break;
                    case EXTRA_JOYSTICK_CGA+1:
                    case EXTRA_JOYSTICK_PET+1:
                    case EXTRA_JOYSTICK_HIT+1:
                        if (machine_class == VICE_MACHINE_PLUS4) {
                            joyamount = 1;
                        } else {
                            joyamount = 2;
                        }
                        break;
                    case EXTRA_JOYSTICK_HUMMER+1:
                    case EXTRA_JOYSTICK_OEM+1:
                        joyamount = 1;
                        break;
                }
                enable_userport_joystick_controls(hwnd, joyamount);
            }
            break;
          case IDC_JOY_DEV1:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                resources_set_int("JoyDevice3",
                                  (int)SendMessage(GetDlgItem(hwnd,
                                  IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_DEV1,
                                                    CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_3(hwnd, res_value);
                    rebuild_button_list_3(hwnd, res_value);
                }
                axis = (int)SendDlgItemMessage(hwnd,IDC_JOY_FIRE1_AXIS,CB_GETCURSEL,0,0);
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
                resources_set_int("JoyDevice4",
                                  (int)SendMessage(GetDlgItem(hwnd,
                                  IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_DEV2,
                                                    CB_GETCURSEL, 0, 0);
                if (res_value >= JOYDEV_HW1) {
                    rebuild_axis_list_4(hwnd, res_value);
                    rebuild_button_list_4(hwnd, res_value);
                }
                axis = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS, CB_GETCURSEL, 0, 0);
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
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE1_AXIS,
                                                    CB_GETCURSEL, 0, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_JOY_FIRE1_SPEED),
                             (res_value == 0));
            }
            return TRUE;
          case IDC_JOY_FIRE1_SPEED:
            if (HIWORD(wparam) == EN_KILLFOCUS) {
                res_value = (int)GetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED,NULL,
                                               FALSE);
                if (res_value > 32)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 32, FALSE);
                if (res_value < 1)
                    SetDlgItemInt(hwnd, IDC_JOY_FIRE1_SPEED, 1, FALSE);
            }
            return TRUE;
          case IDC_JOY_FIRE2_AXIS:
            if (HIWORD(wparam) == CBN_SELCHANGE) {
                res_value = (int)SendDlgItemMessage(hwnd, IDC_JOY_FIRE2_AXIS,
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
            res_value = (int)SendMessage(GetDlgItem(hwnd, IDC_EXTRA_JOY_ADAPTER), CB_GETCURSEL, 0, 0);
            if (machine_class == VICE_MACHINE_PLUS4) {
                resources_set_int("SIDCartJoy", res_value);
            } else {
                if (res_value == 0) {
                    resources_set_int("ExtraJoy", 0);
                } else {
                    res_value--;
                    resources_set_int("ExtraJoy", 1);
                    resources_set_int("ExtraJoyType", res_value);
                }
            }
            resources_set_int("JoyDevice3",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_DEV1), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyDevice4",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_DEV2), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyFire3Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE1_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire3Speed",
                              (int)GetDlgItemInt(hwnd,
                              IDC_JOY_FIRE1_SPEED, NULL, FALSE));
            resources_set_int("JoyAutofire3Axis",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE1_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire3Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_AUTOFIRE1_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyFire4Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE2_BUTTON), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire4Speed",
                              (int)GetDlgItemInt(hwnd,
                              IDC_JOY_FIRE2_SPEED, NULL, FALSE));
            resources_set_int("JoyAutofire4Axis",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_FIRE2_AXIS), CB_GETCURSEL, 0, 0));
            resources_set_int("JoyAutofire4Button",
                              (int)SendMessage(GetDlgItem(hwnd,
                              IDC_JOY_AUTOFIRE2_BUTTON), CB_GETCURSEL, 0, 0));
            EndDialog(hwnd,0);
            return TRUE;
          case IDCANCEL:
            resources_set_int("JoyDevice3", joy3);
            resources_set_int("JoyDevice4", joy4);
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ui_joystick_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_JOY_SETTINGS_DIALOG,
              hwnd,dialog_proc);
}

void ui_extra_joystick_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_EXTRA_JOY_SETTINGS_DIALOG),
              hwnd,dialog_proc_2);
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
}

void ui_joystick_swap_extra_joystick(void)
{
    int device3;
    int device4;

    resources_get_int("JoyDevice3", &device3);
    resources_get_int("JoyDevice4", &device4);
    resources_set_int("JoyDevice3", device4);
    resources_set_int("JoyDevice4", device3);
    resources_get_int("JoyFire3Button", &device3);
    resources_get_int("JoyFire4Button", &device4);
    resources_set_int("JoyFire3Button", device4);
    resources_set_int("JoyFire4Button", device3);
    resources_get_int("JoyAutofire3Speed", &device3);
    resources_get_int("JoyAutofire4Speed", &device4);
    resources_set_int("JoyAutofire3Speed", device4);
    resources_set_int("JoyAutofire4Speed", device3);
    resources_get_int("JoyAutofire3Axis", &device3);
    resources_get_int("JoyAutofire4Axis", &device4);
    resources_set_int("JoyAutofire3Axis", device4);
    resources_set_int("JoyAutofire4Axis", device3);
    resources_get_int("JoyAutofire3Button", &device3);
    resources_get_int("JoyAutofire4Button", &device4);
    resources_set_int("JoyAutofire3Button", device4);
    resources_set_int("JoyAutofire4Button", device3);
}
