/*
 * vic.c - A line-based VIC-I emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

/* Warning: this emulation is very incomplete and buggy.  */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>

#include "clkguard.h"
#include "log.h"
#include "mem.h"
#include "machine.h"
#include "maincpu.h"
#include "palette.h"
#include "raster-modes.h"
#include "screenshot.h"
#include "snapshot.h"
#include "types.h"
#include "utils.h"
#include "vic.h"
#include "vic-cmdline-options.h"
#include "vic-draw.h"
#include "vic-mem.h"
#include "vic-resources.h"
#include "vic-snapshot.h"
#include "vsync.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif

vic_t vic;

static void vic_exposure_handler (unsigned int width, unsigned int height);

static void vic_exposure_handler (unsigned int width, unsigned int height)
{
  raster_resize_viewport (&vic.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint (&vic.raster);
}

/* Notice: The screen origin X register has a 4-pixel granularity, so our
   write accesses are always aligned. */

int vic_raster_draw_alarm_handler (CLOCK offset)
{
    int blank_this_line;
    int cols_in_this_line;

    alarm_set (&vic.raster_draw_alarm, clk + VIC_CYCLES_PER_LINE - offset);

    if (vic.area == 0 && vic.raster.current_line >= vic.raster.display_ystart)
        vic.area = 1;

    blank_this_line = vic.raster.blank_this_line;
    if (blank_this_line)
        vic.raster.display_ystop++;

    cols_in_this_line = vic.text_cols;

    raster_emulate_line (&vic.raster);

    /* xstart may have changed; recalculate xstop */
    vic.raster.display_xstop = vic.raster.display_xstart + vic.text_cols * 8;

    if (vic.area == 1)
    {
        int increase_row = 
            ((unsigned int)vic.raster.ycounter > vic.row_increase_line - 1
            || (unsigned int)vic.raster.ycounter == vic.row_increase_line - 1
                && !blank_this_line);

        if (increase_row)
        {
            vic.row_counter++;
            vic.raster.ycounter = 0;
            vic.memptr += cols_in_this_line;

            vic.raster.display_ystop = vic.raster.current_line
                + (vic.text_lines - vic.row_counter) * vic.char_height;

            /* if XPOS is 0 VIC displays one more rasterline */
            if (vic.raster.display_xstart == 0)
                vic.raster.display_ystop++;
        
        } else if (!blank_this_line) {
            vic.raster.ycounter++;

            if (vic.row_offset > 0)
            {
                /* this only happens if char_height changes from 8 to 16 within line 7 */
                vic.memptr += vic.row_offset;
                vic.row_offset = -1;
            }

            if (vic.raster.current_line >= vic.raster.display_ystop)
                vic.area = 2;
        }
    }

    if (vic.raster.current_line == 0)
    {
        raster_skip_frame (&vic.raster, do_vsync (vic.raster.skip_frame));
        vic.raster.blank_enabled = 1;
        vic.row_counter = 0;
        vic.raster.ycounter = 0;
        vic.memptr = 0;
        vic.area = 0;

        if (vic.pending_ystart >= 0)
        {
            vic.raster.display_ystart = vic.pending_ystart;
            vic.raster.geometry.gfx_position.y = 
                vic.pending_ystart - VIC_FIRST_DISPLAYED_LINE;
            vic.raster.display_ystop = 
                vic.raster.display_ystart + vic.text_lines * vic.char_height;

            vic.pending_ystart = -1;
        }

        if (vic.pending_text_lines >= 0)
        {
            vic.text_lines = vic.pending_text_lines;
            vic.raster.display_ystop = 
                (vic.raster.display_ystart + vic.text_lines * vic.char_height);
            vic.raster.geometry.gfx_size.height = vic.pending_text_lines * 8;
            vic.raster.geometry.text_size.height = vic.pending_text_lines;

            vic.pending_text_lines = -1;
        }

        vic.raster.blank = 0;
    }

    /* Set the next draw event.  */
    vic.last_emulate_line_clk += VIC_CYCLES_PER_LINE;
    vic.draw_clk = vic.last_emulate_line_clk + VIC_CYCLES_PER_LINE;
    alarm_set (&vic.raster_draw_alarm, vic.draw_clk);

    return 0;
}

static int init_raster(void)
{
    raster_t *raster;
    unsigned int width, height;
    char *title;

    raster = &vic.raster;

    if (raster_init(raster, VIC_NUM_VMODES, VIC_NUM_SPRITES) < 0)
        return -1;

    raster_modes_set_idle_mode (raster->modes, VIC_IDLE_MODE);
    raster_set_exposure_handler (raster, (void*)vic_exposure_handler);
    raster_enable_cache (raster, vic_resources.video_cache_enabled);
#ifdef USE_XF86_EXTENSIONS
    raster_enable_double_scan (raster, fullscreen_is_enabled ? vic_resources.fullscreen_double_scan_enabled : vic_resources.double_scan_enabled);
#else
    raster_enable_double_scan (raster, vic_resources.double_scan_enabled);
#endif
    raster_set_canvas_refresh(raster, 1);

    raster_set_geometry (raster,
                         VIC_SCREEN_WIDTH, VIC_SCREEN_HEIGHT,
                         22*8, 23*8,    /* handled dynamically  */
                         22, 23,        /* handled dynamically  */
                         12*4, 38*2 - VIC_FIRST_DISPLAYED_LINE, /* handled dynamically  */
                         1,
                         VIC_FIRST_DISPLAYED_LINE,
                         VIC_LAST_DISPLAYED_LINE,
                         0);

    width = VIC_DISPLAY_WIDTH * VIC_PIXEL_WIDTH;
    height = VIC_LAST_DISPLAYED_LINE - VIC_FIRST_DISPLAYED_LINE + 1;

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled ? vic_resources.fullscreen_double_size_enabled : vic_resources.double_size_enabled) {
#else
    if (vic_resources.double_size_enabled) {
#endif
        width *= 2;
        height *= 2;
        raster_set_pixel_size (raster, VIC_PIXEL_WIDTH * 2, 2);
    } else
        raster_set_pixel_size (raster, VIC_PIXEL_WIDTH, 1);

    raster_resize_viewport (raster, width, height);

    if (vic_load_palette (vic_resources.palette_file_name) < 0) {
        log_error (vic.log, "Cannot load palette.");
        return -1;
    }

    title = concat ("VICE: ", machine_name, " emulator", NULL);
    raster_set_title (raster, title);
    free(title);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = VIC_FIRST_DISPLAYED_LINE;
    raster->display_ystop = VIC_FIRST_DISPLAYED_LINE + 1;
    raster->display_xstart = 0;
    raster->display_xstop = 1;

    return 0;
}

/* Initialization. */
raster_t *vic_init(void)
{
  vic.log = log_open ("VIC");

  alarm_init (&vic.raster_draw_alarm, &maincpu_alarm_context,
              "VicIRasterDraw", vic_raster_draw_alarm_handler);

  if (init_raster() < 0)
    return NULL;

  vic.auxiliary_color = 0;
  vic.mc_border_color = 0;

  vic.color_ptr = ram;
  vic.screen_ptr = ram;
  vic.chargen_ptr = chargen_rom + 0x400;

  /* FIXME */
  vic.char_height = 8;
  vic.row_increase_line = 8;
  vic.text_cols = 22;
  vic.text_lines = 23;

  vic_reset ();

  vic_draw_init ();
#ifdef USE_XF86_EXTENSIONS
  vic_draw_set_double_size (fullscreen_is_enabled ? vic_resources.fullscreen_double_size_enabled : vic_resources.double_size_enabled);
#else
  vic_draw_set_double_size (vic_resources.double_size_enabled);
#endif

  vic_update_memory_ptrs ();

  vic.initialized = 1;

  if (clk_guard_get_clk_base (&maincpu_clk_guard) == 0)
    clk_guard_set_clk_base (&maincpu_clk_guard, VIC20_PAL_CYCLES_PER_RFSH);
  else
    /* Safety measure.  */
    log_error (vic.log, "Trying to override clk base!?  Code is broken.");

  return &vic.raster;
}

canvas_t *vic_get_canvas(void)
{
  return vic.raster.viewport.canvas;
}

/* Reset the VIC-I chip. */
void vic_reset (void)
{
  raster_reset (&vic.raster);
  alarm_set (&vic.raster_draw_alarm, VIC_CYCLES_PER_LINE);

  vic.row_counter = 0;
  vic.memptr = 0;
  vic.pending_ystart = -1;
  vic.pending_text_lines = -1;
  vic.row_offset = -1;
  vic.last_emulate_line_clk = 0;
  vic.area = 0;

}

/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int vic_load_palette (const char *name)
{
  static const char *color_names[] =
    {
      "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue",
      "Yellow", "Orange", "Light Orange", "Pink", "Light Cyan",
      "Light Purple", "Light Green", "Light Blue", "Light Yellow"
    };
  palette_t *palette;

  palette = palette_create (VIC_NUM_COLORS, color_names);
  if (palette == NULL)
    return -1;

  if (palette_load (name, palette) < 0)
    {
      log_message (vic.log, "Cannot load palette file `%s'.", name);
      return -1;
    }

  return raster_set_palette (&vic.raster, palette);
}

/* This hook is called whenever the screen parameters (eg. window size) are
   changed.  */
void vic_resize (void)
{
  if (!vic.initialized)
    return;

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled ? vic_resources.fullscreen_double_size_enabled : vic_resources.double_size_enabled)
#else
    if (vic_resources.double_size_enabled)
#endif

    {
      if (vic.raster.viewport.pixel_size.width == VIC_PIXEL_WIDTH
          && vic.raster.viewport.canvas != NULL) {
          raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH * 2, 2);
        raster_resize_viewport (&vic.raster,
                                vic.raster.viewport.width * 2,
                                vic.raster.viewport.height * 2);
      } else {
          raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH * 2, 2);
      }

      vic_draw_set_double_size (1);
    }
  else
    {
      if (vic.raster.viewport.pixel_size.width == VIC_PIXEL_WIDTH * 2
          && vic.raster.viewport.canvas != NULL) {
          raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH, 1);
        raster_resize_viewport (&vic.raster,
                                vic.raster.viewport.width / 2,
                                vic.raster.viewport.height / 2);
      } else {
          raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH, 1);
      }

      vic_draw_set_double_size (0);
    }
}

/* Set the memory pointers according to the values stored in the VIC
   registers. */
void vic_update_memory_ptrs (void)
{
  static BYTE *old_chargen_ptr = NULL;
  static BYTE *old_color_ptr = NULL;
  static BYTE *old_screen_ptr = NULL;

  ADDRESS char_addr;
  int tmp;
  
  BYTE *new_chargen_ptr;
  BYTE *new_color_ptr;
  BYTE *new_screen_ptr;

  tmp = vic.regs[0x5] & 0xf;
  char_addr = (tmp & 0x8) ? 0x0000 : 0x8000;
  char_addr += (tmp & 0x7) * 0x400;

  if (char_addr >= 0x8000 && char_addr < 0x9000)
    {
      new_chargen_ptr = chargen_rom + 0x400 + (char_addr & 0xfff);
      VIC_DEBUG_REGISTER (("Character memory at $%04X "
                           "(character ROM + $%04X).",
                           char_addr,
                           char_addr & 0xfff));
    }
  else
    {
      if (char_addr == 0x1c00)
        new_chargen_ptr = chargen_rom;    /* handle wraparound */
      else
        new_chargen_ptr = ram + char_addr;
      VIC_DEBUG_REGISTER (("Character memory at $%04X.", char_addr));
    }

  new_color_ptr = ram + 0x9400 + (vic.regs[0x2] & 0x80 ? 0x200 : 0x0);
  new_screen_ptr = ram + (((vic.regs[0x2] & 0x80) << 2)
                          | ((vic.regs[0x5] & 0x70) << 6));

  VIC_DEBUG_REGISTER (("Color memory at $%04X.", vic.color_ptr - ram));
  VIC_DEBUG_REGISTER (("Screen memory at $%04X.", vic.screen_ptr - ram));

  if (new_chargen_ptr != old_chargen_ptr)
  {
    raster_add_ptr_change_foreground (&vic.raster,
          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(clk) + 2),
          (void*)&vic.chargen_ptr,
          new_chargen_ptr);

    old_chargen_ptr = new_chargen_ptr;
  }

  if (new_color_ptr != old_color_ptr)
  {
    raster_add_ptr_change_foreground (&vic.raster,
          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(clk) + 3),
          (void*)&vic.color_ptr,
          new_color_ptr);

    old_color_ptr = new_color_ptr;
  }
  if (new_screen_ptr != old_screen_ptr)
  {
    raster_add_ptr_change_foreground (&vic.raster,
          VIC_RASTER_CHAR(VIC_RASTER_CYCLE(clk) + 3),
          (void*)&vic.screen_ptr,
          new_screen_ptr);

    old_screen_ptr = new_screen_ptr;
  }
}

int vic_init_resources (void)
{
  return vic_resources_init ();
}

int vic_init_cmdline_options (void)
{
  return vic_cmdline_options_init ();
}

int vic_write_snapshot_module (snapshot_t *s)
{
  return vic_snapshot_write_module (s);
}

int vic_read_snapshot_module (snapshot_t *s)
{
  return vic_snapshot_read_module (s);
}

void vic_free (void)
{
  raster_free(&vic.raster);
}

int vic_screenshot(screenshot_t *screenshot)
{
    return raster_screenshot(&vic.raster, screenshot);
}

void vic_video_refresh(void)
{
#ifdef USE_XF86_EXTENSIONS
  vic_resize ();
  raster_enable_double_scan (&vic.raster,
			     fullscreen_is_enabled ?
			     vic_resources.fullscreen_double_scan_enabled :
			     vic_resources.double_scan_enabled);
#endif
}
