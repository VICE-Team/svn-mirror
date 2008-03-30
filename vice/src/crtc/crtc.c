/*
 * crtc.c - A line-based CRTC emulation (under construction).
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

#define _CRTC_C

#include "vice.h"

#include "machine.h"
#include "maincpu.h"
#include "utils.h"
#include "vsync.h"

#include "crtc.h"
#include "crtc-cmdline-options.h"
#include "crtc-draw.h"
#include "crtc-resources.h"



crtc_t crtc;



canvas_t 
crtc_init (void)
{
  raster_t *raster;
  unsigned int width, height;
  char *title;

  crtc.log = log_open ("CRTC");

  alarm_init (&crtc.raster_draw_alarm, &maincpu_alarm_context,
              "CrtcRasterDraw", crtc_raster_draw_alarm_handler);

  raster = &crtc.raster;

  raster_init (raster, CRTC_NUM_VMODES, 0);
  raster_modes_set_idle_mode (&raster->modes, CRTC_IDLE_MODE);
  raster_set_exposure_handler (raster, crtc_exposure_handler);
  raster_enable_cache (raster, crtc_resources.video_cache_enabled);
  raster_enable_double_scan (raster, crtc_resources.double_scan_enabled);

#define CRTC_SCREEN_WIDTH  400
#define CRTC_SCREEN_HEIGHT 250
#define CRTC_SCREEN_XPIX   320
#define CRTC_SCREEN_YPIX   200
#define CRTC_SCREEN_TEXTCOLS 40
#define CRTC_SCREEN_TEXTLINES 25
#define CRTC_SCREEN_BORDERWIDTH  40
#define CRTC_SCREEN_BORDERHEIGHT 25
#define CRTC_FIRST_DISPLAYED_LINE CRTC_SCREEN_BORDERHEIGHT
#define CRTC_LAST_DISPLAYED_LINE (CRTC_FIRST_DISPLAYED_LINE + CRTC_SCREEN_YPIX)

  /* FIXME */
  width = CRTC_SCREEN_WIDTH;
  height = CRTC_SCREEN_HEIGHT;
  if (crtc_resources.double_size_enabled)
    {
      width *= 2;
      height *= 2;
      raster_set_pixel_size (raster, 2, 2);
    }

  raster_set_geometry (raster,
                       CRTC_SCREEN_WIDTH, CRTC_SCREEN_HEIGHT,
                       CRTC_SCREEN_XPIX, CRTC_SCREEN_YPIX,
                       CRTC_SCREEN_TEXTCOLS, CRTC_SCREEN_TEXTLINES,
                       CRTC_SCREEN_BORDERWIDTH, CRTC_SCREEN_BORDERHEIGHT,
                       FALSE,
                       CRTC_FIRST_DISPLAYED_LINE,
                       CRTC_LAST_DISPLAYED_LINE,
                       0);
  raster_resize_viewport (raster, width, height);

  if (crtc_load_palette (crtc_resources.palette_file_name) < 0)
    log_error (crtc.log, "Cannot load palette.");

  title = concat ("VICE: ", machine_name, " emulator", NULL);
  raster_set_title (raster, title);
  free (title);

  raster_realize (raster);

  crtc_draw_init ();
  crtc_draw_set_double_size (crtc_resources.double_size_enabled);
  crtc_reset ();

  raster->display_ystart = 0;
  raster->display_ystop = CRTC_SCREEN_YPIX;
  raster->display_xstart = 0;
  raster->display_xstop = CRTC_SCREEN_XPIX;

  return crtc.raster.viewport.canvas;
}

/* Reset the VIC-II chip.  */
void 
crtc_reset (void)
{
  raster_reset (&crtc.raster);

  alarm_set (&crtc.raster_draw_alarm, CRTC_CYCLES_PER_LINE);
}



/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int
crtc_load_palette (const char *name)
{
  static const char *color_names[CRTC_NUM_COLORS] =
    {
      "Background", "Foreground"
    };
  palette_t *palette;

  palette = palette_create (CRTC_NUM_COLORS, color_names);
  if (palette == NULL)
    return -1;

  if (palette_load (name, palette) < 0)
    {
      log_message (crtc.log, "Cannot load palette file `%s'.", name);
      return -1;
    }

  raster_set_palette (&crtc.raster, palette);
  return 0;
}



/* Set proper functions and constants for the current video settings. */
void 
crtc_resize (void)
{
  if (! crtc.initialized)
    return;

  if (crtc_resources.double_size_enabled)
    {
      if (crtc.raster.viewport.pixel_size.width == 1
          && crtc.raster.viewport.canvas != NULL)
        raster_resize_viewport (&crtc.raster,
                                crtc.raster.viewport.width * 2,
                                crtc.raster.viewport.height * 2);

      raster_set_pixel_size (&crtc.raster, 2, 2);

      crtc_draw_set_double_size (1);
    }
  else
    {
      if (crtc.raster.viewport.pixel_size.width == 2
          && crtc.raster.viewport.canvas != NULL)
        raster_resize_viewport (&crtc.raster,
                                crtc.raster.viewport.width / 2,
                                crtc.raster.viewport.height / 2);

      raster_set_pixel_size (&crtc.raster, 1, 1);

      crtc_draw_set_double_size (0);
    }
}



void
crtc_set_screen_mode (BYTE *screen,
                      int vmask,
                      int num_cols,
                      int hwflags)
{
  /* FIXME: All of this should go into the `crtc' struct.  */
#if 0
  addr_mask = vmask;

  if (screen)
    screenmem = screen;

  crsr_enable = hwflags & 1;
  hw_double_cols = hwflags & 2;

  if (!num_cols)
    new_memptr_inc = 1;
  else
    new_memptr_inc = num_cols;

  /* no *2 for hw_double_cols, as the caller should have done it.
     This num_cols flag should be gone sometime.... */
  new_memptr_inc = crtc_min (SCREEN_MAX_TEXTCOLS, new_memptr_inc);

#ifdef __MSDOS__
  /* FIXME: This does not have any effect until there is a gfx -> text ->
     gfx mode transition.  Moreover, no resources should be changed behind
     user's back...  So this is definitely a Bad Thing (tm).  For now, it's
     fine with us, though.  */
  resources_set_value ("VGAMode",
                       (resource_value_t)  (num_cols > 40
                                            ? VGA_640x480 : VGA_320x200));
#endif

  /* vmask has changed -> */
  crtc_update_memory_ptrs ();

  /* force window reset with parameter =1 */
  crtc_update_timing (1);
#endif
}



/* Redraw the current raster line.  This happens at cycle the last
   cycle of each line.  */
int 
crtc_raster_draw_alarm_handler (long offset)
{
  raster_emulate_line (&crtc.raster);

  if (crtc.raster.current_line == 0)
    {
      raster_skip_frame (&crtc.raster, do_vsync (crtc.raster.skip_frame));

      /* Do vsync stuff.  */
    }

  alarm_set (&crtc.raster_draw_alarm, clk + CRTC_CYCLES_PER_LINE - offset);

  return 0;
}



void 
crtc_exposure_handler (unsigned int width,
                       unsigned int height)
{
  raster_resize_viewport (&crtc.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint (&crtc.raster);
}



/* Free the allocated frame buffer.  FIXME: Not incapsulated.  */
void
video_free(void)
{
  frame_buffer_free (&crtc.raster.frame_buffer);
}

void video_setfullscreen (int v, int width, int height)
{
}

/* ------------------------------------------------------------------- */

int crtc_offscreen(void)
{
    return 0;
}

void crtc_screen_enable(int enable)
{
}

void crtc_set_char(int crom)
{
}


