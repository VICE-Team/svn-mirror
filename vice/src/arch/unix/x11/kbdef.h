/*
 * kbdef.h - X11 keyboard definitions.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

/* Keyboard definitions for the X11 keyboard driver. */

#ifndef _KBDEF_H
#define _KBDEF_H

#include "keyboard.h"

enum shift_type {
    NO_SHIFT =(0),		/* Key is not shifted. */
    VIRTUAL_SHIFT =(1 << 0),	/* The key needs a shift on the real machine. */
    LEFT_SHIFT =(1 << 1),	/* Key is left shift. */
    RIGHT_SHIFT =(1 << 2),	/* Key is right shift. */
    ALLOW_SHIFT =(1 << 3)	/* Allow key to be shifted. */
};

typedef struct keyconv_s {
    KeySym sym;
    int row;
    int column;
    enum shift_type shift;
} keyconv_t;

extern keyconv_t *keyconvmap;

/* Restore key.  */
extern KeySym key_ctrl_restore1;
#ifdef XK_Page_Up
extern KeySym key_ctrl_restore2;
#else
extern KeySym key_ctrl_restore2;
#endif
/* Shift status */
extern short kbd_lshiftrow;
extern short kbd_lshiftcol;
extern short kbd_rshiftrow;
extern short kbd_rshiftcol;

/* 40/80 column key.  */
extern KeySym key_ctrl_column4080;
extern key_ctrl_column4080_func_t key_ctrl_column4080_func;

extern int joypad_status[2][10];

/* Prototypes */
int check_set_joykeys(KeySym key, int joynum);
int check_clr_joykeys(KeySym key, int joynum);

#endif

