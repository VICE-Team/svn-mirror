/*
 * dga2.h
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

#ifndef _DGA2_H
#define _DGA2_H

#include "raster/raster.h"

int fullscreen_available(void);
void fullscreen_set_raster(raster_t *raster);

extern int fullscreen_is_enabled;
extern raster_t *fs_cached_raster;

extern int dga2_init(void);
extern int dga2_available(void);
extern int dga2_enable(struct video_canvas_s *canvas, int enable);
extern int dga2_mode(struct video_canvas_s *canvas, int mode);
extern void dga2_shutdown(void);
extern void dga2_suspend(int level);
extern void dga2_resume(void);
extern void dga2_mode_callback(void *callback);
extern void dga2_create_menus(struct ui_menu_entry_s menu[]);
extern void dga2_set_mouse_timeout(void);
extern int dga2_init_alloc_hooks(struct video_canvas_s *canvas);

#endif

