/*
 * xrandr.h
 *
 * Written by
 * pottendo <pottendo@gmx.net>
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

#ifndef __xrandr_h__
#define __xrandr_h__

struct ui_menu_entry_s;

int xrandr_init(void);
void xrandr_menu_create(struct ui_menu_entry_s *menu);
int xrandr_enable(struct video_canvas_s *canvase, int activate);
int xrandr_mode(struct video_canvas_s *canvase, int mode);
void xrandr_menu_shutdown(struct ui_menu_entry_s *menu);
void xrandr_mode_callback(ui_callback_t cb);
void xrandr_shutdown(void);

#endif /* __xrandr_h__ */
