/*
 * vic.c - A line-based VIC-I emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <stdio.h>
#include <stdlib.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "resources.h"
#include "screenshot.h"
#include "snapshot.h"
#include "types.h"
#include "vic-cmdline-options.h"
#include "vic-draw.h"
#include "vic-mem.h"
#include "vic-resources.h"
#include "vic-snapshot.h"
#include "vic-color.h"
#include "vic.h"
#include "vic20.h"
#include "vic20mem.h"
#include "vic20memrom.h"
#include "videoarch.h"
#include "viewport.h"
#include "vsync.h"


vic_t vic;

static void vic_set_geometry(void);


void vic_change_timing(void)
{
    int mode;

    resources_get_int("MachineVideoStandard", &mode);

    switch (mode) {
      case MACHINE_SYNC_NTSC:
        vic.screen_height = VIC20_NTSC_SCREEN_LINES;
        vic.screen_width = VIC_NTSC_SCREEN_WIDTH;
        vic.display_width = VIC_NTSC_DISPLAY_WIDTH;
        vic.first_displayed_line = VIC20_NTSC_FIRST_DISPLAYED_LINE;
        vic.last_displayed_line = VIC20_NTSC_LAST_DISPLAYED_LINE;
        vic.cycles_per_line = VIC20_NTSC_CYCLES_PER_LINE;
        vic.cycle_offset = VIC20_NTSC_CYCLE_OFFSET;
        vic.max_text_cols = VIC_NTSC_MAX_TEXT_COLS;
        break;
      case MACHINE_SYNC_PAL:
      default:
        vic.screen_height = VIC20_PAL_SCREEN_LINES;
        vic.screen_width = VIC_PAL_SCREEN_WIDTH;
        vic.display_width = VIC_PAL_DISPLAY_WIDTH;
        vic.first_displayed_line = VIC20_PAL_FIRST_DISPLAYED_LINE;
        vic.last_displayed_line = VIC20_PAL_LAST_DISPLAYED_LINE;
        vic.cycles_per_line = VIC20_PAL_CYCLES_PER_LINE;
        vic.cycle_offset = VIC20_PAL_CYCLE_OFFSET;
        vic.max_text_cols = VIC_PAL_MAX_TEXT_COLS;
        break;
    }

    if (vic.initialized) {
        vic_set_geometry();
        raster_mode_change();
    }
}

static void vic_set_geometry(void)
{
    unsigned int width, height;

    width = vic.display_width * VIC_PIXEL_WIDTH;
    height = vic.last_displayed_line - vic.first_displayed_line + 1;

    raster_set_geometry(&vic.raster,
                        width, height,
                        vic.screen_width * VIC_PIXEL_WIDTH,
                        vic.screen_height,
                        22 * 8 * VIC_PIXEL_WIDTH,
                        23 * 8,          /* handled dynamically  */
                        22, 23,          /* handled dynamically  */
                        /* handled dynamically  */
                        12 * 4 * VIC_PIXEL_WIDTH,
                        38 * 2 - vic.first_displayed_line,
                        1,
                        vic.first_displayed_line,
                        vic.last_displayed_line,
                        vic.screen_width + vic.max_text_cols * 8,
                        vic.screen_width + vic.max_text_cols * 8);
#ifdef __MSDOS__
    video_ack_vga_mode();
#endif
}


/* Notice: The screen origin X register has a 4-pixel granularity, so our
   write accesses are always aligned. */

void vic_raster_draw_handler(void)
{
    /* emulate the line */
    raster_line_emulate(&vic.raster);

    /* handle start of frame */
    if (vic.raster.current_line == 0) {
        raster_skip_frame(&vic.raster,
                          vsync_do_vsync(vic.raster.canvas,
                                         vic.raster.skip_frame));
    }
}

static void update_pixel_tables(raster_t *raster)
{
    unsigned int i;

    for (i = 0; i < 256; i++) {
        vic.pixel_table.sing[i] = i;
        *((BYTE *)(vic.pixel_table.doub + i))
        = *((BYTE *)(vic.pixel_table.doub + i) + 1)
        = vic.pixel_table.sing[i];
    }
}

static int init_raster(void)
{
    raster_t *raster;

    raster = &vic.raster;
    video_color_set_canvas(raster->canvas);

    raster->sprite_status = NULL;
    raster_line_changes_init(raster);

    if (raster_init(raster, VIC_NUM_VMODES) < 0) {
        return -1;
    }

    update_pixel_tables(raster);

    raster_modes_set_idle_mode(raster->modes, VIC_IDLE_MODE);
    resources_touch("VICVideoCache");

    vic_set_geometry();

    vic_color_update_palette(raster->canvas);

    raster_set_title(raster, machine_name);

    if (raster_realize(raster) < 0) {
        return -1;
    }

    raster->display_ystart = vic.first_displayed_line;
    raster->display_ystop = vic.first_displayed_line + 1;
    raster->display_xstart = 0;
    raster->display_xstop = 1;

    return 0;
}


/* Initialization. */
raster_t *vic_init(void)
{
    vic.log = log_open("VIC");

    vic_change_timing();

    if (init_raster() < 0) {
        return NULL;
    }

    vic.auxiliary_color = 0;
    vic.mc_border_color = 0;
    vic.old_auxiliary_color = 0;
    vic.old_mc_border_color = 0;
    vic.reverse = 0;
    vic.old_reverse = 0;

    /* FIXME: Where do these values come from? */
    vic.light_pen.triggered = 0;
    vic.light_pen.x = 87;
    vic.light_pen.y = 234;

    /* FIXME */
    vic.char_height = 8;
    vic.row_increase_line = 8;
    vic.pending_text_cols = 22;
    vic.text_lines = 23;

    vic_reset();

    vic_draw_init();

    vic.initialized = 1;

    resources_touch("VICDoubleSize");

    return &vic.raster;
}

struct video_canvas_s *vic_get_canvas(void)
{
    return vic.raster.canvas;
}

/* Reset the VIC-I chip. */
void vic_reset(void)
{
/*    vic_change_timing();*/

    raster_reset(&vic.raster);

/*    vic_set_geometry();*/

    vic.row_counter = 0;
    vic.memptr = 0;
    vic.memptr_inc = 0;
    vic.area = VIC_AREA_IDLE;
    vic.raster_line = 0;
    vic.raster_cycle = 6; /* magic value from cpu_reset() (mainviccpu.c) */
    vic.fetch_state = VIC_FETCH_IDLE;
}

void vic_shutdown(void)
{
    raster_shutdown(&vic.raster);
}

void vic_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&vic.raster, screenshot);
}

void vic_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&vic.raster, refresh);
}

/* Trigger the light pen.  */
void vic_trigger_light_pen(CLOCK mclk)
{
    if (!vic.light_pen.triggered) {
        vic.light_pen.triggered = 1;
        vic.light_pen.x = 2 * ((mclk + 1) % vic.cycles_per_line) + 1;
        vic.light_pen.y = VIC_RASTER_Y(mclk) / 2;
    }
}

/* Calculate lightpen pulse time based on x/y */
CLOCK vic_lightpen_timing(int x, int y)
{
    CLOCK pulse_time = maincpu_clk;

    x += 0x70 - vic.cycle_offset;
    y += vic.first_displayed_line;

    /* Check if x would wrap to previous line */
    if (x < 0 /*TODO*/) {
        /* lightpen is off screen */
        pulse_time = 0;
    } else {
        pulse_time += (x / 8) + (y * vic.cycles_per_line);
        /* Remove frame alarm jitter */
        pulse_time -= maincpu_clk - VIC_LINE_START_CLK(maincpu_clk);
    }

    return pulse_time;
}
