/*
 * x11kbd.h - x11 keyboard interface layer
 *
 * Written by
 *  Martin Pottendorfer <pottendo@utanet.at>
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

/* X11 keyboard driver. */

#ifndef __x11kbd_h__
#define __x11kbd_h__

#include "ui.h"

void x11kbd_press(ui_keysym_t key);
void x11kbd_release(ui_keysym_t key);
void x11kbd_enter_leave(void);
void x11kbd_focus_change (void);

#endif /* __x11kbd_h__ */
