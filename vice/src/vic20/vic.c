/*
 * vic.c - A line-based VIC-I emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

/* Warning: this emulation is very incomplete and buggy.  */

#include "vice.h"

#include "log.h"
#include "mem.h"
#include "machine.h"
#include "maincpu.h"
#include "utils.h"
#include "vsync.h"

#include "vic-draw.h"
#include "vic-mem.h"
#include "vic-cmdline-options.h"
#include "vic-resources.h"
#include "vic-snapshot.h"

#include "vic.h"



vic_t vic;



static int raster_draw_alarm_handler (long offset);
static void exposure_handler (unsigned int width, unsigned int height);



static void 
exposure_handler (unsigned int width, unsigned int height)
{
  raster_resize_viewport (&vic.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint (&vic.raster);
}

/* Notice: The screen origin X register has a 4-pixel granularity, so our
   write accesses are always aligned. */

static int 
raster_draw_alarm_handler (long offset)
{
  int in_visible_area;

  alarm_set (&vic.raster_draw_alarm, clk + VIC_CYCLES_PER_LINE - offset);

  in_visible_area = (vic.raster.current_line >= vic.raster.display_ystart
                     && vic.raster.current_line <= vic.raster.display_ystop);

  raster_emulate_line (&vic.raster);

  if (in_visible_area)
    {
      /* FIXME: I don't think this is exact emulation.  */
      if (vic.raster.ycounter == vic.char_height - 1)
        {
          vic.raster.ycounter = 0;
          vic.memptr += vic.text_cols;
        }
      else
        vic.raster.ycounter++;
    }

  if (vic.raster.current_line == 0)
    {
      raster_skip_frame (&vic.raster, do_vsync (vic.raster.skip_frame));
      vic.raster.blank_enabled = 1;
      vic.raster.ycounter = 0;
      vic.memptr = 0;
    }

  return 0;
}



static void 
init_raster (void)
{
  raster_t *raster;
  unsigned int width, height;
  char *title;

  raster = &vic.raster;

  raster_init (raster, VIC_NUM_VMODES, VIC_NUM_SPRITES);
  raster_modes_set_idle_mode (&raster->modes, VIC_IDLE_MODE);

  raster_set_exposure_handler (raster, exposure_handler);
  raster_enable_cache (raster, vic_resources.video_cache_enabled);
  raster_enable_double_scan (raster, vic_resources.double_scan_enabled);

  raster_set_geometry (raster,
                       VIC_SCREEN_WIDTH, VIC_SCREEN_HEIGHT,
                       1, 1,
                       0, 0,
                       0, 0,
                       1,
                       VIC_FIRST_DISPLAYED_LINE,
                       VIC_LAST_DISPLAYED_LINE,
                       0);

  width = VIC_SCREEN_WIDTH * VIC_PIXEL_WIDTH;
  height = VIC_LAST_DISPLAYED_LINE - VIC_FIRST_DISPLAYED_LINE + 1;
  if (vic_resources.double_size_enabled)
    {
      width *= 2;
      height *= 2;
      raster_set_pixel_size (raster, VIC_PIXEL_WIDTH * 2, 2);
    }
  else
    raster_set_pixel_size (raster, VIC_PIXEL_WIDTH, 1);

  raster_resize_viewport (raster, width, height);

  if (vic_load_palette (vic_resources.palette_file_name) < 0)
    log_error (vic.log, "Cannot load palette.");

  title = concat ("VICE: ", machine_name, " emulator", NULL);
  raster_set_title (raster, title);
  free (title);

  raster_realize (raster);

  raster->display_ystart = VIC_FIRST_DISPLAYED_LINE;
  raster->display_ystop = VIC_FIRST_DISPLAYED_LINE + 1;
  raster->display_xstart = 0;
  raster->display_xstop = 1;
}

/* Initialization. */
canvas_t 
vic_init (void)
{
  vic.log = log_open ("VIC");

  alarm_init (&vic.raster_draw_alarm, &maincpu_alarm_context,
              "VicIRasterDraw", raster_draw_alarm_handler);

  init_raster ();

  vic.color_ptr = ram;
  vic.screen_ptr = ram;
  vic.chargen_ptr = chargen_rom + 0x400;

  /* FIXME */
  vic.char_height = 8;
  vic.text_cols = 22;
  vic.text_lines = 23;

  vic_reset ();

  vic_draw_init ();
  vic_draw_set_double_size (vic_resources.double_size_enabled);

  vic_update_memory_ptrs ();

  vic.initialized = 1;

  if (clk_guard_get_clk_base (&maincpu_clk_guard) == 0)
    clk_guard_set_clk_base (&maincpu_clk_guard, VIC20_PAL_CYCLES_PER_RFSH);
  else
    /* Safety measure.  */
    log_error (vic.log, "Trying to override clk base!?  Code is broken.");

  return vic.raster.viewport.canvas;
}

/* Reset the VIC-I chip. */
void 
vic_reset (void)
{
  raster_reset (&vic.raster);
  alarm_set (&vic.raster_draw_alarm, VIC_CYCLES_PER_LINE);

  vic.memptr = 0;
}



/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int
vic_load_palette (const char *name)
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

  raster_set_palette (&vic.raster, palette);
  return 0;
}



/* This hook is called whenever the screen parameters (eg. window size) are
   changed.  */
void 
vic_resize (void)
{
  if (! vic.initialized)
    return;

  if (vic_resources.double_size_enabled)
    {
      if (vic.raster.viewport.pixel_size.width == VIC_PIXEL_WIDTH
          && vic.raster.viewport.canvas != NULL)
        raster_resize_viewport (&vic.raster,
                                vic.raster.viewport.width * 2,
                                vic.raster.viewport.height * 2);

      raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH * 2, 2);

      vic_draw_set_double_size (1);
    }
  else
    {
      if (vic.raster.viewport.pixel_size.width == VIC_PIXEL_WIDTH * 2
          && vic.raster.viewport.canvas != NULL)
        raster_resize_viewport (&vic.raster,
                                vic.raster.viewport.width / 2,
                                vic.raster.viewport.height / 2);

      raster_set_pixel_size (&vic.raster, VIC_PIXEL_WIDTH, 1);

      vic_draw_set_double_size (0);
    }
}



/* Set the memory pointers according to the values stored in the VIC
   registers. */
void 
vic_update_memory_ptrs (void)
{
  ADDRESS char_addr;
  int tmp;

  tmp = vic.regs[0x5] & 0xf;
  char_addr = (tmp & 0x8) ? 0x0000 : 0x8000;
  char_addr += (tmp & 0x7) * 0x400;

  if (char_addr >= 0x8000 && char_addr < 0x9000)
    {
      vic.chargen_ptr = chargen_rom + 0x400 + (char_addr & 0xfff);
      VIC_DEBUG_REGISTER ((vic_log,
                           "Character memory at $%04X "
                           "(character ROM + $%04X).",
                           char_addr,
                           char_addr & 0xfff));
    }
  else
    {
      if (char_addr == 0x1c00)
        vic.chargen_ptr = chargen_rom;    /* handle wraparound */
      else
        vic.chargen_ptr = ram + char_addr;
      VIC_DEBUG_REGISTER ((vic_log, "Character memory at $%04X.", char_addr));
    }

  vic.color_ptr = ram + 0x9400 + (vic.regs[0x2] & 0x80 ? 0x200 : 0x0);
  vic.screen_ptr = ram + (((vic.regs[0x2] & 0x80) << 2)
                          | ((vic.regs[0x5] & 0x70) << 6));

  VIC_DEBUG_REGISTER ((vic_log, "Color memory at $%04X.",
                       vic.color_ptr - ram));
  VIC_DEBUG_REGISTER ((vic_log, "Screen memory at $%04X.",
                       vic.screen_ptr - ram));
}



int
vic_init_resources (void)
{
  return vic_resources_init ();
}

int
vic_init_cmdline_options (void)
{
  return vic_cmdline_options_init ();
}



int
vic_write_snapshot_module (snapshot_t *s)
{
  return vic_snapshot_write_module (s);
}

int
vic_read_snapshot_module (snapshot_t *s)
{
  return vic_snapshot_read_module (s);
}



/* FIXME: Just a dummy.  */
void 
video_setfullscreen (int v, int width, int height)
{
}

/* Free the allocated frame buffer.  FIXME: Not incapsulated.  */
void 
video_free (void)
{
  frame_buffer_free (&vic.raster.frame_buffer);
}
