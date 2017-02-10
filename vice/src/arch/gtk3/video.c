/*
 * video.c - Native GTK3 UI video stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"

#include "raster.h"
#include "videoarch.h"

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    NOT_IMPLEMENTED();
}

int video_arch_cmdline_options_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

int video_arch_resources_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void video_arch_resources_shutdown(void)
{
    NOT_IMPLEMENTED();
}

char video_canvas_can_resize(video_canvas_t *canvas)
{
    NOT_IMPLEMENTED();
    return 0;
}

video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width, unsigned int *height, int mapped)
{
    NOT_IMPLEMENTED();
    return NULL;
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
    NOT_IMPLEMENTED();
}

void video_canvas_refresh(struct video_canvas_s *canvas, unsigned int xs, unsigned int ys, unsigned int xi, unsigned int yi, unsigned int w, unsigned int h)
{
    NOT_IMPLEMENTED();
}

void video_canvas_resize(struct video_canvas_s *canvas, char resize_canvas)
{
    NOT_IMPLEMENTED();
}

int video_canvas_set_palette(struct video_canvas_s *canvas, struct palette_s *palette)
{
    NOT_IMPLEMENTED();
    return 0;
}

int video_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void video_shutdown(void)
{
    NOT_IMPLEMENTED();
}

