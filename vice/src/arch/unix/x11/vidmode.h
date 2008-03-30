/*
 * vidmode.h
 *
 * Written by
 *  Martin Pottendorfer <pottendo@utanet.at>
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _VIDMODE_H
#define _VIDMODE_H

extern int vm_selected_videomode;
extern int vm_is_enabled;

extern int vidmode_init(void);
extern int vidmode_set_bestmode(void *v, void *p);
extern int vidmode_set_mode(void *v, void *p);
extern int vidmode_available(void);
extern void vidmode_create_menus(void);

#endif

