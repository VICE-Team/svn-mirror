/*
 * raster.c - Common routines for video chip emulation.
 *
 * Written by
 *   Ettore Perazzoli (ettore@comm2000.it)
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

/* TODO: Remove chip-specific hacks (es. `#ifdef __VIC_II__'.  Make
   everything much cleaner.  Possibly, avoid #including `raster.c', but make
   it a separate module if this does not hurt performance.  */

#ifndef _RASTER_H

#define _RASTER_H

#include "types.h"
#include "vmachine.h"
#include "video.h"

/* ------------------------------------------------------------------------- */

#ifndef BYTES_PER_CHAR
#define BYTES_PER_CHAR 	SCREEN_CHARHEIGHT
#endif

#define GET_CHAR_DATA(m, c, l)  (m)[((c) * BYTES_PER_CHAR) + (l)]

#define LO_NIBBLE(c)   ((c) & 0xf)
#define HI_NIBBLE(c)   (((c) & 0xf0) >> 4)

/* ------------------------------------------------------------------------- */

/* This defines the cache for one sprite line. */
struct sprite_line_cache {

    /* Sprite colors. */
    int c1, c2, c3;

    /* Data used on the current line. */
    DWORD data;

    /* X expansion flag. */
    int x_expanded;

    /* X coordinate.  Note: this can be negative, when the sprite "wraps"
       to the left! */
    int x;

    /* Activation flag. */
    int visible;

    /* Priority flag. */
    int in_background;

    /* Multicolor flag. */
    int multicolor;

};

/* This defines the screen cache.  It includes the sprite cache too. */
struct line_cache {

    /* If nonzero, it means that the cache entry is invalid. */
    int is_dirty;

    /* Number of line shown (referred to drawable area) */
    int n;

    /* Bitmap representation of the graphics in foreground. */
    BYTE fgdata[SCREEN_MAX_TEXTCOLS];

    /* Color information. */
    BYTE border_color;
    BYTE bgdata[SCREEN_MAX_TEXTCOLS];

#ifdef __VIC_II__
    /* This is needed in the VIC-II for the area between the end of the left
       border and the start of the graphics, when the X smooth scroll
       register is > 0.  */
    BYTE overscan_background_color;
#endif

    /* The following are generic and are used differently by the video
       emulators. */
    BYTE colordata1[SCREEN_MAX_TEXTCOLS];
    BYTE colordata2[SCREEN_MAX_TEXTCOLS];
    BYTE colordata3[SCREEN_MAX_TEXTCOLS];

    /* X smooth scroll offset. */
    int xsmooth;

    /* Video mode. */
    int video_mode;

    /* Blank mode flag. */
    int blank;

    /* This defines the borders. */
    int display_xstart, display_xstop;

    /* Number of columns enabled on this line (VIC-I & VDC only). */
    int numcols;

    /* Number of sprites on this line. */
    int numsprites;

    /* Bit mask for the sprites that are visible on this line. */
    int sprmask;

#if SCREEN_NUM_SPRITES > 0
    /* Sprite cache. */
    struct sprite_line_cache sprites[SCREEN_NUM_SPRITES];
    BYTE gfx_msk[SCREEN_GFXMSK_SIZE];
#endif

    /* Sprite-sprite and sprite-background collisions that were detected on
       this line. */
    BYTE ss_collmask;
    BYTE sb_collmask;

    /* Character memory pointer. */
    BYTE *chargen_ptr;

    /* Character row counter. */
    int ycounter;

    /* Flags for open left/right borders. */
    int open_right_border, open_left_border;

};
static struct line_cache cache[SCREEN_MAX_HEIGHT];

struct sprite {

    /* Physical X, Y coordinates.  Note: `x' can be negative, when the sprite
       "wraps" to the left!  It can also be different from the value in
       the corresponding register in the video chip. */
    int x, y;

    /* Expansion flags. */
    int x_expanded, y_expanded;

    /* Multicolor mode flag. */
    int multicolor;

    /* If 0, the sprite is in in foreground; if 1, it is in background. */
    int in_background;

    /* Primary sprite color. */
    int color;

    /* Sprite memory pointer (for drawing). */
    int memptr;

    /* Value to add to memptr at the next memory fetch. */
    int memptr_inc;

    /* Sprite expansion flip flop. */
    int exp_flag;

    /* DMA activation flag. If != 0, memory access is enabled for this
       sprite. */
    int dma_flag;

};

/* With this structure, we can define different drawing and fetching functions
   for each video mode.  One constant IDLE_MODE must be defined to the special
   mode used in the idle state. */
struct video_mode_def {

    /* Fill the cache with the screen data and check for differences.
       If nothing has changed, return 0.  Otherwise, return the smallest
       interval that contains the changed parts and return 1.
       If no_check != 0, fill the cache without checking for differences
       and return 1. */
    int (*fill_cache) (struct line_cache * l, int *changed_start,
		       int *changed_end, int no_check);

    /* Draw part of one line to the buffer. */
    void (*draw_line_cached) (struct line_cache * l, int start, int end);

    /* Draw the whole line to the buffer. */
    void (*draw_line) (void);

    /* Draw part of the background to the buffer. */
    void (*draw_background) (int start_pixel, int end_pixel);

    /* Draw part of the foreground to the buffer. */
    void (*draw_foreground) (int start_char, int end_char);

};
static struct video_mode_def video_modes[SCREEN_NUM_VMODES];

/* This defines the smallest rectangle that contains the areas that have
   changed since the latest refresh. */
struct frame_rect {
    /* This flag indicates that we have a null-sized rectangle. */
    int is_null;

    /* Coordinates of the upper left and lower right corners. */
    int xs, ys;
    int xe, ye;
};
static struct frame_rect changed_area;

/* Buffer containing the current frame.  Each frame is calculated line-wise in
   this buffer and then displayed each time we reach the last line. */
static frame_buffer_t frame_buffer;
static PIXEL *frame_buffer_ptr;

/* ------------------------------------------------------------------------- */

/* Here we handle partial updates. */

enum change_type { CHANGE_PTR, CHANGE_BYTE };

/* With this struct, we keep track of the changes that happen before the end
   of the line.  When no such changes happen, we can use the fast
   line-by-line functions.  Otherwise, we use this struct to draw the line in
   pieces. */
struct changes {

    /* Total number of changes. */
    int count;

    struct {

	/* "Where" the change happens (character position for foreground
	   changes, pixel position for other changes). */
	int where;

	/* Data type for changed value. */
	enum change_type type;

	/* Pointer to where the value is stored and new value to assign. */
	union {
	    struct {
		int *oldp;
		int new;
	    } integer;
	    struct {
		void **oldp;
		void *new;
	    } ptr;
	} value;

    } actions[256];
};

/* Changes that affect the background. */
struct changes background_changes;
/* Changes that affect the foreground. */
struct changes foreground_changes;
/* Changes that affect the borders. */
struct changes border_changes;
/* Flag: do we have changes on this line? */
static int have_changes_on_this_line;
/* Changes that must not be visible until the next line. */
struct changes next_line_changes;

/* ------------------------------------------------------------------------- */

/* These tables translate each color number to the corresponding pixel value in
   the frame_buffer.  `pixel_table' gives one pixel, `double_pixel_table' two
   pixels and `quad_pixel_table' four pixels. */
static PIXEL pixel_table[256];
static PIXEL2 double_pixel_table[256];
static PIXEL4 quad_pixel_table[256];
#ifndef __MSDOS__
#define PIXEL(c)	pixel_table[c]
#define PIXEL2(c)	double_pixel_table[c]
#define PIXEL4(c)	quad_pixel_table[c]
#else
#define PIXEL(c)	(c)
#define PIXEL2(c)	((c) | ((c) << 8))
#define PIXEL4(c)	((c) | ((c) << 8) | ((c) << 16) | ((c) << 24))
#endif

/* ------------------------------------------------------------------------- */

/* Output window. */
static canvas_t canvas;

/* Portion of the screen displayed on the output window window. */
static unsigned int window_width, window_height;

/* Offset of the screen on the window. */
static unsigned int window_x_offset, window_y_offset;

/* First and last lines shown in the output window. */
static unsigned int window_first_line, window_last_line;

/* First pixel in one line of the frame buffer to be shown on the output
   window. */
static unsigned int window_first_x;

/* Size of pixels in the output window.  These might have been #defined to
   constant values. */
#ifndef pixel_width
static unsigned int pixel_width;
#endif
#ifndef pixel_height
static unsigned int pixel_height;
#endif

/* Smooth scroll values for the graphics (not the whole screen). */
static int xsmooth, ysmooth;

/* If nonzero, we should skip the next frame. (used for automatic refresh rate
   setting) */
static int skip_next_frame;

/* Next line to be calculated. */
static int rasterline;

/* Clock value of the last line update. */
static CLOCK oldclk;

/* Border and background colors. */
static int border_color, background_color;

/* Color of the overscan area. */
static int overscan_background_color;

/* If this is != 0, no graphics is drawn and the whole line is painted with
   border_color. */
static int blank_enabled;

/* If this is != 0, the current raster line is blank.  The value of this
   variable is set to zero again after the current line is updated.  */
static int blank_this_line;

/* Open border flags. */
static int open_right_border, open_left_border;

/* blank_enabled is set when line display_ystop is reached and reset when line
   display_ystart is reached and blank is 0. */
static int blank;
static int display_ystart, display_ystop;

/* These define the borders for the current line. */
static int display_xstart, display_xstop;

/* Flag: are we in idle state? */
static int idle_state;

/* Flag: should we display the line in idle state? */
static int draw_idle_state;

/* Flag: should we force display (i.e. non-idle) state for the following
   line? */
static int force_display_state;

/* Count character lines (i.e. RC on the VIC-II). */
static int ycounter;

/* Internal memory pointer (i.e. VCBASE in the VIC-II). */
static int memptr;

#ifndef __VIC_II__
/* Value to add to `memptr' at the end of the raster line (not on VIC-II).  */
static int memptr_inc;
#endif

/* Internal memory counter (i.e. VC in the VIC-II). */
static int mem_counter;

#ifdef __VIC_II__
/* Value to add to `mem_counter' after the graphics has been painted. */
static int mem_counter_inc;
#endif

/* Flag: is the current line a `bad' line? */
static int bad_line;

/* Current video mode. */
static int video_mode;

/* This flag is set if a memory fetch has already happened on this line. */
static int memory_fetch_done;

/* If this flag is set, do not emulate at all. */
static int asleep;

/* This is != 0 if we cannot use the values in the cache anymore. */
static int dont_cache = 1;

/* Number of lines that have been recalculated.  When this value reaches the
   number of lines that are displayed in the output, then the cache is valid
   again. */
static int num_cached_lines = 0;

/* Bit mask for the sprites that are activated. */
static BYTE visible_sprite_msk = 0;

/* Bit mask for the sprites that have DMA enabled. */
static BYTE dma_msk = 0;

/* Value for `dma_msk', after sprites have been drawn. */
static BYTE new_dma_msk = 0;

/* Pointer to the start of the video bank. */
static BYTE *vbank_ptr;

/* ------------------------------------------------------------------------- */

/* Sprite-specific definitions. */

#if SCREEN_NUM_SPRITES > 0

/* Sprites. */
static struct sprite sprites[SCREEN_NUM_SPRITES];

/* Additional colors for multicolor sprites. */
static int mc_sprite_color_1, mc_sprite_color_2;

/* Place where the sprite memory pointers are. */
static BYTE *sprite_ptr_base;

/* Sprite-sprite and sprite-background collisions detected on the current
   line. */
static BYTE cl_ss_collmask;
static BYTE cl_sb_collmask;

/* Sprite data. */
/* As we have to fetch sprite data for the next line while drawing the
   current one, we need two buffers.  */
static DWORD sprite_data_1[SCREEN_NUM_SPRITES];
static DWORD sprite_data_2[SCREEN_NUM_SPRITES];
static DWORD *sprite_data;
static DWORD *new_sprite_data;

#else /* No sprites. */

#define SCREEN_MAX_SPRITE_WIDTH	0

#endif


#endif /* defined(_RASTER_H) */
