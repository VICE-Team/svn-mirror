/*
 * video-canvas.c
 *
 * Written by
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

/* #define DEBUG_VIDEO */

#ifdef DEBUG_VIDEO
#define DBG(_x_) log_debug _x_
#else
#define DBG(_x_)
#endif

#include "vice.h"

#include "videoarch.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib.h"
#include "log.h"
#include "machine.h"
#include "types.h"
#include "raster.h"
#include "render-queue.h"
#include "video-canvas.h"
#include "video-color.h"
#include "video-render.h"
#include "video-sound.h"
#include "video.h"
#include "viewport.h"

#define TRACKED_CANVAS_MAX 2

/** \brief Used to enable video_canvas_refresh_all_tracked() */
static video_canvas_t *tracked_canvas[TRACKED_CANVAS_MAX];

/* Temporary! */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* called from raster/raster-resources.c:raster_resources_chip_init */
video_canvas_t *video_canvas_init(void)
{
    int i;
    video_canvas_t *canvas;

    canvas = lib_calloc(1, sizeof(video_canvas_t));
    DBG(("video_canvas_init %p", canvas));
    
    archdep_mutex_create(&canvas->lock);
    
    canvas->render_queue = render_queue_create();

    canvas->videoconfig = lib_calloc(1, sizeof(video_render_config_t));

    canvas->draw_buffer = lib_calloc(1, sizeof(draw_buffer_t));
    canvas->viewport = lib_calloc(1, sizeof(viewport_t));
    canvas->geometry = lib_calloc(1, sizeof(geometry_t));

    video_arch_canvas_init(canvas);

    for (i = 0; i < TRACKED_CANVAS_MAX; i++) {
        if (!tracked_canvas[i]) {
            tracked_canvas[i] = canvas;
            break;
        }
    }

    if (i == TRACKED_CANVAS_MAX) {
        log_error(LOG_ERR, "Creating more than expected video_canvas_t, monitor will not refresh this canvas after each command");
    }

    return canvas;
}

void video_canvas_shutdown(video_canvas_t *canvas)
{
    int i;

    if (canvas != NULL) {
        /* Remove canvas from tracking */
        for (i = 0; i < TRACKED_CANVAS_MAX; i++) {
            if (tracked_canvas[i] == canvas) {
                tracked_canvas[i] = NULL;
                break;
            }
        }
        
        archdep_mutex_destroy(canvas->lock);
        render_queue_destroy(canvas->render_queue);
        lib_free(canvas->videoconfig);
        raster_draw_buffer_shutdown(canvas->draw_buffer);
        lib_free(canvas->draw_buffer);
        if (canvas->draw_buffer_vsid) {
            raster_draw_buffer_shutdown(canvas->draw_buffer_vsid);
            lib_free(canvas->draw_buffer_vsid);
        }
        video_viewport_title_free(canvas->viewport);
        lib_free(canvas->viewport);
        lib_free(canvas->geometry);
        lib_free(canvas);
    }
}

void video_canvas_prepare_backbuffer(video_canvas_t *canvas, draw_buffer_t *draw_buffer, backbuffer_t *backbuffer)
{
    viewport_t *viewport = canvas->viewport;

    /* when the color encoding changed, the palette must be recalculated */
    if (viewport->crt_type != canvas->crt_type) {
        canvas->videoconfig->color_tables.updated = 0;
        canvas->crt_type = viewport->crt_type;
    }

    if (!canvas->videoconfig->color_tables.updated) { /* update colors as necessary */
        video_color_update_palette(canvas);
    }
    
    backbuffer->videoconfig = *(canvas->videoconfig);
    backbuffer->viewport = *viewport;
    
    assert(draw_buffer->padded_allocations_size_bytes == backbuffer->screen_data_used_size_bytes);
    
    memcpy(
        backbuffer->screen_data_padded,
        draw_buffer->padded_allocations[canvas->videoconfig->interlace_field],
        backbuffer->screen_data_used_size_bytes);
}

void video_canvas_render_backbuffer(backbuffer_t *backbuffer)
{
    video_render_main(&backbuffer->videoconfig,
                      backbuffer->screen_data_padded + backbuffer->screen_data_offset,
                      backbuffer->pixel_data,
                      backbuffer->width,
                      backbuffer->height,
                      backbuffer->xs,
                      backbuffer->ys,
                      backbuffer->xi,
                      backbuffer->yi,
                      backbuffer->screen_data_width,
                      backbuffer->width * 4,
                      &backbuffer->viewport);
}

void video_canvas_render(video_canvas_t *canvas, uint8_t *trg, int width,
                         int height, int xs, int ys, int xt, int yt,
                         int pitcht)
{
    viewport_t *viewport = canvas->viewport;

    /* when the color encoding changed, the palette must be recalculated */
    if (viewport->crt_type != canvas->crt_type) {
        canvas->videoconfig->color_tables.updated = 0;
        canvas->crt_type = viewport->crt_type;
    }

    if (!canvas->videoconfig->color_tables.updated) { /* update colors as necessary */
        video_color_update_palette(canvas);
    }

    video_render_main(canvas->videoconfig, canvas->draw_buffer->draw_buffer,
                      trg, width, height, xs, ys, xt, yt,
                      canvas->draw_buffer->draw_buffer_width, pitcht,
                      viewport);
}

extern video_canvas_t *video_canvas_get(int index)
{
    return tracked_canvas[index];
}

/** \brief Force refresh all tracked canvases.
 *
 * Added to enable visible updates each time the monitor
 * prompts for input.
 */
void video_canvas_refresh_all_tracked(void)
{
    int i;

    for (i = 0; i < TRACKED_CANVAS_MAX; i++) {
        if (tracked_canvas[i]) {
            video_canvas_refresh_all(tracked_canvas[i]);
        }
    }
}

void video_canvas_refresh_all(video_canvas_t *canvas)
{
    unsigned int xs; /* A parameter to forward to video_canvas_render() */
    unsigned int ys; /* A parameter to forward to video_canvas_render() */
    unsigned int xi; /* X coordinate of the leftmost pixel to update */
    unsigned int yi; /* Y coordinate of the topmost pixel to update */
    unsigned int w;  /* Width of the rectangle to update */
    unsigned int h;  /* Height of the rectangle to update */
    
    backbuffer_t *backbuffer;
    viewport_t *viewport;
    geometry_t *geometry;
    draw_buffer_t *draw_buffer;

    if (video_disabled_mode) {
        return;
    }
    
    if (canvas->width == 0 || canvas->height == 0) {
        /* Happens during resource init */
        return;
    }
    
    viewport = canvas->viewport;
    geometry = canvas->geometry;
    
    if (machine_class == VICE_MACHINE_VSID) {
        draw_buffer = canvas->draw_buffer_vsid;
    } else {
        draw_buffer = canvas->draw_buffer;
    }
    
    xs = viewport->first_x + geometry->extra_offscreen_border_left;
    ys = viewport->first_line;
    xi = canvas->videoconfig->scalex * viewport->x_offset;
    yi = canvas->videoconfig->scaley * viewport->y_offset;
    w  = canvas->videoconfig->scalex * MIN(draw_buffer->canvas_width, geometry->screen_size.width - viewport->first_x);
    h  = canvas->videoconfig->scaley * MIN(draw_buffer->canvas_height, viewport->last_line - viewport->first_line + 1);
    
    /*
     * If there is an unused render buffer, render a new frame.
     *
     * TODO: Convert this to a copy of everything needed to render the frame, and push the work to the ui/render thread
     */
    
    backbuffer = render_queue_get_from_pool(canvas->render_queue, draw_buffer->padded_allocations_size_bytes, 4 * w * h);
    if (backbuffer == NULL) {
        return;
    }

    backbuffer->width               = w;
    backbuffer->height              = h;
    backbuffer->pixel_aspect_ratio  = geometry->pixel_aspect_ratio;;
    backbuffer->interlaced          = canvas->videoconfig->interlaced;
    backbuffer->interlace_field     = canvas->videoconfig->interlace_field;
    
    backbuffer->xs                  = xs;
    backbuffer->ys                  = ys;
    backbuffer->xi                  = xi;
    backbuffer->yi                  = yi;
    
    backbuffer->screen_data_width   = draw_buffer->draw_buffer_width;
    backbuffer->screen_data_height  = draw_buffer->draw_buffer_height;
    backbuffer->screen_data_offset  = draw_buffer->padded_allocations_offset;
    
//    video_sound_update(canvas->videoconfig, canvas->draw_buffer->draw_buffer,
//                       w, h, xs, ys, canvas->draw_buffer->draw_buffer_width, canvas->viewport);
    
    video_canvas_new_frame_hook(canvas);
    
    /* Rendering to the backbuffer happens here */
    video_canvas_prepare_backbuffer(canvas, draw_buffer, backbuffer);
    
//    if (machine_class == VICE_MACHINE_VSID) {
//        canvas->draw_buffer_vsid->draw_buffer_width = canvas->draw_buffer->draw_buffer_width;
//        canvas->draw_buffer_vsid->draw_buffer_height = canvas->draw_buffer->draw_buffer_height;
//        canvas->draw_buffer_vsid->canvas_physical_width = canvas->draw_buffer->canvas_physical_width;
//        canvas->draw_buffer_vsid->canvas_physical_height = canvas->draw_buffer->canvas_physical_height;
//        canvas->draw_buffer_vsid->canvas_width = canvas->draw_buffer->canvas_width;
//        canvas->draw_buffer_vsid->canvas_height = canvas->draw_buffer->canvas_height;
//        canvas->draw_buffer_vsid->visible_width = canvas->draw_buffer->visible_width;
//        canvas->draw_buffer_vsid->visible_height = canvas->draw_buffer->visible_height;
//
//        video_canvas_prepare_backbuffer(canvas, canvas->draw_buffer_vsid, backbuffer);
//        //video_canvas_render(canvas, (uint8_t *)canvas->screen->pixels, w, h, xs, ys, xi, yi, canvas->screen->pitch);
//    } else {
//        video_canvas_prepare_backbuffer(canvas, canvas->draw_buffer, backbuffer);
//        // video_canvas_render(canvas, backbuffer->pixel_data, w, h, xs, ys, xi, yi, backbuffer->width * 4);
//    }

    /* Place in the queue for another thread to pick up */
    render_queue_enqueue_for_display(canvas->render_queue, backbuffer);
    
    /* UI specific method to notify the render thread of the new frame */
    video_canvas_on_new_backbuffer(canvas);
}

int video_canvas_palette_set(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    struct palette_s *old_palette;

    if (palette == NULL) {
        return 0;
    }

    old_palette = canvas->palette;

    if (canvas->created) {
        if (video_canvas_set_palette(canvas, palette) < 0) {
            return -1;
        }
    } else {
        canvas->palette = palette;
    }

    if (old_palette != NULL) {
        video_color_palette_free(old_palette);
    }

#if 0 /* WTF this was causing each frame to be rendered twice */
   if (canvas->created) {
       video_canvas_refresh_all(canvas);
   }
#endif

    return 0;
}

void video_canvas_create_set(struct video_canvas_s *canvas)
{
    canvas->created = 1;
}
