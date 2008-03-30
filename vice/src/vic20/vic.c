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

#include "alarm.h"
#include "archdep.h"
#include "clkguard.h"
#include "lib.h"
#include "log.h"
#include "mem.h"
#include "machine.h"
#include "maincpu.h"
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
#include "vsync.h"
#include "videoarch.h"


vic_t vic;

static void vic_set_geometry(void);


void vic_change_timing(void)
{
    resource_value_t mode;

    resources_get_value("MachineVideoStandard", (void *)&mode);

    switch ((int)mode) {
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

void vic_raster_draw_alarm_handler(CLOCK offset, void *data)
{
    static int pending_mem_offset;
    static int possible_mem_offset;
    int blank_this_line;

    /* check if first visible line is reached */
    if (vic.area == 0 && vic.raster.current_line >= vic.raster.display_ystart)
        vic.area = 1;

    /* remember if this line stays blank */
    blank_this_line = vic.raster.blank_this_line;
    if (blank_this_line)
        vic.raster.display_ystop++;

    /* check if row step is pending */
    if (vic.row_increase_line == (unsigned int)vic.raster.ycounter
        || 2 * vic.row_increase_line == (unsigned int)vic.raster.ycounter) {
        vic.row_counter++;
        
        vic.raster.ycounter = 0;
        
        vic.raster.display_ystop = vic.raster.current_line 
            + (vic.text_lines - vic.row_counter) * vic.char_height;

        /* if XPOS is 0 VIC displays one more rasterline */
        if (vic.raster.display_xstart == 0)
            vic.raster.display_ystop++;

        pending_mem_offset = possible_mem_offset;
    }

    /*  update memptr */
    if (vic.area == 1)
        vic.memptr += pending_mem_offset;
    
    pending_mem_offset = 0;

    /* max offset for next row */
    possible_mem_offset = vic.text_cols;
    
    /* emulate the line */
    raster_line_emulate(&vic.raster);

    /* xstart may have changed; recalculate xstop */
    vic.raster.display_xstop = vic.raster.display_xstart + vic.text_cols * 8
                               * VIC_PIXEL_WIDTH;
    if (vic.raster.display_xstop >= (int)(vic.screen_width * VIC_PIXEL_WIDTH))
        vic.raster.display_xstop = (int)((vic.screen_width - 1)
                                   * VIC_PIXEL_WIDTH);

    /* increment ycounter and set offset for memptr */
    if (vic.area == 1 && !blank_this_line) {
        vic.raster.ycounter++;

        if (vic.row_offset != 0
            || (unsigned int)vic.raster.ycounter == vic.row_increase_line) {
            /* this only happens if char_height changes between 8 and 16
               within line 7 */
            pending_mem_offset = 
                (vic.row_offset > 0 ? vic.row_offset : possible_mem_offset);
            vic.row_offset = 0;
        }

        if (vic.raster.current_line >= vic.raster.display_ystop)
            vic.area = 2;
    }

    /* handle start of frame */
    if (vic.raster.current_line == 0) {
        raster_skip_frame(&vic.raster,
                          vsync_do_vsync(vic.raster.canvas,
                                         vic.raster.skip_frame));
        vic.raster.blank_enabled = 1;
        vic.row_counter = 0;
        vic.raster.ycounter = 0;
        vic.memptr = 0;
        vic.area = 0;

        if (vic.pending_ystart >= 0) {
            vic.raster.display_ystart = vic.pending_ystart;
            vic.raster.geometry->gfx_position.y = 
                vic.pending_ystart - vic.first_displayed_line;
            vic.raster.display_ystop = 
                vic.raster.display_ystart + vic.text_lines * vic.char_height;

            vic.pending_ystart = -1;
        }

        if (vic.pending_text_lines >= 0) {
            vic.text_lines = vic.pending_text_lines;
            vic.raster.display_ystop = 
                (vic.raster.display_ystart + vic.text_lines * vic.char_height);
            vic.raster.geometry->gfx_size.height = vic.pending_text_lines * 8;
            vic.raster.geometry->text_size.height = vic.pending_text_lines;

            vic.pending_text_lines = -1;
        }

        vic.raster.blank = 0;
        vic.light_pen.triggered = 0;
    }

    /* Set the next draw event.  */
    vic.last_emulate_line_clk += vic.cycles_per_line;
    vic.draw_clk = vic.last_emulate_line_clk + vic.cycles_per_line;
    alarm_set(vic.raster_draw_alarm, vic.draw_clk);
}

static void update_pixel_tables(raster_t *raster)
{
    unsigned int i;

    for (i = 0; i < 256; i++) {
        vic.pixel_table.sing[i] = i;
        *((BYTE *)(vic.pixel_table.doub + i))
        = *((BYTE *)(vic.pixel_table.doub + i) + 1)
        = vic.pixel_table.sing[i];
        *((WORD *)(vic.pixel_table.quad + i))
        = *((WORD *)(vic.pixel_table.quad + i) + 1)
        = vic.pixel_table.doub[i];
    }
}

static int init_raster(void)
{
    raster_t *raster;

    raster = &vic.raster;
    video_color_set_canvas(raster->canvas);

    raster->sprite_status = NULL;
    raster_line_changes_init(raster);

    if (raster_init(raster, VIC_NUM_VMODES) < 0)
        return -1;

    update_pixel_tables(raster);

    raster_modes_set_idle_mode(raster->modes, VIC_IDLE_MODE);
    resources_touch("VICVideoCache");

    vic_set_geometry();

    vic_color_update_palette(raster->canvas);

    raster_set_title(raster, machine_name);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = vic.first_displayed_line;
    raster->display_ystop = vic.first_displayed_line + 1;
    raster->display_xstart = 0;
    raster->display_xstop = 1;

    return 0;
}


static void clk_overflow_callback(CLOCK sub, void *data)
{
    if (vic.last_emulate_line_clk > (CLOCK)0)
        vic.last_emulate_line_clk -= sub;
}


/* Initialization. */
raster_t *vic_init(void)
{
    vic.log = log_open("VIC");

    vic.raster_draw_alarm = alarm_new(maincpu_alarm_context, "VicIRasterDraw",
                                      vic_raster_draw_alarm_handler, NULL);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL);

    vic_change_timing();

    if (init_raster() < 0)
        return NULL;

    vic.auxiliary_color = 0;
    vic.mc_border_color = 0;

    vic.color_ptr = mem_ram;
    vic.screen_ptr = mem_ram;
    vic.chargen_ptr = mem_chargen_rom + 0x400;

    /* FIXME: Where do these values come from? */
    vic.light_pen.triggered = 0;
    vic.light_pen.x = 87;
    vic.light_pen.y = 234;

    /* FIXME */
    vic.char_height = 8;
    vic.row_increase_line = 8;
    vic.text_cols = 22;
    vic.text_lines = 23;

    vic_reset();

    vic_draw_init();

    vic_update_memory_ptrs();

    vic.initialized = 1;

    resources_touch("VICDoubleSize");
    /*vic_resize();*/

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

    vic.last_emulate_line_clk = 0;
    vic.draw_clk = vic.cycles_per_line;
    alarm_set(vic.raster_draw_alarm, vic.draw_clk);

    vic.row_counter = 0;
    vic.memptr = 0;
    vic.pending_ystart = -1;
    vic.pending_text_lines = -1;
    vic.row_offset = -1;
    vic.area = 0;

}

/* Set the memory pointers according to the values stored in the VIC
   registers. */
void vic_update_memory_ptrs(void)
{
    static BYTE *old_chargen_ptr = NULL;
    static BYTE *old_color_ptr = NULL;
    static BYTE *old_screen_ptr = NULL;

    WORD char_addr;
    int tmp;
  
    BYTE *new_chargen_ptr;
    BYTE *new_color_ptr;
    BYTE *new_screen_ptr;

    tmp = vic.regs[0x5] & 0xf;
    char_addr = (tmp & 0x8) ? 0x0000 : 0x8000;
    char_addr += (tmp & 0x7) * 0x400;

    if (char_addr >= 0x8000 && char_addr < 0x9000) {
        new_chargen_ptr = mem_chargen_rom + 0x400 + (char_addr & 0xfff);
        VIC_DEBUG_REGISTER(("Character memory at $%04X "
                           "(character ROM + $%04X).",
                           char_addr,
                           char_addr & 0xfff));
    } else {
        if (char_addr == 0x1c00)
            new_chargen_ptr = mem_chargen_rom;    /* handle wraparound */
        else
            new_chargen_ptr = mem_ram + char_addr;
        VIC_DEBUG_REGISTER (("Character memory at $%04X.", char_addr));
    }

    new_color_ptr = mem_ram + 0x9400 + (vic.regs[0x2] & 0x80 ? 0x200 : 0x0);
    new_screen_ptr = mem_ram + (((vic.regs[0x2] & 0x80) << 2)
                     | ((vic.regs[0x5] & 0x70) << 6));

    VIC_DEBUG_REGISTER(("Color memory at $%04X.", vic.color_ptr - ram));
    VIC_DEBUG_REGISTER(("Screen memory at $%04X.", vic.screen_ptr - ram));

    if (new_chargen_ptr != old_chargen_ptr) {
        raster_changes_foreground_add_ptr(&vic.raster,
                                          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(maincpu_clk)
                                          + 2),
                                          (void*)&vic.chargen_ptr,
                                          new_chargen_ptr);
        old_chargen_ptr = new_chargen_ptr;
    }

    if (new_color_ptr != old_color_ptr) {
        raster_changes_foreground_add_ptr(&vic.raster,
                                          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(maincpu_clk)
                                          + 3),
                                          (void*)&vic.color_ptr,
                                          new_color_ptr);
        old_color_ptr = new_color_ptr;
    }

    if (new_screen_ptr != old_screen_ptr) {
        raster_changes_foreground_add_ptr(&vic.raster,
                                          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(maincpu_clk)
                                          + 3),
                                          (void*)&vic.screen_ptr,
                                          new_screen_ptr);
        old_screen_ptr = new_screen_ptr;
    }
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

