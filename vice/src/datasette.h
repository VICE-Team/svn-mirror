/*
 * datasette.h - CBM cassette implementation.
 *
 * Written by
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#ifndef _DATASETTE_H
#define _DATASETTE_H

#include "tap.h"

#define DATASETTE_CONTROL_STOP    0
#define DATASETTE_CONTROL_START   1
#define DATASETTE_CONTROL_FORWARD 2
#define DATASETTE_CONTROL_REWIND  3
#define DATASETTE_CONTROL_RECORD  4
#define DATASETTE_CONTROL_RESET   5

void datasette_init(void);
void datasette_set_tape_image(tap_t *image);
void datasette_control(int command);
void datasette_set_motor(int flag);
void datasette_reset(void);

/* Emulator specific functions.  */
void datasette_trigger_flux_change(void);

#endif

