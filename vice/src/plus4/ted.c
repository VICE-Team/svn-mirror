/*
 * ted.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include "archdep.h"
#include "clkguard.h"
#include "dma.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "plus4.h"
#include "plus4mem.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "resources.h"
#include "screenshot.h"
#include "ted-cmdline-options.h"
#include "ted-color.h"
#include "ted-draw.h"
#include "ted-fetch.h"
#include "ted-irq.h"
#include "ted-mem.h"
#include "ted-resources.h"
#include "ted-snapshot.h"
#include "ted-sound.h"
#include "ted-timer.h"
#include "ted-timing.h"
#include "ted.h"
#include "tedtypes.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "videoarch.h"
#include "video.h"


ted_t ted;

static void ted_set_geometry(void);
static void ted_exposure_handler(unsigned int width, unsigned int height);

static void clk_overflow_callback(CLOCK sub, void *unused_data)
{
    ted.raster_irq_clk -= sub;
    ted.last_emulate_line_clk -= sub;
    ted.fetch_clk -= sub;
    ted.draw_clk -= sub;
}

void ted_change_timing(machine_timing_t *machine_timing)
{
    ted_timing_set(machine_timing);

    if (ted.initialized) {
        ted_set_geometry();
        raster_mode_change();
    }
}

static CLOCK old_maincpu_clk = 0;

void ted_delay_oldclk(CLOCK num)
{
    old_maincpu_clk += num;
}

inline void ted_delay_clk(void)
{
    CLOCK diff;

    /*log_debug("MCLK %d OMCLK %d", maincpu_clk, old_maincpu_clk);*/

    if (ted.fastmode == 0) {
        diff = maincpu_clk - old_maincpu_clk;
        dma_maincpu_steal_cycles(maincpu_clk, diff, 0);
    }

    old_maincpu_clk = maincpu_clk;

    return;
}

inline void ted_handle_pending_alarms(int num_write_cycles)
{
    ted_delay_clk();

    if (num_write_cycles != 0) {
        int f;

        /* Cycles can be stolen only during the read accesses, so we serve
           only the events that happened during them.  The last read access
           happened at `clk - maincpu_write_cycles()' as all the opcodes
           except BRK and JSR do all the write accesses at the very end.  BRK
           cannot take us here and we would not be able to handle JSR
           correctly anyway, so we don't care about them...  */

        /* Go back to the time when the read accesses happened and serve TED
           events.  */
        maincpu_clk -= num_write_cycles;

        do {
            f = 0;
            if (maincpu_clk > ted.fetch_clk) {
                ted_fetch_alarm_handler(0);
                f = 1;
                ted_delay_clk();
            }
            if (maincpu_clk >= ted.draw_clk) {
                ted_raster_draw_alarm_handler((long)(maincpu_clk
                                              - ted.draw_clk));
                f = 1;
                ted_delay_clk();
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
            if (maincpu_clk >= ted.fetch_clk) {
                ted_fetch_alarm_handler(0);
                f = 1;
                ted_delay_clk();
            }
            if (maincpu_clk >= ted.draw_clk) {
                ted_raster_draw_alarm_handler(0);
                f = 1;
                ted_delay_clk();
            }
        }
        while (f);
    }
}


static void ted_set_geometry(void)
{
    unsigned int width, height;

    width = TED_SCREEN_XPIX + ted.screen_borderwidth * 2;
    height = ted.last_displayed_line - ted.first_displayed_line + 1;

    raster_set_geometry(&ted.raster,
                        width, height,
                        TED_SCREEN_WIDTH, ted.screen_height,
                        TED_SCREEN_XPIX, TED_SCREEN_YPIX,
                        TED_SCREEN_TEXTCOLS, TED_SCREEN_TEXTLINES,
                        ted.screen_borderwidth, ted.row_25_start_line,
                        0,
                        ted.first_displayed_line,
                        ted.last_displayed_line,
                        0, 0);
#ifdef __MSDOS__
    video_ack_vga_mode();
#endif

}

static int init_raster(void)
{
    raster_t *raster;
    char *title;

    raster = &ted.raster;
    video_color_set_raster(raster);

    raster->sprite_status = NULL;
    if (raster_init(raster, TED_NUM_VMODES) < 0)
        return -1;

    raster_modes_set_idle_mode(raster->modes, TED_IDLE_MODE);
    raster_set_exposure_handler(raster, (void*)ted_exposure_handler);
    resources_touch("TEDVideoCache");

    ted_set_geometry();

    if (ted_color_update_palette() < 0) {
        log_error(ted.log, "Cannot load palette.");
        return -1;
    }
    title = util_concat("VICE: ", machine_name, " emulator", NULL);
    raster_set_title(raster, title);
    lib_free(title);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = ted.row_25_start_line;
    raster->display_ystop = ted.row_25_stop_line;
    raster->display_xstart = TED_40COL_START_PIXEL;
    raster->display_xstop = TED_40COL_STOP_PIXEL;

    return 0;
}

/* Initialize the TED emulation.  */
raster_t *ted_init(void)
{
    ted.log = log_open("TED");

    ted_irq_init();

    ted_fetch_init();

    ted.raster_draw_alarm = alarm_new(maincpu_alarm_context,
                                      "TEDRasterDraw",
                                      ted_raster_draw_alarm_handler);

    /* For now.  */
    ted_change_timing(NULL);

    ted_timer_init();

    if (init_raster() < 0)
        return NULL;

    ted_powerup();

    ted_update_video_mode(0);
    ted_update_memory_ptrs(0);

    ted_draw_init();

    ted.initialized = 1;

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL);

    return &ted.raster;
}

struct video_canvas_s *ted_get_canvas(void)
{
    return ted.raster.canvas;
}

/* Reset the TED chip.  */
void ted_reset(void)
{
/*    ted_change_timing();*/

    ted_timer_reset();
    ted_sound_reset();

    raster_reset(&ted.raster);

/*    ted_set_geometry();*/

    ted.last_emulate_line_clk = 0;

    ted.draw_clk = ted.draw_cycle;
    alarm_set(ted.raster_draw_alarm, ted.draw_clk);

    ted.fetch_clk = TED_FETCH_CYCLE;
    alarm_set(ted.raster_fetch_alarm, ted.fetch_clk);

    /* FIXME: I am not sure this is exact emulation.  */
    ted.raster_irq_line = 0;
    ted.raster_irq_clk = 0;

    /* Setup the raster IRQ alarm.  The value is `1' instead of `0' because we
       are at the first line, which has a +1 clock cycle delay in IRQs.  */
    alarm_set(ted.raster_irq_alarm, 1);

    ted.force_display_state = 0;

    ted.reverse_mode = 0;

    /* Remove all the IRQ sources.  */
    ted.regs[0x0a] = 0;

    ted.raster.display_ystart = ted.row_25_start_line;
    ted.raster.display_ystop = ted.row_25_stop_line;

    ted.cursor_visible = 0;
    ted.cursor_phase = 0;

    ted.fastmode = 1;
}

void ted_reset_registers(void)
{
    WORD i;

    if (!ted.initialized)
        return;

    for (i = 0; i <= 0x3f; i++)
        ted_store(i, 0);
}

void ted_powerup(void)
{
    memset(ted.regs, 0, sizeof(ted.regs));

    ted.irq_status = 0;
    ted.raster_irq_line = 0;
    ted.raster_irq_clk = 1;

    ted.allow_bad_lines = 0;
    ted.idle_state = 0;
    ted.force_display_state = 0;
    ted.memory_fetch_done = 0;
    ted.memptr = 0;
    ted.memptr_col = 0;
    ted.mem_counter = 0;
    ted.mem_counter_inc = 0;
    ted.bad_line = 0;
    ted.ycounter_reset_checked = 0;
    ted.force_black_overscan_background_color = 0;
    ted.idle_data = 0;
    ted.idle_data_location = IDLE_NONE;
    ted.last_emulate_line_clk = 0;

    ted_reset();

    ted.raster_irq_line = 0;

    ted.raster.blank = 1;
    ted.raster.display_ystart = ted.row_24_start_line;
    ted.raster.display_ystop = ted.row_24_stop_line;

    ted.raster.ysmooth = 0;
}

/* ---------------------------------------------------------------------*/

/* Handle the exposure event.  */
static void ted_exposure_handler(unsigned int width, unsigned int height)
{
    ted.raster.canvas->draw_buffer->canvas_width = width;
    ted.raster.canvas->draw_buffer->canvas_height = height;
    video_viewport_resize(ted.raster.canvas);
}

#if 0
void ted_set_raster_irq(unsigned int line)
{
    if (line == ted.raster_irq_line && ted.raster_irq_clk != CLOCK_MAX)
        return;

    if (line < (unsigned int)ted.screen_height) {
        unsigned int current_line = TED_RASTER_Y(maincpu_clk);

        ted.raster_irq_clk = (TED_LINE_START_CLK(maincpu_clk)
                             + TED_RASTER_IRQ_DELAY - INTERRUPT_DELAY
                             + (ted.cycles_per_line
                             * (line - current_line)));

        /* Raster interrupts on line 0 are delayed by 1 cycle.  */
        if (line == 0)
            ted.raster_irq_clk++;

        if (line <= current_line)
            ted.raster_irq_clk += (ted.screen_height
                                  * ted.cycles_per_line);
        alarm_set(ted.raster_irq_alarm, ted.raster_irq_clk);
    } else {
        TED_DEBUG_RASTER(("TED: update_raster_irq(): "
                         "raster compare out of range ($%04X)!", line));
        ted.raster_irq_clk = CLOCK_MAX;
        alarm_unset(ted.raster_irq_alarm);
    }

    TED_DEBUG_RASTER(("TED: update_raster_irq(): "
                     "ted.raster_irq_clk = %ul, "
                     "line = $%04X, "
                     "ted.regs[0x0a] & 2 = %d\n",
                     ted.raster_irq_clk, line, ted.regs[0x0a] & 2));

    ted.raster_irq_line = line;
}
#endif

/* Set the memory pointers according to the values in the registers.  */
void ted_update_memory_ptrs(unsigned int cycle)
{
    /* FIXME: This is *horrible*!  */
    static BYTE *old_screen_ptr, *old_bitmap_ptr, *old_chargen_ptr;
    static BYTE *old_color_ptr;
    WORD screen_addr, char_addr, bitmap_addr, color_addr;
    BYTE *screen_base;            /* Pointer to screen memory.  */
    BYTE *char_base;              /* Pointer to character memory.  */
    BYTE *bitmap_base;            /* Pointer to bitmap memory.  */
    BYTE *color_base;             /* Pointer to color memory.  */
    int tmp;
    unsigned int romsel;

    romsel = ted.regs[0x12] & 4;

    screen_addr = ((ted.regs[0x14] & 0xf8) << 8) | 0x400;
    screen_base = mem_get_tedmem_base((screen_addr >> 14) | romsel)
                  + (screen_addr & 0x3fff);

    TED_DEBUG_REGISTER(("\tVideo memory at $%04X", screen_addr));

    bitmap_addr = (ted.regs[0x12] & 0x38) << 10;
    bitmap_base = mem_get_tedmem_base((bitmap_addr >> 14) | romsel)
                  + (bitmap_addr & 0x3fff);

    TED_DEBUG_REGISTER(("\tBitmap memory at $%04X", bitmap_addr));

    char_addr = (ted.regs[0x13] & 0xfc) << 8;
    char_base = mem_get_tedmem_base((char_addr >> 14) | romsel)
                + (char_addr & 0x3fff);

    TED_DEBUG_REGISTER(("\tUser-defined character set at $%04X", char_addr));

    color_addr = ((ted.regs[0x14] & 0xf8) << 8);
    color_base = mem_get_tedmem_base((color_addr >> 14) | romsel)
                 + (color_addr & 0x3fff);

    TED_DEBUG_REGISTER(("\tColor memory at $%04X", color_addr));


    tmp = TED_RASTER_CHAR(cycle);

    if (ted.idle_data_location != IDLE_NONE) {
        if (ted.idle_data_location == IDLE_39FF)
            raster_add_int_change_foreground(&ted.raster,
                                             TED_RASTER_CHAR(cycle),
                                             &ted.idle_data,
                                             mem_ram[0x39ff]);
        else
            raster_add_int_change_foreground(&ted.raster,
                                             TED_RASTER_CHAR(cycle),
                                             &ted.idle_data,
                                             mem_ram[0x3fff]);
    }

    if (ted.raster.skip_frame || (tmp <= 0 && maincpu_clk < ted.draw_clk)) {
        old_screen_ptr = ted.screen_ptr = screen_base;
        old_bitmap_ptr = ted.bitmap_ptr = bitmap_base;
        old_chargen_ptr = ted.chargen_ptr = char_base;
        old_color_ptr = ted.color_ptr = color_base;
    } else if (tmp < TED_SCREEN_TEXTCOLS) {
        if (screen_base != old_screen_ptr) {
            raster_add_ptr_change_foreground(&ted.raster, tmp,
                                             (void **)&ted.screen_ptr,
                                             (void *)screen_base);
            old_screen_ptr = screen_base;
        }

        if (bitmap_base != old_bitmap_ptr) {
            raster_add_ptr_change_foreground(&ted.raster,
                                             tmp,
                                             (void **)&ted.bitmap_ptr,
                                             (void *)(bitmap_base));
            old_bitmap_ptr = bitmap_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_foreground(&ted.raster,
                                             tmp,
                                             (void **)&ted.chargen_ptr,
                                             (void *)char_base);
            old_chargen_ptr = char_base;
        }
        if (color_base != old_color_ptr) {
            raster_add_ptr_change_foreground(&ted.raster, tmp,
                                             (void **)&ted.color_ptr,
                                             (void *)color_base);
            old_color_ptr = color_base;
        }
    } else {
        if (screen_base != old_screen_ptr) {
            raster_add_ptr_change_next_line(&ted.raster,
                                            (void **)&ted.screen_ptr,
                                            (void *)screen_base);
            old_screen_ptr = screen_base;
        }
        if (bitmap_base != old_bitmap_ptr) {
            raster_add_ptr_change_next_line(&ted.raster,
                                            (void **)&ted.bitmap_ptr,
                                            (void *)(bitmap_base));
            old_bitmap_ptr = bitmap_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_next_line(&ted.raster,
                                            (void **)&ted.chargen_ptr,
                                            (void *)char_base);
            old_chargen_ptr = char_base;
        }
        if (color_base != old_color_ptr) {
            raster_add_ptr_change_next_line(&ted.raster,
                                            (void **)&ted.color_ptr,
                                            (void *)color_base);
            old_color_ptr = color_base;
        }
    }
}

/* Set the video mode according to the values in registers 6 and 7 of TED */
void ted_update_video_mode(unsigned int cycle)
{
   static int old_video_mode = -1;
    int new_video_mode;

    new_video_mode = ((ted.regs[0x06] & 0x60)
                     | (ted.regs[0x07] & 0x10)) >> 4;

    if (new_video_mode != old_video_mode) {
        if (TED_IS_ILLEGAL_MODE(new_video_mode)) {
            /* Force the overscan color to black.  */
            raster_add_int_change_background
                (&ted.raster, TED_RASTER_X(cycle),
                &ted.raster.overscan_background_color,
                0);
            raster_add_int_change_background
                (&ted.raster, TED_RASTER_X(cycle),
                &ted.raster.xsmooth_color,
                0);
            ted.force_black_overscan_background_color = 1;
        } else {
            /* The overscan background color is given by the background color
               register.  */
            if (ted.raster.overscan_background_color != ted.regs[0x15]) {
                raster_add_int_change_background
                    (&ted.raster, TED_RASTER_X(cycle),
                    &ted.raster.overscan_background_color,
                    ted.regs[0x15]);
                raster_add_int_change_background
                    (&ted.raster, TED_RASTER_X(cycle),
                    &ted.raster.xsmooth_color,
                    ted.regs[0x15]);
            }
            ted.force_black_overscan_background_color = 0;
        }

        {
            int pos;

            pos = TED_RASTER_CHAR(cycle);

            raster_add_int_change_foreground(&ted.raster, pos,
                                             &ted.raster.video_mode,
                                             new_video_mode);

            if (ted.idle_data_location != IDLE_NONE) {
                if (ted.regs[0x06] & 0x40)
                    raster_add_int_change_foreground
                        (&ted.raster, pos, (void *)&ted.idle_data,
                        mem_ram[0x39ff]);
                else
                    raster_add_int_change_foreground
                        (&ted.raster, pos, (void *)&ted.idle_data,
                        mem_ram[0x3fff]);
            }
        }

        old_video_mode = new_video_mode;
    }

#ifdef TED_VMODE_DEBUG
    switch (new_video_mode) {
      case TED_NORMAL_TEXT_MODE:
        TED_DEBUG_VMODE(("Standard Text"));
        break;
      case TED_MULTICOLOR_TEXT_MODE:
        TED_DEBUG_VMODE(("Multicolor Text"));
        break;
      case TED_HIRES_BITMAP_MODE:
        TED_DEBUG_VMODE(("Hires Bitmap"));
        break;
      case TED_MULTICOLOR_BITMAP_MODE:
        TED_DEBUG_VMODE(("Multicolor Bitmap"));
        break;
      case TED_EXTENDED_TEXT_MODE:
        TED_DEBUG_VMODE(("Extended Text"));
        break;
      case TED_ILLEGAL_TEXT_MODE:
        TED_DEBUG_VMODE(("Illegal Text"));
        break;
      case TED_ILLEGAL_BITMAP_MODE_1:
        TED_DEBUG_VMODE(("Invalid Bitmap"));
        break;
      case TED_ILLEGAL_BITMAP_MODE_2:
        TED_DEBUG_VMODE(("Invalid Bitmap"));
        break;
      default:                    /* cannot happen */
        TED_DEBUG_VMODE(("???"));
    }

    TED_DEBUG_VMODE((" Mode enabled at line $%04X, cycle %d.",
                    TED_RASTER_Y(clk), cycle));
#endif
}

/* Redraw the current raster line.  This happens at cycle TED_DRAW_CYCLE
   of each line.  */
void ted_raster_draw_alarm_handler(CLOCK offset)
{
    int in_visible_area;


    in_visible_area = (ted.raster.current_line
                      >= (unsigned int)ted.first_displayed_line
                      && ted.raster.current_line
                      <= (unsigned int)ted.last_displayed_line);

    raster_line_emulate(&ted.raster);

    if (ted.raster.current_line == 0) {
        raster_skip_frame(&ted.raster,
                          vsync_do_vsync(ted.raster.canvas,
                                         ted.raster.skip_frame));
        ted.memptr = 0;
        ted.memptr_col = 0;
        ted.mem_counter = 0;

        ted.cursor_phase = (ted.cursor_phase + 1) & 0x1f;
        ted.cursor_visible = ted.cursor_phase & 0x10;

#ifdef __MSDOS__
        if (ted.raster.canvas->draw_buffer->canvas_width
            <= TED_SCREEN_XPIX
            && ted.raster.canvas->draw_buffer->canvas_height
            <= TED_SCREEN_YPIX)
            canvas_set_border_color(ted.raster.canvas,
                                    ted.raster.border_color);
#endif
    }

    if (in_visible_area) {
        if (!ted.idle_state)
            ted.mem_counter = (ted.mem_counter
                              + ted.mem_counter_inc) & 0x3ff;
        ted.mem_counter_inc = TED_SCREEN_TEXTCOLS;
        /* `ycounter' makes the chip go to idle state when it reaches the
           maximum value.  */
        if (ted.raster.ycounter == 6) {
            ted.memptr_col = ted.mem_counter;
        }
        if (ted.raster.ycounter == 7) {
            ted.memptr = ted.mem_counter;
            ted.idle_state = 1;
        }
        if (!ted.idle_state || ted.bad_line) {
            ted.raster.ycounter = (ted.raster.ycounter + 1) & 0x7;
            ted.idle_state = 0;
        }
        if (ted.force_display_state) {
            ted.idle_state = 0;
            ted.force_display_state = 0;
        }
        ted.raster.draw_idle_state = ted.idle_state;
        ted.bad_line = 0;
    }

    ted.ycounter_reset_checked = 0;
    ted.memory_fetch_done = 0;

    if (ted.raster.current_line == ted.first_dma_line)
        ted.allow_bad_lines = !ted.raster.blank;

    if (ted.idle_state) {
        if (ted.regs[0x6] & 0x40) {
            ted.idle_data_location = IDLE_39FF;
            ted.idle_data = mem_ram[0x39ff];
        } else {
            ted.idle_data_location = IDLE_3FFF;
            ted.idle_data = mem_ram[0x3fff];
        }
    } else {
        ted.idle_data_location = IDLE_NONE;
    }

    /* Set the next draw event.  */
    ted.last_emulate_line_clk += ted.cycles_per_line;
    ted.draw_clk = ted.last_emulate_line_clk + ted.draw_cycle;
    alarm_set(ted.raster_draw_alarm, ted.draw_clk);
}

void ted_shutdown(void)
{
    raster_shutdown(&ted.raster);
}

void ted_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&ted.raster, screenshot);
}

void ted_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&ted.raster, refresh);
}

