/*
 * vic-draw.c - Drawing functions for the VIC emulation.
 *
 * Written by
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

#include <string.h>

#include "raster-cache-fill.h"
#include "raster-cache-text-std.h"
#include "raster-cache.h"
#include "raster-modes.h"
#include "types.h"
#include "vic.h"
#include "vic-draw.h"


/* Here comes the part that actually repaints each raster line.  This table is
   used to speed up the drawing. */
static WORD drawing_table[256][256][8]; /* [byte][color][position] */


/* FIXME: Here, we are really wasting resources.  When using the
   cache, we store the foreground data in the cache, but then we fetch
   it again when drawing.  */

static void init_drawing_tables(void)
{
    unsigned int byte, color, pos;

    for (byte = 0; byte < 0x100; byte++)
        for (color = 0; color < 0x100; color++)
            if (color & 0x8)            /* Multicolor mode. */
                for (pos = 0; pos < 8; pos += 2)
                    drawing_table[byte][color][pos]
                        = drawing_table[byte][color][pos + 1]
                        = (byte >> (6 - pos)) & 0x3;
            else                      /* Standard mode. */
                for (pos = 0; pos < 8; pos++)
                    drawing_table[byte][color][pos]
                        = ((byte >> (7 - pos)) & 0x1) * 2;
}


static int fill_cache(raster_cache_t *cache, unsigned int *xs,
                      unsigned int *xe, int rr)
{
    int r;

    if (cache->background_data[0] != vic.raster.background_color
        || cache->color_data_2[0] != vic.auxiliary_color
        || cache->numcols != vic.text_cols) {
        cache->background_data[0] = vic.raster.background_color;
        cache->color_data_2[0] = vic.auxiliary_color;
        cache->numcols = vic.text_cols;
        *xs = 0;
        *xe = vic.text_cols - 1;
        rr = 1;
    }

    r = raster_cache_data_fill_text(cache->foreground_data,
                                    vic.screen_ptr + vic.memptr,
                                    vic.chargen_ptr,
                                    vic.char_height,
                                    vic.text_cols,
                                    vic.raster.ycounter
                                    & ((vic.char_height >> 1) | 7),
                                    xs, xe,
                                    rr);

    r |= raster_cache_data_fill(cache->color_data_1,
                                vic.color_ptr + vic.memptr,
                                vic.text_cols,
                                1,
                                xs, xe,
                                rr);


    /* Scale xs and xe from text_cols into 8 pixel units.  */
    *xs = *xs << VIC_PIXEL_WIDTH_SHIFT;
    *xe = ((*xe + 1) << VIC_PIXEL_WIDTH_SHIFT) - 1;

    return r;
}

#define PUT_PIXEL(p, d, c, b, x, t) \
    if (!t || drawing_table[(d)][(b)][(x)]) \
        *((VIC_PIXEL *)(p) + (x)) = (c)[drawing_table[(d)][(b)][(x)]]

#define PUT_PIXEL2(p, d, c, b, x, t) \
    if (!t || drawing_table[(d)][(b)][(x)]) \
        *((VIC_PIXEL2 *)(p) + (x)) = (c)[drawing_table[(d)][(b)][(x)]]

#define GET_CHAR_DATA(code, row) \
    *(vic.chargen_ptr + ((code) * vic.char_height) \
    + (row & ((vic.char_height >> 1) | 7)))

inline static void draw(BYTE *p, unsigned int xs, unsigned int xe, int reverse,
                        int transparent)
/* transparent>0: don't overwrite background */
{
    static VIC_PIXEL c[4];
    unsigned int i;
    int b, x;
    BYTE d;

    /* Last character may exceed border, so we have some extra work */
    /* bordercheck asumes p pointing to display_xstart */
    p += xs * 8 * VIC_PIXEL_WIDTH;

    c[0] = VIC_PIXEL(vic.raster.background_color);
    c[1] = VIC_PIXEL(vic.mc_border_color);
    c[3] = VIC_PIXEL(vic.auxiliary_color);

    for (i = xs; i <= xe; i++, p += 8 * VIC_PIXEL_WIDTH) {
        b = *(vic.color_ptr + vic.memptr + i);
        c[2] = VIC_PIXEL(b & 0x7);
        if (reverse & !(b & 0x8))
            d = ~(GET_CHAR_DATA(*(vic.screen_ptr + vic.memptr + i),
                                vic.raster.ycounter));
        else
            d = GET_CHAR_DATA(*(vic.screen_ptr + vic.memptr + i),
                              vic.raster.ycounter);
        for (x = 0; x < 8; x++)
            PUT_PIXEL(p, d, c, b, x, transparent);
    }
}

static void draw_line(void)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    draw(p, 0, vic.text_cols - 1, 0, 0);
}

static void draw_reverse_line(void)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    draw(p, 0, vic.text_cols - 1, 1, 0);
}

static void draw_line_cached(raster_cache_t *cache, unsigned int xs,
                             unsigned int xe)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    /* Scale back xs and xe from 8 pixel units to text_cols.  */
    draw(p, xs >> VIC_PIXEL_WIDTH_SHIFT,
         ((xe + 1) >> VIC_PIXEL_WIDTH_SHIFT) - 1, 0, 0);
}

static void draw_reverse_line_cached(raster_cache_t *cache, unsigned int xs,
                                     unsigned int xe)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    draw(p, xs >> VIC_PIXEL_WIDTH_SHIFT,
         ((xe + 1) >> VIC_PIXEL_WIDTH_SHIFT) - 1, 1, 0);
}

static void draw_std_background(unsigned int start_pixel,
                                unsigned int end_pixel)
{
    memset(vic.raster.draw_buffer_ptr + start_pixel,
           vic.raster.background_color,
           (end_pixel - start_pixel + 1));
}

static void draw_std_foreground(unsigned int start_char,
                                unsigned int end_char)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    draw(p, start_char, end_char, 0, 1);
}

static void draw_rev_foreground(unsigned int start_char, unsigned int end_char)
{
    BYTE *p;

    p = (vic.raster.draw_buffer_ptr
        + vic.raster.display_xstart);

    draw(p, start_char, end_char, 1, 1);
}

static void setup_modes(void)
{
    raster_modes_set(vic.raster.modes, VIC_STANDARD_MODE,
                     fill_cache,
                     draw_line_cached,
                     draw_line,
                     draw_std_background,
                     draw_std_foreground);
    raster_modes_set(vic.raster.modes, VIC_REVERSE_MODE,
                     fill_cache,
                     draw_reverse_line_cached,
                     draw_reverse_line,
                     draw_std_background,
                     draw_rev_foreground);
}

void vic_draw_init(void)
{
    init_drawing_tables();

    setup_modes();
}

