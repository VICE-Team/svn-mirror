/*
 * vic.c - A line-based VIC-I emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

/* Warning: this emulation is very incomplete and buggy.  */

#ifndef VIC20
#define VIC20
#endif

#define _VIC_C

/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#if (!defined(__MSDOS__) && !defined(__riscos))
#define NEED_2x
#endif /* !__MSDOS__ */

#include "vice.h"

#include <stdlib.h>
#include <stdio.h>

#include "vic.h"
#include "vmachine.h"
#include "interrupt.h"
#include "raster.h"
#include "vic20sound.h"
#include "log.h"
#include "mem.h"
#include "resources.h"
#include "cmdline.h"
#include "utils.h"

/* #define VIC_REGISTERS_DEBUG */

/* ------------------------------------------------------------------------ */

/* VIC resources.  */

/* Flag: Do we use double size?  */
static int double_size_enabled;

/* Flag: Do we enable the video cache?  */
static int video_cache_enabled;

/* Flag: Do we copy lines in double size mode?  */
static int double_scan_enabled;

/* Name of palette file.  */
static char *palette_file_name;

#ifdef USE_VIDMODE_EXTENSION
/* Flag: Fullscreenmode?  */
static int fullscreen = 0; 

/* Flag: Do we use double size?  */
static int fullscreen_double_size_enabled;

/* Flag: Do we copy lines in double size mode?  */
static int fullscreen_double_scan_enabled;

static int fullscreen_width;
static int fullscreen_height;

#endif

static int set_video_cache_enabled(resource_value_t v)
{
    video_cache_enabled = (int) v;
    return 0;
}

/* Prototype for resources - new function from raster.c.  */
static int set_palette_file_name(resource_value_t v);

#ifdef NEED_2x
static int set_double_size_enabled(resource_value_t v)
{
    double_size_enabled = (int) v;
#ifdef USE_VIDMODE_EXTENSION
    if(!fullscreen)
#endif
        video_resize();
    return 0;
}

static int set_double_scan_enabled(resource_value_t v)
{
    double_scan_enabled = (int) v;
#ifdef USE_VIDMODE_EXTENSION
    if(!fullscreen)
#endif
        video_resize();
    return 0;
}
#endif

#ifdef USE_VIDMODE_EXTENSION

void fullscreen_forcerepaint();

#ifdef NEED_2x
static int set_fullscreen_double_size_enabled(resource_value_t v)
{
    fullscreen_double_size_enabled = (int) v;
    fullscreen_forcerepaint();
    return 0;
}
#endif

static int set_fullscreen_double_scan_enabled(resource_value_t v)
{
    fullscreen_double_scan_enabled = (int) v;
    fullscreen_forcerepaint();
    return 0;
}

#endif

static resource_t resources[] = {
    { "DoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_size_enabled, set_double_size_enabled },
    { "DoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_scan_enabled, set_double_scan_enabled },
#ifdef USE_VIDMODE_EXTENSION
#ifdef NEED_2x
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled },
#endif
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled },
#endif
    { "PaletteFile", RES_STRING, (resource_value_t) "default",
      (resource_value_t *) &palette_file_name, set_palette_file_name },
#ifndef __MSDOS__
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#else
    { "VideoCache", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#endif
    { NULL }
};

int vic_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* VIC command-line options.  */

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

int vic_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static log_t vic_log = LOG_ERR;

static void set_memory_ptrs(void);
static void init_drawing_tables(void);
static int fill_cache(struct line_cache *l, int *xs, int *xe, int r);
static void draw_line(void);
static void draw_line_2x(void);
static void draw_line_cached(struct line_cache *l, int xs, int xe);
static void draw_line_cached_2x(struct line_cache *l, int xs, int xe);
static void draw_reverse_line(void);
static void draw_reverse_line_2x(void);
static void draw_reverse_line_cached(struct line_cache *l, int xs, int xe);
static void draw_reverse_line_cached_2x(struct line_cache *l, int xs, int xe);

/* Define the position of the raster beam precisely. */
#define RASTER_Y    	((int)(clk / CYCLES_PER_LINE) % SCREEN_HEIGHT)
#define RASTER_CYCLE	((int)(clk % CYCLES_PER_LINE))

static palette_t *palette;
static BYTE vic[64];
static BYTE auxiliary_color;
static BYTE *colormem;
static BYTE *screenmem;
static BYTE *chargen_ptr = chargen_rom + 0x400;

/* On MS-DOS, do not duplicate pixels.  Otherwise, we would always need at
   least 466 horizontal pixels to contain the whole screen.  */
#ifndef __MSDOS__
#define DUPLICATE_PIXELS
#endif

#ifdef DUPLICATE_PIXELS
typedef PIXEL2 VIC_PIXEL;
#define VIC_PIXEL(n)	PIXEL2(n)
typedef PIXEL4 VIC_PIXEL2;
#define VIC_PIXEL2(n)	PIXEL4(n)
#define VIC_PIXEL_WIDTH	2
#else
typedef PIXEL VIC_PIXEL;
#define VIC_PIXEL(n)	PIXEL(n)
typedef PIXEL2 VIC_PIXEL2;
#define VIC_PIXEL2(n)	PIXEL2(n)
#define VIC_PIXEL_WIDTH	1
#endif

/*
 *  For some reason the RISC OS GCC-preprocessor doesn't like the original
 *  declaration of c[4], so use this hack to force it.
 */
#define DUMMY_SEPARATOR

#include "raster.c"

/* ------------------------------------------------------------------------- */

/* Cycle # within the current line.  */
#define RASTER_CYCLE	((int)(clk % CYCLES_PER_LINE))

/* Current vertical position of the raster.  Unlike `rasterline', which is
   only accurate if a pending `A_RASTERDRAW' event has been served, this is
   guarranteed to be always correct.  It is a bit slow, though.  */
#define RASTER_Y    	((int)(clk / CYCLES_PER_LINE) % SCREEN_HEIGHT)

/* ------------------------------------------------------------------------- */

/* Initialization. */
canvas_t vic_init(void)
{
    static const char *color_names[] = {
        "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue",
        "Yellow", "Orange", "Light Orange", "Pink", "Light Cyan",
        "Light Purple", "Light Green", "Light Blue", "Light Yellow"
    };
    int width, height;

    if (vic_log == LOG_ERR)
        vic_log = log_open("VIC");

    /* FIXME: the maximum pixel width should be 4 instead of 6, but we need
       some extra space for clipping long lines...  This should be done in a
       cleaner way.  */
    if (init_raster(1, 6, 2) < 0)
        return NULL;

    width = VIC_SCREEN_WIDTH;
    height = (VIC_SCREEN_LAST_DISPLAYED_LINE
	      - VIC_SCREEN_FIRST_DISPLAYED_LINE + 1);

    video_resize();

    palette = palette_create(VIC_NUM_COLORS, color_names);
    if (palette == NULL)
        return NULL;

    if (palette_load(palette_file_name, palette) < 0) {
        log_error(vic_log, "Cannot load default palette.");
        return NULL;
    }

    if (open_output_window(VIC_WINDOW_TITLE,
			   width, height, palette,
			   (canvas_redraw_t)vic_exposure_handler)) {
        log_error(vic_log, "Cannot open window for the VIC emulation.");
	return NULL;
    }

    video_mode = VIC_STANDARD_MODE;
    set_memory_ptrs();
    refresh_all();
    init_drawing_tables();

    return canvas;
}

/* This hook is called whenever the screen parameters (eg. window size) are
   changed.  */
void video_resize(void)
{
    static int old_size = 0;

#ifdef USE_VIDMODE_EXTENSION
    if (fullscreen?fullscreen_double_size_enabled:double_size_enabled) {
#else
    if (double_size_enabled) {
#endif
	pixel_width = 2 * VIC_PIXEL_WIDTH;
	pixel_height = 2;
	video_modes[VIC_STANDARD_MODE].fill_cache = fill_cache;
	video_modes[VIC_STANDARD_MODE].draw_line_cached = draw_line_cached_2x;
	video_modes[VIC_STANDARD_MODE].draw_line = draw_line_2x;
	video_modes[VIC_REVERSE_MODE].fill_cache = fill_cache;
	video_modes[VIC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached_2x;
	video_modes[VIC_REVERSE_MODE].draw_line = draw_reverse_line_2x;
	if (old_size == 1) {
	    window_width *= 2;
	    window_height *= 2;
	}
    } else {
	pixel_width = VIC_PIXEL_WIDTH;
	pixel_height = 1;
	video_modes[VIC_STANDARD_MODE].fill_cache = fill_cache;
	video_modes[VIC_STANDARD_MODE].draw_line_cached = draw_line_cached;
	video_modes[VIC_STANDARD_MODE].draw_line = draw_line;
	video_modes[VIC_REVERSE_MODE].fill_cache = fill_cache;
	video_modes[VIC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
	video_modes[VIC_REVERSE_MODE].draw_line = draw_reverse_line;
	if (old_size == 2) {
	    window_width /= 2;
	    window_height /= 2;
	}
    }

#ifdef USE_VIDMODE_EXTENSION
    old_size = (fullscreen?fullscreen_double_size_enabled:double_size_enabled) ? 2 : 1;
#else
    old_size = (double_size_enabled) ? 2 : 1;
#endif

    if (canvas) {
	resize(window_width, window_height);
	frame_buffer_clear(&frame_buffer, PIXEL(0));
	force_repaint();
    }
}

void video_free(void)
{
    frame_buffer_free(&frame_buffer);
}

/* ------------------------------------------------------------------------- */

/* VIC access functions. */

void REGPARM2 store_vic(ADDRESS addr, BYTE value)
{
    addr &= 0xf;
    vic[addr] = value;

#ifdef VIC_REGISTERS_DEBUG
    log_message(vic_log, "VIC: write $90%02X, value = $%02X.", addr, value);
#endif

    switch (addr) {
      case 0:			/* $9000  Screen X Location. */
	value &= 0x7f;
	if (value > 8)
	    value = 8;
	if (value < 1)
	    value = 1;
	display_xstart = value * 4;
	display_xstop = display_xstart + text_cols * 8;
	if (display_xstop >= VIC_SCREEN_WIDTH)
	    display_xstop = VIC_SCREEN_WIDTH - 1;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Screen X location: $%02X.", value);
#endif
	return;
      case 1:			/* $9001  Screen Y Location. */
	display_ystart = value * 2;
	display_ystop = display_ystart + text_lines * char_height;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Screen Y location: $%02X.", value);
#endif
	return;

      case 2:			/* $9002  Columns Displayed. */
	colormem = ram + ((value & 0x80) ? 0x9600 : 0x9400);
	text_cols = value & 0x7f;
	if (text_cols > VIC_SCREEN_MAX_TEXTCOLS)
	    text_cols = VIC_SCREEN_MAX_TEXTCOLS;
	display_xstop = display_xstart + text_cols * 8;
	if (display_xstop >= VIC_SCREEN_WIDTH)
	    display_xstop = VIC_SCREEN_WIDTH - 1;
	set_memory_ptrs();
        memptr_inc = text_cols;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Color RAM at $%04X.", colormem - ram);
	log_message(vic_log, "Columns displayed: %d.", text_cols);
#endif
	break;

      case 3:			/* $9003  Rows Displayed, Character size . */
	text_lines = (value & 0x7e) >> 1;
	if (text_lines > VIC_SCREEN_MAX_TEXTLINES)
	    text_lines = VIC_SCREEN_MAX_TEXTLINES;
	char_height = (value & 0x1) ? 16 : 8;
	display_ystop = display_ystart + text_lines * char_height;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Rows displayed: %d.", text_lines);
	log_message(vic_log, "Character height: %d.", char_height);
#endif
	set_memory_ptrs();
	return;

      case 4:			/* $9004  Raster line count -- read only. */
	return;

      case 5:			/* $9005  Video and char matrix base address. */
	set_memory_ptrs();
	return;

      case 6:			/* $9006. */
      case 7:			/* $9007  Light Pen X,Y. */
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "(light pen register, read-only).");
#endif
	return;

      case 8:			/* $9008. */
      case 9:			/* $9009  Paddle X,Y. */
	return;

      case 10:			/* $900A  Bass Enable and Frequency. */
      case 11:			/* $900B  Alto Enable and Frequency. */
      case 12:			/* $900C  Soprano Enable and Frequency. */
      case 13:			/* $900D  Noise Enable and Frequency. */
	store_sound(addr, value);
	return;

      case 14:			/* $900E  Auxiliary Colour, Master Volume. */
	auxiliary_color = value >> 4;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Auxiliary color set to $%02X.", auxiliary_color);
#endif
	store_sound(addr, value);
	return;

      case 15:			/* $900F  Screen and Border Colors,
				   Reverse Video. */
	border_color = value & 0x7;
	background_color = value >> 4;
	video_mode = (value & 8) ? VIC_STANDARD_MODE : VIC_REVERSE_MODE;
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Border color: $%02X.", border_color);
	log_message(vic_log, "Background color: $%02X.", background_color);
#endif
	return;
    }
}

BYTE REGPARM1 read_vic(ADDRESS addr)
{
    addr &= 0xf;

    switch (addr) {
      case 3:
	return ((RASTER_Y & 1) << 7) | (vic[3] & ~0x80);
      case 4:
	return RASTER_Y >> 1;
      default:
	return vic[addr];
    }
}

/* ------------------------------------------------------------------------- */

/* Set the memory pointers according to the values stored in the VIC
   registers. */
static void set_memory_ptrs(void)
{
    int tmp;
    ADDRESS charaddr;

    tmp = vic[0x5] & 0xf;
    charaddr = (tmp & 0x8) ? 0x0000 : 0x8000;
    charaddr += (tmp & 0x7) * 0x400;
    if (charaddr >= 0x8000 && charaddr < 0x9000) {
	chargen_ptr = chargen_rom + 0x400 + (charaddr & 0xfff);
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log,
                    "Character memory at $%04X (character ROM + $%04X).",
                    charaddr, charaddr & 0xfff);
#endif
    } else {
	if(charaddr == 0x1c00) {
	    chargen_ptr = chargen_rom; 	/* handle wraparound */
	} else {
	   chargen_ptr = ram + charaddr;
	}
#ifdef VIC_REGISTERS_DEBUG
	log_message(vic_log, "Character memory at $%04X.", charaddr);
#endif
    }
    colormem = ram + 0x9400 + (vic[0x2] & 0x80 ? 0x200 : 0x0);
    screenmem = ram + (((vic[0x2] & 0x80) << 2) | ((vic[0x5] & 0x70) << 6));
#ifdef VIC_REGISTERS_DEBUG
    log_message(vic_log, "Color memory at $%04X.", colormem - ram);
    log_message(vic_log, "Screen memory at $%04X.", screenmem - ram);
#endif
}

/* ------------------------------------------------------------------------- */

/* Here comes the part that actually repaints each raster line.  This table is
   used to speed up the drawing. */
static WORD dwg_table[256][256][8];	/* [byte][color][position] */

static void init_drawing_tables(void)
{
    int byte, color, pos;

    for (byte = 0; byte < 0x100; byte++) {
	for (color = 0; color < 0x100; color++) {
	    if (color & 0x8) {	/* Multicolor mode. */
		for (pos = 0; pos < 8; pos += 2) {
		    dwg_table[byte][color][pos]
			= dwg_table[byte][color][pos + 1]
			= (byte >> (6 - pos)) & 0x3;
		}
	    } else {		/* Standard mode. */
		for (pos = 0; pos < 8; pos++) {
		    dwg_table[byte][color][pos] = ((byte >> (7 - pos))
						   & 0x1) * 2;
		}
	    }
	}
    }
}

/* ------------------------------------------------------------------------- */

/* Notice: The screen origin X register has a 4-pixel granularity, so our
   write accesses are always aligned. */

int int_rasterdraw(long offset)
{
    maincpu_set_alarm(A_RASTERDRAW, CYCLES_PER_LINE - offset);
    emulate_line();

    if (rasterline == 0) {
	/* Turn border on. */
	blank_enabled = 1;
    }

    return 0;
}

static int fill_cache(struct line_cache *l, int *xs, int *xe, int r)
{
    if (l->bgdata[0] != background_color || l->colordata2[0] != auxiliary_color
	|| l->numcols != text_cols) {
	l->bgdata[0] = background_color;
	l->colordata2[0] = auxiliary_color;
	l->numcols = text_cols;
	*xs = 0;
	*xe = text_cols;
	r = 1;
    }
    r = _fill_cache(l->colordata1, colormem + memptr, VIC_SCREEN_TEXTCOLS, 1,
		    xs, xe, r);
    r = _fill_cache_text(l->fgdata, screenmem + memptr, chargen_ptr,
			 VIC_SCREEN_TEXTCOLS, ycounter, xs, xe, r);
    return r;
}

#define PUT_PIXEL(p, d, c, b, x) \
      *((VIC_PIXEL *)(p) + (x)) = (c)[dwg_table[(d)][(b)][(x)]]

#define DRAW_LINE(p, xs, xe, reverse)					     \
  do {									     \
      static VIC_PIXEL DUMMY_SEPARATOR c[4];		 		     \
      int b, i;								     \
      BYTE d;								     \
      PIXEL *pp = (PIXEL *)(p) + (xs) * 8 * VIC_PIXEL_WIDTH;		     \
									     \
      c[0] = VIC_PIXEL(background_color);				     \
      c[1] = VIC_PIXEL(border_color);					     \
      c[3] = VIC_PIXEL(auxiliary_color);				     \
      for (i = (xs); i <= (xe); i++, pp += 8 * VIC_PIXEL_WIDTH) {	     \
	  b = (colormem + memptr)[i];					     \
	  c[2] = VIC_PIXEL(b & 0x7);					     \
          if (reverse)							     \
	      d = ~(GET_CHAR_DATA (chargen_ptr, (screenmem + memptr)[i],     \
				   ycounter));				     \
	  else								     \
	      d = GET_CHAR_DATA (chargen_ptr, (screenmem + memptr)[i],	     \
				 ycounter);				     \
	  PUT_PIXEL(pp, d, c, b, 0); PUT_PIXEL(pp, d, c, b, 1);		     \
	  PUT_PIXEL(pp, d, c, b, 2); PUT_PIXEL(pp, d, c, b, 3);	             \
	  PUT_PIXEL(pp, d, c, b, 4); PUT_PIXEL(pp, d, c, b, 5);		     \
	  PUT_PIXEL(pp, d, c, b, 6); PUT_PIXEL(pp, d, c, b, 7);		     \
      }									     \
  } while (0)

static void draw_line(void)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH;

    DRAW_LINE(p, 0, text_cols - 1, 0);
}

static void draw_reverse_line(void)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH;

    DRAW_LINE(p, 0, text_cols - 1, 1);
}

static void draw_line_cached(struct line_cache *l, int xs, int xe)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH;

    DRAW_LINE(p, xs, xe, 0);
}

static void draw_reverse_line_cached(struct line_cache *l, int xs, int xe)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH;

    DRAW_LINE(p, xs, xe, 1);
}

#define PUT_PIXEL_2x(p, d, c, b, x) \
      *((VIC_PIXEL2 *)(p) + (x)) = (c)[dwg_table[(d)][(b)][(x)]]

#define DRAW_LINE_2x(p, xs, xe, reverse)				     \
  do {									     \
      static VIC_PIXEL2 DUMMY_SEPARATOR c[4];				     \
      BYTE d, b;							     \
      int i;								     \
      PIXEL *pp = (PIXEL *)(p) + (xs) * 16 * VIC_PIXEL_WIDTH;		     \
									     \
      c[0] = VIC_PIXEL2(background_color);			             \
      c[1] = VIC_PIXEL2(border_color);					     \
      c[3] = VIC_PIXEL2(auxiliary_color);			             \
      for (i = (xs); i <= (xe); i++, pp += 16 * VIC_PIXEL_WIDTH) {	     \
	  b = (colormem + memptr)[i];					     \
	  c[2] = VIC_PIXEL2(b & 0x7);					     \
          if (reverse)							     \
	      d = ~(GET_CHAR_DATA (chargen_ptr, (screenmem + memptr)[i],     \
				   ycounter));				     \
          else								     \
	      d = GET_CHAR_DATA (chargen_ptr, (screenmem + memptr)[i],	     \
				 ycounter);				     \
	  PUT_PIXEL_2x(pp, d, c, b, 0); PUT_PIXEL_2x(pp, d, c, b, 1);	     \
	  PUT_PIXEL_2x(pp, d, c, b, 2); PUT_PIXEL_2x(pp, d, c, b, 3);	     \
	  PUT_PIXEL_2x(pp, d, c, b, 4); PUT_PIXEL_2x(pp, d, c, b, 5);	     \
	  PUT_PIXEL_2x(pp, d, c, b, 6); PUT_PIXEL_2x(pp, d, c, b, 7);	     \
      }									     \
  } while (0)

static void draw_line_2x(void)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH * 2;

    DRAW_LINE_2x(p, 0, text_cols - 1, 0);
}

static void draw_reverse_line_2x(void)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH * 2;

    DRAW_LINE_2x(p, 0, text_cols - 1, 1);
}

static void draw_line_cached_2x(struct line_cache *l, int xs, int xe)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH * 2;

    DRAW_LINE_2x(p, xs, xe, 0);
}

static void draw_reverse_line_cached_2x(struct line_cache *l, int xs, int xe)
{
    PIXEL *p = frame_buffer_ptr + display_xstart * VIC_PIXEL_WIDTH * 2;

    DRAW_LINE_2x(p, xs, xe, 1);
}

/* ------------------------------------------------------------------------- */

void vic_exposure_handler(unsigned int width, unsigned int height)
{
    if(fullscreen) return;
    resize(width, height);
    force_repaint();
}

void vic_prevent_clk_overflow(CLOCK sub)
{
    oldclk -= sub;
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = "VIC-I";
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

    if (snapshot_module_write_word(m, (WORD) memptr) < 0)
        goto fail;

    /* Color RAM.  */
    if (snapshot_module_write_byte_array(m, ram + 0x9400, 0x800) < 0)
        goto fail;

    for (i = 0; i < 0x10; i++)
        if (snapshot_module_write_byte(m, (BYTE) vic[i]) < 0)
            goto fail;

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int vic_read_snapshot_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;
    BYTE major_version, minor_version;
    WORD w;
    BYTE b;

    sound_close();

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        log_error(vic_log, "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (snapshot_module_read_byte(m, &b) < 0)
        goto fail;
    if (b != RASTER_CYCLE) {
        log_error(vic_log, "Cycle value (%d) incorrect; should be %d.",
                  (int) b, RASTER_CYCLE);
        goto fail;
    }

    if (snapshot_module_read_word(m, &w) < 0)
        goto fail;
    if (w != RASTER_Y) {
        log_error(vic_log, "Raster line value (%d) incorrect; should be %d.",
                  (int) w, RASTER_Y);
        goto fail;
    }

    if (snapshot_module_read_word(m, &w) < 0)
        goto fail;
    memptr = w;

    /* Color RAM.  */
    if (snapshot_module_read_byte_array(m, ram + 0x9400, 0x800) < 0)
        goto fail;

    for (i = 0; i < 0x10; i++) {
        if (snapshot_module_read_byte(m, &b) < 0)
            goto fail;

        /* XXX: This assumes that there are no side effects.  */
        store_vic(i, b);
    }

    maincpu_set_alarm(A_RASTERDRAW, CYCLES_PER_LINE - RASTER_CYCLE);

    force_repaint();
    return snapshot_module_close(m);

 fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

#ifdef USE_VIDMODE_EXTENSION
void video_setfullscreen(int v,int width, int height) {
    fullscreen = v;
    fullscreen_width = width;
    fullscreen_height = height;

    video_resize();
    if(v) {
        resize(width, height);
	force_repaint();
    }
    video_resize();
}

void fullscreen_forcerepaint() {
    if(fullscreen) {
	video_resize();
        resize(fullscreen_width, fullscreen_height);
	force_repaint();
	video_resize();
    }
}
#endif
