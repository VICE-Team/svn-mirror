/*
 * vdc.c - A first attemt at a MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner <markus@brenner.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdlib.h>
#include <stdio.h>

#include "alarm.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "palette.h"
#include "raster-modes.h"
#include "snapshot.h"
#include "utils.h"
#include "vdc-cmdline-options.h"
#include "vdc-draw.h"
#include "vdc-resources.h"
#include "vdc-snapshot.h"
#include "vdc.h"
#include "vdctypes.h"



vdc_t vdc;

#define VDC_CYCLES_PER_LINE() vdc.regs[0]



static void vdc_set_geometry(void)
{
    raster_t *raster;
    unsigned int width, height;

    raster = &vdc.raster;

    width = VDC_SCREEN_WIDTH;
    height = vdc.screen_height;

    raster_set_geometry(raster,
                        VDC_SCREEN_WIDTH, vdc.screen_height,
                        VDC_SCREEN_XPIX, vdc.screen_ypix,
                        VDC_SCREEN_TEXTCOLS, VDC_SCREEN_TEXTLINES,
                        VDC_SCREEN_BORDERWIDTH, VDC_SCREEN_BORDERHEIGHT,
                        0,
                        VDC_FIRST_DISPLAYED_LINE,
                        vdc.last_displayed_line,
                        0);
    raster_resize_viewport(raster, width, height);

    raster->display_ystart = VDC_25ROW_START_LINE;
    raster->display_ystop = vdc.row25_stop_line;
    raster->display_xstart = VDC_80COL_START_PIXEL;
    raster->display_xstop = VDC_80COL_STOP_PIXEL;
}

static int init_raster(void)
{
    raster_t *raster;
    char *title;

    raster = &vdc.raster;

    raster_init(raster, VDC_NUM_VMODES, VDC_NUM_SPRITES);
    raster_modes_set_idle_mode(raster->modes, VDC_IDLE_MODE);
    raster_set_exposure_handler(raster, vdc_exposure_handler);
    raster_enable_cache(raster, vdc_resources.video_cache_enabled);
    raster_enable_double_scan(raster, 0);
    raster_set_canvas_refresh(raster, 1);
/*
    width = VDC_SCREEN_XPIX + VDC_SCREEN_BORDERWIDTH * 2;
    height = VDC_LAST_DISPLAYED_LINE - VDC_FIRST_DISPLAYED_LINE;
*/
    vdc_set_geometry();

    if (vdc_load_palette(vdc_resources.palette_file_name) < 0) {
        log_error(vdc.log, "Cannot load palette.");
        return -1;
    }

    title = concat("VICE: ", machine_name, " emulator VDC window", NULL);
    raster_set_title(raster, title);
    free(title);

    raster_realize(raster);

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
void *vdc_init(void)
{
    vdc.initialized = 0;

    vdc.log = log_open("VDC");

    alarm_init(&vdc.raster_draw_alarm, &maincpu_alarm_context,
               "VdcRasterDraw", vdc_raster_draw_alarm_handler);

    vdc_resize();

    if (init_raster() < 0)
        return NULL;
    vdc_powerup();

/*
    vdc_update_video_mode(0);
    vdc_update_memory_ptrs(0);
*/

    vdc.force_resize = 0;
    vdc.force_repaint = 0;

    vdc_draw_init();
    vdc_draw_set_double_size(/*vdc_resources.double_size_enabled*/ 0);

    vdc.initialized = 1;

    return (void *)vdc.raster.viewport.canvas;
}

/* Reset the VDC chip */
void vdc_reset(void)
{
    raster_reset(&vdc.raster);
    vdc.regs[0] = 49;
    vdc.cursor_visible = 0;
    vdc.cursor_frequency = 0;
    vdc.cursor_counter = 0;
    vdc.text_blink_frequency = 32;
    vdc.text_blink_counter = 0;
    vdc.text_blink_visible = 0;
    alarm_set(&vdc.raster_draw_alarm, VDC_CYCLES_PER_LINE());
}

/* This _should_ put the VDC in the same state as powerup */
void vdc_powerup(void)
{
    memset(vdc.regs, 0, sizeof(vdc.regs));
    vdc.mem_counter = 0;
    vdc.mem_counter_inc = 0; 

    vdc_reset();
}

/* Handle the exposure event. */
void vdc_exposure_handler(unsigned int width, unsigned int height)
{
    raster_resize_viewport(&vdc.raster, width, height);
    /* FIXME: Needed? Maybe this should be triggered by
      `raster_resize_viewport()' automatically. */
    raster_force_repaint(&vdc.raster);
}

/* Set the memory pointers according to the values in the registers. */
void vdc_update_memory_ptrs(unsigned int cycle)
{
}


/* Redraw the current raster line. */

int vdc_raster_draw_alarm_handler(CLOCK offset)
{
    int in_visible_area;

    in_visible_area = (vdc.raster.current_line >= VDC_FIRST_DISPLAYED_LINE
                    && vdc.raster.current_line <= vdc.last_displayed_line);

    if (vdc.raster.current_line == 0)
    {
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

        if (vdc.force_resize) {
            vdc.force_resize = 0;
            vdc_resize();
        }

        if (vdc.force_repaint) {
            vdc.force_repaint = 0;
            raster_force_repaint(&vdc.raster);
        }
    }

    raster_emulate_line(&vdc.raster);

#ifdef __MSDOS__
    if (vdc.raster.viewport.update_canvas)
        canvas_set_border_color (vdc.raster.viewport.canvas,
                                 vdc.raster.border_color);
#endif

    if (in_visible_area)
    {
        vdc.mem_counter_inc = VDC_SCREEN_TEXTCOLS;
        if (vdc.raster.ycounter >= vdc.raster_ycounter_max)
        {
            vdc.mem_counter += vdc.mem_counter_inc;
        }
        vdc.raster.ycounter = (vdc.raster.ycounter + 1)
                              & vdc.raster_ycounter_max;

        if (!(vdc.raster.ycounter & 1))
        {
            /* Don't increment on odd raster scanlines.  */
            vdc.bitmap_counter += vdc.mem_counter_inc;
        }
    }

    /* Set the next draw event. */
    alarm_set(&vdc.raster_draw_alarm, clk + VDC_CYCLES_PER_LINE() - offset);

    return 0;
}



/* WARNING: This does not change the resource value. External modules are
   expected to set the resource value to change the VDC palette instead of
   calling this function directly. */

int vdc_load_palette(const char *name)
{
    static const char *color_names[VDC_NUM_COLORS] =
    {
        "Black",    "Medium Gray", "Blue",        "Light Blue",
        "Green",    "Light Green", "Dark Cyan",   "Light Cyan",
        "Dark Red", "Light Red",   "Dark Purple", "Light Purple",
        "Brown",    "Yellow",      "Light Gray",  "White"
    };
    palette_t *palette;

    palette = palette_create(VDC_NUM_COLORS, color_names);
    if (palette == NULL)
        return -1;

    if (palette_load(name, palette) < 0)
    {
        log_message(vdc.log, "Cannot load palette file '%s'.", name);
        return -1;
    }
    
    raster_set_palette(&vdc.raster, palette);
    return 0;
}


/* Set proper functions and constants for the current video settings. */
void vdc_resize(void)
{
    if (vdc_resources.double_size_enabled) {
        vdc.screen_height = VDC_SCREEN_HEIGHT_LARGE;
        vdc.screen_ypix = VDC_SCREEN_YPIX_LARGE;
        vdc.last_displayed_line = VDC_LAST_DISPLAYED_LINE_LARGE;
        vdc.row25_stop_line = VDC_25ROW_STOP_LINE_LARGE;
        vdc.raster_ycounter_max = VDC_SCREEN_CHARHEIGHT_LARGE - 1;
        vdc.raster_ycounter_divide = 2;
    } else {
        vdc.screen_height = VDC_SCREEN_HEIGHT_SMALL;
        vdc.screen_ypix = VDC_SCREEN_YPIX_SMALL;
        vdc.last_displayed_line = VDC_LAST_DISPLAYED_LINE_SMALL;
        vdc.row25_stop_line = VDC_25ROW_STOP_LINE_SMALL;
        vdc.raster_ycounter_max = VDC_SCREEN_CHARHEIGHT_SMALL - 1;
        vdc.raster_ycounter_divide = 1;
    }

    if (!vdc.initialized)
        return;

    vdc_set_geometry();
}

void vdc_set_canvas_refresh(int enable)
{
    raster_t *raster;

    raster = &vdc.raster;

    raster_set_canvas_refresh(raster, enable);
}


int vdc_write_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_write_module(s);
}


int vdc_read_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_read_module(s);
}



