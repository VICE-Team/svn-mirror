/*
 * vic.h - A VIC-I emulation (under construction)
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

#ifndef _VIC_H
#define _VIC_H

#include "vice.h"

#include "snapshot.h"
#include "video.h"

#if 1
#define VIC_SCREEN_HEIGHT           312
#define VIC_CYCLES_PER_LINE         71
#define VIC_RFSH_PER_SEC            50
#else				/* NTSC */
#warning NTSC
#define VIC_SCREEN_HEIGHT           261
#define VIC_CYCLES_PER_LINE         63
#define VIC_RFSH_PER_SEC            60
#endif

#ifdef _VIC_C
static int char_height = 8;
static int text_cols = 22;
static int text_lines = 23;
#define VIC_SCREEN_MAX_HEIGHT		VIC_SCREEN_HEIGHT
#define VIC_SCREEN_CHARHEIGHT		char_height
#define VIC_SCREEN_MAX_TEXTCOLS		31
#define VIC_SCREEN_MAX_TEXTLINES	30
#define VIC_SCREEN_TEXTLINES		text_lines
#define VIC_SCREEN_TEXTCOLS		text_cols
#define VIC_SCREEN_FIRST_DISPLAYED_LINE 28
#define VIC_SCREEN_LAST_DISPLAYED_LINE  293
#define VIC_SCREEN_WIDTH		233
#define VIC_SCREEN_XPIX			VIC_SCREEN_WIDTH
#define VIC_SCREEN_YPIX			(VIC_SCREEN_LAST_DISPLAYED_LINE \
					 - VIC_SCREEN_FIRST_DISPLAYED_LINE + 1)
#define VIC_SCREEN_BORDERWIDTH		display_xstart
#define VIC_SCREEN_BORDERHEIGHT		display_ystart
#endif
#define VIC_NUM_SPRITES		 0

#define VIC_WINDOW_TITLE	 "VICE: VIC20 emulator"

#define VIC_NUM_COLORS 16

/* Video mode definitions. */

#define VIC_STANDARD_MODE           0
#define VIC_REVERSE_MODE	    1
#define VIC_NUM_VMODES		    2
#define VIC_IDLE_MODE		    VIC_STANDARD_MODE

/* Define proper screen constants for raster.c. */

#ifdef _VIC_C

#include "vic20.h"
#define CYCLES_PER_LINE                 VIC20_PAL_CYCLES_PER_LINE

#define __VIC__
#define NEEDS_GetData
#define NEEDS_GetCharData
#define SCREEN_WIDTH			VIC_SCREEN_WIDTH
#define SCREEN_HEIGHT			VIC_SCREEN_HEIGHT
#define SCREEN_XPIX			VIC_SCREEN_XPIX
#define SCREEN_YPIX			VIC_SCREEN_YPIX
#define SCREEN_TEXTCOLS			VIC_SCREEN_TEXTCOLS
#define SCREEN_MAX_XPIX			VIC_SCREEN_MAX_XPIX
#define SCREEN_MAX_YPIX			VIC_SCREEN_MAX_YPIX
#define SCREEN_MAX_TEXTCOLS		VIC_SCREEN_MAX_TEXTCOLS
#define SCREEN_MAX_HEIGHT		VIC_SCREEN_MAX_HEIGHT
#define SCREEN_TEXTLINES		VIC_SCREEN_TEXTLINES
#define SCREEN_BORDERWIDTH		VIC_SCREEN_BORDERWIDTH
#define SCREEN_BORDERHEIGHT		VIC_SCREEN_BORDERHEIGHT
#define SCREEN_NUM_VMODES		VIC_NUM_VMODES
#define SCREEN_CYCLES_PER_LINE		VIC_CYCLES_PER_LINE
#define SCREEN_RFSH_PER_SEC             VIC_RFSH_PER_SEC
#define SCREEN_CHARHEIGHT		VIC_SCREEN_CHARHEIGHT
#define SCREEN_NUM_SPRITES              VIC_NUM_SPRITES
#define SCREEN_NUM_COLORS               VIC_NUM_COLORS
#define SCREEN_IDLE_MODE		VIC_IDLE_MODE
#define SCREEN_LAST_DISPLAYED_LINE      VIC_SCREEN_LAST_DISPLAYED_LINE
#define SCREEN_FIRST_DISPLAYED_LINE	VIC_SCREEN_FIRST_DISPLAYED_LINE

#define SCREEN_BORDERWIDTH_VARIES
#define SCREEN_BORDERHEIGHT_VARIES

#endif

/* ------------------------------------------------------------------------- */

extern int vic_init_resources(void);
extern int vic_init_cmdline_options(void);
extern void video_resize(void);
extern void video_free(void);
extern int int_rasterdraw(long offset);
extern canvas_t vic_init(void);
extern void vic_prevent_clk_overflow(CLOCK sub);
extern void vic_exposure_handler(unsigned int width, unsigned int height);
extern BYTE REGPARM1 read_vic(ADDRESS addr);
extern void REGPARM2 store_vic(ADDRESS addr, BYTE value);

extern int vic_write_snapshot_module(snapshot_t *s);
extern int vic_read_snapshot_module(snapshot_t *s);

#ifdef USE_VIDMODE_EXTENSION
extern void video_setfullscreen(int v, int width, int height);
#endif

#endif /* _VIC_H */
