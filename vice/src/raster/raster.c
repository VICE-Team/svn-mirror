/*
 * raster.c - Raster-based video chip emulation helper.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "machine.h"
#include "palette.h"
#include "raster-cache.h"
#include "raster-canvas.h"
#include "raster-modes.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "raster.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"


static int realize_canvas(raster_t *raster);

int raster_calc_frame_buffer_width(raster_t *raster)
{
    return raster->geometry.screen_size.width
        + raster->geometry.extra_offscreen_border_left
        + raster->geometry.extra_offscreen_border_right;
}

static int raster_draw_buffer_alloc(video_canvas_t *canvas,
                                    unsigned int fb_width,
                                    unsigned int fb_height,
                                    unsigned int *fb_pitch)
{
    if (canvas->video_draw_buffer_callback)
        return canvas->video_draw_buffer_callback->draw_buffer_alloc(canvas,
            &canvas->draw_buffer->draw_buffer, fb_width, fb_height, fb_pitch);

    canvas->draw_buffer->draw_buffer = (BYTE *)xmalloc(fb_width * fb_height);
    *fb_pitch = fb_width;
    return 0;
}

static void raster_draw_buffer_free(video_canvas_t *canvas)
{
    if (canvas->video_draw_buffer_callback) {
        canvas->video_draw_buffer_callback->draw_buffer_free(canvas,
            canvas->draw_buffer->draw_buffer);
	return;
    }

    free(canvas->draw_buffer->draw_buffer);
}

static void raster_draw_buffer_clear(video_canvas_t *canvas, BYTE value,
                                     unsigned int fb_width,
                                     unsigned int fb_height,
                                     unsigned int fb_pitch)
{
    if (canvas->video_draw_buffer_callback) {
        canvas->video_draw_buffer_callback->draw_buffer_clear(canvas,
            canvas->draw_buffer->draw_buffer, value, fb_width, fb_height,
            fb_pitch);
	return;
    }

    memset(canvas->draw_buffer->draw_buffer, value, fb_width * fb_height);
}

void raster_draw_buffer_ptr_update(raster_t *raster)
{
    if (console_mode || vsid_mode)
        return;

    raster->draw_buffer_ptr
        = raster->viewport.canvas->draw_buffer->draw_buffer
        + raster->current_line * raster_calc_frame_buffer_width(raster)
        + raster->geometry.extra_offscreen_border_left;
}

static void update_pixel_tables(raster_t *raster)
{
    unsigned int i;

    for (i = 0; i < 256; i++) {
        raster->pixel_table.sing[i] = i;
        *((BYTE *)(raster->pixel_table.doub + i))
        = *((BYTE *)(raster->pixel_table.doub + i) + 1)
        = raster->pixel_table.sing[i];
        *((WORD *)(raster->pixel_table.quad + i))
        = *((WORD *)(raster->pixel_table.quad + i) + 1)
        = raster->pixel_table.doub[i];
    }
}

void raster_canvas_init(raster_t *raster)
{
    raster_viewport_t *viewport;

    viewport = &raster->viewport;
}

static int realize_canvas(raster_t *raster)
{
    raster_viewport_t *viewport;

    viewport = &raster->viewport;

    viewport->canvas = video_canvas_init(raster->videoconfig);

    video_canvas_create(viewport->canvas, viewport->title,
                        &viewport->width, &viewport->height, 1,
                        (void_t)(raster->viewport.exposure_handler),
                        raster->palette);

    if (viewport->canvas == NULL)
        return -1;

    if (raster_realize_frame_buffer(raster) < 0)
        return -1;

    /* The canvas might give us something different from what we
       requested.  */
    raster_resize_viewport(raster, viewport->width, viewport->height);
    return 0;
}

int raster_realize_frame_buffer(raster_t *raster)
{
    unsigned int fb_width, fb_height, fb_pitch;
    video_canvas_t *canvas;

    canvas = raster->viewport.canvas;

    if (canvas == NULL)
        return 0;

    if (!console_mode && !vsid_mode)
        raster_draw_buffer_free(canvas);

    fb_width = raster_calc_frame_buffer_width(raster);
    fb_height = raster->geometry.screen_size.height;

    if (!console_mode && !vsid_mode && fb_width > 0 && fb_height > 0) {
        if (raster_draw_buffer_alloc(canvas, fb_width, fb_height, &fb_pitch))
        return -1;

        raster->viewport.canvas->draw_buffer->draw_buffer_width = fb_width;
        raster->viewport.canvas->draw_buffer->draw_buffer_height = fb_height;
        raster->viewport.canvas->draw_buffer->draw_buffer_pitch = fb_pitch;

#if defined (USE_XF86_EXTENSIONS)
        video_register_raster(raster);
#endif

        raster_draw_buffer_clear(canvas, 0, fb_width, fb_height, fb_pitch);
    }

    raster->fake_draw_buffer_line = xrealloc(raster->fake_draw_buffer_line,
                                             fb_width);

    return 0;
}

static int perform_mode_change(raster_t *raster)
{
    raster_viewport_t *viewport;
    struct video_canvas_s *canvas;

    viewport = &raster->viewport;

    if ((canvas = viewport->canvas) == NULL)
        return 0;

    if (raster->palette != NULL) {
        if (video_canvas_set_palette(canvas, raster->palette) < 0)
            return -1;
    }

    if (raster->refresh_tables != NULL)
        raster->refresh_tables();

    raster_force_repaint(raster);

    video_canvas_resize(canvas, viewport->width, viewport->height);
    raster_resize_viewport(raster, viewport->width, viewport->height);
    return 0;
}

/*  Rebuild Color tables of raster. It's used when colordepth
    of the viewport changes.
*/
void raster_rebuild_tables(raster_t *raster)
{
    if (raster->refresh_tables != NULL)
        raster->refresh_tables();
}

inline static void draw_blank(raster_t *raster,
                              unsigned int start,
                              unsigned int end)
{
    memset(raster->draw_buffer_ptr + start,
           raster->border_color, end - start + 1);
}

/* Draw the borders.  */
inline static void draw_borders(raster_t *raster)
{
    if (!raster->open_left_border)
        draw_blank(raster, 0, raster->display_xstart - 1);
    if (!raster->open_right_border)
        draw_blank(raster,
                   raster->display_xstop,
                   raster->geometry.screen_size.width - 1);
}

static void raster_viewport_init(raster_viewport_t *viewport)
{
    viewport->canvas = NULL;
    viewport->width = viewport->height = 0;
    viewport->title = NULL;
    viewport->x_offset = viewport->y_offset = 0;
    viewport->first_line = viewport->last_line = 0;
    viewport->first_x = 0;
    viewport->exposure_handler = NULL;
}

static void raster_geometry_init(raster_geometry_t *geometry)
{
    geometry->screen_size.width = geometry->screen_size.height = 0;
    geometry->gfx_size.width = geometry->gfx_size.height = 0;
    geometry->text_size.width = geometry->text_size.height = 0;
    geometry->gfx_position.x = geometry->gfx_position.y = 0;
    geometry->gfx_area_moves = 0;
    geometry->first_displayed_line = 0;
    geometry->last_displayed_line = 0;
}

#if defined(WIN32) || defined(USE_XF86_EXTENSIONS)
void video_register_raster(raster_t *raster);
#endif

int raster_init(raster_t *raster,
                unsigned int num_modes,
                unsigned int num_sprites)
{
/*  FIXME: This is a WORKAROUND, I need access to fields in the
    raster struct when window has to be updated in certain cases...
    So I have to register it in the video module and do a lookup.
    */
#if defined(WIN32) || defined(USE_XF86_EXTENSIONS)
    video_register_raster(raster);
#endif

    raster_viewport_init(&raster->viewport);
    raster_geometry_init(&raster->geometry);

    raster->modes = (raster_modes_t *)xmalloc(sizeof(raster_modes_t));
    raster_modes_init(raster->modes, num_modes);

    raster->sprite_status = (raster_sprite_status_t *)
                            xmalloc(sizeof(raster_sprite_status_t));
    raster_sprite_status_init(raster->sprite_status, num_sprites);

    raster_reset(raster);

    raster->palette = NULL;

    raster->display_xstart = raster->display_xstop = 0;
    raster->display_ystart = raster->display_ystop = 0;

    raster->cache = NULL;
    raster->cache_enabled = 0;
    raster->dont_cache = 1;
    raster->num_cached_lines = 0;

    raster->update_area.is_null = 1;

    raster->fake_draw_buffer_line = NULL;

    raster->refresh_tables = NULL;

    raster->border_color = 0;
    raster->background_color = 0;
    raster->overscan_background_color = 0;
    raster->xsmooth_color = 0;

    memset(raster->gfx_msk, 0, RASTER_GFX_MSK_SIZE);
    memset(raster->zero_gfx_msk, 0, RASTER_GFX_MSK_SIZE);

    raster_set_canvas_refresh(raster, 1);

    update_pixel_tables(raster);

    return 0;
}

void raster_reset(raster_t *raster)
{
    raster_changes_remove_all(&raster->changes.background);
    raster_changes_remove_all(&raster->changes.foreground);
    raster_changes_remove_all(&raster->changes.border);
    raster_changes_remove_all(&raster->changes.next_line);
    raster->changes.have_on_this_line = 0;

    raster->current_line = 0;

    raster->xsmooth = raster->ysmooth = 0;
    raster->xsmooth_shift_left = 0;
    raster->xsmooth_shift_right = 0;
    raster->skip_frame = 0;

    raster->blank_off = 0;
    raster->blank_enabled = 0;
    raster->blank_this_line = 0;
    raster->open_right_border = 0;
    raster->open_left_border = 0;
    raster->blank = 0;

    raster->draw_idle_state = 0;
    raster->ycounter = 0;
    raster->video_mode = 0;
}

typedef struct raster_list_t {
    raster_t *raster;
    struct raster_list_t *next;
} raster_list_t;

static raster_list_t *ActiveRasters = NULL;

raster_t *raster_new(unsigned int num_modes,
                     unsigned int num_sprites)
{
    raster_t *new;

    new = xmalloc(sizeof(raster_t));
    raster_init(new, num_modes, num_sprites);

/*  FIXME: This is a WORKAROUND, I need access to fields in the
    raster struct when window has to be updated in certain cases...
    So I have to register it in the video module and do a lookup.
*/
#ifdef WIN32
    video_register_raster(new);
#endif

    return new;
}

void raster_mode_change(void)
{
    raster_list_t *rasters = ActiveRasters;

    while (rasters != NULL) {
        perform_mode_change(rasters->raster);
        rasters = rasters->next;
    }
}


void raster_invalidate_cache(raster_t *raster, unsigned int screen_height)
{
    unsigned int i;

    for (i = 0; i < screen_height; i++)
        raster_cache_init(&(raster->cache)[i]);
}

void raster_set_geometry(raster_t *raster,
                         unsigned int screen_width, unsigned int screen_height,
                         unsigned int gfx_width, unsigned int gfx_height,
                         unsigned int text_width, unsigned int text_height,
                         unsigned int gfx_position_x,
                         unsigned int gfx_position_y,
                         int gfx_area_moves,
                         unsigned int first_displayed_line,
                         unsigned int last_displayed_line,
                         unsigned int extra_offscreen_border_left,
                         unsigned int extra_offscreen_border_right)
{
    raster_geometry_t *geometry;

    geometry = &raster->geometry;
    if (screen_height != geometry->screen_size.height
        || raster->cache == NULL) {
        raster->cache = xrealloc(raster->cache,
                                 sizeof(*raster->cache) * screen_height);
        raster_invalidate_cache(raster, screen_height);
    }

    if (geometry->screen_size.width != screen_width
        || geometry->screen_size.height != screen_height
        || geometry->extra_offscreen_border_left != extra_offscreen_border_left
        || geometry->extra_offscreen_border_right
        != extra_offscreen_border_right) {
        geometry->screen_size.width = screen_width;
        geometry->screen_size.height = screen_height;
        geometry->extra_offscreen_border_left = extra_offscreen_border_left;
        geometry->extra_offscreen_border_right = extra_offscreen_border_right;
        raster_realize_frame_buffer(raster);
    }

    geometry->gfx_size.width = gfx_width;
    geometry->gfx_size.height = gfx_height;
    geometry->text_size.width = text_width;
    geometry->text_size.height = text_height;

    geometry->gfx_position.x = gfx_position_x;
    geometry->gfx_position.y = gfx_position_y;

    geometry->gfx_area_moves = gfx_area_moves;

    geometry->first_displayed_line = first_displayed_line;
    geometry->last_displayed_line = last_displayed_line;
}

void raster_set_exposure_handler(raster_t *raster, void *exposure_handler)
{
    raster->viewport.exposure_handler = exposure_handler;
}

void raster_set_table_refresh_handler(raster_t *raster, void (*handler)(void))
{
    raster->refresh_tables = handler;
}


int raster_realize(raster_t *raster)
{
    raster_list_t *rlist;

    if (!console_mode && !vsid_mode)
        if (realize_canvas(raster) < 0)
            return -1;

    raster_canvas_update_all(raster);

    rlist = (raster_list_t*)xmalloc(sizeof(raster_list_t));
    rlist->raster = raster;
    rlist->next = NULL;
    if (ActiveRasters == NULL) {
        ActiveRasters = rlist;
    } else {
        raster_list_t *rasters = ActiveRasters;

        while (rasters->next != NULL)
            rasters = rasters->next;
        rasters->next = rlist;
    }
    return 0;
}

/* Resize the canvas with the specified values and center the screen image on
   it.  The actual size can be different if the parameters are not
   suitable.  */
void raster_resize_viewport(raster_t *raster,
                            unsigned int width,
                            unsigned int height)
{
    raster_geometry_t *geometry;
    raster_viewport_t *viewport;
    raster_rectangle_t *screen_size;
    raster_rectangle_t *gfx_size;
    raster_position_t *gfx_position;
    unsigned int gfx_height;

    geometry = &raster->geometry;

    screen_size = &geometry->screen_size;
    gfx_size = &geometry->gfx_size;
    gfx_position = &geometry->gfx_position;

    viewport = &raster->viewport;

    if (width >= screen_size->width) {
        viewport->x_offset = (width - screen_size->width) / 2;
        viewport->first_x = 0;
    } else {
        viewport->x_offset = 0;

        if (geometry->gfx_area_moves) {
            viewport->first_x = (screen_size->width - width) / 2;
        } else {
            viewport->first_x = gfx_position->x;

            if (width > gfx_size->width)
                viewport->first_x -= (width - gfx_size->width) / 2;
        }
    }

    gfx_height = height - gfx_size->height;

    if (height >= screen_size->height) {
        viewport->y_offset = (height - screen_size->height) / 2;
        viewport->first_line = 0;
        viewport->last_line = screen_size->height - 1;
    } else {
        viewport->y_offset = 0;

        if (geometry->gfx_area_moves) {
            viewport->first_line = (screen_size->height - height) / 2;
        } else {
            /* FIXME: Somewhat buggy.  */
            viewport->first_line = gfx_position->y;

            if (height > gfx_size->height) {
                if (gfx_height <= gfx_position->y)
                    viewport->first_line -= gfx_height / 2;
                else
                    viewport->first_line = 0;
            }

        }
        viewport->last_line = (viewport->first_line + height) - 1;
    }

    /* Hmmm....  FIXME?  */
    if (viewport->canvas != NULL)
        video_canvas_resize(viewport->canvas, width, height);

    viewport->width = width;
    viewport->height = height;

    /* Make sure we don't waste space showing unused lines.  */
    if ((viewport->first_line < geometry->first_displayed_line
        && viewport->last_line < geometry->last_displayed_line)
        || (viewport->first_line > geometry->first_displayed_line
        && viewport->last_line > geometry->last_displayed_line)) {
        viewport->first_line = geometry->first_displayed_line;
        viewport->last_line = (geometry->first_displayed_line + height);
    }
    raster_force_repaint(raster);
}

void raster_force_repaint(raster_t *raster)
{
    raster->dont_cache = 1;
    raster->num_cached_lines = 0;
}

int raster_set_palette(raster_t *raster, palette_t *palette)
{

    if (raster->viewport.canvas != NULL) {
        if (video_canvas_set_palette(raster->viewport.canvas, palette) < 0)
            return -1;
    }

    if (raster->palette != NULL)
        palette_free(raster->palette);

    raster->palette = palette;

    if (raster->refresh_tables != NULL)
        raster->refresh_tables();

    raster_force_repaint(raster);

    return 0;
}

void raster_set_title(raster_t *raster, const char *title)
{
    raster_viewport_t *viewport;

    viewport = &raster->viewport;

    free(viewport->title);
    viewport->title = stralloc(title);

#if 0                           /* FIXME: Not yet in the canvas API.  */
    if (viewport->canvas != NULL)
        canvas_set_title(viewport->canvas, title);
#endif
}

void raster_skip_frame(raster_t *raster, int skip)
{
    raster->skip_frame = skip;
}

void raster_enable_cache(raster_t *raster, int enable)
{
    raster->cache_enabled = enable;
    raster_force_repaint(raster);
}

void raster_set_canvas_refresh(raster_t *raster, int enable)
{
    raster->viewport.update_canvas = enable;
}

void raster_screenshot(raster_t *raster, screenshot_t *screenshot)
{
    screenshot->palette = raster->palette;
    screenshot->pixel_table_sing = raster->pixel_table.sing;
    screenshot->max_width = raster->geometry.screen_size.width;
    screenshot->max_height = raster->geometry.screen_size.height;
    screenshot->x_offset = raster->geometry.extra_offscreen_border_left;
    screenshot->size_width = 1;
    screenshot->size_height = 1;
    screenshot->first_displayed_line = raster->geometry.first_displayed_line;
    screenshot->last_displayed_line = raster->geometry.last_displayed_line;
    screenshot->first_displayed_col
        = raster->geometry.extra_offscreen_border_left
        + raster->viewport.first_x;
    screenshot->draw_buffer = raster->viewport.canvas->draw_buffer->draw_buffer;
    screenshot->draw_buffer_line_size
        = raster->viewport.canvas->draw_buffer->draw_buffer_width;
}

void raster_async_refresh(raster_t *raster, struct canvas_refresh_s *ref)
{
    ref->draw_buffer = raster->viewport.canvas->draw_buffer->draw_buffer;
    ref->draw_buffer_line_size
        = raster->viewport.canvas->draw_buffer->draw_buffer_width;
#ifdef __OS2__
    ref->bufh = raster->draw_buffer_height;
#endif
    ref->x = raster->geometry.extra_offscreen_border_left
        + raster->viewport.first_x;
    ref->y = raster->geometry.first_displayed_line;

    if (raster->viewport.canvas->videoconfig->doublesizex)
        ref->x *= 2;
    if (raster->viewport.canvas->videoconfig->doublesizey)
        ref->y *= 2;
}

void raster_free(raster_t *raster)
{
    if (!console_mode && !vsid_mode) {
        video_canvas_t *canvas;

        canvas = raster->viewport.canvas;

        if (canvas)
            raster_draw_buffer_free(canvas);

        video_canvas_destroy(raster->viewport.canvas);
    }
    free(raster->viewport.title);
    free(raster->modes);
    free(raster->sprite_status);
    free(raster->cache);
    free(raster->palette);
    free(raster->fake_draw_buffer_line);
    /* FIXME: there may be more stuff to be freed */
}

raster_t *raster_get_raster_from_canvas(struct video_canvas_s *canvas)
{
    raster_list_t *rasters = ActiveRasters;

    while (rasters != NULL) {
        if (rasters->raster->viewport.canvas == canvas)
            return rasters->raster;

        rasters = rasters->next;
    }

    return NULL;	
}

