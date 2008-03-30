/*
 * crtc-snapshot.c - A line-based CRTC emulation (under construction).
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

#include "vice.h"

#include "crtc.h"
#include "crtc-snapshot.h"

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

    alarm_set(&raster_draw_alarm, clk + CYCLES_PER_LINE /* - RASTER_CYCLE*/);

    SIGNAL_VERT_BLANK_OFF

    force_repaint();
    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

