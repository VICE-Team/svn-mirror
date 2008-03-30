/*
 * kbd.c - Keyboard emulation for BeOS.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <stdlib.h>

#include "kbd.h"
#include "cmdline.h"
#include "joystick.h"
#include "keyboard.h"
#include "resources.h"
#include "machine.h"
#include "types.h"
#include "utils.h"

/* ------------------------------------------------------------------------ */

/* #define DEBUG_KBD */

/* Debugging stuff.  */
#ifdef DEBUG_KBD
static void kbd_debug(const char *format, ...)
{
}
#define KBD_DEBUG(x) kbd_debug x
#else
#define KBD_DEBUG(x)
#endif

/* ------------------------------------------------------------------------ */

BYTE joystick_value[3];

/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;

/* CAPS key.  */
static key_ctrl_caps_func_t key_ctrl_caps_func = NULL;

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

/* ------------------------------------------------------------------------ */

int kbd_init(int num, ...)
{
    va_list p;
    int i;

    keyconvmaps=(struct _convmap*)xmalloc(num*sizeof(struct _convmap));
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
    keyconv_base=&keyconvmaps[keymap_index>>1];

    return 0;
}

static int set_keymap_index(resource_value_t v, void *param)
{
int real_index;

    keymap_index=(int)v;
    real_index=keymap_index>>1;
    keyconv_base=&keyconvmaps[real_index];

    return 0;
}

static resource_t resources[] = {
    { "KeymapIndex", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &keymap_index, set_keymap_index, NULL },
    { NULL }
};

int kbd_resources_init(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", "Specify index of used keymap" },
    { NULL },
};

int kbd_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------ */

int kbd_handle_keydown(int kcode)
{
    if (kcode == 8) /* F7 */ {
        if (key_ctrl_column4080_func != NULL) {
            key_ctrl_column4080_func();
        }
    }

    if (kcode == 5) /* F4 */ {
        if (key_ctrl_caps_func != NULL) {
            key_ctrl_caps_func();
        }
    }

    if (kcode == 33) /* PgUp */ {
        machine_set_restore_key(1);
    }

    if (!joystick_handle_key(kcode, 1)) {
        keyboard_set_keyarr(keyconv_base->map[kcode].row,
                   keyconv_base->map[kcode].column, 1);
        if (keyconv_base->map[kcode].vshift)
            keyboard_set_keyarr(keyconv_base->virtual_shift_row,
                                keyconv_base->virtual_shift_column, 1);
    }
    return 0;
}

int kbd_handle_keyup(int kcode)
{
    if (kcode == 33) /* PgUp */ {
        machine_set_restore_key(0);
    }

    if (!joystick_handle_key(kcode, 0)) {
        keyboard_set_keyarr(keyconv_base->map[kcode].row,
                            keyconv_base->map[kcode].column, 0);
        if (keyconv_base->map[kcode].vshift)
            keyboard_set_keyarr(keyconv_base->virtual_shift_row,
                                keyconv_base->virtual_shift_column, 0);
    }

	
    return 0;
}

const char *kbd_code_to_string(int kcode)
{
    static char *tab[256] = {
        "<None>", "Esc", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", 
        "F11", "F12", "PrtScr", "Scroll Lock", "16", "^", "1", "2", "3", "4",
        "5", "6", "7", "8", "9", "0", "-", "=", "Backspace", "Ins", "Home",
        "PgUp", "Num Lock", "Numpad /", "Numpad *", "Numpad -", "Tab", "Q", "W", "E", "R", 
        "T", "Y", "U", "I", "O", "P", "{", "}", "\\", "Del", "End",
        "PgDown", "Numpad 7", "Numpad 8", "Numpad 9", "Numpad +", "Caps Lock", "A", "S", "D", "F",
        "G", "H", "J", "K", "L", ";", "'", "Enter", "Numpad 4", "Numpad 5", "Numpad 6",
        "Left Shift", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", 
        "Right Shift", "Up", "Numpad 1", "Numpad 2", "Numpad 3", "Numpad Enter", "Left Ctrl", "Left ALt",
        "Space", "Right Alt", "Right Ctrl", "Left", "Down", "Right", "Numpad 0", "Numpad Dot",
        "Left Win95", "Right Win95", "Break"
    };

    return tab[(int) kcode];
}

/* ------------------------------------------------------------------------ */

void keyboard_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}

void keyboard_register_caps_key(key_ctrl_caps_func_t func)
{
    key_ctrl_caps_func = func;
}

