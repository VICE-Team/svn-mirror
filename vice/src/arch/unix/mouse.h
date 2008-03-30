/*
 * mouse.h - Mouse handling for Unix-Systems.
 *
 * Written by
 *  Oliver Schaertel (orschaer@forwiss.uni-erlangen.de)
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _MOUSE_H
#define _MOUSE_H


#include "types.h"

extern int mouse_init_resources(void);
extern int mouse_init_cmdline_options(void);
extern int mouse_init(void);
extern void mouse_button(int bnumber, int state);


extern int _mouse_enabled;
extern int mouse_x, mouse_y;
extern int mouse_accel;

/* ------------------------------------------------------------------------- */

inline static BYTE mouse_get_x(void)
{
    static int last_mouse_x=0; 

    if(last_mouse_x - mouse_x > 16) {
      last_mouse_x -= 16;
      return (BYTE) ((last_mouse_x * mouse_accel) >> 1) & 0x7e;
    }
    if(last_mouse_x - mouse_x < -16) {
      last_mouse_x += 16;
      return (BYTE) ((last_mouse_x * mouse_accel) >> 1) & 0x7e;
    }
    last_mouse_x = mouse_x;
    return (BYTE) ((last_mouse_x * mouse_accel) >> 1) & 0x7e;
}

inline static BYTE mouse_get_y(void)
{
    static int last_mouse_y=0; 

    if(last_mouse_y - mouse_y > 16) {
      last_mouse_y -= 16;
      return (BYTE) ((last_mouse_y * mouse_accel) >> 1) & 0x7e;
    } 
    if(last_mouse_y - mouse_y < -16) {
      last_mouse_y += 16;
      return (BYTE) ((last_mouse_y * mouse_accel) >> 1) & 0x7e;
    }
    last_mouse_y = mouse_y;
    return (BYTE) ((last_mouse_y * mouse_accel) >> 1) & 0x7e;
}

inline static void mouse_move(int x, int y)
{

    if (! _mouse_enabled) return;

    mouse_x = x;
    mouse_y = 256 - y;
}

#endif
