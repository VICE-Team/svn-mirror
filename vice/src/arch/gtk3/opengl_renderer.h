/**
 * \file directx_renderer.h
 * \brief   DirectX-based renderer for the GTK3 backend.
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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
#ifndef VICE_OPENGL_RENDERER_H
#define VICE_OPENGL_RENDERER_H

#ifdef HAVE_GTK3_GLEW
#include <GL/glew.h>
#endif

#ifndef MACOSX_SUPPORT
#include <X11/Xlib.h>
#include <GL/glxew.h>
#endif

#include <stdbool.h>

#include "videoarch.h"

/** \brief A renderer that uses OpenGL to render to a native child window.
 *
 * Because OpenGL + GTK3 just doesn't work that well, this.
 */
extern vice_renderer_backend_t vice_opengl_backend;

/** \brief Rendering context for the OpenGL backend.
 *  \sa video_canvas_s::renderer_context */
typedef struct vice_opengl_renderer_context_s {
    /** \brief needed to coordinate access to the context between vice and main threads */
    pthread_mutex_t canvas_lock;

    /** \brief used to coordinate access to native rendering resources */
    pthread_mutex_t render_lock;

    /** \brief A 'pool' of one thread used to render backbuffers */
    GThreadPool *render_thread;

    /** \brief A queue of backbuffers ready for painting to the widget */
    void *render_queue;
    
#ifdef MACOSX_SUPPORT
    /** \brief native child window for OpenGL to draw on */
    void *native_view;
#else
    /** \brief connection to the x server */
    Display *x_display;

    /** \brief native child window for OpenGL to draw on */
    Window x_overlay_window;

    /** \brief native graphics context for OpenGL to draw on */
    GC x_overlay_graphics_context;

    /** \brief OpenGL graphics context */
    GLXContext gl_context;
#endif
    /** \brief Less than OpenGL 3.2 uses the legacy renderer */
    bool gl_context_is_legacy;

    /** \brief size of the backing layer in pixels. Can be higher than native_view_width (high dpi) */
    unsigned int gl_backing_layer_width;
    
    /** \brief size of the backing layer in pixels. Can be higher than native_view_height (high dpi) */
    unsigned int gl_backing_layer_height;

    /** \brief location of the native view in gtk coordinates (which are not pixels) */
    int native_view_x;

    /** \brief location of the native view in gtk coordinates (which are not pixels) */
    int native_view_y;

    /** \brief size of the native view in gtk coordinates (which are not pixels) */
    unsigned int native_view_width;
    
    /** \brief size of the native view in gtk coordinates (which are not pixels) */
    unsigned int native_view_height;

    /** \brief minimum size for the drawing area, based on emu and aspect ratio settings */
    unsigned int native_view_min_width;

    /** \brief minimum size for the drawing area, based on emu and aspect ratio settings */
    unsigned int native_view_min_height;
    
    /** \brief background colour for the native view */
    float native_view_bg_r;
    
    /** \brief background colour for the native view */
    float native_view_bg_g;
    
    /** \brief background colour for the native view */
    float native_view_bg_b;

    /** \brief The OpenGL program that comprises our vertex and fragment shaders. */
    GLuint program;

    /** \brief The index of the "position" parameter in the shader program. */
    GLuint position_index;

    /** \brief The index of the "texCoord" parameter in the shader program. */
    GLuint tex_coord_index;

    /** \brief The vertex buffer object that holds our vertex data. */
    GLuint vbo;

    /** \brief The vertex array object that gives structure to our vertex data. */
    GLuint vao;

    /** \brief The texture identifier for the GPU's copy of our  machine display. */
    GLuint texture;

    /** \brief size of the next frame to be emulated */
    unsigned int emulated_width_next;
    
    /** \brief size of the next frame to be emulated */
    unsigned int emulated_height_next;

    /** \brief pixel aspect ratio of the next frame to be emulated */
    float pixel_aspect_ratio_next;

    /** \brief size of the last frame to be rendered on host */
    unsigned int emulated_width_last_rendered;
    
    /** \brief size of the last frame to be rendered on host */
    unsigned int emulated_height_last_rendered;

    /** \brief pixel aspect ratio of the last frame to be rendered on host */
    float pixel_aspect_ratio_last_rendered;

    /** \brief when the last frame was rendered */
    unsigned long last_render_time;

    /** \brief when the host last refreshed */
    unsigned long last_host_frame_time;

} vice_opengl_renderer_context_t;

void vice_opengl_renderer_create_child_view(GtkWidget *widget, vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_resize_child_view(vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_destroy_child_view(vice_opengl_renderer_context_t *context);
     
void vice_opengl_renderer_make_current(vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_set_viewport(vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_set_vsync(vice_opengl_renderer_context_t *context, bool enable_vsync);
void vice_opengl_renderer_render(vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_present_backbuffer(vice_opengl_renderer_context_t *context);
void vice_opengl_renderer_clear_current(vice_opengl_renderer_context_t *context);

#endif /* #ifndef VICE_OPENGL_RENDERER_H */
