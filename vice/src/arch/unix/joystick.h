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

extern void joyini(void);
extern void joyclose(void);
extern void joystick(void);
extern void joyset(void);
extern void joyport1select(int port);
extern void joyport2select(int port);
extern void joyset(void);

extern int ajoyfd[2];
extern int djoyfd[2];

int hjoyport[2];

#endif
