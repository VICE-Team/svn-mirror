/**
 * \file    cairo_renderer.c
 * \brief   Cairo-based renderer for the GTK3 backend
 *
 * \author  Michael C. Martin <mcmartin@gmail.com>
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

#include "cairo_renderer.h"

#include <cairo.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "render_queue.h"
#include "resources.h"
#include "ui.h"
#include "video.h"

#define BACKBUFFER_FORMAT CAIRO_FORMAT_RGB24

/** \brief Rendering context for the Cairo backend.
 *  \sa video_canvas_s::renderer_context */
typedef struct vice_cairo_renderer_context_s {
    /** \brief A queue of resized cairo surfaces ready for painting to the widget */
    void *render_queue;
    
    /** \brief size of the emu bitmap in pixels */
    unsigned int width;
    
    /** \brief size of the emu bitmap in pixels */
    unsigned int height;
    
    /** \brief stride (width plus padding) of the emu bitmap in bytes */
    unsigned int stride;
} context_t;

/** \brief Rendering callback to display the screen as we understand
 *         it.
 *  \param widget The GtkDrawingArea we are rendering to.
 *  \param cr     The Cairo context that we draw through.
 *  \param data   The video_canvas_t we're rendering from.
 *  \return TRUE if no further processing is needed on this event.
 */
static gboolean
draw_canvas_cairo_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    video_canvas_t *canvas = (video_canvas_t *)data;
    context_t *ctx;
    cairo_surface_t *backbuffer = NULL;
    int x, y;

    /*
     * Half-grey background for those parts of the window that aren't
     * video, or black if it's fullscreen.
     * TODO: configurable? Set the bgcolour of the gtk stuff?
     */
    if (ui_is_fullscreen()) {
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    } else {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    }
    cairo_paint(cr);

    if (!canvas) {
        return FALSE;
    }

    pthread_mutex_lock(&canvas->lock);

    ctx = (context_t *)canvas->renderer_context;
    if (!ctx) {
        pthread_mutex_unlock(&canvas->lock);
        return FALSE;
    }
    
    // backbuffer = render_queue_dequeue_for_display(ctx->render_queue, &backbuffer_generation);
    if (!backbuffer) {
        pthread_mutex_unlock(&canvas->lock);
        return FALSE;
    }

    /* Don't hold the lock while cairo slowly renders this image */
    pthread_mutex_unlock(&canvas->lock);

    x = (gtk_widget_get_allocated_width(widget) - cairo_image_surface_get_width(backbuffer)) / 2;
    y = (gtk_widget_get_allocated_height(widget) - cairo_image_surface_get_height(backbuffer)) / 2;
        
    cairo_set_source_surface(cr, backbuffer, x, y);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_paint(cr);
    
    /* Get the lock again and return the backbuffer to the pool */
    pthread_mutex_lock(&canvas->lock);
    // render_queue_return_to_pool(ctx->render_queue, backbuffer);
    pthread_mutex_unlock(&canvas->lock);

    return FALSE;
}

/** \brief  Callback to adjust scaling and offset when the window is
 *          resized but the underlying machine screen is not.
 *  \param widget The GtkDrawingArea being resized.
 *  \param event  The GdkEventConfigure that is triggered this callback.
 *  \param data   The video_canvas_t that controls this drawing area.
 *  \return TRUE if no further processing is needed on this event.
 */
static gboolean
resize_canvas_container_cairo_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    video_canvas_t *canvas = (video_canvas_t *)data;
    context_t *ctx;
    
    pthread_mutex_lock(&canvas->lock);
    
    ctx = canvas ? (context_t *)canvas->renderer_context : NULL;
    if (!ctx) {
        pthread_mutex_unlock(&canvas->lock);
        return FALSE;
    }
    
    pthread_mutex_unlock(&canvas->lock);
    
    return FALSE;
}

/** \brief Cairo implementation of create_widget.
 *
 *  \param canvas The canvas to create the widget for.
 *  \return The newly created canvas.
 *  \sa vice_renderer_backend_s::create_widget
 */
static GtkWidget *vice_cairo_create_widget(video_canvas_t *canvas)
{
    GtkWidget *widget = gtk_drawing_area_new();
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_vexpand(widget, TRUE);
    
    pthread_mutex_lock(&canvas->lock);
    canvas->drawing_area = widget;
    pthread_mutex_unlock(&canvas->lock);
    
    g_signal_connect_unlocked(widget, "draw", G_CALLBACK(draw_canvas_cairo_cb), canvas);
    g_signal_connect_unlocked(widget, "configure_event", G_CALLBACK(resize_canvas_container_cairo_cb), canvas);
    
    return widget;
}

/** \brief Cairo implementation of destroy_context.
 * 
 *  \param canvas The canvas whose renderer_context is to be
 *                deleted
 *  \sa vice_renderer_backend_s::destroy_context
 */
static void vice_cairo_destroy_context(video_canvas_t *canvas)
{
    context_t *ctx;
    
    if (!canvas) {
        return;
    }
    
    pthread_mutex_lock(&canvas->lock);
    
    ctx = (context_t *)canvas->renderer_context;
    if (!ctx) {
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    render_queue_destroy(ctx->render_queue);
    lib_free(ctx);
    
    canvas->renderer_context = NULL;
    
    pthread_mutex_unlock(&canvas->lock);
}

/** \brief Cairo implementation of update_context.
 * \param canvas The canvas being resized or initially created.
 * \param width The new width for the machine's screen.
 * \param height The new height for the machine's screen.
 * \sa vice_renderer_backend_s::update_context
 */
static void vice_cairo_update_context(video_canvas_t *canvas, unsigned int width, unsigned int height)
{
    context_t *ctx;
    
    if (!canvas) {
        return;
    }
    
    pthread_mutex_lock(&canvas->lock);
    
    ctx = (context_t *)canvas->renderer_context;
    if (ctx && ctx->width == width && ctx->height == height) {
        /* Canvas already exists and is the proper size */
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    if (width == 0 || height == 0) {
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    if (!ctx) {
        ctx = lib_calloc(1, sizeof(context_t));
        ctx->render_queue = render_queue_create();
    }
    
    ctx->width = width;
    ctx->height = height;
    ctx->stride = cairo_format_stride_for_width(BACKBUFFER_FORMAT, width);
    
    // for (int i = 0; i < RENDER_QUEUE_MAX_BACKBUFFERS; i++) {
    //     render_queue_add_new(
    //         ctx->render_queue,
    //         cairo_image_surface_create_for_data(
    //             lib_malloc(ctx->stride * height * 4),
    //             BACKBUFFER_FORMAT,
    //             width,
    //             height,
    //             ctx->stride));
    // }
    
    canvas->renderer_context = ctx;

    pthread_mutex_unlock(&canvas->lock);
}

/** \brief Cairo implementation of refresh_rect.
 * \param canvas The canvas being rendered to
 * \param xs     A parameter to forward to video_canvas_render()
 * \param ys     A parameter to forward to video_canvas_render()
 * \param xi     X coordinate of the leftmost pixel to update
 * \param yi     Y coordinate of the topmost pixel to update
 * \param w      Width of the rectangle to update
 * \param h      Height of the rectangle to update
 * \sa vice_renderer_backend_s::refresh_rect */
static void vice_cairo_refresh_rect(video_canvas_t *canvas,
                                    unsigned int xs, unsigned int ys,
                                    unsigned int xi, unsigned int yi,
                                    unsigned int w, unsigned int h)
{
    context_t *ctx;
    cairo_surface_t *backbuffer = NULL;
    
    pthread_mutex_lock(&canvas->lock);
    
    ctx = (context_t *)canvas->renderer_context;
    if (!ctx) {
        pthread_mutex_unlock(&canvas->lock);
        return;
    }

    if (((xi + w) > ctx->width) || ((yi + h) > ctx->height)) {
        /* Trying to draw outside canvas? */
        log_warning(LOG_DEFAULT, "Attempt to draw outside canvas!\nXI%u YI%u W%u H%u CW%u CH%u", xi, yi, w, h, ctx->width, ctx->height);
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    /* Obtain an unused backbuffer to render to */
    //backbuffer = render_queue_get_from_pool(ctx->render_queue, &backbuffer_generation);
    if (!backbuffer) {
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    /* Don't hold the lock while rendering */
    pthread_mutex_unlock(&canvas->lock);
    
    cairo_surface_flush(backbuffer);
    video_canvas_render(canvas, cairo_image_surface_get_data(backbuffer), w, h, xs, ys, xi, yi, ctx->stride, 32);
    cairo_surface_mark_dirty(backbuffer);
    
    /* Need the lock back to safely return the buffer */
    pthread_mutex_lock(&canvas->lock);
    
    /* Place the rendered buffer in the render queue */
    // render_queue_enqueue_for_display(ctx->render_queue, backbuffer);
    
    pthread_mutex_unlock(&canvas->lock);

    gtk_widget_queue_draw(canvas->drawing_area);
}

static void vice_cairo_queue_redraw(GdkFrameClock *clock, video_canvas_t *canvas)
{
    // context_t *ctx;
    // // gint widget_width;
    // // gint widget_height;
    
    // pthread_mutex_lock(&canvas->lock);
    
    // ctx = canvas ? (context_t *)canvas->renderer_context : NULL;

    // if (!ctx || !canvas->drawing_area) {
    //     pthread_mutex_unlock(&canvas->lock);
    //     return;
    // }
    
    // //gtk_widget_get_size_request(canvas->drawing_area, &widget_width, &widget_height);
    
    // /* Make sure the drawing area is the right size for the buffers we're producing */
    // //if (widget_width != ctx->width || widget_height != ctx->height) {
    //     //gtk_widget_set_size_request(canvas->drawing_area, ctx->width, ctx->height);
    // //}
    
    // /* Place the rendered buffer in the render queue */
    // if (render_queue_ready_for_display(ctx->render_queue)) {
    //     gtk_widget_queue_draw(canvas->drawing_area);
    // }
    
    // pthread_mutex_unlock(&canvas->lock);
}

/** \brief Cairo implementation of set_palette.
 * \param canvas The canvas being initialized
 * \sa vice_renderer_backend_s::set_palette */
static void vice_cairo_set_palette(video_canvas_t *canvas)
{
    int i;
    struct palette_s *palette;
    
    pthread_mutex_lock(&canvas->lock);
    
    palette = canvas ? canvas->palette : NULL;
    if (!palette) {
        pthread_mutex_unlock(&canvas->lock);
        return;
    }
    
    /* We use CAIRO_FORMAT_RGB24, which is defined as follows: "Each
     * pixel is a 32-bit quantity, with the upper 8 bits unused. Red,
     * Green, and Blue are stored in the remaining 24 bits in that
     * order." */
    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        /* 0xff000000 is set to make this compatibile with ARGB32 */
        uint32_t cairo_color = 0xff000000 | (color.red << 16) | (color.green << 8) | color.blue;
        video_render_setphysicalcolor(canvas->videoconfig, i, cairo_color, 32);
    }
    
    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(i, i << 16, i << 8, i);
    }
    video_render_initraw(canvas->videoconfig);
    
    pthread_mutex_unlock(&canvas->lock);
}


vice_renderer_backend_t vice_cairo_backend = {
    vice_cairo_create_widget,
    vice_cairo_update_context,
    vice_cairo_destroy_context,
    vice_cairo_refresh_rect,
    vice_cairo_queue_redraw,
    vice_cairo_set_palette
};
