/*
 * raster.c - Raster-based video chip emulation helper.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "machine.h"
#include "utils.h"
#include "video.h"

#include "raster.h"

static void update_pixel_tables (raster_t *raster);
static int realize_canvas (raster_t *raster);
static int realize_frame_buffer (raster_t *raster);
static void update_canvas (raster_t *raster);
static void update_canvas_all (raster_t *raster);



static void
update_pixel_tables (raster_t *raster)
{
  unsigned int i;

  /* Prepare the double and quad pixel tables according to the colormap
     of the canvas.  */
  for (i = 0; i < 0x100; i++)
    *((PIXEL *) (raster->pixel_table.doub + i))
      = *((PIXEL *) (raster->pixel_table.doub + i) + 1)
      = raster->pixel_table.sing[i];
  for (i = 0; i < 0x100; i++)
    *((PIXEL2 *) (raster->pixel_table.quad + i))
      = *((PIXEL2 *) (raster->pixel_table.quad + i) + 1)
      = raster->pixel_table.doub[i];
}

static int
realize_canvas (raster_t *raster)
{
  raster_viewport_t *viewport;

  viewport = &raster->viewport;

  if (viewport->canvas == NULL)
    {
      viewport->canvas = canvas_create (viewport->title,
                                        &viewport->width,
                                        &viewport->height,
                                        1,
                                        raster->viewport.exposure_handler,
                                        raster->palette,
                                        raster->pixel_table.sing);

      if (viewport->canvas == NULL)
        return -1;

      update_pixel_tables (raster);
    }
  else
    canvas_resize (viewport->canvas, viewport->width, viewport->height);

  /* The canvas might give us something different from what we
     requested.  */
  raster_resize_viewport (raster, viewport->width, viewport->height);

  return 0;
}

static int
realize_frame_buffer (raster_t *raster)
{
  unsigned int fb_width, fb_height;

#if 0
  /* Boooh...  Amazingly broken API.  FIXME.  */
  if (raster->frame_buffer != NULL)
    frame_buffer_free (&raster->frame_buffer);
#else
  frame_buffer_free (&raster->frame_buffer);
#endif

  fb_width = ((raster->geometry.screen_size.width
               + raster->geometry.extra_offscreen_border)
              * raster->viewport.pixel_size.width);
  fb_height = (raster->geometry.screen_size.height
               * raster->viewport.pixel_size.height);
  if (fb_width == 0)
    fb_width = 1;
  if (fb_height == 0)
    fb_height = 1;

  if (frame_buffer_alloc (&raster->frame_buffer, fb_width, fb_height))
    return -1;

  frame_buffer_clear (&raster->frame_buffer, RASTER_PIXEL (raster, 0));

  if (raster->fake_frame_buffer_line != NULL)
    free (raster->fake_frame_buffer_line);
  raster->fake_frame_buffer_line = xmalloc (fb_width * sizeof (PIXEL));

  return 0;
}

/* Recalculate frame buffer for new display mode? */
#ifdef __riscos
#if ((X_DISPLAY_DEPTH == 8) || (X_DISPLAY_DEPTH == 0))
#define RECALC_FRAME_BUFFER
#endif
#endif

static void
perform_mode_change(raster_t *raster)
{
  raster_viewport_t *viewport;
  canvas_t canvas;
#ifdef RECALC_FRAME_BUFFER
  PIXEL old_colours[256];
  PIXEL colour_map[256];
  unsigned int i, num_pixels;
  unsigned int num_colours;
  PIXEL *fb;

  memcpy(old_colours, raster->pixel_table.sing, 256);
#endif

  viewport = &raster->viewport;

  if ((canvas = viewport->canvas) == NULL)
    return;

  canvas_set_palette(canvas, raster->palette, raster->pixel_table.sing);

#ifdef RECALC_FRAME_BUFFER
  memset(colour_map, 0, 256);
  num_colours = raster->palette->num_entries;

  for (i=0; i<num_colours; i++) colour_map[old_colours[i]] = i;
  for (i=0; i<256; i++) colour_map[i] = (raster->pixel_table.sing)[colour_map[i]];

  num_pixels = raster->frame_buffer.width * raster->frame_buffer.height;
  fb = raster->frame_buffer.tmpframebuffer;
  for (i=0; i<num_pixels; i++) fb[i] = colour_map[fb[i]];
#endif

  update_pixel_tables(raster);

  if (raster->refresh_tables != NULL)
    raster->refresh_tables();

  raster_force_repaint(raster);

  canvas_resize(canvas, viewport->width, viewport->height);
  raster_resize_viewport(raster, viewport->width, viewport->height);
}






/* Increase `area' so that it also includes [xs; xe] at line y.  WARNING:
   this must be called in order, from top to bottom.  */
inline static void
add_line_to_area (raster_area_t *area,
                  unsigned int y,
                  unsigned int xs,
                  unsigned int xe)
{
  if (area->is_null)
    {
      area->ys = area->ye = y;
      area->xs = xs;
      area->xe = xe;
      area->is_null = 0;
    }
  else
    {
      area->xs = MIN (xs, area->xs);
      area->xe = MAX (xe, area->xe);
      area->ye = y;
    }
}



inline static unsigned int
get_real_mode (raster_t *raster)
{
  if (raster->draw_idle_state)
    return raster_modes_get_idle_mode (&raster->modes);
  else
    return raster->video_mode;
}

static void
update_canvas (raster_t *raster)
{
  raster_area_t *update_area;
  raster_viewport_t *viewport;
  int x, y, xx, yy;
  int w, h;

  update_area = &raster->update_area;
  viewport = &raster->viewport;

  if (update_area->is_null)
    return;

  x = update_area->xs;
  y = update_area->ys;
  xx = update_area->xs - viewport->first_x;
  yy = update_area->ys - viewport->first_line;
  w = update_area->xe - update_area->xs + 1;
  h = update_area->ye - update_area->ys + 1;

  if (xx < 0)
    {
      x -= xx;
      w += xx;
      xx = 0;
    }
  if (yy < 0)
    {
      y -= yy;
      h += yy;
      yy = 0;
    }

  x *= viewport->pixel_size.width;
  xx *= viewport->pixel_size.width;
  w *= viewport->pixel_size.width;

  y *= viewport->pixel_size.height;
  yy *= viewport->pixel_size.height;
  h *= viewport->pixel_size.height;

  x += raster->geometry.extra_offscreen_border;

  canvas_refresh (raster->viewport.canvas,
                  raster->frame_buffer,
                  x, y,
                  xx + viewport->x_offset, yy + viewport->y_offset,
                  w, h);

  update_area->is_null = 1;
}

static void
update_canvas_all (raster_t *raster)
{
  raster_viewport_t *viewport;

  viewport = &raster->viewport;

  canvas_refresh (viewport->canvas,
                  raster->frame_buffer,
                  (viewport->first_x * viewport->pixel_size.width
                   + raster->geometry.extra_offscreen_border),
                  viewport->first_line * viewport->pixel_size.height,
                  viewport->x_offset,
                  viewport->y_offset,
                  MIN (viewport->width,
                       (raster->geometry.screen_size.width
                        * viewport->pixel_size.width)),
                  MIN (viewport->height,
                       (raster->geometry.screen_size.height
                        * viewport->pixel_size.height)));
}



inline static void
draw_sprites (raster_t *raster)
{
  if (raster->sprite_status.draw_function != NULL)
    raster->sprite_status.draw_function (raster->frame_buffer_ptr,
                                         raster->gfx_msk);
}

inline static void
draw_sprites_when_cache_enabled (raster_t *raster,
                                 raster_cache_t *cache)
{
  if (raster->sprite_status.draw_function == NULL)
    return;

  raster->sprite_status.draw_function (raster->frame_buffer_ptr,
                                       cache->gfx_msk);
  cache->sprite_sprite_collisions
    = raster->sprite_status.sprite_sprite_collisions;
  cache->sprite_background_collisions
    = raster->sprite_status.sprite_background_collisions;
}

/* This kludge updates the sprite-sprite collisions without writing to the
   real frame buffer.  We might write a function that actually checks for
   collisions only, but we are lazy.  */
inline static void
update_sprite_collisions (raster_t *raster)
{
  PIXEL *fake_frame_buffer_ptr;

  if (console_mode || psid_mode) {
    return;
  }

  if (raster->sprite_status.draw_function == NULL)
    return;

  fake_frame_buffer_ptr = (raster->fake_frame_buffer_line
                           + raster->geometry.extra_offscreen_border);
  raster->sprite_status.draw_function (fake_frame_buffer_ptr,
                                       raster->zero_gfx_msk);
}

inline static void
draw_blank (raster_t *raster,
            unsigned int start,
            unsigned int end)
{
  unsigned int pixel_width;

  pixel_width = raster->viewport.pixel_size.width;
  vid_memset ((BYTE *) raster->frame_buffer_ptr + start * pixel_width,
              RASTER_PIXEL (raster, raster->border_color),
              (end - start + 1) * pixel_width);
}

inline static void
add_line_and_double_scan (raster_t *raster,
                          unsigned int start,
                          unsigned int end)
{
  unsigned int pixel_width;

  add_line_to_area (&raster->update_area,
                    raster->current_line,
                    0, raster->geometry.screen_size.width - 1);

  if (raster->viewport.pixel_size.height != 2 || !raster->do_double_scan)
    return;

  pixel_width = raster->viewport.pixel_size.width;

  vid_memcpy ((FRAME_BUFFER_LINE_START (raster->frame_buffer,
                                        2 * raster->current_line + 1)
               + raster->geometry.extra_offscreen_border
               + start * pixel_width),
              raster->frame_buffer_ptr + start * pixel_width,
              (end - start + 1) * pixel_width);

  add_line_to_area (&raster->update_area,
                    raster->current_line + 1,
                    start, end);
}



inline static void
handle_blank_line (raster_t *raster)
{
  unsigned int pixel_width;

  if (console_mode || psid_mode) {
    return;
  }

  pixel_width = raster->viewport.pixel_size.width;

  /* Changes... Should/could be handled better.  */
  if (raster->changes.have_on_this_line)
    {
      raster_changes_t *border_changes;
      unsigned int i, xs;

      raster_changes_apply_all (&raster->changes.background);
      raster_changes_apply_all (&raster->changes.foreground);

      border_changes = &raster->changes.border;

      for (xs = i = 0; i < border_changes->count; i++)
        {
          unsigned int xe;

          xe = border_changes->actions[i].where;

          if (xs < xe)
            {
              draw_blank (raster, xs, xe);
              xs = xe;
            }

          raster_changes_apply (border_changes, i);
        }

      if (xs < raster->geometry.screen_size.width - 1)
        draw_blank (raster,
                    xs, raster->geometry.screen_size.width - 1);

      raster_changes_remove_all (border_changes);
      raster->changes.have_on_this_line = 0;

      raster->cache[raster->current_line].border_color = -1;
      raster->cache[raster->current_line].blank = 1;

      add_line_and_double_scan (raster,
                                0, raster->geometry.screen_size.width - 1);
    }
  else if (CANVAS_USES_TRIPLE_BUFFERING (raster->viewport.canvas)
           || raster->dont_cache
           || raster->cache[raster->current_line].is_dirty
           || (raster->border_color
               != raster->cache[raster->current_line].border_color)
           || !raster->cache[raster->current_line].blank)
    {

      /* Even when the actual caching is disabled, redraw blank lines only
         if it is really necessary to do so.  */
      raster->cache[raster->current_line].border_color = raster->border_color;
      raster->cache[raster->current_line].blank = 1;
      raster->cache[raster->current_line].is_dirty = 0;

      draw_blank (raster,
                  0, raster->geometry.screen_size.width - 1);
      add_line_to_area (&raster->update_area,
                        raster->current_line,
                        0, raster->geometry.screen_size.width - 1);

      if (raster->viewport.pixel_size.height == 2
          && raster->do_double_scan)
        {
          vid_memset ((FRAME_BUFFER_LINE_START (raster->frame_buffer,
                                                2 * raster->current_line + 1)
                       + raster->geometry.extra_offscreen_border),
                      RASTER_PIXEL (raster, raster->border_color),
                    (raster->geometry.screen_size.width - 1) * pixel_width);
          add_line_to_area (&raster->update_area,
                            raster->current_line,
                            0, raster->geometry.screen_size.width - 1);
        }
    }

  update_sprite_collisions (raster);
}

/* Draw the borders.  */
inline static void
draw_borders (raster_t *raster)
{
  if (!raster->open_left_border)
    draw_blank (raster, 0, raster->display_xstart - 1);
  if (!raster->open_right_border)
    draw_blank (raster,
                raster->display_xstop,
                raster->geometry.screen_size.width - 1);
}

inline static int
check_for_major_changes_and_update (raster_t *raster,
                                    int *changed_start,
                                    int *changed_end)
{
  raster_cache_t *cache;
  unsigned int video_mode;
  int line;

  video_mode = get_real_mode (raster);

  cache = raster->cache + raster->current_line;
  line = (raster->current_line
          - raster->geometry.gfx_position.y
          - raster->ysmooth
          - 1);

  if (cache->is_dirty
      || raster->dont_cache
      || cache->n != line
      || cache->xsmooth != raster->xsmooth
      || cache->video_mode != video_mode
      || cache->blank
      || cache->ycounter != raster->ycounter
      || cache->border_color != raster->border_color
      || cache->display_xstart != raster->display_xstart
      || cache->display_xstop != raster->display_xstop
      || (cache->open_right_border && !raster->open_right_border)
      || (cache->open_left_border && !raster->open_left_border)
      || (cache->overscan_background_color
          != raster->overscan_background_color))
    {

      unsigned int pixel_width = raster->viewport.pixel_size.width;
      int changed_start_char, changed_end_char;
      int r;

      cache->n = line;
      cache->xsmooth = raster->xsmooth;
      cache->video_mode = video_mode;
      cache->blank = 0;
      cache->ycounter = raster->ycounter;
      cache->border_color = raster->border_color;
      cache->display_xstart = raster->display_xstart;
      cache->display_xstop = raster->display_xstop;
      cache->open_right_border = raster->open_right_border;
      cache->open_left_border = raster->open_left_border;
      cache->overscan_background_color = raster->overscan_background_color;

      /* Fill the space between the border and the graphics with the
         background color (necessary if `xsmooth' is != 0).  */
      if (raster->xsmooth != 0)
        vid_memset ((raster->frame_buffer_ptr
                     + raster->geometry.gfx_position.x * pixel_width),
                    RASTER_PIXEL (raster, raster->overscan_background_color),
                    raster->xsmooth * pixel_width);

      if (raster->open_left_border)
        vid_memset (raster->frame_buffer_ptr,
                    RASTER_PIXEL (raster, raster->overscan_background_color),
                    ((raster->geometry.gfx_position.x + raster->xsmooth)
                     * pixel_width));

      if (raster->open_right_border)
        vid_memset ((raster->frame_buffer_ptr +
                     ((raster->geometry.gfx_position.x
                       + raster->geometry.gfx_size.width
                       + raster->xsmooth)
                      * pixel_width)),
                    RASTER_PIXEL (raster, raster->overscan_background_color),
                    ((raster->geometry.screen_size.width
                      - raster->geometry.gfx_position.x
                      - raster->geometry.gfx_size.width
                      - raster->xsmooth) * pixel_width));

      raster_fill_sprite_cache (raster, cache,
                                &changed_start_char,
                                &changed_end_char);

      r = raster_modes_fill_cache (&raster->modes,
                                   video_mode,
                                   cache,
                                   &changed_start_char,
                                   &changed_end_char, 1);

      /* [ `changed_start' ; `changed_end' ] now covers the whole line, as
         we have called fill_cache() with `1' as the last parameter (no
         check).  */
      raster_modes_draw_line_cached (&raster->modes, video_mode,
                                     cache,
                                     changed_start_char,
                                     changed_end_char);
      draw_sprites_when_cache_enabled (raster, cache);

      *changed_start = 0;
      *changed_end = raster->geometry.screen_size.width - 1;

      draw_borders (raster);

      return 1;

    }
  else
    return 0;
}

inline static int
update_for_minor_changes_without_sprites (raster_t *raster,
                                          int *changed_start,
                                          int *changed_end)
{
  raster_cache_t *cache;
  unsigned int video_mode;
  unsigned int pixel_width;
  int changed_start_char, changed_end_char;
  int needs_update;

  video_mode = get_real_mode (raster);

  cache = raster->cache + raster->current_line;

  changed_start_char = raster->geometry.text_size.width;
  changed_end_char = -1;

  pixel_width = raster->viewport.pixel_size.width;

  needs_update = raster_modes_fill_cache (&raster->modes,
                                          video_mode,
                                          cache,
                                          &changed_start_char,
                                          &changed_end_char,
                                          0);

  if (needs_update)
    {
      raster_modes_draw_line_cached (&raster->modes,
                                     video_mode,
                                     cache,
                                     changed_start_char,
                                     changed_end_char);
      draw_sprites_when_cache_enabled (raster, cache);

      /* Convert from character to pixel coordinates.  FIXME: Hardcoded
         `8'.  */
      *changed_start = (raster->geometry.gfx_position.x
                        + raster->xsmooth
                        + 8 * changed_start_char);

      *changed_end = (raster->geometry.gfx_position.y
                      + raster->xsmooth
                      + 8 * (changed_end_char + 1)
                      - 1);
    }

  /* FIXME: Why always doing so?  */
  draw_borders (raster);

  return needs_update;
}

inline static int
update_for_minor_changes_with_sprites (raster_t *raster,
                                       int *changed_start,
                                       int *changed_end)
{
  raster_cache_t *cache;
  unsigned int video_mode;
  unsigned int pixel_width;
  int sprite_changed_start, sprite_changed_end;
  int changed_start_char, changed_end_char;
  int sprites_need_update;
  int needs_update;

  video_mode = get_real_mode (raster);

  cache = raster->cache + raster->current_line;

  changed_start_char = raster->geometry.text_size.width;
  changed_end_char = -1;

  pixel_width = raster->viewport.pixel_size.width;

  sprites_need_update = raster_fill_sprite_cache (raster,
                                                  cache,
                                                  &sprite_changed_start,
                                                  &sprite_changed_end);

  /* If sprites have changed, do not bother trying to reduce the amount
     of recalculated data, but simply redraw everything.  */
  needs_update = raster_modes_fill_cache (&raster->modes,
                                          video_mode,
                                          cache,
                                          &changed_start_char,
                                          &changed_end_char,
                                          sprites_need_update);

  /* If the background color changes, we might get the wrong color in
     the left part of the screen, between the border and the start of
     the graphics.  */
  if (raster->ysmooth > 0
      && (cache->overscan_background_color
          != raster->overscan_background_color))
    needs_update = 1;

  if (needs_update)
    {
      raster_modes_draw_line_cached (&raster->modes,
                                     video_mode,
                                     cache,
                                     changed_start_char,
                                     changed_end_char);

      /* Fill the space between the border and the graphics with the
         background color (necessary if xsmooth is > 0).  */
      vid_memset (raster->frame_buffer_ptr
                  + raster->geometry.gfx_position.x * pixel_width,
                  RASTER_PIXEL (raster, raster->overscan_background_color),
                  raster->xsmooth * pixel_width);

      /* If xsmooth > 0, drawing the graphics might have corrupted
         part of the border... fix it here.  */
      if (!raster->open_right_border)
        draw_blank (raster,
                    (raster->geometry.gfx_position.x
                     + raster->geometry.gfx_size.width),
                    (raster->geometry.gfx_position.x
                     + raster->geometry.gfx_size.width + 8));

      /* Calculate the interval in pixel coordinates.  */

      if (cache->overscan_background_color
          != raster->overscan_background_color)
        {
          /* FIXME: ???  */
          if (raster->ysmooth > 0)
            *changed_start = raster->geometry.gfx_position.x;
          else
            *changed_start = (raster->geometry.gfx_position.x
                              + raster->xsmooth
                              + 8 * changed_start_char);

          cache->overscan_background_color
            = raster->overscan_background_color;
        }
      else
        *changed_start = (raster->geometry.gfx_position.x
                          + raster->xsmooth
                          + 8 * changed_start_char);

      *changed_end = (raster->geometry.gfx_position.x
                      + raster->xsmooth
                      + 8 * (changed_end_char + 1)
                      - 1);

      if (sprites_need_update)
        {
          /* FIXME: wrong.  */
          if (raster->open_left_border)
            *changed_start = 0;
          if (raster->open_right_border)
            *changed_end = raster->geometry.screen_size.width - 1;

          /* FIXME: Could be optimized better.  */
          draw_sprites_when_cache_enabled (raster, cache);
          draw_borders (raster);

          /* Even if we have recalculated the whole line, we will
             refresh only the part that has actually changed when
             writing to the window.  */
          *changed_start = MIN (*changed_start, sprite_changed_start);
          *changed_end = MAX (*changed_end, sprite_changed_end);

          /* The borders have not changed, so do not repaint them even
             if there are sprites under them.  */
          *changed_start = MAX (*changed_start, raster->display_xstart);
          *changed_end = MIN (*changed_end, raster->display_xstop);
        }
    }

  if (! sprites_need_update)
    {
      raster->sprite_status.sprite_sprite_collisions
        = cache->sprite_sprite_collisions;
      raster->sprite_status.sprite_background_collisions
        = cache->sprite_background_collisions;
    }

  return needs_update;
}

inline static int
update_for_minor_changes (raster_t *raster,
                          int *changed_start,
                          int *changed_end)
{
  if (raster->sprite_status.num_sprites > 0)
    return update_for_minor_changes_with_sprites (raster,
                                                  changed_start,
                                                  changed_end);
  else
    return update_for_minor_changes_without_sprites (raster,
                                                     changed_start,
                                                     changed_end);
}

inline static void
handle_visible_line_with_cache (raster_t *raster)
{
  int needs_update;
  int changed_start, changed_end;
  raster_cache_t *cache;

  cache = &raster->cache[raster->current_line];

  /* Check for "major" changes first.  If there is any, just write straight
     to the cache without any comparisons and redraw the whole line.  */
  needs_update = check_for_major_changes_and_update (raster,
                                                     &changed_start,
                                                     &changed_end);
  if (!needs_update)
    {
      /* There are no `major' changes: try to do some optimization.  */
      needs_update = update_for_minor_changes (raster,
                                               &changed_start,
                                               &changed_end);
    }

  if (needs_update)
    add_line_and_double_scan (raster, changed_start, changed_end);

  cache->is_dirty = 0;
}

inline static void
handle_visible_line_without_cache (raster_t *raster)
{
  unsigned int pixel_width;
  raster_geometry_t *geometry;
  raster_cache_t *cache;

  pixel_width = raster->viewport.pixel_size.width;
  geometry = &raster->geometry;

  /* If screen is scrolled to the right, we need to fill with the
     background color the blank part on the left.  */
  vid_memset ((raster->frame_buffer_ptr
               + geometry->gfx_position.x * pixel_width),
              RASTER_PIXEL (raster, raster->overscan_background_color),
              raster->xsmooth * pixel_width);

  if (raster->open_left_border)
    vid_memset (raster->frame_buffer_ptr,
                RASTER_PIXEL (raster, raster->overscan_background_color),
                ((geometry->gfx_position.x
                  + raster->xsmooth) * pixel_width));
  if (raster->open_right_border)
    vid_memset ((raster->frame_buffer_ptr +
                 ((geometry->gfx_position.x
                   + geometry->gfx_size.width
                   + raster->xsmooth)
                  * pixel_width)),
                RASTER_PIXEL (raster, raster->overscan_background_color),
                (geometry->screen_size.width
                 - geometry->gfx_position.x
                 - geometry->gfx_size.width
                 - raster->xsmooth) * pixel_width);

  /* Draw the graphics and sprites.  */
  raster_modes_draw_line (&raster->modes, get_real_mode (raster));
  draw_sprites (raster);
  draw_borders (raster);

  cache = &raster->cache[raster->current_line];

  if (CANVAS_USES_TRIPLE_BUFFERING (raster->viewport.canvas)
      || raster->dont_cache
      || raster->sprite_status.dma_msk != 0
      || cache->is_dirty
      || cache->blank
      || cache->border_color != raster->border_color
  /* FIXME: Done differently in another place.  */
      || (cache->open_right_border != raster->open_right_border)
      || (cache->open_left_border != raster->open_left_border)
      || (cache->overscan_background_color
          != raster->overscan_background_color)
    )
    {
      cache->blank = 0;
      cache->is_dirty = 0;
      cache->border_color = raster->border_color;
      cache->open_right_border = raster->open_right_border;
      cache->open_left_border = raster->open_left_border;
      cache->overscan_background_color = raster->overscan_background_color;

      add_line_and_double_scan (raster,
                                0,
                                raster->geometry.screen_size.width - 1);
    }
  else
    {
      /* Still do some minimal caching anyway.  */
      /* Only update the part between the borders.  */
      add_line_and_double_scan (raster,
                                geometry->gfx_position.x,
                                (geometry->gfx_position.x
                                 + geometry->gfx_size.width - 1));
    }
}

inline static void
handle_visible_line_with_changes (raster_t *raster)
{
  unsigned int i;
  unsigned int pixel_width;
  int xs, xstop;
  raster_geometry_t *geometry;

  pixel_width = raster->viewport.pixel_size.width;
  geometry = &raster->geometry;

  /* Draw the background.  */
  for (xs = i = 0; i < raster->changes.background.count; i++)
    {
      int xe = raster->changes.background.actions[i].where;

      if (xs < xe)
        {
          raster_modes_draw_background (&raster->modes,
                                        get_real_mode (raster),
                                        xs,
                                        xe - 1);
          xs = xe;
        }
      raster_changes_apply (&raster->changes.background, i);
    }
  if (xs <= (int) geometry->screen_size.width - 1)
    raster_modes_draw_background (&raster->modes,
                                  get_real_mode (raster),
                                  xs,
                                  geometry->screen_size.width - 1);

  /* Draw the foreground graphics.  */
  for (xs = i = 0; i < raster->changes.foreground.count; i++)
    {
      int xe = raster->changes.foreground.actions[i].where;

      if (xs < xe)
        {
          raster_modes_draw_foreground (&raster->modes,
                                        get_real_mode (raster),
                                        xs,
                                        xe - 1);
          xs = xe;
        }
      raster_changes_apply (&raster->changes.foreground, i);
    }
  if (xs <= (int) geometry->text_size.width - 1)
    raster_modes_draw_foreground (&raster->modes,
                                  get_real_mode (raster),
                                  xs,
                                  geometry->text_size.width - 1);

  if (raster->sprite_status.draw_function != NULL)
    raster->sprite_status.draw_function (raster->frame_buffer_ptr,
                                         raster->gfx_msk);

  /* Draw left border.  */
  xstop = raster->display_xstart - 1;
  if (!raster->open_left_border)
    {
      for (xs = i = 0;
           (i < raster->changes.border.count
            && raster->changes.border.actions[i].where <= xstop);
           i++)
        {
          int xe = raster->changes.border.actions[i].where;

          if (xs < xe)
            {
              draw_blank (raster, xs, xe - 1);
              xs = xe;
            }
          raster_changes_apply (&raster->changes.border, i);
        }
      if (xs <= xstop)
        draw_blank (raster, xs, xstop);
    }
  else
    {
      for (i = 0;
           (i < raster->changes.border.count
            && raster->changes.border.actions[i].where <= xstop);
           i++)
        raster_changes_apply (&raster->changes.border, i);
    }

  /* Draw right border.  */
  if (!raster->open_right_border)
    {
      for (;
           (i < raster->changes.border.count
            && (raster->changes.border.actions[i].where
                <= raster->display_xstop));
           i++)
        raster_changes_apply (&raster->changes.border, i);
      for (xs = raster->display_xstop;
           i < raster->changes.border.count;
           i++)
        {
          int xe = raster->changes.border.actions[i].where;

          if (xs < xe)
            {
              draw_blank (raster, xs, xe - 1);
              xs = xe;
            }
          raster_changes_apply (&raster->changes.border, i);
        }
      if (xs <= (int) geometry->screen_size.width - 1)
        draw_blank (raster, xs, geometry->screen_size.width - 1);
    }
  else
    {
      for (i = 0; i < raster->changes.border.count; i++)
        raster_changes_apply (&raster->changes.border, i);
    }

  raster_changes_remove_all (&raster->changes.foreground);
  raster_changes_remove_all (&raster->changes.background);
  raster_changes_remove_all (&raster->changes.border);
  raster->changes.have_on_this_line = 0;

  /* Do not cache this line at all.  */
  raster->cache[raster->current_line].is_dirty = 1;

  add_line_and_double_scan (raster,
                            0, raster->geometry.screen_size.width - 1);

#if 0
  /* This is a dirty hack for use with GDB.  */
  if (raster->current_line == _hidden_hideous_raster_check)
    vid_memset (raster->frame_buffer_ptr,
                RASTER_PIXEL (raster, 0),
                geometry->screen_size.width - 1);
#endif
}

inline static void
handle_visible_line (raster_t *raster)
{
  if (console_mode || psid_mode) {
    return;
  }

  if (raster->changes.have_on_this_line)
    handle_visible_line_with_changes (raster);
  else if (!CANVAS_USES_TRIPLE_BUFFERING (raster->viewport.canvas)
           && raster->cache_enabled
           && !raster->open_left_border
           && !raster->open_right_border)       /* FIXME: shortcut! */
    handle_visible_line_with_cache (raster);
  else
    handle_visible_line_without_cache (raster);
}

inline static void
handle_end_of_frame (raster_t *raster)
{
  /* FIXME for SCREEN_MAX_SPRITE_WIDTH */
  raster->frame_buffer_ptr = (FRAME_BUFFER_START (raster->frame_buffer)
                              + raster->geometry.extra_offscreen_border);

  raster->current_line = 0;

  if (!raster->skip_frame)
    {
      if (raster->dont_cache)
        update_canvas_all (raster);
      else
        update_canvas (raster);
    }
}



static void
raster_viewport_init (raster_viewport_t *viewport)
{
  viewport->canvas = NULL;
  viewport->width = viewport->height = 0;
  viewport->title = NULL;
  viewport->x_offset = viewport->y_offset = 0;
  viewport->first_line = viewport->last_line = 0;
  viewport->first_x = 0;
  viewport->pixel_size.width = viewport->pixel_size.height = 1;
  viewport->exposure_handler = NULL;
}

static void
raster_geometry_init (raster_geometry_t *geometry)
{
  geometry->screen_size.width = geometry->screen_size.height = 0;
  geometry->gfx_size.width = geometry->gfx_size.height = 0;
  geometry->text_size.width = geometry->text_size.height = 0;
  geometry->gfx_position.x = geometry->gfx_position.y = 0;
  geometry->gfx_area_moves = 0;
  geometry->first_displayed_line = 0;
  geometry->last_displayed_line = 0;
}

void
raster_init (raster_t *raster,
             unsigned int num_modes,
             unsigned int num_sprites)
{
  raster_viewport_init (&raster->viewport);
  raster_geometry_init (&raster->geometry);
  raster_modes_init (&raster->modes, num_modes);
  raster_sprite_status_init (&raster->sprite_status, num_sprites);

  /* Woo!  This sucks real bad!  FIXME!  */
  frame_buffer_alloc (&raster->frame_buffer, 1, 1);

  raster_reset (raster);

  raster->palette = NULL;

  raster->display_xstart = raster->display_xstop = 0;
  raster->display_ystart = raster->display_ystop = 0;

  raster->cache = NULL;
  raster->cache_enabled = 0;
  raster->dont_cache = 1;
  raster->num_cached_lines = 0;

  raster->update_area.is_null = 1;

  raster->do_double_scan = 0;

  raster->fake_frame_buffer_line = NULL;

  raster->refresh_tables = NULL;

  raster->border_color = 0;
  raster->background_color = 0;
  raster->overscan_background_color = 0;

  memset (raster->gfx_msk, 0, RASTER_GFX_MSK_SIZE);
  memset (raster->zero_gfx_msk, 0, RASTER_GFX_MSK_SIZE);
}

void
raster_reset (raster_t *raster)
{
  raster_changes_remove_all (&raster->changes.background);
  raster_changes_remove_all (&raster->changes.foreground);
  raster_changes_remove_all (&raster->changes.border);
  raster_changes_remove_all (&raster->changes.next_line);
  raster->changes.have_on_this_line = 0;

  raster->frame_buffer_ptr = (FRAME_BUFFER_START (raster->frame_buffer)
                              + 2 * raster->geometry.extra_offscreen_border);
  raster->current_line = 0;

  raster->xsmooth = raster->ysmooth = 0;
  raster->skip_frame = 0;

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

raster_t *
raster_new (unsigned int num_modes,
            unsigned int num_sprites)
{
  raster_t *new;

  new = xmalloc (sizeof (raster_t));

  raster_init (new, num_modes, num_sprites);

  return new;
}

void
raster_mode_change(void)
{
  raster_list_t *rasters = ActiveRasters;

  while (rasters != NULL)
  {
    perform_mode_change(rasters->raster);
    rasters = rasters->next;
  }
}


void
raster_set_geometry (raster_t *raster,
                     unsigned int screen_width, unsigned int screen_height,
                     unsigned int gfx_width, unsigned int gfx_height,
                     unsigned int text_width, unsigned int text_height,
                   unsigned int gfx_position_x, unsigned int gfx_position_y,
                     int gfx_area_moves,
                     unsigned int first_displayed_line,
                     unsigned int last_displayed_line,
                     unsigned int extra_offscreen_border)
{
  raster_geometry_t *geometry;

  geometry = &raster->geometry;
  if (screen_height != geometry->screen_size.height || raster->cache == NULL) {
      unsigned int i;

      raster->cache = xrealloc (raster->cache,
                                sizeof (*raster->cache) * screen_height);
      for (i = 0; i < screen_height; i++)
           raster_cache_init (raster->cache + i);
  }

  if (geometry->screen_size.width != screen_width
      || geometry->screen_size.height != screen_height
      || geometry->extra_offscreen_border != extra_offscreen_border)
    {
      geometry->screen_size.width = screen_width;
      geometry->screen_size.height = screen_height;
      geometry->extra_offscreen_border = extra_offscreen_border;
      realize_frame_buffer (raster);
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

void
raster_set_exposure_handler (raster_t *raster,
                             canvas_redraw_t exposure_handler)
{
  raster->viewport.exposure_handler = exposure_handler;
}

void
raster_set_table_refresh_handler (raster_t *raster,
                             void (*handler)(void))
{
  raster->refresh_tables = handler;
}


int
raster_realize (raster_t *raster)
{
  raster_list_t *rlist;

  if (realize_canvas (raster) < 0)
    return -1;

  update_canvas_all (raster);

  rlist = (raster_list_t*)xmalloc(sizeof(raster_list_t));
  rlist->raster = raster;
  rlist->next = NULL;
  if (ActiveRasters == NULL)
  {
    ActiveRasters = rlist;
  }
  else
  {
    raster_list_t *rasters = ActiveRasters;

    while (rasters->next != NULL) rasters = rasters->next;
    rasters->next = rlist;
  }
  return 0;
}

/* Resize the canvas with the specified values and center the screen image on
   it.  The actual size can be different if the parameters are not
   suitable.  */
void
raster_resize_viewport (raster_t *raster,
                        unsigned int width,
                        unsigned int height)
{
  raster_geometry_t *geometry;
  raster_viewport_t *viewport;
  raster_rectangle_t *screen_size;
  raster_rectangle_t *pixel_size;
  raster_rectangle_t *gfx_size;
  raster_position_t *gfx_position;

  geometry = &raster->geometry;

  screen_size = &geometry->screen_size;
  gfx_size = &geometry->gfx_size;
  gfx_position = &geometry->gfx_position;

  viewport = &raster->viewport;

  pixel_size = &viewport->pixel_size;

  if (width >= screen_size->width * pixel_size->width)
    {
      viewport->x_offset = (width
                            - (screen_size->width * pixel_size->width)) / 2;
      viewport->first_x = 0;
    }
  else
    {
      viewport->x_offset = 0;

      if (geometry->gfx_area_moves)
        viewport->first_x = (screen_size->width
                             - width / pixel_size->width) / 2;
      else if (width > gfx_size->width * pixel_size->width)
        viewport->first_x = (gfx_position->x
                             - (width / pixel_size->width
                                - gfx_size->width) / 2);
      else
        viewport->first_x = gfx_position->x;
    }

  if (height >= screen_size->height * pixel_size->height)
    {
      viewport->y_offset = (height
                            - screen_size->height * pixel_size->height) / 2;
      viewport->first_line = 0;
      viewport->last_line = viewport->first_line + screen_size->height - 1;
    }
  else
    {
      viewport->y_offset = 0;

      if (geometry->gfx_area_moves)
        viewport->first_line = (screen_size->height
                                - height / pixel_size->height) / 2;
      /* FIXME: Somewhat buggy.  */
      else if (height > gfx_size->height * pixel_size->height)
        viewport->first_line = (gfx_position->y
                                - (height / pixel_size->height
                                   - gfx_size->height) / 2);
      else
        viewport->first_line = gfx_position->y;

      viewport->last_line = (viewport->first_line
                             + height / pixel_size->height);
    }

  /* Hmmm....  FIXME?  */
  if (viewport->canvas != NULL)
    canvas_resize (viewport->canvas, width, height);

  viewport->width = width;
  viewport->height = height;

  /* Make sure we don't waste space showing unused lines.  */
  if ((viewport->first_line < geometry->first_displayed_line
       && viewport->last_line < geometry->last_displayed_line)
      || (viewport->first_line > geometry->first_displayed_line
          && viewport->last_line > geometry->last_displayed_line))
    {
      viewport->first_line = geometry->first_displayed_line;
      viewport->last_line = (geometry->first_displayed_line
                             + height / pixel_size->height);
    }
}

void
raster_set_pixel_size (raster_t *raster,
                       unsigned int width,
                       unsigned int height)
{
  raster->viewport.pixel_size.width = width;
  raster->viewport.pixel_size.height = height;

  realize_frame_buffer (raster);
  raster_force_repaint (raster);
}



/* Emulate one raster line.  */
void
raster_emulate_line (raster_t *raster)
{
  raster_viewport_t *viewport;

  viewport = &raster->viewport;

  /* Emulate the vertical blank flip-flops.  (Well, sort of.)  */
  if (raster->current_line == raster->display_ystart && !raster->blank)
    raster->blank_enabled = 0;
  else if (raster->current_line == raster->display_ystop)
    raster->blank_enabled = 1;

  if (raster->current_line >= raster->geometry.first_displayed_line
      && raster->current_line <= raster->geometry.last_displayed_line)
    {
      if (!raster->skip_frame
          && (raster->current_line >= viewport->first_line
              && raster->current_line <= viewport->last_line))
        {
          if ((raster->blank_this_line || raster->blank_enabled)
              && !raster->open_left_border)
            handle_blank_line (raster);
          else
            handle_visible_line (raster);
          if (++raster->num_cached_lines == (viewport->last_line
                                             - viewport->first_line))
            {
              raster->dont_cache = 0;
              raster->num_cached_lines = 0;
            }
        }
      else
        {
          if (!raster->skip_frame)
            update_sprite_collisions (raster);

          if (raster->changes.have_on_this_line)
            {
              raster_changes_apply_all (&raster->changes.background);
              raster_changes_apply_all (&raster->changes.foreground);
              raster_changes_apply_all (&raster->changes.border);
              raster->changes.have_on_this_line = 0;
            }
        }

      raster->current_line++;
      raster->frame_buffer_ptr
        = (FRAME_BUFFER_LINE_START (raster->frame_buffer,
                                    (raster->current_line
                                     * viewport->pixel_size.height))
           + raster->geometry.extra_offscreen_border);
    }
  else
    {
      if (!raster->skip_frame)
        update_sprite_collisions (raster);

      if (raster->changes.have_on_this_line)
        {
          raster_changes_apply_all (&raster->changes.background);
          raster_changes_apply_all (&raster->changes.foreground);
          raster_changes_apply_all (&raster->changes.border);
          raster->changes.have_on_this_line = 0;
        }

      raster->current_line++;

      if (raster->current_line == raster->geometry.screen_size.height)
        handle_end_of_frame (raster);
      else
        raster->frame_buffer_ptr
          = (FRAME_BUFFER_LINE_START (raster->frame_buffer,
                                      (raster->current_line
                                       * viewport->pixel_size.height))
             + raster->geometry.extra_offscreen_border);
    }

  raster_changes_apply_all (&raster->changes.next_line);

  /* Handle open borders.  */
  raster->open_left_border = raster->open_right_border;
  raster->open_right_border = 0;

  if (raster->sprite_status.num_sprites > 0)
    raster->sprite_status.dma_msk = raster->sprite_status.new_dma_msk;

  raster->blank_this_line = 0;
}



void
raster_force_repaint (raster_t *raster)
{
  raster->dont_cache = 1;
  raster->num_cached_lines = 0;

  if (!console_mode && !psid_mode)
      frame_buffer_clear (&raster->frame_buffer, RASTER_PIXEL (raster, 0));
}

void
raster_set_palette (raster_t *raster,
                    palette_t *palette)
{
  if (raster->viewport.canvas != NULL)
    {
      canvas_set_palette (raster->viewport.canvas,
                          palette,
                          raster->pixel_table.sing);
      update_pixel_tables (raster);
    }

  if (raster->palette != NULL)
    palette_free (raster->palette);

  raster->palette = palette;

  raster_force_repaint (raster);
}

void
raster_set_title (raster_t *raster,
                  const char *title)
{
  raster_viewport_t *viewport;

  viewport = &raster->viewport;

  free (viewport->title);
  viewport->title = stralloc (title);

#if 0                           /* FIXME: Not yet in the canvas API.  */
  if (viewport->canvas != NULL)
    canvas_set_title (viewport->canvas, title);
#endif
}

void
raster_skip_frame (raster_t *raster,
                   int skip)
{
  raster->skip_frame = skip;
}

void
raster_enable_cache (raster_t *raster,
                     int enable)
{
  raster->cache_enabled = enable;
  raster_force_repaint (raster);
}

void
raster_enable_double_scan (raster_t *raster,
                           int enable)
{
  raster->do_double_scan = enable;
  raster_force_repaint (raster);
}
