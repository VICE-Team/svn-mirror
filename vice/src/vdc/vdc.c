/*
 * vdc.c - MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner <markus@brenner.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "alarm.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "raster.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "resources.h"
#include "screenshot.h"
#include "snapshot.h"
#include "vdc-cmdline-options.h"
#include "vdc-draw.h"
#include "vdc-resources.h"
#include "vdc-snapshot.h"
#include "vdc.h"
#include "vdctypes.h"
#include "video.h"

#ifdef __MSDOS__
#include "videoarch.h"
#endif

vdc_t vdc;

static void vdc_raster_draw_alarm_handler(CLOCK offset, void *data);

static void vdc_set_geometry(void)
{
    raster_t *raster;
    unsigned int screen_width, screen_height;
    unsigned int first_displayed_line, last_displayed_line;
    unsigned int screen_xpix, screen_ypix;
    unsigned int border_height, border_width;
    unsigned int vdc_25row_start_line, vdc_25row_stop_line;
    unsigned int displayed_width, displayed_height;
    unsigned int vdc_80col_start_pixel, vdc_80col_stop_pixel;

    raster = &vdc.raster;

    screen_width = VDC_SCREEN_WIDTH;
    screen_height = vdc.screen_height;

    first_displayed_line = vdc.first_displayed_line;
    last_displayed_line = vdc.last_displayed_line;

    screen_xpix = vdc.screen_xpix;
    screen_ypix = vdc.screen_ypix;

    border_width = vdc.border_width;
    border_height = vdc.border_height;

    vdc_25row_start_line = border_height;
    vdc_25row_stop_line = vdc_25row_start_line + screen_ypix;

    vdc_80col_start_pixel = border_width;
    vdc_80col_stop_pixel = vdc_80col_start_pixel + 8 * vdc.screen_text_cols;

    displayed_width = VDC_SCREEN_WIDTH;
    displayed_height = last_displayed_line - first_displayed_line + 1;

/*
printf("SH: %03i SW: %03i\n", screen_height, screen_width);
printf("YP: %03i XP: %03i\n", screen_ypix, screen_xpix);
printf("DH: %03i DW: %03i\n", displayed_height, displayed_width);
printf("BH: %03i BW: %03i\n", border_height, border_width);
printf("SA: %03i SO: %03i\n", vdc_25row_start_line, vdc_25row_stop_line);
printf("LD: %03i FD: %03i\n", last_displayed_line, first_displayed_line);
*/

    raster->display_ystart = vdc_25row_start_line;
    raster->display_ystop = vdc_25row_stop_line;
    raster->display_xstart = vdc_80col_start_pixel;
    raster->display_xstop = vdc_80col_stop_pixel;

    raster_set_geometry(raster,
                        displayed_width, displayed_height,
                        screen_width, screen_height,
                        screen_xpix, screen_ypix,
                        VDC_SCREEN_MAX_TEXTCOLS, vdc.screen_textlines,
                        border_width, vdc_25row_start_line,
                        0,
                        first_displayed_line,
                        last_displayed_line,
                        0, 0);
}

static void vdc_invalidate_cache(raster_t *raster, unsigned int screen_height)
{
    raster_new_cache(raster, screen_height);
}

static video_cbm_palette_t vdc_palette =
{
    VDC_NUM_COLORS,
    NULL,
    0,
    0
};

static int init_raster(void)
{
    raster_t *raster;

    raster = &vdc.raster;

    raster->sprite_status = NULL;
    raster_line_changes_init(raster);

    if (raster_init(raster, VDC_NUM_VMODES) < 0)
        return -1;

    raster_modes_set_idle_mode(raster->modes, VDC_IDLE_MODE);
    resources_touch("VDCVideoCache");

    vdc_set_geometry();

    video_color_palette_internal(vdc.raster.canvas, &vdc_palette);
    if (video_color_update_palette(vdc.raster.canvas) < 0) {
        log_error(vdc.log, "Cannot load palette.");
        return -1;
    }

    raster_set_title(raster, machine_name);

    if (raster_realize(raster) < 0)
        return -1;

    raster->border_color = 0;

    return 0;
}


int vdc_init_resources(void)
{
    return vdc_resources_init();
}

int vdc_init_cmdline_options(void)
{
    return vdc_cmdline_options_init();
}


/* Initialize the VDC emulation. */
raster_t *vdc_init(void)
{
    vdc.initialized = 0;

    vdc.log = log_open("VDC");

    vdc.raster_draw_alarm = alarm_new(maincpu_alarm_context, "VdcRasterDraw",
                                      vdc_raster_draw_alarm_handler, NULL);

    vdc_powerup();

    if (init_raster() < 0)
        return NULL;

    vdc.force_resize = 0;
    vdc.force_repaint = 0;

    vdc_draw_init();

    vdc.initialized = 1;

    /*vdc_set_geometry();*/
    resources_touch("VDCDoubleSize");

    return &vdc.raster;
}

struct video_canvas_s *vdc_get_canvas(void)
{
    return vdc.raster.canvas;
}


static void vdc_set_next_alarm(CLOCK offset)
{
    unsigned int next_alarm;
    static unsigned int next_line_accu = 0;

    next_line_accu += vdc.xsync_increment;
    next_alarm = next_line_accu >> 16;
    next_line_accu -= (next_alarm << 16);

    /* Set the next draw event. */
    alarm_set(vdc.raster_draw_alarm, maincpu_clk + (CLOCK)next_alarm - offset);
}

static void vdc_update_geometry(void)
{
    vdc.screen_height = (vdc.regs[4] + 1) * ((vdc.regs[9] & 0x1f) + 1)
                        + (vdc.regs[5] & 0x1f);

    vdc.last_displayed_line = MIN(VDC_LAST_DISPLAYED_LINE,
                              vdc.screen_height - 1);

    vdc.border_height = VDC_SCREEN_BORDERHEIGHT + (vdc.regs[7] & 0x7f) / 2
                        - (vdc.regs[24] & 0x1f);

    vdc.screen_textlines = vdc.regs[6];

    vdc.screen_ypix = vdc.regs[6] * ((vdc.regs[9] & 0x1f) + 1);

    vdc.raster_ycounter_max = vdc.regs[9] & 0x1f;

    if (vdc.raster_ycounter_max < 16)
        vdc.bytes_per_char = 16;
    else
        vdc.bytes_per_char = 32;

    if (vdc.regs[1] >= 8 && vdc.regs[1] <= VDC_SCREEN_MAX_TEXTCOLS)
        vdc.screen_text_cols = vdc.regs[1];

    vdc.hsync_shift = 80 + (102 - vdc.regs[2]) * 8;

    if ((VDC_SCREEN_MAX_TEXTCOLS - vdc.screen_text_cols) * 8 < vdc.hsync_shift)
        vdc.hsync_shift = (VDC_SCREEN_MAX_TEXTCOLS - vdc.screen_text_cols) * 8;

    vdc.border_width = VDC_SCREEN_BORDERWIDTH + vdc.hsync_shift;

    vdc.update_geometry = 0;
}


/* Reset the VDC chip */
void vdc_reset(void)
{
    if (vdc.initialized)
        raster_reset(&vdc.raster);

    vdc.cursor_visible = 0;
    vdc.cursor_frequency = 0;
    vdc.cursor_counter = 0;
    vdc.text_blink_frequency = 32;
    vdc.text_blink_counter = 0;
    vdc.text_blink_visible = 0;
    vdc.screen_text_cols = VDC_SCREEN_MAX_TEXTCOLS;
    vdc.xsmooth = 0;
    vdc.regs[0] = 126;
    vdc.regs[1] = 102;
    vdc.xchars_total = vdc.regs[0] + 1;
    vdc_calculate_xsync();
    vdc.regs[4] = 39;
    vdc.regs[5] = 0;
    vdc.regs[6] = 25;
    vdc.regs[9] = 7;
    vdc_update_geometry();
    vdc_set_next_alarm((CLOCK)0);
}

/* This _should_ put the VDC in the same state as powerup */
void vdc_powerup(void)
{
    memset(vdc.regs, 0, sizeof(vdc.regs));
    vdc.mem_counter = 0;
    vdc.mem_counter_inc = 0;

    vdc.screen_xpix = VDC_SCREEN_XPIX;
    vdc.first_displayed_line = VDC_FIRST_DISPLAYED_LINE;
    vdc.last_displayed_line = VDC_LAST_DISPLAYED_LINE;

    vdc_reset();
}

/* Set the memory pointers according to the values in the registers. */
void vdc_update_memory_ptrs(unsigned int cycle)
{
}

static void vdc_increment_memory_pointer(void)
{
    vdc.mem_counter_inc = vdc.screen_text_cols;
    if (vdc.raster.ycounter >= vdc.raster_ycounter_max)
        vdc.mem_counter += vdc.mem_counter_inc + vdc.regs[27];

    vdc.raster.ycounter = (vdc.raster.ycounter + 1)
                          % (vdc.raster_ycounter_max + 1);

    vdc.bitmap_counter += vdc.mem_counter_inc + vdc.regs[27];
}

static void vdc_set_video_mode(void)
{
    vdc.raster.video_mode = (vdc.regs[25] & 0x80)
                            ? VDC_BITMAP_MODE : VDC_TEXT_MODE;

    if (vdc.raster.ycounter > (unsigned int)(vdc.regs[23] & 0x1f))
        vdc.raster.video_mode = VDC_IDLE_MODE;
}


/* Redraw the current raster line. */
static void vdc_raster_draw_alarm_handler(CLOCK offset, void *data)
{
    int in_visible_area, in_idle_state;

    in_visible_area = (vdc.raster.current_line
                      >= vdc.first_displayed_line
                      && vdc.raster.current_line
                      <= vdc.last_displayed_line);
    in_idle_state = (vdc.raster.current_line < vdc.border_height)
                    || (vdc.raster.current_line >
                    (vdc.border_height + vdc.screen_ypix));

    if (vdc.raster.current_line == vdc.first_displayed_line * 2 + 1) {
        vdc.screen_adr = ((vdc.regs[12] << 8) | vdc.regs[13])
                         & vdc.vdc_address_mask;
        vdc.attribute_adr = ((vdc.regs[20] << 8) | vdc.regs[21])
                            & vdc.vdc_address_mask;
    }

    if (vdc.raster.current_line == 0) {
        vdc.mem_counter = 0;
        vdc.bitmap_counter = 0;
        vdc.raster.ycounter = 0;

        if (vdc.cursor_frequency > 0) {
            if (vdc.cursor_counter == 0) {
                vdc.cursor_visible ^= 1;
                vdc.cursor_counter = vdc.cursor_frequency;
            }
            vdc.cursor_counter--;
        }

        if (vdc.text_blink_frequency > 0) {
            if (vdc.text_blink_counter == 0) {
                vdc.text_blink_visible ^= 1;
                vdc.text_blink_counter = vdc.text_blink_frequency;
            }
            vdc.text_blink_counter--;
        }


        if (vdc.update_geometry) {
            vdc_update_geometry();
            vdc.force_resize = 1;
            vdc.force_repaint = 1;
            /* Screen height has changed, so do not invalidate cache with
               the new value.  It will be recreated by resize anyway.  */
            vdc.force_cache_flush = 0;
        } else {
            if (vdc.force_cache_flush) {
                vdc_invalidate_cache(&vdc.raster, vdc.screen_height);
                vdc.force_cache_flush = 0;
            }
        }

        if (vdc.force_resize) {
            if (vdc.initialized) {
                vdc_set_geometry();
                raster_mode_change();
            }
            vdc.force_resize = 0;
        }

        if (vdc.force_repaint) {
            vdc.force_repaint = 0;
            raster_force_repaint(&vdc.raster);
        }
    }

    raster_line_emulate(&vdc.raster);

#ifdef __MSDOS__
    if (vdc.raster.canvas->viewport->update_canvas)
        canvas_set_border_color(vdc.raster.canvas,
                                vdc.raster.border_color);
#endif

    if (in_visible_area && !in_idle_state) {
        vdc_increment_memory_pointer();
        vdc_set_video_mode();
    }

    vdc_set_next_alarm(offset);
}


void vdc_calculate_xsync(void)
{
    double vdc_cycles_per_line, host_cycles_per_second;

    host_cycles_per_second = (double)machine_get_cycles_per_second();

    vdc_cycles_per_line = (double)(vdc.xchars_total) * 8.0
                          * host_cycles_per_second / VDC_DOT_CLOCK;

    vdc.xsync_increment = (unsigned int)(vdc_cycles_per_line * 65536);
}

void vdc_set_canvas_refresh(int enable)
{
    raster_set_canvas_refresh(&vdc.raster, enable);
}

int vdc_write_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_write_module(s);
}

int vdc_read_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_read_module(s);
}

void vdc_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&vdc.raster, screenshot);
}

void vdc_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&vdc.raster, refresh);
}

void vdc_shutdown(void)
{
    raster_shutdown(&vdc.raster);
}

