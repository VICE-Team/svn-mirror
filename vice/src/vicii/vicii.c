/*
 * vicii.c - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
 *
 * Written by
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

/* TODO: - speed optimizations;
   - faster sprites and registers.  */

/*
   Current (most important) known limitations:

   - sprite colors (and other attributes) cannot change in the middle of the
   raster line;

   Probably something else which I have not figured out yet...

 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "c64.h"
#include "cartridge.h"
#include "c64cart.h"
#include "clkguard.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "resources.h"
#include "screenshot.h"
#include "snapshot.h"
#include "types.h"
#include "utils.h"
#include "vicii-cmdline-options.h"
#include "vicii-color.h"
#include "vicii-draw.h"
#include "vicii-fetch.h"
#include "vicii-mem.h"
#include "vicii-sprites.h"
#include "vicii-resources.h"
#include "vicii-snapshot.h"
#include "vicii.h"
#include "viciitypes.h"
#include "vsync.h"
#include "videoarch.h"
#include "video.h"


void vic_ii_set_phi1_addr_options(ADDRESS mask, ADDRESS offset)
{
    vic_ii.vaddr_mask_phi1 = mask;
    vic_ii.vaddr_offset_phi1 = offset;

    VIC_II_DEBUG_REGISTER(("Set phi1 video addr mask=%04x, offset=%04x",
                          mask, offset));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi2_addr_options(ADDRESS mask, ADDRESS offset)
{
    vic_ii.vaddr_mask_phi2 = mask;
    vic_ii.vaddr_offset_phi2 = offset;

    VIC_II_DEBUG_REGISTER(("Set phi2 video addr mask=%04x, offset=%04x",
                          mask, offset));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi1_chargen_addr_options(ADDRESS mask, ADDRESS value)
{
    vic_ii.vaddr_chargen_mask_phi1 = mask;
    vic_ii.vaddr_chargen_value_phi1 = value;

    VIC_II_DEBUG_REGISTER(("Set phi1 chargen addr mask=%04x, value=%04x",
                          mask, value));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_phi2_chargen_addr_options(ADDRESS mask, ADDRESS value)
{
    vic_ii.vaddr_chargen_mask_phi2 = mask;
    vic_ii.vaddr_chargen_value_phi2 = value;

    VIC_II_DEBUG_REGISTER(("Set phi2 chargen addr mask=%04x, value=%04x",
                          mask, value));
    vic_ii_update_memory_ptrs_external();
}

void vic_ii_set_chargen_addr_options(ADDRESS mask, ADDRESS value)
{
    vic_ii.vaddr_chargen_mask_phi1 = mask;
    vic_ii.vaddr_chargen_value_phi1 = value;
    vic_ii.vaddr_chargen_mask_phi2 = mask;
    vic_ii.vaddr_chargen_value_phi2 = value;

    VIC_II_DEBUG_REGISTER(("Set chargen addr mask=%04x, value=%04x",
                          mask, value));
    vic_ii_update_memory_ptrs_external();
}

/* ---------------------------------------------------------------------*/

vic_ii_t vic_ii;

static void vic_ii_set_geometry(void);

/* Handle the exposure event.  */
static void vic_ii_exposure_handler(unsigned int width, unsigned int height)
{
    vic_ii.raster.canvas->draw_buffer->canvas_width = width;
    vic_ii.raster.canvas->draw_buffer->canvas_height = height;
    video_viewport_resize(vic_ii.raster.canvas);
}

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

    resources_get_value("MachineVideoStandard", &mode);

    switch ((int)mode) {
      case MACHINE_SYNC_NTSC:
        clk_guard_set_clk_base(&maincpu_clk_guard, C64_NTSC_CYCLES_PER_RFSH);
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
      case MACHINE_SYNC_NTSCOLD:
        clk_guard_set_clk_base(&maincpu_clk_guard, C64_NTSCOLD_CYCLES_PER_RFSH);
        vic_ii.screen_height = VIC_II_NTSCOLD_SCREEN_HEIGHT;
        vic_ii.first_displayed_line = VIC_II_NTSCOLD_FIRST_DISPLAYED_LINE;
        vic_ii.last_displayed_line = VIC_II_NTSCOLD_LAST_DISPLAYED_LINE;
        vic_ii.row_25_start_line = VIC_II_NTSCOLD_25ROW_START_LINE;
        vic_ii.row_25_stop_line = VIC_II_NTSCOLD_25ROW_STOP_LINE;
        vic_ii.row_24_start_line = VIC_II_NTSCOLD_24ROW_START_LINE;
        vic_ii.row_24_stop_line = VIC_II_NTSCOLD_24ROW_STOP_LINE;
        vic_ii.screen_borderwidth = VIC_II_SCREEN_NTSCOLD_BORDERWIDTH;
        vic_ii.screen_borderheight = VIC_II_SCREEN_NTSCOLD_BORDERHEIGHT;
        vic_ii.cycles_per_line = VIC_II_NTSCOLD_CYCLES_PER_LINE;
        vic_ii.draw_cycle = VIC_II_NTSCOLD_DRAW_CYCLE;
        vic_ii.sprite_fetch_cycle = VIC_II_NTSCOLD_SPRITE_FETCH_CYCLE;
        vic_ii.sprite_wrap_x = VIC_II_NTSCOLD_SPRITE_WRAP_X;
        vic_ii.first_dma_line = VIC_II_NTSCOLD_FIRST_DMA_LINE;
        vic_ii.last_dma_line = VIC_II_NTSCOLD_LAST_DMA_LINE;
        vic_ii.offset = VIC_II_NTSCOLD_OFFSET;
        break;
      case MACHINE_SYNC_PAL:
      default:
        clk_guard_set_clk_base(&maincpu_clk_guard, C64_PAL_CYCLES_PER_RFSH);
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

    if (vic_ii.initialized) {
        vic_ii_set_geometry();
        raster_mode_change();
    }
}

static CLOCK old_maincpu_clk = 0;

void vic_ii_delay_oldclk(CLOCK num)
{
    old_maincpu_clk += num;
}

inline void vic_ii_delay_clk(void)
{
#if 0
    CLOCK diff;

    /*log_debug("MCLK %d OMCLK %d", maincpu_clk, old_maincpu_clk);*/

    if (vic_ii.fastmode == 0) {
        diff = maincpu_clk - old_maincpu_clk;
        maincpu_steal_cycles(maincpu_clk, diff, 0);
    }

    old_maincpu_clk = maincpu_clk;

    return;
#endif
}

inline void vic_ii_handle_pending_alarms(int num_write_cycles)
{
    if (vic_ii.viciie != 0)
        vic_ii_delay_clk();

    if (num_write_cycles != 0) {
        int f;

        /* Cycles can be stolen only during the read accesses, so we serve
           only the events that happened during them.  The last read access
           happened at `clk - maincpu_write_cycles()' as all the opcodes
           except BRK and JSR do all the write accesses at the very end.  BRK
           cannot take us here and we would not be able to handle JSR
           correctly anyway, so we don't care about them...  */

        /* Go back to the time when the read accesses happened and serve VIC
         events.  */
        maincpu_clk -= num_write_cycles;

        do {
            f = 0;
            if (maincpu_clk > vic_ii.fetch_clk) {
                vic_ii_fetch_alarm_handler(0);
                f = 1;
                if (vic_ii.viciie != 0)
                    vic_ii_delay_clk();
            }
            if (maincpu_clk >= vic_ii.draw_clk) {
                vic_ii_raster_draw_alarm_handler((long)(maincpu_clk
                                                 - vic_ii.draw_clk));
                f = 1;
                if (vic_ii.viciie != 0)
                    vic_ii_delay_clk();
            }
        }
        while (f);

        /* Go forward to the time when the last write access happens (that's
           the one we care about, as the only instructions that do two write
           accesses - except BRK and JSR - are the RMW ones, which store the
           old value in the first write access, and then store the new one in
           the second write access).  */
        maincpu_clk += num_write_cycles;

    } else {
        int f;

        do {
            f = 0;
            if (maincpu_clk >= vic_ii.fetch_clk) {
                vic_ii_fetch_alarm_handler(0);
                f = 1;
                if (vic_ii.viciie != 0)
                    vic_ii_delay_clk();
            }
            if (maincpu_clk >= vic_ii.draw_clk) {
                vic_ii_raster_draw_alarm_handler(0);
                f = 1;
                if (vic_ii.viciie != 0)
                    vic_ii_delay_clk();
            }
        }
        while (f);
    }
}

void vic_ii_handle_pending_alarms_external(int num_write_cycles)
{
    if (vic_ii.initialized)
        vic_ii_handle_pending_alarms(num_write_cycles);
}

static void vic_ii_set_geometry(void)
{
    unsigned int width, height;

    width = VIC_II_SCREEN_XPIX + vic_ii.screen_borderwidth * 2;
    height = vic_ii.last_displayed_line - vic_ii.first_displayed_line + 1;

    raster_set_geometry(&vic_ii.raster,
                        width, height,
                        VIC_II_SCREEN_XPIX + vic_ii.screen_borderwidth * 2,
                        vic_ii.screen_height,
                        VIC_II_SCREEN_XPIX, VIC_II_SCREEN_YPIX,
                        VIC_II_SCREEN_TEXTCOLS, VIC_II_SCREEN_TEXTLINES,
                        vic_ii.screen_borderwidth, vic_ii.row_25_start_line,
                        0,
                        vic_ii.first_displayed_line,
                        vic_ii.last_displayed_line,
#if 0
                        vic_ii.sprite_wrap_x - VIC_II_SCREEN_XPIX -
                        vic_ii.screen_borderwidth * 2,
#else
                        0,
#endif
                        vic_ii.sprite_wrap_x - VIC_II_SCREEN_XPIX -
                        vic_ii.screen_borderwidth * 2);
#ifdef __MSDOS__
    video_ack_vga_mode();
#endif

}

static int init_raster(void)
{
    raster_t *raster;
    char *title;

    raster = &vic_ii.raster;
    video_color_set_raster(raster);

    if (raster_init(raster, VIC_II_NUM_VMODES, VIC_II_NUM_SPRITES) < 0)
        return -1;
    raster_modes_set_idle_mode(raster->modes, VIC_II_IDLE_MODE);
    raster_set_exposure_handler(raster, (void*)vic_ii_exposure_handler);
    resources_touch("VICIIVideoCache");

    vic_ii_set_geometry();

    if (vic_ii_update_palette() < 0) {
        log_error(vic_ii.log, "Cannot load palette.");
        return -1;
    }
    title = concat("VICE: ", machine_name, " emulator", NULL);
    raster_set_title(raster, title);
    free(title);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = vic_ii.row_25_start_line;
    raster->display_ystop = vic_ii.row_25_stop_line;
    raster->display_xstart = VIC_II_40COL_START_PIXEL;
    raster->display_xstop = VIC_II_40COL_STOP_PIXEL;

    return 0;
}

/* Initialize the VIC-II emulation.  */
raster_t *vic_ii_init(unsigned int flag)
{
    vic_ii.viciie = flag;

    if (vic_ii.viciie == VICII_EXTENDED)
        vic_ii.log = log_open("VIC-IIe");
    else
        vic_ii.log = log_open("VIC-II");

    vic_ii.raster_fetch_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));
    vic_ii.raster_draw_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));
    vic_ii.raster_irq_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));

    alarm_init(vic_ii.raster_fetch_alarm, maincpu_alarm_context,
               "VicIIRasterFetch", vic_ii_fetch_alarm_handler);
    alarm_init(vic_ii.raster_draw_alarm, maincpu_alarm_context,
               "VicIIRasterDraw", vic_ii_raster_draw_alarm_handler);
    alarm_init(vic_ii.raster_irq_alarm, maincpu_alarm_context,
               "VicIIRasterIrq", vic_ii_raster_irq_alarm_handler);

    vic_ii_change_timing();

    if (init_raster() < 0)
        return NULL;

    vic_ii_powerup();

    vic_ii.video_mode = -1;
    vic_ii_update_video_mode(0);
    vic_ii_update_memory_ptrs(0);

    vic_ii_draw_init();
    vic_ii_sprites_init();

    vic_ii.num_idle_3fff = 0;
    vic_ii.num_idle_3fff_old = 0;
    vic_ii.idle_3fff = (idle_3fff_t *)xmalloc(sizeof(idle_3fff_t) * 64);
    vic_ii.idle_3fff_old = (idle_3fff_t *)xmalloc(sizeof(idle_3fff_t) * 64);

    vic_ii.buf_offset = 0;

    vic_ii.initialized = 1;

    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);

    return &vic_ii.raster;
}

struct video_canvas_s *vic_ii_get_canvas(void)
{
    return vic_ii.raster.canvas;
}

/* Reset the VIC-II chip.  */
void vic_ii_reset(void)
{
    vic_ii_reset_registers();

    raster_reset(&vic_ii.raster);

    vic_ii.last_emulate_line_clk = 0;

    vic_ii.draw_clk = vic_ii.draw_cycle;
    alarm_set(vic_ii.raster_draw_alarm, vic_ii.draw_clk);

    vic_ii.fetch_clk = VIC_II_FETCH_CYCLE;
    alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
    vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
    vic_ii.sprite_fetch_idx = 0;
    vic_ii.sprite_fetch_msk = 0;
    vic_ii.sprite_fetch_clk = CLOCK_MAX;

    /* FIXME: I am not sure this is exact emulation.  */
    vic_ii.raster_irq_line = 0;
    vic_ii.raster_irq_clk = 0;

    /* Setup the raster IRQ alarm.  The value is `1' instead of `0' because we
       are at the first line, which has a +1 clock cycle delay in IRQs.  */
    alarm_set(vic_ii.raster_irq_alarm, 1);

    vic_ii.force_display_state = 0;

    vic_ii.light_pen.triggered = 0;
    vic_ii.light_pen.x = vic_ii.light_pen.y = 0;

    /* Remove all the IRQ sources.  */
    vic_ii.regs[0x1a] = 0;

    vic_ii.raster.display_ystart = vic_ii.row_25_start_line;
    vic_ii.raster.display_ystop = vic_ii.row_25_stop_line;

    vic_ii.store_clk = CLOCK_MAX;
}

void vic_ii_reset_registers(void)
{
    ADDRESS i;

    if (!vic_ii.initialized)
        return;

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
    vic_ii.ram_base_phi1 = mem_ram;
    vic_ii.ram_base_phi2 = mem_ram;

    vic_ii.vaddr_mask_phi1 = 0xffff;
    vic_ii.vaddr_mask_phi2 = 0xffff;
    vic_ii.vaddr_offset_phi1 = 0;
    vic_ii.vaddr_offset_phi2 = 0;

    if (vic_ii.viciie == VICII_EXTENDED) {
        vic_ii.vaddr_chargen_mask_phi1 = 0x3000;
        vic_ii.vaddr_chargen_mask_phi2 = 0x3000;
    } else {
        vic_ii.vaddr_chargen_mask_phi1 = 0x7000;
        vic_ii.vaddr_chargen_mask_phi2 = 0x7000;
    }
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
    vic_ii.last_emulate_line_clk = 0;

    vic_ii_reset();

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
    vic_ii_handle_pending_alarms(maincpu_rmw_flag + 1);
    if (maincpu_clk >= vic_ii.draw_clk)
        vic_ii_raster_draw_alarm_handler(maincpu_clk - vic_ii.draw_clk);

    vic_ii.vbank_phi1 = vbank_p1;
    vic_ii.vbank_phi2 = vbank_p2;
    vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE(maincpu_clk));
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
    if (!vic_ii.light_pen.triggered) {
        vic_ii.light_pen.triggered = 1;
        vic_ii.light_pen.x = VIC_II_RASTER_X(mclk % vic_ii.cycles_per_line);
        if (vic_ii.light_pen.x < 0)
            vic_ii.light_pen.x = vic_ii.sprite_wrap_x + vic_ii.light_pen.x;

        /* FIXME: why `+2'? */
        vic_ii.light_pen.x = vic_ii.light_pen.x / 2 + 2;
        vic_ii.light_pen.y = VIC_II_RASTER_Y(mclk);
        vic_ii.irq_status |= 0x8;

        if (vic_ii.regs[0x1a] & 0x8) {
            vic_ii.irq_status |= 0x80;
            maincpu_set_irq_clk(I_RASTER, 1, mclk);
        }
    }
}

/* Make sure all the VIC-II alarms are removed.  This just makes it easier to
   write functions for loading snapshot modules in other video chips without
   caring that the VIC-II alarms are dispatched when they really shouldn't
   be.  */
void vic_ii_prepare_for_snapshot(void)
{
    vic_ii.fetch_clk = CLOCK_MAX;
    alarm_unset(vic_ii.raster_fetch_alarm);
    vic_ii.draw_clk = CLOCK_MAX;
    alarm_unset(vic_ii.raster_draw_alarm);
    vic_ii.raster_irq_clk = CLOCK_MAX;
    alarm_unset(vic_ii.raster_irq_alarm);
}

void vic_ii_set_raster_irq(unsigned int line)
{
    if (line == vic_ii.raster_irq_line && vic_ii.raster_irq_clk != CLOCK_MAX)
        return;

    if (line < vic_ii.screen_height) {
        unsigned int current_line = VIC_II_RASTER_Y(maincpu_clk);

        vic_ii.raster_irq_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                                 + VIC_II_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                                 + (vic_ii.cycles_per_line
                                 * (line - current_line)));

        /* Raster interrupts on line 0 are delayed by 1 cycle.  */
        if (line == 0)
            vic_ii.raster_irq_clk++;

        if (line <= current_line)
            vic_ii.raster_irq_clk += (vic_ii.screen_height
                                     * vic_ii.cycles_per_line);
        alarm_set(vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
    } else {
        VIC_II_DEBUG_RASTER(("update_raster_irq(): "
                            "raster compare out of range ($%04X)!", line));
        alarm_unset(vic_ii.raster_irq_alarm);
    }

    VIC_II_DEBUG_RASTER(("update_raster_irq(): "
                        "vic_ii.raster_irq_clk = %ul, "
                        "line = $%04X, "
                        "vic_ii.regs[0x1a] & 1 = %d",
                        vic_ii.raster_irq_clk, line, vic_ii.regs[0x1a] & 1));

    vic_ii.raster_irq_line = line;
}

/* Change the base of RAM seen by the VIC-II.  */
static inline void vic_ii_set_ram_bases(BYTE *base_p1, BYTE *base_p2)
{
    /* WARNING: assumes `maincpu_rmw_flag' is 0 or 1.  */
    vic_ii_handle_pending_alarms(maincpu_rmw_flag + 1);

    vic_ii.ram_base_phi1 = base_p1;
    vic_ii.ram_base_phi2 = base_p2;
    vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE(maincpu_clk));
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
        vic_ii_update_memory_ptrs(VIC_II_RASTER_CYCLE(maincpu_clk));
}

/* Set the memory pointers according to the values in the registers.  */
void vic_ii_update_memory_ptrs(unsigned int cycle)
{
    /* FIXME: This is *horrible*!  */
    static BYTE *old_screen_ptr, *old_bitmap_ptr, *old_chargen_ptr;
    static int old_vbank_p1 = -1;
    static int old_vbank_p2 = -1;
    ADDRESS screen_addr;          /* Screen start address.  */
    /*BYTE *screen_base;*/            /* Pointer to screen memory.  */
    BYTE *char_base;              /* Pointer to character memory.  */
    BYTE *bitmap_base;            /* Pointer to bitmap memory.  */
    int tmp;

    screen_addr = vic_ii.vbank_phi2 + ((vic_ii.regs[0x18] & 0xf0) << 6);

    screen_addr = (screen_addr & vic_ii.vaddr_mask_phi2)
                  | vic_ii.vaddr_offset_phi2;

    if ((screen_addr & vic_ii.vaddr_chargen_mask_phi2)
        != vic_ii.vaddr_chargen_value_phi2) {
        vic_ii.screen_base = vic_ii.ram_base_phi2 + screen_addr;
        VIC_II_DEBUG_REGISTER(("Video memory at $%04X", screen_addr));
    } else {
        vic_ii.screen_base = mem_chargen_rom_ptr + (screen_addr & 0x800);
        VIC_II_DEBUG_REGISTER(("Video memory at Character ROM + $%04X",
                              screen_addr & 0x800));
    }

    tmp = (vic_ii.regs[0x18] & 0xe) << 10;
    tmp = (tmp + vic_ii.vbank_phi1);
    tmp &= vic_ii.vaddr_mask_phi1;
    tmp |= vic_ii.vaddr_offset_phi1;
    bitmap_base = vic_ii.ram_base_phi1 + (tmp & 0xe000);

    VIC_II_DEBUG_REGISTER(("Bitmap memory at $%04X", tmp & 0xe000));

    if (cart_ultimax_phi1 != 0) {
        char_base = ((tmp & 0x3fff) >= 0x3000
                    ? romh_banks + (romh_bank << 13) + (tmp & 0xfff) + 0x1000
                    : vic_ii.ram_base_phi1 + tmp);
    } else {
        if ((tmp & vic_ii.vaddr_chargen_mask_phi1)
            != vic_ii.vaddr_chargen_value_phi1) {
            char_base = vic_ii.ram_base_phi1 + tmp;
            VIC_II_DEBUG_REGISTER(("User-defined charset at $%04X", tmp));
        } else {
            char_base = mem_chargen_rom_ptr + (tmp & 0x0800);
            VIC_II_DEBUG_REGISTER(("Standard %s charset enabled",
                                  tmp & 0x800 ? "Lower Case" : "Upper Case"));
        }
    }

    tmp = VIC_II_RASTER_CHAR(cycle);

    if (vic_ii.idle_data_location != IDLE_NONE &&
        old_vbank_p2 != vic_ii.vbank_phi2) {
        if (vic_ii.idle_data_location == IDLE_39FF)
            raster_add_int_change_foreground(&vic_ii.raster,
                                             VIC_II_RASTER_CHAR(cycle),
                                             &vic_ii.idle_data,
                                             vic_ii.ram_base_phi2[vic_ii.vbank_phi2
                                             + 0x39ff]);
        else
            raster_add_int_change_foreground(&vic_ii.raster,
                                             VIC_II_RASTER_CHAR(cycle),
                                             &vic_ii.idle_data,
                                             vic_ii.ram_base_phi2[vic_ii.vbank_phi2
                                             + 0x3fff]);
    }

    if (tmp <= 0 && maincpu_clk < vic_ii.draw_clk) {
        old_screen_ptr = vic_ii.screen_ptr = vic_ii.screen_base;
        old_bitmap_ptr = vic_ii.bitmap_ptr = bitmap_base;
        old_chargen_ptr = vic_ii.chargen_ptr = char_base;
        old_vbank_p1 = vic_ii.vbank_phi1;
        old_vbank_p2 = vic_ii.vbank_phi2;
        /* vic_ii.vbank_ptr = vic_ii.ram_base + vic_ii.vbank; */
        vic_ii.raster.sprite_status->ptr_base = vic_ii.screen_base + 0x3f8;
    } else if (tmp < VIC_II_SCREEN_TEXTCOLS) {
        if (vic_ii.screen_base != old_screen_ptr) {
            raster_add_ptr_change_foreground(&vic_ii.raster, tmp,
                                             (void **)&vic_ii.screen_ptr,
                                             (void *)vic_ii.screen_base);
            raster_add_ptr_change_foreground(&vic_ii.raster, tmp,
                              (void **)&vic_ii.raster.sprite_status->ptr_base,
                                             (void *)(vic_ii.screen_base
                                             + 0x3f8));
            old_screen_ptr = vic_ii.screen_base;
        }

        if (bitmap_base != old_bitmap_ptr) {
            raster_add_ptr_change_foreground(&vic_ii.raster,
                                             tmp,
                                             (void **)&vic_ii.bitmap_ptr,
                                             (void *)(bitmap_base));
            old_bitmap_ptr = bitmap_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_foreground(&vic_ii.raster,
                                             tmp,
                                             (void **)&vic_ii.chargen_ptr,
                                             (void *)char_base);
            old_chargen_ptr = char_base;
        }

        if (vic_ii.vbank_phi1 != old_vbank_p1) {
/*
            raster_add_ptr_change_foreground(&vic_ii.raster,
                                             tmp,
                                             (void **)&vic_ii.vbank_ptr,
                                             (void *)(vic_ii.ram_base
                                             + vic_ii.vbank));
*/
            old_vbank_p1 = vic_ii.vbank_phi1;
        }

        if (vic_ii.vbank_phi2 != old_vbank_p2) {
            old_vbank_p2 = vic_ii.vbank_phi2;
        }
    } else {
        if (vic_ii.screen_base != old_screen_ptr) {
            raster_add_ptr_change_next_line(&vic_ii.raster,
                                            (void **)&vic_ii.screen_ptr,
                                            (void *)vic_ii.screen_base);
            raster_add_ptr_change_next_line(&vic_ii.raster,
                              (void **)&vic_ii.raster.sprite_status->ptr_base,
                                            (void *)(vic_ii.screen_base
                                            + 0x3f8));
            old_screen_ptr = vic_ii.screen_base;
        }
        if (bitmap_base != old_bitmap_ptr) {
            raster_add_ptr_change_next_line(&vic_ii.raster,
                                            (void **)&vic_ii.bitmap_ptr,
                                            (void *)(bitmap_base));
            old_bitmap_ptr = bitmap_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_next_line(&vic_ii.raster,
                                            (void **)&vic_ii.chargen_ptr,
                                            (void *)char_base);
            old_chargen_ptr = char_base;
        }

        if (vic_ii.vbank_phi1 != old_vbank_p1) {
/*
            raster_add_ptr_change_next_line(&vic_ii.raster,
                                            (void **)&vic_ii.vbank_ptr,
                                            (void *)(vic_ii.ram_base
                                            + vic_ii.vbank));
*/
            old_vbank_p1 = vic_ii.vbank_phi1;
        }

        if (vic_ii.vbank_phi2 != old_vbank_p2) {
            old_vbank_p2 = vic_ii.vbank_phi2;
        }
    }
}

/* Set the video mode according to the values in registers $D011 and $D016 of
   the VIC-II chip.  */
void vic_ii_update_video_mode(unsigned int cycle)
{
    int new_video_mode;

    new_video_mode = ((vic_ii.regs[0x11] & 0x60)
                     | (vic_ii.regs[0x16] & 0x10)) >> 4;

    if (new_video_mode != vic_ii.video_mode) {
        switch (new_video_mode) {
          case VIC_II_ILLEGAL_TEXT_MODE:
          case VIC_II_ILLEGAL_BITMAP_MODE_1:
          case VIC_II_ILLEGAL_BITMAP_MODE_2:
            /* Force the overscan color to black.  */
            raster_add_int_change_background
                (&vic_ii.raster, VIC_II_RASTER_X(cycle),
                &vic_ii.raster.overscan_background_color, 0);
            raster_add_int_change_background
                (&vic_ii.raster,
                VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.raster.xsmooth_color, 0);
            vic_ii.get_background_from_vbuf = 0;
            vic_ii.force_black_overscan_background_color = 1;
            break;
          case VIC_II_HIRES_BITMAP_MODE:
            raster_add_int_change_background
                (&vic_ii.raster, VIC_II_RASTER_X(cycle),
                &vic_ii.raster.overscan_background_color, 0);
            raster_add_int_change_background
                (&vic_ii.raster,
                VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.raster.xsmooth_color,
                vic_ii.background_color_source & 0x0f);
            vic_ii.get_background_from_vbuf = VIC_II_HIRES_BITMAP_MODE;
            vic_ii.force_black_overscan_background_color = 1;
            break;
          case VIC_II_EXTENDED_TEXT_MODE:
            raster_add_int_change_background
                (&vic_ii.raster, VIC_II_RASTER_X(cycle),
                &vic_ii.raster.overscan_background_color,
                vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
            raster_add_int_change_background
                (&vic_ii.raster,
                VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.raster.xsmooth_color,
                vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
            vic_ii.get_background_from_vbuf = VIC_II_EXTENDED_TEXT_MODE;
            /*vic_ii.force_black_overscan_background_color = 1;*/
            break;
          default:
            /* The overscan background color is given by the background
               color register.  */
            raster_add_int_change_background
                (&vic_ii.raster, VIC_II_RASTER_X(cycle),
                &vic_ii.raster.overscan_background_color,
                vic_ii.regs[0x21]);
            raster_add_int_change_background
                (&vic_ii.raster,
                VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.raster.xsmooth_color,
                vic_ii.regs[0x21]);
            vic_ii.get_background_from_vbuf = 0;
            vic_ii.force_black_overscan_background_color = 0;
            break;
        }

        {
            int pos;

            pos = VIC_II_RASTER_CHAR(cycle);

            /* Multicolor changes are propagated one cycle faster. */
            if (((new_video_mode & 1) ^ (vic_ii.video_mode & 1))
                && cycle > 0)
                pos--;

            raster_add_int_change_background(&vic_ii.raster,
                                             VIC_II_RASTER_X(cycle),
                                             &vic_ii.raster.video_mode,
                                             new_video_mode);
            raster_add_int_change_foreground(&vic_ii.raster, pos,
                                             &vic_ii.raster.video_mode,
                                             new_video_mode);

            if (vic_ii.idle_data_location != IDLE_NONE) {
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

        vic_ii.video_mode = new_video_mode;
    }

#ifdef VIC_II_VMODE_DEBUG
    switch (new_video_mode) {
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

    VIC_II_DEBUG_VMODE(("Mode enabled at line $%04X, cycle %d.",
                        VIC_II_RASTER_Y(maincpu_clk), cycle));
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

    in_visible_area = (vic_ii.raster.current_line
                      >= vic_ii.first_displayed_line
                      && vic_ii.raster.current_line
                      <= vic_ii.last_displayed_line);

    vic_ii.raster.xsmooth_shift_left = 0;

    raster_line_emulate(&vic_ii.raster);

#if 0
    if (vic_ii.raster.current_line >= 60 && vic_ii.raster.current_line <= 60) {
        char buf[1000];
        int j, i;
        for (i = 0; i < 8; i++) {
            memset(buf, 0, sizeof(buf));
            for (j = 0; j < 40; j++)
            sprintf(&buf[strlen(buf)], "%02x",
                    vic_ii.raster.draw_buffer_ptr[vic_ii.raster.xsmooth
                    + vic_ii.raster.geometry->gfx_position.x + i * 40 + j]);
            log_debug(buf);
        } 
    }
#endif

    if (vic_ii.raster.current_line == 0) {
        raster_skip_frame(&vic_ii.raster,
                          vsync_do_vsync(vic_ii.raster.canvas,
                          vic_ii.raster.skip_frame));
        vic_ii.memptr = 0;
        vic_ii.mem_counter = 0;
        vic_ii.light_pen.triggered = 0;
        vic_ii.raster.blank_off = 0;

#ifdef __MSDOS__
        if (vic_ii.raster.canvas->draw_buffer->canvas_width
            <= VIC_II_SCREEN_XPIX
            && vic_ii.raster.canvas->draw_buffer->canvas_height
            <= VIC_II_SCREEN_YPIX
            && vic_ii.raster.canvas->viewport->update_canvas)
            canvas_set_border_color(vic_ii.raster.canvas,
                                    vic_ii.raster.border_color);
#endif
    }

    if (in_visible_area) {
        if (!vic_ii.idle_state)
            vic_ii.mem_counter = (vic_ii.mem_counter
                                 + vic_ii.mem_counter_inc) & 0x3ff;
        vic_ii.mem_counter_inc = VIC_II_SCREEN_TEXTCOLS;
        /* `ycounter' makes the chip go to idle state when it reaches the
           maximum value.  */
        if (vic_ii.raster.ycounter == 7) {
            vic_ii.idle_state = 1;
            vic_ii.memptr = vic_ii.mem_counter;
        }
        if (!vic_ii.idle_state || vic_ii.bad_line) {
            vic_ii.raster.ycounter = (vic_ii.raster.ycounter + 1) & 0x7;
            vic_ii.idle_state = 0;
        }
        if (vic_ii.force_display_state) {
            vic_ii.idle_state = 0;
            vic_ii.force_display_state = 0;
        }
        vic_ii.raster.draw_idle_state = vic_ii.idle_state;
        vic_ii.bad_line = 0;
    }

    vic_ii.ycounter_reset_checked = 0;
    vic_ii.memory_fetch_done = 0;
    vic_ii.buf_offset = 0;

    if (vic_ii.raster.current_line == vic_ii.first_dma_line)
        vic_ii.allow_bad_lines = !vic_ii.raster.blank;

    /* As explained in Christian's article, only the first collision
       (i.e. the first time the collision register becomes non-zero) actually
       triggers an interrupt.  */
    if (vic_ii_resources.sprite_sprite_collisions_enabled
        && vic_ii.raster.sprite_status->sprite_sprite_collisions != 0
        && !prev_sprite_sprite_collisions) {
        vic_ii.irq_status |= 0x4;
        if (vic_ii.regs[0x1a] & 0x4) {
            maincpu_set_irq(I_RASTER, 1);
            vic_ii.irq_status |= 0x80;
        }
    }

    if (vic_ii_resources.sprite_background_collisions_enabled
        && vic_ii.raster.sprite_status->sprite_background_collisions
        && !prev_sprite_background_collisions) {
        vic_ii.irq_status |= 0x2;
        if (vic_ii.regs[0x1a] & 0x2) {
            maincpu_set_irq(I_RASTER, 1);
            vic_ii.irq_status |= 0x80;
        }
    }

    if (vic_ii.idle_state) {
        if (vic_ii.regs[0x11] & 0x40) {
            vic_ii.idle_data_location = IDLE_39FF;
            vic_ii.idle_data = vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x39ff];
        } else {
            vic_ii.idle_data_location = IDLE_3FFF;
            vic_ii.idle_data = vic_ii.ram_base_phi2[vic_ii.vbank_phi2 + 0x3fff];
        }
    } else
        vic_ii.idle_data_location = IDLE_NONE;

    /* Set the next draw event.  */
    vic_ii.last_emulate_line_clk += vic_ii.cycles_per_line;
    vic_ii.draw_clk = vic_ii.last_emulate_line_clk + vic_ii.draw_cycle;
    alarm_set(vic_ii.raster_draw_alarm, vic_ii.draw_clk);
}

/* If necessary, emulate a raster compare IRQ. This is called when the raster
   line counter matches the value stored in the raster line register.  */
void vic_ii_raster_irq_alarm_handler(CLOCK offset)
{
    vic_ii.irq_status |= 0x1;
    if (vic_ii.regs[0x1a] & 0x1) {
        maincpu_set_irq_clk(I_RASTER, 1, vic_ii.raster_irq_clk);
        vic_ii.irq_status |= 0x80;
        VIC_II_DEBUG_RASTER(("*** IRQ requested at line $%04X, "
                            "raster_irq_line = $%04X, off = %ld, cycle = %d.",
                            VIC_II_RASTER_Y(maincpu_clk),
                            vic_ii.raster_irq_line,
                            (long)offset, VIC_II_RASTER_CYCLE(maincpu_clk)));
    }

    vic_ii.raster_irq_clk += vic_ii.screen_height * vic_ii.cycles_per_line;
    alarm_set(vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
}

void vic_ii_set_canvas_refresh(int enable)
{
    raster_set_canvas_refresh(&vic_ii.raster, enable);
}

void vic_ii_free(void)
{
    raster_free(&vic_ii.raster);
}

void vic_ii_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&vic_ii.raster, screenshot);
}

void vic_ii_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&vic_ii.raster, refresh);
}

