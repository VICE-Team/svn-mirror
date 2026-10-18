/*
 * mon_input.h - The VICE built-in monitor, keyboard matrix and joystick
 *               input injection.
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

#ifndef VICE_MON_INPUT_H
#define VICE_MON_INPUT_H

#include "montypes.h"

void mon_keymatrix_show(void);
int mon_keymatrix_set(int row, int col, ACTION action, int frames);
int mon_keymatrix_restore(ACTION action, int frames);
void mon_keymatrix_clear(void);

void mon_joystick_show(void);
int mon_joystick_set(int port, int value, int frames);
void mon_joystick_clear(void);

void mon_input_vsync_hook(void);

#endif
