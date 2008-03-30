/*
 * raster.h - Raster-based video chip emulation helper.
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

#ifndef _RASTER_H
#define _RASTER_H

#include "video.h"
#include "palette.h"

#include "raster-cache.h"
#include "raster-changes.h"
#include "raster-modes.h"
#include "raster-sprite-status.h"

/* We assume that, if already #defined, the provided `MAX' and `MIN' actually
   work.  */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Yeah, static allocation sucks.  But it's faster, and we are not wasting
   much space anyway.  */
#define RASTER_GFX_MSK_SIZE 0x100



/* A simple convenience type for defining rectangular areas.  */
struct _raster_rectangle
 {
    unsigned int width;
    unsigned int height;
  };
typedef struct _raster_rectangle raster_rectangle_t;

/* A simple convenience type for defining screen positions.  */
struct _raster_position
  {
    unsigned int x;
    unsigned int y;
  };
typedef struct _raster_position raster_position_t;

/* A simple convenience type for defining a rectangular area on the screen.  */
struct _raster_area
  {
    unsigned int xs;
    unsigned int ys;
    unsigned int xe;
    unsigned int ye;
    int is_null;
  };
typedef struct _raster_area raster_area_t;

struct _raster_viewport
  {
    /* Output canvas.  */
    canvas_t canvas;

    /* Portion of the screen displayed on the output window window.
       FIXME: We should get this from the canvas.  */
    unsigned int width, height;

    /* Title for the viewport.  FIXME: Duplicated info from the canvas?  */
    char *title;

    /* Offset of the screen on the window.  */
    unsigned int x_offset, y_offset;

    /* First and last lines shown in the output window.  */
    unsigned int first_line, last_line;

    /* First pixel in one line of the frame buffer to be shown on the output
       window.  */
    unsigned int first_x;

    /* Pixel size.  */
    raster_rectangle_t pixel_size;

    /* Exposure handler.  */
    canvas_redraw_t exposure_handler;
  };
typedef struct _raster_viewport raster_viewport_t;

struct _raster_geometry
  {
    /* Total size of the screen, including borders and unused areas.
       (SCREEN_WIDTH, SCREEN_HEIGHT)  */
    raster_rectangle_t screen_size;

    /* Size of the graphics area (i.e. excluding borders and unused areas.
       (SCREEN_XPIX, SCREEN_YPIX)  */
    raster_rectangle_t gfx_size;

    /* Size of the text area.  (SCREEN_TEXTCOLS)  */
    raster_rectangle_t text_size;

    /* Position of the graphics area.  (SCREEN_BORDERWIDTH,
       SCREEN_BORDERHEIGHT) */
    raster_position_t gfx_position;

    /* If nonzero, `gfx_position' is expected to be moved around when poking
       to the chip registers.  */
    int gfx_area_moves;

    /* FIXME: Bad names.  */
    unsigned int first_displayed_line, last_displayed_line;

    unsigned int extra_offscreen_border;
  };
typedef struct _raster_geometry raster_geometry_t;

struct _raster
  {
    raster_viewport_t viewport;

    raster_geometry_t geometry;

    raster_modes_t modes;

    raster_sprite_status_t sprite_status;

    struct
      {
        raster_changes_t background;
        raster_changes_t foreground;
        raster_changes_t border;
        raster_changes_t next_line;
        int have_on_this_line;
      }
    changes;

    struct
      {
        PIXEL sing[0x100];
        PIXEL2 doub[0x100];
        PIXEL4 quad[0x100];
      }
    pixel_table;

    frame_buffer_t frame_buffer;
    PIXEL *frame_buffer_ptr;

    /* This is a temporary frame buffer line used for sprite collision
       checking without drawing to the real frame buffer.  */
    PIXEL *fake_frame_buffer_line;

    /* Palette used for drawing.  */
    palette_t *palette;

    /* This is a bit mask representing each pixel on the screen (1 =
       foreground, 0 = background) and is used both for sprite-background
       collision checking and background sprite drawing.  When cache is
       turned on, a cached mask for each line is used instead (see
       `raster_cache_t.gfx_msk').  */
    BYTE gfx_msk[RASTER_GFX_MSK_SIZE];

    /* This is a temporary graphics mask used for sprite collision checking
       without drawing to the real frame buffer, and is set up to be always
       filled with zeroes.  */
    BYTE zero_gfx_msk[RASTER_GFX_MSK_SIZE];

    /* Smooth scroll values for the graphics (not the whole screen).  */
    int xsmooth, ysmooth;

    /* If nonzero, we should skip the next frame. (used for automatic refresh
       rate setting) */
    int skip_frame;

    /* Next line to be calculated.  */
    unsigned int current_line;

    /* Border and background colors.  */
    int border_color, background_color;

    /* Color of the overscan area.  */
    int overscan_background_color;

    /* If this is != 0, no graphics is drawn and the whole line is painted with
       border_color.  */
    int blank_enabled;

    /* If this is != 0, the current raster line is blank.  The value of this
       variable is set to zero again after the current line is updated.  */
    int blank_this_line;

    /* Open border flags.  */
    int open_right_border, open_left_border;

    /* blank_enabled is set when line display_ystop is reached and reset when
       line display_ystart is reached and blank is 0.  */
    int blank;
    unsigned int display_ystart, display_ystop;

    /* These define the borders for the current line.  */
    int display_xstart, display_xstop;

    /* Flag: should we display the line in idle state? */
    int draw_idle_state;

    /* Count character lines (i.e. RC on the VIC-II).  */
    unsigned int ycounter;

    /* Current video mode.  */
    int video_mode;

    /* Cache.  */
    raster_cache_t *cache;
    int cache_enabled;          /* FIXME: Method to toggle it. */

    /* This is != 0 if we cannot use the values in the cache anymore.  */
    int dont_cache;

    /* Number of lines that have been recalculated.  When this value reaches
       the number of lines that are displayed in the output, then the cache
       is valid again.  */
    unsigned int num_cached_lines;

    /* Area to update.  */
    raster_area_t update_area;

    unsigned int do_double_scan;
  };
typedef struct _raster raster_t;

#define RASTER_PIXEL(raster, c) (raster)->pixel_table.sing[(c)]

/* FIXME: MSDOS does not need double or quad pixel.
`ifdef' them out once all video chips actually honour this.  */

#define RASTER_PIXEL2(raster, c) (raster)->pixel_table.doub[(c)]
#define RASTER_PIXEL4(raster, c) (raster)->pixel_table.quad[(c)]



void raster_init (raster_t *raster, unsigned int num_modes,
                  unsigned int num_sprites);
raster_t *raster_new (unsigned int num_modes, unsigned int num_sprites);
void raster_reset (raster_t *raster);
int raster_realize (raster_t *raster);
void raster_set_exposure_handler (raster_t *raster,
                                  canvas_redraw_t exposure_handler);
void raster_set_geometry (raster_t *raster,
                          unsigned int screen_width,
                          unsigned int screen_height,
                          unsigned int gfx_width,
                          unsigned int gfx_height,
                          unsigned int text_width,
                          unsigned int text_height,
                          unsigned int gfx_position_x,
                          unsigned int gfx_position_y,
                          int gfx_area_moves,
                          unsigned int first_displayed_line,
                          unsigned int last_displayed_line,
                          unsigned int extra_offscreen_border);
void raster_resize_viewport (raster_t *raster,
                             unsigned int width, unsigned int height);
void raster_set_pixel_size (raster_t *raster, unsigned int width,
                            unsigned int height);
void raster_emulate_line (raster_t *raster);
void raster_force_repaint (raster_t *raster);
void raster_set_palette (raster_t *raster, palette_t *palette);
void raster_set_title (raster_t *raster, const char *title);
void raster_skip_frame (raster_t *raster, int skip);
void raster_enable_cache (raster_t *raster, int enable);
void raster_enable_double_scan (raster_t *raster, int enable);



/* Inlined functions.  These need to be *fast*.  */

inline static void
raster_add_int_change_next_line (raster_t *raster,
                                 int *ptr,
                                 int new_value)
{
  if (raster->skip_frame)
    *ptr = new_value;
  else
    raster_changes_add_int (&raster->changes.next_line, 0, ptr, new_value);
}

inline static void
raster_add_ptr_change_next_line (raster_t *raster,
                                 void **ptr,
                                 void *new_value)
{
  if (raster->skip_frame)
    *ptr = new_value;
  else
    raster_changes_add_ptr (&raster->changes.next_line, 0, ptr, new_value);
}

inline static void
raster_add_int_change_foreground (raster_t *raster,
                                  int char_x,
                                  int *ptr,
                                  int new_value)
{
  if (raster->skip_frame || char_x <= 0)
    *ptr = new_value;
  else if (char_x < (int) raster->geometry.text_size.width)
    {
      raster_changes_add_int (&raster->changes.foreground,
                              char_x, ptr, new_value);
      raster->changes.have_on_this_line = 1;
    }
  else
    raster_add_int_change_next_line (raster, ptr, new_value);
}

inline static void
raster_add_ptr_change_foreground (raster_t *raster,
                                  int char_x,
                                  void **ptr,
                                  void *new_value)
{
  if (raster->skip_frame || char_x <= 0)
    *ptr = new_value;
  else if (char_x < (int) raster->geometry.text_size.width)
    {
      raster_changes_add_ptr (&raster->changes.foreground,
                              char_x, ptr, new_value);
      raster->changes.have_on_this_line = 1;
    }
  else
    raster_add_ptr_change_next_line (raster, ptr, new_value);
}

inline static void
raster_add_int_change_background (raster_t *raster,
                                  int raster_x,
                                  int *ptr,
                                  int new_value)
{
  if (raster->skip_frame || raster_x <= 0)
    *ptr = new_value;
  else if (raster_x < (int) raster->geometry.screen_size.width)
    {
      raster_changes_add_int (&raster->changes.background,
                              raster_x, ptr, new_value);
      raster->changes.have_on_this_line = 1;
    }
  else
    raster_add_int_change_next_line (raster, ptr, new_value);
}

inline static void
raster_add_ptr_change_background (raster_t *raster,
                                  int raster_x,
                                  void **ptr,
                                  void *new_value)
{
  if (raster->skip_frame || raster_x <= 0)
    *ptr = new_value;
  else if (raster_x < (int) raster->geometry.screen_size.width)
    {
      raster_changes_add_ptr (&raster->changes.background,
                              raster_x, ptr, new_value);
      raster->changes.have_on_this_line = 1;
    }
  else
    raster_add_ptr_change_next_line (raster, ptr, new_value);
}

inline static void
raster_add_int_change_border (raster_t *raster,
                              int raster_x,
                              int *ptr,
                              int new_value)
{
  if (raster->skip_frame || raster_x <= 0)
    *ptr = new_value;
  else if (raster_x < (int) raster->geometry.screen_size.width)
    {
      raster_changes_add_int (&raster->changes.border,
                              raster_x, ptr, new_value);
      raster->changes.have_on_this_line = 1;
    }
  else
    raster_add_int_change_next_line (raster, ptr, new_value);
}



inline static int
raster_fill_sprite_cache (raster_t *raster,
                          raster_cache_t *cache,
                          int *xs, int *xe)
{
  raster_sprite_t *sprite;
  raster_sprite_cache_t *sprite_cache;
  raster_sprite_status_t *sprite_status;
  int xs_return;
  int xe_return;
  int rr, r, sxe, sxs, sxe1, sxs1, n, msk;
  unsigned int i;
  unsigned int num_sprites;

  xs_return = raster->geometry.screen_size.width;
  xe_return = 0;

  rr = 0;

  sprite_status = &raster->sprite_status;
  num_sprites = sprite_status->num_sprites;

  cache->numsprites = num_sprites;
  cache->sprmask = 0;

  for (msk = 1, i = 0; i < num_sprites; i++, msk <<= 1)
    {
      sprite = sprite_status->sprites + i;
      sprite_cache = cache->sprites + i;
      r = 0;

      if (sprite_status->dma_msk & msk)
        {
          DWORD data;

          data = sprite_status->sprite_data[i];

          cache->sprmask |= msk;
          sxe = sprite->x + (sprite->x_expanded ? 48 : 24);
          sxs = sprite->x;

          if (sprite->x != sprite_cache->x)
            {
              if (sprite_cache->visible)
                {
                  sxe1 = (sprite_cache->x
                          + (sprite_cache->x_expanded ? 48 : 24));
                  sxs1 = sprite_cache->x;
                  n++;
                  if (sxs1 < sxs)
                    sxs = sxs1;
                  if (sxe1 > sxe)
                    sxe = sxe1;
                }
              sprite_cache->x = sprite->x;
              r = 1;
            }

          if (!sprite_cache->visible)
            {
              sprite_cache->visible = 1;
              r = 1;
            }

          if (sprite->x_expanded != sprite_cache->x_expanded)
            {
              sprite_cache->x_expanded = sprite->x_expanded;
              r = 1;
            }

          if (sprite->multicolor != sprite_cache->multicolor)
            {
              sprite_cache->multicolor = sprite->multicolor;
              r = 1;
            }

          if (sprite_status->mc_sprite_color_1 != sprite_cache->c1)
            {
              sprite_cache->c1 = sprite_status->mc_sprite_color_1;
              r = 1;
            }

          if (sprite_status->mc_sprite_color_2 != sprite_cache->c2)
            {
              sprite_cache->c2 = sprite_status->mc_sprite_color_2;
              r = 1;
            }

          if (sprite->color != sprite_cache->c3)
            {
              sprite_cache->c3 = sprite->color;
              r = 1;
            }

          if (sprite->in_background != sprite_cache->in_background)
            {
              sprite_cache->in_background = sprite->in_background;
              r = 1;
            }

          if (sprite_cache->data != data)
            {
              sprite_cache->data = data;
              r = 1;
            }

          if (r)
            {
              xs_return = MIN (xs_return, sxs);
              xe_return = MAX (xe_return, sxe);
              rr = 1;
            }
        }
      else if (sprite_cache->visible)
        {
          sprite_cache->visible = 0;
          sxe = sprite_cache->x + (sprite_cache->x_expanded ? 24 : 48);
          xs_return = MIN (xs_return, sprite_cache->x);
          xe_return = MAX (xe_return, sxe);
          rr = 1;
        }

    }

  if (xe_return >= (int) raster->geometry.screen_size.width)
    *xe = raster->geometry.screen_size.width - 1;
  else
    *xe = xe_return;
  *xs = xs_return;

  return rr;
}



inline static void
vid_memcpy (PIXEL *dst,
            PIXEL *src,
            unsigned int count)
{
  memcpy (dst, src, count * sizeof (PIXEL));
}

#if X_DISPLAY_DEPTH > 8

inline static void
vid_memset (PIXEL *dst,
            PIXEL value,
            unsigned int count)
{
  int i;

  for (i = 0; i < count; i++)
    dst[i] = value;
}

#else

inline static void
vid_memset (PIXEL *dst,
            PIXEL value,
            unsigned int count)
{
  memset (dst, value, count);
}

#endif

#endif /* _RASTER_H */
