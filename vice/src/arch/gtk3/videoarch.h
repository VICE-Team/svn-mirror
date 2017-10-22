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

typedef enum {
    VICE_GTK3_RENDERER_CAIRO,
    VICE_GTK3_RENDERER_OPENGL,
    /* We may want to extend this later with:
     *   - Open GL Legacy
     *   - Open GL ES
     */
    VICE_NUM_GTK3_RENDERERS
} vice_gtk3_renderer_t;

struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;

    vice_gtk3_renderer_t renderer;
    /* GTK3's video canvas is either a GtkDrawingArea or a GtkGLArea,
     * depending on which renderer has been selected. Each is
     * ultimately represented as a GtkWidget object and backed by a
     * 24-bit backbuffer. */
    GtkWidget *drawing_area;
    unsigned char *backbuffer;

    /* The Cairo backend uses these values to properly render the
     * backbuffer image into the drawing area. */
    cairo_surface_t *backing_surface;
    cairo_matrix_t cairo_transform;

    /* The OpenGL backend uses these values to properly render the
     * backbuffer image into the GL context. */
    uint32_t shader, texture;

    /* The remainder is code the core needs to communicate with the
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

#endif
