/**
 * \file opengl_renderer.c
 * \brief   OpenGL renderer for the GTK3 backend.
 *
 * \author Michael C. Martin <mcmartin@gmail.com>
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

#include "vice.h"

#include "opengl_renderer.h"

#include <assert.h>
#include <gtk/gtk.h>
#include <math.h>

#ifdef MACOS_COMPILE
#include <CoreGraphics/CGDirectDisplay.h>
#endif

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "monitor.h"
#include "palette.h"
#include "render-queue.h"
#include "resources.h"
#include "sysfile.h"
#include "ui.h"
#include "uistatusbar.h"
#include "util.h"
#include "vsync.h"
#include "vsyncapi.h"

#ifdef MACOS_COMPILE
#include "macOS-util.h"
#endif

#define CANVAS_LOCK() archdep_mutex_lock(canvas->lock)
#define CANVAS_UNLOCK() archdep_mutex_unlock(canvas->lock)
#define RENDER_LOCK() archdep_mutex_lock(context->render_lock)
#define RENDER_UNLOCK() archdep_mutex_unlock(context->render_lock)

typedef vice_opengl_renderer_context_t context_t;

static void on_widget_realized(GtkWidget *widget, gpointer data);
static void on_widget_unrealized(GtkWidget *widget, gpointer data);
static void on_widget_resized(GtkWidget *widget, GdkRectangle *allocation, gpointer data);
static void on_widget_monitors_changed(GdkScreen *screen, gpointer data);
static void render(void *job_data, void *pool_data);

static GLuint create_shader(GLenum shader_type, const char *text);
static GLuint create_shader_program(char *vertex_shader_filename, char *fragment_shader_filename);

/** \brief Raw geometry for the machine screen.
 *
 * The first sixteen elements describe a rectangle the size of the
 * entire display area, and the last eight assign texture coordinates
 * to each corner.
 */
static float vertexData[] = {
        -1.0f,    -1.0f, 0.0f, 1.0f,
         1.0f,    -1.0f, 0.0f, 1.0f,
        -1.0f,     1.0f, 0.0f, 1.0f,
         1.0f,     1.0f, 0.0f, 1.0f,
         0.0f,     1.0f,
         1.0f,     1.0f,
         0.0f,     0.0f,
         1.0f,     0.0f
};

/**/

static void vice_opengl_initialise_canvas(video_canvas_t *canvas)
{
    context_t *context;

    CANVAS_LOCK();

    /* First initialise the context_t that we'll need everywhere */
    context = lib_calloc(1, sizeof(context_t));
    context->current_vsync_value = -1;

    context->canvas_lock_ptr = &canvas->lock;
    context->canvas_render_queue = canvas->render_queue;
    archdep_mutex_create(&context->render_lock);

    context->canvas = canvas;
    canvas->renderer_context = context;

    g_signal_connect(canvas->event_box, "realize", G_CALLBACK (on_widget_realized), canvas);
    g_signal_connect(canvas->event_box, "unrealize", G_CALLBACK (on_widget_unrealized), canvas);
    g_signal_connect_unlocked(canvas->event_box, "size-allocate", G_CALLBACK(on_widget_resized), canvas);

    CANVAS_UNLOCK();
}

static void vice_opengl_destroy_context(video_canvas_t *canvas)
{
    context_t *context;

    CANVAS_LOCK();

    context = canvas->renderer_context;

    archdep_mutex_destroy(context->render_lock);

    lib_free(context);

    canvas->renderer_context = NULL;

    CANVAS_UNLOCK();
}

static void on_widget_realized(GtkWidget *widget, gpointer data)
{
    video_canvas_t *canvas = data;
    context_t *context = canvas->renderer_context;
    GtkAllocation allocation;
    gint gtk_scale;

    CANVAS_LOCK();

#ifdef MACOS_COMPILE
    /* The content area coordinates include the menu on macOS */
    gtk_widget_translate_coordinates(widget, gtk_widget_get_toplevel(widget), 0, 0, &context->native_view_x, &context->native_view_y);
#endif

    gtk_widget_get_allocation(widget, &allocation);
    context->native_view_width  = allocation.width;
    context->native_view_height = allocation.height;

    gtk_scale = gtk_widget_get_scale_factor(widget);
    context->gl_backing_layer_width     = context->native_view_width  * gtk_scale;
    context->gl_backing_layer_height    = context->native_view_height * gtk_scale;

    /* Create a native child window to render onto */
    vice_opengl_renderer_create_child_view(widget, context);

    /* OpenGL initialisation */
    vice_opengl_renderer_make_current(context);

    if (!context->gl_context_is_legacy) {
        context->shader_builtin             = create_shader_program("viewport.vert", "builtin.frag");
        context->shader_builtin_interlaced  = create_shader_program("viewport.vert", "builtin-interlaced.frag");
        context->shader_bicubic             = create_shader_program("viewport.vert", "bicubic.frag");
        context->shader_bicubic_interlaced  = create_shader_program("viewport.vert", "bicubic-interlaced.frag");

        glGenBuffers(1, &context->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glGenVertexArrays(1, &context->vao);
    }

    glGenTextures(1, &context->current_frame_texture);
    glGenTextures(1, &context->previous_frame_texture);

    vice_opengl_renderer_clear_current(context);

    /* Create an exclusive single thread 'pool' for executing render jobs */
    context->render_thread = render_thread_create(render, canvas);

    /* Monitor display DPI changes */
    g_signal_connect_unlocked(gtk_widget_get_screen(widget), "monitors_changed", G_CALLBACK(on_widget_monitors_changed), canvas);

    CANVAS_UNLOCK();
}

static void on_widget_unrealized(GtkWidget *widget, gpointer data)
{
    video_canvas_t *canvas = data;
    context_t *context = canvas->renderer_context;

    g_signal_handlers_disconnect_by_func(gtk_widget_get_screen(widget), G_CALLBACK(on_widget_monitors_changed), canvas);

    CANVAS_LOCK();

    /* Remove and dealloc the child view */
    vice_opengl_renderer_destroy_child_view(context);

    CANVAS_UNLOCK();
}

/** The underlying GtkDrawingArea has changed size (possibly before being realised) */
static void on_widget_resized(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
    video_canvas_t *canvas = data;
    context_t *context;
    gint gtk_scale;

    CANVAS_LOCK();

    context = canvas->renderer_context;
    if (!context) {
        CANVAS_UNLOCK();
        return;
    }

#ifdef MACOS_COMPILE
    /* The content area coordinates include the menu on macOS */
    gtk_widget_translate_coordinates(widget, gtk_widget_get_toplevel(widget), 0, 0, &context->native_view_x, &context->native_view_y);
#endif

    context->native_view_width = allocation->width;
    context->native_view_height = allocation->height;

    gtk_scale = gtk_widget_get_scale_factor(widget);
    context->gl_backing_layer_width     = context->native_view_width    * gtk_scale;
    context->gl_backing_layer_height    = context->native_view_height   * gtk_scale;

    /* Set the background colour */
    if (ui_is_fullscreen_from_canvas(canvas)) {
        context->native_view_bg_r = 0.0f;
        context->native_view_bg_g = 0.0f;
        context->native_view_bg_b = 0.0f;
    } else {
        context->native_view_bg_r = 0.5f;
        context->native_view_bg_g = 0.5f;
        context->native_view_bg_b = 0.5f;
    }

    CANVAS_UNLOCK();

    /* Update the size of the native child window to match the gtk drawing area */
    vice_opengl_renderer_resize_child_view(context);
}

static void on_widget_monitors_changed(GdkScreen *screen, gpointer data)
{
    video_canvas_t *canvas = data;
    context_t *context;
    GtkWidget *widget;
    GtkAllocation allocation;

    CANVAS_LOCK();

    context = canvas->renderer_context;
    if (!context) {
        CANVAS_UNLOCK();
        return;
    }

    CANVAS_UNLOCK();

    widget = canvas->event_box;

    gtk_widget_get_allocation(widget, &allocation);

    on_widget_resized(canvas->event_box, &allocation, canvas);
}


/** \brief It's time to draw a complete emulated frame */
static void vice_opengl_on_new_backbuffer(video_canvas_t *canvas)
{
    context_t *context;

    CANVAS_LOCK();

    context = canvas->renderer_context;
    if (context && context->render_thread) {
        render_thread_push_job(context->render_thread, render_thread_render);
    }

    CANVAS_UNLOCK();
}


#ifdef MACOS_COMPILE
static void macos_set_host_mouse_visibility(GtkWindow *gtk_window)
{
    /*
     * Without this, wiggle the mouse cases macOS to help you find the cursor by making it big.
     * Which sucks if we've set a blank cursor in mouse grab mode.
     *
     * Each frame, check if we've set a custom cursor, and if have, force hide the system mouse
     * so that the wiggle-to-find-mouse thing doesn't happen.
     *
     * But also unhide the mouse if the window loses focus.
     *
     * TODO: find a way to make this event driven on gdk window focus changes.
     */

    static bool hiding_mouse = false;

    bool should_hide_mouse = false;
    gboolean is_window_active;
    int mouse_grab;
    GList *list;
    GdkWindow *gdk_window;
    int i;

    is_window_active = gtk_window_is_active(gtk_window);
    resources_get_int("Mouse", &mouse_grab);

    if (mouse_grab && is_window_active) {

        should_hide_mouse = true;

        /*
         * Only hide the mouse if no secondary top levels are visible. For example it is
         * possible to make the emu window active when the settings dialog or monitor
         * are open, and hiding the mouse in these cases would be confusing.
         */

        for (list = gtk_window_list_toplevels(); list != NULL && should_hide_mouse; list = list->next) {

            gdk_window = gtk_widget_get_window(list->data);

            if (!gdk_window || !gdk_window_is_visible(gdk_window)) {
                continue;
            }

            /* There's a visible window, only allow the hide if the window is a primary ui window */
            should_hide_mouse = false;
            for (i = 0; i < NUM_WINDOWS; i++) {
                if (GTK_WINDOW(list->data) == GTK_WINDOW(ui_get_window_by_index(i))) {
                    should_hide_mouse = true;
                    break;
                }
            }
        }
    }

    if (should_hide_mouse) {
        if (!hiding_mouse) {
            CGDisplayHideCursor(kCGNullDirectDisplay);
            hiding_mouse = true;
        }
    } else {
        if (hiding_mouse) {
            CGDisplayShowCursor(kCGNullDirectDisplay);
            hiding_mouse = false;
        }
    }
}
#endif


static void vice_opengl_on_ui_frame_clock(GdkFrameClock *clock, video_canvas_t *canvas)
{
    context_t *context = canvas->renderer_context;

    ui_update_statusbars();
    
    /*
     * Sometimes the OS wants to redraw part of the window. Haven't been able to
     * reliably detect those events in some linux environments so we don't trigger
     * a redraw on that sort of event. It's not as simple as catching the GTK draw
     * signal, because we have our own native Xlib window/NSView added over the top
     * of the GTK/GDK window.
     *
     * So, each GdkFrameClock event, check if we are currently paused, and if we
     * are, queue up a refresh of the existing emu frame.
     *
     * This ensures that resizing while paused doesn't glitch like busy win95,
     * and also fixes various issues on some crappy X11 setups :)
     */

    if (ui_pause_active() || monitor_is_inside_monitor() || machine_is_jammed()) {
        render_thread_push_job(context->render_thread, render_thread_render);
    }

#ifdef MACOS_COMPILE
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_toplevel(canvas->event_box));

    macos_set_host_mouse_visibility(window);
#endif
}

static void update_minimum_window_size(gpointer data)
{
    context_t *context = (context_t*) data;
    video_canvas_t *canvas = context->canvas;
    
    CANVAS_LOCK();
    gtk_widget_set_size_request(canvas->event_box, context->native_view_min_width, context->native_view_min_height);
    CANVAS_UNLOCK();
}

static void update_texture(GLuint texture, backbuffer_t *backbuffer)
{    
    /*
     * Update the OpenGL texture with the new backbuffer bitmap
     */

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, backbuffer->width);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, backbuffer->width, backbuffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, backbuffer->pixel_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

typedef struct render_job_data_s {
    backbuffer_t *backbuffer;
    unsigned int view_width;
    unsigned int view_height;
    unsigned int frame_width;
    unsigned int frame_height;
    float scale_x;
    float scale_y;
    bool should_set_vsync;
    int vsync_value;
    bool use_interlaced_render;
    int filter_mode;
    float bg_r;
    float bg_g;
    float bg_b;
    bool gl_context_is_legacy;
    GLuint current_frame_texture;
    GLuint previous_frame_texture;
    GLuint shader_bicubic_interlaced;
    GLuint shader_builtin_interlaced;
    GLuint shader_bicubic;
    GLuint shader_builtin;
    GLuint vao;
    GLuint vbo;
} render_job_data_t;

static void legacy_render(render_job_data_t *job)
{
    /* Used when OpenGL 3.2+ is NOT available */

    GLuint gl_filter;

    float u1 = 0.0f;
    float v1 = 0.0f;
    float u2 = 1.0f;
    float v2 = 1.0f;

    /* We only support builtin linear and nearest on legacy OpenGL contexts */
    gl_filter = job->filter_mode ? GL_LINEAR : GL_NEAREST;

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    if (job->use_interlaced_render) {
        glBindTexture(GL_TEXTURE_2D, job->previous_frame_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(u1, v2);
        glVertex2f(-job->scale_x, -job->scale_y);
        glTexCoord2f(u2, v2);
        glVertex2f(job->scale_x, -job->scale_y);
        glTexCoord2f(u1, v1);
        glVertex2f(-job->scale_x, job->scale_y);
        glTexCoord2f(u2, v1);
        glVertex2f(job->scale_x, job->scale_y);
        glEnd();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glBindTexture(GL_TEXTURE_2D, job->current_frame_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(u1, v2);
    glVertex2f(-job->scale_x, -job->scale_y);
    glTexCoord2f(u2, v2);
    glVertex2f(job->scale_x, -job->scale_y);
    glTexCoord2f(u1, v1);
    glVertex2f(-job->scale_x, job->scale_y);
    glTexCoord2f(u2, v1);
    glVertex2f(job->scale_x, job->scale_y);
    glEnd();

    if(job->use_interlaced_render) {
        glDisable(GL_BLEND);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

static void modern_render(render_job_data_t *job)
{
    /* Used when OpenGL 3.2+ is available */

    GLint gl_filter;

    GLuint program;
    GLuint position_attribute;
    GLuint tex_coord_attribute;
    GLuint scale_uniform;
    GLuint view_size_uniform;
    GLuint source_size_uniform;
    GLuint this_frame_uniform;
    GLuint last_frame_uniform;

    /* For shader filters, we start with nearest neighbor. So only use linear if directly requested. */
    gl_filter = job->filter_mode == 1 ?  GL_LINEAR : GL_NEAREST;

    /* Choose the appropriate shader */
    if (job->use_interlaced_render) {
        if (job->filter_mode == 2) {
            program = job->shader_bicubic_interlaced;
        } else {
            program = job->shader_builtin_interlaced;
        }
    } else {
        if (job->filter_mode == 2) {
            program = job->shader_bicubic;
        } else {
            program = job->shader_builtin;
        }
    }

    glUseProgram(program);

    position_attribute  = glGetAttribLocation(program, "position");
    tex_coord_attribute = glGetAttribLocation(program, "tex");
    scale_uniform       = glGetUniformLocation(program, "scale");
    view_size_uniform   = glGetUniformLocation(program, "view_size");
    source_size_uniform = glGetUniformLocation(program, "source_size");
    this_frame_uniform  = glGetUniformLocation(program, "this_frame");

    if (job->use_interlaced_render) {
        last_frame_uniform  = glGetUniformLocation(program, "last_frame");
    }

    glDisable(GL_BLEND);
    glBindVertexArray(job->vao);
    glBindBuffer(GL_ARRAY_BUFFER, job->vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(position_attribute,  4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(tex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)64);

    glUniform4f(scale_uniform, job->scale_x, job->scale_y, 1.0f, 1.0f);
    glUniform2f(view_size_uniform, job->view_width, job->view_height);
    glUniform2f(source_size_uniform, job->frame_width, job->frame_height);

    if (job->use_interlaced_render) {
        glUniform1i(last_frame_uniform, 0);
        glUniform1i(this_frame_uniform, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, job->previous_frame_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, job->current_frame_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    } else {
        glUniform1i(this_frame_uniform, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, job->current_frame_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (job->use_interlaced_render) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glDisableVertexAttribArray(position_attribute);
    glDisableVertexAttribArray(tex_coord_attribute);

    glUseProgram(0);
}

static bool render_job_prepare(vice_opengl_renderer_context_t *context, render_job_data_t *job_data)
{
    /*
     * Lock the context and copy any needed data into render_job_data.
     * The aim is to hold the canvas lock for as short a time as possible.
     */
    
    video_canvas_t *canvas = context->canvas;

    unsigned int native_view_min_width;
    unsigned int native_view_min_height;
    int keepaspect = 1;
    int trueaspect = 0;

    job_data->scale_x               = 1.0f;
    job_data->scale_y               = 1.0f;
    job_data->use_interlaced_render = false;

    resources_get_int("KeepAspectRatio", &keepaspect);
    resources_get_int("TrueAspectRatio", &trueaspect);
    resources_get_int("VSync",           &job_data->vsync_value);
    resources_get_int("GTKFilter",       &job_data->filter_mode);
    
    job_data->backbuffer = render_queue_dequeue_for_display(context->canvas_render_queue);

    CANVAS_LOCK();
    if (context->render_skip) {
        CANVAS_UNLOCK();
        if (job_data->backbuffer) {
            render_queue_return_to_pool(context->canvas_render_queue, job_data->backbuffer);
        }
        return false;
    }
    
    if (job_data->backbuffer) {
        if (job_data->backbuffer->interlace_field != context->current_interlace_field) {
            context->current_interlace_field    = job_data->backbuffer->interlace_field;
            
            /* Swap the texutres for interlaced rendering */
            GLuint swap_texture             = context->current_frame_texture;
            context->current_frame_texture  = context->previous_frame_texture;
            context->previous_frame_texture = swap_texture;
        }
        
        context->current_frame_width        = job_data->backbuffer->width;
        context->current_frame_height       = job_data->backbuffer->height;
        context->current_pixel_aspect_ratio = job_data->backbuffer->pixel_aspect_ratio;
        
        /* TODO: Verifiy that no frame skipping has occured */
        job_data->use_interlaced_render = job_data->backbuffer->interlaced ? true : false;
    }
    
    job_data->view_width   = context->native_view_width;
    job_data->view_height  = context->native_view_height;
    job_data->frame_width  = context->current_frame_width;
    job_data->frame_height = context->current_frame_height;
    
    /*
     * Recalculate layout
     */

    if (keepaspect) {
        float viewport_aspect;
        float emulated_aspect;

        viewport_aspect = (float)job_data->view_width  / (float)job_data->view_height;
        emulated_aspect = (float)job_data->frame_width / (float)job_data->frame_height;

        if (trueaspect) {
            emulated_aspect *= context->current_pixel_aspect_ratio;
        }

        if (emulated_aspect < viewport_aspect) {
            job_data->scale_x = emulated_aspect / viewport_aspect;
            job_data->scale_y = 1.0f;
        } else {
            job_data->scale_x = 1.0f;
            job_data->scale_y = viewport_aspect / emulated_aspect;
        }
    }

    /* Update values used by light pen system */
    canvas->screen_display_w = (float)context->native_view_width  * job_data->scale_x;
    canvas->screen_display_h = (float)context->native_view_height * job_data->scale_y;
    canvas->screen_origin_x = ((float)context->native_view_width  - canvas->screen_display_w) / 2.0;
    canvas->screen_origin_y = ((float)context->native_view_height - canvas->screen_display_h) / 2.0;

    /* Calculate the minimum drawing area size to be enforced by gtk */
    if (keepaspect && trueaspect) {
        native_view_min_width  = ceil((float)job_data->frame_width * context->current_pixel_aspect_ratio);
        native_view_min_height = job_data->frame_height;
    } else {
        native_view_min_width  = job_data->frame_width;
        native_view_min_height = job_data->frame_height;
    }
    
    /* Update the minimum window size if it has changed */
    if (   native_view_min_width  != context->native_view_min_width
        || native_view_min_height != context->native_view_min_height
        ) {
        context->native_view_min_width  = native_view_min_width;
        context->native_view_min_height = native_view_min_height;
        
        archdep_thread_run_on_main(update_minimum_window_size, context);
    }
    
    /* Do we need to switch vsync modes? */
    if (job_data->vsync_value != context->current_vsync_value) {
        context->current_vsync_value = job_data->vsync_value;
        job_data->should_set_vsync = true;
    }
    
    job_data->bg_r = context->native_view_bg_r;
    job_data->bg_g = context->native_view_bg_g;
    job_data->bg_b = context->native_view_bg_b;
    
    job_data->gl_context_is_legacy      = context->gl_context_is_legacy;
    job_data->current_frame_texture     = context->current_frame_texture;
    job_data->previous_frame_texture    = context->previous_frame_texture;
    
    job_data->shader_bicubic_interlaced = context->shader_bicubic_interlaced;
    job_data->shader_builtin_interlaced = context->shader_builtin_interlaced;
    job_data->shader_bicubic            = context->shader_bicubic;
    job_data->shader_builtin            = context->shader_builtin;
    job_data->vao                       = context->vao;
    job_data->vbo                       = context->vbo;
    
    CANVAS_UNLOCK();
    
    return true;
}

static void render_job_execute(vice_opengl_renderer_context_t *context, render_job_data_t *job_data)
{
    if (job_data->backbuffer) {
        video_canvas_render_backbuffer(job_data->backbuffer, job_data->backbuffer->pixel_data, job_data->backbuffer->width * 4);
    }

    RENDER_LOCK();

    vice_opengl_renderer_make_current(context);

    if (job_data->backbuffer) {
        /* Upload the frame(s) to the GPU and then return it */
        update_texture(job_data->current_frame_texture, job_data->backbuffer);
        render_queue_return_to_pool(context->canvas_render_queue, job_data->backbuffer);
    }

    vice_opengl_renderer_set_viewport(context);

    /* Apply vsync if needed */
    if (job_data->should_set_vsync) {
        vice_opengl_renderer_set_vsync(context, job_data->vsync_value ? true : false);
    }

    /* Begin with a cleared framebuffer */
    glClearColor(job_data->bg_r, job_data->bg_g, job_data->bg_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Invoke the appropriate renderer */
    if (job_data->gl_context_is_legacy) {
        legacy_render(job_data);
    } else {
        modern_render(job_data);
    }

    vice_opengl_renderer_present_backbuffer(context);
    vice_opengl_renderer_clear_current(context);

    RENDER_UNLOCK();
}

static void render(void *job_data, void *pool_data)
{
    render_job_t job = (render_job_t)vice_ptr_to_int(job_data);
    video_canvas_t *canvas = pool_data;
    vice_opengl_renderer_context_t *context = (vice_opengl_renderer_context_t *)canvas->renderer_context;
    
    render_job_data_t render_job_data;
    
    if (job == render_thread_init) {
        archdep_thread_init();

#if defined(MACOS_COMPILE)
        vice_macos_set_render_thread_priority();
#elif defined(LINUX_COMPILE)
        /* TODO: Linux thread prio stuff, need root or some 'capability' though */
#else
        /* TODO: BSD thread prio stuff */
#endif

        log_message(LOG_DEFAULT, "Render thread initialised");
        return;
    }

    if (job == render_thread_shutdown) {
        archdep_thread_shutdown();
        log_message(LOG_DEFAULT, "Render thread shutdown");
        return;
    }
    
    if (render_job_prepare(context, &render_job_data)) {
        render_job_execute(context, &render_job_data);
    }
}

static void vice_opengl_set_palette(video_canvas_t *canvas)
{
    int i;
    video_render_color_tables_t *color_tables = &canvas->videoconfig->color_tables;
    struct palette_s *palette = canvas ? canvas->palette : NULL;

    if (!palette) {
        return;
    }

    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        uint32_t color_code = color.red | (color.green << 8) | (color.blue << 16) | (0xffU << 24);
        video_render_setphysicalcolor(canvas->videoconfig, i, color_code);
    }

#ifdef WORDS_BIGENDIAN
    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(color_tables, i, i << 24, i << 16, i << 8);
    }
    video_render_setrawalpha(color_tables, 0xffU);
#else
    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(color_tables, i, i, i << 8, i << 16);
    }
    video_render_setrawalpha(color_tables, 0xffU << 24);
#endif
    video_render_initraw(canvas->videoconfig);
}

/******/

/** \brief Compile a shader.
 *
 *  If the shader cannot be compiled, error messages from OpenGL will
 *  be dumped to stdout.
 *
 *  \param shader_type The kind of shader being compiled. Must be
 *                     either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 *  \param text        The shader source.
 *  \return The identifier of the shader.
 */
static GLuint create_shader(GLenum shader_type, const char *text)
{
    GLuint shader = glCreateShader(shader_type);
    GLint status = 0;
    glShaderSource(shader, 1, &text, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint info_log_length;
        GLchar *info_log;
        const char *shader_type_name = NULL;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log = lib_malloc(sizeof(GLchar) * (info_log_length + 1));
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

        switch(shader_type)
        {
        case GL_VERTEX_SHADER: shader_type_name = "vertex"; break;
        case GL_FRAGMENT_SHADER: shader_type_name = "fragment"; break;
        default: shader_type_name = "unknown"; break;
        }

        log_error(LOG_DEFAULT, "Compile failure in %s shader:\n%s\n", shader_type_name, info_log);
        lib_free(info_log);

        archdep_vice_exit(1);
    }

    return shader;
}

/** \brief Compile and return a gl program for the given vertext and fragment shader files.
 */
static GLuint create_shader_program(char *vertex_shader_filename, char *fragment_shader_filename)
{
    char *vertex_shader;
    char *fragment_shader;
    GLuint program;
    GLuint vert;
    GLuint frag;
    GLint status;
    FILE *fd;
    char *path;

    /* Load the vertex shader */

    fd = sysfile_open(vertex_shader_filename, "GLSL", &path, "rb");
    if (fd == NULL) {
        log_error(LOG_DEFAULT, "Could not open vertex shader: %s", vertex_shader_filename);
        archdep_vice_exit(1);
    }

    log_message(LOG_DEFAULT, "Loading vertex shader: %s", path);

    if (util_file_load_string(fd, &vertex_shader)) {
        log_error(LOG_DEFAULT, "Could not read vertex shader: %s", path);
        fclose(fd);
        lib_free(path);
        archdep_vice_exit(1);
    }
    fclose(fd);
    lib_free(path);

    /* Load the fragment shader */

    fd = sysfile_open(fragment_shader_filename, "GLSL", &path, "rb");
    if (fd == NULL) {
        log_error(LOG_DEFAULT, "Could not open fragment shader: %s", fragment_shader_filename);
        archdep_vice_exit(1);
    }

    log_message(LOG_DEFAULT, "Loading fragment shader: %s", path);

    if (util_file_load_string(fd, &fragment_shader)) {
        log_error(LOG_DEFAULT, "Could not read fragment shader: %s", path);
        fclose(fd);
        lib_free(path);
        lib_free(vertex_shader);
        archdep_vice_exit(1);
    }
    fclose(fd);
    lib_free(path);

    vert = create_shader(GL_VERTEX_SHADER, vertex_shader);
    frag = create_shader(GL_FRAGMENT_SHADER, fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        GLchar *info_log;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log = lib_malloc(sizeof(GLchar) * (info_log_length + 1));
        glGetProgramInfoLog(program, info_log_length, NULL, info_log);
        log_error(LOG_DEFAULT, "Linker failure: %s\n", info_log);
        lib_free(info_log);
        lib_free(fragment_shader);
        lib_free(vertex_shader);
        archdep_vice_exit(1);
    }

    glDeleteShader(frag);
    glDeleteShader(vert);
    lib_free(fragment_shader);
    lib_free(vertex_shader);

    return program;
}

/******/

vice_renderer_backend_t vice_opengl_backend = {
    vice_opengl_initialise_canvas,
    NULL, /* vice_opengl_update_context, */
    vice_opengl_destroy_context,
    vice_opengl_on_new_backbuffer,
    vice_opengl_on_ui_frame_clock,
    vice_opengl_set_palette
};
