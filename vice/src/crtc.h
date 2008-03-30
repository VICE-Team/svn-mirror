/*
 * crtc.h - A CRTC emulation (under construction)
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

#ifndef _CRTC_H
#define _CRTC_H

#include "vice.h"

#include "snapshot.h"
#include "types.h"
#include "video.h"

#define CRTC_CYCLES_PER_LINE         71
#define CRTC_RFSH_PER_SEC            50

#define CYCLES_PER_LINE              CRTC_CYCLES_PER_LINE

#ifdef _CRTC_C
static int crtc_cols = 40;
#define CRTC_SCREEN_TEXTCOLS		 crtc_cols
#define CRTC_SCREEN_HEIGHT	         312
#endif

/* Warning: Some of these values are *bogus*! */
/* This is a mess.  */
#define CRTC_SCREEN_XPIX		 (CRTC_SCREEN_TEXTCOLS * 8)
#define CRTC_SCREEN_YPIX		 200
#define CRTC_SCREEN_MAX_TEXTCOLS	 80
#define CRTC_SCREEN_MAX_TEXTLINES	 CRTC_SCREEN_TEXTLINES
#define CRTC_SCREEN_MAX_XPIX		 (CRTC_SCREEN_MAX_TEXTCOLS * 8)
#define CRTC_SCREEN_MAX_YPIX		 CRTC_SCREEN_YPIX
#define CRTC_SCREEN_TEXTLINES		 25
#define CRTC_SCREEN_BORDERWIDTH		 12
#define CRTC_SCREEN_BORDERHEIGHT	 51
#define CRTC_SCREEN_MAX_HEIGHT		 CRTC_SCREEN_HEIGHT
#define CRTC_SCREEN_WIDTH		 (CRTC_SCREEN_XPIX \
				          + CRTC_SCREEN_BORDERWIDTH * 2)
#define CRTC_SCREEN_CHARHEIGHT		 screen_charheight
#define CRTC_NUM_SPRITES		 0
#define CRTC_SCREEN_FIRST_DISPLAYED_LINE 0x10
#define CRTC_SCREEN_LAST_DISPLAYED_LINE  0x11f

#define CRTC_NUM_COLORS 2

#define CRTC_STANDARD_MODE	    0
#define CRTC_REVERSE_MODE           1
#define CRTC_NUM_VMODES		    2
#define CRTC_IDLE_MODE		    CRTC_STANDARD_MODE

/*  Define proper screen constants for raster.c. */
#ifdef _CRTC_C
#define __CRTC__
#define NEEDS_GetCharData
#define SCREEN_WIDTH			CRTC_SCREEN_WIDTH
#define SCREEN_HEIGHT			CRTC_SCREEN_HEIGHT
#define SCREEN_XPIX			CRTC_SCREEN_XPIX
#define SCREEN_YPIX			CRTC_SCREEN_YPIX
#define SCREEN_TEXTCOLS			CRTC_SCREEN_TEXTCOLS
#define SCREEN_TEXTLINES		CRTC_SCREEN_TEXTLINES
#define SCREEN_MAX_TEXTCOLS		CRTC_SCREEN_MAX_TEXTCOLS
#define SCREEN_MAX_XPIX			CRTC_SCREEN_MAX_XPIX
#define SCREEN_MAX_YPIX			CRTC_SCREEN_MAX_YPIX
#define SCREEN_MAX_HEIGHT		CRTC_SCREEN_MAX_HEIGHT
#define SCREEN_BORDERWIDTH		CRTC_SCREEN_BORDERWIDTH
#define SCREEN_BORDERHEIGHT		CRTC_SCREEN_BORDERHEIGHT
#define SCREEN_NUM_VMODES		CRTC_NUM_VMODES
#define SCREEN_CYCLES_PER_LINE		CRTC_CYCLES_PER_LINE
#define SCREEN_RFSH_PER_SEC             CRTC_RFSH_PER_SEC
#define SCREEN_CHARHEIGHT		CRTC_SCREEN_CHARHEIGHT
#define SCREEN_NUM_SPRITES              CRTC_NUM_SPRITES
#define SCREEN_NUM_COLORS               CRTC_NUM_COLORS
#define SCREEN_FIRST_DISPLAYED_LINE	CRTC_SCREEN_FIRST_DISPLAYED_LINE
#define SCREEN_LAST_DISPLAYED_LINE      CRTC_SCREEN_LAST_DISPLAYED_LINE
#define SCREEN_IDLE_MODE		CRTC_IDLE_MODE

#define	BYTES_PER_CHAR			16

#endif

extern int crtc_init_resources(void);
extern int crtc_init_cmdline_options(void);
extern canvas_t crtc_init(void);
extern void reset_crtc(void);
extern int int_rasterdraw(long offset);
extern void crtc_set_screen_mode(BYTE *base, int vmask, int cols, int hwcrsr);
extern void video_resize(void);
extern void video_free(void);
extern void crtc_prevent_clk_overflow(CLOCK sub);
extern void REGPARM2 store_crtc(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_crtc(ADDRESS addr);
extern void crtc_set_char(int crom);
extern int crtc_offscreen(void);
extern void crtc_screen_enable(int);

extern int crtc_write_snapshot_module(snapshot_t *s);
extern int crtc_read_snapshot_module(snapshot_t *s);

#endif				/* _CRTC_H */
