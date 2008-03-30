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

struct video_canvas_s;
struct ui_menu_entry_s;

extern int vm_is_enabled;

extern int vidmode_init(void);
extern int vidmode_available(void);

extern int vidmode_enable(struct video_canvas_s *canvas, int enable);
extern int vidmode_mode(struct video_canvas_s *canvas, int mode);
extern void vidmode_mode_callback(void *callback);
extern void vidmode_create_menus(struct ui_menu_entry_s menu[]);

#endif

