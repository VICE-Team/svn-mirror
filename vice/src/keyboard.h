/*
 * keyboard.h - Common keyboard emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
 *  groepaz <groepaz@gmx.net>
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

#ifndef VICE_KEYBOARD_H
#define VICE_KEYBOARD_H

#include "types.h"

/* Maximum of keyboard array (CBM-II values
 * (8 for C64/VIC20, 10 for PET, 11 for C128; we need max).  */
#define KBD_ROWS    16

/* (This is actually the same for all the machines.) */
/* (All have 8, except CBM-II that has 6) */
#define KBD_COLS    8

/* negative rows/columns for extra keys */
#define KBD_ROW_JOY_KEYMAP_A    -1
#define KBD_ROW_JOY_KEYMAP_B    -2
#define KBD_ROW_RESTORE_1       -3
#define KBD_ROW_RESTORE_2       -3
#define KBD_ROW_4080COLUMN      -4
#define KBD_ROW_CAPSLOCK        -4
#define KBD_ROW_JOY_KEYPAD      -5

#define KBD_COL_RESTORE_1        0
#define KBD_COL_RESTORE_2        1
#define KBD_COL_4080COLUMN       0
#define KBD_COL_CAPSLOCK         1

/* joystick port attached keypad */
#define KBD_JOY_KEYPAD_ROWS      4
#define KBD_JOY_KEYPAD_COLS      5

#define KBD_JOY_KEYPAD_NUMKEYS   (KBD_JOY_KEYPAD_ROWS * KBD_JOY_KEYPAD_COLS)

#define KBD_MOD_LSHIFT      (1 << 0)
#define KBD_MOD_RSHIFT      (1 << 1)
#define KBD_MOD_LCTRL       (1 << 2)
#define KBD_MOD_RCTRL       (1 << 3)
#define KBD_MOD_LALT        (1 << 4)
#define KBD_MOD_RALT        (1 << 5)
#define KBD_MOD_SHIFTLOCK   (1 << 6)

struct snapshot_s;

extern void keyboard_init(void);
extern void keyboard_shutdown(void);

extern int keyboard_resources_init(void);
extern int keyboard_cmdline_options_init(void);

extern int keyboard_snapshot_write_module(struct snapshot_s *s);
extern int keyboard_snapshot_read_module(struct snapshot_s *s);

extern void keyboard_set_keyarr(int row, int col, int value);
extern void keyboard_set_keyarr_any(int row, int col, int value);

extern void keyboard_clear_keymatrix(void);

extern void keyboard_event_playback(CLOCK offset, void *data);
extern void keyboard_restore_event_playback(CLOCK offset, void *data);
extern void keyboard_event_delayed_playback(void *data);
extern void keyboard_register_delay(unsigned int delay);
extern void keyboard_register_clear(void);

/* called by the ui */
extern void keyboard_key_pressed(signed long key, int mod);
extern void keyboard_key_released(signed long key, int mod);
extern void keyboard_key_clear(void);
extern void keyboard_set_shiftlock(int state);
/* the emulation may also call this */
extern int keyboard_get_shiftlock(void);

typedef void (*key_ctrl_column4080_func_t)(void);
extern void keyboard_register_column4080_key(key_ctrl_column4080_func_t func);

typedef void (*key_ctrl_caps_func_t)(void);
extern void keyboard_register_caps_key(key_ctrl_caps_func_t func);
extern void keyboard_toggle_caps_key(void);
typedef int (*key_ctrl_get_caps_func_t)(void);
extern void keyboard_register_get_caps_key(key_ctrl_get_caps_func_t func);
extern int keyboard_get_caps_key(void);

typedef void (*key_joy_keypad_func_t)(int row, int col, int pressed);
extern void keyboard_register_joy_keypad(key_joy_keypad_func_t func);

typedef void (*keyboard_machine_func_t)(int *);
extern void keyboard_register_machine(keyboard_machine_func_t func);

/* switch to alternative set (x128) */
extern void keyboard_alternative_set(int alternative);

/* FIXME: These ugly externs will should away sooner or later.
   currently these two arrays are the interface to the emulation (eg CIA core) */
extern int keyarr[KBD_ROWS];
extern int rev_keyarr[KBD_COLS];

#endif
