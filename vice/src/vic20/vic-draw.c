/*
 * vic-draw.c - Drawing functions for the VIC emulation.
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

#include "types.h"

#include "raster.h"
#include "raster-cache.h"

#include "vic.h"
#include "vic-draw.h"



/* Here comes the part that actually repaints each raster line.  This table is
   used to speed up the drawing. */
static WORD drawing_table[256][256][8];	/* [byte][color][position] */



/* FIXME: Here, we are really wasting resources.  When using the
   cache, we store the foreground data in the cache, but then we fetch
   it again when drawing.  */

static void
init_drawing_tables (void)
{
  unsigned int byte, color, pos;

  for (byte = 0; byte < 0x100; byte++)
    for (color = 0; color < 0x100; color++)
      if (color & 0x8)		/* Multicolor mode. */
	for (pos = 0; pos < 8; pos += 2)
	  drawing_table[byte][color][pos]
            = drawing_table[byte][color][pos + 1]
	    = (byte >> (6 - pos)) & 0x3;
      else			/* Standard mode. */
	for (pos = 0; pos < 8; pos++)
	  drawing_table[byte][color][pos] = ((byte >> (7 - pos)) & 0x1) * 2;
}



static int
fill_cache (raster_cache_t *cache,
            int *xs,
            int *xe,
            int r)
{
  if (cache->background_data[0] != vic.raster.background_color
      || cache->color_data_2[0] != vic.auxiliary_color
      || cache->numcols != vic.text_cols)
    {
      cache->background_data[0] = vic.raster.background_color;
      cache->color_data_2[0] = vic.auxiliary_color;
      cache->numcols = vic.text_cols;
      *xs = 0;
      *xe = vic.text_cols;
      r = 1;
    }

  r = raster_cache_data_fill_text (cache->foreground_data,
                                   vic.screen_ptr + vic.memptr,
                                   vic.chargen_ptr,
                                   vic.char_height,
                                   vic.text_cols,
                                   vic.raster.ycounter,
                                   xs, xe,
                                   r);

  r = raster_cache_data_fill (cache->color_data_1,
                              vic.color_ptr + vic.memptr,
                              vic.text_cols,
                              1,
                              xs, xe,
                              r);

  return r;
}



#define PUT_PIXEL(p, d, c, b, x) \
  *((VIC_PIXEL *)(p) + (x)) = (c)[drawing_table[(d)][(b)][(x)]]

#define PUT_PIXEL2(p, d, c, b, x) \
  *((VIC_PIXEL2 *)(p) + (x)) = (c)[drawing_table[(d)][(b)][(x)]]

#define GET_CHAR_DATA(code, row) \
  *(vic.chargen_ptr + ((code) * vic.char_height) + (row))

inline static void
draw (PIXEL *p,
      unsigned int xs,
      unsigned int xe,
      int reverse)
{
  static VIC_PIXEL c[4];
  unsigned int b, i;
  BYTE d;

  p += xs * 8 * VIC_PIXEL_WIDTH;

  c[0] = VIC_PIXEL (vic.raster.background_color);
  c[1] = VIC_PIXEL (vic.raster.border_color);
  c[3] = VIC_PIXEL (vic.auxiliary_color);

  for (i = xs; i <= xe; i++, p += 8 * VIC_PIXEL_WIDTH)
    {
      b = *(vic.color_ptr + vic.memptr + i);
      c[2] = VIC_PIXEL (b & 0x7);
      if (reverse)
        d = ~(GET_CHAR_DATA (*(vic.screen_ptr + vic.memptr + i),
                             vic.raster.ycounter));
      else
        d = GET_CHAR_DATA (*(vic.screen_ptr + vic.memptr + i),
                           vic.raster.ycounter);
      PUT_PIXEL (p, d, c, b, 0);
      PUT_PIXEL (p, d, c, b, 1);
      PUT_PIXEL (p, d, c, b, 2);
      PUT_PIXEL (p, d, c, b, 3);
      PUT_PIXEL (p, d, c, b, 4);
      PUT_PIXEL (p, d, c, b, 5);
      PUT_PIXEL (p, d, c, b, 6);
      PUT_PIXEL (p, d, c, b, 7);
    }
}

static void
draw_line (void)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH);

  draw (p, 0, vic.text_cols - 1, 0);
}

static void
draw_reverse_line (void)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH);

  draw (p, 0, vic.text_cols - 1, 1);
}

static void
draw_line_cached (raster_cache_t *cache,
                  unsigned int xs,
                  unsigned int xe)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH);

  draw (p, xs, xe, 0);
}

static void
draw_reverse_line_cached (raster_cache_t *cache,
                          unsigned int xs,
                          unsigned int xe)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH);

  draw (p, xs, xe, 1);
}

inline static void
draw_2x(PIXEL *p,
        unsigned int xs,
        unsigned int xe,
        int reverse)
{
  static VIC_PIXEL2 c[4];
  BYTE d, b;
  unsigned int i;

  p += xs * 16 * VIC_PIXEL_WIDTH;

  c[0] = VIC_PIXEL2 (vic.raster.background_color);
  c[1] = VIC_PIXEL2 (vic.raster.border_color);
  c[3] = VIC_PIXEL2 (vic.auxiliary_color);

  for (i = xs; i <= xe; i++, p += 16 * VIC_PIXEL_WIDTH)
    {
      b = *(vic.color_ptr + vic.memptr + i);
      c[2] = VIC_PIXEL2 (b & 0x7);

      if (reverse)
        d = ~(GET_CHAR_DATA ((vic.screen_ptr + vic.memptr)[i],
                             vic.raster.ycounter));
      else
        d = GET_CHAR_DATA ((vic.screen_ptr + vic.memptr)[i],
                           vic.raster.ycounter);

      PUT_PIXEL2 (p, d, c, b, 0);
      PUT_PIXEL2 (p, d, c, b, 1);
      PUT_PIXEL2 (p, d, c, b, 2);
      PUT_PIXEL2 (p, d, c, b, 3);
      PUT_PIXEL2 (p, d, c, b, 4);
      PUT_PIXEL2 (p, d, c, b, 5);
      PUT_PIXEL2 (p, d, c, b, 6);
      PUT_PIXEL2 (p, d, c, b, 7);
    }
}

static void
draw_line_2x (void)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH * 2);

  draw_2x (p, 0, vic.text_cols - 1, 0);
}

static void
draw_reverse_line_2x (void)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH * 2);

  draw_2x (p, 0, vic.text_cols - 1, 1);
}

static void
draw_line_cached_2x (raster_cache_t *cache,
                     unsigned int xs,
                     unsigned int xe)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH * 2);

  draw_2x (p, xs, xe, 0);
}

static void
draw_reverse_line_cached_2x (raster_cache_t *cache,
                             unsigned int xs,
                             unsigned int xe)
{
  PIXEL *p;

  p = (vic.raster.frame_buffer_ptr
       + vic.raster.display_xstart * VIC_PIXEL_WIDTH * 2);

  draw_2x (p, xs, xe, 1);
}



void
vic_draw_init (void)
{
  init_drawing_tables ();
  raster_set_table_refresh_handler (&vic.raster, init_drawing_tables);
}

void
vic_draw_set_double_size (int enabled)
{
  if (enabled)
    {
      raster_modes_set(&vic.raster.modes, VIC_STANDARD_MODE,
                       fill_cache,
                       draw_line_cached_2x,
                       draw_line_2x,
                       NULL,
                       NULL);
      raster_modes_set(&vic.raster.modes, VIC_REVERSE_MODE,
                       fill_cache,
                       draw_reverse_line_cached_2x,
                       draw_reverse_line_2x,
                       NULL,
                       NULL);
    }
  else
    {
      raster_modes_set(&vic.raster.modes, VIC_STANDARD_MODE,
                       fill_cache,
                       draw_line_cached,
                       draw_line,
                       NULL,
                       NULL);
      raster_modes_set(&vic.raster.modes, VIC_REVERSE_MODE,
                       fill_cache,
                       draw_reverse_line_cached,
                       draw_reverse_line,
                       NULL,
                       NULL);
    }
}
