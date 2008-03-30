/*
 * keyboard.h - Common keyboard emulation.
 *
 * Written by
 *  
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/* Maximum of keyboard array (CBM-II values
 * (8 for C64/VIC20, 10 for PET, 11 for C128; we need max).  */
#define KBD_ROWS    16

/* (This is actually the same for all the machines.) */
/* (All have 8, except CBM-II that has 6) */
#define KBD_COLS    8

extern void keyboard_init(void);
extern void keyboard_set_keyarr(int row, int col, int value);
extern void keyboard_set_keyarr_and_latch(int row, int col, int value);
extern void keyboard_clear_keymatrix(void);

enum joystick_bits_s {
    CBM_NORTH = 0x1,
    CBM_SOUTH = 0x2,
    CBM_WEST  = 0x4,
    CBM_EAST  = 0x8,
    CBM_FIRE  = 0x10
};
typedef enum joystick_bits_s joystick_bits_t;

extern void joystick_set_value_absolute(unsigned int joyport, BYTE value);
extern void joystick_set_value_or(unsigned int joyport, BYTE value);
extern void joystick_set_value_and(unsigned int joyport, BYTE value);
extern void joystick_clear(unsigned int joyport);

extern int keyboard_set_keymap_index(void *v, void *param);
extern int keyboard_set_keymap_file(void *v, void *param);

/* This ugly externs will go away sooner or later.  */
extern int keyarr[KBD_ROWS];
extern int rev_keyarr[KBD_COLS];
extern BYTE joystick_value[3];

extern int c64_kbd_init(void);
extern int c128_kbd_init(void);
extern int vic20_kbd_init(void);
extern int pet_kbd_init(void);
extern int plus4_kbd_init(void);
extern int c610_kbd_init(void);

extern int kbd_cmdline_options_init(void);
extern int kbd_resources_init(void);

extern int pet_kbd_cmdline_options_init(void);
extern int pet_kbd_resources_init(void);

typedef void (*key_ctrl_column4080_func_t)(void);
extern void kbd_register_column4080_key(key_ctrl_column4080_func_t func);

typedef void (*key_ctrl_caps_func_t)(void);
extern void kbd_register_caps_key(key_ctrl_caps_func_t func);

#endif

