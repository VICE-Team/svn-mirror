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


#define	crtc_max(a,b)	(((a)>(b))?(a):(b))
#define	crtc_min(a,b)	(((a)<(b))?(a):(b))

#define	MAX_PIXEL_WIDTH		2
#define	MAX_PIXEL_HEIGHT	2

#ifdef __MSDOS__

#define	DOUBLE_SIZE_ENABLED()	(double_size_enabled || (screen_xpix > 320))

#else /* __MSDOS__ */

#ifdef USE_VIDMODE_EXTENSION

#define	DOUBLE_SIZE_ENABLED() \
(fullscreen?fullscreen_double_size_enabled:double_size_enabled)

#else

#define	DOUBLE_SIZE_ENABLED()	(double_size_enabled)

#endif

#endif /* __MSDOS__ */

/* ------------------------------------------------------------------------- */

static void crtc_update_timing(int change);
static void crtc_init_dwg_tables(void);
static void crtc_update_memory_ptrs(void);
static void crtc_arrange_window(int width, int height);
static int fill_cache(struct line_cache *l, int *xs, int *xe, int r);
static void draw_standard_line(void);
static void draw_reverse_line(void);
static void draw_standard_line_2x(void);
static void draw_reverse_line_2x(void);
static void draw_standard_line_cached(struct line_cache *l, int xs, int xe);
static void draw_reverse_line_cached(struct line_cache *l, int xs, int xe);
static void draw_standard_line_cached_2x(struct line_cache *l, int xs, int xe);
static void draw_reverse_line_cached_2x(struct line_cache *l, int xs, int xe);

/* Define the position of the raster beam precisely.  */

static palette_t *palette;
static BYTE crtc[19];
static BYTE *chargen_ptr = NULL;
static int chargen_rel = 0;
static BYTE *screenmem = NULL;
static ADDRESS addr_mask = 0;
static ADDRESS scraddr = 0;
static int hw_double_cols = 0;

static int crsr_enable;
static int crsrpos;
static int scrpos;
static int crsrrel;
static int crsrmode;    /* 0=blank, 1=continously, 2=1/32, 3=1/16 frame rate */
static int crsrstart;   /* 1st cursor scan line */
static int crsrend;     /* last cursor scan line */
static int crsrstate;   /* 0 = off, 1 = on, toggled by int_* */
static int crsrcnt;

/* those values describe the current screen and are set in 
   crtc_update_timing(). */
static int screen_charheight 		= 8;
static int crtc_screen_textlines	= 25;
static int crtc_cycles_per_line		= 100;
static int crtc_vertical_total 		= 25;
static int crtc_vertical_adjust 	= 0;

/* these values are derived from the above and must never be changed 
   directly but only over crtc_update_timing(). */
static int screen_xpix;
static int screen_ypix;
static int screen_rasterlines;

/* those values are the new values for the variables without "new_" and are 
   evaluated whenever crtc_update_timing() is called. */
static int new_screen_charheight 	= 8;
static int new_crtc_screen_textlines	= 25;
static int new_crtc_cycles_per_line	= 100;
static int new_crtc_vertical_total 	= 25;
static int new_crtc_vertical_adjust 	= 0;
static int new_memptr_inc		= 80;

static CLOCK rasterline_start_clk = 0;

/* CRTC alarms.  */
static alarm_t raster_draw_alarm;

void init_drawing_tables(void) {
    crtc_init_dwg_tables();
}

/* ------------------------------------------------------------------------- */

#include "raster.c"

/* -------------------------------------------------------------------------- */

void video_free(void)
{
    frame_buffer_free(&frame_buffer);
}

static void crtc_arrange_window(int width, int height)
{
#ifdef USE_VIDMODE_EXTENSION
    if(fullscreen) {
      frame_buffer_clear(&frame_buffer, PIXEL(0));
      return;
    }
#endif
    resize(width, height);
    refresh_changed();
    refresh_all();
}

static void crtc_update_timing(int change) 
{
    int new_window_height = window_height;
    int new_window_width = window_width;

    if (canvas)
        refresh_changed();

    if (new_crtc_cycles_per_line != crtc_cycles_per_line) {
	crtc_cycles_per_line = new_crtc_cycles_per_line;
	log_message(crtc_log, "set cycles per line to %d.",
                    crtc_cycles_per_line);
	change = 1;
    }

    if (new_memptr_inc != memptr_inc) {
	if (DOUBLE_SIZE_ENABLED()) {
	    if ((!IS_DOUBLE_WIDTH_ALLOWED(memptr_inc))
		&& IS_DOUBLE_WIDTH_ALLOWED(new_memptr_inc) ) {
		/* make window smaller */
                pixel_width = 2;
                video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
                video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached_2x;
                video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line_2x;
                video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
                video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached_2x;
                video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line_2x;
	    } else
	    if (IS_DOUBLE_WIDTH_ALLOWED(memptr_inc)
		&& (!IS_DOUBLE_WIDTH_ALLOWED(new_memptr_inc)) ) {
		/* make window wider */
                pixel_width = 1;
                video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
                video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
                video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
                video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
                video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
                video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
	  }
	}
        memptr_inc = new_memptr_inc;


	change = 1;
    }

    if ( new_crtc_screen_textlines != crtc_screen_textlines
	|| new_screen_charheight != screen_charheight 
	|| new_crtc_vertical_total != crtc_vertical_total
	|| new_crtc_vertical_adjust != crtc_vertical_adjust
	) {

	if (DOUBLE_SIZE_ENABLED()) {
	    if (IS_DOUBLE_HEIGHT_ALLOWED(new_crtc_screen_textlines 
				* new_screen_charheight)
		&& !IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines
				* screen_charheight)) {
		pixel_height = 2;
	    } else
	    if (!IS_DOUBLE_HEIGHT_ALLOWED(new_crtc_screen_textlines 
				* new_screen_charheight)
		&& IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines
				* screen_charheight)) {
		pixel_height = 1;
	    }
	}
	
	while (new_crtc_screen_textlines * new_screen_charheight * pixel_height
		+ 2 * SCREEN_BORDERHEIGHT >= FRAMEB_HEIGHT)
	    new_screen_charheight--;

        crtc_screen_textlines = new_crtc_screen_textlines;
        screen_charheight = new_screen_charheight;
	crtc_vertical_total = new_crtc_vertical_total;
	crtc_vertical_adjust = new_crtc_vertical_adjust;

	change = 1;
    }

    if ( change ) {
	/* now compute the new screen/window sizes */
	screen_xpix = memptr_inc * 8;
	screen_ypix = crtc_screen_textlines * screen_charheight;

	new_window_width = pixel_width * 
			(screen_xpix + 2 * SCREEN_BORDERWIDTH);
	new_window_height = pixel_height * 
			(screen_ypix + 2 * SCREEN_BORDERWIDTH);

	screen_rasterlines = crtc_vertical_total * screen_charheight 
			+ crtc_vertical_adjust;

	machine_set_cycles_per_frame(screen_rasterlines * crtc_cycles_per_line);

	/* from screen height */
	if (rasterline >= SCREEN_LAST_RASTERLINE) {
	    handle_end_of_frame();
	}

        if (canvas) {
            display_xstart = SCREEN_BORDERWIDTH;
            display_xstop = SCREEN_BORDERWIDTH + screen_xpix;

            display_ystart = SCREEN_BORDERHEIGHT;
            display_ystop = SCREEN_BORDERHEIGHT + screen_ypix;

            canvas_resize(canvas, new_window_width, new_window_height);
            crtc_arrange_window(new_window_width, new_window_height);
            video_resize();
        }
    }
}

/* -------------------------------------------------------------------------- */

static void crsr_set_dirty(void)
{
    int i, j;

    /* cursor enabled */
    if (!crsr_enable) return;

    /* cursor out of screen */
    if (crsrrel<0 || (crsrrel >= (memptr_inc * crtc_screen_textlines)) )
        return;

    i = (crsrrel / memptr_inc) * screen_charheight + crsrstart;
    i += SCREEN_BORDERHEIGHT + ysmooth;

    j = (crsrrel / memptr_inc) * screen_charheight + crsrend;
    j += SCREEN_BORDERHEIGHT + ysmooth;

    while (i<=j) {
	if (i < SCREEN_HEIGHT)
	  cache[i].is_dirty = 1;
	i++;
    }
}


void reset_crtc(void)
{
    /* spec says to initialize "all internal scan counter circuits.
     * When /RES is low, all internal counters stop and clear and all
     * scan and video output go low; control registers are unaffected.
     * All scan timing initiates when /RES goes high. "
     * "In this way, /RES can synchronize display frame timing with
     * line frequency."
     *
     * Well, we just emulate...
     */

    if (crsrmode) crsr_set_dirty();

    crsrpos = 0;
    scrpos = 0;
    crsrrel = 0;
    crsrmode = 0;
    crsrstart = 0;
    crsrend = 0;
    crsrstate = 0;
    crsrcnt = 0;

    alarm_set(&raster_draw_alarm, clk + CYCLES_PER_LINE);
    return;
}

int crtc_offscreen(void)
{
    /* This test is pretty bogus. But if we keep it this way performance
       on an old PET is horrible, as we do not emulate the full border, only
       a small part. And at the moment we are not cycle exact anyway */
    return rasterline >= (SCREEN_YPIX / 2);
}

void crtc_set_screen_mode(BYTE *screen, int vmask, int num_cols, int hwflags)
{
    addr_mask = vmask;

    if (screen) {
        screenmem = screen;
    }
    crsr_enable = hwflags & 1;
    hw_double_cols = hwflags & 2;

    if (!num_cols) {
        new_memptr_inc=1;
    } else {
        new_memptr_inc = num_cols;
    }
    /* no *2 for hw_double_cols, as the caller should have done it.
       This num_cols flag should be gone sometime.... */
    new_memptr_inc = crtc_min( SCREEN_MAX_TEXTCOLS, new_memptr_inc );

#ifdef __MSDOS__
    /* FIXME: This does not have any effect until there is a gfx -> text ->
       gfx mode transition.  Moreover, no resources should be changed behind
       user's back...  So this is definitely a Bad Thing (tm).  For now, it's
       fine with us, though.  */
    resources_set_value("VGAMode",
                        (resource_value_t) (num_cols > 40
                                            ? VGA_640x480 : VGA_320x200));
#endif

    /* vmask has changed -> */
    crtc_update_memory_ptrs();

    /* force window reset with parameter =1 */
    crtc_update_timing(1);
}

void crtc_screen_enable(int en)
{
    en = en ? 0 : 1;
    blank = en;
    blank_enabled = en;
}

static void crtc_update_memory_ptrs(void)
{
    scraddr = crtc[13] + ((crtc[12] & 0x3f) << 8);

    /* depends on machine */
    do_update_memory_ptrs();

    /* chargen_rel is computed for 8bytes/char, but charom is 16bytes/char */
    chargen_ptr = chargen_rom + (chargen_rel << 1);

    scraddr &= addr_mask;
}

/* -------------------------------------------------------------------------- */


int int_rasterdraw(long offset)
{
    /* update timing/window size once per frame */
    if (rasterline == 0) {
	crtc_update_timing(0);
    }

    alarm_set(&raster_draw_alarm, clk + CYCLES_PER_LINE - offset);

    rasterline_start_clk = clk - offset;

    emulate_line();

    /* This generates one raster interrupt per frame. */
    if (rasterline == 0) {
        /* we assume this to start the screen */
	SIGNAL_VERT_BLANK_OFF
        if (crsrmode & 0x02) {
            if (crsrcnt) crsrcnt--;
            else {
		crsr_set_dirty();
                crsrcnt = (crsrmode & 0x01) ? 16 : 32;
                crsrstate ^= 1;
            }
        }
    } else if (rasterline == SCREEN_YPIX) {
        /* and this to end the screen */
	SIGNAL_VERT_BLANK_ON
    }

    return 0;
}

static int fill_cache(struct line_cache *l, int *xs, int *xe, int r)
{
    static BYTE fake_char_data = 0;
    int retval, n;

    retval = _fill_cache_text(l->fgdata, screenmem + memptr, chargen_ptr,
                              memptr_inc, ycounter, xs, xe, r);

    n = (SCREEN_MAX_TEXTCOLS >= memptr_inc) 
			? SCREEN_MAX_TEXTCOLS - memptr_inc : 0;

    /* All the characters beyond the `memptr_inc'th one are blank.  */
    if (n > 0) {
        int xs1 = SCREEN_TEXTCOLS, xe1 = -1;

        if (_fill_cache(l->fgdata + memptr_inc, &fake_char_data, n, 0,
                        &xs1, &xe1, r)) {
            xs1 += memptr_inc, *xs = crtc_min(xs1, *xs);
            xe1 += memptr_inc, *xe = crtc_max(xe1, *xe);
            retval = 1;
        }
    }

    return retval;
}


#define DRAW(reverse_flag)                                              \
    do {                                                                \
        PIXEL *p = frame_buffer_ptr + SCREEN_BORDERWIDTH;               \
        register int i, d;                                              \
                                                                        \
        if (crsrmode 							\
		&& crsrstate						\
		&& ycounter >= crsrstart				\
		&& ycounter <= crsrend )				\
        for (i = 0; i < memptr_inc; i++, p += 8) {			\
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i], 	\
				ycounter);				\
            if ( (memptr+i)==crsrrel )					\
                d ^= 0xff;						\
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table_0[d];				\
            *((PIXEL4 *) p + 1) = dwg_table_1[d];			\
        }								\
        else								\
									\
        for (i = 0; i < memptr_inc; i++, p += 8) {                      \
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i],     \
                              ycounter);                                \
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table_0[d];                           \
            *((PIXEL4 *) p + 1) = dwg_table_1[d];                       \
        }                                                               \
/*									\
        d = (reverse_flag) ? 0xff: 0;                                   \
        for (; i < SCREEN_MAX_TEXTCOLS; i++, p += 8) {                  \
                *((PIXEL4 *) p) = dwg_table_0[d];                       \
                *((PIXEL4 *) p + 1) = dwg_table_1[d];                   \
        }  */                                                           \
    } while (0)

static void draw_standard_line(void)
{
    DRAW(0);
}

static void draw_reverse_line(void)
{
    DRAW(1);
}


#define DRAW_2x(reverse_flag)                                           \
    do {                                                                \
        PIXEL *p = (frame_buffer_ptr                                    \
                    + SCREEN_BORDERWIDTH * pixel_width);                \
        register int i, d;                                              \
                                                                        \
        if (crsrmode							\
		&& crsrstate						\
		&& ycounter >= crsrstart				\
		&& ycounter <= crsrend )				\
        for (i = 0; i < memptr_inc; i++, p += 16) {			\
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i], 	\
			ycounter);					\
            if ( (memptr+i)==crsrrel )					\
                d ^= 0xff;						\
            if ((reverse_flag))                                         \
                d ^= 0xff;                                              \
            *((PIXEL4 *) p) = dwg_table2x_0[d];				\
            *((PIXEL4 *) p + 1) = dwg_table2x_1[d];			\
            *((PIXEL4 *) p + 2) = dwg_table2x_2[d];			\
            *((PIXEL4 *) p + 3) = dwg_table2x_3[d];			\
        }								\
        else								\
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

static void draw_standard_line_2x(void)
{
    DRAW_2x(0);
}

static void draw_reverse_line_2x(void)
{
    DRAW_2x(1);
}


#define DRAW_CACHED(l, xs, xe, reverse_flag)                            \
    do {                                                                \
        PIXEL *p = frame_buffer_ptr + SCREEN_BORDERWIDTH + (xs) * 8;    \
        register int i;                                                 \
        register int mempos = ((l->n+1)/screen_charheight )*memptr_inc;	\
	register int ypos = (l->n+1) % screen_charheight;		\
									\
        for (i = (xs); i <= (xe); i++, p += 8) {                        \
            BYTE d = (l)->fgdata[i];                                    \
            if ((reverse_flag))                                         \
                d = ~d;                                                 \
                                                                        \
            if (crsrmode 						\
		&& crsrstate						\
                && ypos >= crsrstart 					\
		&& ypos <=crsrend					\
		&& mempos+i==crsrrel 					\
		) {							\
                d = ~d;                                                 \
            } 								\
            *((PIXEL4 *) p) = dwg_table_0[d];				\
            *((PIXEL4 *) p + 1) = dwg_table_1[d];			\
        }                                                               \
    } while (0)

static void draw_standard_line_cached(struct line_cache *l, int xs, int xe)
{
    DRAW_CACHED(l, xs, xe, 0);
}

static void draw_reverse_line_cached(struct line_cache *l, int xs, int xe)
{
    DRAW_CACHED(l, xs, xe, 1);
}

#define DRAW_CACHED_2x(l, xs, xe, reverse_flag)                 \
    do {                                                        \
        PIXEL *p = (frame_buffer_ptr                            \
                    + 2 * (SCREEN_BORDERWIDTH + (xs) * 8));     \
        register int i;                                         \
        register int mempos = ((l->n+1)/screen_charheight )	\
					*memptr_inc;         	\
        register int ypos = (l->n+1) % screen_charheight;       \
                                                                \
        for (i = (xs); i <= (xe); i++, p += 16) {               \
            BYTE d = (l)->fgdata[i];                            \
            if ((reverse_flag))                                 \
                d = ~d;                                         \
								\
            if (crsrmode 					\
		&& crsrstate  					\
                && ypos >= crsrstart 				\
		&& ypos <=crsrend				\
		&& mempos+i==crsrrel 				\
		) { 						\
                d = ~d;                                         \
            }						        \
            *((PIXEL4 *) p) = dwg_table2x_0[d];                 \
            *((PIXEL4 *) p + 1) = dwg_table2x_1[d];             \
            *((PIXEL4 *) p + 2) = dwg_table2x_2[d];             \
            *((PIXEL4 *) p + 3) = dwg_table2x_3[d];             \
	}							\
    } while (0)

static void draw_standard_line_cached_2x(struct line_cache *l, int xs, int xe)
{
    DRAW_CACHED_2x(l, xs, xe, 0);
}

static void draw_reverse_line_cached_2x(struct line_cache *l, int xs, int xe)
{
    DRAW_CACHED_2x(l, xs, xe, 1);
}

/* ------------------------------------------------------------------------- */

void crtc_prevent_clk_overflow(CLOCK sub)
{
    oldclk -= sub;
    rasterline_start_clk -= sub;
}

#ifdef USE_VIDMODE_EXTENSION
void video_setfullscreen(int v,int width, int height) {
    fullscreen = v;
    fullscreen_width = width;
    fullscreen_height = height;

    video_resize();
    if(v) {
        resize(width, height);
	refresh_changed();
	refresh_all();
    }
    video_resize();
}

void fullscreen_forcerepaint() {
    if(fullscreen) {
	video_resize();
        resize(fullscreen_width, fullscreen_height);
	refresh_changed();
	refresh_all();
	video_resize();
    }
}
#endif
