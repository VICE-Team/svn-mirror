/*
 * vdc-draw.c - Rendering for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Markus Brenner <markus@brenner.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "vdc-draw.h"
#include "vdc-resources.h"
#include "vdc.h"
#include "vdctypes.h"

/* The following tables are used to speed up the drawing.  We do not use
   multi-dimensional arrays as we can optimize better this way...  */

/* foreground(4) | background(4) | nibble(4) -> 4 pixels.  */
#ifdef AVOID_STATIC_ARRAYS
static PIXEL4 *hr_table;
#else
static PIXEL4 hr_table[16 * 16 * 16];
#endif

#ifdef VDC_NEED_2X
/* foreground(4) | background(4) | idx(2) | nibble(4) -> 4 pixels.  */
#ifdef AVOID_STATIC_ARRAYS
static PIXEL4 *hr_table_2x;
#else
static PIXEL4 hr_table_2x[16 * 16 * 2 * 16];
#endif
#endif



/* These functions draw the background from `start_pixel' to `end_pixel'.  */
/*
static void
draw_std_background (unsigned int start_pixel,
                     unsigned int end_pixel)
{
  vid_memset (vdc.raster.frame_buffer_ptr + start_pixel,
              RASTER_PIXEL (&vdc.raster,
                            vdc.raster.overscan_background_color),
              end_pixel - start_pixel + 1);
}

#ifdef VDC_NEED_2X
static void
draw_std_background_2x (unsigned int start_pixel,
                        unsigned int end_pixel)
{
  vid_memset (vdc.raster.frame_buffer_ptr + 2 * start_pixel,
              RASTER_PIXEL (&vdc.raster,
                            vdc.raster.overscan_background_color),
              2 * (end_pixel - start_pixel + 1));
}
#endif
*/


/* Initialize the drawing tables.  */
static void
init_drawing_tables (void)
{
  DWORD i;
  unsigned int f, b;

#ifdef AVOID_STATIC_ARRAYS
  if (!hr_table)
    {
      hr_table = xmalloc (sizeof (*hr_table) * 16 * 16 * 16);
#ifdef VDC_NEED_2X
      hr_table_2x = xmalloc (sizeof (*hr_table_2x) * 16 * 16 * 2 * 16);
#endif
    }
#endif

  for (i = 0; i <= 0xf; i++)
    {
      for (f = 0; f <= 0xf; f++)
        {
          for (b = 0; b <= 0xf; b++)
            {
              PIXEL fp, bp;
              PIXEL *p;
              int offset;

              fp = RASTER_PIXEL (&vdc.raster, f);
              bp = RASTER_PIXEL (&vdc.raster, b);
              offset = (f << 8) | (b << 4);
              p = (PIXEL *) (hr_table + offset + i);

              *p = i & 0x8 ? fp : bp;
              *(p + 1) = i & 0x4 ? fp : bp;
              *(p + 2) = i & 0x2 ? fp : bp;
              *(p + 3) = i & 0x1 ? fp : bp;

#ifdef VDC_NEED_2X
              p = (PIXEL *) (hr_table_2x + (offset << 1) + i);
              *p = *(p + 1) = i & 0x8 ? fp : bp;
              *(p + 2) = *(p + 3) = i & 0x4 ? fp : bp;
              *(p + 0x40) = *(p + 0x41) = i & 0x2 ? fp : bp;
              *(p + 0x42) = *(p + 0x43) = i & 0x1 ? fp : bp;
#endif
            }
        }
    }
}


static int
get_std_text(raster_cache_t *cache,
             int *xs,
             int *xe,
             int rr)
{
    /* fill the line cache in text mode.
       The VDC combines text mode from
       a) the video RAM
          in conjunction with
       b) the character RAM

       c) the attribute RAM
     */
    int r;	/* return value */
    *xs = 0;
    *xe = VDC_SCREEN_TEXTCOLS;

    r = raster_cache_data_fill_attr_text(cache->foreground_data,
                                    vdc.ram+vdc.screen_adr+vdc.mem_counter,
                                    vdc.ram+vdc.attribute_adr+vdc.mem_counter,
                                    vdc.ram+vdc.chargen_adr,
                                    16,
                                    VDC_SCREEN_TEXTCOLS,
                                    (vdc.raster.ycounter
                                    / vdc.raster_ycounter_divide),
                                    xs, xe,
                                    rr,
                                    vdc.text_blink_visible);

    r |= raster_cache_data_fill(cache->color_data_1,
                                vdc.ram+vdc.attribute_adr+vdc.mem_counter,
                                VDC_SCREEN_TEXTCOLS,
                                1,
                                xs, xe,
                                rr);

    if (vdc.cursor_visible) {
        int crsrpos = vdc.crsrpos - vdc.mem_counter;

        if (crsrpos >= 0 && crsrpos < VDC_SCREEN_TEXTCOLS
            && (vdc.raster.ycounter / vdc.raster_ycounter_divide)
            >= (vdc.regs[10] & 0x1f)
            && (vdc.raster.ycounter / vdc.raster_ycounter_divide)
            < (vdc.regs[11] & 0x1f))
            cache->foreground_data[crsrpos] ^= 0xff;
    }
    return r;
}

static void
draw_std_text_cached(raster_cache_t *cache,
                     unsigned int xs,
                     unsigned int xe)
{
    PIXEL *p;
    PIXEL4 *table_ptr;

    unsigned int i;

    /* Only draw even rasterlines.  */
    if ((vdc.raster.ycounter & 1) && !vdc_resources.double_scan_enabled
        && vdc_resources.double_size_enabled)
        return;

    p = vdc.raster.frame_buffer_ptr + VDC_SCREEN_BORDERWIDTH + xs * 8;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    for (i = xs; i <= xe; i++, p += 8)
    {
        PIXEL4 *ptr = table_ptr + ((cache->color_data_1[i] & 0x0f) << 8);
        int d = cache->foreground_data[i];

        *((PIXEL4 *) p)     = *(ptr + (d >> 4));
        *((PIXEL4 *) p + 1) = *(ptr + (d & 0x0f));
    }
}

static void
draw_std_text(void)
{
    PIXEL *p;
    PIXEL4 *table_ptr;
    BYTE *attr_ptr, *screen_ptr, *char_ptr;

    unsigned int i;

    /* Only draw even rasterlines.  */
    if ((vdc.raster.ycounter & 1) && !vdc_resources.double_scan_enabled
        && vdc_resources.double_size_enabled)
        return;

    p = vdc.raster.frame_buffer_ptr + VDC_SCREEN_BORDERWIDTH;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    attr_ptr = vdc.ram+vdc.attribute_adr+vdc.mem_counter;
    screen_ptr = vdc.ram+vdc.screen_adr+vdc.mem_counter;
    char_ptr = vdc.ram+vdc.chargen_adr 
               + (vdc.raster.ycounter / vdc.raster_ycounter_divide);

    for (i = 0; i < vdc.mem_counter_inc; i++, p+= 8)
    {
        PIXEL4 *ptr = table_ptr + ((*(attr_ptr + i) & 0x0f) << 8);

	int d = *(char_ptr
            + ((*(attr_ptr + i) & 0x80) ? 0x1000 : 0)
            + (*(screen_ptr + i) * 16));

        *((PIXEL4 *) p)     = *(ptr + (d >> 4));
        *((PIXEL4 *) p + 1) = *(ptr + (d & 0x0f));
    }
}


static int
get_std_bitmap(raster_cache_t *cache,
             int *xs,
             int *xe,
             int rr)
{
    /* fill the line cache in text mode.
       The VDC combines text mode from
       a) the video RAM
          in conjunction with
       b) the character RAM

       c) the attribute RAM
     */
    int r;	/* return value */
    *xs = 0;
    *xe = VDC_SCREEN_TEXTCOLS;

    r = raster_cache_data_fill(cache->foreground_data,
                               vdc.ram+vdc.screen_adr+vdc.bitmap_counter,
                               VDC_SCREEN_TEXTCOLS,
                               1,
                               xs, xe,
                               rr);

    if (vdc.regs[25] & 0x40)
        r |= raster_cache_data_fill(cache->color_data_1,
                                    vdc.ram+vdc.attribute_adr+vdc.mem_counter,
                                    VDC_SCREEN_TEXTCOLS,
                                    1,
                                    xs, xe,
                                    rr);
    else
        r |= raster_cache_data_fill_const(cache->color_data_1,
                                    vdc.regs[26] >> 4,
                                    VDC_SCREEN_TEXTCOLS,
                                    1,
                                    xs, xe,
                                    rr);
    return r;
}

static void
draw_std_bitmap_cached(raster_cache_t *cache,
                     unsigned int xs,
                     unsigned int xe)
{
    PIXEL *p;
    PIXEL4 *table_ptr;

    unsigned int i;

    /* only draw even rasterlines */
    if ((vdc.raster.ycounter & 1) && !vdc_resources.double_scan_enabled
        && vdc_resources.double_size_enabled)
        return;

    p = vdc.raster.frame_buffer_ptr + VDC_SCREEN_BORDERWIDTH + xs * 8;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    for (i = xs; i <= xe; i++, p += 8)
    {
        PIXEL4 *ptr = table_ptr + ((cache->color_data_1[i] & 0x0f) << 8);
        int d = cache->foreground_data[i];

        *((PIXEL4 *) p)     = *(ptr + (d >> 4));
        *((PIXEL4 *) p + 1) = *(ptr + (d & 0x0f));
    }
}

static void
draw_std_bitmap(void)
{
    PIXEL *p;
    PIXEL4 *table_ptr;
    BYTE *attr_ptr, *bitmap_ptr;

    unsigned int i;

    /* only draw even rasterlines */
    if ((vdc.raster.ycounter & 1) && !vdc_resources.double_scan_enabled
        && vdc_resources.double_size_enabled)
        return;

    p = vdc.raster.frame_buffer_ptr + VDC_SCREEN_BORDERWIDTH;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    attr_ptr = vdc.ram+vdc.attribute_adr+vdc.mem_counter;
    bitmap_ptr = vdc.ram+vdc.screen_adr+vdc.bitmap_counter;

    for (i = 0; i < vdc.mem_counter_inc; i++, p+= 8)
    {
        PIXEL4 *ptr;
	int d;

	if (vdc.regs[25] & 0x40)
            ptr = table_ptr + ((*(attr_ptr + i) & 0x0f) << 8);
	else
            ptr = table_ptr + ((vdc.regs[26] & 0xf0) << 4);

	d = *(bitmap_ptr + i);

        *((PIXEL4 *) p)     = *(ptr + (d >> 4));
        *((PIXEL4 *) p + 1) = *(ptr + (d & 0x0f));
    }
}


static void setup_single_size_modes(void)
{
    raster_modes_set(&vdc.raster.modes, VDC_TEXT_MODE,
                     get_std_text,
                     draw_std_text_cached,
                     draw_std_text,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(&vdc.raster.modes, VDC_BITMAP_MODE,
                     get_std_bitmap,
                     draw_std_bitmap_cached,
                     draw_std_bitmap,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(&vdc.raster.modes, VDC_IDLE_MODE,
                     NULL,
                     NULL,
                     NULL,
                     NULL, /* draw_std_background */
                     NULL); /*draw_std_text_foreground */
}

static void setup_double_size_modes(void)
{
    raster_modes_set(&vdc.raster.modes, VDC_TEXT_MODE,
/*
                     cache_std_text,
*/
                     NULL,
                     NULL,
                     NULL,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(&vdc.raster.modes, VDC_BITMAP_MODE,
                     NULL,
                     NULL,
                     NULL,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(&vdc.raster.modes, VDC_IDLE_MODE,
                     NULL,
                     NULL,
                     NULL,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */
}



void
vdc_draw_init (void)
{
  init_drawing_tables ();

  raster_set_table_refresh_handler(&vdc.raster, init_drawing_tables);

#ifdef VDC_NEED_2X
  vdc_draw_set_double_size (0);
#endif

}

void
vdc_draw_set_double_size (int enabled)
{
#ifdef VDC_NEED_2X
  if (enabled)
    setup_double_size_modes ();
  else
#endif
    setup_single_size_modes ();
}
