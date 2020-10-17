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

#ifdef HAVE_GTK3_OPENGL

#include "opengl_renderer.h"

#include <assert.h>
#include <gtk/gtk.h>
#include <math.h>

#include "lib.h"
#include "log.h"
#include "palette.h"
#include "render_queue.h"
#include "resources.h"
#include "ui.h"
#include "vsync.h"
#include "vsyncapi.h"

#define CANVAS_LOCK() pthread_mutex_lock(&canvas->lock)
#define CANVAS_UNLOCK() pthread_mutex_unlock(&canvas->lock)
#define RENDER_LOCK() pthread_mutex_lock(&context->render_lock)
#define RENDER_UNLOCK() pthread_mutex_unlock(&context->render_lock)

typedef vice_opengl_renderer_context_t context_t;

static void on_widget_realized(GtkWidget *widget, gpointer data);
static void on_widget_unrealized(GtkWidget *widget, gpointer data);
static void on_widget_resized(GtkWidget *widget, GdkRectangle *allocation, gpointer data);
static void on_widget_monitors_changed(GdkScreen *screen, gpointer data);
static void render(void *job_data, void *pool_data);

static GLuint create_shader(GLenum shader_type, const char *text);
static void create_shader_program(context_t *context);

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

/** \brief Our renderer's vertex shader.
 *
 * This simply scales the geometry it is provided and provides
 * smoothly interpolated texture coordinates between each vertex. The
 * world coordinates remain [-1, 1] in all dimensions. */
static const char *vertexShader =
    "#version 150\n"
    "uniform vec4 scale;\n"
    "uniform vec2 validTex;\n"
    "uniform vec2 texSize;\n"
    "in vec4 position;\n"
    "in vec2 tex;\n"
    "smooth out vec2 texCoord;\n"
    "void main() {\n"
    "  gl_Position = position * scale;\n"
    "  texCoord = (tex * (validTex - 1.0) + 0.5) / texSize;\n"
    "}\n";

/** \brief Our renderer's fragment shader.
 *
 * This does nothing but texture lookups based on the values fed to it
 * by the vertex shader. */
static const char *fragmentShader =
    "#version 150\n"
    "uniform sampler2D sampler;\n"
    "smooth in vec2 texCoord;\n"
    "out vec4 outputColor;\n"
    "void main() { outputColor = texture(sampler, texCoord); }\n";

/**/

static void vice_opengl_initialise_canvas(video_canvas_t *canvas)
{
    context_t *context;

    /* First initialise the context_t that we'll need everywhere */
    context = lib_calloc(1, sizeof(context_t));
    
    context->canvas_lock = canvas->lock;
    pthread_mutex_init(&context->render_lock, NULL);
    context->render_queue = render_queue_create();
    
    canvas->renderer_context = context;

    g_signal_connect(canvas->event_box, "realize", G_CALLBACK (on_widget_realized), canvas);
    g_signal_connect(canvas->event_box, "unrealize", G_CALLBACK (on_widget_unrealized), canvas);
    g_signal_connect_unlocked(canvas->event_box, "size-allocate", G_CALLBACK(on_widget_resized), canvas);
}

static void vice_opengl_destroy_context(video_canvas_t *canvas)
{
    context_t *context;
    
    CANVAS_LOCK();
    
    context = canvas->renderer_context;
    
    /* Release all backbuffers on the render queue and delloc it */
    render_queue_destroy(context->render_queue);
    context->render_queue = NULL;

    pthread_mutex_destroy(&context->render_lock);
    
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

#ifdef MACOSX_SUPPORT
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
        create_shader_program(context);
        glGenBuffers(1, &context->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glGenVertexArrays(1, &context->vao);
    }

    glGenTextures(1, &context->texture);
    
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

#ifdef MACOSX_SUPPORT
    gtk_widget_translate_coordinates(widget, gtk_widget_get_toplevel(widget), 0, 0, &context->native_view_x, &context->native_view_y);
#endif
    
    context->native_view_width = allocation->width;
    context->native_view_height = allocation->height;

    gtk_scale = gtk_widget_get_scale_factor(widget);
    context->gl_backing_layer_width     = context->native_view_width    * gtk_scale;
    context->gl_backing_layer_height    = context->native_view_height   * gtk_scale;
    
    /* Set the background colour */
    if (ui_is_fullscreen()) {
        context->native_view_bg_r = 0.0f;
        context->native_view_bg_g = 0.0f;
        context->native_view_bg_b = 0.0f;
    } else {
        context->native_view_bg_r = 0.5f;
        context->native_view_bg_g = 0.5f;
        context->native_view_bg_b = 0.5f;
    }

    /* Update the size of the native child window to match the gtk drawing area */
    vice_opengl_renderer_resize_child_view(context);

    CANVAS_UNLOCK();
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

/******/

/** \brief The emulated screen size or aspect ratio has changed */
static void vice_opengl_update_context(video_canvas_t *canvas, unsigned int width, unsigned int height)
{
    context_t *context;

    CANVAS_LOCK();

    context = canvas->renderer_context;

    context->emulated_width_next = width;
    context->emulated_height_next = height;
    context->pixel_aspect_ratio_next = canvas->geometry->pixel_aspect_ratio;
    
    CANVAS_UNLOCK();
}

/** \brief It's time to draw a complete emulated frame */
static void vice_opengl_refresh_rect(video_canvas_t *canvas,
                                     unsigned int xs, unsigned int ys,
                                     unsigned int xi, unsigned int yi,
                                     unsigned int w, unsigned int h)
{
    context_t *context;
    backbuffer_t *backbuffer;
    int pixel_data_size_bytes;
    
    CANVAS_LOCK();
    
    context = canvas->renderer_context;
    if (!context || !context->render_queue) {
        CANVAS_UNLOCK();
        return;
    }

    /* Obtain an unused backbuffer to render to */
    pixel_data_size_bytes = context->emulated_width_next * context->emulated_height_next * 4;
    backbuffer = render_queue_get_from_pool(context->render_queue, pixel_data_size_bytes);

    if (!backbuffer) {
        CANVAS_UNLOCK();
        return;
    }

    backbuffer->width = context->emulated_width_next;
    backbuffer->height = context->emulated_height_next;
    backbuffer->pixel_aspect_ratio = context->pixel_aspect_ratio_next;

    CANVAS_UNLOCK();

    video_canvas_render(canvas, backbuffer->pixel_data, w, h, xs, ys, xi, yi, backbuffer->width * 4, 32);

    CANVAS_LOCK();
    if (context->render_thread) {
        render_queue_enqueue_for_display(context->render_queue, backbuffer);
        render_thread_push_job(context->render_thread, context);
    } else {
        /* Thread no longer running, probably shutting down */
        render_queue_return_to_pool(context->render_queue, backbuffer);
    }
    CANVAS_UNLOCK();
}

static void vice_opengl_on_ui_frame_clock(GdkFrameClock *clock, video_canvas_t *canvas)
{
    context_t *context = canvas->renderer_context;
    
    ui_update_statusbars();

    CANVAS_LOCK();

    /* TODO we really shouldn't be setting this every frame! */
    gtk_widget_set_size_request(canvas->event_box, context->native_view_min_width, context->native_view_min_height);

    /*
     * Sometimes the OS wants to redraw part of the window. Haven't been able to
     * reliably detect those events in some linux environments so we don't trigger
     * a redraw on that sort of event. It's not as simple as catching the GTK draw
     * signal, because we have our own native Xlib window/NSView added over the top
     * of the GTK/GDK window.
     * 
     * So, each GdkFrameClock event, check if we have rendered since the last
     * event. If we have not, queue up a refresh of the existing emu frame.
     * 
     * This ensures that resizing while paused doesn't glitch like busy win95,
     * and also fixes various issues on some crappy X11 setups :)
     */

    if (context->last_render_time < context->last_host_frame_time) {
        if (context->render_thread) {
            render_thread_push_job(context->render_thread, context);
        }
    }

    context->last_host_frame_time = vsyncarch_gettime();

    CANVAS_UNLOCK();
}

static void render(void *job_data, void *pool_data)
{
    video_canvas_t *canvas = pool_data;
    vice_opengl_renderer_context_t *context = job_data;
    backbuffer_t *backbuffer;
    unsigned int backbuffer_width;
    unsigned int backbuffer_height;
    float backbuffer_pixel_aspect_ratio;

    backbuffer = render_queue_dequeue_for_display(context->render_queue);
    
    CANVAS_LOCK();

    if (backbuffer) {
        backbuffer_width                = backbuffer->width;
        backbuffer_height               = backbuffer->height;
        backbuffer_pixel_aspect_ratio   = backbuffer->pixel_aspect_ratio;

        /* cache this backbuffer size for use during resize */
        context->emulated_width_last_rendered       = backbuffer_width;
        context->emulated_height_last_rendered      = backbuffer_height;
        context->pixel_aspect_ratio_last_rendered   = backbuffer_pixel_aspect_ratio;
        context->last_render_time                   = vsyncarch_gettime();
    } else {
        /* Use the last rendered frame size and ratio for layout */
        backbuffer_width                = context->emulated_width_last_rendered;
        backbuffer_height               = context->emulated_height_last_rendered;
        backbuffer_pixel_aspect_ratio   = context->pixel_aspect_ratio_last_rendered;
    }

    /* This happens during startup, before an initial frame is rendered */
    if (!backbuffer_width || !backbuffer_height) {
        CANVAS_UNLOCK();

        if (backbuffer) {
            render_queue_return_to_pool(context->render_queue, backbuffer);
        }

        return;
    }
    
    /* Recalculate layout */
    int keepaspect = 1;
    int trueaspect = 0;
    float scale_x;
    float scale_y;

    resources_get_int("KeepAspectRatio", &keepaspect);
    resources_get_int("TrueAspectRatio", &trueaspect);

    if (keepaspect) {
        float viewport_aspect;
        float emulated_aspect;       

        viewport_aspect = (float)context->native_view_width / (float)context->native_view_height;
        emulated_aspect = (float)backbuffer_width / (float)backbuffer_height;

        if (trueaspect) {
            emulated_aspect *= backbuffer_pixel_aspect_ratio;
        }

        if (emulated_aspect < viewport_aspect) {
            scale_x = emulated_aspect / viewport_aspect;
            scale_y = 1.0f;
        } else {
            scale_x = 1.0f;
            scale_y = viewport_aspect / emulated_aspect;
        }
    } else {
        scale_x = 1.0f;
        scale_y = 1.0f;
    }

    canvas->screen_display_w = (float)context->native_view_width  * scale_x;
    canvas->screen_display_h = (float)context->native_view_height * scale_y;
    canvas->screen_origin_x = ((float)context->native_view_width  - canvas->screen_display_w) / 2.0;
    canvas->screen_origin_y = ((float)context->native_view_height - canvas->screen_display_h) / 2.0;

    /* Calculate the minimum drawing area size to be enforced by gtk */
    if (keepaspect && trueaspect) {
        context->native_view_min_width = ceil((float)backbuffer_width * backbuffer_pixel_aspect_ratio);
        context->native_view_min_height = backbuffer_height;
    } else {
        context->native_view_min_width = backbuffer_width;
        context->native_view_min_height = backbuffer_height;
    }

    RENDER_LOCK();
    
    CANVAS_UNLOCK();

    int filter = 1;
    resources_get_int("GTKFilter", &filter);

    vice_opengl_renderer_make_current(context);
    vice_opengl_renderer_set_viewport(context);

    glClearColor(context->native_view_bg_r, context->native_view_bg_g, context->native_view_bg_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Update the OpenGL texture with the new backbuffer bitmap */
    if (backbuffer) {
        glBindTexture(GL_TEXTURE_2D, context->texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, backbuffer_width);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, backbuffer_width, backbuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, backbuffer->pixel_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (context->gl_context_is_legacy) {
        /* Legacy renderer */
        float u1;
        float v1;
        float u2;
        float v2;
        
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, context->texture);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        
        /* TODO: Figure out if half-pixel correction in unused_opengl_renderer.c still needed */
        u1 = 0.0f;
        u2 = 1.0f;
        v1 = 0.0f;
        v2 = 1.0f;

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(u1, v2);
        glVertex2f(-scale_x, -scale_y);
        glTexCoord2f(u2, v2);
        glVertex2f(scale_x, -scale_y);
        glTexCoord2f(u1, v1);
        glVertex2f(-scale_x, scale_y);
        glTexCoord2f(u2, v1);
        glVertex2f(scale_x, scale_y);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        /* Modern renderer */
        GLuint scale_uniform, valid_tex_uniform, tex_size_uniform;
        GLuint sampler_uniform;
        
        glUseProgram(context->program);
        
        glBindVertexArray(context->vao);
        glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(context->position_index, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(context->tex_coord_index, 2, GL_FLOAT, GL_FALSE, 0, (void*)64);
        
        /** \todo cache the uniform locations along with the vertex attributes */
        scale_uniform = glGetUniformLocation(context->program, "scale");
        valid_tex_uniform = glGetUniformLocation(context->program, "validTex");
        tex_size_uniform = glGetUniformLocation(context->program, "texSize");
        sampler_uniform = glGetUniformLocation(context->program, "sampler");

        glUniform4f(scale_uniform, scale_x, scale_y, 1.0f, 1.0f);
        glUniform2f(valid_tex_uniform, context->native_view_width, context->native_view_height);
        glUniform2f(tex_size_uniform, context->native_view_width, context->native_view_height);
        glUniform1i(sampler_uniform, 0);
        
        glBindTexture(GL_TEXTURE_2D, context->texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glDisableVertexAttribArray(context->position_index);
        glDisableVertexAttribArray(context->tex_coord_index);
        glUseProgram(0);
    }

    /*
     * Sync to monitor refresh when the emulated screen has changed,
     * but don't if we are just redrawing the same content. This keeps
     * resize nice and smooth.
     */

    if (backbuffer) {
        int vsync = 1;
        resources_get_int("VSync", &vsync);
        vice_opengl_renderer_set_vsync(context, vsync ? true : false);
    } else {
        vice_opengl_renderer_set_vsync(context, false);
    }

    /*
     * A glFlush() alone seems to work, however after the app has been in the background for a while,
     * you see a bunch of old frames very quickly rendered in a 'catch up'. This seems to prevent that.
     * 
     * Also, using glFlush() results in incorrect transition to fullscreen when paused (X11).
     */
    
    vice_opengl_renderer_present_backbuffer(context);

    vice_opengl_renderer_clear_current(context);
    
    RENDER_UNLOCK();

    if (backbuffer) {
        render_queue_return_to_pool(context->render_queue, backbuffer);
    }
}

static void vice_opengl_set_palette(video_canvas_t *canvas)
{
    int i;
    struct palette_s *palette = canvas ? canvas->palette : NULL;
    if (!palette) {
        return;
    }
    
    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        uint32_t color_code = color.red | (color.green << 8) | (color.blue << 16) | (0xffU << 24);
        video_render_setphysicalcolor(canvas->videoconfig, i, color_code, 32);
    }

    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(i, i, i << 8, i << 16);
    }
    video_render_setrawalpha(0xffU << 24);
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
    }

    return shader;
}

/** \brief Compile and link the renderer's shaders.
 *
 *  If successful, the vice_opengl_renderer_context_s::program,
 *  vice_opengl_renderer_context_s::position_index, and
 *  vice_opengl_renderer_context_s::tex_coord_index fields will be
 *  filled in with values for future use.
 *
 *  \param ctx The renderer context that will receive the results.
 */
static void create_shader_program(context_t *context)
{
    GLuint program = glCreateProgram();
    GLuint vert = create_shader(GL_VERTEX_SHADER, vertexShader);
    GLuint frag = create_shader(GL_FRAGMENT_SHADER, fragmentShader);
    GLint status;

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
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    context->position_index = glGetAttribLocation(program, "position");
    context->tex_coord_index = glGetAttribLocation(program, "tex");
    context->program = program;
}

/******/

vice_renderer_backend_t vice_opengl_backend = {
    vice_opengl_initialise_canvas,
    vice_opengl_update_context,
    vice_opengl_destroy_context,
    vice_opengl_refresh_rect,
    vice_opengl_on_ui_frame_clock,
    vice_opengl_set_palette
};

#endif
