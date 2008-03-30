/*
 * vdc-draw.c - Rendering for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Markus Brenner <markus@brenner.de>
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

#include "raster-cache-const.h"
#include "raster-cache-fill.h"
#include "raster-cache.h"
#include "raster-modes.h"
#include "types.h"
#include "vdc-draw.h"
#include "vdc-resources.h"
#include "vdc.h"
#include "vdctypes.h"

/* The following tables are used to speed up the drawing.  We do not use
   multi-dimensional arrays as we can optimize better this way...  */

/* foreground(4) | background(4) | nibble(4) -> 4 pixels.  */
static DWORD hr_table[16 * 16 * 16];


/* These functions draw the background from `start_pixel' to `end_pixel'.  */
/*
static void draw_std_background(unsigned int start_pixel,
                                unsigned int end_pixel)
{
    memset(vdc.raster.draw_buffer_ptr + start_pixel,
           vdc.raster.idle_background_color,
           end_pixel - start_pixel + 1);
}
*/

/* Initialize the drawing tables.  */
static void init_drawing_tables(void)
{
    DWORD i;
    unsigned int f, b;

    for (i = 0; i <= 0xf; i++) {
        for (f = 0; f <= 0xf; f++) {
            for (b = 0; b <= 0xf; b++) {
                BYTE fp, bp;
                BYTE *p;
                int offset;

                fp = f;
                bp = b;
                offset = (f << 8) | (b << 4);
                p = (BYTE *)(hr_table + offset + i);

                *p = i & 0x8 ? fp : bp;
                *(p + 1) = i & 0x4 ? fp : bp;
                *(p + 2) = i & 0x2 ? fp : bp;
                *(p + 3) = i & 0x1 ? fp : bp;
            }
        }
    }
}

/*-----------------------------------------------------------------------*/

inline static BYTE get_attr_char_data(BYTE c, BYTE a, int l, BYTE *char_mem,
                                      int bytes_per_char, int blink,
                                      int revers, int curpos, int index)
{
    BYTE data;
	static BYTE mask[16] = { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0x00,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	static BYTE crsrblink[4] = { 0x01, 0x00, 0x08, 0x10 };

    if (a & VDC_ALTCHARSET_ATTR)
        char_mem += 0x1000;

	if (l > vdc.regs[23]) data = 0x00;
	else data = char_mem[(c * bytes_per_char) + l] & mask[vdc.regs[22] & 0x0F];
	if ((l == vdc.regs[29]) && (a & VDC_UNDERLINE_ATTR)) data = 0xFF;
	if ((a & VDC_FLASH_ATTR) && (vdc.attribute_blink)) data = 0x00; // underline byte also blinks!
	if (a & VDC_REVERSE_ATTR) data ^= 0xFF;
	if (vdc.regs[24] & 0x40) data ^= 0xFF;

	// on a 80x25 text screen (2000 characters) this is only true for 1 character.
	if (curpos == index) {
		// invert anything at all?
		if ((vdc.frame_counter | 1) & crsrblink[(vdc.regs[10] >> 5) & 3]) {
			// invert current byte of the character?
			if ((l >= (vdc.regs[10] & 0x1F)) && (l < (vdc.regs[11] & 0x1F))) data ^= 0xFF;
		}
	}

    return data;
}

inline static int cache_data_fill_attr_text(BYTE *dest,
                                            const BYTE *src,
                                            BYTE *attr,
                                            BYTE *char_mem,
                                            int bytes_per_char,
                                            unsigned int length,
                                            int l,
                                            unsigned int *xs,
                                            unsigned int *xe,
                                            int no_check,
                                            int blink,
                                            int revers,
                                            int curpos)
{
    unsigned int i;

    if (no_check) {
        *xs = 0;
        *xe = length - 1;
        for (i = 0; i < length; i++, src++, attr++)
            dest[i] = get_attr_char_data(src[0], attr[0], l, char_mem,
                                         bytes_per_char, blink, revers,
                                         curpos, i);
        return 1;
    } else {
        BYTE b;

        for (i = 0; i < length; i++, src++, attr++) {
            if (dest[i] != get_attr_char_data(src[0], attr[0], l, char_mem,
                bytes_per_char, blink, revers, curpos, i))
                break;
        }
        if (i < length) {
            *xs = *xe = i;

            for (; i < length; i++, src++, attr++) {
                b = get_attr_char_data(src[0], attr[0], l, char_mem,
                                       bytes_per_char, blink, revers,
                                       curpos, i);
                if (dest[i] != b) {
                    dest[i] = b;
                    *xe = i;
                }
            }
            return 1;
        } else {
            return 0;
        }
    }
}

inline static int cache_data_fill_attr_text_const(BYTE *dest,
                                                  const BYTE *src,
                                                  BYTE attr,
                                                  BYTE *char_mem,
                                                  int bytes_per_char,
                                                  unsigned int length,
                                                  int l,
                                                  unsigned int *xs,
                                                  unsigned int *xe,
                                                  int no_check,
                                                  int blink,
                                                  int revers,
                                                  int curpos)
{
    unsigned int i;

    if (no_check) {
        *xs = 0;
        *xe = length - 1;
        for (i = 0; i < length; i++, src++)
            dest[i] = get_attr_char_data(src[0], attr, l, char_mem,
                                         bytes_per_char, blink, revers,
                                         curpos, i);
        return 1;
    } else {
        BYTE b;

        for (i = 0; i < length; i++, src++) {
            if (dest[i] != get_attr_char_data(src[0], attr, l, char_mem,
                bytes_per_char, blink, revers, curpos, i))
                break;
        }
        if (i < length) {
            *xs = *xe = i;

            for (; i < length; i++, src++) {
                b = get_attr_char_data(src[0], attr, l, char_mem,
                                       bytes_per_char, blink, revers,
                                       curpos, i);
                if (dest[i] != b) {
                    dest[i] = b;
                    *xe = i;
                }
            }
            return 1;
        } else {
            return 0;
        }
    }
}

inline static int cache_data_fill(BYTE *dest,
                                  const BYTE *src,
                                  unsigned int length,
                                  int src_step,
                                  unsigned int *xs,
                                  unsigned int *xe,
                                  int no_check,
                                  int reverse)
{
    unsigned int i;

    if (no_check) {
        *xs = 0;
        *xe = length - 1;
        for (i = 0; i < length; i++, src += src_step)
            dest[i] = src[0] ^ reverse;
        return 1;
    } else {
        for (i = 0; i < length && dest[i] == (src[0] ^ reverse);
            i++, src += src_step)
          /* do nothing */ ;

        if (i < length) {
            *xs = *xe = i;

            for (; i < length; i++, src += src_step) {
                if (dest[i] != (src[0] ^ reverse)) {
                    dest[i] = src[0] ^ reverse;
                    *xe = i;
                }
            }
            return 1;
        } else {
            return 0;
        }
    }
}

/*-----------------------------------------------------------------------*/

static int get_std_text(raster_cache_t *cache, unsigned int *xs,
                        unsigned int *xe, int rr)
{
    /* fill the line cache in text mode.
       The VDC combines text mode from
       a) the video RAM
          in conjunction with
       b) the character RAM

       c) the attribute RAM
     */
    int r, cursor_pos = -1;

	cursor_pos = vdc.crsrpos - vdc.mem_counter;

    if (vdc.regs[25] & 0x40) {
        r = cache_data_fill_attr_text(cache->foreground_data,
                                vdc.ram + vdc.screen_adr + vdc.mem_counter,
                                vdc.ram + vdc.attribute_adr + vdc.mem_counter,
                                vdc.ram + vdc.chargen_adr,
                                vdc.bytes_per_char,
                                vdc.screen_text_cols,
                                vdc.raster.ycounter,
                                xs, xe,
                                rr,
                                0,
                                0,
                                cursor_pos);
        r |= raster_cache_data_fill(cache->color_data_1,
                                vdc.ram + vdc.attribute_adr + vdc.mem_counter,
                                vdc.screen_text_cols,
                                1,
                                xs, xe,
                                rr);
    } else {
        r = cache_data_fill_attr_text_const(cache->foreground_data,
                                vdc.ram + vdc.screen_adr + vdc.mem_counter,
                                (BYTE)(vdc.regs[26] & 0x0f),
                                vdc.ram + vdc.chargen_adr,
                                vdc.bytes_per_char,
                                (int)vdc.screen_text_cols,
                                vdc.raster.ycounter,
                                xs, xe,
                                rr,
                                0,
                                0,
                                cursor_pos);
        r |= raster_cache_data_fill_const(cache->color_data_1,
                                (BYTE)(vdc.regs[26] >> 4),
                                (int)vdc.screen_text_cols,
                                xs, xe,
                                rr);
    }

    return r;
}

static void draw_std_text_cached(raster_cache_t *cache, unsigned int xs,
                                 unsigned int xe)
{
    BYTE *p;
    DWORD *table_ptr;

    unsigned int i;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth + xs * 8;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    for (i = xs; i <= (unsigned int)xe; i++, p += 8) {
        DWORD *ptr = table_ptr + ((cache->color_data_1[i] & 0x0f) << 8);
        int d = cache->foreground_data[i];

        *((DWORD *)p) = *(ptr + (d >> 4));
        *((DWORD *)p + 1) = *(ptr + (d & 0x0f));
    }
}

static void draw_std_text(void)
{
    BYTE *p;
    DWORD *table_ptr;
    BYTE *attr_ptr, *screen_ptr, *char_ptr;

    unsigned int i;
    unsigned int cpos = 0xffff;

	cpos = vdc.crsrpos - vdc.mem_counter;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth;
    table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

    attr_ptr = vdc.ram + vdc.attribute_adr + vdc.mem_counter;
    screen_ptr = vdc.ram + vdc.screen_adr + vdc.mem_counter;
    char_ptr = vdc.ram + vdc.chargen_adr + vdc.raster.ycounter;

    for (i = 0; i < vdc.screen_text_cols; i++, p += 8) {
        DWORD *ptr = table_ptr + ((*(attr_ptr + i) & 0x0f) << 8);

        int d = *(char_ptr
            + ((*(attr_ptr + i) & VDC_ALTCHARSET_ATTR) ? 0x1000 : 0)
            + (*(screen_ptr + i) * vdc.bytes_per_char));

        if (*(attr_ptr + i) & VDC_REVERSE_ATTR
            || (vdc.attribute_blink && (*(attr_ptr + i) & VDC_FLASH_ATTR)))
            d ^= 0xff;

        if (cpos == i)
            d ^= 0xff;

        if (vdc.regs[24] & VDC_REVERSE_ATTR)
            d ^= 0xff;

        *((DWORD *)p) = *(ptr + (d >> 4));
        *((DWORD *)p + 1) = *(ptr + (d & 0x0f));
    }

}


static int get_std_bitmap(raster_cache_t *cache, unsigned int *xs,
                          unsigned int *xe, int rr)
{
    /* fill the line cache in text mode.
       The VDC combines text mode from
       a) the video RAM
          in conjunction with
       b) the character RAM

       c) the attribute RAM
     */
    int r;      /* return value */

    r = cache_data_fill(cache->foreground_data,
                        vdc.ram + vdc.screen_adr + vdc.bitmap_counter,
                        vdc.screen_text_cols,
                        1,
                        xs, xe,
                        rr,
                        (vdc.regs[24] & VDC_REVERSE_ATTR) ? 0xff : 0x0);

    if (vdc.regs[25] & 0x40)
        r |= raster_cache_data_fill(cache->color_data_1,
                                    vdc.ram + vdc.attribute_adr
                                    + vdc.mem_counter,
                                    vdc.screen_text_cols,
                                    1,
                                    xs, xe,
                                    rr);
    else
        r |= raster_cache_data_fill_const(cache->color_data_1,
                                          (BYTE)(vdc.regs[26] >> 4),
                                          (int)vdc.screen_text_cols,
                                          xs, xe,
                                          rr);
    return r;
}

static void draw_std_bitmap_cached(raster_cache_t *cache, unsigned int xs,
                                   unsigned int xe)
{
    BYTE *p;
    DWORD *table_ptr, *ptr;

    unsigned int i;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth + xs * 8;

    if (vdc.regs[25] & 0x40) {
        for (i = xs; i <= (unsigned int)xe; i++, p += 8) {
            int d = cache->foreground_data[i];

            table_ptr = hr_table + (cache->color_data_1[i] & 0xf0);
            ptr = table_ptr + ((cache->color_data_1[i] & 0x0f) << 8);

            *((DWORD *)p) = *(ptr + (d >> 4));
            *((DWORD *)p + 1) = *(ptr + (d & 0x0f));
        }
    } else {
        table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);

        for (i = xs; i <= (unsigned int)xe; i++, p += 8) {
            int d = cache->foreground_data[i];

            ptr = table_ptr + ((cache->color_data_1[i] & 0x0f) << 8);

            *((DWORD *)p) = *(ptr + (d >> 4));
            *((DWORD *)p + 1) = *(ptr + (d & 0x0f));
        }
    }
}

static void draw_std_bitmap(void)
{
    BYTE *p;
    DWORD *table_ptr;
    BYTE *attr_ptr, *bitmap_ptr;

    unsigned int i;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth;

    attr_ptr = vdc.ram + vdc.attribute_adr + vdc.mem_counter;
    bitmap_ptr = vdc.ram + vdc.screen_adr + vdc.bitmap_counter;

    for (i = 0; i < vdc.mem_counter_inc; i++, p+= 8) {
        DWORD *ptr;
        int d;

        if (vdc.regs[25] & 0x40) {
            table_ptr = hr_table + (*(attr_ptr + i) & 0xf0);
            ptr = table_ptr + ((*(attr_ptr + i) & 0x0f) << 8);
        } else {
            table_ptr = hr_table + ((vdc.regs[26] & 0x0f) << 4);
            ptr = table_ptr + ((vdc.regs[26] & 0xf0) << 4);
        }

        d = *(bitmap_ptr + i);

        if (vdc.regs[24] & VDC_REVERSE_ATTR)
            d ^= 0xff;

        *((DWORD *)p) = *(ptr + (d >> 4));
        *((DWORD *)p + 1) = *(ptr + (d & 0x0f));
    }
}


static int get_idle(raster_cache_t *cache, unsigned int *xs, unsigned int *xe,
                    int rr)
{
    if (rr || (vdc.regs[26] >> 4) != cache->color_data_1[0]) {
        *xs = 0;
        *xe = vdc.screen_text_cols;
        cache->color_data_1[0] = vdc.regs[26] >> 4;
        return 1;
    }

    return 0;
}

static void draw_idle_cached(raster_cache_t *cache, unsigned int xs,
                             unsigned int xe)
{
    BYTE *p;
    DWORD idleval;

    unsigned int i;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth + xs * 8;

    idleval = *(hr_table + ((cache->color_data_1[0] & 0x0f) << 8));

    for (i = xs; i <= (unsigned int)xe; i++, p += 8) {
        *((DWORD *)p) = idleval;
        *((DWORD *)p + 1) = idleval;
    }
}

static void draw_idle(void)
{
    BYTE *p;
    DWORD idleval;

    unsigned int i;

    p = vdc.raster.draw_buffer_ptr + vdc.border_width
        + vdc.raster.xsmooth;

    idleval = *(hr_table + ((vdc.regs[26] & 0xf0) << 4));

    for (i = 0; i < vdc.mem_counter_inc; i++, p+= 8) {
        *((DWORD *)p) = idleval;
        *((DWORD *)p + 1) = idleval;
    }
}


static void setup_modes(void)
{
    raster_modes_set(vdc.raster.modes, VDC_TEXT_MODE,
                     get_std_text,
                     draw_std_text_cached,
                     draw_std_text,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(vdc.raster.modes, VDC_BITMAP_MODE,
                     get_std_bitmap,
                     draw_std_bitmap_cached,
                     draw_std_bitmap,
                     NULL, /* draw_std_background */
                     NULL); /* draw_std_text_foreground */

    raster_modes_set(vdc.raster.modes, VDC_IDLE_MODE,
                     get_idle,
                     draw_idle_cached,
                     draw_idle,
                     NULL, /* draw_std_background */
                     NULL); /*draw_std_text_foreground */
}

void vdc_draw_init(void)
{
    init_drawing_tables();

    setup_modes();
}

