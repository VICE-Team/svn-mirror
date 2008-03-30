/*
 * video-canvas.c
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>

#include "machine.h"
#include "lib.h"
#include "types.h"
#include "video-render.h"
#include "video.h"
#include "videoarch.h"


/* Temporary! */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif


video_canvas_t *video_canvas_init(void)
{
    video_canvas_t *canvas;

    canvas = (video_canvas_t *)lib_calloc(1, sizeof(video_canvas_t));

    canvas->videoconfig = (video_render_config_t *)lib_calloc(1,
        sizeof(video_render_config_t));
    canvas->draw_buffer = (draw_buffer_t *)lib_calloc(1, sizeof(draw_buffer_t));
    canvas->viewport = (viewport_t *)lib_calloc(1, sizeof(viewport_t));
    canvas->geometry = (geometry_t *)lib_calloc(1, sizeof(geometry_t));

    video_arch_canvas_init(canvas);

    return canvas;
}

void video_canvas_shutdown(video_canvas_t *canvas)
{
    if (canvas != NULL) {
        free(canvas->videoconfig);
        free(canvas->draw_buffer);
        free(canvas->viewport);
        free(canvas->geometry);
    }
}

void video_canvas_render(video_canvas_t *canvas, BYTE *trg, int width,
                         int height, int xs, int ys, int xt, int yt,
                         int pitcht, int depth)
{
#ifdef VIDEO_SCALE_SOURCE
    if (canvas->videoconfig->doublesizex)
        xs /= 2;
    if (canvas->videoconfig->doublesizey)
        ys /= 2;
#endif

    video_render_main(canvas->videoconfig, canvas->draw_buffer->draw_buffer,
                      trg, width, height, xs, ys, xt, yt,
                      canvas->draw_buffer->draw_buffer_width, pitcht, depth);

}

void video_canvas_refresh_all(video_canvas_t *canvas)
{
    viewport_t *viewport;
    geometry_t *geometry;

    if (console_mode || vsid_mode) {
        return;
    }

    viewport = canvas->viewport;
    geometry = canvas->geometry;

    video_canvas_refresh(canvas,
                 viewport->first_x
                 + geometry->extra_offscreen_border_left,
                 viewport->first_line,
                 viewport->x_offset,
                 viewport->y_offset,
                 MIN(canvas->draw_buffer->canvas_width,
                     geometry->screen_size.width - viewport->first_x),
                 MIN(canvas->draw_buffer->canvas_height,
                     geometry->screen_size.height - viewport->first_line));
}

void video_canvas_redraw_size(video_canvas_t *canvas, unsigned int width,
                              unsigned int height)
{
    if (canvas->videoconfig->doublesizex)
        width /= 2;
    if (canvas->videoconfig->doublesizey)
        height /= 2;

    if (width != canvas->draw_buffer->canvas_width
        || height != canvas->draw_buffer->canvas_height) {
        canvas->draw_buffer->canvas_width = width;
        canvas->draw_buffer->canvas_height = height;
        video_viewport_resize(canvas);
    }
    video_canvas_refresh_all(canvas);
}

