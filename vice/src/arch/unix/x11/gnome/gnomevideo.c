/*
 * ui.c - Gnome based graphical user interface.  
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
 *  Heavily derived from Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
 *  GTK+ port.
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

#include "vice.h"

#include <string.h>
#include <stdlib.h>

#include "fullscreenarch.h"
#include "log.h"
#include "types.h"
#include "videoarch.h"
#include "video.h"
#include "machine.h"
#include "lib.h"

#include "ui.h"
#include "uiarch.h"
#include "uicolor.h"
#ifdef HAVE_OPENGL_SYNC
#include "openGL_sync.h"
#endif

static log_t gnomevideo_log = LOG_ERR;

int video_arch_resources_init(void)
{
#ifdef HAVE_OPENGL_SYNC
    openGL_register_resources();
#endif
    return 0;
}

void video_arch_resources_shutdown(void)
{
}

int video_init(void)
{
    if (gnomevideo_log == LOG_ERR)
        gnomevideo_log = log_open("GnomeVideo");

    return 0;
}

void video_shutdown(void)
{
}

int video_init_cmdline_options(void)
{
    return 0;
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->video_draw_buffer_callback = NULL;

#ifdef HAVE_FULLSCREEN
    canvas->fullscreenconfig
        = (fullscreenconfig_t *)lib_calloc(1, sizeof(fullscreenconfig_t));
    fullscreen_init_alloc_hooks(canvas);
#endif
}


video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width,
                                    unsigned int *height, int mapped)
{
    int res;

    canvas->gdk_image = NULL;
#ifdef HAVE_HWSCALE
    canvas->hwscale_image = NULL;
#endif

    res = ui_open_canvas_window(canvas, canvas->viewport->title,
				*width, *height, 1);
    if (res < 0) {
        return NULL;
    }

#ifdef HAVE_OPENGL_SYNC
    openGL_sync_init(canvas);
#endif

    return canvas;
}

void video_canvas_destroy(video_canvas_t *canvas)
{
#ifdef HAVE_FULLSCREEN
    if (canvas != NULL) {
        fullscreen_shutdown_alloc_hooks(canvas);
        lib_free(canvas->fullscreenconfig);
    }
#endif
    if (canvas->gdk_image != NULL)
        g_object_unref(canvas->gdk_image);
#ifdef HAVE_HWSCALE
    lib_free(canvas->hwscale_image);
#endif

    video_canvas_shutdown(canvas);
}

/* set it, update if we know the endianness required by the image */
int video_canvas_set_palette(video_canvas_t *canvas, struct palette_s *palette)
{
    canvas->palette = palette;
    return uicolor_set_palette(canvas, canvas->palette);
}

/* Change the size of the canvas. */
void video_canvas_resize(video_canvas_t *canvas, unsigned int width,
                         unsigned int height)
{
    if (console_mode || vsid_mode)
        return;

    if (canvas->videoconfig->doublesizex)
        width *= 2;

    if (canvas->videoconfig->doublesizey)
        height *= 2;

    if (canvas->gdk_image != NULL)
        g_object_unref(canvas->gdk_image);
    canvas->gdk_image = gdk_image_new(GDK_IMAGE_FASTEST, gtk_widget_get_visual(canvas->emuwindow), width, height);
#ifdef HAVE_HWSCALE
    lib_free(canvas->hwscale_image);
    canvas->hwscale_image = lib_malloc(canvas->gdk_image->width * canvas->gdk_image->height * 4);
#endif
    if (video_canvas_set_palette(canvas, canvas->palette) < 0) {
        log_debug("Setting palette for this mode failed. (Try 16/24/32 bpp.)");
        exit(-1);
    }

    ui_resize_canvas_window(canvas, width, height);
    video_canvas_redraw_size(canvas, width, height);
}

/* Make the canvas visible. */
void video_canvas_map(video_canvas_t *s)
{
    fprintf(stderr, "**Function `canvas_map' not implemented.\n");
}

/* Make the canvas not visible. */
void video_canvas_unmap(video_canvas_t *s)
{
    fprintf(stderr, "**Function `canvas_unmap' not implemented.\n");
}

/* Refresh a canvas.  */
void video_canvas_refresh(video_canvas_t *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
#if 0
    log_debug("XS%i YS%i XI%i YI%i W%i H%i PS%i", xs, ys, xi, yi, w, h,
              canvas->draw_buffer->draw_buffer_width);
#endif

    if (console_mode || vsid_mode)
        return;

    if (canvas->videoconfig->doublesizex) {
        xi *= 2;
        w *= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        yi *= 2;
        h *= 2;
    }

#ifdef HAVE_FULLSCREEN
    if (canvas->video_fullscreen_refresh_func) {
        canvas->video_fullscreen_refresh_func(canvas, xs, ys, xi, yi, w, h);
        return;
    }
#endif

    if (xi + w > canvas->gdk_image->width || yi + h > canvas->gdk_image->height) {
        log_debug("Attempt to draw outside canvas!\n"
                  "XI%i YI%i W%i H%i CW%i CH%i\n",
                  xi, yi, w, h, canvas->gdk_image->width, canvas->gdk_image->height);
        exit(-1);
    }

#ifdef HAVE_HWSCALE
    if (canvas->videoconfig->hwscale) {
        video_canvas_render(canvas, canvas->hwscale_image,
                            w, h, xs, ys, xi, yi, canvas->gdk_image->width * 4,
                            32);
        gtk_widget_queue_draw(canvas->emuwindow);
    } else
#endif
    {
        video_canvas_render(canvas, canvas->gdk_image->mem,
                            w, h, xs, ys, xi, yi, canvas->gdk_image->bpl,
                            canvas->gdk_image->bits_per_pixel);
        gtk_widget_queue_draw_area(canvas->emuwindow, xi, yi, w, h);
    }
}
