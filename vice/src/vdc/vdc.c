/*
 * vdc.c - A first attemt at a MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner (markus@brenner.de)
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

#define _VDC_C

#include "vice.h"

#include "alarm.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "palette.h"
#include "snapshot.h"
#include "utils.h"

#include "vdc-cmdline-options.h"
#include "vdc-draw.h"
#include "vdc-resources.h"
#include "vdc-snapshot.h"

#include "vdc.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef __riscos
#include "ROlib.h"
#endif



vdc_t vdc;

#define VDC_CYCLES_PER_LINE() vdc.regs[0]



static void init_raster(void)
{
    raster_t *raster;
    unsigned int width, height;
    char *title;

    raster = &vdc.raster;

    raster_init(raster, VDC_NUM_VMODES, VDC_NUM_SPRITES);
    raster_modes_set_idle_mode(&raster->modes, VDC_IDLE_MODE);
    raster_set_exposure_handler(raster, vdc_exposure_handler);
    raster_enable_cache(raster, vdc_resources.video_cache_enabled);
    raster_enable_double_scan(raster, vdc_resources.double_scan_enabled);

/*
    width = VDC_SCREEN_XPIX + VDC_SCREEN_BORDERWIDTH * 2;
    height = VDC_LAST_DISPLAYED_LINE - VDC_FIRST_DISPLAYED_LINE;
*/
    width = VDC_SCREEN_WIDTH;
    height = VDC_SCREEN_HEIGHT;

    if (vdc_resources.double_size_enabled)
    {
        width *= 2;
        height *= 2;
        raster_set_pixel_size (raster, 2, 2);
    }

    raster_set_geometry(raster,
                        VDC_SCREEN_WIDTH, VDC_SCREEN_HEIGHT,
                        VDC_SCREEN_XPIX, VDC_SCREEN_YPIX,
                        VDC_SCREEN_TEXTCOLS, VDC_SCREEN_TEXTLINES,
                        VDC_SCREEN_BORDERWIDTH, VDC_SCREEN_BORDERHEIGHT,
                        FALSE,
                        VDC_FIRST_DISPLAYED_LINE,
                        VDC_LAST_DISPLAYED_LINE,
                        0);
    raster_resize_viewport(raster, width, height);

    if (vdc_load_palette(vdc_resources.palette_file_name) < 0)
        log_error(vdc.log, "Cannot load palette.");

    title = concat("VICE: ", machine_name, " emulator VDC window", NULL);
    raster_set_title(raster, title);
    free(title);

    raster_realize(raster);

    raster->display_ystart = VDC_25ROW_START_LINE;
    raster->display_ystop = VDC_25ROW_STOP_LINE;
    raster->display_xstart = VDC_80COL_START_PIXEL;
    raster->display_xstop = VDC_80COL_STOP_PIXEL;
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
canvas_t vdc_init(void)
{
    vdc.log = log_open("VDC");

    alarm_init(&vdc.raster_draw_alarm, &maincpu_alarm_context,
               "VdcRasterDraw", vdc_raster_draw_alarm_handler);

    init_raster();
    vdc_powerup();

/*
    vdc_update_video_mode(0);
    vdc_update_memory_ptrs(0);
*/

    vdc_draw_init();
    vdc_draw_set_double_size(vdc_resources.double_size_enabled);

    vdc.initialized = 1;

    return vdc.raster.viewport.canvas;
}

/* Reset the VDC chip */
void vdc_reset(void)
{
    raster_reset(&vdc.raster);
    vdc.regs[0] = 49;
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
    log_message(vdc.log, "exposure_handler");
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

int vdc_raster_draw_alarm_handler(long offset)
{
    int in_visible_area;

/*
    log_message(vdc.log, "raster_draw_alarm - line %d",vdc.raster.current_line);
*/
    raster_emulate_line(&vdc.raster);

    in_visible_area = (vdc.raster.current_line >= VDC_FIRST_DISPLAYED_LINE
                    && vdc.raster.current_line <= VDC_LAST_DISPLAYED_LINE);

    if (vdc.raster.current_line == 0)
    {
        vdc.mem_counter = 0; 
        vdc.bitmap_counter = 0;
        vdc.raster.ycounter = 0;
    }

    if (in_visible_area)
    {
        vdc.mem_counter_inc = VDC_SCREEN_TEXTCOLS;
        vdc.bitmap_counter += vdc.mem_counter_inc;
        if (vdc.raster.ycounter == 7)
        {
            vdc.mem_counter += vdc.mem_counter_inc;
        }
        vdc.raster.ycounter = (vdc.raster.ycounter + 1) & 0x07;
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
    if (!vdc.initialized)
        return;

    if (vdc_resources.double_size_enabled)
    {
        if (vdc.raster.viewport.pixel_size.width == 1
            && vdc.raster.viewport.canvas != NULL)
            raster_resize_viewport(&vdc.raster,
                                   vdc.raster.viewport.width * 2,
                                   vdc.raster.viewport.height * 2);

        raster_set_pixel_size(&vdc.raster, 2, 2);
        vdc_draw_set_double_size(1);
    }
    else
    {
        if (vdc.raster.viewport.pixel_size.width == 2
            && vdc.raster.viewport.canvas != NULL)
            raster_resize_viewport(&vdc.raster,
                                   vdc.raster.viewport.width / 2,
                                   vdc.raster.viewport.height / 2);
     
        raster_set_pixel_size(&vdc.raster, 1, 1);
        vdc_draw_set_double_size(0);
    }
}


int vdc_write_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_write_module(s);
}


int vdc_read_snapshot_module(snapshot_t *s)
{
    return vdc_snapshot_read_module(s);
}



