/**
 * \file   videoarch.h
 * \brief  Headless graphics routines
 *
 * \author Ettore Perazzoli
 * \author Teemu Rantanen <tvr@cs.hut.fi>
 * \author Andreas Boose <viceteam@t-online.de>
 * \author Michael C. Martin <mcmartin@gmail.com>
 */

/*
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

#include "archdep.h"
#include "video.h"

typedef void (*video_refresh_func_t)(struct video_canvas_s *, int, int, int, int, unsigned int, unsigned int);

typedef struct video_canvas_s {
    /** \brief Nonzero if it is safe to access other members of the
     *         structure. */
    unsigned int initialized;

    /** \brief Nonzero if the structure has been fully realized. */
    unsigned int created;

    /** \brief Index of the canvas, needed for x128 and xcbm2 */
    int index;
    
    /** \brief Used to coordinate vice thread access */
    void *lock;
    
    /** \brief A queue of rendered backbuffers ready to be displayed */
    void *render_queue;

    struct video_render_config_s *videoconfig;
    int crt_type;
    struct draw_buffer_s *draw_buffer;
    struct draw_buffer_s *draw_buffer_vsid;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    struct raster_s *parent_raster;

    struct fullscreenconfig_s *fullscreenconfig;
    video_refresh_func_t video_fullscreen_refresh_func;
} video_canvas_t;

typedef struct vice_renderer_backend_s {
} vice_renderer_backend_t;

#endif
