/*
 * raster-canvas.c - Raster-based video chip emulation helper.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "vice.h"

#include <stdio.h>

#include "machine.h"
#include "raster-canvas.h"
#include "raster.h"
#include "videoarch.h"

void raster_canvas_update_all(raster_t *raster)
{
    raster_viewport_t *viewport;

    if (console_mode || vsid_mode || !raster->draw_buffer)
        return;

    viewport = &raster->viewport;

    if (!viewport->update_canvas)
        return;

    video_canvas_refresh(viewport->canvas,
                         raster->draw_buffer,
                         raster->draw_buffer_width,
#ifdef __OS2__
                         raster->draw_buffer_height,
#endif
                         viewport->first_x
                                + raster->geometry.extra_offscreen_border_left,
                         viewport->first_line,
                         viewport->x_offset,
                         viewport->y_offset,
                         MIN((viewport->width / viewport->pixel_size.width),
                             (raster->geometry.screen_size.width)),
                         MIN((viewport->height / viewport->pixel_size.height),
                             (raster->geometry.screen_size.height)));
}

inline static void update_canvas(raster_t *raster)
{
    raster_area_t *update_area;
    raster_viewport_t *viewport;
    int x, y, xx, yy;
    int w, h;

    if (console_mode || vsid_mode)
        return;

    update_area = &raster->update_area;
    viewport = &raster->viewport;

    if (update_area->is_null || !(viewport->update_canvas))
        return;

    x = update_area->xs;
    y = update_area->ys;
    xx = update_area->xs - viewport->first_x;
    yy = update_area->ys - viewport->first_line;
    w = update_area->xe - update_area->xs + 1;
    h = update_area->ye - update_area->ys + 1;

    if (video_render_get_fake_pal_state()) {
        /* if pal emu is activated, more pixels have to be updated */
        x -= 4;
        xx -= 4;
        w += 8;
        h ++;
    }

    if (xx < 0) {
        x -= xx;
        w += xx;
        xx = 0;
    }

    if (yy < 0) {
        y -= yy;
        h += yy;
        yy = 0;
    }
    x += raster->geometry.extra_offscreen_border_left;

    xx += viewport->x_offset;
    yy += viewport->y_offset;

    video_canvas_refresh(viewport->canvas,
                         raster->draw_buffer,
                         raster->draw_buffer_width,
#ifdef __OS2__
                         raster->draw_buffer_height,
#endif
                         x, y, xx, yy,
                         MIN(w, viewport->width / viewport->pixel_size.width
                             - xx),
                         MIN(h, viewport->height / viewport->pixel_size.height
                             - yy));

    update_area->is_null = 1;
}

void raster_canvas_handle_end_of_frame(raster_t *raster)
{
    if (!console_mode && !vsid_mode) {
      /* FIXME for SCREEN_MAX_SPRITE_WIDTH */
      raster->draw_buffer_ptr = raster->draw_buffer
                                + raster->geometry.extra_offscreen_border_left;
    }

    raster->current_line = 0;

    if (raster->skip_frame)
        return;

    if (raster->dont_cache)
        raster_canvas_update_all(raster);
    else
        update_canvas(raster);
}

