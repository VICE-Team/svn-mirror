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

#include "types.h"
#include "utils.h"
#include "video-render.h"
#include "video.h"
#include "videoarch.h"


video_canvas_t *video_canvas_init(video_render_config_t *videoconfig)
{
    video_canvas_t *canvas;

    canvas = (video_canvas_t *)xcalloc(1, sizeof(video_canvas_t));

    canvas->videoconfig = videoconfig;
    canvas->draw_buffer = (draw_buffer_t *)xcalloc(1, sizeof(draw_buffer_t));

    video_arch_canvas_init(canvas);

    return canvas;
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

