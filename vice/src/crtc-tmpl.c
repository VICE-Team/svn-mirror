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

#define _CRTC_C

#define CRTC_WINDOW_TITLE            MY_WINDOW_TITLE

/*
#define NDEBUG
#include <assert.h>
#define memset(a,b,c)   (assert((a)),memset((a),(b),(c)))
*/

#include "vice.h"

/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#ifndef __MSDOS__
#define NEED_2x
#endif /* __MSDOS__ */

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stdio.h>
#endif

#include "cmdline.h"
#include "crtc.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "raster.h"
#include "resources.h"
#include "snapshot.h"
#include "utils.h"
#include "vmachine.h"

INCLUDES

#define	crtc_max(a,b)	(((a)>(b))?(a):(b))
#define	crtc_min(a,b)	(((a)<(b))?(a):(b))

#define	MAX_PIXEL_WIDTH		2
#define	MAX_PIXEL_HEIGHT	2

#define IS_DOUBLE_WIDTH_ALLOWED(a)	\
	(((a) * 8 + 2 * SCREEN_BORDERWIDTH) <= (FRAMEB_WIDTH / MAX_PIXEL_WIDTH))
#define IS_DOUBLE_HEIGHT_ALLOWED(a)	\
	(((a) + 2 * SCREEN_BORDERHEIGHT) <= (FRAMEB_HEIGHT / MAX_PIXEL_HEIGHT))

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

static log_t crtc_log = LOG_ERR;

static PIXEL4 dwg_table_0[256], dwg_table_1[256];
static PIXEL4 dwg_table2x_0[256], dwg_table2x_1[256];
static PIXEL4 dwg_table2x_2[256], dwg_table2x_3[256];

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

/* prototype for resources - moved to raster.c */
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

    if (crtc_log == LOG_ERR)
        crtc_log = log_open("CRTC");
    
    if (init_raster(1, MAX_PIXEL_WIDTH, MAX_PIXEL_HEIGHT) < 0)
        return NULL;

    video_resize();

    palette = palette_create(CRTC_NUM_COLORS, color_names);
    if (palette == NULL)
        return NULL;
    if (palette_load(palette_file_name, palette) < 0) {
        log_message(crtc_log, "Cannot load palette file `%s'.",
                    palette_file_name);
        return NULL;
    }

    if (open_output_window(CRTC_WINDOW_TITLE,
			   SCREEN_WIDTH, 
			   SCREEN_HEIGHT,
                           palette,
			   (canvas_redraw_t) crtc_arrange_window)) {
	log_error(crtc_log, "Cannot open window for CRTC emulation.");
	return NULL;
    }

    video_mode = CRTC_STANDARD_MODE;

    if (canvas) {
        refresh_changed();
        refresh_all();
    }

    chargen_rel = 0;
    chargen_ptr = chargen_rom + chargen_rel;
    border_color = 0;
    background_color = 0;
    display_ystart = SCREEN_BORDERHEIGHT;
    display_ystop = SCREEN_BORDERHEIGHT + CRTC_SCREEN_MAX_YPIX;

    crtc_init_dwg_tables();

    if (canvas) {
	store_crtc(0, 49);
	store_crtc(1, 40);
	store_crtc(4, 49);
	store_crtc(5, 0);
	store_crtc(6, 25);
	store_crtc(9, 7);
	crtc_update_timing(1);
    }
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

    if (DOUBLE_SIZE_ENABLED()) {
	if (IS_DOUBLE_WIDTH_ALLOWED(memptr_inc)) {
	    pixel_width = 2;
	    video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
	    video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached_2x;
	    video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line_2x;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached_2x;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line_2x;
	    if (old_size == 1)
		window_width *= 2;
	} else {
	    /* When in 80 column mode, only the height is doubled. */
	    pixel_width = 1;
	    video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
	    video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
	    video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
	}
	if (IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines*screen_charheight)) {
	    pixel_height = 2;
	    if (old_size == 1) {
		window_height *= 2;
	    }
	} else {
	    pixel_height = 1;
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
	    if (IS_DOUBLE_WIDTH_ALLOWED(memptr_inc))
		window_width /= 2;
	    if (IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines*screen_charheight))
	        window_height /= 2;
	}
    }
    old_size = DOUBLE_SIZE_ENABLED() ? 2 : 1;

    if (canvas) {
	resize(window_width, window_height);
	frame_buffer_clear(&frame_buffer, pixel_table[0]);
	force_repaint();
        refresh_changed();
	refresh_all();
    }
}

void video_free(void)
{
    frame_buffer_free(&frame_buffer);
}

static void crtc_arrange_window(int width, int height)
{
    if(fullscreen) return;
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
	log_message(crtc_log, "CRTC: set cycles per line to %d.",
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

/* -------------------------------------------------------------------------- */

/* CRTC interface functions.
   FIXME: Several registers are not implemented.  */

void REGPARM2 store_crtc(ADDRESS addr, BYTE value)
{
    crtc[addr] = value;

    switch (addr) {
      case 0:			/* R00  Horizontal total (characters + 1) */
	new_crtc_cycles_per_line = crtc[0] + 1;

      case 1:			/* R01  Horizontal characters displayed */
	if (!crtc[1])
	    return;
	new_memptr_inc = crtc[1];
        if (hw_double_cols) {
            new_memptr_inc *= 2;
        }
	/* catch screens to large for our text cache */
	new_memptr_inc = crtc_min( SCREEN_MAX_TEXTCOLS, new_memptr_inc );
        break;

      case 2:			/* R02  Horizontal Sync Position */
	break;

      case 3:			/* R03  Horizontal/Vertical Sync widths */
	break;

      case 7:			/* R07  Vertical sync position */
	break;

      case 8:			/* R08  unused: Interlace and Skew */
	break;

      case 6:			/* R06  Number of display lines on screen */
        new_crtc_screen_textlines = crtc[6] & 0x7f;
        break;

      case 9:			/* R09  Rasters between two display lines */
	new_screen_charheight = crtc_min(16, crtc[9] + 1);
	break;

      case 4:			/* R04  Vertical total (character) rows */
        new_crtc_vertical_total = crtc[4] + 1;
	break;

      case 5:			/* R05  Vertical total line adjust */
        new_crtc_vertical_adjust = crtc[5];
	break;

      case 10:			/* R10  Cursor (not implemented on the PET) */
        crsrstart = value & 0x1f;
        value = ((value >> 5) & 0x03) ^ 0x01;
        if (crsr_enable && (crsrmode != value)) {
          crsrmode = value;
          crsrstate = 1;
          crsrcnt = 16;
	  crsr_set_dirty();
        }
	break;

      case 11:			/* R11  Cursor (not implemented on the PET) */
	crsr_set_dirty();
        crsrend = value & 0x1f;
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
    /* chargen_rel is computed for 8bytes/char, but charom is 16bytes/char */
    chargen_ptr = chargen_rom + (chargen_rel << 1);
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

    maincpu_set_alarm_clk(A_RASTERDRAW, CYCLES_PER_LINE);
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

    MEMORY_PTRS

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

    maincpu_set_alarm(A_RASTERDRAW, CYCLES_PER_LINE - offset);

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

int crtc_write_snapshot_module(snapshot_t *s)
{
    int i, ef = 0;
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (ef
	|| snapshot_module_write_byte(m, (BYTE) clk-rasterline_start_clk) < 0
        || snapshot_module_write_word(m, (WORD) rasterline) < 0
        || snapshot_module_write_word(m, (WORD) addr_mask) < 0
        || snapshot_module_write_byte(m, (BYTE) 
		((crsr_enable ? 1 : 0) | (hw_double_cols ? 2 : 0))) < 0
	) {
	ef = -1;
    }

    for (i = 0; (!ef) && (i < 20); i++)
        ef = snapshot_module_write_byte(m, crtc[i]);

    if (ef 
	|| snapshot_module_write_byte(m, crsrcnt + (crsrstate ? 0x80 : 0)) < 0
	) {
	ef = -1;
    }

    if (ef) {
        snapshot_module_close(m);
    } else {
    	ef = snapshot_module_close(m);
    }

    crtc_update_memory_ptrs();
    return ef;
}

int crtc_read_snapshot_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;
    WORD w;
    BYTE b;
    WORD vmask;
    BYTE hwflags;
    BYTE major, minor;

    m = snapshot_module_open(s, snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    if (major != SNAP_MAJOR) {
        log_error(crtc_log,
                  "Major snapshot number (%d) invalid; %d expected.",
                  major, SNAP_MAJOR);
        goto fail;
    }

    if (snapshot_module_read_byte(m, &b) < 0)
        goto fail;
    /* for the moment simply ignore this value */

    if (snapshot_module_read_word(m, &w) < 0)
        goto fail;
    /* for the moment simply ignore this value */

    if ( 0 
        || snapshot_module_read_word(m, &vmask) < 0
        || snapshot_module_read_byte(m, &hwflags))
        goto fail;

    crtc_set_screen_mode(NULL, vmask, memptr_inc, hwflags);
    crtc_update_memory_ptrs();

    for (i = 0; i < 20; i++) {
        if (snapshot_module_read_byte(m, &b) < 0)
            goto fail;

        /* XXX: This assumes that there are no side effects. 
	   Well, there are, but the cursor state is restored later */
        store_crtc(i, b);
    }

    if ( snapshot_module_read_byte(m, &b) < 0 ) goto fail;
    crsrcnt = b & 0x3f;
    crsrstate = (b & 0x80) ? 1 : 0;

    maincpu_set_alarm(A_RASTERDRAW, CYCLES_PER_LINE /* - RASTER_CYCLE*/);

    SIGNAL_VERT_BLANK_OFF

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
