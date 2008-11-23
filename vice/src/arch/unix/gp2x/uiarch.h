/*
 * uiarch.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#ifndef VICE_UIARCH_H
#define VICE_UIARCH_H

#include "vice.h"
#include "types.h"

#define UI_CALLBACK(name)                               \
    void name(void)

typedef void * ui_callback_t;
typedef void * ui_callback_data_t;
typedef void * ui_keysym_t;
typedef void * ui_hotkey_modifier_t;
typedef void * ui_window_t;

#define UI_MENU_CB_PARAM NULL

#define False 0
#define True 1

#define XK_m NULL
#define UI_HOTMOD_META NULL

extern void ui_set_left_menu(int);
extern void ui_set_right_menu(int);
extern void ui_set_speedmenu(int);
extern void ui_set_tape_menu(int);
extern int ui_menu_create(const char *name, ...);

#define ui_menu_separator ""

#define CHECK_MENUS 0

extern void ui_menu_set_sensitive(int, int);
extern void ui_menu_set_tick(int, int);
extern int w;

extern void ui_about(char *);

#define XK_0 0
#define XK_1 1
#define XK_2 1
#define XK_3 1
#define XK_4 1
#define XK_5 1
#define XK_6 1
#define XK_7 1
#define XK_8 8
#define XK_9 9

#define XK_a 0
#define XK_b 0
#define XK_c 0
#define XK_e 0
#define XK_h 0
#define XK_i 0
#define XK_j 0
#define XK_k 0
#define XK_l 0
#define XK_n 0
#define XK_s 0
#define XK_t 0
#define XK_u 0
#define XK_p 0
#define XK_q 0
#define XK_r 0
#define XK_s 0
#define XK_w 0
#define XK_z 0
#define XK_N 0

#define XK_F9 0
#define XK_F10 0
#define XK_F11 0
#define XK_F12 0

#endif
