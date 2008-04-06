/*
 * joystick.c - Joystick support for MS-DOS.
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

/* This is a quick hack, joystick support should be integrated better via the
   `app_resources'. */

#include "vice.h"

#include <stdio.h>
#include <allegro.h>

#include "resources.h"
#include "kbd.h"              /* FIXME: Maybe we should move `joy[]' here... */

int joystick_init_resources(void)
{
    return 0;
}

int joystick_init_cmdline_options(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Flag: is joystick present? */
int joystick_available = 0;

/* Flag: have we initialized the Allegro joystick driver? */
static int joystick_init_done = 0;

/* ------------------------------------------------------------------------- */

/* Initialize joystick support. */
void joystick_init(void)
{
    if (joystick_init_done)
	return;

    if (!initialise_joystick()) {
	printf("One joystick found.\n");
	joystick_available = 1;
    } else {
	joystick_available = 0;
	printf("No joysticks found.\n");
    }
}

/* Update the `joy' variables according to the joystick status. */
void joystick_update(void)
{
    int value;

    if (!joystick_available)
	return;

    value = 0;

    poll_joystick();
    if (joy_left)
	value |= 4;
    if (joy_right)
	value |= 8;
    if (joy_up)
	value |= 1;
    if (joy_down)
	value |= 2;
    if (joy_b1 || joy_b2)
	value |= 16;

#if 0
    /* FIXME: in this a quick & dirty implementation, we use the joystick
       port that is not used by the numpad-based emulation. */
    joy[3 - app_resources.joyPort] = value;
#endif
}
