/*
 * kbd.c - Keyboard emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Tibor Biczo (crown@mail.matav.hu)
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

#include "kbd.h"
#include "cmdline.h"
#include "resources.h"

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

BYTE _kbd_extended_key_tab[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, K_KPENTER, K_RIGHTCTRL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, K_HOME, K_UP, K_PGUP, 0, K_LEFT, 0, K_RIGHT, 0, K_END,
    K_DOWN, K_PGDOWN, K_INS, K_DEL, 0, 0, 0, 0, 0, 0, 0, K_LEFTW95,
    K_RIGHTW95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];
BYTE joy[3];

struct _convmap {
    /* Conversion map.  */
    keyconv *map;
    /* Location of the virtual shift key in the keyboard matrix.  */
    int virtual_shift_row, virtual_shift_column;
};

static struct _convmap *keyconvmaps;
static struct _convmap *keyconv_base;
static int num_keyconvmaps;

static int keymap_index;

/* FIXME: Ugly hack.  */
//extern keyconv c64_keyboard[];
//keyconv *keyconvmap = c64_keyboard;

/* What is the location of the virtual shift key in the keyboard matrix?  */
//static int virtual_shift_column = 7;
//static int virtual_shift_row = 1;

/* ------------------------------------------------------------------------ */

int kbd_init(int num, ...)
{
    DEBUG(("Allocating keymaps"));
    keyconvmaps=(struct _convmap*)malloc(num*sizeof(struct _convmap));
    DEBUG(("Installing keymaps"));
    {
        va_list p;
        int i;

        num_keyconvmaps=num;

        va_start(p,num);
        for (i =0; i<num_keyconvmaps; i++) {
            keyconv *map;
            unsigned int sizeof_map;
            int shift_row,shift_column;

            shift_row=va_arg(p,int);
            shift_column=va_arg(p,int);
            map=va_arg(p,keyconv*);
            sizeof_map=va_arg(p,unsigned int);

            keyconvmaps[i].map = map;
            keyconvmaps[i].virtual_shift_row = shift_row;
            keyconvmaps[i].virtual_shift_column = shift_column;
        }
    }
    keyconv_base=&keyconvmaps[keymap_index>>1];
    return 0;
}

static int set_keymap_index(resource_value_t v)
{
    keymap_index=(int)v;

    return 0;
}

static resource_t resources[] = {
    { "KeymapIndex", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &keymap_index, set_keymap_index },
    { NULL }
};

int kbd_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", "Specify index of used keymap" },
    { NULL },
};

int kbd_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
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

    /*  Translate Extended scancodes */
    if (key_data & (1<<24)) {
        kcode=_kbd_extended_key_tab[kcode];
    }

    DEBUG(("Keydown, code %d (0x%02x)\n", kcode, kcode));
    if (!joystick_handle_key(kcode, 1)) {
        set_keyarr(keyconv_base->map[kcode].row,
                   keyconv_base->map[kcode].column, 1);
        if (keyconv_base->map[kcode].vshift)
            set_keyarr(keyconv_base->virtual_shift_row, keyconv_base->virtual_shift_column, 1);
    }

    return 0;
}

int kbd_handle_keyup(DWORD virtual_key, DWORD key_data)
{
    int kcode = (key_data >> 16) & 0xff;

    /*  Translate Extended scancodes */
    if (key_data & (1<<24)) {
        kcode=_kbd_extended_key_tab[kcode];
    }

    if (!joystick_handle_key(kcode, 0)) {
        set_keyarr(keyconv_base->map[kcode].row, keyconv_base->map[kcode].column, 0);
        if (keyconv_base->map[kcode].vshift)
            set_keyarr(keyconv_base->virtual_shift_row, keyconv_base->virtual_shift_column, 0);
    }

    return 0;
}
