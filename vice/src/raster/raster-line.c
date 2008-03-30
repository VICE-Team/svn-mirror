/*
 * raster-line.c - Raster-based video chip emulation helper.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <string.h>

#include "machine.h"
#include "raster-cache.h"
#include "raster-canvas.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "raster.h"
#include "videoarch.h"


inline static unsigned int get_real_mode(raster_t *raster)
{
    if (raster->draw_idle_state)
        return raster_modes_get_idle_mode(raster->modes);
    else
        return raster->video_mode;
}

/* Increase `area' so that it also includes [xs; xe] at line y.  */
inline static void add_line_to_area(raster_area_t *area,
                                    unsigned int y,
                                    unsigned int xs,
                                    unsigned int xe)
{
    if (area->is_null) {
        area->ys = area->ye = y;
        area->xs = xs;
        area->xe = xe;
        area->is_null = 0;
    } else {
        area->xs = MIN(xs, area->xs);
        area->xe = MAX(xe, area->xe);
        area->ys = MIN(y, area->ys);
        area->ye = MAX(y, area->ye);
    }
}

inline static void add_line_and_double_scan(raster_t *raster,
                                            unsigned int start,
                                            unsigned int end)
{
    add_line_to_area(&raster->update_area, raster->current_line, start, end);
}

inline static void draw_blank(raster_t *raster, unsigned int start,
                              unsigned int end)
{
    memset(raster->draw_buffer_ptr + start,
           raster->border_color, end - start + 1);
}

/* This kludge updates the sprite-sprite collisions without writing to the
   real frame buffer.  We might write a function that actually checks for
   collisions only, but we are lazy.  */
inline static void update_sprite_collisions(raster_t *raster)
{
    BYTE *fake_draw_buffer_ptr;

    if (console_mode || vsid_mode)
        return;

    if (raster->sprite_status->draw_function == NULL)
        return;

    fake_draw_buffer_ptr = raster->fake_draw_buffer_line
                           + raster->geometry.extra_offscreen_border_left;

    raster->sprite_status->draw_function(fake_draw_buffer_ptr,
                                         raster->zero_gfx_msk);
}

inline static void handle_blank_line(raster_t *raster)
{
    if (console_mode || vsid_mode)
        return;

    /* Changes... Should/could be handled better.  */
    if (raster->changes.have_on_this_line) {
        raster_changes_t *border_changes;
        unsigned int i, xs;

        raster_changes_apply_all(&raster->changes.background);
        raster_changes_apply_all(&raster->changes.foreground);

        border_changes = &raster->changes.border;

        for (xs = i = 0; i < border_changes->count; i++) {
            unsigned int xe;

            xe = border_changes->actions[i].where;

            if (xs < xe) {
                draw_blank(raster, xs, xe);
                xs = xe;
            }

            raster_changes_apply(border_changes, i);
        }

        if (xs < raster->geometry.screen_size.width - 1)
            draw_blank(raster, xs, raster->geometry.screen_size.width - 1);

        raster_changes_remove_all(border_changes);
        raster->changes.have_on_this_line = 0;

        raster->cache[raster->current_line].border_color = 0xFF;
        raster->cache[raster->current_line].blank = 1;

        add_line_to_area(&raster->update_area, raster->current_line,
                         0, raster->geometry.screen_size.width - 1);
    } else if (CANVAS_USES_TRIPLE_BUFFERING(raster->viewport.canvas)
        || raster->dont_cache
        || raster->cache[raster->current_line].is_dirty
        || (raster->border_color
            != raster->cache[raster->current_line].border_color)
        || !raster->cache[raster->current_line].blank) {

        /* Even when the actual caching is disabled, redraw blank lines only
           if it is really necessary to do so.  */
        raster->cache[raster->current_line].border_color = raster->border_color;        raster->cache[raster->current_line].blank = 1;
        raster->cache[raster->current_line].is_dirty = 0;

        draw_blank(raster, 0, raster->geometry.screen_size.width - 1);
        add_line_to_area(&raster->update_area,
                         raster->current_line,
                         0, raster->geometry.screen_size.width - 1);
    }

    update_sprite_collisions(raster);
}

static void update_draw_buffer_ptr(raster_t *raster)
{
    if (console_mode || vsid_mode)
        return;

    raster->draw_buffer_ptr
        = raster->draw_buffer
        + raster->current_line * raster_calc_frame_buffer_width(raster)
        + raster->geometry.extra_offscreen_border_left;
}

inline static void draw_sprites_when_cache_enabled(raster_t *raster,
                                                   raster_cache_t *cache)
{
    if (raster->sprite_status->draw_function == NULL)
        return;

    raster->sprite_status->draw_function(raster->draw_buffer_ptr,
                                         cache->gfx_msk);
    cache->sprite_sprite_collisions
        = raster->sprite_status->sprite_sprite_collisions;
    cache->sprite_background_collisions
        = raster->sprite_status->sprite_background_collisions;
}

inline static int raster_fill_sprite_cache(raster_t *raster,
                                           raster_cache_t *cache,
                                           int *xs, int *xe)
{
    raster_sprite_t *sprite;
    raster_sprite_cache_t *sprite_cache;
    raster_sprite_status_t *sprite_status;
    int xs_return;
    int xe_return;
    int rr, r, sxe, sxs, sxe1, sxs1, msk;
    unsigned int i;
    unsigned int num_sprites;

    xs_return = raster->geometry.screen_size.width;
    xe_return = 0;

    rr = 0;

    sprite_status = raster->sprite_status;
    num_sprites = sprite_status->num_sprites;

    cache->numsprites = num_sprites;
    cache->sprmask = 0;

    for (msk = 1, i = 0; i < num_sprites; i++, msk <<= 1) {
        sprite = sprite_status->sprites + i;
        sprite_cache = cache->sprites + i;
        r = 0;

        if (sprite_status->dma_msk & msk) {
            DWORD data;

            data = sprite_status->sprite_data[i];

            cache->sprmask |= msk;
            sxe = sprite->x + (sprite->x_expanded ? 48 : 24);
            sxs = sprite->x;

            if (sprite->x != sprite_cache->x) {
                if (sprite_cache->visible) {
                    sxe1 = (sprite_cache->x
                           + (sprite_cache->x_expanded ? 48 : 24));
                    sxs1 = sprite_cache->x;
                    if (sxs1 < sxs)
                        sxs = sxs1;
                    if (sxe1 > sxe)
                        sxe = sxe1;
                }
                sprite_cache->x = sprite->x;
                r = 1;
            }

            if (!sprite_cache->visible) {
                sprite_cache->visible = 1;
                r = 1;
            }

            if (sprite->x_expanded != sprite_cache->x_expanded) {
                sprite_cache->x_expanded = sprite->x_expanded;
                r = 1;
            }

            if (sprite->multicolor != sprite_cache->multicolor) {
                sprite_cache->multicolor = sprite->multicolor;
                r = 1;
            }

            if (sprite_status->mc_sprite_color_1 != sprite_cache->c1) {
                sprite_cache->c1 = sprite_status->mc_sprite_color_1;
                r = 1;
            }

            if (sprite_status->mc_sprite_color_2 != sprite_cache->c2) {
                sprite_cache->c2 = sprite_status->mc_sprite_color_2;
                r = 1;
            }

            if (sprite->color != sprite_cache->c3) {
                sprite_cache->c3 = sprite->color;
                r = 1;
            }

            if (sprite->in_background != sprite_cache->in_background) {
                sprite_cache->in_background = sprite->in_background;
                r = 1;
            }

            if (sprite_cache->data != data) {
                sprite_cache->data = data;
                r = 1;
            }

            if (r) {
                xs_return = MIN(xs_return, sxs);
                xe_return = MAX(xe_return, sxe);
                rr = 1;
            }

        } else if (sprite_cache->visible) {
            sprite_cache->visible = 0;
            sxe = sprite_cache->x + (sprite_cache->x_expanded ? 24 : 48);
            xs_return = MIN(xs_return, sprite_cache->x);
            xe_return = MAX(xe_return, sxe);
            rr = 1;
        }

    }

    if (xe_return >= (int)raster->geometry.screen_size.width)
        *xe = raster->geometry.screen_size.width - 1;
    else
        *xe = xe_return;

    *xs = xs_return;

    return rr;
}


inline static void draw_sprites(raster_t *raster)
{
    if (raster->sprite_status->draw_function != NULL)
        raster->sprite_status->draw_function(raster->draw_buffer_ptr,
                                             raster->gfx_msk);
}

inline static void draw_borders(raster_t *raster)
{
    if (!raster->open_left_border)
        draw_blank(raster, 0, raster->display_xstart - 1);
    if (!raster->open_right_border)
        draw_blank(raster,
                   raster->display_xstop,
                   raster->geometry.screen_size.width - 1);
}

inline static int update_for_minor_changes_with_sprites(raster_t *raster,
                                                        int *changed_start,
                                                        int *changed_end)
{
    raster_cache_t *cache;
    unsigned int video_mode;
    int sprite_changed_start, sprite_changed_end;
    int changed_start_char, changed_end_char;
    int sprites_need_update;
    int needs_update;

    video_mode = get_real_mode(raster);

    cache = &(raster->cache)[raster->current_line];

    changed_start_char = raster->geometry.text_size.width;
    changed_end_char = -1;

    sprites_need_update = raster_fill_sprite_cache(raster,
                                                   cache,
                                                   &sprite_changed_start,
                                                   &sprite_changed_end);

    /* If sprites have changed, do not bother trying to reduce the amount
       of recalculated data, but simply redraw everything.  */
    needs_update = raster_modes_fill_cache(raster->modes,
                                           video_mode,
                                           cache,
                                           &changed_start_char,
                                           &changed_end_char,
                                           sprites_need_update);

    /* If the background color changes, we might get the wrong color in
       the left part of the screen, between the border and the start of
       the graphics.  */
    if (raster->ysmooth > 0
        && ((cache->overscan_background_color
        != raster->overscan_background_color)
        || cache->xsmooth_color != raster->xsmooth_color))
        needs_update = 1;

    if (needs_update) {
        raster_modes_draw_line_cached(raster->modes,
                                      video_mode,
                                      cache,
                                      changed_start_char,
                                      changed_end_char);

        /* Fill the space between the border and the graphics with the
           background color (necessary if xsmooth is > 0).  */
        if (raster->xsmooth != 0)
            memset(raster->draw_buffer_ptr + raster->geometry.gfx_position.x,
                   raster->xsmooth_color, raster->xsmooth);

        if (raster->sprite_status->num_sprites > 0) {
            /* FIXME: Could be optimized better.  */
            draw_sprites_when_cache_enabled(raster, cache);
            draw_borders(raster);
        } else if (raster->xsmooth > 0) {
            /* If xsmooth > 0, drawing the graphics might have corrupted
               part of the border... fix it here.  */
            if (!raster->open_right_border)
                draw_blank(raster,
                           raster->geometry.gfx_position.x
                           + raster->geometry.gfx_size.width,
                           raster->geometry.gfx_position.x
                           + raster->geometry.gfx_size.width + 8);

        }

        /* Calculate the interval in pixel coordinates.  */

        *changed_start = raster->geometry.gfx_position.x;

        if (cache->overscan_background_color
            != raster->overscan_background_color) {
            /* FIXME: ???  */

            if (raster->ysmooth <= 0)
                *changed_start += raster->xsmooth + 8 * changed_start_char;

            cache->overscan_background_color
                = raster->overscan_background_color;
        }

        *changed_end = raster->geometry.gfx_position.x
                       + raster->xsmooth
                       + 8 * (changed_end_char + 1)
                       - 1;

        if (sprites_need_update) {
            /* FIXME: wrong.  */
            if (raster->open_left_border)
                *changed_start = 0;
            if (raster->open_right_border)
                *changed_end = raster->geometry.screen_size.width - 1;

            /* Even if we have recalculated the whole line, we will
               refresh only the part that has actually changed when
               writing to the window.  */
            *changed_start = MIN(*changed_start, sprite_changed_start);
            *changed_end = MAX(*changed_end, sprite_changed_end);

            /* The borders have not changed, so do not repaint them even
               if there are sprites under them.  */
            *changed_start = MAX(*changed_start, raster->display_xstart);
            *changed_end = MIN(*changed_end, raster->display_xstop);
        }
    }

    if (!sprites_need_update) {
        raster->sprite_status->sprite_sprite_collisions
            = cache->sprite_sprite_collisions;
        raster->sprite_status->sprite_background_collisions
            = cache->sprite_background_collisions;
    }

    return needs_update;
}

inline static int update_for_minor_changes_without_sprites(raster_t *raster,
                                                           int *changed_start,
                                                           int *changed_end)
{
    raster_cache_t *cache;
    unsigned int video_mode;
    int changed_start_char, changed_end_char;
    int needs_update;

    video_mode = get_real_mode(raster);

    cache = &(raster->cache)[raster->current_line];

    changed_start_char = raster->geometry.text_size.width;
    changed_end_char = -1;

    needs_update = raster_modes_fill_cache(raster->modes,
                                           video_mode,
                                           cache,
                                           &changed_start_char,
                                           &changed_end_char,
                                           0);

    if (needs_update) {
        raster_modes_draw_line_cached(raster->modes,
                                      video_mode,
                                      cache,
                                      changed_start_char,
                                      changed_end_char);
        draw_sprites_when_cache_enabled(raster, cache);

        /* Convert from character to pixel coordinates.  FIXME: Hardcoded
           `8'.  */
        *changed_start = raster->geometry.gfx_position.x
                          + raster->xsmooth
                          + 8 * changed_start_char;

        *changed_end = raster->geometry.gfx_position.x
                        + raster->xsmooth
                        + 8 * (changed_end_char + 1)
                        - 1;
    }

    /* FIXME: Why always doing so?  */
    draw_borders(raster);

    return needs_update;
}

inline static int update_for_minor_changes(raster_t *raster,
                                           int *changed_start,
                                           int *changed_end)
{
    if (raster->sprite_status->num_sprites > 0)
        return update_for_minor_changes_with_sprites(raster,
                                                     changed_start,
                                                     changed_end);
    else
        return update_for_minor_changes_without_sprites(raster,
                                                        changed_start,
                                                        changed_end);
}

inline static void fill_background(raster_t *raster)
{
    if (raster->xsmooth != 0) {
        if (raster->draw_idle_state)
            memset(raster->draw_buffer_ptr + raster->geometry.gfx_position.x,
                   raster->overscan_background_color, raster->xsmooth);
        else
            memset(raster->draw_buffer_ptr + raster->geometry.gfx_position.x,
                   raster->xsmooth_color, raster->xsmooth);
    }

    if (raster->open_left_border) {
        if (raster->draw_idle_state)
            memset(raster->draw_buffer_ptr, raster->overscan_background_color,
                   (raster->geometry.gfx_position.x + raster->xsmooth));
        else
            memset(raster->draw_buffer_ptr, raster->xsmooth_color,
                   (raster->geometry.gfx_position.x + raster->xsmooth));
    }

    if (raster->open_right_border) {
        if (raster->draw_idle_state)
            memset(raster->draw_buffer_ptr +
                   raster->geometry.gfx_position.x
                   + raster->geometry.gfx_size.width
                   + raster->xsmooth,
                   raster->overscan_background_color,
                   raster->geometry.screen_size.width
                   - raster->geometry.gfx_position.x
                   - raster->geometry.gfx_size.width
                   - raster->xsmooth);
        else
            memset(raster->draw_buffer_ptr +
                   raster->geometry.gfx_position.x
                   + raster->geometry.gfx_size.width
                   + raster->xsmooth,
                   raster->xsmooth_color,
                   raster->geometry.screen_size.width
                   - raster->geometry.gfx_position.x
                   - raster->geometry.gfx_size.width
                   - raster->xsmooth);
    }
}

inline static int check_for_major_changes_and_update(raster_t *raster,
                                                     int *changed_start,
                                                     int *changed_end)
{
    raster_cache_t *cache;
    unsigned int video_mode;
    int line;

    video_mode = get_real_mode(raster);

    cache = &(raster->cache)[raster->current_line];
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
        || cache->xsmooth_color != raster->xsmooth_color
        || (cache->overscan_background_color
            != raster->overscan_background_color)) {

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
        cache->xsmooth_color = raster->xsmooth_color;
        cache->overscan_background_color = raster->overscan_background_color;

        /* Fill the space between the border and the graphics with the
           background color (necessary if `xsmooth' is != 0).  */

        fill_background(raster);

        raster_fill_sprite_cache(raster, cache,
                                 &changed_start_char,
                                 &changed_end_char);

        r = raster_modes_fill_cache(raster->modes,
                                    video_mode,
                                    cache,
                                    &changed_start_char,
                                    &changed_end_char, 1);

        /* [ `changed_start' ; `changed_end' ] now covers the whole line, as
           we have called fill_cache() with `1' as the last parameter (no
           check).  */
        raster_modes_draw_line_cached(raster->modes, video_mode,
                                      cache,
                                      changed_start_char,
                                      changed_end_char);
        draw_sprites_when_cache_enabled(raster, cache);

        *changed_start = 0;
        *changed_end = raster->geometry.screen_size.width - 1;

        draw_borders(raster);

        return 1;
    } else {
        return 0;
    }
}


inline static void handle_visible_line_with_cache(raster_t *raster)
{
    int needs_update;
    int changed_start, changed_end;
    raster_cache_t *cache;

    cache = &raster->cache[raster->current_line];

    /* Check for "major" changes first.  If there is any, just write straight
       to the cache without any comparisons and redraw the whole line.  */
    needs_update = check_for_major_changes_and_update(raster,
                                                      &changed_start,
                                                      &changed_end);
    if (!needs_update) {
        /* There are no `major' changes: try to do some optimization.  */
        needs_update = update_for_minor_changes(raster,
                                                &changed_start,
                                                &changed_end);
    }

    if (needs_update) {
        add_line_to_area(&raster->update_area, raster->current_line,
                         changed_start, changed_end);
    }

    cache->is_dirty = 0;
}

inline static void handle_visible_line_without_cache(raster_t *raster)
{
    raster_geometry_t *geometry;
    raster_cache_t *cache;

    geometry = &raster->geometry;

    /* If screen is scrolled to the right, we need to fill with the
       background color the blank part on the left.  */

    fill_background(raster);

    /* Draw the graphics and sprites.  */
    raster_modes_draw_line(raster->modes, get_real_mode(raster));
    draw_sprites(raster);
    draw_borders(raster);

    cache = &raster->cache[raster->current_line];

    if (CANVAS_USES_TRIPLE_BUFFERING(raster->viewport.canvas)
        || raster->dont_cache
        || raster->sprite_status->dma_msk != 0
        || cache->is_dirty
        || cache->blank
        || cache->border_color != raster->border_color
    /* FIXME: Done differently in another place.  */
        || (cache->open_right_border != raster->open_right_border)
        || (cache->open_left_border != raster->open_left_border)
        || (cache->overscan_background_color
        != raster->overscan_background_color
        || cache->xsmooth_color != raster->xsmooth_color)
    ) {
        cache->blank = 0;
        cache->is_dirty = 0;
        cache->border_color = raster->border_color;
        cache->open_right_border = raster->open_right_border;
        cache->open_left_border = raster->open_left_border;
        cache->xsmooth_color = raster->xsmooth_color;
        cache->overscan_background_color = raster->overscan_background_color;

        add_line_to_area(&raster->update_area, raster->current_line,
                         0, raster->geometry.screen_size.width - 1);
    } else {
        /* Still do some minimal caching anyway.  */
        /* Only update the part between the borders.  */
        add_line_to_area(&raster->update_area, raster->current_line,
                         geometry->gfx_position.x,
                         geometry->gfx_position.x
                         + geometry->gfx_size.width - 1);
    }
}

inline static void handle_visible_line_with_changes(raster_t *raster)
{
    unsigned int i;
    int xs, xstop;
    raster_geometry_t *geometry;

    geometry = &raster->geometry;

    /* Draw the background.  */
    for (xs = i = 0; i < raster->changes.background.count; i++) {
        int xe = raster->changes.background.actions[i].where;

        if (xs < xe) {
            raster_modes_draw_background(raster->modes,
                                         get_real_mode(raster),
                                         xs,
                                         xe - 1);
            xs = xe;
        }
        raster_changes_apply(&raster->changes.background, i);
    }
    if (xs <= (int)geometry->screen_size.width - 1)
        raster_modes_draw_background(raster->modes,
                                     get_real_mode(raster),
                                     xs,
                                     geometry->screen_size.width - 1);

    /* Draw the foreground graphics.  */
    for (xs = i = 0; i < raster->changes.foreground.count; i++) {
        int xe = raster->changes.foreground.actions[i].where;

        if (xs < xe) {
            raster_modes_draw_foreground(raster->modes,
                                         get_real_mode(raster),
                                         xs,
                                         xe - 1);
            xs = xe;
        }
        raster->xsmooth_shift_left = 0;
        raster_changes_apply(&raster->changes.foreground, i);
    }
    if (xs <= (int)geometry->text_size.width - 1)
        raster_modes_draw_foreground(raster->modes,
                                     get_real_mode(raster),
                                     xs,
                                     geometry->text_size.width - 1);

    raster->xsmooth_shift_left = 0;
    draw_sprites(raster);

    /* Draw left border.  */
    xstop = raster->display_xstart - 1;
    if (!raster->open_left_border) {
        for (xs = i = 0;
            (i < raster->changes.border.count
            && raster->changes.border.actions[i].where <= xstop);
            i++) {
            int xe = raster->changes.border.actions[i].where;

            if (xs < xe) {
                draw_blank(raster, xs, xe - 1);
                xs = xe;
            }
            raster_changes_apply(&raster->changes.border, i);
        }
        if (xs <= xstop)
            draw_blank(raster, xs, xstop);
    } else {
        for (i = 0;
            (i < raster->changes.border.count
            && raster->changes.border.actions[i].where <= xstop);
            i++)
            raster_changes_apply(&raster->changes.border, i);
    }

    /* Draw right border.  */
    if (!raster->open_right_border) {
        for (;
            (i < raster->changes.border.count
            && (raster->changes.border.actions[i].where
            <= raster->display_xstop));
            i++)
            raster_changes_apply(&raster->changes.border, i);
        for (xs = raster->display_xstop;
            i < raster->changes.border.count;
            i++) {
            int xe = raster->changes.border.actions[i].where;

            if (xs < xe) {
                draw_blank(raster, xs, xe - 1);
                xs = xe;
            }
            raster_changes_apply(&raster->changes.border, i);
        }
        if (xs <= (int)geometry->screen_size.width - 1)
            draw_blank(raster, xs, geometry->screen_size.width - 1);
    } else {
        for (i = 0; i < raster->changes.border.count; i++)
            raster_changes_apply(&raster->changes.border, i);
    }

    raster_changes_remove_all(&raster->changes.foreground);
    raster_changes_remove_all(&raster->changes.background);
    raster_changes_remove_all(&raster->changes.border);
    raster->changes.have_on_this_line = 0;

    /* Do not cache this line at all.  */
    raster->cache[raster->current_line].is_dirty = 1;

    add_line_to_area(&raster->update_area, raster->current_line,
                     0, raster->geometry.screen_size.width - 1);
}

inline static void handle_visible_line(raster_t *raster)
{
    if (console_mode || vsid_mode)
        return;

    if (raster->changes.have_on_this_line)
        handle_visible_line_with_changes(raster);
    else if (!CANVAS_USES_TRIPLE_BUFFERING (raster->viewport.canvas)
        && raster->cache_enabled
        && !raster->open_left_border
        && !raster->open_right_border)       /* FIXME: shortcut! */
        handle_visible_line_with_cache(raster);
    else
        handle_visible_line_without_cache(raster);
}

void raster_line_emulate(raster_t *raster)
{
    raster_viewport_t *viewport;

    viewport = &raster->viewport;

    /* Emulate the vertical blank flip-flops.  (Well, sort of.)  */
    if (raster->current_line == raster->display_ystart && (!raster->blank
        || raster->blank_off))
        raster->blank_enabled = 0;
    if (raster->current_line == raster->display_ystop)
        raster->blank_enabled = 1;

    if (raster->current_line >= raster->geometry.first_displayed_line
        && raster->current_line <= raster->geometry.last_displayed_line
        && raster->current_line >= viewport->first_line
        && raster->current_line <= viewport->last_line) {

        if ((raster->blank_this_line || raster->blank_enabled)
            && !raster->open_left_border)
            handle_blank_line(raster);
        else
            handle_visible_line(raster);

        if (++raster->num_cached_lines == (viewport->last_line
                                           - viewport->first_line)) {
            raster->dont_cache = 0;
            raster->num_cached_lines = 0;
        }
    } else {
        update_sprite_collisions(raster);

        if (raster->changes.have_on_this_line) {
            raster_changes_apply_all(&raster->changes.background);
            raster_changes_apply_all(&raster->changes.foreground);
            raster_changes_apply_all(&raster->changes.border);
            raster->changes.have_on_this_line = 0;
        }
    }

    raster->current_line++;

    if (raster->current_line == raster->geometry.screen_size.height) {
        raster_canvas_handle_end_of_frame(raster);
    } else {
        update_draw_buffer_ptr(raster);
    }

    raster_changes_apply_all(&raster->changes.next_line);

    /* Handle open borders.  */
    raster->open_left_border = raster->open_right_border;
    raster->open_right_border = 0;

    if (raster->sprite_status->num_sprites > 0)
        raster->sprite_status->dma_msk = raster->sprite_status->new_dma_msk;

    raster->blank_this_line = 0;
}

