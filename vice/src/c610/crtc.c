
/*
 * ../../../src/c610/crtc.c
 * This file is generated from ../../../src/crtc-tmpl.c and ../../../src/c610/crtc.def,
 * Do not edit!
 */
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

/* FIXME: lots of stuff depends on 8 scanlines/char! */

#define _CRTC_C

#define CRTC_WINDOW_TITLE            "VICE: CBM-II emulator"

#include "vice.h"

/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#ifndef __MSDOS__
#define NEED_2x
#endif /* __MSDOS__ */

#include <stdlib.h>
#include <stdio.h>

#include "crtc.h"
#include "raster.h"
#include "vmachine.h"
#include "interrupt.h"
#include "mem.h"
#include "resources.h"
#include "cmdline.h"
#include "utils.h"


#include "c610tpi.h"

#define	max(a,b)	(((a)>(b))?(a):(b))
#define	min(a,b)	(((a)<(b))?(a):(b))

/* ------------------------------------------------------------------------- */

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

/* Define the position of the raster beam precisely. */
#define RASTER_Y    	((int)(clk / CYCLES_PER_LINE) % SCREEN_HEIGHT)
#define RASTER_CYCLE	((int)(clk % CYCLES_PER_LINE))

static palette_t *palette;
static BYTE crtc[19];
static BYTE *chargen_ptr = NULL;
static int chargen_rel = 0;
static BYTE *screenmem = NULL;
static ADDRESS addr_mask = 0;
static ADDRESS scraddr = 0;

static int crsr_enable;
static int crsrpos;
static int scrpos;
static int crsrrel;
static int crsrmode;    /* 0=blank, 1=continously, 2=1/32, 3=1/16 frame rate */
static int crsrstart;   /* 1st cursor scan line */
static int crsrend;     /* last cursor scan line */
static int crsrstate;   /* 0 = off, 1 = on, toggled by int_* */
static int crsrcnt;

PIXEL4 dwg_table_0[256], dwg_table_1[256];
PIXEL4 dwg_table2x_0[256], dwg_table2x_1[256];
PIXEL4 dwg_table2x_2[256], dwg_table2x_3[256];

/* ------------------------------------------------------------------------- */

/* CRTC resources.  */

/* Name of palette file.  */
static char *palette_file_name;

/* Flag: Do we use double size?  */
static int double_size_enabled;

/* Flag: Do we enable the video cache?  */
static int video_cache_enabled;

/* Flag: Do we copy lines in double size mode?  */
static int double_scan_enabled;

static int set_video_cache_enabled(resource_value_t v)
{
    video_cache_enabled = (int) v;
    return 0;
}

/* prototype for resources - moved to raster.c */
static int set_palette_file_name(resource_value_t v);
#if 0
static int set_palette_file_name(resource_value_t v)
{
    /* If called before initialization, just set the resource value.  The
       palette file will be loaded afterwards.  */
    if (palette == NULL) {
        string_set(&palette_file_name, (char *) v);
        return 0;
    }

    if (palette_load((char *) v, palette) < 0)
        return -1;
    canvas_set_palette(canvas, palette, pixel_table);

    /* Make sure the pixel tables are recalculated properly.  */
    video_resize();

    string_set(&palette_file_name, (char *) v);
    return 0;
}
#endif

static int set_double_size_enabled(resource_value_t v)
{
    double_size_enabled = (int) v;
    video_resize();
    return 0;
}

static int set_double_scan_enabled(resource_value_t v)
{
    double_scan_enabled = (int) v;
    video_resize();
    return 0;
}

static resource_t resources[] = {
    { "PaletteFile", RES_STRING, (resource_value_t) "default",
      (resource_value_t *) &palette_file_name, set_palette_file_name },
#ifdef NEED_2x
    { "DoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_size_enabled, set_double_size_enabled },
#endif
#if defined NEED_2x || defined __MSDOS__
    { "DoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_scan_enabled, set_double_scan_enabled },
#endif
#ifndef __MSDOS__
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#else
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#endif
    { NULL }
};

int crtc_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* CRTC command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-vcache", SET_RESOURCE, 0, NULL, NULL,
      "VideoCache", (resource_value_t) 1,
      NULL, "Enable the video cache" },
    { "+vcache", SET_RESOURCE, 0, NULL, NULL,
      "VideoCache", (resource_value_t) 0,
      NULL, "Disable the video cache" },
    { "-palette", SET_RESOURCE, 1, NULL, NULL,
      "PaletteFile", NULL,
      "<name>", "Specify palette file name" },
#ifdef NEED_2x
    { "-dsize", SET_RESOURCE, 0, NULL, NULL,
      "DoubleSize", (resource_value_t) 1,
      NULL, "Enable double size" },
    { "+dsize", SET_RESOURCE, 0, NULL, NULL,
      "DoubleSize", (resource_value_t) 0,
      NULL, "Disable double size" },
    { "-dscan", SET_RESOURCE, 0, NULL, NULL,
      "DoubleScan", (resource_value_t) 1,
      NULL, "Enable double scan" },
    { "+dscan", SET_RESOURCE, 0, NULL, NULL,
      "DoubleScan", (resource_value_t) 0,
      NULL, "Disable double scan" },
#endif
    { NULL }
};

int crtc_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

void init_drawing_tables(void) {
    crtc_init_dwg_tables();
}

/* ------------------------------------------------------------------------- */

#include "raster.c"

/* -------------------------------------------------------------------------- */

canvas_t crtc_init(void)
{
    static const char *color_names[CRTC_NUM_COLORS] = {
        "Background", "Foreground"
    };

#ifdef __MSDOS__
    /* FIXME: Should set VGA mode.  */
    if (SCREEN_XPIX > 320)
	double_size_enabled = 1;
    else
        double_size_enabled = 0;
#endif

    init_raster(1, 2, 2);
    video_resize();

    palette = palette_create(CRTC_NUM_COLORS, color_names);
    if (palette == NULL)
        return NULL;
    if (palette_load(palette_file_name, palette) < 0) {
        printf("Cannot load palette file `%s'.\n", palette_file_name);
        return NULL;
    }

    if (open_output_window(CRTC_WINDOW_TITLE,
			   CRTC_SCREEN_XPIX + 10,
                           CRTC_SCREEN_YPIX + 10,
                           palette,
			   (canvas_redraw_t) crtc_arrange_window)) {
	fprintf(stderr, "fatal error: can't open window for CRTC emulation.\n");
	return NULL;
    }

    video_mode = CRTC_STANDARD_MODE;
    memptr_inc = crtc_cols;

    refresh_all();

    chargen_rel = 0;
    chargen_ptr = chargen_rom + chargen_rel;
    border_color = 0;
    background_color = 0;
    display_ystart = CRTC_SCREEN_BORDERHEIGHT;
    display_ystop = CRTC_SCREEN_BORDERHEIGHT + CRTC_SCREEN_MAX_YPIX;

    crtc_init_dwg_tables();

    return canvas;
}

static void crtc_init_dwg_tables(void)
{
    int byte, p;
    BYTE msk;

    for (byte = 0; byte < 0x0100; byte++) {
	*((PIXEL *) (dwg_table2x_0 + byte))
	    = *((PIXEL *) (dwg_table2x_0 + byte) + 1)
	    = PIXEL(byte & 0x80 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_0 + byte) + 2)
	    = *((PIXEL *) (dwg_table2x_0 + byte) + 3)
	    = PIXEL(byte & 0x40 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_1 + byte))
	    = *((PIXEL *) (dwg_table2x_1 + byte) + 1)
	    = PIXEL(byte & 0x20 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_1 + byte) + 2)
	    = *((PIXEL *) (dwg_table2x_1 + byte) + 3)
	    = PIXEL(byte & 0x10 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_2 + byte))
	    = *((PIXEL *) (dwg_table2x_2 + byte) + 1)
	    = PIXEL(byte & 0x08 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_2 + byte) + 2)
	    = *((PIXEL *) (dwg_table2x_2 + byte) + 3)
	    = PIXEL(byte & 0x04 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_3 + byte))
	    = *((PIXEL *) (dwg_table2x_3 + byte) + 1)
	    = PIXEL(byte & 0x02 ? 1 : 0);
	*((PIXEL *) (dwg_table2x_3 + byte) + 2)
	    = *((PIXEL *) (dwg_table2x_3 + byte) + 3)
	    = PIXEL(byte & 0x01 ? 1 : 0);
    }

    for (byte = 0; byte < 0x0100; byte++) {
	for (msk = 0x80, p = 0; p < 4; msk >>= 1, p++)
	    *((PIXEL *)(dwg_table_0 + byte) + p) = PIXEL(byte & msk ? 1 : 0);
	for (p = 0; p < 4; msk >>= 1, p++)
	    *((PIXEL *)(dwg_table_1 + byte) + p) = PIXEL(byte & msk ? 1 : 0);
    }
}

/* Set proper functions and constants for the current video settings. */
void video_resize(void)
{
    static int old_size = 0;

    if (double_size_enabled) {
	pixel_height = 2;
	if (crtc_cols == 40) {
	    pixel_width = 2;
	    video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
	    video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached_2x;
	    video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line_2x;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached_2x;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line_2x;
	    if (old_size == 1) {
		window_width *= 2;
		window_height *= 2;
		if (canvas)
		    canvas_resize(canvas, window_width, window_height);
	    }
	} else {
	    /* When in 80 column mode, only the height is doubled. */
	    pixel_width = 1;
	    video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
	    video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
	    video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
	    if (old_size == 1)
		window_height *= 2;
	}
    } else {
	pixel_width = 1;
	pixel_height = 1;
	video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
	video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
	video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
        video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
        video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
        video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
	if (old_size == 2) {
	    if (crtc_cols == 40)
		window_width /= 2;
	    window_height /= 2;
	}
    }
    old_size = double_size_enabled ? 2 : 1;

    if (canvas) {
	resize(window_width, window_height);
	frame_buffer_clear(&frame_buffer, pixel_table[0]);
	force_repaint();
	refresh_all();
    }
}

void video_free(void)
{
    frame_buffer_free(&frame_buffer);
}

static void crtc_arrange_window(int width, int height)
{
    resize(width, height);
    refresh_all();
}

/* -------------------------------------------------------------------------- */

static void crsr_set_dirty(void)
{
    int i, j;

    i = (crsrrel / memptr_inc) * 8 + crsrstart;
    i += SCREEN_BORDERHEIGHT + ysmooth;

    j = (crsrrel / memptr_inc) * 8 + crsrend;
    j += SCREEN_BORDERHEIGHT + ysmooth;

    while (i<=j) {
	cache[i++].is_dirty = 1;
    }
}

/* -------------------------------------------------------------------------- */

/* CRTC interface functions.
   FIXME: Several registers are not implemented.  */

void REGPARM2 store_crtc(ADDRESS addr, BYTE value)
{
    crtc[addr] = value;

    switch (addr) {
      case 0:			/* R00  Horizontal total (characters + 1) */
      case 1:			/* R01  Horizontal characters displayed */
        memptr_inc = crtc[1];
        if (crtc_cols == 80)
            memptr_inc *= 2;
        if (memptr_inc > crtc_cols)
            memptr_inc = crtc_cols;
	break;

      case 2:			/* R02  Horizontal Sync Position */
	break;

      case 3:			/* R03  Horizontal/Vertical Sync widths */
	break;

      case 4:			/* R04  Raster line count */
	break;

      case 5:			/* R05  Vertical Screen position */
	break;

      case 6:			/* R06  Number of display lines on screen */
	break;

      case 7:			/* R07  Lines displayed */
	break;

      case 8:			/* R08  unused: Interlace and Skew */
	break;

      case 9:			/* R09  Rasters between two display lines */
	/* FIXME */
	break;

      case 10:			/* R10  Cursor (not implemented on the PET) */
        crsrstart = min(7, value & 0x1f);
        value = ((value >> 5) & 0x03) ^ 0x01;
        if(crsr_enable && (crsrmode != value)) {
          /* printf("crtc: write R10 new cursormode = %d\n",value); */
          crsrmode = value;
          crsrstate = 1;
          crsrcnt = 16;
	  crsr_set_dirty();
        }
	break;

      case 11:			/* R11  Cursor (not implemented on the PET) */
	crsr_set_dirty();
        crsrend = min(7, value & 0x1f);
	crsr_set_dirty();
	break;

      case 12:			/* R12  Control register */

        /* This is actually the upper 6 video RAM address bits.
	 * But CBM decided that the two uppermost bits should be used
	 * for control (Can anyone kill the guy who got that idea?)
	 * The usage here is from the 8032 schematics on funet.
	 *
	 * Bit 0: 1=add 256 to screen start address ( 512 for 80-columns)
	 * Bit 1: 1=add 512 to screen start address (1024 for 80-columns)
	 * Bit 2: no connection
	 * Bit 3: no connection
	 * Bit 4: invert video signal
	 * Bit 5: use top half of 4K character generator
	 * Bit 6: (no pin on the CRTC, video address is 14 bit only)
	 * Bit 7: (no pin on the CRTC, video address is 14 bit only)
	 */

	crsr_set_dirty();
        crtc_update_memory_ptrs();
        scrpos = ((scrpos & 0x00ff) | ((value << 8) & 0x3f00)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 13:			/* R13  Address of first character */
	/* Value + 32786 (8000) */
	crsr_set_dirty();
	crtc_update_memory_ptrs();
        scrpos = ((scrpos & 0x3f00) | (value & 0xff)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 14:
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x00ff) | ((value << 8) & 0x3f00)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 15:			/* R14-5 Cursor location HI/LO -- unused */
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x3f00) | (value & 0xff)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 16:
      case 17:			/* R16-7 Light Pen HI/LO -- read only */
	break;

      case 18:
      case 19:			/* R18-9 Update address HI/LO (only 6545)  */
	break;
    }
}

BYTE REGPARM1 read_crtc(ADDRESS addr)
{
    switch (addr) {
      case 14:
      case 15:			/* Cursor location HI/LO */
	return crtc[addr];

      case 16:
      case 17:			/* Light Pen X,Y */
	return 0xff;

      default:
	return 0;		/* All the rest are write-only registers */
    }
}

BYTE REGPARM1 peek_crtc(ADDRESS addr)
{
    return read_crtc(addr);
}

void store_colorram(ADDRESS addr, BYTE value)
{
    /* No color RAM. */
}

BYTE read_colorram(ADDRESS addr)
{
    /* Bogus. */
    return 0;
}

void crtc_set_char(int crom)
{
    chargen_rel = (chargen_rel & ~0x800) | (crom ? 0x800 : 0);
    chargen_ptr = chargen_rom + chargen_rel;
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

    crsr_set_dirty();

    crsrpos = 0;
    scrpos = 0;
    crsrrel = 0;
    crsrmode = 0;
    crsrstart = 0;
    crsrend = 0;
    crsrstate = 0;
    crsrcnt = 0;

    maincpu_set_alarm_clk(A_RASTERDRAW, CYCLES_PER_LINE);
    return;
}

int crtc_offscreen(void)
{
    return rasterline >= CRTC_SCREEN_YPIX;
}

void crtc_set_screen_mode(BYTE *screen, int vmask, int num_cols, int hwcursor)
{
    int w;

    if (double_size_enabled)
        w = window_width;
    else
        w = (float)window_width * ((float)num_cols / (float)crtc_cols);

    memptr_inc = crtc_cols = num_cols;

    addr_mask = vmask;
    screenmem = screen;
    crsr_enable = hwcursor;

    display_xstart = SCREEN_BORDERWIDTH;
    display_xstop = SCREEN_BORDERWIDTH + SCREEN_XPIX;
    display_ystart = SCREEN_BORDERHEIGHT;
    display_ystop = SCREEN_BORDERHEIGHT + SCREEN_YPIX;

#ifdef __MSDOS__
    /* FIXME: This does not have any effect until there is a gfx -> text ->
       gfx mode transition.  Moreover, no resources should be changed behind
       user's back...  So this is definitely a Bad Thing (tm).  For now, it's
       fine with us, though.  */
    resources_set_value("VGAMode",
                        (resource_value_t) (crtc_cols > 40
                                            ? VGA_640x480 : VGA_320x200));
    if (SCREEN_XPIX > 320)
	double_size_enabled = 1;
    else
        double_size_enabled = 0;
#endif

    if (canvas) {
	canvas_resize(canvas, w, window_height);
	crtc_arrange_window(w, window_height);
	video_resize();
    }
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


    if (!(scraddr & 0x1000)) {
        video_mode = CRTC_STANDARD_MODE;
    } else {
        video_mode = CRTC_REVERSE_MODE;
    }
    chargen_rel = (chargen_rel & ~0x1000) | ((scraddr & 0x800) ? 0x1000 : 0);

    chargen_ptr = chargen_rom + chargen_rel;

    scraddr &= addr_mask;
}

/* -------------------------------------------------------------------------- */

int int_rasterdraw(long offset)
{
    maincpu_set_alarm(A_RASTERDRAW, CYCLES_PER_LINE - offset);
    emulate_line();

    /* This generates one raster interrupt per frame. */
    if (rasterline == 0) {
        /* we assume this to start the screen */
	tpi1_set_int(0, 1);
        if(crsrmode & 0x02) {
            if(crsrcnt) crsrcnt--;
            else {
		crsr_set_dirty();
                crsrcnt = (crsrmode & 0x01) ? 16 : 32;
                crsrstate ^= 1;
            }
        }
    } else if (rasterline == CRTC_SCREEN_YPIX) {
        /* and this to end the screen */
	tpi1_set_int(0, 0);
    }

    return 0;
}

static int fill_cache(struct line_cache *l, int *xs, int *xe, int r)
{
    static BYTE fake_char_data = 0;
    int retval, n;

    retval = _fill_cache_text(l->fgdata, screenmem + memptr, chargen_ptr,
                              memptr_inc, ycounter, xs, xe, r);

    n = crtc_cols - memptr_inc;

    /* All the characters beyond the `memptr_inc'th one are blank.  */
    if (n > 0) {
        int xs1 = SCREEN_TEXTCOLS, xe1 = -1;

        if (_fill_cache(l->fgdata + memptr_inc, &fake_char_data, n, 0,
                        &xs1, &xe1, r)) {
            xs1 += memptr_inc, *xs = MIN(xs1, *xs);
            xe1 += memptr_inc, *xe = MAX(xe1, *xe);
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
        if(crsrmode)							\
        for (i = 0; i < memptr_inc; i++, p += 8) {			\
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i], 	\
				ycounter);				\
            if(crsrstate						\
                    && (memptr+i)==crsrrel				\
                    && ycounter >= crsrstart				\
                    && ycounter <=crsrend)				\
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
									\
        d = (reverse_flag) ? 0xff: 0;                                   \
        for (; i < crtc_cols; i++, p += 8) {                            \
                *((PIXEL4 *) p) = dwg_table_0[d];                       \
                *((PIXEL4 *) p + 1) = dwg_table_1[d];                   \
        }                                                               \
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
        if(crsrmode)							\
        for (i = 0; i < memptr_inc; i++, p += 16) {			\
            d = GET_CHAR_DATA(chargen_ptr, (screenmem + memptr)[i], 	\
			ycounter);					\
            if(crsrstate						\
                    && (memptr+i)==crsrrel				\
                    && ycounter >= crsrstart				\
                    && ycounter <= crsrend)				\
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
                                                                        \
        d = (reverse_flag) ? 0xff : 0;                                  \
        for (; i < crtc_cols; i++, p += 16) {                           \
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
        register int mempos = ((l->n+1)/8 )*memptr_inc;			\
	register int ypos = (l->n+1) & 7;				\
									\
        for (i = (xs); i <= (xe); i++, p += 8) {                        \
            BYTE d = (l)->fgdata[i];                                    \
            if ((reverse_flag))                                         \
                d = ~d;                                                 \
                                                                        \
            if(crsrmode && mempos+i==crsrrel && crsrstate		\
                    && ypos >= crsrstart && ypos <=crsrend) {		\
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
        register int mempos = ((l->n+1)/8 )*memptr_inc;         \
        register int ypos = (l->n+1) & 7;                       \
                                                                \
        for (i = (xs); i <= (xe); i++, p += 16) {               \
            BYTE d = (l)->fgdata[i];                            \
            if ((reverse_flag))                                 \
                d = ~d;                                         \
								\
            if(crsrmode && mempos+i==crsrrel && crsrstate       \
                    && ypos >= crsrstart && ypos <=crsrend) {   \
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
}

/* ------------------------------------------------------------------------- */

/* Snapshot.  */

/* FIXME: This does not fully save/restore the chip state; e.g. the current
   character line and the current value of the memory pointer are not taken
   into account.  This means that the first frame after the restore will be
   wrong, but things will be fine in the next frame.  This is not a real
   problem for the CRTC, as these data are not timing-sensitive as on the
   VIC-II, but some day we will have to fix it.  For now, it is even better
   to leave as it is because we are not accurately modeling the CRTC chip and
   the behavior of the raster, so the data we would write would only be
   bogus.  */

static char snap_module_name[] = "CRTC";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int vic_write_snapshot_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_byte(m, (BYTE) RASTER_CYCLE) < 0
        || snapshot_module_write_word(m, (WORD) RASTER_Y) < 0)
        goto fail;

    if (snapshot_module_write_word(m, (WORD) (screenmem - ram)) < 0
        || snapshot_module_write_word(m, (WORD) addr_mask) < 0
        || snapshot_module_write_byte(m, (BYTE) crtc_cols) < 0
        || snapshot_module_write_byte(m, (BYTE) crsr_enable) < 0)
        goto fail;

    for (i = 0; i < 20; i++)
        if (snapshot_module_write_byte(m, crtc[i]) < 0)
            goto fail;

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int crtc_read_snapshot_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;
    WORD w;
    BYTE b;
    WORD vmask, screen;
    BYTE num_cols, hwcursor;
    BYTE major, minor;

    m = snapshot_module_open(s, snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    if (major != SNAP_MAJOR) {
        fprintf(stderr, "CRTC: Major snapshot number (%d) invalid; %d expected.\n",
                major, SNAP_MAJOR);
        goto fail;
    }

    if (snapshot_module_read_byte(m, &b) < 0)
        goto fail;
    if (b != RASTER_CYCLE) {
        fprintf(stderr, "CRTC: Cycle value (%d) incorrect; should be %d.\n",
                (int) b, RASTER_CYCLE);
        goto fail;
    }

    if (snapshot_module_read_word(m, &w) < 0)
        goto fail;
    if (w != RASTER_Y) {
        fprintf(stderr, "CRTC: Raster line value (%d) incorrect; should be %d.\n",
                (int) b, RASTER_Y);
        goto fail;
    }

    if (snapshot_module_read_word(m, &screen) < 0
        || snapshot_module_read_word(m, &vmask) < 0
        || snapshot_module_read_byte(m, &num_cols) < 0
        || snapshot_module_read_byte(m, &hwcursor))
        goto fail;
    crtc_set_screen_mode(ram + (int) screen, vmask, num_cols, hwcursor);

    for (i = 0; i < 20; i++) {
        if (snapshot_module_read_byte(m, &b) < 0)
            goto fail;

        /* XXX: This assumes that there are no side effects.  */
        store_crtc(i, b);
    }

    force_repaint();
    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}
