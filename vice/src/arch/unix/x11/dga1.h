/*
 * dga1.h
 *
 * Written by
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

#ifndef _DGA1_H
#define _DGA1_H

struct video_canvas_s;
struct ui_menu_entry_s;

extern int dga1_init(void);
extern int dga1_available(void);
extern int dga1_enable(struct video_canvas_s *canvas, int enable);
extern int dga1_mode(struct video_canvas_s *canvas, int mode);
extern void dga1_shutdown(void);
extern void dga1_suspend(int level);
extern void dga1_resume(void);
extern void dga1_mode_callback(void *callback);
extern void dga1_create_menus(struct ui_menu_entry_s menu[]);

#endif

