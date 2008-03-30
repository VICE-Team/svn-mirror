/*
 * raster.c - Common routines for video chip emulation.
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

/* This file is ugly, and needs better organization.  In particular, it would
   be nice to remove all the machine-specific hacks.  And it would be nice to
   put it into a standalone module, although this would probably affect
   performance.  */

#define _RASTER_C

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include "vice.h"
#include "vmachine.h"
#include "resources.h"

#ifndef _RASTER_H
#include "raster.h"
#endif

#include "vsync.h"

#undef MIN
#undef MAX
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

/* XShmPutImage is a bit more picky about its parameters than XPutImage.
   This is useful to debug XLib failures caused by wrong calls.  */
/* #define RASTER_DEBUG_PUTIMAGE_CALLS */

inline static void refresh_all(void);

/* ------------------------------------------------------------------------- */

inline static void apply_change(struct changes *c, int idx)
{
    switch (c->actions[idx].type) {
      case CHANGE_BYTE:
	*(c->actions[idx].value.integer.oldp) = c->actions[idx].value.integer.new;
	break;
      case CHANGE_PTR:
      default:
	*(c->actions[idx].value.ptr.oldp) = c->actions[idx].value.ptr.new;
    }
}

inline static void apply_all_changes(struct changes *c)
{
    int i;

    for (i = 0; i < c->count; i++)
        apply_change(c, i);
    c->count = 0;
}

inline static void add_int_change(struct changes *c, int where,
				  int *ptr, int new_value)
{
    int count = c->count++;

    c->actions[count].where = where;
    c->actions[count].type = CHANGE_BYTE;
    c->actions[count].value.integer.oldp = ptr;
    c->actions[count].value.integer.new = new_value;
}

inline static void add_ptr_change(struct changes *c, int where,
				  void **ptr, void *new_value)
{
    int count = c->count++;

    c->actions[count].where = where;
    c->actions[count].type = CHANGE_PTR;
    c->actions[count].value.ptr.oldp = ptr;
    c->actions[count].value.ptr.new = new_value;
}

inline static void add_int_change_next_line(int *ptr, int new_value)
{
    if (skip_next_frame)
	*ptr = new_value;
    else
	add_int_change(&next_line_changes, 0, ptr, new_value);
}

inline static void add_ptr_change_next_line(void **ptr, void *new_value)
{
    if (skip_next_frame)
	*ptr = new_value;
    else
	add_ptr_change(&next_line_changes, 0, ptr, new_value);
}

inline static void add_int_change_foreground(int char_x, int *ptr,
                                             int new_value)
{
    if (skip_next_frame || char_x <= 0) {
	*ptr = new_value;
    } else if (char_x < SCREEN_TEXTCOLS) {
	add_int_change(&foreground_changes, char_x, ptr, new_value);
	have_changes_on_this_line = 1;
    } else {
	add_int_change_next_line(ptr, new_value);
    }
}

inline static void add_ptr_change_foreground(int char_x, void **ptr,
					     void *new_value)
{
    if (skip_next_frame || char_x <= 0) {
	*ptr = new_value;
    } else if (char_x < SCREEN_TEXTCOLS) {
	add_ptr_change(&foreground_changes, char_x, ptr, new_value);
	have_changes_on_this_line = 1;
    } else {
	add_ptr_change_next_line(ptr, new_value);
    }
}

inline static void add_int_change_background(int raster_x, int *ptr,
					     int new_value)
{
    if (skip_next_frame || raster_x <= 0) {
	*ptr = new_value;
    } else if (raster_x < SCREEN_WIDTH) {
	add_int_change(&background_changes, raster_x, ptr, new_value);
	have_changes_on_this_line = 1;
    } else {
	add_int_change_next_line(ptr, new_value);
    }
}

inline static void add_ptr_change_background(int raster_x, void **ptr,
					     void *new_value)
{
    if (skip_next_frame || raster_x <= 0) {
	*ptr = new_value;
    } else if (raster_x < SCREEN_WIDTH) {
	add_ptr_change(&background_changes, raster_x, ptr, new_value);
	have_changes_on_this_line = 1;
    } else {
	add_ptr_change_next_line(ptr, new_value);
    }
}

inline static void add_int_change_border(int raster_x, int *ptr,
					 int new_value)
{
    if (skip_next_frame || raster_x <= 0) {
	*ptr = new_value;
    } else if (raster_x < SCREEN_WIDTH) {
	add_int_change(&border_changes, raster_x, ptr, new_value);
	have_changes_on_this_line = 1;
    } else {
	add_int_change_next_line(ptr, new_value);
    }
}

/* ------------------------------------------------------------------------- */

#define vid_memcpy(dst, src, cnt) memcpy(dst, src, cnt * sizeof(PIXEL))

#if X_DISPLAY_DEPTH > 8
inline static void vid_memset(PIXEL *dst, PIXEL c, int cnt)
{
    int i;

    for (i = 0; i < cnt; i++)
	dst[i] = c;
}

/* This is used to paint borders and blank lines.  */
#define DRAW_BLANK(p, start, end, pixel_width)			\
    do {							\
	PIXEL *dst;						\
	int i;							\
    								\
	dst = (PIXEL *)p + start * pixel_width;			\
	for(i = 0;i < ((end) - (start) + 1) * pixel_width; i++)	\
	       *(dst++) = PIXEL((int)border_color);		\
    } while(0)

#else  /* 8 bit depth */
#define vid_memset(dst, c, cnt) memset(dst,c,cnt)

/* This is used to paint borders and blank lines.  */
#define DRAW_BLANK(p, start, end, pixel_width)		\
    vid_memset(((BYTE *)(p) + (start) * pixel_width),	\
	       PIXEL(border_color),			\
	       ((end) - (start) + 1) * pixel_width)

#endif

/* ------------------------------------------------------------------------- */

/* Initialization.  */
static void reset_raster(void)
{
    int i;

    frame_buffer_ptr = (FRAME_BUFFER_START(frame_buffer)
			+ 2 * SCREEN_MAX_SPRITE_WIDTH);

    rasterline = 0;
    oldclk = 0;
    xsmooth = ysmooth = 0;
    blank = 0;
    mem_counter = memptr = 0;

#ifdef __VIC_II__
    mem_counter_inc = SCREEN_TEXTCOLS;
#endif

    ycounter = 0;
    asleep = 0;

    changed_area.is_null = 1;
    for (i = 0; i < SCREEN_HEIGHT; i++) {
#if SCREEN_NUM_SPRITES > 0
	int j;
	for (j = 0; j < SCREEN_NUM_SPRITES; j++)
	    memset(&(cache[i].sprites[j]), 0, sizeof(struct sprite_line_cache));
#endif
	cache[i].is_dirty = 1;
    }

#if SCREEN_NUM_SPRITES > 0
#if 0
    for (i = 0; i < SCREEN_NUM_SPRITES; i++)
	memset(&(sprites[i]), 0, sizeof(struct sprite));
#endif
    sprite_data = sprite_data_1;
    new_sprite_data = sprite_data_2;
#endif
}

static int init_raster(int active, int max_pixel_width, int max_pixel_height)
{
    /* Keep enough space on the left and the right for one sprite of the
       maximum size.  This way we can clip sprites more easily.  */
    if (frame_buffer_alloc(&frame_buffer,
			   ((SCREEN_WIDTH + 2 * 2 * SCREEN_MAX_SPRITE_WIDTH)
			    * max_pixel_width),
			   (SCREEN_HEIGHT + 1) * max_pixel_height))
	return -1;

    reset_raster();

    return 0;
}

/* Resize the canvas with the specified values and center the screen image on
   it.  The actual size can be different if the parameters are not suitable.  */
static void resize(unsigned int width, unsigned int height)
{
    if (width >= SCREEN_WIDTH * pixel_width) {
	window_x_offset = (width - SCREEN_WIDTH * pixel_width) / 2;
	window_first_x = 0;
    } else {
	window_x_offset = 0;
#ifndef SCREEN_BORDERWIDTH_VARIES
	if (width > SCREEN_XPIX * pixel_width)
	    window_first_x = (SCREEN_BORDERWIDTH
			      - (width / pixel_width - SCREEN_XPIX) / 2);
	else
	    window_first_x = SCREEN_BORDERWIDTH;
#else
	window_first_x = (SCREEN_WIDTH - width / pixel_width) / 2;
#endif
    }

    if (height >= SCREEN_HEIGHT * pixel_height) {
	window_y_offset = (height - SCREEN_HEIGHT * pixel_height) / 2;
	window_first_line = 0;
	window_last_line = window_first_line + SCREEN_HEIGHT - 1;
    } else {
	window_y_offset = 0;
#ifndef SCREEN_BORDERHEIGHT_VARIES
	if (height > SCREEN_YPIX * pixel_height)
	    window_first_line = (SCREEN_BORDERHEIGHT
				 - (height / pixel_height - SCREEN_YPIX) / 2);
	else
	    window_first_line = SCREEN_BORDERHEIGHT;
#else
	window_first_line = (SCREEN_HEIGHT - height / pixel_height) / 2;
#endif
	window_last_line = window_first_line + height / pixel_height;
    }

    canvas_resize(canvas, width, height);

    /* Make sure we don't waste space showing unused lines.  */
    if ((window_first_line < SCREEN_FIRST_DISPLAYED_LINE
	 && window_last_line < SCREEN_LAST_DISPLAYED_LINE)
	|| (window_first_line > SCREEN_FIRST_DISPLAYED_LINE
	    && window_last_line > SCREEN_LAST_DISPLAYED_LINE)) {
	window_first_line = SCREEN_FIRST_DISPLAYED_LINE;
	window_last_line = window_first_line + height / pixel_height;
    }

    window_width = width;
    window_height = height;
}

/* Open the emulation window.  */
static int open_output_window(char *win_name, unsigned int width,
			      unsigned int height, palette_t *palette,
			      canvas_redraw_t exposure_handler)
{
    int i;

    window_width = width * pixel_width;
    window_height = height * pixel_height;

    canvas = canvas_create(win_name, &window_width, &window_height, !asleep,
			   exposure_handler, palette, pixel_table);

    /* Prepare the double and quad pixel tables.  */
    for (i = 0; i < 0x100; i++)
	*((PIXEL *)(double_pixel_table + i))
	    = *((PIXEL *)(double_pixel_table + i) + 1) = pixel_table[i];
    for (i = 0; i < 0x100; i++)
	*((PIXEL2 *)(quad_pixel_table + i))
	    = *((PIXEL2 *)(quad_pixel_table + i) + 1) = double_pixel_table[i];

    if (!canvas)
	return 1;

    resize(window_width, window_height);

    frame_buffer_clear(&frame_buffer, PIXEL(0));

    refresh_all();
    return 0;
}

/* ------------------------------------------------------------------------- */

/* set a new palette */

static int set_palette_file_name(resource_value_t v)
{
    int i;
    /* If called before initialization, just set the resource value.  The
       palette file will be loaded afterwards.  */
    if (palette == NULL) {
        string_set(&palette_file_name, (char *) v);
        return 0;
    }

    if (palette_load((char *) v, palette) < 0) {
        fprintf(stderr, "Couldn't load palette `%s'\n", (char *) v);
        return -1;
    }
    canvas_set_palette(canvas, palette, pixel_table);

    /* Prepare the double and quad pixel tables.  */
    for (i = 0; i < 0x100; i++)
	*((PIXEL *)(double_pixel_table + i))
	    = *((PIXEL *)(double_pixel_table + i) + 1) = pixel_table[i];
    for (i = 0; i < 0x100; i++)
	*((PIXEL2 *)(quad_pixel_table + i))
	    = *((PIXEL2 *)(quad_pixel_table + i) + 1) = double_pixel_table[i];

    init_drawing_tables();

    /* Make sure the pixel tables are recalculated properly.  */
    video_resize();

    string_set(&palette_file_name, (char *) v);
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Read length bytes from src and store them in dest, checking for differences
   between the two arrays.  The smallest interval that contains different bytes
   is returned as [*xs; *xe].  */
inline static int _fill_cache(BYTE *dest, BYTE *src, int length, int srcstep,
			      int *xs, int *xe, int no_check)
{
    if (no_check) {
	int i;

	*xs = 0;
	*xe = length - 1;
	if (srcstep == 1)
	    memcpy(dest, src, length);
	else
	    for (i = 0; i < length; i++, src += srcstep)
		dest[i] = src[0];
	return 1;
    } else {
	int x = 0, i;

	for (i = 0; dest[i] == src[0] && i < length; i++, src += srcstep)
	    /* do nothing */ ;

	if (i < length) {
	    if (*xs > i)
		*xs = i;
	    for (; i < length; i++, src += srcstep)
		if (dest[i] != src[0]) {
		    dest[i] = src[0];
		    x = i;
		}
	    if (*xe < x)
		*xe = x;
	    return 1;
	} else
	    return 0;
    }
}

/* Do as _fill_cache(), but split each byte into low and high nibble.  These
   are stored into different destinations.  */
inline static int _fill_cache_nibbles(BYTE * desthi, BYTE * destlo, BYTE * src,
				      int length, int srcstep, int *xs, int *xe,
				      int no_check)
{
    if (no_check) {
	int i;

	*xs = 0;
	*xe = length - 1;
	for (i = 0; i < length; i++, src += srcstep) {
	    desthi[i] = HI_NIBBLE(src[0]);
	    destlo[i] = LO_NIBBLE(src[0]);
	}
	return 1;
    } else {
	int i, x = 0;
	BYTE b;

	for (i = 0;
	     desthi[i] == HI_NIBBLE(src[0])
	     && destlo[i] == LO_NIBBLE(src[0]) && i < length;
	     i++, src += srcstep)
	    /* do nothing */ ;

	if (i < length) {
	    if (*xs > i)
		*xs = i;
	    for (; i < length; i++, src += srcstep)
		if (desthi[i] != (b = HI_NIBBLE(src[0]))) {
		    desthi[i] = b;
		    x = i;
		} else if (destlo[i] != (b = LO_NIBBLE(src[0]))) {
		    destlo[i] = b;
		    x = i;
		}
	    if (*xe < x)
		*xe = x;
	    return 1;
	} else
	    return 0;
    }
}


/* This function is used for text modes.  It checks for differences in the
   character memory too.  */
inline static int _fill_cache_text(BYTE * dest, BYTE * src, BYTE * charmem,
				   int length, int l, int *xs, int *xe,
				   int no_check)
{
    if (no_check) {
	int i;

	*xs = 0;
	*xe = length - 1;
	for (i = 0; i < length; i++, src++)
	    dest[i] = GET_CHAR_DATA(charmem, src[0], l);
	return 1;
    } else {
	BYTE b;
	int i;

	for (i = 0;
	     dest[i] == GET_CHAR_DATA(charmem, src[0], l) && i < length;
	     i++, src++)
	    /* do nothing */ ;

	if (i < length) {
	    *xs = *xe = i;
	    for (; i < length; i++, src++)
		if (dest[i] != (b = GET_CHAR_DATA(charmem, src[0], l))) {
		    dest[i] = b;
		    *xe = i;
		}
	    return 1;
	} else
	    return 0;
    }
}


#if SCREEN_NUM_SPRITES > 0
/* Fill the sprite cache with the new sprite data. [*xs; *xe] is the changed
   interval (in pixels).  */
inline static int _fill_sprite_cache(struct line_cache *ll, int *xs, int *xe)
{
    int rr = 0, i, r, sxe, sxs, sxe1, sxs1, n = 0, msk;
    struct sprite spr;
    struct sprite_line_cache *l = ll->sprites;
    struct sprite_line_cache *sprl;
    int xs_return = SCREEN_WIDTH, xe_return = 0;

    ll->numsprites = SCREEN_NUM_SPRITES;
    ll->sprmask = 0;
    for (msk = 1, i = 0; i < SCREEN_NUM_SPRITES; i++, msk <<= 1) {
	spr = sprites[i];
	sprl = l + i;
	r = 0;
	if (dma_msk & msk) {
	    DWORD data = sprite_data[i];

	    ll->sprmask |= msk;
	    sxe = spr.x + (spr.x_expanded ? 48 : 24);
	    sxs = spr.x;
	    if (spr.x != sprl->x) {
		if (sprl->visible) {
		    sxe1 = sprl->x + (sprl->x_expanded ? 48 : 24);
		    sxs1 = sprl->x;
		    n++;
		    if (sxs1 < sxs)
			sxs = sxs1;
		    if (sxe1 > sxe)
			sxe = sxe1;
		}
		sprl->x = spr.x;
		r = 1;
	    }
	    if (!sprl->visible) {
		sprl->visible = 1;
		r = 1;
	    }
	    if (spr.x_expanded != sprl->x_expanded) {
		sprl->x_expanded = spr.x_expanded;
		r = 1;
	    }
	    if (spr.multicolor != sprl->multicolor) {
		sprl->multicolor = spr.multicolor;
		r = 1;
	    }
	    if (mc_sprite_color_1 != sprl->c1) {
		sprl->c1 = mc_sprite_color_1;
		r = 1;
	    }
	    if (mc_sprite_color_2 != sprl->c2) {
		sprl->c2 = mc_sprite_color_2;
		r = 1;
	    }
	    if (spr.color != sprl->c3) {
		sprl->c3 = spr.color;
		r = 1;
	    }
	    if (spr.in_background != sprl->in_background) {
		sprl->in_background = spr.in_background;
		r = 1;
	    }

	    if (sprl->data != data) {
		sprl->data = data;
		r = 1;
	    }

	    if (r) {
		xs_return = MIN(xs_return, sxs);
		xe_return = MAX(xe_return, sxe);
		rr = 1;
	    }
	} else if (sprl->visible) {
	    sprl->visible = 0;
	    sxe = sprl->x + (sprl->x_expanded ? 24 : 48);
	    xs_return = MIN(xs_return, sprl->x);
	    xe_return = MAX(xe_return, sxe);
	    rr = 1;
	}
    }

    if (xe_return >= SCREEN_WIDTH)
	*xe = SCREEN_WIDTH - 1;
    else
	*xe = xe_return;
    *xs = xs_return;

    return rr;
}
#endif				/* SCREEN_NUM_SPRITES > 0 */

/* ------------------------------------------------------------------------- */

/* Increase the size of the rectangle to be refreshed so that it also includes
   the interval [xs; xe] of line y.  The lines must be added in order: from
   top to bottom.  */
inline static void add_line(int y, int xs, int xe)
{
#ifdef RASTER_DEBUG_PUTIMAGE_CALLS
    printf("add_line(): y = %d, xs = %d, xe = %d\n", y, xs, xe);
#endif

    if (changed_area.is_null) {
	changed_area.ys = changed_area.ye = y;
	changed_area.xs = xs;
	changed_area.xe = xe;
	changed_area.is_null = 0;
    } else {
	changed_area.xs = MIN(xs, changed_area.xs);
	changed_area.xe = MAX(xe, changed_area.xe);
	changed_area.ye = y;
    }

#ifdef RASTER_DEBUG_PUTIMAGE_CALLS
    printf("add_line(): current changed_area has "
	   "ys = %d, ye = %d, xs = %d, xe = %d\n",
     changed_area.ys, changed_area.ye, changed_area.xs, changed_area.xe);
#endif
}

/* Refresh the changed rectangle.  */
inline static void refresh_changed(void)
{
    int x, y, xx, yy;
    int w, h;

    if (changed_area.is_null)
	return;

    x = changed_area.xs;
    y = changed_area.ys;
    xx = changed_area.xs - window_first_x;
    yy = changed_area.ys - window_first_line;
    w = changed_area.xe - changed_area.xs + 1;
    h = changed_area.ye - changed_area.ys + 1;

    if (xx < 0) {
	x -= xx;
	w += xx;
	xx = 0;
    }
    if (yy < 0) {
	y -= yy;
	h += yy;
	yy = 0;
    }

    x *= pixel_width;
    xx *= pixel_width;
    w *= pixel_width;

    y *= pixel_height;
    yy *= pixel_height;
    h *= pixel_height;

#if defined (RASTER_DEBUG_PUTIMAGE_CALLS)
    printf("Refresh %d %d %d %d %d %d \n", x, y, xx, yy, w, h);
#endif

    x += 2 * SCREEN_MAX_SPRITE_WIDTH;

    canvas_refresh(canvas, frame_buffer, x, y,
		   xx + window_x_offset, yy + window_y_offset,
		   w, h);

    changed_area.is_null = 1;
}

/* Unconditionally refresh the whole screen.  */
inline static void refresh_all(void)
{
#if defined (RASTER_DEBUG_PUTIMAGE_CALLS)
    printf("Global refresh %d %d %d %d %d %d\n",
	   window_first_x * pixel_width + 2 * SCREEN_MAX_SPRITE_WIDTH,
	   window_first_line * pixel_height,
	   window_x_offset, window_y_offset,
	   MIN(window_width, SCREEN_WIDTH * pixel_width),
	   MIN(window_height, SCREEN_HEIGHT * pixel_height));
#endif
    canvas_refresh(canvas, frame_buffer,
		   window_first_x * pixel_width + 2 * SCREEN_MAX_SPRITE_WIDTH,
		   window_first_line * pixel_height,
		   window_x_offset, window_y_offset,
		   MIN(window_width, SCREEN_WIDTH * pixel_width),
		   MIN(window_height, SCREEN_HEIGHT * pixel_height));
}

/* ------------------------------------------------------------------------- */

inline static void handle_blank_line(void)
{
    /* Changes... Should/could be handled better.  */
    if (have_changes_on_this_line) {
        int i, xs;

    	apply_all_changes(&background_changes);
    	apply_all_changes(&foreground_changes);
    	for (xs = i = 0; i < border_changes.count; i++) {
    	    int xe = border_changes.actions[i].where;

    	    if (xs < xe) {
    	        DRAW_BLANK(frame_buffer_ptr, xs, xe, pixel_width);
    	        xs = xe;
    	    }
    	    apply_change(&border_changes, i);
    	}
    	if (xs <= SCREEN_WIDTH - 1)
    	    DRAW_BLANK(frame_buffer_ptr, xs, SCREEN_WIDTH - 1, pixel_width);
    	border_changes.count = 0;
    	cache[rasterline].border_color = -1;
	cache[rasterline].blank = 1;
    	have_changes_on_this_line = 0;
	add_line(rasterline, 0, SCREEN_WIDTH - 1);
	if (pixel_height == 2 && double_scan_enabled)
	    vid_memcpy((FRAME_BUFFER_LINE_START(frame_buffer, 2*rasterline + 1)
			+ 2 * SCREEN_MAX_SPRITE_WIDTH),
	               frame_buffer_ptr, pixel_width * SCREEN_WIDTH);
    } else if (CANVAS_USES_TRIPLE_BUFFERING(canvas)
               || dont_cache
	       || cache[rasterline].is_dirty
	       || border_color != cache[rasterline].border_color
	       || !cache[rasterline].blank) {

        /* Even when the actual caching is disabled, redraw blank lines only if
           it is really necessary to do so.  */
	cache[rasterline].border_color = border_color;
	cache[rasterline].blank = 1;
	cache[rasterline].is_dirty = 0;

	DRAW_BLANK(frame_buffer_ptr, 0, SCREEN_WIDTH - 1, pixel_width);
	if (pixel_height == 2 && double_scan_enabled)
	    DRAW_BLANK((FRAME_BUFFER_LINE_START(frame_buffer, 2*rasterline + 1)
			+ 2 * SCREEN_MAX_SPRITE_WIDTH),
		       0, SCREEN_WIDTH - 1, pixel_width);
	add_line(rasterline, 0, SCREEN_WIDTH - 1);
    }

#ifdef __VIC_II__
    update_sprite_collisions();
#endif
}

/* Draw the borders.  */
inline static void draw_borders(void)
{
    if (!open_left_border)
	DRAW_BLANK(frame_buffer_ptr, 0, display_xstart - 1, pixel_width);
    if (!open_right_border)
	DRAW_BLANK(frame_buffer_ptr, display_xstop, SCREEN_WIDTH - 1,
		   pixel_width);
}

inline static void handle_visible_line_with_cache(void)
{
    int needs_update = 0;
    int line;
    int vm = draw_idle_state ? SCREEN_IDLE_MODE : video_mode;
    int changed_start = SCREEN_TEXTCOLS, changed_end = -1;
    struct line_cache *l = &cache[rasterline];

    line = rasterline - SCREEN_BORDERHEIGHT - ysmooth - 1;

    /* Check for "major" changes first.  If there is any, just write straight
       to the cache without any comparisons and redraw the whole line.  */
    if (l->is_dirty
	|| dont_cache
	|| l->n != line
	|| l->xsmooth != xsmooth
	|| l->video_mode != vm
	|| l->blank
	|| l->ycounter != ycounter
	|| l->border_color != border_color
	|| l->display_xstart != display_xstart
	|| l->display_xstop != display_xstop
	|| (l->open_right_border && !open_right_border)
        || (l->open_left_border && !open_left_border)) {

	needs_update = 1;
	l->n = line;
	l->video_mode = vm;
	l->blank = 0;
	l->xsmooth = xsmooth;
	l->ycounter = ycounter;
	l->border_color = border_color;
	l->display_xstart = display_xstart;
	l->display_xstop = display_xstop;
	l->open_right_border = open_right_border;
	l->open_left_border = open_left_border;

#if defined(__VIC_II__)
	l->overscan_background_color = overscan_background_color;
#endif /* defined(__VIC_II__) */

	/* Fill the space between the border and the graphics with the
	   background color (necessary if `xsmooth' is != 0).  */
	vid_memset(frame_buffer_ptr + SCREEN_BORDERWIDTH * pixel_width,
		   PIXEL(overscan_background_color), xsmooth * pixel_width);

	if (open_left_border)
	    vid_memset(frame_buffer_ptr, PIXEL(overscan_background_color),
		       (SCREEN_BORDERWIDTH + xsmooth) * pixel_width);
	if (open_right_border)
	    vid_memset((frame_buffer_ptr +
			((SCREEN_BORDERWIDTH + SCREEN_XPIX + xsmooth)
			 * pixel_width)), PIXEL(overscan_background_color),
		       (SCREEN_WIDTH - SCREEN_BORDERWIDTH - SCREEN_XPIX
			- xsmooth) * pixel_width);

#if SCREEN_NUM_SPRITES > 0
	_fill_sprite_cache(l, &changed_start, &changed_end);
#endif

	video_modes[vm].fill_cache(l, &changed_start, &changed_end, 1);

	/* [ `changed_start' ; `changed_end' ] now covers the whole line, as
	   we have called fill_cache() with `1' as the last parameter (no
	   check).  */
	video_modes[vm].draw_line_cached(l, changed_start, changed_end);

	changed_start = 0;
	changed_end = SCREEN_WIDTH - 1;

	draw_borders();

    } else {

	/* There are no `major' changes: try to do some optimization.  */

#if SCREEN_NUM_SPRITES > 0

	int sprites_need_update;
	int sprite_changed_start, sprite_changed_end;

	sprites_need_update = _fill_sprite_cache(l, &sprite_changed_start,
						 &sprite_changed_end);
	/* If sprites have changed, do not bother trying to reduce the amount
	   of recalculated data, but simply redraw everything.  */
	needs_update = video_modes[vm].fill_cache(l, &changed_start,
						  &changed_end,
						  sprites_need_update);

#if defined(__VIC_II__)
	/* If the background color changes, we might get the wrong color in
           the left part of the screen, between the border and the start of
           the graphics.  */
	if (ysmooth > 0
	    && l->overscan_background_color != overscan_background_color)
	    needs_update = 1;
#endif /* defined(__VIC_II__) */

	if (needs_update) {
	    video_modes[vm].draw_line_cached(l, changed_start, changed_end);

	    /* Fill the space between the border and the graphics with the
	       background color (necessary if xsmooth is > 0).  */
	    vid_memset(frame_buffer_ptr + SCREEN_BORDERWIDTH * pixel_width,
		       PIXEL(overscan_background_color), xsmooth * pixel_width);

	    /* If xsmooth > 0, drawing the graphics might have corrupted
	       part of the border... fix it here.  */
	    if (!open_right_border)
		DRAW_BLANK(frame_buffer_ptr, SCREEN_BORDERWIDTH + SCREEN_XPIX,
			   SCREEN_BORDERWIDTH + SCREEN_XPIX + 8, pixel_width);

	    /* Calculate the interval in pixel coordinates.  */

#if defined(__VIC_II__)
	    if (l->overscan_background_color != overscan_background_color) {
		if (ysmooth > 0)
		    changed_start = SCREEN_BORDERWIDTH;
		else
		    changed_start = (SCREEN_BORDERWIDTH + xsmooth
				     + 8 * changed_start);
		l->overscan_background_color = overscan_background_color;
	    } else {
		changed_start = (SCREEN_BORDERWIDTH + xsmooth
				 + 8 * changed_start);
	    }
#else  /* defined(__VIC_II__) */
	    changed_start = SCREEN_BORDERWIDTH + xsmooth + 8 * changed_start;
#endif /* defined(__VIC_II__) */

	    changed_end = (SCREEN_BORDERWIDTH + xsmooth + 8 * (changed_end + 1)
			   - 1);

	    if (sprites_need_update) {
		/* FIXME: wrong.  */
		if (open_left_border)
		    changed_start = 0;
		if (open_right_border)
		    changed_end = SCREEN_WIDTH - 1;

		/* Even if we have recalculated the whole line, we will refresh
		   only the part that has actually changed when writing to the
		   window.  */
		changed_start = MIN(changed_start, sprite_changed_start);
		changed_end = MAX(changed_end, sprite_changed_end);

		/* The borders have not changed, so do not repaint them even
		   if there are sprites under them.  */
		changed_start = MAX(changed_start, display_xstart);
		changed_end = MIN(changed_end, display_xstop);
	    }
	}
#if SCREEN_NUM_SPRITES > 0
        else {                /* if (needs_update) */
            cl_ss_collmask = l->ss_collmask;
            cl_sb_collmask = l->sb_collmask;
        }
#endif

#else  /* no sprites */

	needs_update = video_modes[vm].fill_cache(l, &changed_start,
						  &changed_end, 0);

	if (needs_update) {
	    video_modes[vm].draw_line_cached(l, changed_start, changed_end);

	    /* Convert from character to pixel coordinates.  */
	    changed_start = SCREEN_BORDERWIDTH + xsmooth + 8 * changed_start;
	    changed_end = (SCREEN_BORDERWIDTH + xsmooth + 8 * (changed_end + 1)
			   - 1);
	}
#endif				/* no sprites */

	draw_borders();
    }

    if (needs_update) {
	add_line(rasterline, changed_start, changed_end);

	if (pixel_height == 2 && double_scan_enabled) {
	    vid_memcpy((FRAME_BUFFER_LINE_START(frame_buffer, 2*rasterline + 1)
		        + 2*SCREEN_MAX_SPRITE_WIDTH
			+ changed_start * pixel_width),
		       frame_buffer_ptr + changed_start * pixel_width,
		       (changed_end - changed_start + 1) * pixel_width);
	}
    }

    l->is_dirty = 0;
}

inline static void handle_visible_line_without_cache()
{
    /* If screen is scrolled to the right, we need to fill with the background
       color the blank part on the left.  */
    vid_memset(frame_buffer_ptr + SCREEN_BORDERWIDTH * pixel_width,
	       PIXEL(overscan_background_color), xsmooth * pixel_width);

    if (open_left_border)
	vid_memset(frame_buffer_ptr, PIXEL(overscan_background_color),
		   (SCREEN_BORDERWIDTH + xsmooth) * pixel_width);
    if (open_right_border)
	vid_memset((frame_buffer_ptr +
		    ((SCREEN_BORDERWIDTH + SCREEN_XPIX + xsmooth)
		     * pixel_width)), PIXEL(overscan_background_color),
		    (SCREEN_WIDTH - SCREEN_BORDERWIDTH - SCREEN_XPIX
		     - xsmooth) * pixel_width);

    /* Draw the graphics and sprites.  */
    if (draw_idle_state)
	video_modes[SCREEN_IDLE_MODE].draw_line();
    else
	video_modes[video_mode].draw_line();

    draw_borders();

    if (CANVAS_USES_TRIPLE_BUFFERING(canvas)
        || dont_cache
	|| dma_msk
	|| cache[rasterline].is_dirty
	|| cache[rasterline].blank
	|| cache[rasterline].border_color != border_color
	|| (cache[rasterline].open_right_border != open_right_border)
	|| (cache[rasterline].open_left_border != open_left_border)
#if defined(__VIC_II__)
	|| (cache[rasterline].overscan_background_color
	    != overscan_background_color)
#endif
	) {
	cache[rasterline].blank = 0;
	cache[rasterline].is_dirty = 0;
	cache[rasterline].border_color = border_color;
	cache[rasterline].open_right_border = open_right_border;
	cache[rasterline].open_left_border = open_left_border;
#if defined(__VIC_II__)
	cache[rasterline].overscan_background_color = overscan_background_color;
#endif
	add_line(rasterline, 0, SCREEN_WIDTH - 1);

    } else {

        /* Still do some minimal caching anyway.  */
	/* Only update the part between the borders.  */
	add_line(rasterline,
		 SCREEN_BORDERWIDTH,
		 SCREEN_BORDERWIDTH + SCREEN_XPIX - 1);

    }

    if (pixel_height == 2 && double_scan_enabled)
	vid_memcpy((FRAME_BUFFER_LINE_START(frame_buffer, rasterline*2 + 1)
		    + 2 * SCREEN_MAX_SPRITE_WIDTH),
		   frame_buffer_ptr,
		   SCREEN_WIDTH * pixel_width);
}

inline static void handle_visible_line_with_changes(void)
{
    int xs, xstop, i;

    for (xs = i = 0; i < background_changes.count; i++) {
	int xe = background_changes.actions[i].where;

        if (xs < xe) {
            int vm = draw_idle_state ? SCREEN_IDLE_MODE : video_mode;

	    video_modes[vm].draw_background(xs, xe - 1);
  	    xs = xe;
        }
	apply_change(&background_changes, i);
    }
    if (xs <= SCREEN_WIDTH - 1) {
	int vm = draw_idle_state ? SCREEN_IDLE_MODE : video_mode;

        video_modes[vm].draw_background(xs, SCREEN_WIDTH - 1);
    }

    for (xs = i = 0; i < foreground_changes.count; i++) {
	int xe = foreground_changes.actions[i].where;

        if (xs < xe) {
	    int vm = draw_idle_state ? SCREEN_IDLE_MODE : video_mode;

	    video_modes[vm].draw_foreground(xs, xe - 1);
	    xs = xe;
	}
	apply_change(&foreground_changes, i);
    }
    if (xs <= SCREEN_TEXTCOLS - 1) {
	int vm = draw_idle_state ? SCREEN_IDLE_MODE : video_mode;

        video_modes[vm].draw_foreground(xs, SCREEN_TEXTCOLS - 1);
    }

#if SCREEN_NUM_SPRITES > 0
    draw_sprites();
#endif

    /* Draw left border.  */
    xstop = display_xstart - 1;
    if (!open_left_border) {
	for (xs = i = 0;
	     (i < border_changes.count
	      && border_changes.actions[i].where <= xstop);
	     i++) {
	    int xe = border_changes.actions[i].where;

	    if (xs < xe) {
	        DRAW_BLANK(frame_buffer_ptr, xs, xe - 1, pixel_width);
	        xs = xe;
	    }
	    apply_change(&border_changes, i);
	}
	if (xs <= xstop)
	    DRAW_BLANK(frame_buffer_ptr, xs, xstop, pixel_width);
    } else {
	for (i = 0;
	     (i < border_changes.count
	      && border_changes.actions[i].where <= xstop);
	     i++)
	    apply_change(&border_changes, i);
    }

    /* Draw right border.  */
    if (!open_right_border) {
	for (;
	     (i < border_changes.count
	      && border_changes.actions[i].where <= display_xstop);
	     i++)
	    apply_change(&border_changes, i);
	for (xs = display_xstop; i < border_changes.count; i++) {
	    int xe = border_changes.actions[i].where;

            if (xs < xe) {
	        DRAW_BLANK(frame_buffer_ptr, xs, xe - 1, pixel_width);
	        xs = xe;
	    }
	    apply_change(&border_changes, i);
	}
	if (xs <= SCREEN_WIDTH - 1)
	    DRAW_BLANK(frame_buffer_ptr, xs, SCREEN_WIDTH - 1, pixel_width);
    } else {
	for (i = 0; i < border_changes.count; i++)
	    apply_change(&border_changes, i);
    }

    foreground_changes.count =  0;
    background_changes.count =  0;
    border_changes.count = 0;

    /* Do not cache this line.  */
    cache[rasterline].is_dirty = 1;

    add_line(rasterline, 0, SCREEN_WIDTH - 1);
    if (pixel_height == 2 && double_scan_enabled)
        vid_memcpy(FRAME_BUFFER_LINE_START(frame_buffer, 2*rasterline + 1)
		   + 2 * SCREEN_MAX_SPRITE_WIDTH,
                   frame_buffer_ptr, pixel_width * SCREEN_WIDTH);

    have_changes_on_this_line = 0;
}

inline static void handle_visible_line(void)
{
    if (have_changes_on_this_line)
	handle_visible_line_with_changes();
    else if (!CANVAS_USES_TRIPLE_BUFFERING(canvas)
             && video_cache_enabled
	     && !open_left_border
	     && !open_right_border) /* FIXME: shortcut! */
	handle_visible_line_with_cache();
    else
	handle_visible_line_without_cache();

#if !defined (__VIC_II__) && !defined (__VDC__)
    if (++ycounter >= SCREEN_CHARHEIGHT) {
	ycounter = 0;
	memptr += memptr_inc;
#ifdef __CRTC__
	memptr &= addr_mask;
#endif
    }
#endif
}

inline static void handle_end_of_frame(void)
{
    frame_buffer_ptr = (FRAME_BUFFER_START(frame_buffer)
			+ 2 * SCREEN_MAX_SPRITE_WIDTH);
    mem_counter = memptr = rasterline = 0;

#ifndef __VIC_II__
    ycounter = 0;
#endif
#ifdef __CRTC__
    memptr = scraddr & addr_mask;
#endif

    if (!skip_next_frame) {
	if (dont_cache)
	    refresh_all();
	else
	    refresh_changed();
    }
    skip_next_frame = do_vsync(skip_next_frame);

#if defined(__MSDOS__) && !defined(VIC20)
    if (window_width == SCREEN_XPIX && window_height == SCREEN_YPIX)
	canvas_set_border_color(canvas, border_color);
#endif
}

/* Emulate one raster line.  */
inline static void emulate_line(void)
{
    oldclk += CYCLES_PER_LINE;

    /* Emulate the vertical blank flip-flops.  (Well, sort of.)  */
    if (rasterline == display_ystart && !blank)
	blank_enabled = 0;
    else if (rasterline == display_ystop)
	blank_enabled = 1;

    if (rasterline >= SCREEN_FIRST_DISPLAYED_LINE
	&& rasterline <= SCREEN_LAST_DISPLAYED_LINE) {
	if (!skip_next_frame && !asleep
            && (rasterline >= window_first_line
                && rasterline <= window_last_line)) {
	    if ((blank_this_line || blank_enabled) && !open_left_border)
		handle_blank_line();
	    else
		handle_visible_line();
	    if (++num_cached_lines == (window_last_line - window_first_line)) {
		dont_cache = 0;
		num_cached_lines = 0;
	    }
	} else {
#ifdef __VIC_II__
	    if (!skip_next_frame)
		update_sprite_collisions();
#endif
	    if (have_changes_on_this_line) {
	        apply_all_changes(&background_changes);
	        apply_all_changes(&foreground_changes);
	        apply_all_changes(&border_changes);
	        have_changes_on_this_line = 0;
	    }
	}

#ifdef __VIC_II__
	if (!idle_state)
	    mem_counter = (mem_counter + mem_counter_inc) & 0x3ff;
	mem_counter_inc = SCREEN_TEXTCOLS;
	/* `ycounter' makes the chip go to idle state when it reaches the
	   maximum value.  */
	if (ycounter == 7) {
	    idle_state = 1;
	    memptr = mem_counter;
	}
	if (!idle_state || bad_line) {
	    ycounter = (ycounter + 1) & 0x7;
	    idle_state = 0;
	}
	if (force_display_state) {
	    idle_state = 0;
	    force_display_state = 0;
	}
	draw_idle_state = idle_state;
	bad_line = 0;
#endif

	rasterline++;
	frame_buffer_ptr = FRAME_BUFFER_LINE_START(frame_buffer,
						   rasterline * pixel_height);
	frame_buffer_ptr += 2 * SCREEN_MAX_SPRITE_WIDTH;

#ifdef __VIC_II__
 	if (rasterline == 0x30)
	    allow_bad_lines = !blank;
#endif

    } else {

#ifdef __VIC_II__
	if (!skip_next_frame)
	    update_sprite_collisions();
#endif

        if (have_changes_on_this_line) {
            apply_all_changes(&background_changes);
            apply_all_changes(&foreground_changes);
            apply_all_changes(&border_changes);
            have_changes_on_this_line = 0;
        }
	rasterline++;
	if (rasterline == SCREEN_HEIGHT) {
	    handle_end_of_frame();
	} else {
	    frame_buffer_ptr = FRAME_BUFFER_LINE_START(frame_buffer,
						    rasterline * pixel_height);
	    frame_buffer_ptr += 2 * SCREEN_MAX_SPRITE_WIDTH;
	}

    }

    apply_all_changes(&next_line_changes);

#ifdef __VIC_II__
    /* Handle open borders.  */
    open_left_border = open_right_border;
    open_right_border = 0;
#endif

#if SCREEN_NUM_SPRITES > 0
    dma_msk = new_dma_msk;
#endif

    memory_fetch_done = 0;

    blank_this_line = 0;
}

/* Disable all the caching for the next frame.  */
inline static void force_repaint(void)
{
    dont_cache = 1;
    num_cached_lines = 0;
}
