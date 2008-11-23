/*
 * input_gp2x.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
 *  Mustafa 'GnoStiC' Tufan <mtufan@gmail.com>
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

#ifndef VICE_INPUT_GP2X_H
#define VICE_INPUT_GP2X_H

extern unsigned int input_up, input_down, input_left, input_right, 
	input_a, input_b, input_x, input_y, 
	input_select, input_start;

extern unsigned long gp2x_usbjoys;
extern unsigned int cur_port;

extern void gp2x_poll_input();

extern signed long kbd_arch_keyname_to_keynum(char *keyname);
extern const char *kbd_arch_keynum_to_keyname(signed long keynum);

#endif
