/*
 * vidmode.h
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

#ifndef __vidmode_h__
#define __vidmode_h__

#include <X11/Xlib.h>

extern int vm_selected_videomode;
extern int vm_is_enabled;

int vidmode_init(Display *display, int screen);
int vidmode_set_bestmode(void *v, void *p);
int vidmode_set_mode(void *v, void *p);
int vidmode_available(void);
void vidmode_create_menus(void);

#endif /* __vidmode_h__ */

