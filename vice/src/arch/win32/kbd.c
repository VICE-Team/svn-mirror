/*
 * kbd.c - Keyboard emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kbd.h"
#include "keyboard.h"
#include "joy.h"
#include "types.h"


BYTE _kbd_extended_key_tab[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, K_KPENTER, K_RIGHTCTRL, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, K_KPDIV, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, K_NUMLOCK, 0, K_HOME, K_UP, K_PGUP, 0, K_LEFT, 0, K_RIGHT, 0, K_END,
    K_DOWN, K_PGDOWN, K_INS, K_DEL, 0, 0, 0, 0, 0, 0, 0, K_LEFTW95, K_RIGHTW95, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* ------------------------------------------------------------------------ */

/* Windows would not want us to handle raw scancodes like this...  But we
   need it nevertheless.  */

int kbd_handle_keydown(DWORD virtual_key, DWORD key_data)
{
    int kcode = (key_data >> 16) & 0xff;

    if (virtual_key == 0xff)
        return 0;

    /*  Translate Extended scancodes */
    if (key_data & (1 << 24)) {
        kcode = _kbd_extended_key_tab[kcode];
    }

    if (!joystick_handle_key(kcode, 1))
        keyboard_key_pressed((signed long)kcode);

    return 0;
}

int kbd_handle_keyup(DWORD virtual_key, DWORD key_data)
{
    int kcode = (key_data >> 16) & 0xff;

    if (virtual_key == 0xff)
        return 0;

    /*  Translate Extended scancodes */
    if (key_data & (1 << 24)) {
        kcode = _kbd_extended_key_tab[kcode];
    }

    if (!joystick_handle_key(kcode, 0))
        keyboard_key_released((signed long)kcode);

    return 0;
}

const char *kbd_code_to_string(kbd_code_t kcode)
{
    static char *tab[256] = {
        "None", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-",
        "=", "Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O",
        "P", "{", "}", "Enter", "Left Ctrl", "A", "S", "D", "F", "G", "H", "J",
        "K", "L", ";", "'", "`", "Left Shift", "\\", "Z", "X", "C", "V", "B",
        "N", "M", ",", ".", "/", "Right Shift", "Numpad *", "Left Alt",
        "Space", "Caps Lock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
        "F9", "F10", "Num Lock", "Scroll Lock", "Numpad 7", "Numpad 8",
        "Numpad 9", "Numpad -", "Numpad 4", "Numpad 5", "Numpad 6",
        "Numpad +", "Numpad 1", "Numpad 2", "Numpad 3", "Numpad 0",
        "Numpad .", "SysReq", "85", "86", "F11", "F12", "Home",
        "Up", "PgUp", "Left", "Right", "End", "Down", "PgDown", "Ins", "Del",
        "Numpad Enter", "Right Ctrl", "Pause", "PrtScr", "Numpad /",
        "Right Alt", "Break", "Left Win95", "Right Win95"
    };

    return tab[(int)kcode];
}

/* ------------------------------------------------------------------------ */

void kbd_arch_init(void)
{
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    return (signed long)atoi(keyname);
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    static char keyname[20];

    memset(keyname, 0, 20);

    sprintf(keyname, "%li", keynum);

    return keyname;
}

