/*
 * userport_snespad.h: Userport single SNES pad device emulation.
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef VICE_USERPORT_SNESPAD_H
#define VICE_USERPORT_SNESPAD_H

#include "types.h"

#define USERPORT_SNESPAD_BUTTON_B        0
#define USERPORT_SNESPAD_BUTTON_Y        1
#define USERPORT_SNESPAD_BUTTON_SELECT   2
#define USERPORT_SNESPAD_BUTTON_START    3
#define USERPORT_SNESPAD_UP              4
#define USERPORT_SNESPAD_DOWN            5
#define USERPORT_SNESPAD_LEFT            6
#define USERPORT_SNESPAD_RIGHT           7
#define USERPORT_SNESPAD_BUTTON_A        8
#define USERPORT_SNESPAD_BUTTON_X        9
#define USERPORT_SNESPAD_BUMPER_LEFT    10
#define USERPORT_SNESPAD_BUMPER_RIGHT   11

/* End Of Sequence */
#define USERPORT_SNESPAD_EOS            12

extern int userport_snespad_resources_init(void);
extern int userport_snespad_cmdline_options_init(void);

#endif
