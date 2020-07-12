/** \file   opengl_renderer_unix.c
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
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

#include "vice.h"

#ifdef HAVE_GTK3_OPENGL

#include "opengl_renderer.h"

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "render_queue.h"

#define CANVAS_LOCK() pthread_mutex_lock(&context->canvas_lock)
#define CANVAS_UNLOCK() pthread_mutex_unlock(&context->canvas_lock)
#define RENDER_LOCK() pthread_mutex_lock(&context->render_lock)
#define RENDER_UNLOCK() pthread_mutex_unlock(&context->render_lock)

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

static bool isExtensionSupported(const char *extList, const char *extension);

/**/

void vice_opengl_renderer_make_current(vice_opengl_renderer_context_t *context)
{
    glXMakeCurrent(context->x_display, context->x_overlay_window, context->gl_context);
}

void vice_opengl_renderer_set_viewport(vice_opengl_renderer_context_t *context)
{
    glViewport(0, 0, context->gl_backing_layer_width, context->gl_backing_layer_height);
}

void vice_opengl_renderer_present_backbuffer(vice_opengl_renderer_context_t *context)
{
    glXSwapBuffers(context->x_display, context->x_overlay_window);
}

void vice_opengl_renderer_clear_current(vice_opengl_renderer_context_t *context)
{
    glXMakeCurrent(context->x_display, 0, NULL);
}

/**/

void vice_opengl_renderer_create_child_view(GtkWidget *widget, vice_opengl_renderer_context_t *context)
{
    /*
     * OpenGL context initialisation adapted from
     * https://www.khronos.org/opengl/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
     */

    /* use the same connection to the x server that GDK is using */
    context->x_display = GDK_WINDOW_XDISPLAY(gtk_widget_get_window(widget));

    /* we need to create a child window compatible with the type of OpenGL stuff we want */
    static int visual_attribs[] =
        {
            GLX_X_RENDERABLE    , True,
            GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
            GLX_RED_SIZE        , 8,
            GLX_GREEN_SIZE      , 8,
            GLX_BLUE_SIZE       , 8,
            GLX_ALPHA_SIZE      , 8,
            GLX_DOUBLEBUFFER    , True,
            None
        };

    int glx_major;
    int glx_minor;

    if (!glXQueryVersion(context->x_display, &glx_major, &glx_minor)) {
        printf("Failed to get GLX version\n");
        exit(1);
    }

    printf("GLX version: %d.%d\n", glx_major, glx_minor);

    // FBConfigs were added in GLX version 1.3.
    if (glx_major < 1 || (glx_major == 1 && glx_minor < 3)) {
        printf("At least GLX 1.3 is required\n");
        exit(1);
    }

    printf( "Getting matching framebuffer configs\n" );
    int fbcount;
    PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddressARB((const GLubyte *)"glXChooseFBConfig");
    GLXFBConfig *framebuffer_configs = glXChooseFBConfig(context->x_display, DefaultScreen(context->x_display), visual_attribs, &fbcount);
    if (!framebuffer_configs) {
        printf( "Failed to retrieve a framebuffer config\n");
        exit(1);
    }
    printf("Found %d matching FB configs.\n", fbcount);

    /* Just pick the first one I guess. */
    GLXFBConfig framebuffer_config = framebuffer_configs[0];
    XFree(framebuffer_configs);

    // Get a visual
    PFNGLXGETVISUALFROMFBCONFIGPROC glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)glXGetProcAddressARB((const GLubyte *)"glXGetVisualFromFBConfig");
    XVisualInfo *x_visual = glXGetVisualFromFBConfig(context->x_display, framebuffer_config);
    
    XSetWindowAttributes x_set_window_attributes;

    x_set_window_attributes.colormap =
        XCreateColormap(
            context->x_display,
            GDK_WINDOW_XID(gtk_widget_get_window(widget)),
            x_visual->visual,
            AllocNone);
    x_set_window_attributes.background_pixmap = None ;
    x_set_window_attributes.border_pixel      = 0;
    x_set_window_attributes.event_mask        = 0;

    context->x_overlay_window =
        XCreateWindow(
            context->x_display,
            GDK_WINDOW_XID(gtk_widget_get_window(widget)),
            context->native_view_x, context->native_view_y, context->gl_backing_layer_width, context->gl_backing_layer_height,
            0,
            x_visual->depth,
            InputOutput,
            x_visual->visual,
            CWColormap | CWBackPixmap | CWBorderPixel | CWEventMask,
            &x_set_window_attributes);
    
    XMapWindow(context->x_display, context->x_overlay_window);

    // Done with the visual info data
    XFree(x_visual);

    // Get the screen's GLX extension list
    const char *glx_extensions = glXQueryExtensionsString(context->x_display, DefaultScreen(context->x_display));

    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if (!isExtensionSupported(glx_extensions, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
        /* Legact context -- TODO, actually support using this */
        PFNGLXCREATENEWCONTEXTPROC glXCreateNewContext = (PFNGLXCREATENEWCONTEXTPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateNewContext");
        context->gl_context = glXCreateNewContext(context->x_display, framebuffer_config, GLX_RGBA_TYPE, NULL, True);
    }
    else
    {
        int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 2,
                None
            };
        
        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
        context->gl_context = glXCreateContextAttribsARB(context->x_display, framebuffer_config, NULL, True, context_attribs);

        // Sync to ensure any errors generated are processed.
        XSync(context->x_display, False);

        if (context->gl_context) {
            printf( "Created OpenGL 3.2 context\n" );
        }
        else
        {
            printf( "Failed to obtain an OpenGL 3.0 context, requesting a legacy context\n" );

            /*
             * Couldn't create GL 3.2 context.  Fall back to old-style 2.x context.
             * When a context version below 3.0 is requested, implementations will
             * return the newest context version compatible with OpenGL versions less
             * than version 3.0.
             */

            context_attribs[1] = 1;
            context_attribs[3] = 0;

            context->gl_context = glXCreateContextAttribsARB(context->x_display, framebuffer_config, NULL, True, context_attribs);
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync(context->x_display, False);

    // Restore the original error handler
    // XSetErrorHandler( oldHandler );

    // Verifying that context is a direct context
    if (!glXIsDirect(context->x_display, context->gl_context)) {
        printf("Indirect GLX rendering context obtained\n");
    } else {
        printf("Direct GLX rendering context obtained\n");
    }

    vice_opengl_renderer_make_current(context);

    /* make sure OpenGL extension pointers are loaded for the general renderer code */
    glewInit();

    /* Enable vsync */
    if (GLX_EXT_swap_control) {
        GLint gl_int = 1;
        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
        glXSwapIntervalEXT(context->x_display, glXGetCurrentDrawable(), gl_int);
    }

    vice_opengl_renderer_clear_current(context);
}

void vice_opengl_renderer_resize_child_view(vice_opengl_renderer_context_t *context)
{
    if (!context->x_overlay_window) {
        return;
    }

    RENDER_LOCK();

    XMoveResizeWindow(
        context->x_display,
        context->x_overlay_window,
        context->native_view_x,
        context->native_view_y,
        context->gl_backing_layer_width,
        context->gl_backing_layer_height);
    
    RENDER_UNLOCK();
}

void vice_opengl_renderer_destroy_child_view(vice_opengl_renderer_context_t *context)
{
    // TODO!
}

static bool isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0') {
        return false;
    }

    /* It takes a bit of care to be fool-proof about parsing the
    OpenGL extensions string. Don't be fooled by sub-strings,
    etc. */
    for (start=extList;;) {
        where = strstr(start, extension);

        if (!where) {
            break;
        }

        terminator = where + strlen(extension);

        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }

    return false;
}

#endif /* #ifdef HAVE_GTK3_OPENGL */
