/*
 * crtc.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * 16/24bpp support added by
 *  Steven Tieu (stieu@physics.ubc.ca)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include "crtc.h"

#include "crtc-draw.h"



static PIXEL4 dwg_table_0[256], dwg_table_1[256];
static PIXEL4 dwg_table2x_0[256], dwg_table2x_1[256];
static PIXEL4 dwg_table2x_2[256], dwg_table2x_3[256];

static void
init_drawing_tables (void)
{
    int byte, p;
    BYTE msk;

    for (byte = 0; byte < 0x0100; byte++) {
        *((PIXEL *) (dwg_table2x_0 + byte))
            = *((PIXEL *) (dwg_table2x_0 + byte) + 1)
            = RASTER_PIXEL(&crtc.raster, byte & 0x80 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_0 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_0 + byte) + 3)
            = RASTER_PIXEL(&crtc.raster, byte & 0x40 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_1 + byte))
            = *((PIXEL *) (dwg_table2x_1 + byte) + 1)
            = RASTER_PIXEL(&crtc.raster, byte & 0x20 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_1 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_1 + byte) + 3)
            = RASTER_PIXEL(&crtc.raster, byte & 0x10 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_2 + byte))
            = *((PIXEL *) (dwg_table2x_2 + byte) + 1)
            = RASTER_PIXEL(&crtc.raster, byte & 0x08 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_2 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_2 + byte) + 3)
            = RASTER_PIXEL(&crtc.raster, byte & 0x04 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_3 + byte))
            = *((PIXEL *) (dwg_table2x_3 + byte) + 1)
            = RASTER_PIXEL(&crtc.raster, byte & 0x02 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_3 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_3 + byte) + 3)
            = RASTER_PIXEL(&crtc.raster, byte & 0x01 ? 1 : 0);
    }

    for (byte = 0; byte < 0x0100; byte++) {
        for (msk = 0x80, p = 0; p < 4; msk >>= 1, p++)
            *((PIXEL *)(dwg_table_0 + byte) + p) 
		= RASTER_PIXEL(&crtc.raster, byte & msk ? 1 : 0);
        for (p = 0; p < 4; msk >>= 1, p++)
            *((PIXEL *)(dwg_table_1 + byte) + p) 
		= RASTER_PIXEL(&crtc.raster, byte & msk ? 1 : 0);
    }
}




static void draw_standard_background (unsigned int start_pixel,
           				          unsigned int end_pixel)
{
  vid_memset (crtc.raster.frame_buffer_ptr + start_pixel,
              RASTER_PIXEL (&crtc.raster, 0), 
	      end_pixel - start_pixel + 1);
}

#ifdef CRTC_NEED_2X
static void draw_standard_background_2x (unsigned int start_pixel,
                     				   unsigned int end_pixel)
{
  vid_memset (crtc.raster.frame_buffer_ptr + 2 * start_pixel,
              RASTER_PIXEL (&crtc.raster, 0), 
	      2 * (end_pixel - start_pixel + 1));
}
#endif

/* for debugging make it a function... */
void DRAW(int reverse_flag, int xs, int xe) 
{
    do {                                                                \
        PIXEL *p = crtc.raster.frame_buffer_ptr + CRTC_SCREEN_BORDERWIDTH;   \
	BYTE *chargen_ptr, *screen_ptr;					\
        register int i, d;                                              \
									\
	/* pointer to current chargen line */
	chargen_ptr = crtc.chargen_base 				\
		+ crtc.chargen_rel 					\
		+ crtc.raster.ycounter;					\
	/* pointer to current screen line */
	screen_ptr = crtc.screen_base + crtc.screen_rel;
#if 0
        if (crsrmode                                                    \
                && crsrstate                                            \
                && ycounter >= crsrstart                                \
                && ycounter <= crsrend )                                \
        for (i = 0; i < memptr_inc; i++, p += 8) {                      \
            d = chargen_ptr, (screenmem + memptr)[i],                   \
                                ycounter);                              \
            if ( (memptr+i)==crsrrel )                                  \
                d ^= 0xff;                                              \
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table_0[d];                           \
            *((PIXEL4 *) p + 1) = dwg_table_1[d];                       \
        }                                                               \
        else 
#endif
        for (i = xs; i <= xe; i++) {                                    \
	    /* we use 16 bytes/char character generator */
            d = *(chargen_ptr + (screen_ptr[i] << 4));                  \
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p + i * 2) = dwg_table_0[d];                   \
            *((PIXEL4 *) p + i * 2 + 1) = dwg_table_1[d];               \
        }                                                               \
/*                                                                     \
        d = (reverse_flag) ? 0xff: 0;                                   \
        for (; i < SCREEN_MAX_TEXTCOLS; i++, p += 8) {                  \
                *((PIXEL4 *) p) = dwg_table_0[d];                       \
                *((PIXEL4 *) p + 1) = dwg_table_1[d];                   \
        }  */                                                           \
    } while (0);
}

static void draw_standard_line(void)
{
    DRAW(0, 0, crtc.memptr_inc - 1);
}

static void draw_reverse_line(void)
{
    DRAW(1, 0, crtc.memptr_inc - 1);
}

#if 0

#define DRAW_2x(reverse_flag)                                           \
    do {                                                                \
        PIXEL *p = (frame_buffer_ptr                                    \
                    + SCREEN_BORDERWIDTH * pixel_width);                \
        register int i, d;                                              \
                                                                        \
        if (crsrmode                                                    \
                && crsrstate                                            \
                && ycounter >= crsrstart                                \
                && ycounter <= crsrend )                                \
        for (i = 0; i < memptr_inc; i++, p += 16) {                     \
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i],     \
                        ycounter);                                      \
            if ( (memptr+i)==crsrrel )                                  \
                d ^= 0xff;                                              \
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table2x_0[d];                         \
            *((PIXEL4 *) p + 1) = dwg_table2x_1[d];                     \
            *((PIXEL4 *) p + 2) = dwg_table2x_2[d];                     \
            *((PIXEL4 *) p + 3) = dwg_table2x_3[d];                     \
        }                                                               \
        else                                                            \
                                                                        \
        for (i = 0; i < memptr_inc; i++, p += 16) {                     \
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i],     \
                              ycounter);                                \
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table2x_0[d];                         \
            *((PIXEL4 *) p + 1) = dwg_table2x_1[d];                     \
            *((PIXEL4 *) p + 2) = dwg_table2x_2[d];                     \
            *((PIXEL4 *) p + 3) = dwg_table2x_3[d];                     \
        }                                                               \
    } while (0)

static void draw_standard_line_2x(unsigned int start_pixel,
                                                 unsigned int end_pixel)
{
    DRAW_2x(0);
}

static void draw_reverse_line_2x(unsigned int start_pixel,
                                                 unsigned int end_pixel)
{
    DRAW_2x(1);
}

#define DRAW_CACHED(l, xs, xe, reverse_flag)                            \
    do {                                                                \
        PIXEL *p = frame_buffer_ptr + SCREEN_BORDERWIDTH + (xs) * 8;    \
        register int i;                                                 \
        register int mempos = ((l->n+1)/screen_charheight )*memptr_inc; \
        register int ypos = (l->n+1) % screen_charheight;               \
                                                                        \
        for (i = (xs); i <= (xe); i++, p += 8) {                        \
            BYTE d = (l)->fgdata[i];                                    \
            if ((reverse_flag))                                         \
                d = ~d;                                                 \
                                                                        \
            if (crsrmode                                                \
                && crsrstate                                            \
                && ypos >= crsrstart                                    \
                && ypos <=crsrend                                       \
                && mempos+i==crsrrel                                    \
                ) {                                                     \
                d = ~d;                                                 \
            }                                                           \
            *((PIXEL4 *) p) = dwg_table_0[d];                           \
            *((PIXEL4 *) p + 1) = dwg_table_1[d];                       \
        }                                                               \
    } while (0)


static void draw_standard_line_cached(raster_cache_t *l, int xs, int xe)
{
    DRAW_CACHED(l, xs, xe, 0);
}

static void draw_reverse_line_cached(raster_cache_t *l, int xs, int xe)
{
    DRAW_CACHED(l, xs, xe, 1);
}


#define DRAW_CACHED_2x(l, xs, xe, reverse_flag)                 \
    do {                                                        \
        PIXEL *p = (frame_buffer_ptr                            \
                    + 2 * (SCREEN_BORDERWIDTH + (xs) * 8));     \
        register int i;                                         \
        register int mempos = ((l->n+1)/screen_charheight )     \
                                        *memptr_inc;            \
        register int ypos = (l->n+1) % screen_charheight;       \
                                                                \
        for (i = (xs); i <= (xe); i++, p += 16) {               \
            BYTE d = (l)->fgdata[i];                            \
            if ((reverse_flag))                                 \
                d = ~d;                                         \
                                                                \
            if (crsrmode                                        \
                && crsrstate                                    \
                && ypos >= crsrstart                            \
                && ypos <=crsrend                               \
                && mempos+i==crsrrel                            \
                ) {                                             \
                d = ~d;                                         \
            }                                                   \
            *((PIXEL4 *) p) = dwg_table2x_0[d];                 \
            *((PIXEL4 *) p + 1) = dwg_table2x_1[d];             \
            *((PIXEL4 *) p + 2) = dwg_table2x_2[d];             \
            *((PIXEL4 *) p + 3) = dwg_table2x_3[d];             \
        }                                                       \
    } while (0)

static void draw_standard_line_cached_2x(raster_cache_t *l, int xs, int xe)
{
    DRAW_CACHED_2x(l, xs, xe, 0);
}

static void draw_reverse_line_cached_2x(raster_cache_t *l, int xs, int xe)
{
    DRAW_CACHED_2x(l, xs, xe, 1);
}

#endif



static void setup_double_size_modes (void)
{
#if 0
    raster_modes_set (&crtc.raster.modes, CRTC_STANDARD_MODE,
                    NULL /* get_std_text */,
                    NULL /* draw_std_text_cached_2x */,
                    draw_std_text_2x,
                    draw_std_background_2x,
                    draw_std_text_foreground_2x);

    raster_modes_set (&crtc.raster.modes, CRTC_REVERSE_MODE,
                    get_rev_text,
                    draw_rev_text_cached_2x,
                    draw_rev_text_2x,
                    draw_rev_background_2x,
                    draw_rev_text_foreground_2x);
#endif
}

static void setup_single_size_modes (void)
{
    raster_modes_set (&crtc.raster.modes, CRTC_STANDARD_MODE,
                    NULL /* get_std_text */,
                    NULL /* draw_std_text_cached */,
                    draw_standard_line,
                    draw_standard_background,
                    NULL /* draw_std_text_foreground */ );
#if 0
    raster_modes_set (&crtc.raster.modes, CRTC_REVERSE_MODE,
                    get_rev_text,
                    draw_rev_text_cached,
                    draw_rev_text,
                    draw_rev_background,
                    draw_rev_text_foreground);
#endif
}



void
crtc_draw_init (void)
{
  init_drawing_tables ();

  raster_set_table_refresh_handler(&crtc.raster, init_drawing_tables);

#ifdef CRTC_NEED_2X
  crtc_draw_set_double_size (0);
#endif
}

void
crtc_draw_set_double_size (int enabled)
{
#ifdef CRTC_NEED_2X
  if (enabled)
    setup_double_size_modes ();
  else
#endif
    setup_single_size_modes ();
}
