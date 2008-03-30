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
#include "dma.h"
#include "lib.h"
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
#include "types.h"
#include "utils.h"
#include "vicii-cmdline-options.h"
#include "vicii-color.h"
#include "vicii-draw.h"
#include "vicii-fetch.h"
#include "vicii-irq.h"
#include "vicii-mem.h"
#include "vicii-sprites.h"
#include "vicii-resources.h"
#include "vicii-timing.h"
#include "vicii.h"
#include "viciitypes.h"
#include "vsync.h"
#include "videoarch.h"
#include "video.h"


void vicii_set_phi1_addr_options(WORD mask, WORD offset)
{
    vicii.vaddr_mask_phi1 = mask;
    vicii.vaddr_offset_phi1 = offset;

    VICII_DEBUG_REGISTER(("Set phi1 video addr mask=%04x, offset=%04x",
                         mask, offset));
    vicii_update_memory_ptrs_external();
}

void vicii_set_phi2_addr_options(WORD mask, WORD offset)
{
    vicii.vaddr_mask_phi2 = mask;
    vicii.vaddr_offset_phi2 = offset;

    VICII_DEBUG_REGISTER(("Set phi2 video addr mask=%04x, offset=%04x",
                         mask, offset));
    vicii_update_memory_ptrs_external();
}

void vicii_set_phi1_chargen_addr_options(WORD mask, WORD value)
{
    vicii.vaddr_chargen_mask_phi1 = mask;
    vicii.vaddr_chargen_value_phi1 = value;

    VICII_DEBUG_REGISTER(("Set phi1 chargen addr mask=%04x, value=%04x",
                         mask, value));
    vicii_update_memory_ptrs_external();
}

void vicii_set_phi2_chargen_addr_options(WORD mask, WORD value)
{
    vicii.vaddr_chargen_mask_phi2 = mask;
    vicii.vaddr_chargen_value_phi2 = value;

    VICII_DEBUG_REGISTER(("Set phi2 chargen addr mask=%04x, value=%04x",
                         mask, value));
    vicii_update_memory_ptrs_external();
}

void vicii_set_chargen_addr_options(WORD mask, WORD value)
{
    vicii.vaddr_chargen_mask_phi1 = mask;
    vicii.vaddr_chargen_value_phi1 = value;
    vicii.vaddr_chargen_mask_phi2 = mask;
    vicii.vaddr_chargen_value_phi2 = value;

    VICII_DEBUG_REGISTER(("Set chargen addr mask=%04x, value=%04x",
                         mask, value));
    vicii_update_memory_ptrs_external();
}

/* ---------------------------------------------------------------------*/

vicii_t vicii;

static void vicii_set_geometry(void);

static void clk_overflow_callback(CLOCK sub, void *unused_data)
{
    vicii.raster_irq_clk -= sub;
    vicii.last_emulate_line_clk -= sub;
    vicii.fetch_clk -= sub;
    vicii.draw_clk -= sub;
}

void vicii_change_timing(machine_timing_t *machine_timing)
{
    vicii_timing_set(machine_timing);

    if (vicii.initialized) {
        vicii_set_geometry();
        raster_mode_change();
    }
}

static CLOCK old_maincpu_clk = 0;

void vicii_delay_oldclk(CLOCK num)
{
    old_maincpu_clk += num;
}

inline void vicii_delay_clk(void)
{
#if 0
    CLOCK diff;

    /*log_debug("MCLK %d OMCLK %d", maincpu_clk, old_maincpu_clk);*/

    if (vicii.fastmode == 0) {
        diff = maincpu_clk - old_maincpu_clk;
        dma_maincpu_steal_cycles(maincpu_clk, diff, 0);
    }

    old_maincpu_clk = maincpu_clk;

    return;
#endif
}

inline void vicii_handle_pending_alarms(int num_write_cycles)
{
    if (vicii.viciie != 0)
        vicii_delay_clk();

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
            if (maincpu_clk > vicii.fetch_clk) {
                vicii_fetch_alarm_handler(0);
                f = 1;
                if (vicii.viciie != 0)
                    vicii_delay_clk();
            }
            if (maincpu_clk >= vicii.draw_clk) {
                vicii_raster_draw_alarm_handler((long)(maincpu_clk
                                                - vicii.draw_clk));
                f = 1;
                if (vicii.viciie != 0)
                    vicii_delay_clk();
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
            if (maincpu_clk >= vicii.fetch_clk) {
                vicii_fetch_alarm_handler(0);
                f = 1;
                if (vicii.viciie != 0)
                    vicii_delay_clk();
            }
            if (maincpu_clk >= vicii.draw_clk) {
                vicii_raster_draw_alarm_handler(0);
                f = 1;
                if (vicii.viciie != 0)
                    vicii_delay_clk();
            }
        }
        while (f);
    }
}

void vicii_handle_pending_alarms_external(int num_write_cycles)
{
    if (vicii.initialized)
        vicii_handle_pending_alarms(num_write_cycles);
}

static void vicii_set_geometry(void)
{
    unsigned int width, height;

    width = VICII_SCREEN_XPIX + vicii.screen_borderwidth * 2;
    height = vicii.last_displayed_line - vicii.first_displayed_line + 1;

    raster_set_geometry(&vicii.raster,
                        width, height,
                        VICII_SCREEN_XPIX + vicii.screen_borderwidth * 2,
                        vicii.screen_height,
                        VICII_SCREEN_XPIX, VICII_SCREEN_YPIX,
                        VICII_SCREEN_TEXTCOLS, VICII_SCREEN_TEXTLINES,
                        vicii.screen_borderwidth, vicii.row_25_start_line,
                        0,
                        vicii.first_displayed_line,
                        vicii.last_displayed_line,
                        - VICII_RASTER_X(0),
                        vicii.sprite_wrap_x - VICII_SCREEN_XPIX -
                        vicii.screen_borderwidth * 2 + VICII_RASTER_X(0));
#ifdef __MSDOS__
    video_ack_vga_mode();
#endif

}

static int init_raster(void)
{
    raster_t *raster;
    char *title;

    raster = &vicii.raster;
    video_color_set_canvas(raster->canvas);

    raster_sprite_status_new(raster, VICII_NUM_SPRITES);
    raster_line_changes_sprite_init(raster);

    if (raster_init(raster, VICII_NUM_VMODES) < 0)
        return -1;
    raster_modes_set_idle_mode(raster->modes, VICII_IDLE_MODE);
    resources_touch("VICIIVideoCache");

    vicii_set_geometry();

    if (vicii_color_update_palette() < 0) {
        log_error(vicii.log, "Cannot load palette.");
        return -1;
    }
    title = util_concat("VICE: ", machine_name, " emulator", NULL);
    raster_set_title(raster, title);
    lib_free(title);

    if (raster_realize(raster) < 0)
        return -1;

    raster->display_ystart = vicii.row_25_start_line;
    raster->display_ystop = vicii.row_25_stop_line;
    raster->display_xstart = VICII_40COL_START_PIXEL;
    raster->display_xstop = VICII_40COL_STOP_PIXEL;

    return 0;
}

/* Initialize the VIC-II emulation.  */
raster_t *vicii_init(unsigned int flag)
{
    vicii.viciie = flag;

    if (vicii.viciie == VICII_EXTENDED)
        vicii.log = log_open("VIC-IIe");
    else
        vicii.log = log_open("VIC-II");

    vicii_irq_init();

    vicii_fetch_init();

    vicii.raster_draw_alarm = alarm_new(maincpu_alarm_context,
                                        "VicIIRasterDraw",
                                        vicii_raster_draw_alarm_handler);
    if (init_raster() < 0)
        return NULL;

    vicii_powerup();

    vicii.video_mode = -1;
    vicii_update_video_mode(0);
    vicii_update_memory_ptrs(0);

    vicii_draw_init();
    vicii_sprites_init();

    vicii.num_idle_3fff = 0;
    vicii.num_idle_3fff_old = 0;
    vicii.idle_3fff = (idle_3fff_t *)lib_malloc(sizeof(idle_3fff_t) * 64);
    vicii.idle_3fff_old = (idle_3fff_t *)lib_malloc(sizeof(idle_3fff_t) * 64);

    vicii.buf_offset = 0;

    vicii.initialized = 1;

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL);

    return &vicii.raster;
}

struct video_canvas_s *vicii_get_canvas(void)
{
    return vicii.raster.canvas;
}

/* Reset the VIC-II chip.  */
void vicii_reset(void)
{
    raster_reset(&vicii.raster);

    vicii.last_emulate_line_clk = 0;

    vicii.draw_clk = vicii.draw_cycle;
    alarm_set(vicii.raster_draw_alarm, vicii.draw_clk);

    vicii.fetch_clk = VICII_FETCH_CYCLE;
    alarm_set(vicii.raster_fetch_alarm, vicii.fetch_clk);
    vicii.fetch_idx = VICII_FETCH_MATRIX;
    vicii.sprite_fetch_idx = 0;
    vicii.sprite_fetch_msk = 0;
    vicii.sprite_fetch_clk = CLOCK_MAX;

    /* FIXME: I am not sure this is exact emulation.  */
    vicii.raster_irq_line = 0;
    vicii.raster_irq_clk = 0;
    vicii.regs[0x11] = 0;
    vicii.regs[0x12] = 0;

    /* Setup the raster IRQ alarm.  The value is `1' instead of `0' because we
       are at the first line, which has a +1 clock cycle delay in IRQs.  */
    alarm_set(vicii.raster_irq_alarm, 1);

    vicii.force_display_state = 0;

    vicii.light_pen.triggered = 0;
    vicii.light_pen.x = vicii.light_pen.y = 0;

    /* Remove all the IRQ sources.  */
    vicii.regs[0x1a] = 0;

    vicii.raster.display_ystart = vicii.row_25_start_line;
    vicii.raster.display_ystop = vicii.row_25_stop_line;

    vicii.store_clk = CLOCK_MAX;
}

void vicii_reset_registers(void)
{
    WORD i;

    if (!vicii.initialized)
        return;

    for (i = 0; i <= 0x3f; i++)
        vicii_store(i, 0);

    raster_sprite_status_reset(vicii.raster.sprite_status);
}

/* This /should/ put the VIC-II in the same state as after a powerup, if
   `reset_vicii()' is called afterwards.  But FIXME, as we are not really
   emulating everything correctly here; just $D011.  */
void vicii_powerup(void)
{
    memset(vicii.regs, 0, sizeof(vicii.regs));

    vicii.irq_status = 0;
    vicii.raster_irq_line = 0;
    vicii.raster_irq_clk = 1;
    vicii.ram_base_phi1 = mem_ram;
    vicii.ram_base_phi2 = mem_ram;

    vicii.vaddr_mask_phi1 = 0xffff;
    vicii.vaddr_mask_phi2 = 0xffff;
    vicii.vaddr_offset_phi1 = 0;
    vicii.vaddr_offset_phi2 = 0;

    vicii.allow_bad_lines = 0;
    vicii.sprite_sprite_collisions = vicii.sprite_background_collisions = 0;
    vicii.fetch_idx = VICII_FETCH_MATRIX;
    vicii.idle_state = 0;
    vicii.force_display_state = 0;
    vicii.memory_fetch_done = 0;
    vicii.memptr = 0;
    vicii.mem_counter = 0;
    vicii.mem_counter_inc = 0;
    vicii.bad_line = 0;
    vicii.ycounter_reset_checked = 0;
    vicii.force_black_overscan_background_color = 0;
    vicii.light_pen.x = vicii.light_pen.y = vicii.light_pen.triggered = 0;
    vicii.vbank_phi1 = 0;
    vicii.vbank_phi2 = 0;
    /* vicii.vbank_ptr = ram; */
    vicii.idle_data = 0;
    vicii.idle_data_location = IDLE_NONE;
    vicii.last_emulate_line_clk = 0;

    vicii_reset();

    vicii.raster.blank = 1;
    vicii.raster.display_ystart = vicii.row_24_start_line;
    vicii.raster.display_ystop = vicii.row_24_stop_line;

    vicii.raster.ysmooth = 0;
}

/* ---------------------------------------------------------------------*/

/* This hook is called whenever video bank must be changed.  */
static inline void vicii_set_vbanks(int vbank_p1, int vbank_p2)
{
    /* Warning: assumes it's called within a memory write access.
       FIXME: Change name?  */
    /* Also, we assume the bank has *really* changed, and do not do any
       special optimizations for the not-really-changed case.  */
    vicii_handle_pending_alarms(maincpu_rmw_flag + 1);
    if (maincpu_clk >= vicii.draw_clk)
        vicii_raster_draw_alarm_handler(maincpu_clk - vicii.draw_clk);

    vicii.vbank_phi1 = vbank_p1;
    vicii.vbank_phi2 = vbank_p2;
    vicii_update_memory_ptrs(VICII_RASTER_CYCLE(maincpu_clk));
}

/* Phi1 and Phi2 accesses */
void vicii_set_vbank(int num_vbank)
{
    int tmp = num_vbank << 14;
    vicii_set_vbanks(tmp, tmp);
}

/* Phi1 accesses */
void vicii_set_phi1_vbank(int num_vbank)
{
    vicii_set_vbanks(num_vbank << 14, vicii.vbank_phi2);
}

/* Phi2 accesses */
void vicii_set_phi2_vbank(int num_vbank)
{
    vicii_set_vbanks(vicii.vbank_phi1, num_vbank << 14);
}

/* ---------------------------------------------------------------------*/

/* Trigger the light pen.  */
void vicii_trigger_light_pen(CLOCK mclk)
{
    if (!vicii.light_pen.triggered) {
        vicii.light_pen.triggered = 1;
        vicii.light_pen.x = VICII_RASTER_X(mclk % vicii.cycles_per_line);

        if (vicii.light_pen.x < 0)
            vicii.light_pen.x = vicii.sprite_wrap_x + vicii.light_pen.x;

        /* FIXME: why `+2'? */
        vicii.light_pen.x = vicii.light_pen.x / 2 + 2;
        vicii.light_pen.y = VICII_RASTER_Y(mclk);

        vicii_irq_lightpen_set(mclk);
    }
}

/* Change the base of RAM seen by the VIC-II.  */
static inline void vicii_set_ram_bases(BYTE *base_p1, BYTE *base_p2)
{
    /* WARNING: assumes `maincpu_rmw_flag' is 0 or 1.  */
    vicii_handle_pending_alarms(maincpu_rmw_flag + 1);

    vicii.ram_base_phi1 = base_p1;
    vicii.ram_base_phi2 = base_p2;
    vicii_update_memory_ptrs(VICII_RASTER_CYCLE(maincpu_clk));
}

void vicii_set_ram_base(BYTE *base)
{
    vicii_set_ram_bases(base, base);
}

void vicii_set_phi1_ram_base(BYTE *base)
{
    vicii_set_ram_bases(base, vicii.ram_base_phi2);
}

void vicii_set_phi2_ram_base(BYTE *base)
{
    vicii_set_ram_bases(vicii.ram_base_phi1, base);
}


void vicii_update_memory_ptrs_external(void)
{
    if (vicii.initialized > 0)
        vicii_update_memory_ptrs(VICII_RASTER_CYCLE(maincpu_clk));
}

/* Set the memory pointers according to the values in the registers.  */
void vicii_update_memory_ptrs(unsigned int cycle)
{
    /* FIXME: This is *horrible*!  */
    static BYTE *old_screen_ptr, *old_bitmap_low_ptr, *old_bitmap_high_ptr;
    static BYTE *old_chargen_ptr;
    static int old_vbank_p1 = -1;
    static int old_vbank_p2 = -1;
    WORD screen_addr;             /* Screen start address.  */
    /*BYTE *screen_base;*/        /* Pointer to screen memory.  */
    BYTE *char_base;              /* Pointer to character memory.  */
    BYTE *bitmap_low_base;        /* Pointer to bitmap memory (low part).  */
    BYTE *bitmap_high_base;       /* Pointer to bitmap memory (high part).  */
    int tmp, bitmap_bank;

    screen_addr = vicii.vbank_phi2 + ((vicii.regs[0x18] & 0xf0) << 6);

    screen_addr = (screen_addr & vicii.vaddr_mask_phi2)
                  | vicii.vaddr_offset_phi2;

    if ((screen_addr & vicii.vaddr_chargen_mask_phi2)
        != vicii.vaddr_chargen_value_phi2) {
        vicii.screen_base = vicii.ram_base_phi2 + screen_addr;
        VICII_DEBUG_REGISTER(("Video memory at $%04X", screen_addr));
    } else {
        vicii.screen_base = mem_chargen_rom_ptr + (screen_addr & 0x800);
        VICII_DEBUG_REGISTER(("Video memory at Character ROM + $%04X",
                              screen_addr & 0x800));
    }

    tmp = (vicii.regs[0x18] & 0xe) << 10;
    tmp = (tmp + vicii.vbank_phi1);
    tmp &= vicii.vaddr_mask_phi1;
    tmp |= vicii.vaddr_offset_phi1;

    bitmap_bank = tmp & 0xe000;
    bitmap_low_base = vicii.ram_base_phi1 + bitmap_bank;

    VICII_DEBUG_REGISTER(("Bitmap memory at $%04X", tmp & 0xe000));

    if (cart_ultimax_phi1 != 0) {
        if ((tmp & 0x3fff) >= 0x3000)
            char_base = romh_banks + (romh_bank << 13) + (tmp & 0xfff) + 0x1000;
        else
            char_base = vicii.ram_base_phi1 + tmp;

        if (((bitmap_bank + 0x1000) & 0x3fff) >= 0x3000)
            bitmap_high_base = romh_banks + (romh_bank << 13) + 0x1000;
        else
            bitmap_high_base = vicii.ram_base_phi1 + bitmap_bank + 0x1000;

    } else {
        if ((tmp & vicii.vaddr_chargen_mask_phi1)
            != vicii.vaddr_chargen_value_phi1)
            char_base = vicii.ram_base_phi1 + tmp;
        else
            char_base = mem_chargen_rom_ptr + (tmp & 0x0800);

        if (((bitmap_bank + 0x1000) & vicii.vaddr_chargen_mask_phi1)
            != vicii.vaddr_chargen_value_phi1)
            bitmap_high_base = vicii.ram_base_phi1 + bitmap_bank + 0x1000;
        else
            bitmap_high_base = mem_chargen_rom_ptr;
    }

    tmp = VICII_RASTER_CHAR(cycle);

    if (vicii.idle_data_location != IDLE_NONE &&
        old_vbank_p2 != vicii.vbank_phi2) {
        if (vicii.idle_data_location == IDLE_39FF)
            raster_add_int_change_foreground(&vicii.raster,
                                             VICII_RASTER_CHAR(cycle),
                                             &vicii.idle_data,
                                             vicii.ram_base_phi2[vicii.vbank_phi2
                                             + 0x39ff]);
        else
            raster_add_int_change_foreground(&vicii.raster,
                                             VICII_RASTER_CHAR(cycle),
                                             &vicii.idle_data,
                                             vicii.ram_base_phi2[vicii.vbank_phi2
                                             + 0x3fff]);
    }

    if (tmp <= 0 && maincpu_clk < vicii.draw_clk) {
        old_screen_ptr = vicii.screen_ptr = vicii.screen_base;
        old_bitmap_low_ptr = vicii.bitmap_low_ptr = bitmap_low_base;
        old_bitmap_high_ptr = vicii.bitmap_high_ptr = bitmap_high_base;
        old_chargen_ptr = vicii.chargen_ptr = char_base;
        old_vbank_p1 = vicii.vbank_phi1;
        old_vbank_p2 = vicii.vbank_phi2;
        /* vicii.vbank_ptr = vicii.ram_base + vicii.vbank; */
        vicii.raster.sprite_status->ptr_base = vicii.screen_base + 0x3f8;
    } else if (tmp < VICII_SCREEN_TEXTCOLS) {
        if (vicii.screen_base != old_screen_ptr) {
            raster_add_ptr_change_foreground(&vicii.raster, tmp,
                                             (void **)&vicii.screen_ptr,
                                             (void *)vicii.screen_base);
            raster_add_ptr_change_foreground(&vicii.raster, tmp,
                              (void **)&vicii.raster.sprite_status->ptr_base,
                                             (void *)(vicii.screen_base
                                             + 0x3f8));
            old_screen_ptr = vicii.screen_base;
        }

        if (bitmap_low_base != old_bitmap_low_ptr) {
            raster_add_ptr_change_foreground(&vicii.raster,
                                             tmp,
                                             (void **)&vicii.bitmap_low_ptr,
                                             (void *)(bitmap_low_base));
            old_bitmap_low_ptr = bitmap_low_base;
        }

        if (bitmap_high_base != old_bitmap_high_ptr) {
            raster_add_ptr_change_foreground(&vicii.raster,
                                             tmp,
                                             (void **)&vicii.bitmap_high_ptr,
                                             (void *)(bitmap_high_base));
            old_bitmap_high_ptr = bitmap_high_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_foreground(&vicii.raster,
                                             tmp,
                                             (void **)&vicii.chargen_ptr,
                                             (void *)char_base);
            old_chargen_ptr = char_base;
        }

        if (vicii.vbank_phi1 != old_vbank_p1) {
/*
            raster_add_ptr_change_foreground(&vicii.raster,
                                             tmp,
                                             (void **)&vicii.vbank_ptr,
                                             (void *)(vicii.ram_base
                                             + vicii.vbank));
*/
            old_vbank_p1 = vicii.vbank_phi1;
        }

        if (vicii.vbank_phi2 != old_vbank_p2) {
            old_vbank_p2 = vicii.vbank_phi2;
        }
    } else {
        if (vicii.screen_base != old_screen_ptr) {
            raster_add_ptr_change_next_line(&vicii.raster,
                                            (void **)&vicii.screen_ptr,
                                            (void *)vicii.screen_base);
            raster_add_ptr_change_next_line(&vicii.raster,
                              (void **)&vicii.raster.sprite_status->ptr_base,
                                            (void *)(vicii.screen_base
                                            + 0x3f8));
            old_screen_ptr = vicii.screen_base;
        }

        if (bitmap_low_base != old_bitmap_low_ptr) {
            raster_add_ptr_change_next_line(&vicii.raster,
                                            (void **)&vicii.bitmap_low_ptr,
                                            (void *)(bitmap_low_base));
            old_bitmap_low_ptr = bitmap_low_base;
        }

        if (bitmap_high_base != old_bitmap_high_ptr) {
            raster_add_ptr_change_next_line(&vicii.raster,
                                            (void **)&vicii.bitmap_high_ptr,
                                            (void *)(bitmap_high_base));
            old_bitmap_high_ptr = bitmap_high_base;
        }

        if (char_base != old_chargen_ptr) {
            raster_add_ptr_change_next_line(&vicii.raster,
                                            (void **)&vicii.chargen_ptr,
                                            (void *)char_base);
            old_chargen_ptr = char_base;
        }

        if (vicii.vbank_phi1 != old_vbank_p1) {
/*
            raster_add_ptr_change_next_line(&vicii.raster,
                                            (void **)&vicii.vbank_ptr,
                                            (void *)(vicii.ram_base
                                            + vicii.vbank));
*/
            old_vbank_p1 = vicii.vbank_phi1;
        }

        if (vicii.vbank_phi2 != old_vbank_p2) {
            old_vbank_p2 = vicii.vbank_phi2;
        }
    }
}

/* Set the video mode according to the values in registers $D011 and $D016 of
   the VIC-II chip.  */
void vicii_update_video_mode(unsigned int cycle)
{
    int new_video_mode;

    new_video_mode = ((vicii.regs[0x11] & 0x60)
                     | (vicii.regs[0x16] & 0x10)) >> 4;

    if (new_video_mode != vicii.video_mode) {
        switch (new_video_mode) {
          case VICII_ILLEGAL_TEXT_MODE:
          case VICII_ILLEGAL_BITMAP_MODE_1:
          case VICII_ILLEGAL_BITMAP_MODE_2:
            /* Force the overscan color to black.  */
            raster_add_int_change_background
                (&vicii.raster, VICII_RASTER_X(cycle),
                &vicii.raster.idle_background_color, 0);
            raster_add_int_change_background
                (&vicii.raster,
                VICII_RASTER_X(VICII_RASTER_CYCLE(maincpu_clk)),
                &vicii.raster.xsmooth_color, 0);
            vicii.get_background_from_vbuf = 0;
            vicii.force_black_overscan_background_color = 1;
            break;
          case VICII_HIRES_BITMAP_MODE:
            raster_add_int_change_background
                (&vicii.raster, VICII_RASTER_X(cycle),
                &vicii.raster.idle_background_color, 0);
            raster_add_int_change_background
                (&vicii.raster,
                VICII_RASTER_X(VICII_RASTER_CYCLE(maincpu_clk)),
                &vicii.raster.xsmooth_color,
                vicii.background_color_source & 0x0f);
            vicii.get_background_from_vbuf = VICII_HIRES_BITMAP_MODE;
            vicii.force_black_overscan_background_color = 1;
            break;
          case VICII_EXTENDED_TEXT_MODE:
            raster_add_int_change_background
                (&vicii.raster, VICII_RASTER_X(cycle),
                &vicii.raster.idle_background_color,
                vicii.regs[0x21]);
            raster_add_int_change_background
                (&vicii.raster,
                VICII_RASTER_X(VICII_RASTER_CYCLE(maincpu_clk)),
                &vicii.raster.xsmooth_color,
                vicii.regs[0x21 + (vicii.background_color_source >> 6)]);
            vicii.get_background_from_vbuf = VICII_EXTENDED_TEXT_MODE;
            vicii.force_black_overscan_background_color = 0;
            break;
          default:
            /* The overscan background color is given by the background
               color register.  */
            raster_add_int_change_background
                (&vicii.raster, VICII_RASTER_X(cycle),
                &vicii.raster.idle_background_color,
                vicii.regs[0x21]);
            raster_add_int_change_background
                (&vicii.raster,
                VICII_RASTER_X(VICII_RASTER_CYCLE(maincpu_clk)),
                &vicii.raster.xsmooth_color,
                vicii.regs[0x21]);
            vicii.get_background_from_vbuf = 0;
            vicii.force_black_overscan_background_color = 0;
            break;
        }

        {
            int pos;

            pos = VICII_RASTER_CHAR(cycle);

            /* Multicolor changes are propagated one cycle faster. */
            if (((new_video_mode & 1) ^ (vicii.video_mode & 1))
                && cycle > 0)
                pos--;

            raster_add_int_change_background(&vicii.raster,
                                             VICII_RASTER_X(cycle),
                                             &vicii.raster.video_mode,
                                             new_video_mode);
            raster_add_int_change_foreground(&vicii.raster, pos,
                                             &vicii.raster.video_mode,
                                             new_video_mode);

            if (vicii.idle_data_location != IDLE_NONE) {
                if (vicii.regs[0x11] & 0x40)
                    raster_add_int_change_foreground
                    (&vicii.raster, pos, (void *)&vicii.idle_data,
                    vicii.ram_base_phi2[vicii.vbank_phi2 + 0x39ff]);
                else
                    raster_add_int_change_foreground
                    (&vicii.raster, pos, (void *)&vicii.idle_data,
                    vicii.ram_base_phi2[vicii.vbank_phi2 + 0x3fff]);
            }
        }

        vicii.video_mode = new_video_mode;
    }

#ifdef VICII_VMODE_DEBUG
    switch (new_video_mode) {
      case VICII_NORMAL_TEXT_MODE:
        VICII_DEBUG_VMODE(("Standard Text"));
        break;
      case VICII_MULTICOLOR_TEXT_MODE:
        VICII_DEBUG_VMODE(("Multicolor Text"));
        break;
      case VICII_HIRES_BITMAP_MODE:
        VICII_DEBUG_VMODE(("Hires Bitmap"));
        break;
      case VICII_MULTICOLOR_BITMAP_MODE:
        VICII_DEBUG_VMODE(("Multicolor Bitmap"));
        break;
      case VICII_EXTENDED_TEXT_MODE:
        VICII_DEBUG_VMODE(("Extended Text"));
        break;
      case VICII_ILLEGAL_TEXT_MODE:
        VICII_DEBUG_VMODE(("Illegal Text"));
        break;
      case VICII_ILLEGAL_BITMAP_MODE_1:
        VICII_DEBUG_VMODE(("Invalid Bitmap"));
        break;
      case VICII_ILLEGAL_BITMAP_MODE_2:
        VICII_DEBUG_VMODE(("Invalid Bitmap"));
        break;
      default:                    /* cannot happen */
        VICII_DEBUG_VMODE(("???"));
    }

    VICII_DEBUG_VMODE(("Mode enabled at line $%04X, cycle %d.",
                       VICII_RASTER_Y(maincpu_clk), cycle));
#endif
}

/* Redraw the current raster line.  This happens at cycle VICII_DRAW_CYCLE
   of each line.  */
void vicii_raster_draw_alarm_handler(CLOCK offset)
{
    BYTE prev_sprite_sprite_collisions;
    BYTE prev_sprite_background_collisions;
    int in_visible_area;

    prev_sprite_sprite_collisions = vicii.sprite_sprite_collisions;
    prev_sprite_background_collisions = vicii.sprite_background_collisions;

    in_visible_area = (vicii.raster.current_line
                      >= (unsigned int)vicii.first_displayed_line
                      && vicii.raster.current_line
                      <= (unsigned int)vicii.last_displayed_line);

    vicii.raster.xsmooth_shift_left = 0;

    vicii_sprites_reset_xshift();

    raster_line_emulate(&vicii.raster);

#if 0
    if (vicii.raster.current_line >= 60 && vicii.raster.current_line <= 60) {
        char buf[1000];
        int j, i;
        for (i = 0; i < 8; i++) {
            memset(buf, 0, sizeof(buf));
            for (j = 0; j < 40; j++)
            sprintf(&buf[strlen(buf)], "%02x",
                    vicii.raster.draw_buffer_ptr[vicii.raster.xsmooth
                    + vicii.raster.geometry->gfx_position.x + i * 40 + j]);
            log_debug(buf);
        } 
    }
#endif

    if (vicii.raster.current_line == 0) {
        raster_skip_frame(&vicii.raster,
                          vsync_do_vsync(vicii.raster.canvas,
                          vicii.raster.skip_frame));
        vicii.memptr = 0;
        vicii.mem_counter = 0;
        vicii.light_pen.triggered = 0;
        vicii.raster.blank_off = 0;

#ifdef __MSDOS__
        if (vicii.raster.canvas->draw_buffer->canvas_width
            <= VICII_SCREEN_XPIX
            && vicii.raster.canvas->draw_buffer->canvas_height
            <= VICII_SCREEN_YPIX
            && vicii.raster.canvas->viewport->update_canvas)
            canvas_set_border_color(vicii.raster.canvas,
                                    vicii.raster.border_color);
#endif
    }

    if (in_visible_area) {
        if (!vicii.idle_state)
            vicii.mem_counter = (vicii.mem_counter
                                + vicii.mem_counter_inc) & 0x3ff;
        vicii.mem_counter_inc = VICII_SCREEN_TEXTCOLS;
        /* `ycounter' makes the chip go to idle state when it reaches the
           maximum value.  */
        if (vicii.raster.ycounter == 7) {
            vicii.idle_state = 1;
            vicii.memptr = vicii.mem_counter;
        }
        if (!vicii.idle_state || vicii.bad_line) {
            vicii.raster.ycounter = (vicii.raster.ycounter + 1) & 0x7;
            vicii.idle_state = 0;
        }
        if (vicii.force_display_state) {
            vicii.idle_state = 0;
            vicii.force_display_state = 0;
        }
        vicii.raster.draw_idle_state = vicii.idle_state;
        vicii.bad_line = 0;
    }

    vicii.ycounter_reset_checked = 0;
    vicii.memory_fetch_done = 0;
    vicii.buf_offset = 0;

    if (vicii.raster.current_line == vicii.first_dma_line)
        vicii.allow_bad_lines = !vicii.raster.blank;

    /* As explained in Christian's article, only the first collision
       (i.e. the first time the collision register becomes non-zero) actually
       triggers an interrupt.  */
    if (vicii_resources.sprite_sprite_collisions_enabled
        && vicii.raster.sprite_status->sprite_sprite_collisions != 0
        && !prev_sprite_sprite_collisions) {
        vicii_irq_sscoll_set();
    }

    if (vicii_resources.sprite_background_collisions_enabled
        && vicii.raster.sprite_status->sprite_background_collisions
        && !prev_sprite_background_collisions) {
        vicii_irq_sbcoll_set();
    }

    if (vicii.idle_state) {
        if (vicii.regs[0x11] & 0x40) {
            vicii.idle_data_location = IDLE_39FF;
            vicii.idle_data = vicii.ram_base_phi2[vicii.vbank_phi2 + 0x39ff];
        } else {
            vicii.idle_data_location = IDLE_3FFF;
            vicii.idle_data = vicii.ram_base_phi2[vicii.vbank_phi2 + 0x3fff];
        }
    } else
        vicii.idle_data_location = IDLE_NONE;

    /* Set the next draw event.  */
    vicii.last_emulate_line_clk += vicii.cycles_per_line;
    vicii.draw_clk = vicii.last_emulate_line_clk + vicii.draw_cycle;
    alarm_set(vicii.raster_draw_alarm, vicii.draw_clk);
}

void vicii_set_canvas_refresh(int enable)
{
    raster_set_canvas_refresh(&vicii.raster, enable);
}

void vicii_shutdown(void)
{
    lib_free(vicii.idle_3fff);
    lib_free(vicii.idle_3fff_old);
    vicii_sprites_shutdown();
    raster_sprite_status_destroy(&vicii.raster);
    raster_shutdown(&vicii.raster);
}

void vicii_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&vicii.raster, screenshot);
}

void vicii_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&vicii.raster, refresh);
}

