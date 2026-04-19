/*
 * mouse_digital.h - Digital "joy" mouse handling (header)
 *
 * Written by
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

#ifndef VICE_MOUSE_DIGITAL_H
#define VICE_MOUSE_DIGITAL_H

void mouse_digital_init(void);
void mouse_digital_set_enabled(int enabled);
void mouse_digital_button_right(int pressed);
void mouse_digital_button_left(int pressed);

int mouse_digital_register(void);

void digital_mouse_set_machine_parameter(long clock_rate);

void digital_mouse_store(int port, uint8_t val);
uint8_t digital_mouse_read(void);

#endif
