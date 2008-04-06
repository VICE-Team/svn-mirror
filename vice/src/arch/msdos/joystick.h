/*
 * joystick.h - Joystick support for MS-DOS.
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

#ifndef _VICE_JOYSTICK_H
#define _VICE_JOYSTICK_H

typedef enum {
    JOYDEV_NONE,
    JOYDEV_HW1,
    JOYDEV_HW2,
    JOYDEV_NUMPAD
} joystick_device_t;

extern void joystick_init(void);
extern int joystick_init_resources(void);
extern int joystick_init_cmdline_options(void);
extern void joystick_update(void);
extern void joystick_handle_key(void);

#endif
