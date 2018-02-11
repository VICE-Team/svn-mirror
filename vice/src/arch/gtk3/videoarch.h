/**
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

struct vice_renderer_backend_s;

struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;

    /* All the UI elements in a canvas window are contained within a GtkGrid. */
    GtkWidget *grid;
    /* GTK3's video canvas is either a GtkDrawingArea or a GtkGLArea,
     * depending on which renderer has been selected. Each is
     * ultimately represented as a GtkWidget object. */
    GtkWidget *drawing_area;
#if 0
    /* The video canvas is surrounded by an GtkEventBox which can
     * capture events like mouse motion or clicks. */
    GtkWidget *event_box;
#endif
    /* The renderer backend selected for use this run. */
    struct vice_renderer_backend_s *renderer_backend;
    /* Renderers have data unique to themselves, too. They'll know
     * what this is. */
    void *renderer_context;
    /* Cursors are unique to windows, so if we want to hide the mouse
     * cursor, this will hold the "blank" cursor for this canvas. */
    GdkCursor *blank_ptr;
    /* And if the light pen is enabled, this will hold the special
     * light pen cursor for this canvas. */
    GdkCursor *pen_ptr;
    /* Number of frames the mouse hasn't moved while still on the canvas. */
    unsigned int still_frames;
    guint still_frame_callback_id;

    /* The remainder are fields the core needs to communicate with the
     * renderers. */
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
#if 0
    float refreshrate; /* currently displayed refresh rate */
#endif

    struct video_draw_buffer_callback_s *video_draw_buffer_callback;

    int offx; /* for lightpen */
    int window_index; /* index of window that belongs to this canvas */
};
typedef struct video_canvas_s video_canvas_t;

void video_canvas_adjust_aspect_ratio(struct video_canvas_s *canvas);

/* The renderer backend selected for use this run. */
struct vice_renderer_backend_s {
    GtkWidget *(*create_widget)(video_canvas_t *canvas);
    void (*update_context)(video_canvas_t *canvas,
                           unsigned int width, unsigned int height);
    void (*destroy_context)(video_canvas_t *canvas);
    void (*refresh_rect)(video_canvas_t *canvas,
                         unsigned int xs, unsigned int ys,
                         unsigned int xi, unsigned int yi,
                         unsigned int w, unsigned int h);
    void (*set_palette)(video_canvas_t *canvas);
};
typedef struct vice_renderer_backend_s vice_renderer_backend_t;

#endif
