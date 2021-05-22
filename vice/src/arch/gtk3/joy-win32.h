/** \file   joy-win32.h
 * \brief   Joystick support for Windows - header
 *
 * \author  Ettore Perazzoli <ettore@comm2000.it>
 */

/*
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

#ifndef VICE_JOY_WIN32_H
#define VICE_JOY_WIN32_H

typedef int joystick_device_t;

/* standard devices */
#define JOYDEV_NONE    0
#define JOYDEV_NUMPAD  1
#define JOYDEV_KEYSET1 2
#define JOYDEV_KEYSET2 3
/* extra devices */
#define JOYDEV_HW1     4
#define JOYDEV_HW2     5
#define JOYDEV_HW3     6
#define JOYDEV_HW4     7
#define JOYDEV_HW5     8
#define JOYDEV_HW6     9
#define JOYDEV_HW7     10
#define JOYDEV_HW8     11

#define JOYDEV_DEFAULT   JOYDEV_NUMPAD

extern void joystick_close(void);
extern void joystick(void);

extern int joystick_uses_direct_input(void);

extern void joystick_ui_reset_device_list(void);
extern const char *joystick_ui_get_next_device_name(int *id);

#endif
