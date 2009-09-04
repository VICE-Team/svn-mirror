/*
 * proc.c - Keyboard processing
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_WININPUT     // VK_CAPSLOCK, KC_*, WinSetKeyboardStateTable
#include <os2.h>

#include "vice.h"

#include "log.h"
#include "parse.h"        // keyconvmap
#include "machine.h"      // machine_set_restore_key
#include "joy.h"          // joystick_handle_key
#include "keyboard.h"     // keyboard_set_keyarr
#include "resources.h"    // resource_value_t

#ifdef __X128__

/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;

/* CAPS key.  */
static key_ctrl_caps_func_t key_ctrl_caps_func = NULL;
#endif

// -----------------------------------------------------------------

inline void kbd_set_key(const CHAR code1, const CHAR code2, const USHORT release, const USHORT shift)
{
    //
    // Serach for the key in the list
    //
    int nr;

    for (nr = 0; nr < keyconvmap.entries; nr++) {
        if (keyconvmap.map[shift][nr].code == (code1 | code2 << 8)) {
            break;
        }
    }

    //
    // if the key wasn't found in the list don't do any action
    //
    if (nr == keyconvmap.entries) {
        if (!release) {
            log_message(LOG_DEFAULT, "proc.c: Unknown key %d %d pressed by user.", code1, code2);
        }
        return;
    }

    //
    // Pass pressed and mapped key to Vice
    //
    keyboard_set_keyarr(keyconvmap.map[shift][nr].row, keyconvmap.map[shift][nr].column, release);

    //
    // process virtual shift key
    //
    switch (keyconvmap.map[shift][nr].vshift) {
        case 0x1: // left shifted, press/release left shift
            keyboard_set_keyarr(keyconvmap.lshift_row, keyconvmap.lshift_col, release);
            break;
        case 0x2: // right shifted, press/release right shift
            keyboard_set_keyarr(keyconvmap.rshift_row, keyconvmap.rshift_col, release);
            break;
        case 0x3: // unshifted, release virtual shift keys
            keyboard_set_keyarr(keyconvmap.lshift_row, keyconvmap.lshift_col, 0);
            keyboard_set_keyarr(keyconvmap.rshift_row, keyconvmap.rshift_col, 0);
            break;
    }
}

void kbd_proc(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    USHORT fsFlags = SHORT1FROMMP(mp1);
    CHAR usScancode = CHAR4FROMMP(mp1);
    CHAR usKeycode = SHORT1FROMMP(mp2);
    CHAR usVK = SHORT2FROMMP(mp2);
    USHORT release = !(fsFlags & KC_KEYUP);

    //
    // ----- Process virtual keys -----
    //
    if (fsFlags & KC_VIRTUALKEY) {
        switch (usVK) {
#ifdef __X128__
            case VK_F10: // press/release caps key
                if (key_ctrl_caps_func) {
                    key_ctrl_caps_func();
                }
                return;
            case VK_F11: // press/release 40/80-key
                if (key_ctrl_column4080_func) {
                    key_ctrl_column4080_func();
                }
                return;
#endif
            case VK_F12:      // restore key pressed
                machine_set_restore_key(release);
                return;
            case VK_SCRLLOCK: // toggle warp mode if ScrlLock is pressed
                resources_set_int("WarpMode", (int)(fsFlags & KC_TOGGLE));
                return;
            case VK_CAPSLOCK:  // turn capslock led off if capslock is pressed
                {
                    /* This is not a beautiful way, but the one I know :-) */
                    BYTE keyState[256];

                    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
                    keyState[VK_CAPSLOCK] &= ~1;
                    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
                }
                break;
        }
    }

    //
    // ----- give all keypresses without Alt to Vice -----
    //
    if (!(fsFlags & KC_ALT)) {
        //
        // repeats are handled by vice itself
        //
        if (fsFlags & KC_PREVDOWN) {
            return;
        }

        //
        // check if the key is defined as joystick action
        //
        if (joystick_handle_key((kbd_code_t)usScancode, release)) {
            return;
        }

        //
        // this is a good canidate to be passed to vice
        //
        kbd_set_key(keyconvmap.symbolic ? usKeycode : usScancode, keyconvmap.symbolic ? usVK : 0, release, fsFlags & KC_SHIFT ? 1 : 0);
        return;
    }

    //
    // if key is released: return
    //
    if (release) {
        return;
    }
}

// ------------------------------------------------------------------

#ifdef __X128__
void keyboard_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}

void keyboard_register_caps_key(key_ctrl_caps_func_t func)
{
    key_ctrl_caps_func = func;
}
#endif
