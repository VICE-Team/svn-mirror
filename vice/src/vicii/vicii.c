/*
 * vicii.c - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
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

/* A *big* thank goes to Andreas Boose (boose@rzgw.rz.fh-hannover.de) for
   helping me to find bugs and improve the emulation.  */

/* TODO: - speed optimizations;
   - faster sprites and registers.  */

/*
   Current (most important) known limitations:

   - if we switch from display to idle state in the middle of one line, we
   only paint it completely in idle or display mode (we choose the most
   likely one, though);

   - sprite colors (and other attributes) cannot change in the middle of the
   raster line;

   - changes of $D016 within one line are not always correctly handled;

   - g-accesses and c-accesses are not 100% emulated.

   Probably something else which I have not figured out yet...

 */

#define _VICII_C

#include "vice.h"

#include "alarm.h"
#include "c64cart.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "palette.h"
#include "snapshot.h"
#include "utils.h"
#include "vsync.h"

#include "vicii-cmdline-options.h"
#include "vicii-draw.h"
#include "vicii-sprites.h"
#include "vicii-resources.h"
#include "vicii-snapshot.h"

#include "vicii.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef __riscos
#include "ROlib.h"
#endif



vic_ii_t vic_ii;



static void 
clk_overflow_callback (CLOCK sub, void *unused_data)
{
  vic_ii.raster_irq_clk -= sub;
  vic_ii.last_emulate_line_clk -= sub;
  vic_ii.fetch_clk -= sub;
  vic_ii.draw_clk -= sub;
}

static void 
init_raster (void)
{
  raster_t *raster;
  unsigned int width, height;
  char *title;

  raster = &vic_ii.raster;

  raster_init (raster, VIC_II_NUM_VMODES, VIC_II_NUM_SPRITES);
  raster_modes_set_idle_mode (&raster->modes, VIC_II_IDLE_MODE);
  raster_set_exposure_handler (raster, vic_ii_exposure_handler);
  raster_enable_cache (raster, vic_ii_resources.video_cache_enabled);
  raster_enable_double_scan (raster, vic_ii_resources.double_scan_enabled);

  width = VIC_II_SCREEN_XPIX + VIC_II_SCREEN_BORDERWIDTH * 2;
  height = VIC_II_LAST_DISPLAYED_LINE - VIC_II_FIRST_DISPLAYED_LINE;
  if (vic_ii_resources.double_size_enabled)
    {
      width *= 2;
      height *= 2;
      raster_set_pixel_size (raster, 2, 2);
    }

  raster_set_geometry (raster,
                       VIC_II_SCREEN_WIDTH, VIC_II_SCREEN_HEIGHT,
                       VIC_II_SCREEN_XPIX, VIC_II_SCREEN_YPIX,
                       VIC_II_SCREEN_TEXTCOLS, VIC_II_SCREEN_TEXTLINES,
                       VIC_II_SCREEN_BORDERWIDTH, VIC_II_SCREEN_BORDERHEIGHT,
                       FALSE,
                       VIC_II_FIRST_DISPLAYED_LINE,
                       VIC_II_LAST_DISPLAYED_LINE,
                       2 * VIC_II_MAX_SPRITE_WIDTH);
  raster_resize_viewport (raster, width, height);

  if (vic_ii_load_palette (vic_ii_resources.palette_file_name) < 0)
    log_error (vic_ii.log, "Cannot load palette.");

  title = concat ("VICE: ", machine_name, " emulator", NULL);
  raster_set_title (raster, title);
  free (title);

  raster_realize (raster);

  raster->display_ystart = VIC_II_25ROW_START_LINE;
  raster->display_ystop = VIC_II_25ROW_STOP_LINE;
  raster->display_xstart = VIC_II_40COL_START_PIXEL;
  raster->display_xstop = VIC_II_40COL_STOP_PIXEL;
}



/* Emulate a matrix line fetch, `num' bytes starting from `offs'.  This takes
   care of the 10-bit counter wraparound.  */
inline void 
vic_ii_fetch_matrix (int offs, int num)
{
  BYTE *p;
  int start_char;
  int c;

  p = vic_ii.ram_base + vic_ii.vbank + ((vic_ii.regs[0x18] & 0xf0) << 6);
  start_char = (vic_ii.mem_counter + offs) & 0x3ff;
  c = 0x3ff - start_char + 1;

  if (c >= num)
    {
      memcpy (vic_ii.vbuf + offs, p + start_char, num);
      memcpy (vic_ii.cbuf + offs, vic_ii.color_ram + start_char, num);
    }
  else
    {
      memcpy (vic_ii.vbuf + offs, p + start_char, c);
      memcpy (vic_ii.vbuf + offs + c, p, num - c);
      memcpy (vic_ii.cbuf + offs, vic_ii.color_ram + start_char, c);
      memcpy (vic_ii.cbuf + offs + c, vic_ii.color_ram, num - c);
    }
}

/* If we are on a bad line, do the DMA.  Return nonzero if cycles have been
   stolen.  */
inline static int 
do_matrix_fetch (CLOCK sub)
{
  if (!vic_ii.memory_fetch_done)
    {
      raster_t *raster;

      raster = &vic_ii.raster;

      vic_ii.memory_fetch_done = 1;
      vic_ii.mem_counter = vic_ii.memptr;

      if ((raster->current_line & 7) == (unsigned int) raster->ysmooth
          && vic_ii.allow_bad_lines
          && raster->current_line >= VIC_II_FIRST_DMA_LINE
          && raster->current_line <= VIC_II_LAST_DMA_LINE)
        {
          vic_ii_fetch_matrix (0, VIC_II_SCREEN_TEXTCOLS);

          raster->draw_idle_state = 0;
          raster->ycounter = 0;

          vic_ii.idle_state = 0;
          vic_ii.idle_data_location = IDLE_NONE;
          vic_ii.ycounter_reset_checked = 1;
          vic_ii.memory_fetch_done = 2;

          maincpu_steal_cycles (vic_ii.fetch_clk,
                                VIC_II_SCREEN_TEXTCOLS + 3 - sub);

          vic_ii.bad_line = 1;
          return 1;
        }
    }

  return 0;
}

/* Enable DMA for sprite `num'.  */
inline static void 
turn_sprite_dma_on (unsigned int num)
{
  raster_sprite_status_t *sprite_status;
  raster_sprite_t *sprite;

  sprite_status = &vic_ii.raster.sprite_status;
  sprite = sprite_status->sprites + num;

  sprite_status->new_dma_msk |= 1 << num;
  sprite->dma_flag = 1;
  sprite->memptr = 0;
  sprite->exp_flag = sprite->y_expanded ? 0 : 1;
  sprite->memptr_inc = sprite->exp_flag ? 3 : 0;
}

/* Check for sprite DMA.  */
inline static void 
check_sprite_dma (void)
{
  raster_sprite_status_t *sprite_status;
  int i, b;

  sprite_status = &vic_ii.raster.sprite_status;

  if (! sprite_status->visible_msk && ! sprite_status->dma_msk)
    return;

  sprite_status->new_dma_msk = sprite_status->dma_msk;

  for (i = 0, b = 1; i < VIC_II_NUM_SPRITES; i++, b <<= 1)
    {
      raster_sprite_t *sprite;

      sprite = sprite_status->sprites + i;

      if ((sprite_status->visible_msk & b)
          && sprite->y == ((int) vic_ii.raster.current_line & 0xff)
          && !sprite->dma_flag)
        turn_sprite_dma_on (i);
      else if (sprite->dma_flag)
        {
          sprite->memptr = (sprite->memptr + sprite->memptr_inc) & 0x3f;

          if (sprite->y_expanded)
            sprite->exp_flag = !sprite->exp_flag;

          sprite->memptr_inc = sprite->exp_flag ? 3 : 0;

          if (sprite->memptr == 63)
            {
              sprite->dma_flag = 0;
              sprite_status->new_dma_msk &= ~b;

              if ((sprite_status->visible_msk & b)
                  && sprite->y == ((int) vic_ii.raster.current_line & 0xff))
                turn_sprite_dma_on (i);
            }
        }
    }
}



int
vic_ii_init_resources (void)
{
  return vic_ii_resources_init ();
}

int
vic_ii_init_cmdline_options (void)
{
  return vic_ii_cmdline_options_init ();
}



/* Initialize the VIC-II emulation.  */
canvas_t 
vic_ii_init (void)
{
  vic_ii.log = log_open ("VIC-II");

  alarm_init (&vic_ii.raster_fetch_alarm, &maincpu_alarm_context,
              "VicIIRasterFetch", vic_ii_raster_fetch_alarm_handler);
  alarm_init (&vic_ii.raster_draw_alarm, &maincpu_alarm_context,
              "VicIIRasterDraw", vic_ii_raster_draw_alarm_handler);
  alarm_init (&vic_ii.raster_irq_alarm, &maincpu_alarm_context,
              "VicIIRasterIrq", vic_ii_raster_irq_alarm_handler);

  init_raster ();

  vic_ii_powerup ();

  vic_ii_update_video_mode (0);
  vic_ii_update_memory_ptrs (0);

  vic_ii_draw_init ();
  vic_ii_draw_set_double_size (vic_ii_resources.double_size_enabled);

  vic_ii_sprites_init ();
  vic_ii_sprites_set_double_size (vic_ii_resources.double_size_enabled);

  vic_ii.initialized = 1;

  clk_guard_add_callback (&maincpu_clk_guard, clk_overflow_callback, NULL);

  if (clk_guard_get_clk_base (&maincpu_clk_guard) == 0)
    clk_guard_set_clk_base (&maincpu_clk_guard, C64_PAL_CYCLES_PER_RFSH);
  else
    /* Safety measure.  */
    log_error (vic_ii.log, "Trying to override clk base!?  Code is broken.");

  return vic_ii.raster.viewport.canvas;
}

/* Reset the VIC-II chip.  */
void 
vic_ii_reset (void)
{
  raster_reset (&vic_ii.raster);

  vic_ii.last_emulate_line_clk = 0;

  vic_ii.draw_clk = VIC_II_DRAW_CYCLE;
  alarm_set (&vic_ii.raster_draw_alarm, vic_ii.draw_clk);

  vic_ii.fetch_clk = VIC_II_FETCH_CYCLE;
  alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
  vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
  vic_ii.sprite_fetch_idx = 0;
  vic_ii.sprite_fetch_msk = 0;
  vic_ii.sprite_fetch_clk = CLOCK_MAX;

  /* FIXME: I am not sure this is exact emulation.  */
  vic_ii.raster_irq_line = 0;
  vic_ii.raster_irq_clk = 0;

  /* Setup the raster IRQ alarm.  The value is `1' instead of `0' because we
     are at the first line, which has a +1 clock cycle delay in IRQs.  */
  alarm_set (&vic_ii.raster_irq_alarm, 1);

  vic_ii.force_display_state = 0;

  vic_ii.light_pen.triggered = 0;
  vic_ii.light_pen.x = vic_ii.light_pen.y = 0;

  /* Remove all the IRQ sources.  */
  vic_ii.regs[0x1a] = 0;
}

/* This /should/ put the VIC-II in the same state as after a powerup, if
   `reset_vic_ii()' is called afterwards.  But FIXME, as we are not really
   emulating everything correctly here; just $D011.  */
void 
vic_ii_powerup (void)
{
  memset (vic_ii.regs, 0, sizeof (vic_ii.regs));

  vic_ii.irq_status = 0;
  vic_ii.raster_irq_line = 0;
  vic_ii.raster_irq_clk = 1;
  vic_ii.ram_base = ram;
  vic_ii.allow_bad_lines = 0;
  vic_ii.sprite_sprite_collisions = vic_ii.sprite_background_collisions = 0;
  vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
  vic_ii.idle_state = 0;
  vic_ii.force_display_state = 0;
  vic_ii.memory_fetch_done = 0;
  vic_ii.memptr = 0;
  vic_ii.mem_counter = 0;
  vic_ii.mem_counter_inc = 0;
  vic_ii.bad_line = 0;
  vic_ii.ycounter_reset_checked = 0;
  vic_ii.force_black_overscan_background_color = 0;
  vic_ii.light_pen.x = vic_ii.light_pen.y = vic_ii.light_pen.triggered = 0;
  vic_ii.vbank = 0;
  vic_ii.vbank_ptr = ram;
  vic_ii.idle_data = 0;
  vic_ii.idle_data_location = IDLE_NONE;
  vic_ii.extended_keyboard_rows_enabled = 0;
  vic_ii.last_emulate_line_clk = 0;

  vic_ii_reset ();

  vic_ii.regs[0x11] = 0;

  vic_ii.raster_irq_line = 0;

  vic_ii.raster.blank = 1;
  vic_ii.raster.display_ystart = VIC_II_24ROW_START_LINE;
  vic_ii.raster.display_ystop = VIC_II_24ROW_STOP_LINE;

  vic_ii.raster.ysmooth = 0;
}

/* This hook is called whenever video bank must be changed.  */
void 
vic_ii_set_vbank (int num_vbank)
{
  /* Warning: assumes it's called within a memory write access.
     FIXME: Change name?  */
  /* Also, we assume the bank has *really* changed, and do not do any
     special optimizations for the not-really-changed case.  */
  vic_ii_handle_pending_alarms (rmw_flag + 1);
  if (clk >= vic_ii.draw_clk)
    vic_ii_raster_draw_alarm_handler (clk - vic_ii.draw_clk);

  vic_ii.vbank = num_vbank << 14;
  vic_ii_update_memory_ptrs (VIC_II_RASTER_CYCLE (clk));
}

/* Trigger the light pen.  */
void 
vic_ii_trigger_light_pen (CLOCK mclk)
{
  if (!vic_ii.light_pen.triggered)
    {
      vic_ii.light_pen.triggered = 1;
      vic_ii.light_pen.x = VIC_II_RASTER_X (mclk % VIC_II_CYCLES_PER_LINE);
      if (vic_ii.light_pen.x < 0)
        vic_ii.light_pen.x = VIC_II_SPRITE_WRAP_X + vic_ii.light_pen.x;

      /* FIXME: why `+2'? */
      vic_ii.light_pen.x = vic_ii.light_pen.x / 2 + 2;
      vic_ii.light_pen.y = VIC_II_RASTER_Y (mclk);
      vic_ii.irq_status |= 0x8;

      if (vic_ii.regs[0x1a] & 0x8)
        {
          vic_ii.irq_status |= 0x80;
          maincpu_set_irq_clk (I_RASTER, 1, mclk);
        }
    }
}

/* Handle the exposure event.  */
void 
vic_ii_exposure_handler (unsigned int width, unsigned int height)
{
  raster_resize_viewport (&vic_ii.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint (&vic_ii.raster);
}

/* Toggle support for C128 extended keyboard rows.  */
void 
vic_ii_enable_extended_keyboard_rows (int flag)
{
  vic_ii.extended_keyboard_rows_enabled = flag;
}

/* Make sure all the VIC-II alarms are removed.  This just makes it easier to
   write functions for loading snapshot modules in other video chips without
   caring that the VIC-II alarms are dispatched when they really shouldn't
   be.  */
void 
vic_ii_prepare_for_snapshot (void)
{
  vic_ii.fetch_clk = CLOCK_MAX;
  alarm_unset (&vic_ii.raster_fetch_alarm);
  vic_ii.draw_clk = CLOCK_MAX;
  alarm_unset (&vic_ii.raster_draw_alarm);
}



void
vic_ii_set_raster_irq (unsigned int line)
{
  if (line == vic_ii.raster_irq_line)
    return;

  if (line < VIC_II_SCREEN_HEIGHT)
    {
      unsigned int current_line = VIC_II_RASTER_Y (clk);

      vic_ii.raster_irq_clk = (VIC_II_LINE_START_CLK (clk)
                               + VIC_II_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                               + (VIC_II_CYCLES_PER_LINE
                                  * (line - current_line)));

      /* Raster interrupts on line 0 are delayed by 1 cycle.  */
      if (line == 0)
        vic_ii.raster_irq_clk++;

      if (line <= current_line)
        vic_ii.raster_irq_clk += (VIC_II_SCREEN_HEIGHT
                                  * VIC_II_CYCLES_PER_LINE);
      alarm_set (&vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
    }
  else
    {
      VIC_II_DEBUG_RASTER (("VIC: update_raster_irq(): "
                            "raster compare out of range ($%04X)!\n",
                            line));
      alarm_unset (&vic_ii.raster_irq_alarm);
    }

  VIC_II_DEBUG_RASTER (("VIC: update_raster_irq(): "
                        "vic_ii.raster_irq_clk = %ul, "
                        "line = $%04X, "
                        "vic_ii.regs[0x1a]&1 = %d\n",
                        vic_ii.raster_irq_clk,
                        line,
                        vic_ii.regs[0x1a] & 1));

  vic_ii.raster_irq_line = line;
}



/* Change the base of RAM seen by the VIC-II.  */
void 
vic_ii_set_ram_base (BYTE * base)
{
  /* WARNING: assumes `rmw_flag' is 0 or 1.  */
  vic_ii_handle_pending_alarms (rmw_flag + 1);

  vic_ii.ram_base = base;
  vic_ii_update_memory_ptrs (VIC_II_RASTER_CYCLE (clk));
}


/* Set the memory pointers according to the values in the registers.  */
void
vic_ii_update_memory_ptrs (unsigned int cycle)
{
  /* FIXME: This is *horrible*!  */
  static BYTE *old_screen_ptr, *old_bitmap_ptr, *old_chargen_ptr;
  static int old_vbank = -1;
  ADDRESS screen_addr;          /* Screen start address.  */
  BYTE *screen_base;            /* Pointer to screen memory.  */
  BYTE *char_base;              /* Pointer to character memory.  */
  BYTE *bitmap_base;            /* Pointer to bitmap memory.  */
  int tmp;

  screen_addr = vic_ii.vbank + ((vic_ii.regs[0x18] & 0xf0) << 6);

  if ((screen_addr & 0x7000) != 0x1000)
    {
      screen_base = vic_ii.ram_base + screen_addr;
      VIC_II_DEBUG_REGISTER (("\tVideo memory at $%04X\n", screen_addr));
    }
  else
    {
      screen_base = chargen_rom + (screen_addr & 0x800);
      VIC_II_DEBUG_REGISTER (("\tVideo memory at Character ROM + $%04X\n",
                              screen_addr & 0x800));
    }

  tmp = (vic_ii.regs[0x18] & 0xe) << 10;
  bitmap_base = vic_ii.ram_base + (tmp & 0xe000);
  tmp += vic_ii.vbank;

  VIC_II_DEBUG_REGISTER (("\tBitmap memory at $%04X\n",
                          bitmap_base - vic_ii.ram_base + vic_ii.vbank));

  if ((tmp & 0x7000) != 0x1000)
    {
      char_base = vic_ii.ram_base + tmp;
      VIC_II_DEBUG_REGISTER (("\tUser-defined character set at $%04X\n", tmp));
    }
  else
    {
      char_base = chargen_rom + (tmp & 0x0800);
      VIC_II_DEBUG_REGISTER (("\tStandard %s character set enabled\n",
                              tmp & 0x800 ? "Lower Case" : "Upper Case"));
    }

  if (ultimax != 0)
    char_base = ((tmp & 0x3fff) >= 0x3000
                 ? romh_banks + (romh_bank << 13) + (tmp & 0x7ff) + 0x1000
                 : vic_ii.ram_base + tmp);

  tmp = VIC_II_RASTER_CHAR (cycle);

  if (vic_ii.idle_data_location != IDLE_NONE && old_vbank != vic_ii.vbank)
    {
      if (vic_ii.idle_data_location == IDLE_39FF)
        raster_add_int_change_foreground (&vic_ii.raster,
                                          VIC_II_RASTER_CHAR (cycle),
                                          &vic_ii.idle_data,
                                          vic_ii.ram_base[vic_ii.vbank
                                                          + 0x39ff]);
      else
        raster_add_int_change_foreground (&vic_ii.raster,
                                          VIC_II_RASTER_CHAR (cycle),
                                          &vic_ii.idle_data,
                                          vic_ii.ram_base[vic_ii.vbank
                                                          + 0x3fff]);
    }

  if (vic_ii.raster.skip_frame || (tmp <= 0 && clk < vic_ii.draw_clk))
    {
      old_screen_ptr = vic_ii.screen_ptr = screen_base;
      old_bitmap_ptr = vic_ii.bitmap_ptr = bitmap_base + vic_ii.vbank;
      old_chargen_ptr = vic_ii.chargen_ptr = char_base;
      old_vbank = vic_ii.vbank;
      vic_ii.vbank_ptr = vic_ii.ram_base + vic_ii.vbank;
      vic_ii.raster.sprite_status.ptr_base = screen_base + 0x3f8;
    }
  else if (tmp < VIC_II_SCREEN_TEXTCOLS)
    {
      if (screen_base != old_screen_ptr)
        {
          raster_add_ptr_change_foreground (&vic_ii.raster, tmp,
                                            (void **) &vic_ii.screen_ptr,
                                            (void *) screen_base);
          raster_add_ptr_change_foreground (&vic_ii.raster, tmp,
                            (void **) &vic_ii.raster.sprite_status.ptr_base,
                                            (void *) (screen_base + 0x3f8));
          old_screen_ptr = screen_base;
        }

      if (bitmap_base + vic_ii.vbank != old_bitmap_ptr)
        {
          raster_add_ptr_change_foreground (&vic_ii.raster,
                                            tmp,
                                            (void **) &vic_ii.bitmap_ptr,
                                            (void *) (bitmap_base
                                                      + vic_ii.vbank));
          old_bitmap_ptr = bitmap_base + vic_ii.vbank;
        }

      if (char_base != old_chargen_ptr)
        {
          raster_add_ptr_change_foreground (&vic_ii.raster,
                                            tmp,
                                            (void **) &vic_ii.chargen_ptr,
                                            (void *) char_base);
          old_chargen_ptr = char_base;
        }

      if (vic_ii.vbank != old_vbank)
        {
          raster_add_ptr_change_foreground (&vic_ii.raster,
                                            tmp,
                                            (void **) &vic_ii.vbank_ptr,
                                            (void *) (vic_ii.ram_base
                                                      + vic_ii.vbank));
          old_vbank = vic_ii.vbank;
        }
    }
  else
    {
      if (screen_base != old_screen_ptr)
        {
          raster_add_ptr_change_next_line (&vic_ii.raster,
                                           (void **) &vic_ii.screen_ptr,
                                           (void *) screen_base);
          raster_add_ptr_change_next_line (&vic_ii.raster,
                            (void **) &vic_ii.raster.sprite_status.ptr_base,
                                           (void *) (screen_base + 0x3f8));
          old_screen_ptr = screen_base;
        }
      if (bitmap_base + vic_ii.vbank != old_bitmap_ptr)
        {
          raster_add_ptr_change_next_line (&vic_ii.raster,
                                           (void **) &vic_ii.bitmap_ptr,
                                           (void *) (bitmap_base
                                                     + vic_ii.vbank));
          old_bitmap_ptr = bitmap_base + vic_ii.vbank;
        }

      if (char_base != old_chargen_ptr)
        {
          raster_add_ptr_change_next_line (&vic_ii.raster,
                                           (void **) &vic_ii.chargen_ptr,
                                           (void *) char_base);
          old_chargen_ptr = char_base;
        }

      if (vic_ii.vbank != old_vbank)
        {
          raster_add_ptr_change_next_line (&vic_ii.raster,
                                           (void **) &vic_ii.vbank_ptr,
                                           (void *) (vic_ii.ram_base
                                                     + vic_ii.vbank));
          old_vbank = vic_ii.vbank;
        }
    }
}



/* Set the video mode according to the values in registers $D011 and $D016 of
   the VIC-II chip.  */
void
vic_ii_update_video_mode (unsigned int cycle)
{
  static int old_video_mode = -1;
  int new_video_mode;

  new_video_mode = ((vic_ii.regs[0x11] & 0x60)
                    | (vic_ii.regs[0x16] & 0x10)) >> 4;

  if (new_video_mode != old_video_mode)
    {
      if (new_video_mode == VIC_II_HIRES_BITMAP_MODE
          || VIC_II_IS_ILLEGAL_MODE (new_video_mode))
        {
          /* Force the overscan color to black.  */
          raster_add_int_change_background
            (&vic_ii.raster, VIC_II_RASTER_X (cycle),
             &vic_ii.raster.overscan_background_color,
             0);
          vic_ii.force_black_overscan_background_color = 1;
        }
      else
        {
          /* The overscan background color is given by the background color
             register.  */
          if (vic_ii.raster.overscan_background_color != vic_ii.regs[0x21])
            raster_add_int_change_background
              (&vic_ii.raster, VIC_II_RASTER_X (cycle),
               &vic_ii.raster.overscan_background_color,
               vic_ii.regs[0x21]);
          vic_ii.force_black_overscan_background_color = 0;
        }

      {
        int pos;

        pos = VIC_II_RASTER_CHAR (cycle);

        raster_add_int_change_foreground (&vic_ii.raster, pos,
                                          &vic_ii.raster.video_mode,
                                          new_video_mode);

        if (vic_ii.idle_data_location != IDLE_NONE)
          {
            if (vic_ii.regs[0x11] & 0x40)
              raster_add_int_change_foreground
                (&vic_ii.raster, pos, (void *) &vic_ii.idle_data,
                 vic_ii.ram_base[vic_ii.vbank + 0x39ff]);
            else
              raster_add_int_change_foreground
                (&vic_ii.raster, pos, (void *) &vic_ii.idle_data,
                 vic_ii.ram_base[vic_ii.vbank + 0x3fff]);
          }
      }

      old_video_mode = new_video_mode;
    }

#ifdef VIC_II_VMODE_DEBUG
  switch (new_video_mode)
    {
    case VIC_II_NORMAL_TEXT_MODE:
      VIC_II_DEBUG_VMODE (("Standard Text"));
      break;
    case VIC_II_MULTICOLOR_TEXT_MODE:
      VIC_II_DEBUG_VMODE (("Multicolor Text"));
      break;
    case VIC_II_HIRES_BITMAP_MODE:
      VIC_II_DEBUG_VMODE (("Hires Bitmap"));
      break;
    case VIC_II_MULTICOLOR_BITMAP_MODE:
      VIC_II_DEBUG_VMODE (("Multicolor Bitmap"));
      break;
    case VIC_II_EXTENDED_TEXT_MODE:
      VIC_II_DEBUG_VMODE (("Extended Text"));
      break;
    case VIC_II_ILLEGAL_TEXT_MODE:
      VIC_II_DEBUG_VMODE (("Illegal Text"));
      break;
    case VIC_II_ILLEGAL_BITMAP_MODE_1:
      VIC_II_DEBUG_VMODE (("Invalid Bitmap"));
      break;
    case VIC_II_ILLEGAL_BITMAP_MODE_2:
      VIC_II_DEBUG_VMODE (("Invalid Bitmap"));
      break;
    default:                    /* cannot happen */
      VIC_II_DEBUG_VMODE (("???"));
    }

  VIC_II_DEBUG_VMODE ((" Mode enabled at line $%04X, cycle %d.\n",
                       VIC_II_RASTER_Y (clk), cycle));
#endif
}



/* Redraw the current raster line.  This happens at cycle VIC_II_DRAW_CYCLE
   of each line.  */
int 
vic_ii_raster_draw_alarm_handler (long offset)
{
  BYTE prev_sprite_sprite_collisions;
  BYTE prev_sprite_background_collisions;
  int in_visible_area;

  prev_sprite_sprite_collisions = vic_ii.sprite_sprite_collisions;
  prev_sprite_background_collisions = vic_ii.sprite_background_collisions;

  in_visible_area = (vic_ii.raster.current_line >= VIC_II_FIRST_DISPLAYED_LINE
                 && vic_ii.raster.current_line <= VIC_II_LAST_DISPLAYED_LINE);

  raster_emulate_line (&vic_ii.raster);

  if (vic_ii.raster.current_line == 0)
    {
      raster_skip_frame (&vic_ii.raster, do_vsync (vic_ii.raster.skip_frame));
      vic_ii.memptr = 0;
      vic_ii.mem_counter = 0;
      vic_ii.light_pen.triggered = 0;
    }

  if (in_visible_area)
    {
      if (!vic_ii.idle_state)
        vic_ii.mem_counter = (vic_ii.mem_counter
                              + vic_ii.mem_counter_inc) & 0x3ff;
      vic_ii.mem_counter_inc = VIC_II_SCREEN_TEXTCOLS;
      /* `ycounter' makes the chip go to idle state when it reaches the
         maximum value.  */
      if (vic_ii.raster.ycounter == 7)
        {
          vic_ii.idle_state = 1;
          vic_ii.memptr = vic_ii.mem_counter;
        }
      if (!vic_ii.idle_state || vic_ii.bad_line)
        {
          vic_ii.raster.ycounter = (vic_ii.raster.ycounter + 1) & 0x7;
          vic_ii.idle_state = 0;
        }
      if (vic_ii.force_display_state)
        {
          vic_ii.idle_state = 0;
          vic_ii.force_display_state = 0;
        }
      vic_ii.raster.draw_idle_state = vic_ii.idle_state;
      vic_ii.bad_line = 0;
    }

  vic_ii.ycounter_reset_checked = 0;
  vic_ii.memory_fetch_done = 0;

  if (vic_ii.raster.current_line == 0x30)
    vic_ii.allow_bad_lines = !vic_ii.raster.blank;

  /* As explained in Christian's article, only the first collision
     (i.e. the first time the collision register becomes non-zero) actually
     triggers an interrupt.  */
  if (vic_ii_resources.sprite_sprite_collisions_enabled
      && vic_ii.raster.sprite_status.sprite_sprite_collisions != 0
      && !prev_sprite_sprite_collisions)
    {
      vic_ii.irq_status |= 0x4;
      if (vic_ii.regs[0x1a] & 0x4)
        {
          maincpu_set_irq (I_RASTER, 1);
          vic_ii.irq_status |= 0x80;
        }
    }

  if (vic_ii_resources.sprite_background_collisions_enabled
      && vic_ii.raster.sprite_status.sprite_background_collisions
      && !prev_sprite_background_collisions)
    {
      vic_ii.irq_status |= 0x2;
      if (vic_ii.regs[0x1a] & 0x2)
        {
          maincpu_set_irq (I_RASTER, 1);
          vic_ii.irq_status |= 0x80;
        }
    }

  if (vic_ii.idle_state)
    {
      if (vic_ii.regs[0x11] & 0x40)
        {
          vic_ii.idle_data_location = IDLE_39FF;
          vic_ii.idle_data = vic_ii.ram_base[vic_ii.vbank + 0x39ff];
        }
      else
        {
          vic_ii.idle_data_location = IDLE_3FFF;
          vic_ii.idle_data = vic_ii.ram_base[vic_ii.vbank + 0x3fff];
        }
    }
  else
    vic_ii.idle_data_location = IDLE_NONE;

  /* Set the next draw event.  */
  vic_ii.last_emulate_line_clk += VIC_II_CYCLES_PER_LINE;
  vic_ii.draw_clk = vic_ii.last_emulate_line_clk + VIC_II_DRAW_CYCLE;
  alarm_set (&vic_ii.raster_draw_alarm, vic_ii.draw_clk);

  return 0;
}



inline static int
handle_fetch_matrix(long offset,
                    CLOCK sub,
                    CLOCK *write_offset)
{
  raster_t *raster;
  raster_sprite_status_t *sprite_status;

  *write_offset = 0;

  raster = &vic_ii.raster;
  sprite_status = &raster->sprite_status;

  if (sprite_status->visible_msk == 0 && sprite_status->dma_msk == 0)
    {
      do_matrix_fetch (sub);

      /* As sprites are all turned off, there is no need for a sprite DMA
         check; next time we will VIC_II_FETCH_MATRIX again.  This works
         because a VIC_II_CHECK_SPRITE_DMA is forced in `store_vic()'
         whenever the mask becomes nonzero.  */

      /* This makes sure we only create VIC_II_FETCH_MATRIX events in the bad
         line range.  These checks are (a little) redundant for safety.  */
      if (raster->current_line < VIC_II_FIRST_DMA_LINE)
        vic_ii.fetch_clk += ((VIC_II_FIRST_DMA_LINE
                              - raster->current_line)
                             * VIC_II_CYCLES_PER_LINE);
      else if (raster->current_line >= VIC_II_LAST_DMA_LINE)
        vic_ii.fetch_clk += ((VIC_II_SCREEN_HEIGHT
                              - raster->current_line
                              + VIC_II_FIRST_DMA_LINE)
                             * VIC_II_CYCLES_PER_LINE);
      else
        vic_ii.fetch_clk += VIC_II_CYCLES_PER_LINE;

      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
      return 1;
    }
  else
    {
      int fetch_done;

      fetch_done = do_matrix_fetch (sub);

      /* Sprites might be turned on, check for sprite DMA next
         time.  */
      vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;

      /* Calculate time for next event.  */
      vic_ii.fetch_clk = (VIC_II_LINE_START_CLK (clk)
                          + VIC_II_SPRITE_FETCH_CYCLE);

      if (vic_ii.fetch_clk > clk || offset == 0)
        {
          /* Prepare the next fetch event.  */
          alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
          return 1;
        }

      if (fetch_done && sub == 0)
        *write_offset = VIC_II_SCREEN_TEXTCOLS + 3;
    }

  return 0;
}

inline static void
swap_sprite_data_buffers (void)
{
  DWORD *tmp;
  raster_sprite_status_t *sprite_status;

  /* Swap sprite data buffers.  */
  sprite_status = &vic_ii.raster.sprite_status;
  tmp = sprite_status->sprite_data;
  sprite_status->sprite_data = sprite_status->new_sprite_data;
  sprite_status->new_sprite_data = tmp;
}

inline static int
handle_check_sprite_dma (long offset,
                         CLOCK sub)
{
  swap_sprite_data_buffers ();

  check_sprite_dma ();

  /* FIXME?  Slow!  */
  vic_ii.sprite_fetch_clk = (VIC_II_LINE_START_CLK (clk)
                             + VIC_II_SPRITE_FETCH_CYCLE);
  vic_ii.sprite_fetch_msk = vic_ii.raster.sprite_status.new_dma_msk;

  if (vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][0].cycle == -1)
    {
      if (vic_ii.raster.current_line >= VIC_II_FIRST_DMA_LINE - 1
          && vic_ii.raster.current_line <= VIC_II_LAST_DMA_LINE + 1)
        {
          vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              - VIC_II_SPRITE_FETCH_CYCLE
                              + VIC_II_FETCH_CYCLE
                              + VIC_II_CYCLES_PER_LINE);
        }
      else
        {
          vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              + VIC_II_CYCLES_PER_LINE);
        }
    }
  else
    {
      /* Next time, fetch sprite data.  */
      vic_ii.fetch_idx = VIC_II_FETCH_SPRITE;
      vic_ii.sprite_fetch_idx = 0;
      vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                          + vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][0].cycle);
    }

  if (vic_ii.fetch_clk > clk || offset == 0)
    {
      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
      return 1;
    }

  return 0;
}

inline static int
handle_fetch_sprite (long offset,
                     CLOCK sub,
                     CLOCK *write_offset)
{
  const vic_ii_sprites_fetch_t *sf;
  unsigned int i;
  int next_cycle;

  /* FIXME: optimize.  */

  sf = &vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][vic_ii.sprite_fetch_idx];

  if (!vic_ii.raster.skip_frame)
    {
      raster_sprite_status_t *sprite_status;
      BYTE *bank, *spr_base;

      sprite_status = &vic_ii.raster.sprite_status;
      bank = vic_ii.ram_base + vic_ii.vbank;
      spr_base = (bank + 0x3f8 + ((vic_ii.regs[0x18] & 0xf0) << 6)
                  + sf->first);

      /* Fetch sprite data.  */
      for (i = sf->first; i <= sf->last; i++, spr_base++)
        {
          BYTE *src;
          BYTE *dest;
          int my_memptr;

          src = bank + (*spr_base << 6);
          my_memptr = sprite_status->sprites[i].memptr;
          dest = (BYTE *) (sprite_status->new_sprite_data + i);

          if (ultimax && *spr_base >= 0xc0)
            src = (romh_banks + 0x1000 + (romh_bank << 13)
                   + ((*spr_base - 0xc0) << 6));

          dest[0] = src[my_memptr];
          dest[1] = src[++my_memptr & 0x3f];
          dest[2] = src[++my_memptr & 0x3f];
        }
    }

  maincpu_steal_cycles (vic_ii.fetch_clk, sf->num - sub);

  if (sub == 0)
    *write_offset = sf->num;
  else
    *write_offset = 0;

  next_cycle = (sf + 1)->cycle;
  vic_ii.sprite_fetch_idx++;

  if (next_cycle == -1)
    {
      /* Next time, handle bad lines.  */
      if (vic_ii.raster.current_line >= VIC_II_FIRST_DMA_LINE - 1
          && vic_ii.raster.current_line <= VIC_II_LAST_DMA_LINE + 1)
        {
          vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              - VIC_II_SPRITE_FETCH_CYCLE
                              + VIC_II_FETCH_CYCLE
                              + VIC_II_CYCLES_PER_LINE);
        }
      else
        {
          vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              + VIC_II_CYCLES_PER_LINE);
        }
    }
  else
    vic_ii.fetch_clk = vic_ii.sprite_fetch_clk + next_cycle;

  if (clk >= vic_ii.draw_clk)
    vic_ii_raster_draw_alarm_handler (clk - vic_ii.draw_clk);

  if (vic_ii.fetch_clk > clk || offset == 0)
    {
      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
      return 1;
    }

  if (clk >= vic_ii.raster_irq_clk)
    vic_ii_raster_irq_alarm_handler (clk - vic_ii.raster_irq_clk);

  return 0;
}

/* Handle sprite/matrix fetch events.  FIXME: could be made slightly
   faster.  */
int 
vic_ii_raster_fetch_alarm_handler (long offset)
{
  CLOCK last_opcode_first_write_clk, last_opcode_last_write_clk;

  /* This kludgy thing is used to emulate the behavior of the 6510 when BA
     goes low.  When BA goes low, every read access stops the processor
     until BA is high again; write accesses happen as usual instead.  */

  if (offset > 0)
    {
      switch (OPINFO_NUMBER (last_opcode_info))
        {
        case 0:
          /* In BRK, IRQ and NMI the 3rd, 4th and 5th cycles are write
             accesses, while the 1st, 2nd, 6th and 7th are read accesses.  */
          last_opcode_first_write_clk = clk - 5;
          last_opcode_last_write_clk = clk - 3;
          break;

        case 0x20:
          /* In JSR, the 4th and 5th cycles are write accesses, while the
             1st, 2nd, 3rd and 6th are read accesses.  */
          last_opcode_first_write_clk = clk - 3;
          last_opcode_last_write_clk = clk - 2;
          break;

        default:
          /* In all the other opcodes, all the write accesses are the last
             ones.  */
          if (maincpu_num_write_cycles () != 0)
            {
              last_opcode_last_write_clk = clk - 1;
              last_opcode_first_write_clk = clk - maincpu_num_write_cycles ();
            }
          else
            {
              last_opcode_first_write_clk = (CLOCK) 0;
              last_opcode_last_write_clk = last_opcode_first_write_clk;
            }
          break;
        }
    }
  else                          /* offset <= 0, i.e. offset == 0 */
    {
      /* If we are called with no offset, we don't have to care about write
         accesses.  */
      last_opcode_first_write_clk = last_opcode_last_write_clk = 0;
    }

  while (1)
    {
      CLOCK sub;
      CLOCK write_offset;
      int leave;

      if (vic_ii.fetch_clk < last_opcode_first_write_clk
          || vic_ii.fetch_clk > last_opcode_last_write_clk)
        sub = 0;
      else
        sub = last_opcode_last_write_clk - vic_ii.fetch_clk + 1;

      switch (vic_ii.fetch_idx)
        {
        case VIC_II_FETCH_MATRIX:
          leave = handle_fetch_matrix (offset, sub, &write_offset);
          last_opcode_first_write_clk += write_offset;
          last_opcode_last_write_clk += write_offset;
          break;

        case VIC_II_CHECK_SPRITE_DMA:
          leave = handle_check_sprite_dma (offset, sub);
          break;

        case VIC_II_FETCH_SPRITE:
        default:                /* Make compiler happy.  */
          leave = handle_fetch_sprite (offset, sub, &write_offset);
          last_opcode_first_write_clk += write_offset;
          last_opcode_last_write_clk += write_offset;
          break;
        }

      if (leave)
        break;
    }

  return 0;
}



/* If necessary, emulate a raster compare IRQ. This is called when the raster
   line counter matches the value stored in the raster line register.  */
int 
vic_ii_raster_irq_alarm_handler (long offset)
{
  vic_ii.irq_status |= 0x1;
  if (vic_ii.regs[0x1a] & 0x1)
    {
      maincpu_set_irq_clk (I_RASTER, 1, vic_ii.raster_irq_clk);
      vic_ii.irq_status |= 0x80;
      VIC_II_DEBUG_RASTER (("VIC: *** IRQ requested at line $%04X, "
                "vic_ii.raster_irq_line=$%04X, offset = %ld, cycle = %d.\n",
                      VIC_II_RASTER_Y (clk), vic_ii.raster_irq_line, offset,
                            VIC_II_RASTER_CYCLE (clk)));
    }

  vic_ii.raster_irq_clk += VIC_II_SCREEN_HEIGHT * VIC_II_CYCLES_PER_LINE;
  alarm_set (&vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);

  return 0;
}



/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int
vic_ii_load_palette (const char *name)
{
  static const char *color_names[VIC_II_NUM_COLORS] =
    {
      "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue",
      "Yellow", "Orange", "Brown", "Light Red", "Dark Gray", "Medium Gray",
      "Light Green", "Light Blue", "Light Gray"
    };
  palette_t *palette;

  palette = palette_create (VIC_II_NUM_COLORS, color_names);
  if (palette == NULL)
    return -1;

  if (palette_load (name, palette) < 0)
    {
      log_message (vic_ii.log, "Cannot load palette file `%s'.", name);
      return -1;
    }

  raster_set_palette (&vic_ii.raster, palette);
  return 0;
}



/* Set proper functions and constants for the current video settings.  */
void 
vic_ii_resize (void)
{
  if (!vic_ii.initialized)
    return;

  if (vic_ii_resources.double_size_enabled)
    {
      if (vic_ii.raster.viewport.pixel_size.width == 1
          && vic_ii.raster.viewport.canvas != NULL)
        raster_resize_viewport (&vic_ii.raster,
                                vic_ii.raster.viewport.width * 2,
                                vic_ii.raster.viewport.height * 2);

      raster_set_pixel_size (&vic_ii.raster, 2, 2);

      vic_ii_draw_set_double_size (1);
      vic_ii_sprites_set_double_size (1);
    }
  else
    {
      if (vic_ii.raster.viewport.pixel_size.width == 2
          && vic_ii.raster.viewport.canvas != NULL)
        raster_resize_viewport (&vic_ii.raster,
                                vic_ii.raster.viewport.width / 2,
                                vic_ii.raster.viewport.height / 2);

      raster_set_pixel_size (&vic_ii.raster, 1, 1);

      vic_ii_draw_set_double_size (0);
      vic_ii_sprites_set_double_size (0);
    }
}



int
vic_ii_write_snapshot_module (snapshot_t *s)
{
  return vic_ii_snapshot_write_module (s);
}

int
vic_ii_read_snapshot_module (snapshot_t *s)
{
  return vic_ii_snapshot_read_module (s);
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
  frame_buffer_free (&vic_ii.raster.frame_buffer);
}
