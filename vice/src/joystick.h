/*
 * joystick.h - Common joystick emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "types.h"

struct snapshot_s;

extern int joystick_init(void);

extern int joystick_check_set(signed long key, int joynum);
extern int joystick_check_clr(signed long key, int joynum);
extern void joystick_joypad_clear(void);

extern void joystick_set_value_absolute(unsigned int joyport, BYTE value);
extern void joystick_set_value_or(unsigned int joyport, BYTE value);
extern void joystick_set_value_and(unsigned int joyport, BYTE value);
extern void joystick_clear(unsigned int joyport);
extern void joystick_clear_all(void);
extern BYTE joystick_get_value_absolute(unsigned int joyport);

extern void joystick_event_playback(CLOCK offset, void *data);
extern int joystick_snapshot_write_module(struct snapshot_s *s);
extern int joystick_snapshot_read_module(struct snapshot_s *s);

#endif

