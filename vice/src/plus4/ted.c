/*
 * ted.c
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include "clkguard.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "palette.h"
#include "plus4.h"
#include "raster-modes.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "resources.h"
#include "screenshot.h"
#include "snapshot.h"
#include "ted-cmdline-options.h"
#include "ted-color.h"
#include "ted-draw.h"
#include "ted-mem.h"
#include "ted-resources.h"
#include "ted-snapshot.h"
#include "ted-sprites.h"
#include "ted.h"
#include "tedtypes.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "video.h"

/* KLUDGE! */
BYTE chargen_rom[0x1000];

/* FIXME: PAL/NTSC constants should be moved from drive.h */
#define DRIVE_SYNC_PAL     -1
#define DRIVE_SYNC_NTSC    -2
#define DRIVE_SYNC_NTSCOLD -3

/* ---------------------------------------------------------------------*/

/* set phi1 address options */
void vic_ii_set_phi1_addr_options(ADDRESS mask, ADDRESS offset)
{
    vic_ii.vaddr_mask_phi1 = mask;
    vic_ii.vaddr_offset_phi1 = offset;

    VIC_II_DEBUG_REGISTER(("Set phi1 video addr mask=%04x, offset=%04x\n",
                          mask, offset));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi2_addr_options(ADDRESS mask, ADDRESS offset)
{
    vic_ii.vaddr_mask_phi2 = mask;
    vic_ii.vaddr_offset_phi2 = offset;

    VIC_II_DEBUG_REGISTER(("Set phi2 video addr mask=%04x, offset=%04x\n",
                          mask, offset));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi1_chargen_addr_options(ADDRESS mask, ADDRESS value)
{
    vic_ii.vaddr_chargen_mask_phi1 = mask;
    vic_ii.vaddr_chargen_value_phi1 = value;

    VIC_II_DEBUG_REGISTER(("Set phi1 chargen addr mask=%04x, value=%04x\n",
                          mask, value));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi2_chargen_addr_options(ADDRESS mask, ADDRESS value)
{
    vic_ii.vaddr_chargen_mask_phi2 = mask;
    vic_ii.vaddr_chargen_value_phi2 = value;

    VIC_II_DEBUG_REGISTER(("Set phi2 chargen addr mask=%04x, value=%04x\n",
                          mask, value));
    vic_ii_update_memory_ptrs_external();
}

/* ---------------------------------------------------------------------*/

vic_ii_t vic_ii;

static void vic_ii_raster_irq_alarm_handler(CLOCK offset);

static void clk_overflow_callback(CLOCK sub, void *unused_data)
{
  vic_ii.raster_irq_clk -= sub;
  vic_ii.last_emulate_line_clk -= sub;
  vic_ii.fetch_clk -= sub;
  vic_ii.draw_clk -= sub;
}

void vic_ii_change_timing(void)
{
    resource_value_t mode;

    resources_get_value("VideoStandard", &mode);

    switch ((int)mode) {
      case DRIVE_SYNC_NTSC:
        clk_guard_set_clk_base(&maincpu_clk_guard, PLUS4_NTSC_CYCLES_PER_RFSH);
        vic_ii.screen_height = VIC_II_NTSC_SCREEN_HEIGHT;
        vic_ii.first_displayed_line = VIC_II_NTSC_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_NTSC_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_NTSC_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_NTSC_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_NTSC_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_NTSC_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_NTSC_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_NTSC_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_NTSC_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_NTSC_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_NTSC_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_NTSC_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_NTSC_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_NTSC_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_NTSC_OFFSET;
        break;
      case DRIVE_SYNC_PAL:
      default:
        clk_guard_set_clk_base(&maincpu_clk_guard, PLUS4_PAL_CYCLES_PER_RFSH);
        vic_ii.screen_height = VIC_II_PAL_SCREEN_HEIGHT;
        vic_ii.first_displayed_line = VIC_II_PAL_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_PAL_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_PAL_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_PAL_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_PAL_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_PAL_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_PAL_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_PAL_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_PAL_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_PAL_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_PAL_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_PAL_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_PAL_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_PAL_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_PAL_OFFSET;
        break;
    }
}

inline void ted_handle_pending_alarms(int num_write_cycles)
{
  if (num_write_cycles != 0)
    {
      int f;

      /* Cycles can be stolen only during the read accesses, so we serve
         only the events that happened during them.  The last read access
         happened at `clk - maincpu_write_cycles()' as all the opcodes
         except BRK and JSR do all the write accesses at the very end.  BRK
         cannot take us here and we would not be able to handle JSR
         correctly anyway, so we don't care about them...  */

      /* Go back to the time when the read accesses happened and serve VIC
         events.  */
      clk -= num_write_cycles;

      do
        {
          f = 0;
          if (clk > vic_ii.fetch_clk)
            {
              vic_ii_raster_fetch_alarm_handler (0);
              f = 1;
            }
          if (clk >= vic_ii.draw_clk)
            {
              vic_ii_raster_draw_alarm_handler((long)(clk - vic_ii.draw_clk));
              f = 1;
            }
        }
      while (f);

      /* Go forward to the time when the last write access happens (that's
         the one we care about, as the only instructions that do two write
         accesses - except BRK and JSR - are the RMW ones, which store the
         old value in the first write access, and then store the new one in
         the second write access).  */
      clk += num_write_cycles;

    }
  else
    {
      int f;

      do
        {
          f = 0;
          if (clk >= vic_ii.fetch_clk)
            {
              vic_ii_raster_fetch_alarm_handler (0);
              f = 1;
            }
          if (clk >= vic_ii.draw_clk)
            {
              vic_ii_raster_draw_alarm_handler (0);
              f = 1;
            }
        }
      while (f);
    }
}


static void vic_ii_set_geometry(void)
{
  unsigned int width, height;

  width = VIC_II_SCREEN_XPIX + vic_ii.screen_borderwidth * 2;
  height = vic_ii.last_displayed_line - vic_ii.first_displayed_line + 1;
#ifdef VIC_II_NEED_2X
#ifdef USE_XF86_EXTENSIONS
  if (fullscreen_is_enabled
      ? vic_ii_resources.fullscreen_double_size_enabled
      : vic_ii_resources.double_size_enabled)
#else
  if (vic_ii_resources.double_size_enabled)
#endif
    {
      width *= 2;
      height *= 2;
      raster_set_pixel_size(&vic_ii.raster, 2, 2);
    }
#endif

  raster_set_geometry(&vic_ii.raster,
                      VIC_II_SCREEN_WIDTH, vic_ii.screen_height,
                      VIC_II_SCREEN_XPIX, VIC_II_SCREEN_YPIX,
                      VIC_II_SCREEN_TEXTCOLS, VIC_II_SCREEN_TEXTLINES,
                      vic_ii.screen_borderwidth, vic_ii.row_25_start_line,
                      0,
                      vic_ii.first_displayed_line,
                      vic_ii.last_displayed_line,
                      2 * VIC_II_MAX_SPRITE_WIDTH);
  raster_resize_viewport(&vic_ii.raster, width, height);

#ifdef __MSDOS__
  video_ack_vga_mode();
#endif

}

static int init_raster (void)
{
    raster_t *raster;
    char *title;

    raster = &vic_ii.raster;

    if (raster_init(raster, VIC_II_NUM_VMODES, VIC_II_NUM_SPRITES) < 0)
        return -1;
    raster_modes_set_idle_mode(raster->modes, VIC_II_IDLE_MODE);
    raster_set_exposure_handler(raster, (void*)vic_ii_exposure_handler);
    raster_enable_cache(raster, vic_ii_resources.video_cache_enabled);
#ifdef VIC_II_NEED_2X
#ifdef USE_XF86_EXTENSIONS
    raster_enable_double_scan(raster, fullscreen_is_enabled
                              ? vic_ii_resources.fullscreen_double_scan_enabled 
                              : vic_ii_resources.double_scan_enabled);
#else
    raster_enable_double_scan(raster, vic_ii_resources.double_scan_enabled);
#endif
#else
    raster_enable_double_scan(raster, 0);
#endif
    raster_set_canvas_refresh(raster, 1);

    vic_ii_set_geometry();

    if (vic_ii_activate_palette() < 0) {
        log_error(vic_ii.log, "Cannot load palette.");
        return -1;
    }
    title = concat("VICE: ", machine_name, " emulator", NULL);
    raster_set_title(raster, title);
    free (title);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = vic_ii.row_25_start_line;
    raster->display_ystop = vic_ii.row_25_stop_line;
    raster->display_xstart = VIC_II_40COL_START_PIXEL;
    raster->display_xstop = VIC_II_40COL_STOP_PIXEL;

    return 0;
}

/* Emulate a matrix line fetch, `num' bytes starting from `offs'.  This takes
   care of the 10-bit counter wraparound.  */
inline void vic_ii_fetch_matrix(int offs, int num)
{
  BYTE *p;
  int start_char;
  int c;

  /* Matrix fetches are done during Phi2, the fabulous "bad lines" */
  p = vic_ii.ram_base_phi2 + vic_ii.vbank_phi2
                                + ((vic_ii.regs[0x18] & 0xf0) << 6);
  start_char = (vic_ii.mem_counter + offs) & 0x3ff;
  c = 0x3ff - start_char + 1;

  if (c >= num)
    {
      memcpy(vic_ii.vbuf + offs, p + start_char, num);
      memcpy(vic_ii.cbuf + offs, vic_ii.color_ram + start_char, num);
    }
  else
    {
      memcpy(vic_ii.vbuf + offs, p + start_char, c);
      memcpy(vic_ii.vbuf + offs + c, p, num - c);
      memcpy(vic_ii.cbuf + offs, vic_ii.color_ram + start_char, c);
      memcpy(vic_ii.cbuf + offs + c, vic_ii.color_ram, num - c);
    }
}

/* If we are on a bad line, do the DMA.  Return nonzero if cycles have been
   stolen.  */
inline static int do_matrix_fetch(CLOCK sub)
{
  if (!vic_ii.memory_fetch_done)
    {
      raster_t *raster;

      raster = &vic_ii.raster;

      vic_ii.memory_fetch_done = 1;
      vic_ii.mem_counter = vic_ii.memptr;

      if ((raster->current_line & 7) == (unsigned int) raster->ysmooth
          && vic_ii.allow_bad_lines
          && raster->current_line >= vic_ii.first_dma_line
          && raster->current_line <= vic_ii.last_dma_line)
        {
          vic_ii_fetch_matrix (0, VIC_II_SCREEN_TEXTCOLS);

          raster->draw_idle_state = 0;
          raster->ycounter = 0;

          vic_ii.idle_state = 0;
          vic_ii.idle_data_location = IDLE_NONE;
          vic_ii.ycounter_reset_checked = 1;
          vic_ii.memory_fetch_done = 2;

          maincpu_steal_cycles(vic_ii.fetch_clk,
                               VIC_II_SCREEN_TEXTCOLS + 3 - sub);

          vic_ii.bad_line = 1;
          return 1;
        }
    }

  return 0;
}

/* Enable DMA for sprite `num'.  */
inline static void turn_sprite_dma_on(unsigned int num)
{
  raster_sprite_status_t *sprite_status;
  raster_sprite_t *sprite;

  sprite_status = vic_ii.raster.sprite_status;
  sprite = sprite_status->sprites + num;

  sprite_status->new_dma_msk |= 1 << num;
  sprite->dma_flag = 1;
  sprite->memptr = 0;
  sprite->exp_flag = sprite->y_expanded ? 0 : 1;
  sprite->memptr_inc = sprite->exp_flag ? 3 : 0;
}

/* Check for sprite DMA.  */
inline static void check_sprite_dma(void)
{
  raster_sprite_status_t *sprite_status;
  int i, b;

  sprite_status = vic_ii.raster.sprite_status;

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
                  && sprite->y == ((int)vic_ii.raster.current_line & 0xff))
                turn_sprite_dma_on(i);
            }
        }
    }
}

int ted_init_resources(void)
{
  return ted_resources_init();
}

int ted_init_cmdline_options(void)
{
  return ted_cmdline_options_init();
}

/* Initialize the VIC-II emulation.  */
raster_t *vic_ii_init(void)
{
  vic_ii.log = log_open ("VIC-II");

  alarm_init(&vic_ii.raster_fetch_alarm, maincpu_alarm_context,
             "VicIIRasterFetch", vic_ii_raster_fetch_alarm_handler);
  alarm_init(&vic_ii.raster_draw_alarm, maincpu_alarm_context,
             "VicIIRasterDraw", vic_ii_raster_draw_alarm_handler);
  alarm_init(&vic_ii.raster_irq_alarm, maincpu_alarm_context,
             "VicIIRasterIrq", vic_ii_raster_irq_alarm_handler);

  vic_ii_change_timing();

  if (init_raster() < 0)
      return NULL;

  vic_ii_powerup();

  vic_ii_update_video_mode(0);
  vic_ii_update_memory_ptrs(0);

  vic_ii_draw_init();
#ifdef VIC_II_NEED_2X
#ifdef USE_XF86_EXTENSIONS
  vic_ii_draw_set_double_size(fullscreen_is_enabled
                              ? vic_ii_resources.fullscreen_double_size_enabled
                              : vic_ii_resources.double_size_enabled);
#else
  vic_ii_draw_set_double_size(vic_ii_resources.double_size_enabled);
#endif
#else
  vic_ii_draw_set_double_size(0);
#endif

  vic_ii_sprites_init();
#ifdef VIC_II_NEED_2X
#ifdef USE_XF86_EXTENSIONS
  vic_ii_sprites_set_double_size(fullscreen_is_enabled
                                 ? vic_ii_resources.fullscreen_double_size_enabled
                                 : vic_ii_resources.double_size_enabled);
#else
  vic_ii_sprites_set_double_size(vic_ii_resources.double_size_enabled);
#endif
#else
  vic_ii_sprites_set_double_size(0);
#endif

  vic_ii.initialized = 1;

  clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);

  return &vic_ii.raster;
}

struct canvas_s *vic_ii_get_canvas(void)
{
  return vic_ii.raster.viewport.canvas;
}

/* Reset the VIC-II chip.  */
void ted_reset(void)
{
  vic_ii_change_timing();

  raster_reset(&vic_ii.raster);

  vic_ii_set_geometry();

  vic_ii.last_emulate_line_clk = 0;

  vic_ii.draw_clk = vic_ii.draw_cycle;
  alarm_set(&vic_ii.raster_draw_alarm, vic_ii.draw_clk);

  vic_ii.fetch_clk = VIC_II_FETCH_CYCLE;
  alarm_set(&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
  vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
  vic_ii.sprite_fetch_idx = 0;
  vic_ii.sprite_fetch_msk = 0;
  vic_ii.sprite_fetch_clk = CLOCK_MAX;

  /* FIXME: I am not sure this is exact emulation.  */
  vic_ii.raster_irq_line = 0;
  vic_ii.raster_irq_clk = 0;

  /* Setup the raster IRQ alarm.  The value is `1' instead of `0' because we
     are at the first line, which has a +1 clock cycle delay in IRQs.  */
  alarm_set(&vic_ii.raster_irq_alarm, 1);

  vic_ii.force_display_state = 0;

  vic_ii.light_pen.triggered = 0;
  vic_ii.light_pen.x = vic_ii.light_pen.y = 0;

  /* Remove all the IRQ sources.  */
  vic_ii.regs[0x1a] = 0;

  vic_ii.raster.display_ystart = vic_ii.row_25_start_line;
  vic_ii.raster.display_ystop = vic_ii.row_25_stop_line;
}

void ted_reset_registers(void)
{
    ADDRESS i;

    for (i = 0; i <= 0x3f; i++)
        vic_store(i, 0);
}

/* This /should/ put the VIC-II in the same state as after a powerup, if
   `reset_vic_ii()' is called afterwards.  But FIXME, as we are not really
   emulating everything correctly here; just $D011.  */
void vic_ii_powerup(void)
{
  memset(vic_ii.regs, 0, sizeof(vic_ii.regs));

  vic_ii.irq_status = 0;
  vic_ii.raster_irq_line = 0;
  vic_ii.raster_irq_clk = 1;
  vic_ii.ram_base_phi1 = ram;
  vic_ii.ram_base_phi2 = ram;

  vic_ii.vaddr_mask_phi1 = 0xffff;
  vic_ii.vaddr_mask_phi2 = 0xffff;
  vic_ii.vaddr_offset_phi1 = 0;
  vic_ii.vaddr_offset_phi2 = 0;
  vic_ii.vaddr_chargen_mask_phi1 = 0x7000;
  vic_ii.vaddr_chargen_mask_phi2 = 0x7000;
  vic_ii.vaddr_chargen_value_phi1 = 0x1000;
  vic_ii.vaddr_chargen_value_phi2 = 0x1000;

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
  vic_ii.vbank_phi1 = 0;
  vic_ii.vbank_phi2 = 0;
  /* vic_ii.vbank_ptr = ram; */
  vic_ii.idle_data = 0;
  vic_ii.idle_data_location = IDLE_NONE;
  vic_ii.extended_keyboard_rows_enabled = 0;
  vic_ii.last_emulate_line_clk = 0;

  ted_reset();

  vic_ii.regs[0x11] = 0;

  vic_ii.raster_irq_line = 0;

  vic_ii.raster.blank = 1;
  vic_ii.raster.display_ystart = vic_ii.row_24_start_line;
  vic_ii.raster.display_ystop = vic_ii.row_24_stop_line;

  vic_ii.raster.ysmooth = 0;
}

/* ---------------------------------------------------------------------*/

/* This hook is called whenever video bank must be changed.  */
static inline void vic_ii_set_vbanks(int vbank_p1, int vbank_p2)
{
  /* Warning: assumes it's called within a memory write access.
     FIXME: Change name?  */
  /* Also, we assume the bank has *really* changed, and do not do any
     special optimizations for the not-really-changed case.  */
  ted_handle_pending_alarms (rmw_flag + 1);
  if (clk >= vic_ii.draw_clk)
    vic_ii_raster_draw_alarm_handler(clk - vic_ii.draw_clk);

  vic_ii.vbank_phi1 = vbank_p1;
  vic_ii.vbank_phi2 = vbank_p2;
  vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE (clk));
}

/* Phi1 and Phi2 accesses */
void vic_ii_set_vbank(int num_vbank)
{
  int tmp = num_vbank << 14;
  vic_ii_set_vbanks(tmp, tmp);
}

/* Phi1 accesses */
void vic_ii_set_phi1_vbank(int num_vbank)
{
  vic_ii_set_vbanks(num_vbank << 14, vic_ii.vbank_phi2);
}

/* Phi2 accesses */
void vic_ii_set_phi2_vbank(int num_vbank)
{
  vic_ii_set_vbanks(vic_ii.vbank_phi1, num_vbank << 14);
}

/* ---------------------------------------------------------------------*/

/* Trigger the light pen.  */
void vic_ii_trigger_light_pen(CLOCK mclk)
{
  if (!vic_ii.light_pen.triggered)
    {
      vic_ii.light_pen.triggered = 1;
      vic_ii.light_pen.x = VIC_II_RASTER_X(mclk % vic_ii.cycles_per_line);
      if (vic_ii.light_pen.x < 0)
        vic_ii.light_pen.x = vic_ii.sprite_wrap_x + vic_ii.light_pen.x;

      /* FIXME: why `+2'? */
      vic_ii.light_pen.x = vic_ii.light_pen.x / 2 + 2;
      vic_ii.light_pen.y = VIC_II_RASTER_Y(mclk);
      vic_ii.irq_status |= 0x8;

      if (vic_ii.regs[0x1a] & 0x8)
        {
          vic_ii.irq_status |= 0x80;
          maincpu_set_irq_clk(I_RASTER, 1, mclk);
        }
    }
}

/* Handle the exposure event.  */
void vic_ii_exposure_handler(unsigned int width, unsigned int height)
{
  raster_resize_viewport(&vic_ii.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint(&vic_ii.raster);
}

/* Toggle support for C128 extended keyboard rows.  */
void vic_ii_enable_extended_keyboard_rows (int flag)
{
  vic_ii.extended_keyboard_rows_enabled = flag;
}

/* Make sure all the VIC-II alarms are removed.  This just makes it easier to
   write functions for loading snapshot modules in other video chips without
   caring that the VIC-II alarms are dispatched when they really shouldn't
   be.  */
void vic_ii_prepare_for_snapshot(void)
{
  vic_ii.fetch_clk = CLOCK_MAX;
  alarm_unset(&vic_ii.raster_fetch_alarm);
  vic_ii.draw_clk = CLOCK_MAX;
  alarm_unset(&vic_ii.raster_draw_alarm);
  vic_ii.raster_irq_clk = CLOCK_MAX;
  alarm_unset(&vic_ii.raster_irq_alarm);
}

void vic_ii_set_raster_irq(unsigned int line)
{
  if (line == vic_ii.raster_irq_line && vic_ii.raster_irq_clk != CLOCK_MAX)
    return;

  if (line < vic_ii.screen_height)
    {
      unsigned int current_line = VIC_II_RASTER_Y (clk);

      vic_ii.raster_irq_clk = (VIC_II_LINE_START_CLK (clk)
                               + VIC_II_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                               + (vic_ii.cycles_per_line
                                  * (line - current_line)));

      /* Raster interrupts on line 0 are delayed by 1 cycle.  */
      if (line == 0)
        vic_ii.raster_irq_clk++;

      if (line <= current_line)
        vic_ii.raster_irq_clk += (vic_ii.screen_height
                                  * vic_ii.cycles_per_line);
      alarm_set(&vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
    }
  else
    {
      VIC_II_DEBUG_RASTER(("VIC: update_raster_irq(): "
                          "raster compare out of range ($%04X)!\n",
                          line));
      alarm_unset(&vic_ii.raster_irq_alarm);
    }

  VIC_II_DEBUG_RASTER(("VIC: update_raster_irq(): "
                      "vic_ii.raster_irq_clk = %ul, "
                      "line = $%04X, "
                      "vic_ii.regs[0x1a]&1 = %d\n",
                      vic_ii.raster_irq_clk,
                      line,
                      vic_ii.regs[0x1a] & 1));

  vic_ii.raster_irq_line = line;
}

/* Change the base of RAM seen by the VIC-II.  */
static inline void vic_ii_set_ram_bases(BYTE *base_p1, BYTE *base_p2)
{
  /* WARNING: assumes `rmw_flag' is 0 or 1.  */
  ted_handle_pending_alarms(rmw_flag + 1);

  vic_ii.ram_base_phi1 = base_p1;
  vic_ii.ram_base_phi2 = base_p2;
  vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE (clk));
}

void vic_ii_set_ram_base(BYTE *base)
{
  vic_ii_set_ram_bases(base, base);
}

void vic_ii_set_phi1_ram_base(BYTE *base)
{
  vic_ii_set_ram_bases(base, vic_ii.ram_base_phi2);
}

void vic_ii_set_phi2_ram_base(BYTE *base)
{
  vic_ii_set_ram_bases(vic_ii.ram_base_phi1, base);
}


void vic_ii_update_memory_ptrs_external(void)
{
    if (vic_ii.initialized > 0)
        vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE(clk));
}

/* Set the memory pointers according to the values in the registers.  */
void vic_ii_update_memory_ptrs(unsigned int cycle)
{
  /* FIXME: This is *horrible*!  */
  static BYTE *old_screen_ptr, *old_bitmap_ptr, *old_chargen_ptr;
  static int old_vbank_p1 = -1;
  static int old_vbank_p2 = -1;
  ADDRESS screen_addr;          /* Screen start address.  */
  BYTE *screen_base;            /* Pointer to screen memory.  */
  BYTE *char_base;              /* Pointer to character memory.  */
  BYTE *bitmap_base;            /* Pointer to bitmap memory.  */
  int tmp;

  screen_addr = vic_ii.vbank_phi2 + ((vic_ii.regs[0x18] & 0xf0) << 6);

  screen_addr = (screen_addr & vic_ii.vaddr_mask_phi2)
                | vic_ii.vaddr_offset_phi2;

  if ((screen_addr & vic_ii.vaddr_chargen_mask_phi2)
      != vic_ii.vaddr_chargen_value_phi2)
    {
      screen_base = vic_ii.ram_base_phi2 + screen_addr;
      VIC_II_DEBUG_REGISTER(("\tVideo memory at $%04X\n", screen_addr));
    }
  else
    {
      screen_base = chargen_rom + (screen_addr & 0x800);
      VIC_II_DEBUG_REGISTER(("\tVideo memory at Character ROM + $%04X\n",
                            screen_addr & 0x800));
    }

  tmp = (vic_ii.regs[0x18] & 0xe) << 10;
  tmp = (tmp + vic_ii.vbank_phi1);
  tmp &= vic_ii.vaddr_mask_phi1;
  tmp |= vic_ii.vaddr_offset_phi1;
  bitmap_base = vic_ii.ram_base_phi1 + (tmp & 0xe000);

  VIC_II_DEBUG_REGISTER(("\tBitmap memory at $%04X\n", tmp & 0xe000));

  if ((tmp & vic_ii.vaddr_chargen_mask_phi1) != vic_ii.vaddr_chargen_value_phi1)
    {
      char_base = vic_ii.ram_base_phi1 + tmp;
      VIC_II_DEBUG_REGISTER(("\tUser-defined character set at $%04X\n", tmp));
    }
  else
    {
      char_base = chargen_rom + (tmp & 0x0800);
      VIC_II_DEBUG_REGISTER(("\tStandard %s character set enabled\n",
                              tmp & 0x800 ? "Lower Case" : "Upper Case"));
    }

  tmp = VIC_II_RASTER_CHAR(cycle);

  if (vic_ii.idle_data_location != IDLE_NONE &&
                                old_vbank_p2 != vic_ii.vbank_phi2)
    {
      if (vic_ii.idle_data_location == IDLE_39FF)
        raster_add_int_change_foreground(&vic_ii.raster,
                                         VIC_II_RASTER_CHAR (cycle),
                                         &vic_ii.idle_data,
                                         vic_ii.ram_base_phi2[vic_ii.vbank_phi2
                                         + 0x39ff]);
      else
        raster_add_int_change_foreground(&vic_ii.raster,
                                         VIC_II_RASTER_CHAR (cycle),
                                         &vic_ii.idle_data,
                                         vic_ii.ram_base_phi2[vic_ii.vbank_phi2
                                         + 0x3fff]);
    }

  if (vic_ii.raster.skip_frame || (tmp <= 0 && clk < vic_ii.draw_clk))
    {
      old_screen_ptr = vic_ii.screen_ptr = screen_base;
      old_bitmap_ptr = vic_ii.bitmap_ptr = bitmap_base;
      old_chargen_ptr = vic_ii.chargen_ptr = char_base;
      old_vbank_p1 = vic_ii.vbank_phi1;
      old_vbank_p2 = vic_ii.vbank_phi2;
      /* vic_ii.vbank_ptr = vic_ii.ram_base + vic_ii.vbank; */
      vic_ii.raster.sprite_status->ptr_base = screen_base + 0x3f8;
    }
  else if (tmp < VIC_II_SCREEN_TEXTCOLS)
    {
      if (screen_base != old_screen_ptr)
        {
          raster_add_ptr_change_foreground(&vic_ii.raster, tmp,
                                           (void **) &vic_ii.screen_ptr,
                                           (void *) screen_base);
          raster_add_ptr_change_foreground(&vic_ii.raster, tmp,
                            (void **)&vic_ii.raster.sprite_status->ptr_base,
                                           (void *)(screen_base + 0x3f8));
          old_screen_ptr = screen_base;
        }

      if (bitmap_base != old_bitmap_ptr)
        {
          raster_add_ptr_change_foreground(&vic_ii.raster,
                                           tmp,
                                           (void **)&vic_ii.bitmap_ptr,
                                           (void *)(bitmap_base));
          old_bitmap_ptr = bitmap_base;
        }

      if (char_base != old_chargen_ptr)
        {
          raster_add_ptr_change_foreground(&vic_ii.raster,
                                           tmp,
                                           (void **)&vic_ii.chargen_ptr,
                                           (void *)char_base);
          old_chargen_ptr = char_base;
        }

      if (vic_ii.vbank_phi1 != old_vbank_p1)
        {
/*
          raster_add_ptr_change_foreground(&vic_ii.raster,
                                           tmp,
                                           (void **)&vic_ii.vbank_ptr,
                                           (void *)(vic_ii.ram_base
                                                    + vic_ii.vbank));
*/
          old_vbank_p1 = vic_ii.vbank_phi1;
        }

      if (vic_ii.vbank_phi2 != old_vbank_p2)
        {
          old_vbank_p2 = vic_ii.vbank_phi2;
        }
    }
  else
    {
      if (screen_base != old_screen_ptr)
        {
          raster_add_ptr_change_next_line(&vic_ii.raster,
                                          (void **)&vic_ii.screen_ptr,
                                          (void *)screen_base);
          raster_add_ptr_change_next_line(&vic_ii.raster,
                            (void **)&vic_ii.raster.sprite_status->ptr_base,
                                          (void *)(screen_base + 0x3f8));
          old_screen_ptr = screen_base;
        }
      if (bitmap_base != old_bitmap_ptr)
        {
          raster_add_ptr_change_next_line(&vic_ii.raster,
                                          (void **)&vic_ii.bitmap_ptr,
                                          (void *)(bitmap_base));
          old_bitmap_ptr = bitmap_base;
        }

      if (char_base != old_chargen_ptr)
        {
          raster_add_ptr_change_next_line(&vic_ii.raster,
                                          (void **)&vic_ii.chargen_ptr,
                                          (void *)char_base);
          old_chargen_ptr = char_base;
        }

      if (vic_ii.vbank_phi1 != old_vbank_p1)
        {
/*
          raster_add_ptr_change_next_line(&vic_ii.raster,
                                          (void **)&vic_ii.vbank_ptr,
                                          (void *)(vic_ii.ram_base
                                                   + vic_ii.vbank));
*/
          old_vbank_p1 = vic_ii.vbank_phi1;
        }

      if (vic_ii.vbank_phi2 != old_vbank_p2)
        {
          old_vbank_p2 = vic_ii.vbank_phi2;
        }
    }
}

/* Set the video mode according to the values in registers $D011 and $D016 of
   the VIC-II chip.  */
void vic_ii_update_video_mode(unsigned int cycle)
{
  static int old_video_mode = -1;
  int new_video_mode;

  new_video_mode = ((vic_ii.regs[0x11] & 0x60)
                    | (vic_ii.regs[0x16] & 0x10)) >> 4;

  if (new_video_mode != old_video_mode)
    {
      if (VIC_II_IS_ILLEGAL_MODE(new_video_mode))
        {
          /* Force the overscan color to black.  */
          raster_add_int_change_background
            (&vic_ii.raster, VIC_II_RASTER_X(cycle),
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
              (&vic_ii.raster, VIC_II_RASTER_X(cycle),
               &vic_ii.raster.overscan_background_color,
               vic_ii.regs[0x21]);
          vic_ii.force_black_overscan_background_color = 0;
        }

      {
        int pos;

        pos = VIC_II_RASTER_CHAR(cycle);

        raster_add_int_change_foreground(&vic_ii.raster, pos,
                                         &vic_ii.raster.video_mode,
                                         new_video_mode);

        if (vic_ii.idle_data_location != IDLE_NONE)
          {
            if (vic_ii.regs[0x11] & 0x40)
              raster_add_int_change_foreground
                (&vic_ii.raster, pos, (void *)&vic_ii.idle_data,
                 vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x39ff]);
            else
              raster_add_int_change_foreground
                (&vic_ii.raster, pos, (void *)&vic_ii.idle_data,
                 vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x3fff]);
          }
      }

      old_video_mode = new_video_mode;
    }

#ifdef VIC_II_VMODE_DEBUG
  switch (new_video_mode)
    {
    case VIC_II_NORMAL_TEXT_MODE:
      VIC_II_DEBUG_VMODE(("Standard Text"));
      break;
    case VIC_II_MULTICOLOR_TEXT_MODE:
      VIC_II_DEBUG_VMODE(("Multicolor Text"));
      break;
    case VIC_II_HIRES_BITMAP_MODE:
      VIC_II_DEBUG_VMODE(("Hires Bitmap"));
      break;
    case VIC_II_MULTICOLOR_BITMAP_MODE:
      VIC_II_DEBUG_VMODE(("Multicolor Bitmap"));
      break;
    case VIC_II_EXTENDED_TEXT_MODE:
      VIC_II_DEBUG_VMODE(("Extended Text"));
      break;
    case VIC_II_ILLEGAL_TEXT_MODE:
      VIC_II_DEBUG_VMODE(("Illegal Text"));
      break;
    case VIC_II_ILLEGAL_BITMAP_MODE_1:
      VIC_II_DEBUG_VMODE(("Invalid Bitmap"));
      break;
    case VIC_II_ILLEGAL_BITMAP_MODE_2:
      VIC_II_DEBUG_VMODE(("Invalid Bitmap"));
      break;
    default:                    /* cannot happen */
      VIC_II_DEBUG_VMODE(("???"));
    }

  VIC_II_DEBUG_VMODE((" Mode enabled at line $%04X, cycle %d.\n",
                      VIC_II_RASTER_Y (clk), cycle));
#endif
}

/* Redraw the current raster line.  This happens at cycle VIC_II_DRAW_CYCLE
   of each line.  */
void vic_ii_raster_draw_alarm_handler(CLOCK offset)
{
  BYTE prev_sprite_sprite_collisions;
  BYTE prev_sprite_background_collisions;
  int in_visible_area;

  prev_sprite_sprite_collisions = vic_ii.sprite_sprite_collisions;
  prev_sprite_background_collisions = vic_ii.sprite_background_collisions;

  in_visible_area = (vic_ii.raster.current_line >= vic_ii.first_displayed_line
                 && vic_ii.raster.current_line <= vic_ii.last_displayed_line);

  raster_emulate_line(&vic_ii.raster);

  if (vic_ii.raster.current_line == 0)
    {
      raster_skip_frame(&vic_ii.raster,
                        vsync_do_vsync(vic_ii.raster.skip_frame));
      vic_ii.memptr = 0;
      vic_ii.mem_counter = 0;
      vic_ii.light_pen.triggered = 0;

#ifdef __MSDOS__
      if (vic_ii.raster.viewport.width <= VIC_II_SCREEN_XPIX
          && vic_ii.raster.viewport.height <= VIC_II_SCREEN_YPIX
          && vic_ii.raster.viewport.update_canvas)
        canvas_set_border_color(vic_ii.raster.viewport.canvas,
                                vic_ii.raster.border_color);
#endif
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

  if (vic_ii.raster.current_line == vic_ii.first_dma_line)
    vic_ii.allow_bad_lines = !vic_ii.raster.blank;

  /* As explained in Christian's article, only the first collision
     (i.e. the first time the collision register becomes non-zero) actually
     triggers an interrupt.  */
  if (vic_ii_resources.sprite_sprite_collisions_enabled
      && vic_ii.raster.sprite_status->sprite_sprite_collisions != 0
      && !prev_sprite_sprite_collisions)
    {
      vic_ii.irq_status |= 0x4;
      if (vic_ii.regs[0x1a] & 0x4)
        {
          maincpu_set_irq(I_RASTER, 1);
          vic_ii.irq_status |= 0x80;
        }
    }

  if (vic_ii_resources.sprite_background_collisions_enabled
      && vic_ii.raster.sprite_status->sprite_background_collisions
      && !prev_sprite_background_collisions)
    {
      vic_ii.irq_status |= 0x2;
      if (vic_ii.regs[0x1a] & 0x2)
        {
          maincpu_set_irq(I_RASTER, 1);
          vic_ii.irq_status |= 0x80;
        }
    }

  if (vic_ii.idle_state)
    {
      if (vic_ii.regs[0x11] & 0x40)
        {
          vic_ii.idle_data_location = IDLE_39FF;
          vic_ii.idle_data = vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x39ff];
        }
      else
        {
          vic_ii.idle_data_location = IDLE_3FFF;
          vic_ii.idle_data = vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x3fff];
        }
    }
  else
    vic_ii.idle_data_location = IDLE_NONE;

  /* Set the next draw event.  */
  vic_ii.last_emulate_line_clk += vic_ii.cycles_per_line;
  vic_ii.draw_clk = vic_ii.last_emulate_line_clk + vic_ii.draw_cycle;
  alarm_set(&vic_ii.raster_draw_alarm, vic_ii.draw_clk);
}

inline static int handle_fetch_matrix(long offset, CLOCK sub,
                                      CLOCK *write_offset)
{
  raster_t *raster;
  raster_sprite_status_t *sprite_status;

  *write_offset = 0;

  raster = &vic_ii.raster;
  sprite_status = raster->sprite_status;

  if (sprite_status->visible_msk == 0 && sprite_status->dma_msk == 0)
    {
      do_matrix_fetch(sub);

      /* As sprites are all turned off, there is no need for a sprite DMA
         check; next time we will VIC_II_FETCH_MATRIX again.  This works
         because a VIC_II_CHECK_SPRITE_DMA is forced in `vic_store()'
         whenever the mask becomes nonzero.  */

      /* This makes sure we only create VIC_II_FETCH_MATRIX events in the bad
         line range.  These checks are (a little) redundant for safety.  */
      if (raster->current_line < vic_ii.first_dma_line)
        vic_ii.fetch_clk += ((vic_ii.first_dma_line
                              - raster->current_line)
                             * vic_ii.cycles_per_line);
      else if (raster->current_line >= vic_ii.last_dma_line)
        vic_ii.fetch_clk += ((vic_ii.screen_height
                              - raster->current_line
                              + vic_ii.first_dma_line)
                             * vic_ii.cycles_per_line);
      else
        vic_ii.fetch_clk += vic_ii.cycles_per_line;

      alarm_set(&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
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
                          + vic_ii.sprite_fetch_cycle);

      if (vic_ii.fetch_clk > clk || offset == 0)
        {
          /* Prepare the next fetch event.  */
          alarm_set(&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
          return 1;
        }

      if (fetch_done && sub == 0)
        *write_offset = VIC_II_SCREEN_TEXTCOLS + 3;
    }

  return 0;
}

inline static void swap_sprite_data_buffers(void)
{
  DWORD *tmp;
  raster_sprite_status_t *sprite_status;

  /* Swap sprite data buffers.  */
  sprite_status = vic_ii.raster.sprite_status;
  tmp = sprite_status->sprite_data;
  sprite_status->sprite_data = sprite_status->new_sprite_data;
  sprite_status->new_sprite_data = tmp;
}

inline static int handle_check_sprite_dma(long offset, CLOCK sub)
{
  swap_sprite_data_buffers ();

  check_sprite_dma ();

  /* FIXME?  Slow!  */
  vic_ii.sprite_fetch_clk = (VIC_II_LINE_START_CLK (clk)
                             + vic_ii.sprite_fetch_cycle);
  vic_ii.sprite_fetch_msk = vic_ii.raster.sprite_status->new_dma_msk;

  if (vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][0].cycle == -1)
    {
      if (vic_ii.raster.current_line >= vic_ii.first_dma_line - 1
          && vic_ii.raster.current_line <= vic_ii.last_dma_line + 1)
        {
          vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              -vic_ii.sprite_fetch_cycle
                              + VIC_II_FETCH_CYCLE
                              + vic_ii.cycles_per_line);
        }
      else
        {
          vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              + vic_ii.cycles_per_line);
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
      alarm_set(&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
      return 1;
    }

  return 0;
}

inline static int handle_fetch_sprite(long offset, CLOCK sub,
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

      sprite_status = vic_ii.raster.sprite_status;
      /* FIXME: the 3 byte sprite data is instead taken during a Ph1/Ph2/Ph1
         sequence. This is of minor interest, though, only for CBM-II... */
      bank = vic_ii.ram_base_phi1 + vic_ii.vbank_phi1;
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

          dest[0] = src[my_memptr];
          dest[1] = src[++my_memptr & 0x3f];
          dest[2] = src[++my_memptr & 0x3f];
        }
    }

  maincpu_steal_cycles(vic_ii.fetch_clk, sf->num - sub);

  *write_offset = sub == 0 ? sf->num : 0;

  next_cycle = (sf + 1)->cycle;
  vic_ii.sprite_fetch_idx++;

  if (next_cycle == -1)
    {
      /* Next time, handle bad lines.  */
      if (vic_ii.raster.current_line >= vic_ii.first_dma_line - 1
          && vic_ii.raster.current_line <= vic_ii.last_dma_line + 1)
        {
          vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              - vic_ii.sprite_fetch_cycle
                              + VIC_II_FETCH_CYCLE
                              + vic_ii.cycles_per_line);
        }
      else
        {
          vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
          vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                              + vic_ii.cycles_per_line);
        }
    }
  else
    vic_ii.fetch_clk = vic_ii.sprite_fetch_clk + next_cycle;

  if (clk >= vic_ii.draw_clk)
    vic_ii_raster_draw_alarm_handler(clk - vic_ii.draw_clk);

  if (vic_ii.fetch_clk > clk || offset == 0)
    {
      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
      return 1;
    }

  if (clk >= vic_ii.raster_irq_clk)
    vic_ii_raster_irq_alarm_handler(clk - vic_ii.raster_irq_clk);

  return 0;
}

/* Handle sprite/matrix fetch events.  FIXME: could be made slightly
   faster.  */
void vic_ii_raster_fetch_alarm_handler(CLOCK offset)
{
  CLOCK last_opcode_first_write_clk, last_opcode_last_write_clk;

  /* This kludgy thing is used to emulate the behavior of the 6510 when BA
     goes low.  When BA goes low, every read access stops the processor
     until BA is high again; write accesses happen as usual instead.  */

  if (offset > 0)
    {
      switch (OPINFO_NUMBER(last_opcode_info))
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
          leave = handle_fetch_matrix(offset, sub, &write_offset);
          last_opcode_first_write_clk += write_offset;
          last_opcode_last_write_clk += write_offset;
          break;

        case VIC_II_CHECK_SPRITE_DMA:
          leave = handle_check_sprite_dma (offset, sub);
          break;

        case VIC_II_FETCH_SPRITE:
        default:                /* Make compiler happy.  */
          leave = handle_fetch_sprite(offset, sub, &write_offset);
          last_opcode_first_write_clk += write_offset;
          last_opcode_last_write_clk += write_offset;
          break;
        }

      if (leave)
        break;
    }
}

/* If necessary, emulate a raster compare IRQ. This is called when the raster
   line counter matches the value stored in the raster line register.  */
static void vic_ii_raster_irq_alarm_handler(CLOCK offset)
{
  vic_ii.irq_status |= 0x1;
  if (vic_ii.regs[0x1a] & 0x1)
    {
      maincpu_set_irq_clk(I_RASTER, 1, vic_ii.raster_irq_clk);
      vic_ii.irq_status |= 0x80;
      VIC_II_DEBUG_RASTER (("VIC: *** IRQ requested at line $%04X, "
                "vic_ii.raster_irq_line=$%04X, offset = %ld, cycle = %d.\n",
                      VIC_II_RASTER_Y (clk), vic_ii.raster_irq_line, offset,
                            VIC_II_RASTER_CYCLE(clk)));
    }

  vic_ii.raster_irq_clk += vic_ii.screen_height * vic_ii.cycles_per_line;
  alarm_set (&vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
}

int vic_ii_calc_palette(int sat,int con,int bri,int gam,int newlum,int mixedcols)
{
  palette_t *palette;

  palette = vic_ii_color_calcpalette(VIC_II_COLOR_PALETTE_16,sat,con,bri,gam,newlum);
  return raster_set_palette(&vic_ii.raster, palette);
}


/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int vic_ii_load_palette(const char *name)
{
  palette_t *palette;

  palette = palette_create(VIC_II_NUM_COLORS, vic_ii_color_names);
  if (palette == NULL)
    return -1;

  if (!console_mode && !vsid_mode && palette_load(name, palette) < 0)
    {
      log_message(vic_ii.log, "Cannot load palette file `%s'.", name);
      return -1;
    }

  return raster_set_palette(&vic_ii.raster, palette);
}

/* Set proper functions and constants for the current video settings.  */
void vic_ii_resize(void)
{
  if (!vic_ii.initialized)
    return;

#ifdef VIC_II_NEED_2X
#ifdef USE_XF86_EXTENSIONS
  if (fullscreen_is_enabled
      ? vic_ii_resources.fullscreen_double_size_enabled
      : vic_ii_resources.double_size_enabled)
#else
  if (vic_ii_resources.double_size_enabled)
#endif
#else
  if (0)
#endif
    {
      if (vic_ii.raster.viewport.pixel_size.width == 1
          && vic_ii.raster.viewport.canvas != NULL) {
        raster_set_pixel_size(&vic_ii.raster, 2, 2);
        raster_resize_viewport(&vic_ii.raster,
                               vic_ii.raster.viewport.width * 2,
                               vic_ii.raster.viewport.height * 2);
      } else {
          raster_set_pixel_size(&vic_ii.raster, 2, 2);
      }

      vic_ii_draw_set_double_size(1);
      vic_ii_sprites_set_double_size(1);
    }
  else
    {
      if (vic_ii.raster.viewport.pixel_size.width == 2
          && vic_ii.raster.viewport.canvas != NULL) {
          raster_set_pixel_size(&vic_ii.raster, 1, 1);
        raster_resize_viewport(&vic_ii.raster,
                               vic_ii.raster.viewport.width / 2,
                               vic_ii.raster.viewport.height / 2);
      } else {
          raster_set_pixel_size(&vic_ii.raster, 1, 1);
      }

      vic_ii_draw_set_double_size(0);
      vic_ii_sprites_set_double_size(0);
    }
}

void vic_ii_set_set_canvas_refresh(int enable)
{
    raster_t *raster;

    raster = &vic_ii.raster;

    raster_set_canvas_refresh(raster, enable);
}

int vic_ii_write_snapshot_module(snapshot_t *s)
{
    return ted_snapshot_write_module(s);
}

int vic_ii_read_snapshot_module(snapshot_t *s)
{
    return ted_snapshot_read_module(s);
}

void ted_free(void)
{
    raster_free(&vic_ii.raster);
}

int vic_ii_screenshot(screenshot_t *screenshot)
{
    return raster_screenshot(&vic_ii.raster, screenshot);
}

void vic_ii_video_refresh(void)
{
#ifdef USE_XF86_EXTENSIONS

  vic_ii_resize();
  raster_enable_double_scan(&vic_ii.raster,
                            fullscreen_is_enabled ?
                            vic_ii_resources.fullscreen_double_scan_enabled :
                            vic_ii_resources.double_scan_enabled);
#endif
}

