/*
 * crtc-snapshot.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *
 * 16/24bpp support added by
 *  Steven Tieu <stieu@physics.ubc.ca>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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
#include "maincpu.h"
#include "types.h"



/* Snapshot.  */

static char snap_module_name[] = "CRTC";
#define SNAP_MAJOR 1
#define SNAP_MINOR 0

int crtc_write_snapshot_module (snapshot_t * s)
{
    int i, ef = 0;
    int current_char;
    int screen_rel;
    snapshot_module_t *m;

    /* derive some values */
    current_char = clk - crtc.rl_start;
    screen_rel = crtc.screen_rel;
    if ((crtc.raster.ycounter == crtc.regs[9])
	&& (current_char > crtc.rl_visible)
	&& crtc.henable) {
	screen_rel += crtc.rl_visible * crtc.hw_cols;
    }

    m = snapshot_module_create (s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    /* hardware-options */
    if (ef 
	/* valid video address bits */
        || snapshot_module_write_word (m, (WORD) crtc.vaddr_mask) < 0
	/* which bit selects different charset .. */
        || snapshot_module_write_word (m, (WORD) crtc.vaddr_charswitch) < 0
	/* ...with offset in charset .. */
        || snapshot_module_write_word (m, (WORD) crtc.vaddr_charoffset) < 0
	/* which bit reverses the screen */
        || snapshot_module_write_word (m, (WORD) crtc.vaddr_revswitch) < 0

	/* size of character generator in byte - 1 */
        || snapshot_module_write_word (m, (WORD) crtc.chargen_mask) < 0
	/* offset given by external circuitry */
        || snapshot_module_write_word (m, (WORD) crtc.chargen_offset) < 0

	/* hardware cursor enabled? */
        || snapshot_module_write_byte (m, (BYTE) (crtc.hw_cursor ? 1 : 0)) < 0
	/* hardware column per character clock cycle */
        || snapshot_module_write_byte (m, (BYTE) crtc.hw_cols) < 0
	/* (external) hardware blanked */
        || snapshot_module_write_byte (m, (BYTE) crtc.hw_blank) < 0
        ) {
        ef = -1;
    }

    /* save the registers */
    for (i = 0; (!ef) && (i < 20); i++) {
        ef = snapshot_module_write_byte (m, crtc.regs[i]);
    }

    /* save the internal state of the CRTC counters */
    if (ef
	/* index in CRTC register file */
        || snapshot_module_write_byte (m, (BYTE) crtc.regno) < 0
	/* clock in the rasterline */
        || snapshot_module_write_byte (m, (BYTE) current_char) < 0
	/* current character line */
        || snapshot_module_write_byte (m, (BYTE) crtc.current_charline) < 0
	/* rasterline in character */
        || snapshot_module_write_byte (m, (BYTE) crtc.raster.ycounter) < 0

	/* cursor state & counter */
        || snapshot_module_write_byte (m, (BYTE) crtc.crsrcnt) < 0
        || snapshot_module_write_byte (m, (BYTE) crtc.crsrstate) < 0
        || snapshot_module_write_byte (m, (BYTE) crtc.cursor_lines) < 0

	/* memory pointer */
        || snapshot_module_write_word (m, (WORD) crtc.chargen_rel) < 0
        || snapshot_module_write_word (m, (WORD) screen_rel) < 0

	/* vsync */
        || snapshot_module_write_word (m, (WORD) crtc.vsync) < 0
	/* venable */
        || snapshot_module_write_byte (m, (BYTE) crtc.venable) < 0
	) {
	ef = -1;
    }

    /* VICE-dependent runtime variables */
    if (ef 
	/* screen size */
        || snapshot_module_write_word (m, (WORD) crtc.screen_width) < 0
        || snapshot_module_write_word (m, (WORD) crtc.screen_height) < 0

	/* horizontal centering */
        || snapshot_module_write_word (m, (WORD) crtc.screen_xoffset) < 0
	/* horizontal jitter */
        || snapshot_module_write_word (m, (WORD) crtc.hjitter) < 0

	/* vertical centering */
        || snapshot_module_write_word (m, (WORD) crtc.screen_yoffset) < 0

	/* expected number of rasterlines for the frame */
        || snapshot_module_write_word (m, (WORD) crtc.framelines) < 0
	/* current frameline */
        || snapshot_module_write_word (m, (WORD) crtc.current_line) < 0
        ) {
        ef = -1;
    }

    
    if (ef) {
        snapshot_module_close (m);
    } else {
        ef = snapshot_module_close (m);
    }

    return ef;
}



int crtc_read_snapshot_module (snapshot_t * s)
{
    int i, ef = 0;
    snapshot_module_t *m;
    WORD w;
    BYTE b;
    BYTE major, minor;

    m = snapshot_module_open (s, snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    if (major != SNAP_MAJOR) {
        log_error (crtc.log,
		 "Major snapshot number (%d) invalid; %d expected.",
		 major, SNAP_MAJOR);
        snapshot_module_close (m);
        return -1;
    }

    /* hardware-options */
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.vaddr_mask = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.vaddr_charswitch = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.vaddr_charoffset = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.vaddr_revswitch = w;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.chargen_mask = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.chargen_offset = w;

    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.hw_cursor = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.hw_cols = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.hw_blank = b;

    crtc.rl_start = clk;	/* just to be sure */

    /* read the registers */
    for (i = 0; (!ef) && (i < 20); i++) {
        if (!(ef = snapshot_module_read_byte (m, &b))) {
	    crtc_store(0, i);
	    crtc_store(1, b);
	}
    }

    /* save the internal state of the CRTC counters */
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.regno = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
        crtc.rl_start = clk - b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.current_charline = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.raster.ycounter = b;

    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.crsrcnt = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.crsrstate = b;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.cursor_lines = b;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.chargen_rel = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.screen_rel = w;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.vsync = w;
    if ((!ef) && !(ef = snapshot_module_read_byte (m, &b)))
	crtc.venable = b;

    /* VICE-dependent runtime variables */
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.screen_width = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.screen_height = w;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.screen_xoffset = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.hjitter = w;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.screen_yoffset = w;

    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.framelines = w;
    if ((!ef) && !(ef = snapshot_module_read_word (m, &w)))
	crtc.current_line = w;

    crtc.raster.current_line = crtc.current_line + crtc.screen_yoffset;

/* FIXME: compatibility mode for old snapshots */
#if 0
  if (snapshot_module_read_byte (m, &b) < 0)
    goto fail;
  /* for the moment simply ignore this value */

  if (snapshot_module_read_word (m, &w) < 0)
    goto fail;
  /* for the moment simply ignore this value */

  if (0
      || snapshot_module_read_word (m, &vmask) < 0
      || snapshot_module_read_byte (m, &hwflags))
    goto fail;

  crtc_set_screen_mode (NULL, vmask, memptr_inc, hwflags);
  crtc_update_memory_ptrs ();

  for (i = 0; i < 20; i++)
    {
      if (snapshot_module_read_byte (m, &b) < 0)
	goto fail;

      /* XXX: This assumes that there are no side effects. 
         Well, there are, but the cursor state is restored later */
      store_crtc (i, b);
    }

  if (snapshot_module_read_byte (m, &b) < 0)
    goto fail;
  crsrcnt = b & 0x3f;
  crsrstate = (b & 0x80) ? 1 : 0;

  alarm_set (&raster_draw_alarm, clk + CYCLES_PER_LINE /* - RASTER_CYCLE */ );

  SIGNAL_VERT_BLANK_OFF

    force_repaint ();
#endif

    crtc_update_window();

    if (ef) {
	log_error(crtc.log, "Failed to load snapshot module %s",
		snap_module_name); 
  	snapshot_module_close (m);
    } else {
  	ef = snapshot_module_close (m);
    }
    return ef;
}
