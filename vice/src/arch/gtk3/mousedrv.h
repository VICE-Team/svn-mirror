/*
 * mousedrv.h - Mouse handling for native GTK3 UI
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_MOUSEDRV_H
#define VICE_MOUSEDRV_H

#include "types.h"

#include "mouse.h"

int mousedrv_resources_init(mouse_func_t *funcs);
int mousedrv_cmdline_options_init(void);
void mousedrv_init(void);

void mousedrv_mouse_changed(void);

int mousedrv_get_x(void);
int mousedrv_get_y(void);
unsigned long mousedrv_get_timestamp(void);

void mouse_button(int bnumber, int state);
void mouse_move(float dx, float dy);

void mousedrv_button_left(int pressed);
void mousedrv_button_right(int pressed);
void mousedrv_button_middle(int pressed);
void mousedrv_button_up(int pressed);
void mousedrv_button_down(int pressed);

#endif
