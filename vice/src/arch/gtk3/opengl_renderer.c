/** \file   src/arch/gtk3/opengl_renderer.c
 * \brief   OpenGL-based renderer for the GTK3 backend.
 *
 *  Michael C. Martin <mcmartin@gmail.com>
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

#include "opengl_renderer.h"

#ifdef HAVE_GTK3_OPENGL

#include <string.h>

#define GL_GLEXT_PROTOTYPES
#ifdef MACOSX_SUPPORT
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "lib.h"
#include "resources.h"
#include "ui.h"
#include "video.h"

typedef struct vice_opengl_renderer_context_s {
    GLuint program, position_index, tex_coord_index;
    GLuint vbo, vao;
    unsigned int width, height;
} context_t;

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

static const char *vertexShader = "#version 150\n"
    "uniform vec4 scale;\n"
    "in vec4 position;\n"
    "in vec2 tex;\n"
    "smooth out vec2 texCoord;\n"
    "void main() {\n"
    "  gl_Position = position * scale;\n"
    "  texCoord = tex;\n"
    "}\n";

static const char *fragmentShader = "#version 150\n"
    "uniform sampler2D sampler;\n"
    "smooth in vec2 texCoord;\n"
    "out vec4 outputColor;\n"
    "void main() {outputColor.rg = texCoord.xy;outputColor.b = 0;}\n";
/*
    The above shader just draws a gradient. Using the sampler will use this:

    "void main() { outputColor = texture2D(sampler, texCoord); }\n";
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

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", shader_type_name, info_log);
        lib_free(info_log);
    }
    
    return shader;
}    

static void create_shader_program(context_t *ctx)
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
        fprintf(stderr, "Linker failure: %s\n", info_log);
        lib_free(info_log);
    }
    
    glDeleteShader(vert);
    glDeleteShader(frag);
    ctx->position_index = glGetAttribLocation(program, "position");
    ctx->tex_coord_index = glGetAttribLocation(program, "tex");
    ctx->program = program;
}

static void realize_opengl_cb (GtkGLArea *area, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;
    context_t *ctx = NULL;
    
    gtk_gl_area_make_current(area);
    if (gtk_gl_area_get_error(area) != NULL) {
        return;
    }
    if (canvas->renderer_context) {
        fprintf(stderr, "WARNING: Re-realizing the GtkGL area! This will leak.\n");
    }
    ctx = lib_malloc(sizeof(context_t));
    memset(ctx, 0, sizeof(context_t));
    canvas->renderer_context = ctx;
    create_shader_program(ctx);
    glGenBuffers(1, &ctx->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(1, &ctx->vao);
    /*
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
    */
}

static gboolean render_opengl_cb (GtkGLArea *area, GdkGLContext *unused, gpointer data)
{
    video_canvas_t *canvas = data;
    context_t *ctx = canvas ? (context_t *)canvas->renderer_context : NULL;

    if (ui_is_fullscreen()) {
        glClearColor (0.0f,0.0f,0.0f,1.0f);
    } else {
        glClearColor (0.5f,0.5f,0.5f,1.0f);
    }
    glClear (GL_COLOR_BUFFER_BIT);

    if (ctx && ctx->program) {
        glUseProgram(ctx->program);

        glBindVertexArray(ctx->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(ctx->position_index, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(ctx->tex_coord_index, 2, GL_FLOAT, GL_FALSE, 0, (void*)64);
        /* TODO
        glBindTexture(GL_TEXTURE_2D, texture);
        */

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(ctx->position_index);
        glDisableVertexAttribArray(ctx->tex_coord_index);
        glUseProgram(0);
    }

    return TRUE;
}

static void
resize_opengl_cb (GtkGLArea *area, gint width, gint height, gpointer user_data)
{
    GLuint scale_uniform, sampler_uniform;
    float scale_x = 1.0f, scale_y = 1.0f;
    video_canvas_t *canvas = (video_canvas_t *)user_data;
    context_t *ctx = canvas ? (context_t *)canvas->renderer_context : NULL;
    int keepaspect = 1, trueaspect = 0;
    if (!ctx || ctx->width == 0 || ctx->height == 0) {
        return;
    }
        
    if (width <= 0) {
        width = 1;
    }
    if (height <= 0) {
        height = 1;
    }

    resources_get_int("KeepAspectRatio", &keepaspect);
    resources_get_int("TrueAspectRatio", &trueaspect);
    
    if (keepaspect) {
        float canvas_aspect, viewport_aspect;

        viewport_aspect = (float)width / (float)height;
        canvas_aspect = (float)ctx->width / (float)ctx->height;
        if (trueaspect) {
            canvas_aspect *= canvas->geometry->pixel_aspect_ratio;
        }
        if (canvas_aspect < viewport_aspect) {
            scale_x = canvas_aspect / viewport_aspect;
        } else {
            scale_y = viewport_aspect / canvas_aspect;
        }
    }

    glUseProgram(ctx->program);
    scale_uniform = glGetUniformLocation(ctx->program, "scale");
    glUniform4f(scale_uniform, scale_x, scale_y, 1.0f, 1.0f);
    sampler_uniform = glGetUniformLocation(ctx->program, "sampler");
    glUniform1i(sampler_uniform, 0);
    glUseProgram(0);
}

static GtkWidget *vice_opengl_create_widget(video_canvas_t *canvas)
{
    GtkWidget *widget = gtk_gl_area_new();
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_vexpand(widget, TRUE);
    canvas->drawing_area = widget;
    canvas->renderer_context = NULL;
    g_signal_connect (widget, "realize", G_CALLBACK (realize_opengl_cb), canvas);
    g_signal_connect (widget, "render", G_CALLBACK (render_opengl_cb), canvas);
    g_signal_connect (widget, "resize", G_CALLBACK (resize_opengl_cb), canvas);   
    return widget;
}

static void vice_opengl_destroy_context(video_canvas_t *canvas)
{
    if (canvas) {
        context_t *ctx = (context_t *)canvas->renderer_context;
        if (!ctx) {
            return;
        }
        lib_free(ctx);
        canvas->renderer_context = NULL;
    }
}

static int vice_opengl_get_backbuffer_info(video_canvas_t *canvas, unsigned int *width, unsigned int *height, unsigned int *stride)
{
    context_t *ctx = canvas ? (context_t *)canvas->renderer_context : NULL;
    if (!ctx || ctx->width == 0 || ctx->height == 0) {
        return 1;
    }
    if (width) {
        *width = ctx->width;
    }
    if (height) {
        *height = ctx->height;
    }
    if (stride) {
        *stride = ctx->width * 4;
    }
    return 0;
}

static void vice_opengl_update_context(video_canvas_t *canvas, unsigned int width, unsigned int height)
{
    context_t *ctx = canvas ? (context_t *)canvas->renderer_context : NULL;
    if (ctx) {
        double aspect = 1.0;
        int keepaspect = 1, trueaspect = 0;
        gint widget_width, widget_height;
        ctx->width = width;
        ctx->height = height;
        resources_get_int("KeepAspectRatio", &keepaspect);
        resources_get_int("TrueAspectRatio", &trueaspect);
        if (keepaspect && trueaspect) {
            aspect = canvas->geometry->pixel_aspect_ratio;
        }

        /* Configure the matrix to fit it in the widget as it exists */
        widget_width = gtk_widget_get_allocated_width(canvas->drawing_area);
        widget_height = gtk_widget_get_allocated_height(canvas->drawing_area);
        resize_opengl_cb(GTK_GL_AREA(canvas->drawing_area), widget_width, widget_height, canvas);

        /* Fix the widget's size request */
        gtk_widget_set_size_request(canvas->drawing_area, width * aspect, height);
    }
}

static void vice_opengl_refresh_rect(video_canvas_t *canvas,
                                     unsigned int xs, unsigned int ys,
                                     unsigned int xi, unsigned int yi,
                                     unsigned int w, unsigned int h)
{
    /* INCOMPLETE */
    gtk_widget_queue_draw(canvas->drawing_area);    
}

static void vice_opengl_set_palette(video_canvas_t *canvas)
{
    int i;
    struct palette_s *palette = canvas ? canvas->palette : NULL;
    if (!palette) {
        return;
    }
    /* If we get this far we know canvas is also non-NULL */
    
    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        uint32_t color_code = color.red | (color.green << 8) | (color.blue << 16) | (0xff << 24);
        video_render_setphysicalcolor(canvas->videoconfig, i, color_code, 32);
    }

    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(i, i, i << 8, i << 16);
    }
    video_render_setrawalpha(255 << 24);
    video_render_initraw(canvas->videoconfig);
}

static int vice_opengl_backend_init(void)
{
    /* This *should* be a test where we try to initialize everything
     * and do their version check, but for now we just let it slide if
     * we could compile it in the first place */
    /* FOR FUTURE WORK: This probably ends up being the wrong place
     * for checking this. We can't actually tell that OpenGL will or
     * won't work until the widget is put in place and realized. We
     * need a mechanism for shifting backends *dynamically* without
     * breaking. */
    return 1;
}

static void vice_opengl_backend_shutdown(void)
{
}

vice_renderer_backend_t vice_opengl_backend = {
    vice_opengl_backend_init,
    vice_opengl_backend_shutdown,
    vice_opengl_create_widget,
    vice_opengl_update_context,
    vice_opengl_destroy_context,
    vice_opengl_get_backbuffer_info,
    vice_opengl_refresh_rect,
    vice_opengl_set_palette
};

/* Not yet */
/*
vice_renderer_backend_t *vice_renderer_backend = &vice_opengl_backend;
*/

#endif
