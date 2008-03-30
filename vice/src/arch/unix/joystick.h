/*
 * joystick.h - Joystick support for Linux.
 *
 * Written by
 *  Bernhard Kuhn    (kuhn@eikon.e-technik.tu-muenchen.de)
 *  Ulmer Lionel     (ulmer@poly.polytechnique.fr)
 *
 * Patches by
 *  Daniel Sladic    (sladic@eecg.toronto.edu)
 *
 * 1.1.xxx Linux API by
 *  Luca Montecchiani	(m.luca@usa.net) (http://i.am/m.luca)
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

#ifndef _JOYSTICK_H
#define _JOYSTICK_H

extern void joystick_init(void);
extern void joystick_close(void);
extern void joystick(void);
extern void old_joystick_init(void);
extern void old_joystick_close(void);
extern void old_joystick(void);
extern void new_joystick_init(void);
extern void new_joystick_close(void);
extern void new_joystick(void);
extern int  joystick_init_resources(void);
extern int  joystick_init_cmdline_options(void);

extern int joystick_port_map[2];

#define JOYDEV_NONE         0
#define JOYDEV_NUMPAD       1
#define JOYDEV_CUSTOM_KEYS  2
#define JOYDEV_ANALOG_0     3
#define JOYDEV_ANALOG_1     4
#define JOYDEV_DIGITAL_0    5
#define JOYDEV_DIGITAL_1    6

#endif
