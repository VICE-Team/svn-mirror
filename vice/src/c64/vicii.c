/*
 * vicii.c - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
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

/* A *big* thank goes to Andreas Boose (boose@rzgw.rz.fh-hannover.de) for
   helping me to find bugs and improve the emulation.  */

/* TODO: - speed optimizations;
         - faster sprites and registers. */

/*
   Current (most important) known limitations:

   - if we switch from display to idle state in the middle of one line, we
     only paint it completely in idle or display mode (we choose the most
     likely one, though);

   - sprite colors (and other attributes) cannot change in the middle of the
     rasterline;

   - changes of $D016 within one line are not always correctly handled;

   - g-accesses and c-accesses are not 100% emulated.

   Probably something else which I have not figured out yet...

 */

#define _VICII_C

#include "vice.h"

/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#ifndef __MSDOS__
#define NEED_2x
#else  /* __MSDOS__ */
#define pixel_width 1
#define pixel_height 1
#endif /* !__MSDOS__ */

#include "vicii.h"

#include "cmdline.h"
#include "interrupt.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "resources.h"
#include "utils.h"
#include "vmachine.h"

/* FIXME: ugliest thing ever. */
static void draw_sprites(void);
static void update_sprite_collisions(void);

#include "raster.h"
#include "sprites.h"
#include "sprcycles.h"
#include "sprcrunch.h"

#include <stdlib.h>
#include <stdio.h>

/* ------------------------------------------------------------------------- */

/* VIC-II palette.  */
static palette_t *palette;

/* VIC-II registers.  */
static int vic[64];

/* Interrupt register.  */
static int videoint = 0;

/* Line for raster compare IRQ.  */
static int int_raster_line = 0;
/* Clock value for raster compare IRQ. */
static CLOCK int_raster_clk;

/* Internal color memory.  */
static BYTE color_ram[0x400];

/* Video memory pointers.  */
static BYTE *screen_ptr;
static BYTE *chargen_ptr;
static BYTE *bitmap_ptr;
static BYTE * const color_ptr = color_ram;

/* Screen memory buffers (chars and color).  */
static BYTE vbuf[SCREEN_TEXTCOLS];
static BYTE cbuf[SCREEN_TEXTCOLS];

/* If this flag is set, bad lines (DMA's) can happen.  */
static int allow_bad_lines;

/* Sprite-sprite and sprite-background collision registers.  */
static BYTE ss_collmask = 0;
static BYTE sb_collmask = 0;

/* Extended background colors (1, 2 and 3).  */
static int ext_background_color[3];

/* Tick when int_rasterfetch() is called.  */
CLOCK vic_ii_fetch_clk;

/* Tick when int_rasterdraw() is called.  */
CLOCK vic_ii_draw_clk;

/* What do we do when the `A_RASTERFETCH' event happens?  */
static enum {
    FETCH_MATRIX,
    CHECK_SPRITE_DMA,
    FETCH_SPRITE
} fetch_idx = FETCH_MATRIX;

/* Flag: Check for ycounter reset already done on this line? (cycle 13) */
static int ycounter_reset_checked;

/* Flag: Does the currently selected video mode force the overscan background
   color to be black?  (This happens with the hires bitmap and illegal
   modes.)  */
static int force_black_overscan_background_color;

/* C128 MMU... unluckily, this is not used yet.  This can result in on-screen
   garbage when the VIC-II is being used.  */
#if defined (C128)
extern BYTE mmu[];
#endif

/* Light pen.  */
static struct {
    int triggered;
    int x, y;
} light_pen;

/* Start of the memory bank seen by the VIC-II.  */
static int vbank = 0;

/* Data to display in idle state.  */
static int idle_data;

/* Where do we currently fetch idle stata from?  If `IDLE_NONE', we are not
   in idle state and thus do not need to update `idle_data'.  */
static enum { IDLE_NONE, IDLE_3FFF, IDLE_39FF } idle_data_location;

/* ------------------------------------------------------------------------- */

/* VIC-II resources.  */

/* Flag: Do we emulate the sprite-sprite collision register and IRQ?  */
static int sprite_sprite_collisions_enabled;

/* Flag: Do we emulate the sprite-background collision register and IRQ?  */
static int sprite_background_collisions_enabled;

/* Name of palette file.  */
static char *palette_file_name;

/* Flag: Do we use double size?  */
static int double_size_enabled;

/* Flag: Do we enable the video cache?  */
static int video_cache_enabled;

/* Flag: Do we copy lines in double size mode?  */
static int double_scan_enabled;

static int set_sprite_sprite_collisions_enabled(resource_value_t v)
{
    sprite_sprite_collisions_enabled = (int) v;
    return 0;
}

static int set_sprite_background_collisions_enabled(resource_value_t v)
{
    sprite_background_collisions_enabled = (int) v;
    return 0;
}

static int set_video_cache_enabled(resource_value_t v)
{
    video_cache_enabled = (int) v;
    return 0;
}

static int set_palette_file_name(resource_value_t v)
{
#ifdef __MSDOS__
    if (palette == NULL)
        return 0;

    if (palette_load((char *) v, palette) < 0)
        return -1;
    canvas_set_palette(canvas, palette, pixel_table);

    /* Make sure the pixel tables are recalculated properly.  */
    video_resize();
#endif

    string_set(&palette_file_name, (char *) v);
    return 0;
}

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
    { "CheckSsColl", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &sprite_sprite_collisions_enabled, set_sprite_sprite_collisions_enabled },
    { "CheckSbColl", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &sprite_background_collisions_enabled, set_sprite_background_collisions_enabled },
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
    { "PaletteFile", RES_STRING, (resource_value_t) "default",
      (resource_value_t *) &palette_file_name, set_palette_file_name },
#ifdef NEED_2x
    { "DoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_size_enabled, set_double_size_enabled },
    { "DoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &double_scan_enabled, set_double_scan_enabled },
#endif
    { NULL }
};

int vic_ii_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* VIC-II command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-vcache", SET_RESOURCE, 0, NULL, NULL,
      "VideoCache", (resource_value_t) 1,
      NULL, "Enable the video cache" },
    { "+vcache", SET_RESOURCE, 0, NULL, NULL,
      "VideoCache", (resource_value_t) 0,
      NULL, "Disable the video cache" },
    { "-checksb", SET_RESOURCE, 0, NULL, NULL,
      "CheckSbColl", (resource_value_t) 1,
      NULL, "Enable sprite-background collision registers" },
    { "+checksb", SET_RESOURCE, 0, NULL, NULL,
      "CheckSbColl", (resource_value_t) 0,
      NULL, "Disable sprite-background collision registers" },
    { "-checkss", SET_RESOURCE, 0, NULL, NULL,
      "CheckSsColl", (resource_value_t) 1,
      NULL, "Enable sprite-sprite collision registers" },
    { "+checkss", SET_RESOURCE, 0, NULL, NULL,
      "CheckSsColl", (resource_value_t) 0,
      NULL, "Disable sprite-sprite collision registers" },
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

int vic_ii_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Debugging options. */
/* #define VIC_II_VMODE_DEBUG */
/* #define VIC_II_RASTER_DEBUG */
/* #define VIC_II_REGISTERS_DEBUG */

#ifdef VIC_II_VMODE_DEBUG
#define DEBUG_VMODE(x)		printf x
#else
#define DEBUG_VMODE(x)
#endif

#ifdef VIC_II_RASTER_DEBUG
#define DEBUG_RASTER(x) 	printf x
#else
#define DEBUG_RASTER(x)
#endif

#ifdef VIC_II_REGISTERS_DEBUG
#define DEBUG_REGISTER(x)	printf x
#else
#define DEBUG_REGISTER(x)
#endif

/* This is used for performance evaluation. */
#undef NO_REDRAW

/* ------------------------------------------------------------------------- */

/* These timings are taken from the ``VIC Article'' by Christian Bauer
   (bauec002@goofy.zdv.uni-mainz.de).  Thanks Christian!
   Note: we measure cycles from 0 to 62, not from 1 to 63 as he does. */

/* Cycle # at which the VIC takes the bus in a bad line (BA goes low).  */
#define	FETCH_CYCLE		11

/* Cycle # at which sprite DMA is set.  */
#define SPRITE_FETCH_CYCLE	54

/* Cycle # at which the current raster line is re-drawn.  It is set to
   `CYCLES_PER_LINE', so this actually happens at the very beginning
   (i.e. cycle 0) of the next line.  */
#define DRAW_CYCLE		CYCLES_PER_LINE

/* Delay for the raster line interrupt.  This is not due to the VIC-II, since
   it triggers the IRQ line at the beginning of the line, but to the 6510 that
   needs at least 2 cycles to detect it.  */
#define RASTER_INT_DELAY	2

/* Current char being drawn by the raster.  < 0 or >= SCREEN_TEXTCOLS if
   outside the visible range.  */
#define RASTER_CHAR(cycle)	((cycle) - 15)

/* Current horizontal position (in pixels) of the raster.  < 0 or >=
   SCREEN_WIDTH if outside the visible range. */
#define RASTER_X(cycle)		(((cycle) - 13) * 8)

/* Current vertical position of the raster.  Unlike `rasterline', which is
   only accurate if a pending `A_RASTERDRAW' event has been served, this is
   guarranteed to be always correct.  It is a bit slow, though.  */
#define RASTER_Y    	((int)(clk / CYCLES_PER_LINE) % SCREEN_HEIGHT)

/* Cycle # within the current line.  */
#define RASTER_CYCLE	((int)(clk % CYCLES_PER_LINE))

/* `clk' value for the beginning of the current line.  */
#define LINE_START_CLK	((clk / CYCLES_PER_LINE) * CYCLES_PER_LINE)

/* # of the previous and next raster line.  Handles wrap over.  */
#define PREVIOUS_LINE(line)   (((line) > 0) \
			       ? (line) - 1 : VIC_II_SCREEN_HEIGHT - 1)
#define NEXT_LINE(line)	      (((line) + 1) % VIC_II_SCREEN_HEIGHT)

/* Bad line range.  */
#define FIRST_DMA_LINE	0x30
#define LAST_DMA_LINE   0xf7

#include "raster.c"

static void init_drawing_tables(void);

/* ------------------------------------------------------------------------- */

/* Set the video mode according to the values in registers $D011 and $D016 of
   the VIC-II chip. */
inline static void set_video_mode(int cycle)
{
    static int old_video_mode = -1;
    int new_video_mode;

    new_video_mode = ((vic[0x11] & 0x60) | (vic[0x16] & 0x10)) >> 4;

    if (new_video_mode != old_video_mode) {
	if (new_video_mode == VIC_II_HIRES_BITMAP_MODE
	    || VIC_II_IS_ILLEGAL_MODE(new_video_mode)) {
	    /* Force the overscan color to black.  */
	    if (overscan_background_color != 0)
		add_int_change_background(RASTER_X(cycle),
					  &overscan_background_color,
					  0);
	    force_black_overscan_background_color = 1;
	} else {
	    /* The overscan background color is given by the background color
               register.  */
	    if (overscan_background_color != vic[0x21])
		add_int_change_background(RASTER_X(cycle),
					  &overscan_background_color,
					  vic[0x21]);
	    force_black_overscan_background_color = 0;
	}

        {
            int pos = RASTER_CHAR(cycle);

            add_int_change_foreground(pos, &video_mode, new_video_mode);

            if (idle_data_location != IDLE_NONE) {
                if (vic[0x11] & 0x40)
                    add_int_change_foreground(pos, (void *) &idle_data,
                                              ram[vbank + 0x39ff]);
                else
                    add_int_change_foreground(pos, (void *) &idle_data,
                                              ram[vbank + 0x3fff]);
            }
        }

	old_video_mode = new_video_mode;
   }

#ifdef VIC_II_VMODE_DEBUG
    switch (new_video_mode) {
      case VIC_II_NORMAL_TEXT_MODE:
	DEBUG_VMODE(("Standard Text"));
	break;
      case VIC_II_MULTICOLOR_TEXT_MODE:
	DEBUG_VMODE(("Multicolor Text"));
	break;
      case VIC_II_HIRES_BITMAP_MODE:
	DEBUG_VMODE(("Hires Bitmap"));
	break;
      case VIC_II_MULTICOLOR_BITMAP_MODE:
	DEBUG_VMODE(("Multicolor Bitmap"));
	break;
      case VIC_II_EXTENDED_TEXT_MODE:
	DEBUG_VMODE(("Extended Text"));
	break;
      case VIC_II_ILLEGAL_TEXT_MODE:
	DEBUG_VMODE(("Illegal Text"));
	break;
      case VIC_II_ILLEGAL_BITMAP_MODE_1:
	DEBUG_VMODE(("Invalid Bitmap"));
	break;
      case VIC_II_ILLEGAL_BITMAP_MODE_2:
	DEBUG_VMODE(("Invalid Bitmap"));
	break;
      default:			/* cannot happen */
	DEBUG_VMODE(("???"));
    }

    DEBUG_VMODE((" Mode enabled at line $%04X, cycle %d.\n", RASTER_Y, cycle));
#endif
}

/* Set the memory pointers according to the values in the registers. */
static void set_memory_ptrs(int cycle)
{
    static BYTE *old_screen_ptr, *old_bitmap_ptr, *old_chargen_ptr;
    static int old_vbank = -1;
    ADDRESS scraddr;		/* Screen start address. */
    BYTE *screenbase;		/* Pointer to screen memory. */
    BYTE *charbase;		/* Pointer to character memory. */
    BYTE *bitmapbase;		/* Pointer to bitmap memory. */
    int tmp;

    scraddr = vbank + ((vic[0x18] & 0xf0) << 6);

    if ((scraddr & 0x7000) != 0x1000) {
	screenbase = ram + scraddr;
	DEBUG_REGISTER(("\tVideo memory at $%04X\n", scraddr));
    } else {
	screenbase = chargen_rom + (scraddr & 0x800);
	DEBUG_REGISTER(("\tVideo memory at Character ROM + $%04X\n",
			scraddr & 0x800));
    }

    tmp = (vic[0x18] & 0xe) << 10;
    bitmapbase = ram + (tmp & 0xe000);
    tmp += vbank;

    DEBUG_REGISTER(("\tBitmap memory at $%04X\n", bitmapbase - ram + vbank));

    if ((tmp & 0x7000) != 0x1000) {
	charbase = ram + tmp;
	DEBUG_REGISTER(("\tUser-defined character set at $%04X\n", tmp));
    } else {
	charbase = chargen_rom + (tmp & 0x0800);
	DEBUG_REGISTER(("\tStandard %s character set enabled\n",
			tmp & 0x800 ? "Lower Case" : "Upper Case"));
    }

    tmp = RASTER_CHAR(cycle);

    if (idle_data_location != IDLE_NONE && old_vbank != vbank) {
        if (idle_data_location == IDLE_39FF)
            add_int_change_foreground(RASTER_CHAR(cycle), &idle_data,
                                      ram[vbank + 0x39ff]);
        else
            add_int_change_foreground(RASTER_CHAR(cycle), &idle_data,
                                      ram[vbank + 0x3fff]);
    }

    if (skip_next_frame || (tmp <= 0 && clk < vic_ii_draw_clk)) {
        old_screen_ptr = screen_ptr = screenbase;
        old_bitmap_ptr = bitmap_ptr = bitmapbase + vbank;
        old_chargen_ptr = chargen_ptr = charbase;
	old_vbank = vbank;
        vbank_ptr = ram + vbank;
        sprite_ptr_base = screenbase + 0x3f8;
    } else if (tmp < SCREEN_TEXTCOLS) {
	if (screenbase != old_screen_ptr) {
	    add_ptr_change_foreground(tmp, (void **)&screen_ptr,
				      (void *)screenbase);
	    add_ptr_change_foreground(tmp, (void **)&sprite_ptr_base,
				      (void *)(screenbase + 0x3f8));
	    old_screen_ptr = screenbase;
	}
	if (bitmapbase + vbank != old_bitmap_ptr) {
	    add_ptr_change_foreground(tmp, (void **)&bitmap_ptr,
				      (void *)(bitmapbase + vbank));
	    old_bitmap_ptr = bitmapbase + vbank;
	}
	if (charbase != old_chargen_ptr) {
	    add_ptr_change_foreground(tmp, (void **)&chargen_ptr,
				      (void *)charbase);
	    old_chargen_ptr = charbase;
	}
	if (vbank != old_vbank) {
	    add_ptr_change_foreground(tmp, (void **)&vbank_ptr,
				      (void *)(ram + vbank));
	    old_vbank = vbank;
	}
    } else {
	if (screenbase != old_screen_ptr) {
	    add_ptr_change_next_line((void **)&screen_ptr,
				     (void *)screenbase);
	    add_ptr_change_next_line((void **)&sprite_ptr_base,
				     (void *)(screenbase + 0x3f8));
	    old_screen_ptr = screenbase;
	}
	if (bitmapbase + vbank != old_bitmap_ptr) {
	    add_ptr_change_next_line((void **)&bitmap_ptr,
				     (void *)(bitmapbase + vbank));
	    old_bitmap_ptr = bitmapbase + vbank;
	}
	if (charbase != old_chargen_ptr) {
	    add_ptr_change_next_line((void **)&chargen_ptr,
				     (void *)charbase);
	    old_chargen_ptr = charbase;
	}
	if (vbank != old_vbank) {
	    add_ptr_change_next_line((void **)&vbank_ptr,
				     (void *)(ram + vbank));
	    old_vbank = vbank;
	}
    }
}

/* Calculate the value of clk when int_raster() should be called next time. */
inline static void update_int_raster(void)
{
    if (int_raster_line < VIC_II_SCREEN_HEIGHT) {
	int current_line = RASTER_Y;

	int_raster_clk = (LINE_START_CLK + RASTER_INT_DELAY - INTERRUPT_DELAY
			  + CYCLES_PER_LINE * (int_raster_line
					       - current_line));

	/* Raster interrupts on line 0 are delayed by 1 cycle.  */
	if (int_raster_line == 0)
	    int_raster_clk++;

	if (int_raster_line <= current_line)
	    int_raster_clk += VIC_II_SCREEN_HEIGHT * CYCLES_PER_LINE;
	maincpu_set_alarm_clk(A_RASTER, int_raster_clk);
    } else {
	DEBUG_RASTER(("VIC: update_int_raster(): "
		      "raster compare out of range ($%04X)!\n",
		      int_raster_line));
	maincpu_unset_alarm(A_RASTER);
    }

    DEBUG_RASTER(("VIC: update_int_raster(): "
		  "int_raster_clk = %ul, line = $%04X, vic[0x1a]&1 = %d\n",
		  int_raster_clk, int_raster_line, vic[0x1a] & 1));
}

/* Initialize the VIC-II emulation. */
canvas_t vic_ii_init(void)
{
    static const char *color_names[VIC_II_NUM_COLORS] = {
        "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue",
        "Yellow", "Orange", "Brown", "Light Red", "Dark Gray", "Medium Gray",
        "Light Green", "Light Blue", "Light Gray"
    };
    char title[256];

#ifdef NEED_2x
    init_raster(1, 2, 2);
#else
    init_raster(1, 1, 1);
#endif

    video_resize();

    palette = palette_create(VIC_II_NUM_COLORS, color_names);
    if (palette == NULL)
        return NULL;

    if (palette_load(palette_file_name, palette) < 0) {
        printf("Cannot load palette file `%s'.\n", palette_file_name);
        return NULL;
    }

    sprintf(title, "VICE: %s emulator", machine_name);
    if (open_output_window(title,
			   SCREEN_XPIX + SCREEN_BORDERWIDTH * 2,
			   (VIC_II_LAST_DISPLAYED_LINE
			    - VIC_II_FIRST_DISPLAYED_LINE),
			   palette,
			   ((canvas_redraw_t)vic_ii_exposure_handler))) {
	fprintf(stderr,
		"fatal error: can't open window for the VIC-II emulation.\n");
	return NULL;
    }
    display_ystart = VIC_II_25ROW_START_LINE;
    display_ystop = VIC_II_25ROW_STOP_LINE;
    set_video_mode(0);
    set_memory_ptrs(0);
    init_drawing_tables();
    refresh_all();
    return canvas;
}

/* Reset the VIC-II chip. */
void reset_vic_ii(void)
{
    reset_raster();

    light_pen.triggered = light_pen.x = light_pen.y = 0;

    vic_ii_draw_clk = DRAW_CYCLE;
    maincpu_set_alarm_clk(A_RASTERDRAW, vic_ii_draw_clk);

    vic_ii_fetch_clk = FETCH_CYCLE;
    maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
    fetch_idx = FETCH_MATRIX;

    /* FIXME: I am not sure this is exact emulation.  */
    int_raster_line = 0;
    int_raster_clk = 0;
    maincpu_set_alarm_clk(A_RASTER, 1);
    /* Remove all the IRQ sources.  */
    vic[0x1a] = 0;
}

/* This hook is called whenever video bank must be changed.  */
void vic_ii_set_vbank(int num_vbank)
{
    /* Warning: assumes it's called within a memory write access.  */
    /* Also, we assume the bank has *really* changed, and do not do any
       special optimizations for the not-really-changed case.  */
    vic_ii_handle_pending_alarms(rmw_flag + 1);
    if (clk >= vic_ii_draw_clk)
	int_rasterdraw(clk - vic_ii_draw_clk);
    vbank = num_vbank << 14;
    set_memory_ptrs(RASTER_CYCLE);
}

/* Trigger the light pen. */
void vic_ii_trigger_light_pen(CLOCK mclk)
{
    if (!light_pen.triggered) {
	light_pen.triggered = 1;
	light_pen.x = RASTER_X(mclk % CYCLES_PER_LINE);
	if (light_pen.x < 0)
	    light_pen.x = SCREEN_SPRITE_WRAP_X + light_pen.x;
	/* FIXME: why `+2'? */
	light_pen.x = light_pen.x / 2 + 2;
	light_pen.y = (mclk / CYCLES_PER_LINE) % SCREEN_HEIGHT;
	videoint |= 0x8;
	if (vic[0x1a] & 0x8) {
	    videoint |= 0x80;
	    maincpu_set_irq_clk(I_RASTER, 1, mclk);
	}
    }
}

/* Handle the exposure event. */
void vic_ii_exposure_handler(unsigned int width, unsigned int height)
{
    resize(width, height);
    force_repaint();
}

/* Free the allocated frame buffer. */
void video_free(void)
{
    frame_buffer_free(&frame_buffer);
#ifdef C128
    vdc_video_free();
#endif
}

/* ------------------------------------------------------------------------- */

/* Emulate a matrix line fetch, `num' bytes starting from `offs'.  This takes
   care of the 10-bit counter wraparound.  */
inline static void fetch_matrix(int offs, int num)
{
    BYTE *p = ram + vbank + ((vic[0x18] & 0xf0) << 6);
    int start_char = (mem_counter + offs) & 0x3ff;
    int c = 0x3ff - start_char + 1;

    if (c >= num) {
	memcpy(vbuf + offs, p + start_char, num);
	memcpy(cbuf + offs, color_ptr + start_char, num);
    } else {
	memcpy(vbuf + offs, p + start_char, c);
	memcpy(vbuf + offs + c, p, num - c);
	memcpy(cbuf + offs, color_ptr + start_char, c);
	memcpy(cbuf + offs + c, color_ptr, num - c);
    }
}

/* Here we try to emulate $D011...  */
inline static void store_d011(BYTE value)
{
    int r = int_raster_line;
    int cycle = RASTER_CYCLE;
    int line = RASTER_Y;

    DEBUG_REGISTER(("\tControl register: $%02X\n", value));
    DEBUG_REGISTER(("$D011 trickery at cycle %d, line $%04X, value $%02X\n",
		    cycle, line, value));

    int_raster_line = ((int_raster_line & 0xff) | ((value & 0x80) << 1));

    DEBUG_REGISTER(("\tRaster interrupt line set to $%04X\n",
		    int_raster_line));

    if (int_raster_line != r)
	update_int_raster();

    /* This is the funniest part... handle bad line tricks. */

    if (line == FIRST_DMA_LINE && (value & 0x10) != 0)
	allow_bad_lines = 1;

    if (ysmooth != (value & 7)
	&& line >= FIRST_DMA_LINE
	&& line <= LAST_DMA_LINE) {

	int was_bad_line, now_bad_line;

	/* Check whether bad line state has changed.  */
	was_bad_line = (allow_bad_lines && (ysmooth == (line & 7)));
	now_bad_line = (allow_bad_lines && ((value & 7) == (line & 7)));

	if (was_bad_line && !now_bad_line) {

	    /* Bad line becomes good.  */
	    bad_line = 0;

	    /* By changing the values in the registers, one can make the VIC
	       switch from idle to display state, but not from display to
	       idle state.  So we are always in display state if this
	       happens.  This is only true if the value changes in some
	       cycle > 0, though; otherwise, the line never becomes bad.  */
	    if (cycle > 0) {
		draw_idle_state = idle_state = 0;
                idle_data_location = IDLE_NONE;
		if (cycle > FETCH_CYCLE + 2 && !ycounter_reset_checked) {
		    ycounter = 0;
		    ycounter_reset_checked = 1;
		}
	    }

	} else if (!was_bad_line && now_bad_line) {

	    if (cycle >= FETCH_CYCLE
	        && cycle <= FETCH_CYCLE + SCREEN_TEXTCOLS + 3) {

		int pos;        /* Value of line counter when this happens. */
		int inc;        /* Total increment for the line counter. */
		int num_chars;	/* Total number of characters to fetch. */
		int num_0xff_fetches;	/* Number of 0xff fetches to do. */

		bad_line = 1;

		if (cycle <= FETCH_CYCLE + 2)
		    ycounter = 0;

		ycounter_reset_checked = 1;

		num_chars = SCREEN_TEXTCOLS - (cycle - (FETCH_CYCLE + 3));

		if (num_chars <= SCREEN_TEXTCOLS) {

		    /* Matrix fetches starts immediately, but the VIC needs
                       at least 3 cycles to become the bus master.  Before
                       this happens, it fetches 0xff. */
		    num_0xff_fetches = 3;

		    /* If we were in idle state before creating the bad
		       line, the counters have not been incremented. */

		    if (idle_state) {
			pos = 0;
			inc = num_chars;
			if (inc < 0)
			    inc = 0;
		    } else {
			pos = cycle - (FETCH_CYCLE + 3);
			if (pos > SCREEN_TEXTCOLS - 1)
			    pos = SCREEN_TEXTCOLS - 1;
			inc = SCREEN_TEXTCOLS;
		    }

		} else {
		    pos = 0;
		    num_chars = inc = SCREEN_TEXTCOLS;
		    num_0xff_fetches = cycle - FETCH_CYCLE;
		}

		/* This is normally done at cycle `FETCH_CYCLE + 2'. */
		mem_counter = memptr;

		/* Force the DMA. */
		/* Note that `cbuf' should be loaded from the value of the
		   next opcode when the VIC-II is not the bus master yet, but
		   we force 0xf instead. */
		if (num_chars <= num_0xff_fetches) {
		    memset(vbuf + pos, 0xff, num_chars);
		    memset(cbuf + pos, ram[reg_pc] & 0xf, num_chars);
		} else {
		    memset(vbuf + pos, 0xff, num_0xff_fetches);
		    memset(cbuf + pos, ram[reg_pc] & 0xf, num_0xff_fetches);
		    fetch_matrix(pos + num_0xff_fetches,
				 num_chars - num_0xff_fetches);
		}

		/* Set the value by which `mem_counter' is incremented on
                   this line. */
		mem_counter_inc = inc;

		/* Take over the bus until the memory fetch is done. */
		clk = LINE_START_CLK + FETCH_CYCLE + SCREEN_TEXTCOLS + 3;

		/* Remember we have done a DMA. */
		memory_fetch_done = 2;

	        /* As we are on a bad line, switch to display state. */
	        idle_state = 0;

	        /* Try to display things correctly.  This is not exact,
	           but should be OK for most cases (FIXME?). */
		if (inc == SCREEN_TEXTCOLS) {
	            draw_idle_state = 0;
                    idle_data_location = IDLE_NONE;
                }

	    } else if (cycle <= FETCH_CYCLE + SCREEN_TEXTCOLS + 6) {

		/* Bad line has been generated after fetch interval, but
                   before ycounter is incremented. */

		bad_line = 1;

		/* If in idle state, counter is not incremented. */
		if (idle_state)
		    mem_counter_inc = 0;

		/* We are not in idle state anymore. */
		/* This is not 100% correct, but should be OK for most cases.
                   (FIXME?)  */
		draw_idle_state = idle_state = 0;
                idle_data_location = IDLE_NONE;

	    } else {

		/* Line is now bad, so we must switch to display state.
                   Anyway, we cannot do it here as the `ycounter' handling
                   must happen in as in idle state. */
		force_display_state = 1;

	    }
	}
    }

    ysmooth = value & 0x7;

    /* Check for 24 <-> 25 line mode switch.  */
    if ((value ^ vic[0x11]) & 8) {
	if (value & 0x8) {

	    /* 24 -> 25 row mode switch.  */

	    display_ystart = VIC_II_25ROW_START_LINE;
	    display_ystop = VIC_II_25ROW_STOP_LINE;

	    if (line == VIC_II_24ROW_STOP_LINE && cycle > 0) {
	        /* If on the first line of the 24-line border, we
                   still see the 25-line (lowmost) border because the
                   border flip flop has already been turned on.  */
		blank_enabled = 1;
            } else if (!blank && line == VIC_II_24ROW_START_LINE
                       && cycle > 0) {
                /* A 24 -> 25 switch somewhere on the first line of
                   the 24-row mode is enough to disable screen
                   blanking. */
                blank_enabled = 0;
            }

	    DEBUG_REGISTER(("\t25 line mode enabled\n"));

	} else {

	    /* 25 -> 24 row mode switch.  */

	    display_ystart = VIC_II_24ROW_START_LINE;
	    display_ystop = VIC_II_24ROW_STOP_LINE;

	    /* If on the last line of the 25-line border, we still see the
               24-line (upmost) border because the border flip flop has
               already been turned off.  */
	    if (!blank && line == VIC_II_25ROW_START_LINE && cycle > 0) {
		blank_enabled = 0;
            } else if (line == VIC_II_25ROW_STOP_LINE && cycle > 0) {
                blank_enabled = 1;
            }

	    DEBUG_REGISTER(("\t24 line mode enabled\n"));

	}
    }

    blank = !(value & 0x10);	/* `DEN' bit.  */

    vic[0x11] = value;

    /* FIXME: save time. */
    set_video_mode(cycle);
}

/* ------------------------------------------------------------------------- */

/* Store a value in the video bank (it is assumed to be in RAM).  */
inline void REGPARM2 store_vbank(ADDRESS addr, BYTE value)
{
    /* This can only cause "aesthetical" errors, so let's save some time if
       the current frame will not be visible.  */
    if (!skip_next_frame) {
	/* Argh... this is a dirty kludge!  We should probably find a cleaner
	   solution.  */
	int f;
	CLOCK mclk;

	/* WARNING: Assumes `rmw_flag' is 0 or 1. */
	mclk = clk - rmw_flag - 1;
	do {
	    f = 0;
	    if (mclk >= vic_ii_fetch_clk) {
		int_rasterfetch(clk - vic_ii_fetch_clk);
		f = 1;
		/* WARNING: Assumes `rmw_flag' is 0 or 1. */
		mclk = clk - rmw_flag - 1;
	    }
	    if (mclk >= vic_ii_draw_clk) {
		int_rasterdraw(0);
		f = 1;
	    }
	} while (f);
    }

    ram[addr] = value;
}

/* As `store_vbank()', but for the $3900...$39FF address range.  */
void REGPARM2 store_vbank_39xx(ADDRESS addr, BYTE value)
{
    store_vbank(addr, value);
    if (idle_data_location == IDLE_39FF && (addr & 0x3fff) == 0x39ff)
        add_int_change_foreground(RASTER_CHAR(RASTER_CYCLE), &idle_data,
                                  value);
}

/* As `store_vbank()', but for the $3F00...$3FFF address range.  */
void REGPARM2 store_vbank_3fxx(ADDRESS addr, BYTE value)
{
    store_vbank(addr, value);
    if (idle_data_location == IDLE_3FFF && (addr & 0x3fff) == 0x3fff)
        add_int_change_foreground(RASTER_CHAR(RASTER_CYCLE), &idle_data,
                                  value);
}

/* Helper function for `store_vic()': set the X coordinate of the `num'th
   sprite to `new_x'; the current raster X position is `raster_x'.  */
static void set_sprite_x(int num, int new_x, int raster_x)
{
    new_x += 8;

    if (new_x > SCREEN_SPRITE_WRAP_X - SCREEN_MAX_SPRITE_WIDTH) {
	/* Sprites in the $1F8 - $1FF range are not visible at all
	   and never cause collisions.  */
	if (new_x >= 0x1f8 + 8)
	    new_x = SCREEN_WIDTH;
	else
	    new_x -= SCREEN_SPRITE_WRAP_X;
    }

    if (new_x < sprites[num].x) {
	if (raster_x <= new_x)
	    sprites[num].x = new_x;
	else if (raster_x < sprites[num].x)
	    sprites[num].x = SCREEN_WIDTH;
	add_int_change_next_line(&sprites[num].x, new_x);
    } else {			/* new_x >= sprites[num].x */
	if (raster_x < sprites[num].x)
	    sprites[num].x = new_x;
	add_int_change_next_line(&sprites[num].x, new_x);
    }
}

/* Enable DMA for sprite `num'.  */
inline static void turn_sprite_dma_on(int num)
{
    new_dma_msk |= 1 << num;
    sprites[num].dma_flag = 1;
    sprites[num].memptr = 0;
    sprites[num].exp_flag = sprites[num].y_expanded ? 0 : 1;
    sprites[num].memptr_inc = sprites[num].exp_flag ? 3 : 0;
}

/* Store a value in a VIC-II register. */
void REGPARM2 store_vic(ADDRESS addr, BYTE value)
{
    addr &= 0x3f;

    /* WARNING: assumes `rmw_flag' is 0 or 1. */
    vic_ii_handle_pending_alarms(rmw_flag + 1);

    /* This is necessary as we must be sure that the previous line has been
       updated and `rasterline' is actually set to the current Y position of
       the raster.  Otherwise we might mix the changes for this line with the
       changes for the previous one. */
    if (clk >= vic_ii_draw_clk)
	int_rasterdraw(clk - vic_ii_draw_clk);

    DEBUG_REGISTER(("VIC: WRITE $D0%02X at cycle %d of rasterline $%04X\n",
		    addr, RASTER_CYCLE, RASTER_Y));

    switch (addr) {
      case 0x0:		/* $D000: Sprite #0 X position LSB */
      case 0x2:		/* $D002: Sprite #1 X position LSB */
      case 0x4:		/* $D004: Sprite #2 X position LSB */
      case 0x6:		/* $D006: Sprite #3 X position LSB */
      case 0x8:		/* $D008: Sprite #4 X position LSB */
      case 0xa:		/* $D00a: Sprite #5 X position LSB */
      case 0xc:		/* $D00c: Sprite #6 X position LSB */
      case 0xe:		/* $D00e: Sprite #7 X position LSB */
	if (value != vic[addr]) {
	    int n = addr >> 1;	/* Number of changed sprite. */
	    int new_x;

	    vic[addr] = value;

	    new_x = (value | (vic[0x10] & (1 << n) ? 0x100 : 0));
	    set_sprite_x(n, new_x, RASTER_X(RASTER_CYCLE));

	    DEBUG_REGISTER(("\tSprite #%d X position LSB: $%02X\n", n, value));
	}
	break;

      case 0x1:		/* $D001: Sprite #0 Y position */
      case 0x3:		/* $D003: Sprite #1 Y position */
      case 0x5:		/* $D005: Sprite #2 Y position */
      case 0x7:		/* $D007: Sprite #3 Y position */
      case 0x9:		/* $D009: Sprite #4 Y position */
      case 0xb:		/* $D00B: Sprite #5 Y position */
      case 0xd:		/* $D00D: Sprite #6 Y position */
      case 0xf:		/* $D00F: Sprite #7 Y position */
	if (vic[addr] != value) {
	    int cycle = RASTER_CYCLE;

	    if (cycle == SPRITE_FETCH_CYCLE + 1
		&& value == (rasterline & 0xff)) {
		fetch_idx = CHECK_SPRITE_DMA;
		vic_ii_fetch_clk = LINE_START_CLK + SPRITE_FETCH_CYCLE + 1;
		maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
	    }
	    sprites[addr >> 1].y = value;
	    vic[addr] = value;
	    DEBUG_REGISTER(("\tSprite #%d Y position: $%02X\n",,
			    addr >> 1, value));
	}
	break;

      case 0x10:		/* $D010: Sprite X position MSB */
	if (value != vic[0x10]) {
	    int i;
	    BYTE b;
	    int raster_x = RASTER_X(RASTER_CYCLE);

	    vic[0x10] = value;

	    /* Recalculate the sprite X coordinates. */
	    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
		int new_x;

		new_x = (vic[2 * i] | (value & b ? 0x100 : 0));
		set_sprite_x(i, new_x, raster_x);
	    }
	}
	DEBUG_REGISTER(("\tSprite X position MSBs: $%02X\n", value));
	return;

      case 0x11:		/* $D011: video mode, Y scroll, 24/25 line mode
				   and raster MSB */
	store_d011(value);
	return;

      case 0x12:		/* $D012: Raster line compare */
	if (value != vic[0x12])	{
	    int r = int_raster_line;
	    int line = RASTER_Y;

	    vic[0x12] = value;
	    DEBUG_REGISTER(("\tRaster compare register: $%02X\n", value));
	    int_raster_line = (r & 0x100) | value;
	    DEBUG_REGISTER(("\tRaster interrupt line set to $%04X\n",
			    int_raster_line));
	    if (r != int_raster_line)
		update_int_raster();

	    /* Check whether we should activate the IRQ line now.  */
	    if (vic[0x1a] & 0x1) {
		int trigger_irq = 0;

		if (rmw_flag) {
		    if (RASTER_CYCLE == 0) {
                        int previous_line = PREVIOUS_LINE(line);

			if (previous_line != r
                            && (r & 0x100) == (previous_line & 0x100))
			    trigger_irq = 1;
		    } else if (line != r && (r & 0x100) == (line & 0x100))
			trigger_irq = 1;
		}
		if (int_raster_line == line && line != r)
		    trigger_irq = 1;
		if (trigger_irq) {
		    videoint |= 0x81;
		    maincpu_set_irq(I_RASTER, 1);
		}
	    }
	}
        return;

      case 0x13:		/* $D013: Light Pen X */
      case 0x14:		/* $D014: Light Pen Y */
	return;

      case 0x15:		/* $D015: Sprite Enable */
	{
	    int cycle = RASTER_CYCLE;

	    /* On the real C64, sprite DMA is checked two times: first at
	       `SPRITE_FETCH_CYCLE', and then at `SPRITE_FETCH_CYCLE + 1'.
	       In the average case, one DMA check is OK and there is no need
	       to emulate both, but we have to kludge things a bit in case
	       sprites are activated at cycle `SPRITE_FETCH_CYCLE + 1'.  */
	    if (cycle == SPRITE_FETCH_CYCLE + 1
		&& ((value ^ vic[0x15]) & value) != 0) {
		fetch_idx = CHECK_SPRITE_DMA;
		vic_ii_fetch_clk = LINE_START_CLK + SPRITE_FETCH_CYCLE + 1;
		maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
	    }

	    /* Sprites are turned on: force a DMA check.  */
	    if (visible_sprite_msk == 0 && dma_msk == 0 && value != 0) {
		if ((fetch_idx == FETCH_MATRIX
		     && vic_ii_fetch_clk > clk
		     && cycle > FETCH_CYCLE
		     && cycle <= SPRITE_FETCH_CYCLE)
		    || rasterline < FIRST_DMA_LINE
		    || rasterline > LAST_DMA_LINE) {
                    CLOCK new_fetch_clk;

		    new_fetch_clk = LINE_START_CLK + SPRITE_FETCH_CYCLE;
		    if (cycle > SPRITE_FETCH_CYCLE)
			new_fetch_clk += CYCLES_PER_LINE;
                    if (new_fetch_clk < vic_ii_fetch_clk) {
                        fetch_idx = CHECK_SPRITE_DMA;
                        vic_ii_fetch_clk = new_fetch_clk;
                        maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
                    }
		}
	    }
	}

	vic[0x15] = visible_sprite_msk = value;
	DEBUG_REGISTER(("\tSprite Enable register: $%02X\n", value));
	return;

      case 0x16:		/* $D016 */
	{
	    int cycle = RASTER_CYCLE;

	    DEBUG_REGISTER(("\tControl register: $%02X\n", value));

	    /* FIXME: Line-based emulation!  */
	    if ((value & 7) != (vic[0x16] & 7)) {
#if 1
		if (skip_next_frame || RASTER_CHAR(cycle) <= 0)
		    xsmooth = value & 0x7;
		else
		    add_int_change_next_line(&xsmooth, value & 0x7);
#else
                add_int_change_foreground(RASTER_CHAR(cycle), &xsmooth,
                                          value & 7);
#endif /* 0 */
	    }

	    /* Bit 4 (CSEL) selects 38/40 column mode.  */
	    if ((value & 0x8) != (vic[0x16] & 0x8)) {
		if (value & 0x8) {
		    /* 40 column mode.  */
		    if (cycle <= 17)
			display_xstart = VIC_II_40COL_START_PIXEL;
		    else
			add_int_change_next_line(&display_xstart,
						 VIC_II_40COL_START_PIXEL);
		    if (cycle <= 56)
			display_xstop = VIC_II_40COL_STOP_PIXEL;
		    else
			add_int_change_next_line(&display_xstop,
                                                 VIC_II_40COL_STOP_PIXEL);
		    DEBUG_REGISTER(("\t40 column mode enabled\n"));

		    /* If CSEL changes from 0 to 1 at cycle 17, the border is
		       not turned off and this line is blank.  */
                    if (cycle == 17 && !(vic[0x16] & 0x8))
                        blank_this_line = 1;
		} else {
		    /* 38 column mode.  */
		    if (cycle <= 17)
			display_xstart = VIC_II_38COL_START_PIXEL;
		    else
			add_int_change_next_line(&display_xstart,
						 VIC_II_38COL_START_PIXEL);
		    if (cycle <= 56)
			display_xstop = VIC_II_38COL_STOP_PIXEL;
		    else
			add_int_change_next_line(&display_xstop,
						 VIC_II_38COL_STOP_PIXEL);
		    DEBUG_REGISTER(("\t38 column mode enabled\n"));

		    /* If CSEL changes from 1 to 0 at cycle 56, the lateral
		       border is open.  */
		    if (cycle == 56 && (vic[0x16] & 0x8)
                        && (!blank_enabled || open_left_border))
			open_right_border = 1;
		}
	    }

	    vic[0x16] = value;
	    set_video_mode(cycle);
	    return;
	}

      case 0x17:		/* $D017: Sprite Y-expand */
	if (value != vic[0x17]) {
	    int cycle = RASTER_CYCLE;
	    int i;
	    BYTE b;

	    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
		sprites[i].y_expanded = value & b ? 1 : 0;

		if (!sprites[i].y_expanded && !sprites[i].exp_flag) {

                    /* Sprite crunch!  */
		    if (cycle == 15)
			sprites[i].memptr_inc = sprite_crunch_table[sprites[i].memptr];
		    else if (cycle < 15)
			sprites[i].memptr_inc = 3;
		    sprites[i].exp_flag = 1;

		}

		/* (Enabling sprite Y-expansion never causes side effects.)  */

	    }

	    vic[0x17] = value;
	}
	DEBUG_REGISTER(("\tSprite Y Expand register: $%02X\n", value));
	return;

      case 0x18:		/* $D018: Video and char matrix base address */
	DEBUG_REGISTER(("\tMemory register: $%02X\n", value));
	if (vic[0x18] != value) {
	    vic[0x18] = value;
	    set_memory_ptrs(RASTER_CYCLE);
	}
	return;

      case 0x19:		/* $D019: IRQ flag register */
	if (rmw_flag) {		/* (emulates the Read-Modify-Write bug) */
	    videoint = 0;
	} else {
	    videoint &= ~((value & 0xf) | 0x80);
	    if (videoint & vic[0x1a])
		videoint |= 0x80;
	}

	/* Update the IRQ line accordingly...
	   The external VIC IRQ line is an AND of the internal collision and
	   raster IRQ lines. */
	if (videoint & 0x80) {
	    maincpu_set_irq(I_RASTER, 1);
	} else {
	    maincpu_set_irq(I_RASTER, 0);
	}

	DEBUG_REGISTER(("\tIRQ flag register: $%02X\n", videoint));
	return;

      case 0x1a:		/* $D01A: IRQ mask register */
	vic[0x1a] = value & 0xf;

	if (vic[0x1a] & videoint) {
	    videoint |= 0x80;
	    maincpu_set_irq(I_RASTER, 1);
	} else {
	    videoint &= 0x7f;
	    maincpu_set_irq(I_RASTER, 0);
	}

	DEBUG_REGISTER(("\tIRQ mask register: $%02X\n", vic[0x1a]));
	return;

      case 0x1b:		/* $D01B: Sprite priority */
	if (value != vic[0x1b]) {
	    int i;
	    BYTE b;
	    int raster_x = RASTER_X(RASTER_CYCLE);

	    vic[0x1b] = value;
	    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
		if (sprites[i].x < raster_x)
		    add_int_change_next_line(&sprites[i].in_background,
					     value & b ? 1 : 0);
		else
		    sprites[i].in_background = value & b ? 1 : 0;
	    }
	    DEBUG_REGISTER(("\tSprite priority register: $%02X\n", value));
	}
	return;

      case 0x1c:		/* $D01C: Sprite Multicolor select */
	if (value != vic[0x1c]) {
	    int i;
	    BYTE b;
	    int raster_x = RASTER_X(RASTER_CYCLE);

	    vic[0x1c] = value;
	    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
		if (sprites[i].x < raster_x)
		    add_int_change_next_line(&sprites[i].multicolor,
					     value & b ? 1 : 0);
		else
		    sprites[i].multicolor = value & b ? 1 : 0;
	    }
	}
	DEBUG_REGISTER(("\tSprite Multicolor Enable register: $%02X\n",
			value));
	return;

      case 0x1d:		/* $D01D: Sprite X-expand */
	if (value != vic[0x1d]) {
            int raster_x = RASTER_X(RASTER_CYCLE);
	    int i;
	    BYTE b;

	    vic[0x1d] = value;
	    /* FIXME: how is this handled in the middle of one line?  */
	    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
                if (1 || raster_x < sprites[i].x) {
                    sprites[i].x_expanded = value & b ? 1 : 0;
                } else {
                    /* FIXME */  /* We are in trouble! */
                }
            }
	}
	DEBUG_REGISTER(("\tSprite X Expand register: $%02X\n", value));
	return;

      case 0x1e:		/* $D01E: Sprite-sprite collision */
      case 0x1f:		/* $D01F: Sprite-background collision */
	DEBUG_REGISTER(("\t(collision register, Read Only)\n"));
	return;

      case 0x20:		/* $D020: Border color */
        value &= 0xf;
	if (vic[0x20] != value) {
	    vic[0x20] = value;
	    add_int_change_border(RASTER_X(RASTER_CYCLE),
				  &border_color, vic[0x20]);
   	}
	DEBUG_REGISTER(("\tBorder color register: $%02X\n", value));
	return;

      case 0x21:		/* $D021: Background #0 color */
      	value &= 0xf;
        if (vic[0x21] != value) {
  	    vic[0x21] = value;

	    if (!force_black_overscan_background_color)
		add_int_change_background(RASTER_X(RASTER_CYCLE),
					  &overscan_background_color,
					  value);

	    add_int_change_background(RASTER_X(RASTER_CYCLE),
				      &background_color,
				      value);
	}
	DEBUG_REGISTER(("\tBackground #0 color register: $%02X\n", value));
	return;

      case 0x22:		/* $D022: Background #1 color */
      case 0x23:		/* $D023: Background #2 color */
      case 0x24:		/* $D024: Background #3 color */
	value &= 0xf;
	if (vic[addr] != value) {
	    vic[addr] = value;
	    add_int_change_foreground(RASTER_CHAR(RASTER_CYCLE),
				      &ext_background_color[addr - 0x22],
				      value);
	}
	DEBUG_REGISTER(("\tBackground color #%d register: $%02X\n",
			addr - 0x21, value));
	return;

      case 0x25:		/* $D025: Sprite multicolor register #0 */
	value &= 0xf;
	if (vic[0x25] != value) {
	    vic[0x25] = value;
	    /* FIXME: this is approximated. */
	    if (RASTER_CYCLE > CYCLES_PER_LINE / 2)
		add_int_change_next_line(&mc_sprite_color_1, value);
	    else
		mc_sprite_color_1 = value;
	}
	DEBUG_REGISTER(("\tSprite multicolor register #0: $%02X\n", value));
	return;

      case 0x26:		/* $D026: Sprite multicolor register #1 */
	value &= 0xf;
	if (vic[0x26] != value) {
	    vic[0x26] = value;
	    /* FIXME: this is approximated. */
	    if (RASTER_CYCLE > CYCLES_PER_LINE / 2)
		add_int_change_next_line(&mc_sprite_color_2, value);
	    else
		mc_sprite_color_2 = value;
	}
	DEBUG_REGISTER(("\tSprite multicolor register #1: $%02X\n", value));
	return;

      case 0x27:		/* $D027: Sprite #0 color */
      case 0x28:		/* $D028: Sprite #1 color */
      case 0x29:		/* $D029: Sprite #2 color */
      case 0x2a:		/* $D02A: Sprite #3 color */
      case 0x2b:		/* $D02B: Sprite #4 color */
      case 0x2c:		/* $D02C: Sprite #5 color */
      case 0x2d:		/* $D02D: Sprite #6 color */
      case 0x2e:		/* $D02E: Sprite #7 color */
	value &= 0xf;
	if (vic[addr] != value) {
	    int n = addr - 0x27;

	    vic[addr] = value;

	    if (sprites[n].x < RASTER_X(RASTER_CYCLE))
		add_int_change_next_line(&(sprites[n].color), value);
	    else
		sprites[n].color = value;
	}
	DEBUG_REGISTER(("\tSprite #%d color register: $%02X\n",
			addr - 0x27, value));
	return;

      case 0x2f:		/* $D02F: Unused */
      case 0x30:		/* $D030: Unused */
      case 0x31:		/* $D031: Unused */
      case 0x32:		/* $D032: Unused */
      case 0x33:		/* $D033: Unused */
      case 0x34:		/* $D034: Unused */
      case 0x35:		/* $D035: Unused */
      case 0x36:		/* $D036: Unused */
      case 0x37:		/* $D037: Unused */
      case 0x38:		/* $D038: Unused */
      case 0x39:		/* $D039: Unused */
      case 0x3a:		/* $D03A: Unused */
      case 0x3b:		/* $D03B: Unused */
      case 0x3c:		/* $D03C: Unused */
      case 0x3d:		/* $D03D: Unused */
      case 0x3e:		/* $D03E: Unused */
      case 0x3f:		/* $D03F: Unused */
	DEBUG_REGISTER(("\t(unused)\n"));
	return;
    }
}

/* Read a value from a VIC-II register. */
BYTE REGPARM1 read_vic(ADDRESS addr)
{
    addr &= 0x3f;

    /* Serve all pending events. */
    vic_ii_handle_pending_alarms(0);

    DEBUG_REGISTER(("VIC: READ $D0%02X at cycle %d of rasterline $%04X:\n",
		    addr, RASTER_CYCLE, RASTER_Y));

    switch (addr) {
      case 0x0:		/* $D000: Sprite #0 X position LSB */
      case 0x2:		/* $D002: Sprite #1 X position LSB */
      case 0x4:		/* $D004: Sprite #2 X position LSB */
      case 0x6:		/* $D006: Sprite #3 X position LSB */
      case 0x8:		/* $D008: Sprite #4 X position LSB */
      case 0xa:		/* $D00a: Sprite #5 X position LSB */
      case 0xc:		/* $D00c: Sprite #6 X position LSB */
      case 0xe:		/* $D00e: Sprite #7 X position LSB */
	DEBUG_REGISTER(("\tSprite #%d X position LSB: $%02X\n",
			addr >> 1, vic[addr]));
	return vic[addr];

      case 0x1:		/* $D001: Sprite #0 Y position */
      case 0x3:		/* $D003: Sprite #1 Y position */
      case 0x5:		/* $D005: Sprite #2 Y position */
      case 0x7:		/* $D007: Sprite #3 Y position */
      case 0x9:		/* $D009: Sprite #4 Y position */
      case 0xb:		/* $D00B: Sprite #5 Y position */
      case 0xd:		/* $D00D: Sprite #6 Y position */
      case 0xf:		/* $D00F: Sprite #7 Y position */
	DEBUG_REGISTER(("\tSprite #%d Y position: $%02X\n", addr >> 1,
			vic[addr]));
	return vic[addr];

      case 0x10:		/* $D010: Sprite X position MSB */
	DEBUG_REGISTER(("\tSprite X position MSB: $%02X\n", vic[addr]));
	return vic[addr];

      case 0x11:		/* $D011: video mode, Y scroll, 24/25 line mode
				   and raster MSB */
      case 0x12:		/* $D012: Raster line compare */
	{
	    int tmp = RASTER_Y;

	    DEBUG_REGISTER(("\tRaster Line register %s value = $%04X\n",
			    (addr == 0x11 ? "(highest bit) " : ""), tmp));

	    /* Line 0 is 62 cycles long, while line (SCREEN_HEIGHT - 1) is 64
               cycles long.  As a result, the counter is incremented one
               cycle later on line 0.  */
	    if (tmp == 0 && RASTER_CYCLE == 0)
		tmp = SCREEN_HEIGHT - 1;

	    if (addr == 0x11)
		return (vic[addr] & 0x7f) | ((tmp & 0x100) >> 1);
	    else
		return tmp & 0xff;
	}

      case 0x13:		/* $D013: Light Pen X */
	DEBUG_REGISTER(("\tLight pen X: %d\n", light_pen.x));
	return light_pen.x;

      case 0x14:		/* $D014: Light Pen Y */
	DEBUG_REGISTER(("\tLight pen Y: %d\n", light_pen.y));
	return light_pen.y;

      case 0x15:		/* $D015: Sprite Enable */
	DEBUG_REGISTER(("\tSprite Enable register: $%02X\n", vic[addr]));
	return vic[addr];

      case 0x16:		/* $D016 */
	DEBUG_REGISTER(("\t$D016 Control register read: $%02X\n", vic[addr]));
	return vic[addr] | 0xc0;

      case 0x17:		/* $D017: Sprite Y-expand */
	DEBUG_REGISTER(("\tSprite Y Expand register: $%02X\n", vic[addr]));
	return vic[addr];

      case 0x18:		/* $D018: Video and char matrix base address */
	DEBUG_REGISTER(("\tVideo memory address register: $%02X\n",
			vic[addr]));
	return vic[addr] | 0x1;

      case 0x19:		/* $D019: IRQ flag register */
	DEBUG_RASTER(("VIC: read interrupt register: $%02X\n",
		      videoint | 0x70));
	DEBUG_REGISTER(("\tInterrupt register: $%02X\n", videoint | 0x70));
	if (RASTER_Y == int_raster_line && (vic[0x1a] & 0x1))
	    /* As int_raster() is called 2 cycles later than it should be to
	       emulate the 6510 internal IRQ delay, videoint might not have
	       bit 0 set as it should. */
	    return videoint | 0x71;
	else
	    return videoint | 0x70;

      case 0x1a:		/* $D01A: IRQ mask register  */
	DEBUG_REGISTER(("\tMask register: $%02X\n", vic[addr] | 0xf0));
	return vic[addr] | 0xf0;

      case 0x1b:		/* $D01B: Sprite priority */
	DEBUG_REGISTER(("\tSprite Priority register: $%02X\n", vic[addr]));
	return vic[addr];

      case 0x1c:		/* $D01C: Sprite Multicolor select */
	DEBUG_REGISTER(("\tSprite Multicolor Enable register: $%02X\n",
			vic[addr]));
	return vic[addr];

      case 0x1d:		/* $D01D: Sprite X-expand */
	DEBUG_REGISTER(("\tSprite X Expand register: $%02X\n", vic[addr]));
	return vic[addr];

      case 0x1e:		/* $D01E: Sprite-sprite collision */
	/* Remove the pending sprite-sprite interrupt, as the collision
	   register is reset upon read accesses. */
	if (!(videoint & 0x3)) {
	    videoint &= ~0x84;
	    maincpu_set_irq(I_RASTER, 0);
	} else {
	    videoint &= ~0x04;
	}
	if (sprite_sprite_collisions_enabled) {
	    vic[addr] = ss_collmask;
	    ss_collmask = 0;
	    DEBUG_REGISTER(("\tSprite-sprite collision mask: $%02X\n",
			    vic[addr]));
	    return vic[addr];
	} else {
	    DEBUG_REGISTER(("\tSprite-sprite collision mask: $00 "
			    "(emulation disabled)\n"));
	    ss_collmask = 0;
	    return 0;
	}

      case 0x1f:		/* $D01F: Sprite-background collision */
	/* Remove the pending sprite-background interrupt, as the collision
	   register is reset upon read accesses. */
	if (!(videoint & 0x5)) {
	    videoint &= ~0x82;
	    maincpu_set_irq(I_RASTER, 0);
	} else {
	    videoint &= ~0x2;
	}
	if (sprite_background_collisions_enabled) {
	    vic[addr] = sb_collmask;
	    sb_collmask = 0;
	    DEBUG_REGISTER(("\tSprite-background collision mask: $%02X\n",
			    vic[addr]));
#if defined (DEBUG_SB_COLLISIONS)
	    printf("VIC: sb_collmask reset by $D01F read at line 0x%X.\n",
		   RASTER_Y);
#endif
	    return vic[addr];
	} else {
	    DEBUG_REGISTER(("\tSprite-background collision mask: $00 "
			    "(emulation disabled)\n"));
	    sb_collmask = 0;
	    return 0;
	}

      case 0x20:		/* $D020: Border color */
	DEBUG_REGISTER(("\tBorder Color register: $%02X\n", vic[addr]));
	return vic[addr] | 0xf0;

      case 0x21:		/* $D021: Background #0 color */
      case 0x22:		/* $D022: Background #1 color */
      case 0x23:		/* $D023: Background #2 color */
      case 0x24:		/* $D024: Background #3 color */
	DEBUG_REGISTER(("\tBackground Color #%d register: $%02X\n",
			addr - 0x21, vic[addr]));
	return vic[addr] | 0xf0;

      case 0x25:		/* $D025: Sprite multicolor register #0 */
      case 0x26:		/* $D026: Sprite multicolor register #1 */
	DEBUG_REGISTER(("\tMulticolor register #%d: $%02X\n",
			addr - 0x22, vic[addr]));
	return vic[addr] | 0xf0;

      case 0x27:		/* $D027: Sprite #0 color */
      case 0x28:		/* $D028: Sprite #1 color */
      case 0x29:		/* $D029: Sprite #2 color */
      case 0x2a:		/* $D02A: Sprite #3 color */
      case 0x2b:		/* $D02B: Sprite #4 color */
      case 0x2c:		/* $D02C: Sprite #5 color */
      case 0x2d:		/* $D02D: Sprite #6 color */
      case 0x2e:		/* $D02E: Sprite #7 color */
	DEBUG_REGISTER(("\tSprite #%d color: $%02X\n",
			addr - 0x22, vic[addr]));
	return vic[addr] | 0xf0;

#if defined (C128)
      case 0x2f:		/* $D02F: Additional KBD rows */
	return (vic[0x2f] | 0xf8);
#else
      case 0x2f:		/* $D02F: Unused */
#endif

      case 0x30:		/* $D030: Unused */
      case 0x31:		/* $D031: Unused */
      case 0x32:		/* $D032: Unused */
      case 0x33:		/* $D033: Unused */
      case 0x34:		/* $D034: Unused */
      case 0x35:		/* $D035: Unused */
      case 0x36:		/* $D036: Unused */
      case 0x37:		/* $D037: Unused */
      case 0x38:		/* $D038: Unused */
      case 0x39:		/* $D039: Unused */
      case 0x3a:		/* $D03A: Unused */
      case 0x3b:		/* $D03B: Unused */
      case 0x3c:		/* $D03C: Unused */
      case 0x3d:		/* $D03D: Unused */
      case 0x3e:		/* $D03E: Unused */
      case 0x3f:		/* $D03F: Unused */
	return 0xff;

      default:
	return 0xff;
    }
}

void REGPARM2 store_colorram(ADDRESS addr, BYTE value)
{
    color_ram[addr & 0x3ff] = value & 0xf;
}

BYTE REGPARM1 read_colorram(ADDRESS addr)
{
    return color_ram[addr & 0x3ff] | (rand() & 0xf0);
}

/* ------------------------------------------------------------------------- */

/* If we are on a bad line, do the DMA.  Return nonzero if cycles have been
   stolen.  */
inline static int do_memory_fetch(CLOCK sub)
{
    if (!memory_fetch_done) {
	memory_fetch_done = 1;
	mem_counter = memptr;

 	if ((rasterline & 7) == ysmooth
 	    && allow_bad_lines
 	    && rasterline >= FIRST_DMA_LINE
 	    && rasterline <= LAST_DMA_LINE) {

	    fetch_matrix(0, SCREEN_TEXTCOLS);
	    draw_idle_state = idle_state = ycounter = 0;
            idle_data_location = IDLE_NONE;
	    ycounter_reset_checked = 1;
	    memory_fetch_done = 2;
            maincpu_steal_cycles(vic_ii_fetch_clk, SCREEN_TEXTCOLS + 3 - sub);

	    bad_line = 1;
	    return 1;
	}
    }

    return 0;
}

/* Check for sprite DMA. */
inline static void check_sprite_dma(void)
{
    int i, b;

    if (!visible_sprite_msk && !dma_msk)
	return;

    new_dma_msk = dma_msk;
    for (i = 0, b = 1; i < SCREEN_NUM_SPRITES; i++, b <<= 1) {
	if ((visible_sprite_msk & b)
	    && sprites[i].y == (rasterline & 0xff)) {
            turn_sprite_dma_on(i);
	} else if (sprites[i].dma_flag) {
	    sprites[i].memptr = ((sprites[i].memptr + sprites[i].memptr_inc)
	    			 & 0x3f);
	    if (sprites[i].y_expanded)
	        sprites[i].exp_flag = !sprites[i].exp_flag;
	    sprites[i].memptr_inc = sprites[i].exp_flag ? 3 : 0;
	    if (sprites[i].memptr == 63) {
		sprites[i].dma_flag = 0;
		new_dma_msk &= ~b;
                if ((visible_sprite_msk & b)
                    && sprites[i].y == (rasterline & 0xff))
                    turn_sprite_dma_on(i);
	    }
	}
    }
}

/* ------------------------------------------------------------------------- */

/* Redraw the current raster line.  This happens at cycle DRAW_CYCLE of each
   line. */
int int_rasterdraw(long offset)
{
#ifndef NO_REDRAW

    BYTE prev_ss_collmask = ss_collmask, prev_sb_collmask = sb_collmask;

    emulate_line();

    ycounter_reset_checked = 0;

    /* As explained in Christian's article, only the first collision (i.e. the
       first time the collision register becomes non-zero) actually triggers an
       interrupt.  */
    if (sprite_sprite_collisions_enabled
        && cl_ss_collmask && !prev_ss_collmask) {
	videoint |= 0x4;
	if (vic[0x1a] & 0x4) {
	    maincpu_set_irq(I_RASTER, 1);
	    videoint |= 0x80;
	}
    }
    if (sprite_background_collisions_enabled
        && cl_sb_collmask && !prev_sb_collmask) {
	videoint |= 0x2;
	if (vic[0x1a] & 0x2) {
	    maincpu_set_irq(I_RASTER, 1);
	    videoint |= 0x80;
	}
    }

#else  /* NO_REDRAW */

    rasterline = (rasterline + 1) % SCREEN_HEIGHT;
    oldclk += CYCLES_PER_LINE;

#endif

    if (idle_state) {
        idle_data_location = (vic[0x11] & 0x40) ? IDLE_39FF : IDLE_3FFF;
        if (idle_data_location == IDLE_39FF)
            idle_data = ram[vbank + 0x39ff];
        else
            idle_data = ram[vbank + 0x3fff];
    } else {
        idle_data_location = IDLE_NONE;
    }

    /* Set the next RASTERDRAW event. */
    vic_ii_draw_clk = oldclk + DRAW_CYCLE;
    maincpu_set_alarm_clk(A_RASTERDRAW, vic_ii_draw_clk);

    if (rasterline == 0)
	light_pen.triggered = 0;

    return 0;
}

/* Handle sprite/matrix fetch events.  FIXME: could be made slightly
   faster. */
int int_rasterfetch(long offset)
{
    static CLOCK sprite_fetch_clk = 0;
    static int sprite_fetch_idx = 0;
    static int fetch_msk;
    CLOCK last_opcode_first_write_clk, last_opcode_last_write_clk;

    /* This kludgy thing is used to emulate the behavior of the 6510 when BA
       goes low.  When BA goes low, every read access stops the processor
       until BA is high again; write accesses happen as usual instead.  */

    if (offset > 0) {
	switch (OPINFO_NUMBER(last_opcode_info)) {
	  case 0:
	    /* In BRK, IRQ and NMI the 3rd, 4th and 5th cycles are write
	       accesses, while the 1st, 2nd, 6th and 7th are read accesses.  */
            last_opcode_first_write_clk = clk - 5;
            last_opcode_last_write_clk = clk - 3;
	    break;
	  case 0x20:
	    /* In JSR, the 4th and 5th cycles are write accesses, while the
	       1st, 2nd, 3rd and 6th are read accesses.  */
            last_opcode_first_write_clk = clk - 3;
            last_opcode_last_write_clk = clk - 2;
	    break;
	  default:
	    /* In all the other opcodes, all the write accesses are the last
	       ones.  */
            if (maincpu_num_write_cycles() != 0) {
                last_opcode_last_write_clk = clk - 1;
                last_opcode_first_write_clk = clk - maincpu_num_write_cycles();
            } else {
                last_opcode_first_write_clk = (CLOCK) 0;
                last_opcode_last_write_clk = last_opcode_first_write_clk;
            }
	    break;
	}
    } else {			/* offset <= 0, i.e. offset == 0 */
        /* If we are called with no offset, we don't have to care about write
           accesses.  */
	last_opcode_first_write_clk = last_opcode_last_write_clk = 0;
    }

    while (1) {
        CLOCK sub;

        if (vic_ii_fetch_clk < last_opcode_first_write_clk
            || vic_ii_fetch_clk > last_opcode_last_write_clk)
            sub = 0;
        else
            sub = last_opcode_last_write_clk - vic_ii_fetch_clk + 1;

	switch (fetch_idx) {

	  case FETCH_MATRIX:

	    if (visible_sprite_msk == 0 && dma_msk == 0) {

		do_memory_fetch(sub);

		/* As sprites are all turned off, there is no need for a
                   sprite DMA check; next time we will FETCH_MATRIX again.
                   This works because a CHECK_SPRITE_DMA is forced in
                   `store_vic()' whenever the mask becomes nonzero. */

		/* This makes sure we only create FETCH_MATRIX events in the
                   bad line range.  These checks are (a little) redundant for
                   safety. */
		if (rasterline < FIRST_DMA_LINE) {
		    vic_ii_fetch_clk += ((FIRST_DMA_LINE - rasterline)
					 * CYCLES_PER_LINE);
		} else if (rasterline >= LAST_DMA_LINE) {
		    vic_ii_fetch_clk += ((SCREEN_HEIGHT - rasterline
					  + FIRST_DMA_LINE) * CYCLES_PER_LINE);
		} else
		    vic_ii_fetch_clk += CYCLES_PER_LINE;

		maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
		return 0;

	    } else {	 /* visible_sprite_msk != 0 || dma_msk != 0 */

		int memory_fetch_done = do_memory_fetch(sub);

		/* Sprites might be turned on, check for sprite DMA next
                   time. */
		fetch_idx = CHECK_SPRITE_DMA;

		/* Calculate time for next event. */
		vic_ii_fetch_clk = LINE_START_CLK + SPRITE_FETCH_CYCLE;

		if (vic_ii_fetch_clk > clk || offset == 0) {
		    /* Prepare the next fetch event. */
		    maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
		    return 0;
		}

                if (memory_fetch_done && sub == 0) {
                    last_opcode_first_write_clk += SCREEN_TEXTCOLS + 3;
                    last_opcode_last_write_clk += SCREEN_TEXTCOLS + 3;
                }
	    }
            break;

	  case CHECK_SPRITE_DMA:

	    /* Swap sprite data buffers. */
	    {
		DWORD *tmp;
		tmp = sprite_data;
		sprite_data = new_sprite_data;
		new_sprite_data = tmp;
	    }

	    check_sprite_dma();

            /* FIXME?  Slow!  */
	    sprite_fetch_clk = LINE_START_CLK + SPRITE_FETCH_CYCLE;

	    fetch_msk = new_dma_msk;

	    if (sprite_fetch_tab[fetch_msk][0].cycle == -1) {
		if (rasterline >= FIRST_DMA_LINE - 1
		    && rasterline <= LAST_DMA_LINE + 1) {
		    fetch_idx = FETCH_MATRIX;
		    vic_ii_fetch_clk = (sprite_fetch_clk - SPRITE_FETCH_CYCLE
				        + FETCH_CYCLE + CYCLES_PER_LINE);
		} else {
		    fetch_idx = CHECK_SPRITE_DMA;
		    vic_ii_fetch_clk = sprite_fetch_clk + CYCLES_PER_LINE;
		}
	    } else {
		/* Next time, fetch sprite data. */
		fetch_idx = FETCH_SPRITE;
		sprite_fetch_idx = 0;
		vic_ii_fetch_clk = (sprite_fetch_clk
				    + sprite_fetch_tab[fetch_msk][0].cycle);
	    }

            if (vic_ii_fetch_clk > clk || offset == 0) {
                maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
                return 0;
            }

            break;

	  case FETCH_SPRITE:

	    {
		int next_cycle;
		int i;
		struct sprite_fetch *sf;

		/* FIXME: optimize. */

		sf = &sprite_fetch_tab[fetch_msk][sprite_fetch_idx];

		if (!skip_next_frame) {
		    BYTE *bank = ram + vbank;
		    BYTE *spr_base = (bank + 0x3f8 +
				      + ((vic[0x18] & 0xf0) << 6)
				      + sf->first);

		    /* Fetch sprite data.  */
		    for (i = sf->first; i <= sf->last; i++, spr_base++) {
			BYTE *src = bank + (*spr_base << 6);
			BYTE *dest = (BYTE *)(new_sprite_data + i);
			int memptr = sprites[i].memptr;

			dest[0] = src[memptr];
			dest[1] = src[++memptr & 0x3f];
			dest[2] = src[++memptr & 0x3f];
		    }
		}

		maincpu_steal_cycles(vic_ii_fetch_clk, sf->num - sub);

                if (sub == 0) {
                    last_opcode_first_write_clk += sf->num;
                    last_opcode_last_write_clk += sf->num;
                }

		next_cycle = (sf + 1)->cycle;
		sprite_fetch_idx++;

		if (next_cycle == -1) {
		    /* Next time, handle bad lines. */
		    if (rasterline >= FIRST_DMA_LINE - 1
			&& rasterline <= LAST_DMA_LINE + 1) {
			fetch_idx = FETCH_MATRIX;
			vic_ii_fetch_clk = (sprite_fetch_clk
					    - SPRITE_FETCH_CYCLE + FETCH_CYCLE
					    + CYCLES_PER_LINE);
		    } else {
			fetch_idx = CHECK_SPRITE_DMA;
			vic_ii_fetch_clk = sprite_fetch_clk + CYCLES_PER_LINE;
		    }
		} else {
		    vic_ii_fetch_clk = sprite_fetch_clk + next_cycle;
		}

		if (clk >= vic_ii_draw_clk)
		    int_rasterdraw(clk - vic_ii_draw_clk);

                if (vic_ii_fetch_clk > clk || offset == 0) {
		    maincpu_set_alarm_clk(A_RASTERFETCH, vic_ii_fetch_clk);
		    return 0;
                }

                if (clk >= int_raster_clk)
                    int_raster(clk - int_raster_clk);
	    }

	    break;

	} /* switch (fetch_idx) */

    } /* while (1) */

    return 0;
}

/* If necessary, emulate a raster compare IRQ. This is called when the raster
   line counter matches the value stored in the raster line register. */
int int_raster(long offset)
{
    videoint |= 0x1;
    if (vic[0x1a] & 0x1) {
	maincpu_set_irq_clk(I_RASTER, 1, int_raster_clk);
	videoint |= 0x80;
	DEBUG_RASTER(("VIC: *** IRQ requested at line $%04X, "
		      "int_raster_line=$%04X, offset = %ld, cycle = %d.\n",
		      RASTER_Y, int_raster_line, offset,
		      RASTER_CYCLE));
    }
    int_raster_clk += VIC_II_SCREEN_HEIGHT * CYCLES_PER_LINE;
    maincpu_set_alarm_clk(A_RASTER, int_raster_clk);
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Here comes the part that actually repaints each raster line and checks for
   sprite collisions.  The following tables are used to speed up the
   drawing. */
/* We do not use multi-dimensional arrays as we can optimize better this
   way... */

/* foreground(4) | background(4) | nibble(4) -> 4 pixels. */
static PIXEL4 hr_table[16 * 16 * 16];

#ifdef NEED_2x
/* foreground(4) | background(4) | idx(2) | nibble(4) -> 4 pixels. */
static PIXEL4 hr_table_2x[16 * 16 * 2 * 16];
#endif

/* mc flag(1) | idx(2) | byte(8) -> index into double-pixel table. */
static WORD mc_table[2 * 4 * 256];

/* Sprite tables. */
static DWORD sprite_doubling_table[65536];
static WORD mcmsktable[512];
static BYTE mcsprtable[256];

/* This is a bit mask representing each pixel on the screen (1 = foreground,
   0 = background) and is used both for sprite-background collision checking
   and background sprite drawing.  When cache is turned on, a cached mask for
   each line is used instead. */
static BYTE gfx_msk[GFXMSK_SIZE];

/* Each byte in this array is a bit mask representing the sprites that
   have a pixel turned on in that position.  This is used for sprite-sprite
   collision checking. */
static BYTE sprline[SCREEN_WIDTH + 2 * SCREEN_MAX_SPRITE_WIDTH];

/* Initialize the drawing tables. */
static void init_drawing_tables(void)
{
    DWORD i;
    unsigned int f, b;
    WORD wmsk;
    DWORD lmsk;
    char tmptable[4] = { 0, 4, 5, 3 };

    for (i = 0; i <= 0xf; i++) {
	for (f = 0; f <= 0xf; f++) {
	    for (b = 0; b <= 0xf; b++) {
		PIXEL fp = PIXEL(f);
		PIXEL bp = PIXEL(b);
		int offset = (f << 8) | (b << 4);
		PIXEL *p = (PIXEL *)(hr_table + offset + i);

		*p = i & 0x8 ? fp : bp;
		*(p + 1) = i & 0x4 ? fp : bp;
		*(p + 2) = i & 0x2 ? fp : bp;
		*(p + 3) = i & 0x1 ? fp : bp;

#ifdef NEED_2x
		p = (PIXEL *)(hr_table_2x + (offset << 1) + i);
		*p = *(p + 1) = i & 0x8 ? fp : bp;
		*(p + 2) = *(p + 3) = i & 0x4 ? fp : bp;
		*(p + 0x40) = *(p + 0x41) = i & 0x2 ? fp : bp;
		*(p + 0x42) = *(p + 0x43) = i & 0x1 ? fp : bp;
#endif
	    }
	}
    }

    for (i = 0; i <= 0xff; i++) {
	mc_table[i + 0x100] = i >> 6;
	mc_table[i + 0x300] = (i >> 4) & 0x3;
	mc_table[i + 0x500] = (i >> 2) & 0x3;
	mc_table[i + 0x700] = i & 0x3;
	mc_table[i] = tmptable[i >> 6];
	mc_table[i + 0x200] = tmptable[(i >> 4) & 0x3];
	mc_table[i + 0x400] = tmptable[(i >> 2) & 0x3];
	mc_table[i + 0x600] = tmptable[i & 0x3];
	mcsprtable[i] = ((i & 0xc0 ? 0xc0 : 0) | (i & 0x30 ? 0x30 : 0)
			 | (i & 0x0c ? 0x0c : 0) | (i & 0x03 ? 0x03 : 0));
	mcmsktable[i + 0x100] = 0;
	mcmsktable[i + 0x100] |= ((i >> 6) & 0x2) ? 0xc0 : 0;
	mcmsktable[i + 0x100] |= ((i >> 4) & 0x2) ? 0x30 : 0;
	mcmsktable[i + 0x100] |= ((i >> 2) & 0x2) ? 0x0c : 0;
	mcmsktable[i + 0x100] |= (i & 0x2) ? 0x03 : 0;
	mcmsktable[i] = i;
    }

    for (i = 0; i <= 0xffff; i++) {
	sprite_doubling_table[i] = 0;
	for (lmsk = 0xc0000000, wmsk = 0x8000; wmsk; lmsk >>= 2, wmsk >>= 1)
	    if (i & wmsk)
		sprite_doubling_table[i] |= lmsk;
    }
}

/* Draw one hires sprite. */
inline static void draw_hires_sprite(PIXEL *line_ptr,
				     BYTE *gfx_msk_ptr,
				     int n,
				     int double_size)
{
    if (sprites[n].x < SCREEN_WIDTH) {
	DWORD sprmsk, collmsk;
	BYTE *msk_ptr = gfx_msk_ptr + ((sprites[n].x + SCREEN_MAX_SPRITE_WIDTH
					- xsmooth) / 8);
	BYTE *sptr = sprline + SCREEN_MAX_SPRITE_WIDTH + sprites[n].x;
	PIXEL *ptr = line_ptr + sprites[n].x * ((double_size) ? 2 : 1);
	BYTE *data_ptr = (BYTE *)(sprite_data + n);
	int lshift = (sprites[n].x - xsmooth) & 0x7;
	int in_background = sprites[n].in_background;

	if (sprites[n].x_expanded) {
	    WORD sbit = 0x101 << n;
	    WORD cmsk = 0;

	    collmsk = ((((msk_ptr[1] << 24) | (msk_ptr[2] << 16)
			 | (msk_ptr[3] << 8) | msk_ptr[4]) << lshift)
		       | (msk_ptr[5] >> (8 - lshift)));
	    sprmsk = sprite_doubling_table[(data_ptr[0] << 8)
					   | data_ptr[1]];
	    cmsk = 0;
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, collmsk, 32, sbit, ptr, sptr,
				   sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, collmsk, 32, sbit, ptr, sptr,
				sprites[n].color, cmsk);
	    } else {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, 0, 32, sbit, ptr, sptr,
				   sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, 0, 32, sbit, ptr, sptr,
				sprites[n].color, cmsk);
	    }
	    sprmsk = sprite_doubling_table[data_ptr[2]];
	    collmsk = ((((msk_ptr[5] << 8) | msk_ptr[6]) << lshift)
		       | (msk_ptr[7] >> (8 - lshift)));
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, collmsk, 16, sbit, ptr + 64,
				   sptr + 32, sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, collmsk, 16, sbit, ptr + 32,
				sptr + 32, sprites[n].color, cmsk);
	    } else {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, 0, 16, sbit, ptr + 64,
				   sptr + 32, sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, 0, 16, sbit, ptr + 32,
				sptr + 32, sprites[n].color, cmsk);
	    }
	    if (cmsk)
		cl_ss_collmask |= (cmsk >> 8) | ((cmsk | sbit) & 0xff);
	} else {		/* Unexpanded */
	    BYTE sbit = 1 << n;
	    BYTE cmsk = 0;

	    collmsk = ((((msk_ptr[0] << 24) | (msk_ptr[1] << 16)
			 | (msk_ptr[2] << 8) | msk_ptr[3]) << lshift)
		       | (msk_ptr[4] >> (8 - lshift)));
	    sprmsk = (data_ptr[0] << 16) | (data_ptr[1] << 8) | data_ptr[2];
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, collmsk, 24, sbit, ptr, sptr,
				   sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, collmsk, 24, sbit, ptr, sptr,
				sprites[n].color, cmsk);
	    } else {
		if (double_size)
		    SPRITE_MASK_2x(sprmsk, 0, 24, sbit, ptr, sptr,
				   sprites[n].color, cmsk);
		else
		    SPRITE_MASK(sprmsk, 0, 24, sbit, ptr, sptr,
				sprites[n].color, cmsk);
	    }
	    if (cmsk)
		cl_ss_collmask |= cmsk | sbit;
	}
    }
}

/* Draw one multicolor sprite. */
inline static void draw_mc_sprite(PIXEL *line_ptr,
				  BYTE *gfx_msk_ptr,
				  int n,
				  int double_size)
{
    if (sprites[n].x < SCREEN_WIDTH) {
	DWORD sprmsk, mcsprmsk;
	BYTE *msk_ptr;
	PIXEL *ptr = line_ptr + sprites[n].x * (double_size ? 2 : 1);
	BYTE *sptr = sprline + SCREEN_MAX_SPRITE_WIDTH + sprites[n].x;
	BYTE *data_ptr = (BYTE *)(sprite_data + n);
	int in_background = sprites[n].in_background;
	BYTE cmsk = 0, sbit = 1 << n;
	int lshift = (sprites[n].x - xsmooth) & 0x7;
	DWORD c[4];

	c[1] = mc_sprite_color_1;
	c[2] = sprites[n].color;
	c[3] = mc_sprite_color_2;
	msk_ptr = gfx_msk_ptr + ((sprites[n].x + SCREEN_MAX_SPRITE_WIDTH
				  - xsmooth) / 8);
	mcsprmsk = (data_ptr[0] << 16) | (data_ptr[1] << 8) | data_ptr[2];
	if (sprites[n].x_expanded) {
	    DWORD collmsk = ((((msk_ptr[1] << 24) | (msk_ptr[2] << 16)
			       | (msk_ptr[3] << 8) | msk_ptr[4]) << lshift)
			     | (msk_ptr[5] >> (8 - lshift)));

	    sprmsk = sprite_doubling_table[((mcsprtable[data_ptr[0]] << 8)
					    | mcsprtable[data_ptr[1]])];
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    MCSPRITE_DOUBLE_MASK_2x(mcsprmsk, collmsk, 32,
					    sbit, ptr, sptr, c, cmsk);
		else
		    MCSPRITE_DOUBLE_MASK(mcsprmsk, collmsk, 32,
					 sbit, ptr, sptr, c, cmsk);
	    } else {
		if (double_size)
		    MCSPRITE_DOUBLE_MASK_2x(mcsprmsk, 0, 32,
					    sbit, ptr, sptr, c, cmsk);
		else
		    MCSPRITE_DOUBLE_MASK(mcsprmsk, 0, 32,
					 sbit, ptr, sptr, c, cmsk);
	    }
	    sprmsk = sprite_doubling_table[mcsprtable[data_ptr[2]]];
	    collmsk = ((((msk_ptr[5] << 8) | msk_ptr[6]) << lshift)
		       | (msk_ptr[7] >> (8 - lshift)));
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    MCSPRITE_DOUBLE_MASK_2x(mcsprmsk, collmsk, 16, sbit,
					    ptr + 64, sptr + 32, c, cmsk);
		else
		    MCSPRITE_DOUBLE_MASK(mcsprmsk, collmsk, 16, sbit,
					 ptr + 32, sptr + 32, c, cmsk);
	    } else {
		if (double_size)
		    MCSPRITE_DOUBLE_MASK_2x(mcsprmsk, 0, 16, sbit,
					    ptr + 64, sptr + 32, c, cmsk);
		else
		    MCSPRITE_DOUBLE_MASK(mcsprmsk, 0, 16, sbit,
					 ptr + 32, sptr + 32, c, cmsk);
	    }
	} else {		/* Unexpanded */
	    DWORD collmsk = ((((msk_ptr[0] << 24) | (msk_ptr[1] << 16)
			       | (msk_ptr[2] << 8) | msk_ptr[3]) << lshift)
			     | (msk_ptr[4] >> (8 - lshift)));
	    sprmsk = ((mcsprtable[data_ptr[0]] << 16)
		      | (mcsprtable[data_ptr[1]] << 8)
		      | mcsprtable[data_ptr[2]]);
	    if (!idle_state && (sprmsk & collmsk) != 0)
		cl_sb_collmask |= sbit;
	    if (in_background) {
		if (double_size)
		    MCSPRITE_MASK_2x(mcsprmsk, collmsk, 24, sbit, ptr,
				     sptr, c, cmsk);
		else
		    MCSPRITE_MASK(mcsprmsk, collmsk, 24, sbit, ptr,
				  sptr, c, cmsk);
	    } else {
		if (double_size)
		    MCSPRITE_MASK_2x(mcsprmsk, 0, 24, sbit, ptr,
				     sptr, c, cmsk);
		else
		    MCSPRITE_MASK(mcsprmsk, 0, 24, sbit, ptr,
				  sptr, c, cmsk);
	    }
	}
	if (cmsk)
	    cl_ss_collmask |= cmsk | (sbit);
    }
}

inline static void draw_all_sprites(PIXEL *line_ptr, BYTE *gfx_msk_ptr)
{
    cl_ss_collmask = cl_sb_collmask = 0;

    if (dma_msk) {
	int n;

	memset(sprline, 0, sizeof(sprline));

	for (n = 0; n < 8; n++) {
	    if (dma_msk & (1 << n)) {
		if (sprites[n].multicolor)
		    draw_mc_sprite(line_ptr, gfx_msk_ptr, n, 0);
		else
		    draw_hires_sprite(line_ptr, gfx_msk_ptr, n,  0);
	    }
	}

	ss_collmask |= cl_ss_collmask;
	sb_collmask |= cl_sb_collmask;
    }
}

#ifdef NEED_2x
inline static void draw_all_sprites_2x(PIXEL *line_ptr, BYTE *gfx_msk_ptr)
{
    cl_ss_collmask = cl_sb_collmask = 0;

    if (dma_msk) {
	int n;

	memset(sprline, 0, sizeof(sprline));

	for (n = 0; n < 8; n++) {
	    if (dma_msk & (1 << n)) {
		if (sprites[n].multicolor)
		    draw_mc_sprite(line_ptr, gfx_msk_ptr, n, 1);
		else
		    draw_hires_sprite(line_ptr, gfx_msk_ptr, n, 1);
	    }
	}

	ss_collmask |= cl_ss_collmask;
	sb_collmask |= cl_sb_collmask;
    }
}
#endif

static void draw_sprites(void)
{
#ifdef NEED_2x
    if (pixel_width == 1)
        draw_all_sprites(frame_buffer_ptr, gfx_msk);
    else
        draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
#else
    draw_all_sprites(frame_buffer_ptr, gfx_msk);
#endif
}

/* This kludge updates the sprite-sprite collisions without writing to the
   real frame buffer.  We might write a function that actually checks for
   collisions only, but we are lazy.  */
static void update_sprite_collisions(void)
{
    static PIXEL fake_frame_buffer_line[SCREEN_WIDTH
				        + 4 * SCREEN_MAX_SPRITE_WIDTH];
    static BYTE fake_gfx_msk[GFXMSK_SIZE]; /* Always zero.  */
    static PIXEL *fake_frame_buffer_ptr = (fake_frame_buffer_line
					   + 2 * SCREEN_MAX_SPRITE_WIDTH);

    draw_all_sprites(fake_frame_buffer_ptr, fake_gfx_msk);
}

/* ------------------------------------------------------------------------- */

/* These functions draw the background from `start_pixel' to `end_pixel'.  */

static void draw_std_background(int start_pixel, int end_pixel)
{
    vid_memset(frame_buffer_ptr + start_pixel,
	       PIXEL(overscan_background_color),
	       end_pixel - start_pixel + 1);
}

#ifdef NEED_2x
static void draw_std_background_2x(int start_pixel, int end_pixel)
{
    vid_memset(frame_buffer_ptr + 2 * start_pixel,
	       PIXEL2(overscan_background_color),
	       2 * (end_pixel - start_pixel + 1));
}
#endif

/* ------------------------------------------------------------------------- */

/* If unaligned 32-bit access is not allowed, the graphics is stored in a
   temporary aligned buffer, and later copied to the real frame buffer.  This
   is ugly, but should be hopefully faster than accessing 8 bits at a time
   anyway. */

#ifndef ALLOW_UNALIGNED_ACCESS
static PIXEL4 _aligned_line_buffer[VIC_II_SCREEN_XPIX / 2 + 1];
static PIXEL * const aligned_line_buffer = (PIXEL *)_aligned_line_buffer;
#endif

/* Pointer to the start of the graphics area on the frame buffer. */
#define GFX_PTR(pixel_width) \
    (frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth) * (pixel_width))

#ifdef ALLOW_UNALIGNED_ACCESS
#define ALIGN_DRAW_FUNC(name, xs, xe, gfx_msk_ptr, pixel_width)	\
   name(GFX_PTR(pixel_width), (xs), (xe), (gfx_msk_ptr))
#else
#define ALIGN_DRAW_FUNC(name, xs, xe, gfx_msk_ptr, pixel_width)		\
   do {									\
       name(aligned_line_buffer, (xs), (xe), (gfx_msk_ptr));		\
       vid_memcpy(GFX_PTR(pixel_width) + (xs) * 8 * (pixel_width),	\
	          aligned_line_buffer + (xs) * 8 * (pixel_width),	\
	          ((xe) - (xs) + 1) * 8 * (pixel_width));		\
   } while (0)
#endif


/* FIXME: in the cache, we store the foreground bitmap values for the
   characters, but we do not use them when drawing and this is slow!  */

/* Standard text mode. */

static int get_std_text(struct line_cache *l, int *xs, int *xe, int rr)
{
    int r = 0;

    if (background_color != l->bgdata[0] || l->chargen_ptr != chargen_ptr) {
	l->bgdata[0] = background_color;
	l->chargen_ptr = chargen_ptr;
	*xs = 0;
	*xe = SCREEN_TEXTCOLS;
	rr = 1;
    }

    r = _fill_cache_text(l->fgdata, vbuf, chargen_ptr, SCREEN_TEXTCOLS,
			 ycounter, xs, xe, rr);
    r |= _fill_cache(l->colordata1, cbuf, SCREEN_TEXTCOLS, 1, xs, xe, rr);

    if (!r) {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }

    return r;
}

inline static void _draw_std_text(PIXEL *p, int xs, int xe, BYTE *gfx_msk_ptr)
{
    PIXEL4 *table_ptr = hr_table + (background_color << 4);
    BYTE *char_ptr = chargen_ptr + ycounter;
    int i;

    for (i = xs; i <= xe; i++) {
	PIXEL4 *ptr = table_ptr + (cbuf[i] << 8);
	int d = (*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i)
		 = *(char_ptr + vbuf[i] * 8));

	*((PIXEL4 *)p + i * 2) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + i * 2 + 1) = *(ptr + (d & 0xf));
    }
}

static void draw_std_text_cached(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_std_text, xs, xe, l->gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

static void draw_std_text(void)
{
    ALIGN_DRAW_FUNC(_draw_std_text, 0, VIC_II_SCREEN_TEXTCOLS - 1, gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, gfx_msk);
}

#ifdef NEED_2x
inline static void _draw_std_text_2x(PIXEL *p, int xs, int xe,
				     BYTE *gfx_msk_ptr)
{
    PIXEL4 *table_ptr = hr_table_2x + (background_color << 5);
    BYTE *char_ptr = chargen_ptr + ycounter;
    int i;

    for (i = xs; i <= xe; i++) {
	PIXEL4 *ptr = table_ptr + (cbuf[i] << 9);
	int d = (*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i)
		 = *(char_ptr + vbuf[i] * 8));

	*((PIXEL4 *)p + i * 4) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + i * 4 + 1) = *(ptr + 0x10 + (d >> 4));
	*((PIXEL4 *)p + i * 4 + 2) = *(ptr + (d & 0xf));
	*((PIXEL4 *)p + i * 4 + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void draw_std_text_cached_2x(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_std_text_2x, xs, xe, l->gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

static void draw_std_text_2x(void)
{
    ALIGN_DRAW_FUNC(_draw_std_text_2x, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 2);
    draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
}

#endif /* NEED_2x */

#define DRAW_STD_TEXT_BYTE(p, b, f)		\
    if ((b) & 0x80) *(p) = (f);			\
    if ((b) & 0x40) *((p) + 1) = (f);		\
    if ((b) & 0x20) *((p) + 2) = (f);		\
    if ((b) & 0x10) *((p) + 3) = (f);		\
    if ((b) & 0x08) *((p) + 4) = (f);		\
    if ((b) & 0x04) *((p) + 5) = (f);		\
    if ((b) & 0x02) *((p) + 6) = (f);		\
    if ((b) & 0x01) *((p) + 7) = (f);

static void draw_std_text_foreground(int start_char, int end_char)
{
    int i;
    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL *p = (frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth
		+ 8 * start_char);

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = char_ptr[vbuf[i] * 8];
	PIXEL f = PIXEL(cbuf[i]);

	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	DRAW_STD_TEXT_BYTE(p, b, f);
    }
}

#ifdef NEED_2x
static void draw_std_text_foreground_2x(int start_char, int end_char)
{
    int i;
    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL2 *p = (PIXEL2 *)(frame_buffer_ptr + 2 * SCREEN_BORDERWIDTH
			   + 2 * xsmooth) + 8 * start_char;

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = char_ptr[vbuf[i] * 8];
	PIXEL2 f = PIXEL2(cbuf[i]);

	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	/* Notice that we are always aligned on 2-bytes boundaries here. */
	DRAW_STD_TEXT_BYTE(p, b, f);
    }
}
#endif

/* ------------------------------------------------------------------------- */

/* Hires Bitmap mode. */

static int get_hires_bitmap(struct line_cache *l, int *xs, int *xe, int rr)
{
    int r = 0;

    r |= _fill_cache_nibbles(l->colordata1, l->bgdata, vbuf, SCREEN_TEXTCOLS,
			     1, xs, xe, rr);
    r |= _fill_cache(l->fgdata, bitmap_ptr + memptr * 8 + ycounter,
		     SCREEN_TEXTCOLS, 8, xs, xe, rr);

    if (!r) {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }

    return r;
}

inline static void _draw_hires_bitmap(PIXEL *p, int xs, int xe,
				      BYTE *gfx_msk_ptr)
{
    BYTE *bmptr = bitmap_ptr;
    int i, j;

    for (j = ((memptr << 3) + ycounter + xs * 8) & 0x1fff, i = xs;
	 i <= xe;
	 i++, j = (j + 8) & 0x1fff) {
	PIXEL4 *ptr = hr_table + (vbuf[i] << 4);
	int d;

	d = *(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = bmptr[j];
	*((PIXEL4 *)p + i * 2) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + i * 2 + 1) = *(ptr + (d & 0xf));
    }
}

static void draw_hires_bitmap(void)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, gfx_msk);
}

static void draw_hires_bitmap_cached(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap, xs, xe, l->gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#ifdef NEED_2x

inline static void _draw_hires_bitmap_2x(PIXEL *p, int xs, int xe,
					BYTE *gfx_msk_ptr)
{
    BYTE *bmptr = bitmap_ptr;
    int i, j;

    for (j = ((memptr << 3) + ycounter + xs * 8) & 0x1fff, i = xs;
	 i <= xe;
	 i++, j = (j + 8) & 0x1fff) {
	PIXEL4 *ptr = hr_table_2x + (vbuf[i] << 5);
	int d;

	d = *(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = bmptr[j];
	*((PIXEL4 *)p + i * 4) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + i * 4 + 1) = *(ptr + 0x10 + (d >> 4));
	*((PIXEL4 *)p + i * 4 + 2) = *(ptr + (d & 0xf));
	*((PIXEL4 *)p + i * 4 + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void draw_hires_bitmap_2x(void)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap_2x, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
}

static void draw_hires_bitmap_cached_2x(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap_2x, xs, xe, l->gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#endif /* NEED_2x */

static void draw_hires_bitmap_foreground(int start_char, int end_char)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap, start_char, end_char, gfx_msk, 1);
}

#ifdef NEED_2x
static void draw_hires_bitmap_foreground_2x(int start_char, int end_char)
{
    ALIGN_DRAW_FUNC(_draw_hires_bitmap_2x, start_char, end_char, gfx_msk, 2);
}
#endif

/* ------------------------------------------------------------------------- */

/* Multicolor text mode. */

static int get_mc_text(struct line_cache *l, int *xs, int *xe, int rr)
{
    int r = 0;

    if (background_color != l->bgdata[0] || l->colordata1[0] != vic[0x22]
	|| l->colordata1[1] != vic[0x23] || l->chargen_ptr != chargen_ptr) {
	l->bgdata[0] = background_color;
	l->colordata1[0] = vic[0x22];
	l->colordata1[1] = vic[0x23];
	l->chargen_ptr = chargen_ptr;
	*xs = 0;
	*xe = VIC_II_SCREEN_TEXTCOLS - 1;
	rr = 1;
    }

    r = _fill_cache_text(l->fgdata, vbuf, chargen_ptr, SCREEN_TEXTCOLS,
			 ycounter, xs, xe, rr);
    r |= _fill_cache(l->colordata3, cbuf, SCREEN_TEXTCOLS, 1, xs, xe, rr);

    if (!r) {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }

    return r;
}

inline static void _draw_mc_text(PIXEL *p, int xs, int xe, BYTE *gfx_msk_ptr)
{
    PIXEL2 c[7];
    BYTE *char_ptr = chargen_ptr + ycounter;
    int i;

    c[0] = PIXEL2(background_color);
    c[1] = PIXEL2(ext_background_color[0]);
    c[2] = PIXEL2(ext_background_color[1]);
    *((PIXEL *)c + 8) = *((PIXEL *)c + 11) = PIXEL(background_color);
    for (i = xs; i <= xe; i++) {
	unsigned int d = *(char_ptr + vbuf[i] * 8);
	unsigned int k = (cbuf[i] & 0x8) << 5;

	*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[k | d];
#ifdef ALLOW_UNALIGNED_ACCESS
	c[3] = *((PIXEL2 *)((PIXEL *)c + 9)) = PIXEL2(cbuf[i] & 0x7);
#else
	c[3] = PIXEL2(cbuf[i] & 0x7);
	*((PIXEL *)c + 9) = *((PIXEL *)c + 10) = PIXEL2(cbuf[i] & 0x7);
#endif
	*((PIXEL2 *)p + 4 * i) = c[mc_table[k | d]];
	*((PIXEL2 *)p + 4 * i + 1) = c[mc_table[0x200 + (k | d)]];
	*((PIXEL2 *)p + 4 * i + 2) = c[mc_table[0x400 + (k | d)]];
	*((PIXEL2 *)p + 4 * i + 3) = c[mc_table[0x600 + (k | d)]];
    }
}

static void draw_mc_text(void)
{
    ALIGN_DRAW_FUNC(_draw_mc_text, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, gfx_msk);
}

static void draw_mc_text_cached(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_mc_text, xs, xe, l->gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#ifdef NEED_2x

inline static void _draw_mc_text_2x(PIXEL *p, int xs, int xe,
				    BYTE *gfx_msk_ptr)
{
    PIXEL4 c[7];
    int i;
    BYTE *char_ptr = chargen_ptr + ycounter;

    c[0] = PIXEL4(background_color);
    c[1] = PIXEL4(ext_background_color[0]);
    c[2] = PIXEL4(ext_background_color[1]);
    *((PIXEL2 *)c + 8) = *((PIXEL2 *)c + 11) = PIXEL2(background_color);
    for (i = xs; i <= xe; i++) {
	unsigned int d = *(char_ptr + vbuf[i] * 8);
	unsigned int k = (cbuf[i] & 0x8) << 5;

	*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[k | d];
#ifdef ALLOW_UNALIGNED_ACCESS
	c[3] = *((PIXEL4 *)((PIXEL2 *)c + 9)) = PIXEL4(cbuf[i] & 0x7);
#else
	c[3] = PIXEL4(cbuf[i] & 0x7);
	*((PIXEL2 *)c + 9) = *((PIXEL2 *)c + 10) = PIXEL2(cbuf[i] & 0x7);
#endif
	*((PIXEL4 *)p + 4 * i) = c[mc_table[k | d]];
	*((PIXEL4 *)p + 4 * i + 1) = c[mc_table[0x200 + (k | d)]];
	*((PIXEL4 *)p + 4 * i + 2) = c[mc_table[0x400 + (k | d)]];
	*((PIXEL4 *)p + 4 * i + 3) = c[mc_table[0x600 + (k | d)]];
    }
}

static void draw_mc_text_2x(void)
{
    ALIGN_DRAW_FUNC(_draw_mc_text_2x, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
}

static void draw_mc_text_cached_2x(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_mc_text_2x, xs, xe, l->gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}
#endif

/* FIXME: aligned/unaligned versions. */
#define DRAW_MC_BYTE(p, b, f1, f2, f3)		\
    if ((b) & 0x80) {				\
        if ((b) & 0x40)				\
	    *(p) = *((p) + 1) = (f3);		\
	else					\
	    *(p) = *((p) + 1) = (f2);		\
    } else {					\
	if ((b) & 0x40)				\
	    *(p) = *((p) + 1) = (f1);		\
    }						\
    if ((b) & 0x20) {				\
        if ((b) & 0x10)				\
	    *((p) + 2) = *((p) + 3) = (f3);	\
	else					\
	    *((p) + 2) = *((p) + 3) = (f2);	\
    } else {					\
	if ((b) & 0x10)				\
	    *((p) + 2) = *((p) + 3) = (f1);	\
    }						\
    if ((b) & 0x08) {				\
        if ((b) & 0x04)				\
	    *((p) + 4) = *((p) + 5) = (f3);	\
	else					\
	    *((p) + 4) = *((p) + 5) = (f2);	\
    } else {					\
	if ((b) & 0x04)				\
	    *((p) + 4) = *((p) + 5) = (f1);	\
    }						\
    if ((b) & 0x02) {				\
        if ((b) & 0x01)				\
	    *((p) + 6) = *((p) + 7) = (f3);	\
	else					\
	    *((p) + 6) = *((p) + 7) = (f2);	\
    } else {					\
	if ((b) & 0x01)				\
	    *((p) + 6) = *((p) + 7) = (f1);	\
    }

static void draw_mc_text_foreground(int start_char, int end_char)
{
    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL c1 = PIXEL(ext_background_color[0]);
    PIXEL c2 = PIXEL(ext_background_color[1]);
    PIXEL *p = (frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth
		+ 8 * start_char);
    int i;

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = *(char_ptr + vbuf[i] * 8);
	BYTE c = cbuf[i];

	if (c & 0x8) {
	    PIXEL c3 = PIXEL(c & 0x7);

	    DRAW_MC_BYTE(p, b, c1, c2, c3);
	    *(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[0x100 + b];
	} else {
	    PIXEL c3 = PIXEL(c);

	    DRAW_STD_TEXT_BYTE(p, b, c3);
	    *(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	}
    }
}

#ifdef NEED_2x
static void draw_mc_text_foreground_2x(int start_char, int end_char)
{
    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL2 c1 = PIXEL2(ext_background_color[0]);
    PIXEL2 c2 = PIXEL2(ext_background_color[1]);
    PIXEL2 *p = (PIXEL2 *)(frame_buffer_ptr + 2 * SCREEN_BORDERWIDTH
			   + 2 * xsmooth) + 8 * start_char;
    int i;

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = *(char_ptr + vbuf[i] * 8);
	BYTE c = cbuf[i];

	if (c & 0x8) {
	    PIXEL2 c3 = PIXEL2(c & 0x7);

	    DRAW_MC_BYTE(p, b, c1, c2, c3);
	    *(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[0x100 + b];
	} else {
	    PIXEL2 c3 = PIXEL2(c);

	    DRAW_STD_TEXT_BYTE(p, b, c3);
	    *(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	}
    }
}
#endif

/* ------------------------------------------------------------------------- */

/* Multicolor Bitmap Mode. */

static int get_mc_bitmap(struct line_cache *l, int *xs, int *xe, int r)
{
    if (background_color != l->bgdata[0]) {
	l->bgdata[0] = background_color;
	r = 1;
	*xs = 0;
	*xe = SCREEN_TEXTCOLS;
    }
    r = _fill_cache_nibbles(l->colordata1, l->colordata2, vbuf,
			    SCREEN_TEXTCOLS, 1, xs, xe, r);
    r = _fill_cache(l->colordata3, cbuf, SCREEN_TEXTCOLS, 1, xs, xe, r);
    r = _fill_cache(l->fgdata, bitmap_ptr + memptr * 8 + ycounter,
		    SCREEN_TEXTCOLS, 8, xs, xe, r);

    if (!r) {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }
    return r;
}

inline static void _draw_mc_bitmap(PIXEL *p, int xs, int xe, BYTE *gfx_msk_ptr)
{
    BYTE *colptr = cbuf;
    BYTE *bmptr = bitmap_ptr;
    PIXEL2 c[4];
    int i, j;

    c[0] = PIXEL2(background_color);
    for (j = ((memptr << 3) + ycounter + xs * 8) & 0x1fff, i = xs;
	 i <= xe;
	 i++, j = (j + 8) & 0x1fff) {
	unsigned int d = bmptr[j];

	*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[d | 0x100];
	c[1] = PIXEL2(vbuf[i] >> 4);
	c[2] = PIXEL2(vbuf[i] & 0xf);
	c[3] = PIXEL2(colptr[i]);
	*((PIXEL2 *)p + 4 * i) = c[mc_table[0x100 + d]];
	*((PIXEL2 *)p + 4 * i + 1) = c[mc_table[0x300 + d]];
	*((PIXEL2 *)p + 4 * i + 2) = c[mc_table[0x500 + d]];
	*((PIXEL2 *)p + 4 * i + 3) = c[mc_table[0x700 + d]];
    }
}

static void draw_mc_bitmap(void)
{
    ALIGN_DRAW_FUNC(_draw_mc_bitmap, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, gfx_msk);
}

static void draw_mc_bitmap_cached(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_mc_bitmap, xs, xe, l->gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#ifdef NEED_2x

inline static void _draw_mc_bitmap_2x(PIXEL *p, int xs, int xe,
				      BYTE *gfx_msk_ptr)
{
    BYTE *colptr = cbuf;
    BYTE *bmptr = bitmap_ptr;
    PIXEL4 c[4];
    int i, j;

    c[0] = PIXEL4(background_color);
    for (j = ((memptr << 3) + ycounter + xs * 8) & 0x1fff, i = xs;
	 i <= xe;
	 j = (j + 8) & 0x1fff, i++) {
	unsigned int d = bmptr[j];

	*(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[d | 0x100];
	c[1] = PIXEL4(vbuf[i] >> 4);
	c[2] = PIXEL4(vbuf[i] & 0xf);
	c[3] = PIXEL4(colptr[i]);
	*((PIXEL4 *)p + 4 * i) = c[mc_table[0x100 + d]];
	*((PIXEL4 *)p + 4 * i + 1) = c[mc_table[0x300 + d]];
	*((PIXEL4 *)p + 4 * i + 2) = c[mc_table[0x500 + d]];
	*((PIXEL4 *)p + 4 * i + 3) = c[mc_table[0x700 + d]];
    }
}

static void draw_mc_bitmap_2x(void)
{
    ALIGN_DRAW_FUNC(_draw_mc_bitmap_2x, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
}

static void draw_mc_bitmap_cached_2x(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_mc_bitmap_2x, xs, xe, l->gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#endif /* NEED_2x */

static void draw_mc_bitmap_foreground(int start_char, int end_char)
{
    PIXEL *p = (frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth
		+ 8 * start_char);
    BYTE *bmptr = bitmap_ptr;
    int i, j;

    for (j = ((memptr << 3) + ycounter + 8*start_char) & 0x1fff, i = start_char;
	 i <= end_char;
	 j = (j + 8) & 0x1fff, i++, p += 8) {
	PIXEL c1 = PIXEL(vbuf[i] >> 4);
	PIXEL c2 = PIXEL(vbuf[i] & 0xf);
	PIXEL c3 = PIXEL(cbuf[i]);
	BYTE b = bmptr[j];

	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[0x100 + b];
	DRAW_MC_BYTE(p, b, c1, c2, c3);
    }
}

#ifdef NEED_2x

static void draw_mc_bitmap_foreground_2x(int start_char, int end_char)
{
    PIXEL2 *p = ((PIXEL2 *)frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth
		 + 8 * start_char);
    BYTE *bmptr = bitmap_ptr;
    int i, j;

    for (j = ((memptr << 3) + ycounter + 8*start_char) & 0x1fff, i = start_char;
	 i <= end_char;
	 j = (j + 8) & 0x1fff, i++, p += 8) {
	PIXEL2 c1 = PIXEL2(vbuf[i] >> 4);
	PIXEL2 c2 = PIXEL2(vbuf[i] & 0xf);
	PIXEL2 c3 = PIXEL2(cbuf[i]);
	BYTE b = bmptr[j];

	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = mcmsktable[0x100 + b];
	DRAW_MC_BYTE(p, b, c1, c2, c3);
    }
}

#endif

/* ------------------------------------------------------------------------- */

/* Extended Text Mode. */

static int get_ext_text(struct line_cache *l, int *xs, int *xe, int r)
{
    if (r || vic[0x21] != l->colordata2[0] || vic[0x22] != l->colordata2[1]
	|| vic[0x23] != l->colordata2[2] || vic[0x24] != l->colordata2[3]) {
	l->colordata2[0] = vic[0x21];
	l->colordata2[1] = vic[0x22];
	l->colordata2[2] = vic[0x23];
	l->colordata2[3] = vic[0x24];
	r = 1;
    }

    r = _fill_cache(l->colordata1, cbuf, SCREEN_TEXTCOLS, 1, xs, xe, r);
    r = _fill_cache(l->colordata2, vbuf, SCREEN_TEXTCOLS, 1, xs, xe, r);
    r = _fill_cache(l->fgdata, vbuf, SCREEN_TEXTCOLS, 1, xs, xe, r);

    if (!r) {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }

    return r;
}

inline static void _draw_ext_text(PIXEL *p, int xs, int xe,
				  BYTE *gfx_msk_ptr)
{
    BYTE *char_ptr = chargen_ptr + ycounter;
    int i;

    for (i = xs; i <= xe; i++) {
	PIXEL4 *ptr = hr_table + (cbuf[i] << 8);
	int bg_idx = vbuf[i] >> 6;
	int d = *(char_ptr + (vbuf[i] & 0x3f) * 8);

	if (bg_idx == 0)
	    ptr += background_color << 4;
	else
	    ptr += ext_background_color[bg_idx - 1] << 4;
        *(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = d;
        *((PIXEL4 *)p + 2 * i) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + 2 * i + 1) = *(ptr + (d & 0xf));
    }
}

static void draw_ext_text(void)
{
    ALIGN_DRAW_FUNC(_draw_ext_text, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, gfx_msk);
}

static void draw_ext_text_cached(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_ext_text, xs, xe, l->gfx_msk, 1);

    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#ifdef NEED_2x

inline static void _draw_ext_text_2x(PIXEL *p, int xs, int xe,
				     BYTE *gfx_msk_ptr)
{
    BYTE *char_ptr = chargen_ptr + ycounter;
    int i;

    for (i = xs; i <= xe; i++) {
	PIXEL4 *ptr = hr_table_2x + (cbuf[i] << 9);
	int bg_idx = vbuf[i] >> 6;
	int d = *(char_ptr + (vbuf[i] & 0x3f) * 8);

	if (bg_idx == 0)
	    ptr += background_color << 5;
	else
	    ptr += ext_background_color[bg_idx - 1] << 5;
        *(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE + i) = d;
	*((PIXEL4 *)p + 4 * i) = *(ptr + (d >> 4));
	*((PIXEL4 *)p + 4 * i + 1) = *(ptr + 0x10 + (d >> 4));
	*((PIXEL4 *)p + 4 * i + 2) = *(ptr + (d & 0xf));
	*((PIXEL4 *)p + 4 * i + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void draw_ext_text_cached_2x(struct line_cache *l, int xs, int xe)
{
    ALIGN_DRAW_FUNC(_draw_ext_text_2x, xs, xe, l->gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

static void draw_ext_text_2x(void)
{
    ALIGN_DRAW_FUNC(_draw_ext_text_2x, 0, SCREEN_TEXTCOLS - 1, gfx_msk, 2);

    draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
}

#endif /* NEED_2x */

/* FIXME: This is *slow* and might not be 100% correct.  */
static void draw_ext_text_foreground(int start_char, int end_char)
{
    int i;

    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL *p = (frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth
		+ 8 * start_char);

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = char_ptr[(vbuf[i] & 0x3f) * 8];
	PIXEL f = PIXEL(cbuf[i]);
	int bg_idx = vbuf[i] >> 6;

	if (bg_idx > 0) {
#ifdef ALLOW_UNALIGNED_ACCESS
	    *((PIXEL4 *)p) = *((PIXEL4 *)p + 1) =
		PIXEL4(ext_background_color[bg_idx - 1]);
#else
	    p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] =
		PIXEL(ext_background_color[bg_idx - 1]);
#endif
	}
	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	DRAW_STD_TEXT_BYTE(p, b, f);
    }
}

#ifdef NEED_2x
static void draw_ext_text_foreground_2x(int start_char, int end_char)
{
    int i;

    BYTE *char_ptr = chargen_ptr + ycounter;
    PIXEL2 *p = (PIXEL2 *)(frame_buffer_ptr + 2 * SCREEN_BORDERWIDTH
			   + 2 * xsmooth) + 8 * start_char;

    for (i = start_char; i <= end_char; i++, p += 8) {
	BYTE b = char_ptr[(vbuf[i] & 0x3f) * 8];
	PIXEL2 f = PIXEL2(cbuf[i]);
	int bg_idx = vbuf[i] >> 6;

	if (bg_idx > 0) {
#ifdef ALLOW_UNALIGNED_ACCESS
	    *((PIXEL4 *)p) = *((PIXEL4 *)p + 1)
		= *((PIXEL4 *)p + 2) = *((PIXEL4 *)p + 3)
		= PIXEL4(ext_background_color[bg_idx - 1]);
#else
	    p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] =
		PIXEL2(ext_background_color[bg_idx - 1]);
#endif
	}
	*(gfx_msk + GFXMSK_LEFTBORDER_SIZE + i) = b;
	DRAW_STD_TEXT_BYTE(p, b, f);
    }
}
#endif

/* ------------------------------------------------------------------------- */

/* Illegal mode.  Everything is black. */

static int get_black(struct line_cache *l, int *xs, int *xe, int r)
{
    /* Let's simplify here: if also the previous time we had the Black Mode,
       nothing has changed.  If we had not, the whole line has changed. */

    if (r) {
	*xs = 0;
	*xe = SCREEN_TEXTCOLS - 1;
    } else {
	ss_collmask |= l->ss_collmask;
	sb_collmask |= l->sb_collmask;
    }

    return r;
}

static void draw_black(void)
{
    PIXEL *p = frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth) * pixel_width;

    vid_memset(p, PIXEL(0), SCREEN_TEXTCOLS * 8 * pixel_width);

    /* FIXME: this is not exact! */
    memset(gfx_msk + GFXMSK_LEFTBORDER_SIZE, 0, SCREEN_TEXTCOLS);

#ifdef NEED_2x
    if (pixel_width == 1)
	draw_all_sprites(frame_buffer_ptr, gfx_msk);
    else
	draw_all_sprites_2x(frame_buffer_ptr, gfx_msk);
#else
    draw_all_sprites(frame_buffer_ptr, gfx_msk);
#endif
}

static void draw_black_cached(struct line_cache *l, int xs, int xe)
{
    PIXEL *p = frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth) * pixel_width;

    vid_memset(p, PIXEL(0), SCREEN_TEXTCOLS * 8 * pixel_width);
    memset(gfx_msk + GFXMSK_LEFTBORDER_SIZE, 0, SCREEN_TEXTCOLS);

#ifdef NEED_2x
    if (pixel_width == 1)
	draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
    else
	draw_all_sprites_2x(frame_buffer_ptr, l->gfx_msk);
#else
    draw_all_sprites(frame_buffer_ptr, l->gfx_msk);
#endif

    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

static void draw_black_foreground(int start_char, int end_char)
{
    PIXEL *p = frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth +
    				   8 * start_char) * pixel_width;

    vid_memset(p, PIXEL(0), (end_char - start_char + 1) * 8 * pixel_width);
    memset(gfx_msk + GFXMSK_LEFTBORDER_SIZE, 0, SCREEN_TEXTCOLS);
}

/* ------------------------------------------------------------------------- */

/* Idle state. */

static int get_idle(struct line_cache *l, int *xs, int *xe, int rr)
{
    if (rr
        || background_color != l->colordata1[0]
        || idle_data != l->fgdata[0]) {
	l->colordata1[0] = background_color;
	l->fgdata[0] = (BYTE)idle_data;
	*xs = 0;
	*xe = VIC_II_SCREEN_TEXTCOLS - 1;
	return 1;
    } else
	return 0;
}

inline static void _draw_idle(int xs, int xe, int _pixel_width,
			      BYTE *gfx_msk_ptr)
{
    PIXEL *p;
    BYTE d = (BYTE) idle_data;
    int i;

#ifdef ALLOW_UNALIGNED_ACCESS
    p = frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth) * _pixel_width;
#else
    p = aligned_line_buffer;
#endif

    if (VIC_II_IS_ILLEGAL_MODE(video_mode)) {
        vid_memset(p, PIXEL(0), SCREEN_XPIX * _pixel_width);
    } else if (_pixel_width == 1) {
	/* The foreground color is always black (0). */
	unsigned int offs = overscan_background_color << 4;
	PIXEL4 c1 = *(hr_table + offs + (d >> 4));
	PIXEL4 c2 = *(hr_table + offs + (d & 0xf));

	for (i = xs * 8; i <= xe * 8; i += 8) {
	    *((PIXEL4 *)(p + i)) = c1;
	    *((PIXEL4 *)(p + i + 4)) = c2;
	}
    }
#ifdef NEED_2x
    else if (_pixel_width == 2) {
	/* The foreground color is always black (0). */
	unsigned int offs = overscan_background_color << 5;
	PIXEL4 c1 = *(hr_table_2x + offs + (d >> 4));
	PIXEL4 c2 = *(hr_table_2x + 0x10 + offs + (d >> 4));
	PIXEL4 c3 = *(hr_table_2x + offs + (d & 0xf));
	PIXEL4 c4 = *(hr_table_2x + 0x10 + offs + (d & 0xf));

	for (i = xs * 16; i <= xe * 16; i += 16) {
	    *((PIXEL4 *)(p + i)) = c1;
	    *((PIXEL4 *)(p + i + 4)) = c2;
	    *((PIXEL4 *)(p + i + 8)) = c3;
	    *((PIXEL4 *)(p + i + 12)) = c4;
	}
    }
#endif

#ifndef ALLOW_UNALIGNED_ACCESS
    vid_memcpy(frame_buffer_ptr + (SCREEN_BORDERWIDTH + xsmooth) * _pixel_width,
	       aligned_line_buffer + xs * 8 * _pixel_width,
	       (xe - xs + 1) * 8 * _pixel_width);
#endif

    memset(gfx_msk_ptr + GFXMSK_LEFTBORDER_SIZE, d, SCREEN_TEXTCOLS);

#ifdef NEED_2x
    if (_pixel_width == 1)
	draw_all_sprites(frame_buffer_ptr, gfx_msk_ptr);
    else
	draw_all_sprites_2x(frame_buffer_ptr, gfx_msk_ptr);
#else
    draw_all_sprites(frame_buffer_ptr, gfx_msk_ptr);
#endif
}

static void draw_idle(void)
{
    _draw_idle(0, VIC_II_SCREEN_TEXTCOLS - 1, 1, gfx_msk);
}

static void draw_idle_cached(struct line_cache *l, int xs, int xe)
{
    _draw_idle(xs, xe, 1, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}

#ifdef NEED_2x
static void draw_idle_2x(void)
{
    _draw_idle(0, VIC_II_SCREEN_TEXTCOLS - 1, 2, gfx_msk);
}

static void draw_idle_cached_2x(struct line_cache *l, int xs, int xe)
{
    _draw_idle(xs, xe, 2, l->gfx_msk);
    l->ss_collmask = cl_ss_collmask;
    l->sb_collmask = cl_sb_collmask;
}
#endif

static void draw_idle_foreground(int start_char, int end_char)
{
    PIXEL *p = frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth;
    PIXEL c = PIXEL(0);
    BYTE d = (BYTE) idle_data;
    int i;

    for (i = start_char; i <= end_char; i++) {
        DRAW_STD_TEXT_BYTE(p + i * 8, d, c);
        gfx_msk[GFXMSK_LEFTBORDER_SIZE + i] = d;
    }
}

#ifdef NEED_2x
static void draw_idle_foreground_2x(int start_char, int end_char)
{
    PIXEL2 *p = (PIXEL2 *)frame_buffer_ptr + SCREEN_BORDERWIDTH + xsmooth;
    PIXEL2 c = PIXEL2(0);
    BYTE d = (BYTE) idle_data;
    int i;

    for (i = start_char; i <= end_char; i++) {
        DRAW_STD_TEXT_BYTE(p + i * 8, d, c);
        gfx_msk[GFXMSK_LEFTBORDER_SIZE + i] = d;
    }
}
#endif

/* ------------------------------------------------------------------------- */

/* Set proper functions and constants for the current video settings. */
void video_resize(void)
{
    static int old_size = 0;

    video_modes[VIC_II_NORMAL_TEXT_MODE].fill_cache = get_std_text;
    video_modes[VIC_II_MULTICOLOR_TEXT_MODE].fill_cache = get_mc_text;
    video_modes[VIC_II_HIRES_BITMAP_MODE].fill_cache = get_hires_bitmap;
    video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].fill_cache = get_mc_bitmap;
    video_modes[VIC_II_EXTENDED_TEXT_MODE].fill_cache = get_ext_text;
    video_modes[VIC_II_ILLEGAL_TEXT_MODE].fill_cache = get_black;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_1].fill_cache = get_black;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_2].fill_cache = get_black;
    video_modes[VIC_II_ILLEGAL_TEXT_MODE].draw_line_cached = draw_black_cached;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_1].draw_line_cached = draw_black_cached;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_2].draw_line_cached = draw_black_cached;
    video_modes[VIC_II_ILLEGAL_TEXT_MODE].draw_line = draw_black;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_1].draw_line = draw_black;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_2].draw_line = draw_black;
    video_modes[VIC_II_ILLEGAL_TEXT_MODE].draw_foreground = draw_black_foreground;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_1].draw_foreground = draw_black_foreground;
    video_modes[VIC_II_ILLEGAL_BITMAP_MODE_2].draw_foreground = draw_black_foreground;
    video_modes[VIC_II_IDLE_MODE].fill_cache = get_idle;

#ifdef NEED_2x
    if (double_size_enabled) {
        int i;

        for (i = 0; i < SCREEN_NUM_VMODES; i++)
	    video_modes[i].draw_background = draw_std_background_2x;

	pixel_width = 2;
	pixel_height = 2;

	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_line_cached = draw_std_text_cached_2x;
	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_line = draw_std_text_2x;
	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_foreground = draw_std_text_foreground_2x;

	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_line_cached = draw_mc_text_cached_2x;
	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_line = draw_mc_text_2x;
	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_foreground = draw_mc_text_foreground_2x;

	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_line_cached = draw_ext_text_cached_2x;
	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_line = draw_ext_text_2x;
	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_foreground = draw_ext_text_foreground_2x;

	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_line_cached = draw_hires_bitmap_cached_2x;
	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_line = draw_hires_bitmap_2x;
	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_foreground = draw_hires_bitmap_foreground_2x;

	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_line_cached = draw_mc_bitmap_cached_2x;
	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_line = draw_mc_bitmap_2x;
	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_foreground = draw_mc_bitmap_foreground_2x;

	video_modes[VIC_II_IDLE_MODE].draw_line = draw_idle_2x;
	video_modes[VIC_II_IDLE_MODE].draw_line_cached = draw_idle_cached_2x;
	video_modes[VIC_II_IDLE_MODE].draw_foreground = draw_idle_foreground_2x;

	if (old_size == 1) {
	    window_width *= 2;
	    window_height *= 2;
	}
    } else
#endif /* NEED_2x */
    {
        int i;

        for (i = 0; i < SCREEN_NUM_VMODES; i++)
	    video_modes[i].draw_background = draw_std_background;

#ifndef pixel_width
	pixel_width = 1;
	pixel_height = 1;
#endif

	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_line_cached = draw_std_text_cached;
	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_line = draw_std_text;
	video_modes[VIC_II_NORMAL_TEXT_MODE].draw_foreground = draw_std_text_foreground;

	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_line_cached = draw_mc_text_cached;
	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_line = draw_mc_text;
	video_modes[VIC_II_MULTICOLOR_TEXT_MODE].draw_foreground = draw_mc_text_foreground;

	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_line_cached = draw_ext_text_cached;
	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_line = draw_ext_text;
	video_modes[VIC_II_EXTENDED_TEXT_MODE].draw_foreground = draw_ext_text_foreground;

	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_line_cached = draw_hires_bitmap_cached;
	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_line = draw_hires_bitmap;
	video_modes[VIC_II_HIRES_BITMAP_MODE].draw_foreground = draw_hires_bitmap_foreground;

	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_line_cached = draw_mc_bitmap_cached;
	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_line = draw_mc_bitmap;
	video_modes[VIC_II_MULTICOLOR_BITMAP_MODE].draw_foreground = draw_mc_bitmap_foreground;

	video_modes[VIC_II_IDLE_MODE].draw_line = draw_idle;
	video_modes[VIC_II_IDLE_MODE].draw_line_cached = draw_idle_cached;
	video_modes[VIC_II_IDLE_MODE].draw_foreground = draw_idle_foreground;

	if (old_size == 2) {
	    window_width /= 2;
	    window_height /= 2;
	}
    }

    old_size = double_size_enabled ? 2 : 1;

    if (canvas) {
	resize(window_width, window_height);
	force_repaint();
	frame_buffer_clear(&frame_buffer, PIXEL(0));
	refresh_all();
    }
}

void vic_ii_prevent_clk_overflow(CLOCK sub)
{
    int_raster_clk -= sub;
    oldclk -= sub;
    vic_ii_fetch_clk -= sub;
    vic_ii_draw_clk -= sub;
}
