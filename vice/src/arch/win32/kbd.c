/*
 * kbd.c - Keyboard emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "kbd.h"

/* ------------------------------------------------------------------------ */

/* #define DEBUG_KBD */

/* Debugging stuff.  */
#ifdef DEBUG_KBD
static void kbd_debug(const char *format, ...)
{
	char tmp[1024];
	va_list args;

	va_start(args, format);
	vsprintf(tmp, format, args);
	va_end(args);
	OutputDebugString(tmp);
}
#define DEBUG(x) kbd_debug x
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------ */

int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];
BYTE joy[3];

/* FIXME: Ugly hack.  */
extern keyconv c64_keyboard[];
keyconv *keyconvmap = c64_keyboard;

/* What is the location of the virtual shift key in the keyboard matrix?  */
static int virtual_shift_column = 7;
static int virtual_shift_row = 1;

/* ------------------------------------------------------------------------ */

int kbd_init(void)
{
    return 0;
}

int kbd_init_resources(void)
{
    return 0;
}

int kbd_init_cmdline_options(void)
{
    return 0;
}

/* ------------------------------------------------------------------------ */

/* Joystick-through-keyboard.  */

static int joystick_handle_key(int kcode, int pressed)
{
    return 0;
}

/* ------------------------------------------------------------------------ */

inline static void set_keyarr(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
	keyarr[row] |= 1 << col;
	rev_keyarr[col] |= 1 << row;
    } else {
	keyarr[row] &= ~(1 << col);
	rev_keyarr[col] &= ~(1 << row);
    }
}

/* Windows would not want us to handle raw scancodes like this...  But we
   need it nevertheless.  */

int kbd_handle_keydown(DWORD virtual_key, DWORD key_data)
{
    int kcode = (key_data >> 16) & 0xff;

    DEBUG(("Keydown, code %d (0x%02x)\n", kcode, kcode));
    if (!joystick_handle_key(kcode, 1)) {
        set_keyarr(keyconvmap[kcode].row,
                   keyconvmap[kcode].column, 1);
        if (keyconvmap[kcode].vshift)
            set_keyarr(virtual_shift_row, virtual_shift_column, 1);
    }

    return 0;
}

int kbd_handle_keyup(DWORD virtual_key, DWORD key_data)
{
    int kcode = (key_data >> 16) & 0xff;

    if (!joystick_handle_key(kcode, 0)) {
        set_keyarr(keyconvmap[kcode].row, keyconvmap[kcode].column, 0);
        if (keyconvmap[kcode].vshift)
            set_keyarr(virtual_shift_row, virtual_shift_column, 0);
    }

    return 0;
}

