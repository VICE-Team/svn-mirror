/*
 * videoarch.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#ifndef VICE_VIDEOARCH_H
#define VICE_VIDEOARCH_H

#include "vice.h"
#include "types.h"
#include "video.h"

struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;
    unsigned int width, height;
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    struct video_resource_chip_s *video_resource_chip;

    unsigned int depth;

    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
};
typedef struct video_canvas_s video_canvas_t;

extern int machine_ui_done;
extern int vicii_setup_delay;
extern int xoffset, yoffset;
extern int xoffset_centred, yoffset_centred;
extern int xoffset_uncentred, yoffset_uncentred;
extern video_canvas_t *current_canvas;

#endif
