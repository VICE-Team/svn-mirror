/*
 * vicii.h - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
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

#ifndef _VICII_H
#define _VICII_H

#include "vice.h"
#include "maincpu.h"
#include "video.h"

/* Color definitions. */

#define VIC_II_NUM_COLORS	16

#ifdef _VICII_C

/* These have been measured with a digitizer by MagerValp@Goth.Org. Thanks! */
static color_def_t color_defs[VIC_II_NUM_COLORS] =
{
    { 0x0000, 0x0000, 0x0000,  0 },	/* Black */
    { 0xfd00, 0xfe00, 0xfc00, 16 },	/* White */
    { 0xbe00, 0x1a00, 0x2400,  4 },	/* Red */
    { 0x3000, 0xe600, 0xc600, 12 },	/* Cyan */
    { 0xb400, 0x1a00, 0xe200,  8 },	/* Purple */
    { 0x1f00, 0xd200, 0x1e00,  4 },	/* Green */
    { 0x2100, 0x1b00, 0xae00,  4 },	/* Blue */
    { 0xdf00, 0xf600, 0x0a00, 12 },	/* Yellow */
    { 0xb800, 0x4100, 0x0400,  4 },	/* Orange */
    { 0x6a00, 0x3300, 0x0400,  4 },	/* Brown */
    { 0xfe00, 0x4a00, 0x5700,  8 },	/* Light Red */
    { 0x4200, 0x4500, 0x4000,  4 },	/* Dark Gray */
    { 0x7000, 0x7400, 0x6f00,  8 },	/* Medium Gray */
    { 0x5900, 0xfe00, 0x5900,  8 },	/* Light Green */
    { 0x5f00, 0x5300, 0xfe00,  8 },	/* Light Blue */
    { 0xa400, 0xa700, 0xa200, 12 }	/* Light Gray */
};

#endif /* defined(_VICII_C) */


/* Screen constants. */

#define VIC_II_SCREEN_HEIGHT	       312
#define VIC_II_CYCLES_PER_LINE         63
#define VIC_II_RFSH_PER_SEC            50

#if 0
#define VIC_II_SCREEN_WIDTH		411
#else
/* Not the real size, but more than enough for everything.  Emulating a smaller
   screen makes it faster.  */
#define VIC_II_SCREEN_WIDTH		384
#endif

#define VIC_II_SCREEN_XPIX		320
#define VIC_II_SCREEN_YPIX		200
#define VIC_II_SCREEN_TEXTCOLS		40
#define VIC_II_SCREEN_TEXTLINES        	25
#define VIC_II_SCREEN_MAX_TEXTCOLS     	VIC_II_SCREEN_TEXTCOLS
#define VIC_II_SCREEN_MAX_XPIX		XPIX
#define VIC_II_SCREEN_MAX_YPIX		YPIX
#define VIC_II_SCREEN_MAX_HEIGHT	VIC_II_SCREEN_HEIGHT
#define VIC_II_SCREEN_BORDERWIDTH	32
#define VIC_II_SCREEN_BORDERHEIGHT     	51
#define VIC_II_SCREEN_CHARHEIGHT	8
#define VIC_II_NUM_SPRITES		8
#define VIC_II_MAX_SPRITE_WIDTH		48
#define VIC_II_SPRITE_WRAP_X		504

#define VIC_II_FIRST_DISPLAYED_LINE	0x10
#define VIC_II_LAST_DISPLAYED_LINE	0x11f
#define VIC_II_25ROW_START_LINE		0x33
#define VIC_II_25ROW_STOP_LINE		0xfb
#define VIC_II_24ROW_START_LINE		0x37
#define VIC_II_24ROW_STOP_LINE		0xf7
#define VIC_II_40COL_START_PIXEL	0x20
#define VIC_II_40COL_STOP_PIXEL		0x160
#define VIC_II_38COL_START_PIXEL	0x27
#define VIC_II_38COL_STOP_PIXEL		0x157

/* Available video modes.  The number is given by
   ((vic[0x11] & 0x60) | (vic[0x16] & 0x10)) >> 4. */
#define VIC_II_NORMAL_TEXT_MODE         0
#define VIC_II_MULTICOLOR_TEXT_MODE     1
#define VIC_II_HIRES_BITMAP_MODE        2
#define VIC_II_MULTICOLOR_BITMAP_MODE   3
#define VIC_II_EXTENDED_TEXT_MODE       4
#define VIC_II_ILLEGAL_TEXT_MODE	5
#define VIC_II_ILLEGAL_BITMAP_MODE_1	6
#define VIC_II_ILLEGAL_BITMAP_MODE_2	7
/* Special mode for idle state. (used by raster.c) */
#define VIC_II_IDLE_MODE		8

#define VIC_II_NUM_VMODES               9

#define VIC_II_IS_ILLEGAL_MODE(x)	((x) >= VIC_II_ILLEGAL_TEXT_MODE \
					 && (x) != VIC_II_IDLE_MODE)
#define VIC_II_IS_BITMAP_MODE(x)	((x) & 0x02)

#ifndef C128
#define VIC_II_WINDOW_TITLE		"VICE: C64 emulator"
#else
#define VIC_II_WINDOW_TITLE		"C128 emulator (40 column)"
#endif


/* Define proper constants for raster.h. */
#ifdef _VICII_C
#define __VIC_II__

#define NEEDS_GetData
#define NEEDS_GetDataHiLo
#define NEEDS_GetCharData
#define SCREEN_WIDTH			VIC_II_SCREEN_WIDTH
#define SCREEN_HEIGHT			VIC_II_SCREEN_HEIGHT
#define SCREEN_XPIX			VIC_II_SCREEN_XPIX
#define SCREEN_YPIX			VIC_II_SCREEN_YPIX
#define SCREEN_TEXTCOLS			VIC_II_SCREEN_TEXTCOLS
#define SCREEN_TEXTLINES		VIC_II_SCREEN_TEXTLINES
#define SCREEN_MAX_TEXTCOLS		VIC_II_SCREEN_MAX_TEXTCOLS
#define SCREEN_MAX_XPIX			VIC_II_SCREEN_MAX_XPIX
#define SCREEN_MAX_YPIX			VIC_II_SCREEN_MAX_YPIX
#define SCREEN_MAX_HEIGHT		VIC_II_SCREEN_MAX_HEIGHT
#define SCREEN_BORDERWIDTH		VIC_II_SCREEN_BORDERWIDTH
#define SCREEN_BORDERHEIGHT		VIC_II_SCREEN_BORDERHEIGHT
#define SCREEN_NUM_VMODES		VIC_II_NUM_VMODES
#define SCREEN_CYCLES_PER_LINE		VIC_II_CYCLES_PER_LINE
#define SCREEN_RFSH_PER_SEC             VIC_II_RFSH_PER_SEC
#define SCREEN_CHARHEIGHT		VIC_II_SCREEN_CHARHEIGHT
#define SCREEN_NUM_COLORS               VIC_II_NUM_COLORS
#define SCREEN_SKIP_FRAMES		(app_resources.refreshRate - 1)
#define SCREEN_FIRST_DISPLAYED_LINE	VIC_II_FIRST_DISPLAYED_LINE
#define SCREEN_LAST_DISPLAYED_LINE	VIC_II_LAST_DISPLAYED_LINE
#define SCREEN_IDLE_MODE	        VIC_II_IDLE_MODE

#define SCREEN_NUM_SPRITES              VIC_II_NUM_SPRITES
#define SCREEN_MAX_SPRITE_WIDTH		VIC_II_MAX_SPRITE_WIDTH
#define SCREEN_SPRITE_WRAP_X		VIC_II_SPRITE_WRAP_X

#define GFXMSK_LEFTBORDER_SIZE 	  ((SCREEN_MAX_SPRITE_WIDTH +	\
				    SCREEN_BORDERWIDTH) / 8 + 1)
#define GFXMSK_SIZE		  ((SCREEN_WIDTH 			\
                                    + SCREEN_MAX_SPRITE_WIDTH) / 8 + 1)
#define SCREEN_GFXMSK_SIZE GFXMSK_SIZE

#endif /* defined(__VIC_II__) */

extern canvas_t vic_ii_init(void);
extern void vic_ii_exposure_handler(unsigned int width, unsigned int height);
extern BYTE REGPARM1 read_vic(ADDRESS addr);
extern void REGPARM2 store_vic(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_colorram(ADDRESS addr);
extern void REGPARM2 store_colorram(ADDRESS addr, BYTE value);
extern void REGPARM2 store_vbank(ADDRESS addr, BYTE value);
extern void REGPARM2 store_vbank_39xx(ADDRESS addr, BYTE value);
extern void REGPARM2 store_vbank_3fxx(ADDRESS addr, BYTE value);
extern void video_resize(void);
extern void video_free(void);
extern int int_rasterdraw(long offset);
extern int int_rasterfetch(long offset);
extern int int_raster(long offset);
extern void vic_ii_prevent_clk_overflow(void);
extern void vic_ii_trigger_light_pen(CLOCK mclk);
extern void vic_ii_set_vbank(int new_vbank);
extern void reset_vic_ii(void);
extern int vbank;

extern CLOCK vic_ii_fetch_clk, vic_ii_draw_clk;

/* ------------------------------------------------------------------------- */

inline static void vic_ii_handle_pending_alarms(int num_write_cycles)
{
    if (num_write_cycles != 0) {
	int f;

	/* Cycles can be stolen only during the read accesses, so we serve
           only the events that happened during them.  The last read access
           happened at `clk - maincpu_write_cycles()' as all the opcodes
           except BRK and JSR do all the write accesses at the very end.  BRK
           cannot take us here and we would not be able to handle JSR
           correctly anyway, so we don't care about them... */

	/* Go back to the time when the read accesses happened and serve VIC
           events. */
	clk -= num_write_cycles;

	do {
	    f = 0;
	    if (clk > vic_ii_fetch_clk) {
		int_rasterfetch(0);
		f = 1;
	    }
	    if (clk >= vic_ii_draw_clk) {
		int_rasterdraw(clk - vic_ii_draw_clk);
		f = 1;
	    }
	} while (f);

	/* Go forward to the time when the last write access happens (that's
	   the one we care about, as the only instructions that do two write
	   accesses - except BRK and JSR - are the RMW ones, which store the
	   old value in the first write access, and then store the new one in
	   the second write access). */
	clk += num_write_cycles;

    } else {
	int f;

	do {
	    f = 0;
	    if (clk >= vic_ii_fetch_clk) {
		int_rasterfetch(0);
		f = 1;
	    }
	    if (clk >= vic_ii_draw_clk) {
		int_rasterdraw(0);
		f = 1;
	    }
	} while (f);
    }
}

#endif				/* _VICII_H */
