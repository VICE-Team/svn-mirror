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
#include "log.h"

#ifdef _CRTC_C
#define __CRTC__

/* border around the crtc screen in the window. We do not have border effects,
   so keep it small. */
#define SCREEN_BORDERWIDTH		 12
#define SCREEN_BORDERHEIGHT		 12

/* Size of the screen for the timing */
#define CYCLES_PER_LINE			crtc_cycles_per_line
#define	SCREEN_LAST_RASTERLINE		(screen_rasterlines - 1)

/* size of chars in the chargen_rom in bytes/char */
#define	BYTES_PER_CHAR			16

/* size of the bitmaped rasterline cache */
#define CRTC_SCREEN_MAX_YPIX		(16*35)
#define CRTC_SCREEN_MAX_TEXTCOLS	(100)

/* size of character in pixels, and derived window size in pixels */
#define	SCREEN_CHARHEIGHT		screen_charheight
#define	SCREEN_XPIX			screen_xpix
#define	SCREEN_YPIX			screen_ypix

/* size of crtc window for the display - variable */
#define	SCREEN_HEIGHT			(SCREEN_YPIX + 2 * SCREEN_BORDERHEIGHT)
#define	SCREEN_WIDTH			(SCREEN_XPIX + 2 * SCREEN_BORDERWIDTH)

/* size of character-filled window part - variable */
#define	SCREEN_TEXTCOLS			memptr_inc
#define	SCREEN_TEXTLINES		crtc_screen_textlines

/*  Define proper screen constants for raster.c. */

/* some constants */
#define CRTC_STANDARD_MODE		0
#define CRTC_REVERSE_MODE       	1
#define CRTC_IDLE_MODE			CRTC_STANDARD_MODE
#define CRTC_NUM_COLORS 		2

#define SCREEN_NUM_COLORS 		CRTC_NUM_COLORS
#define SCREEN_NUM_VMODES		2
#define SCREEN_IDLE_MODE		CRTC_IDLE_MODE

#define NEEDS_GetCharData

#define SCREEN_MAX_HEIGHT		(CRTC_SCREEN_MAX_YPIX \
						+ 2*SCREEN_BORDERHEIGHT)
#define SCREEN_MAX_TEXTCOLS		CRTC_SCREEN_MAX_TEXTCOLS
#define SCREEN_FIRST_DISPLAYED_LINE	SCREEN_BORDERHEIGHT
#define SCREEN_LAST_DISPLAYED_LINE      (SCREEN_BORDERHEIGHT-1+SCREEN_YPIX)


/* Framebuffer size. The buffer itself is fixed size,
   only the data area changes. The data area is determined by the 
   SCREEN_HEIGHT and SCREEN_WIDTH values. Those values times the current
   pixel_height/width must never be larger then the framebuffer */
#define	FRAMEB_WIDTH			(8*CRTC_SCREEN_MAX_TEXTCOLS \
						+ 2*SCREEN_BORDERWIDTH)
#define FRAMEB_HEIGHT			(CRTC_SCREEN_MAX_YPIX \
						+ 2*SCREEN_BORDERHEIGHT)
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

#ifdef USE_VIDMODE_EXTENSION
extern void video_setfullscreen(int v,int width, int height);
#endif

extern log_t crtc_log;

#endif				/* _CRTC_H */
