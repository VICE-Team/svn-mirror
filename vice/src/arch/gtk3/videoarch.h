/** \file   src/arch/gtk3/videoarch.h
 * \broef   Native GTK3 graphics routines.
 *
 * based on the X11 version written by
 *  Ettore Perazzoli
 *  Teemu Rantanen <tvr@cs.hut.fi>
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
#ifndef VICE_VIDEOARCH_H
#define VICE_VIDEOARCH_H

#include "vice.h"

#include "viewport.h"
#include "video.h"

#include <gtk/gtk.h>

struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;

    /* GTK3's video canvas is either a GtkDrawingArea or a GtkGLArea,
     * depending on which renderer has been selected. Each is
     * ultimately represented as a GtkWidget object. */
    GtkWidget *drawing_area;
    /* Renderers have data unique to themselves, too. They'll know
     * what this is. */
    void *renderer_context;

    /* The remainder are fields the core needs to communicate with the
     * renderers. */
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    float refreshrate; /* currently displayed refresh rate */

    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
    struct fullscreenconfig_s *fullscreenconfig;

    int offx; /* for lightpen */
    int app_shell; /* app shell that belongs to this canvas */
};
typedef struct video_canvas_s video_canvas_t;

void video_canvas_adjust_aspect_ratio(struct video_canvas_s *canvas);

/* The renderer backend selected for use this run. */
struct vice_renderer_backend_s {
    GtkWidget *(*create_widget)(video_canvas_t *canvas);
    void (*update_context)(video_canvas_t *canvas,
                           unsigned int width, unsigned int height);
    void (*destroy_context)(video_canvas_t *canvas);
    int (*get_backbuffer_info)(video_canvas_t *canvas, unsigned int *width,
                                unsigned int *height, unsigned int *stride);
    void (*refresh_rect)(video_canvas_t *canvas,
                         unsigned int xs, unsigned int ys,
                         unsigned int xi, unsigned int yi,
                         unsigned int w, unsigned int h);
    void (*set_palette)(video_canvas_t *canvas);
};
typedef struct vice_renderer_backend_s vice_renderer_backend_t;

extern vice_renderer_backend_t *vice_renderer_backend;

#endif
