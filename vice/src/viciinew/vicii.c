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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64.h"
#include "cartridge.h"
#include "c64cart.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "mon_util.h"
#include "raster-line.h"
#include "raster-modes.h"
#include "raster-sprite.h"
#include "resources.h"
#include "screenshot.h"
#include "types.h"
#include "vicii-chip-model.h"
#include "vicii-cmdline-options.h"
#include "vicii-color.h"
#include "vicii-draw.h"
#include "vicii-draw-cycle.h"
#include "vicii-fetch.h"
#include "vicii-irq.h"
#include "vicii-mem.h"
#include "vicii-resources.h"
#include "vicii-timing.h"
#include "vicii.h"
#include "viciitypes.h"
#include "vsync.h"
#include "video.h"
#include "videoarch.h"
#include "viewport.h"


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

void vicii_change_timing(machine_timing_t *machine_timing, int border_mode)
{
    vicii_timing_set(machine_timing, border_mode);

    if (vicii.initialized) {
        vicii_set_geometry();
        raster_mode_change();
    }
}

void vicii_handle_pending_alarms_external(int num_write_cycles)
{
    return;
}

void vicii_handle_pending_alarms_external_write(void)
{
    return;
}

static void vicii_set_geometry(void)
{
    unsigned int width, height;

    width = vicii.screen_leftborderwidth + VICII_SCREEN_XPIX + vicii.screen_rightborderwidth;
    height = vicii.last_displayed_line - vicii.first_displayed_line + 1;

    raster_set_geometry(&vicii.raster,
                        width, height, /* canvas dimensions */
                        width, vicii.screen_height, /* screen dimensions */
                        width, height, /* gfx dimensions */
                        width/8, height/8, /* text dimensions */
                        0, 0, /* gfx position */
                        0, /* gfx area doesn't move */
                        vicii.first_displayed_line,
                        vicii.last_displayed_line,
                        0, /* extra offscreen border left */
                        0) /* extra offscreen border right */;

#ifdef __MSDOS__
    video_ack_vga_mode();
#endif

    vicii.raster.display_ystart = 0;
    vicii.raster.display_ystop = vicii.screen_height;
    vicii.raster.display_xstart = 0;
    vicii.raster.display_xstop = width;

}

static int init_raster(void)
{
    raster_t *raster;

    raster = &vicii.raster;
    video_color_set_canvas(raster->canvas);

    raster->sprite_status = NULL;
    raster_line_changes_init(raster);

    /* We only use the dummy mode for "drawing" to raster.
       Report only 1 video mode and set the idle mode to it. */
    if (raster_init(raster, 1) < 0) {
        return -1;
    }
    raster_modes_set_idle_mode(raster->modes, VICII_DUMMY_MODE);

    resources_touch("VICIIVideoCache");

    vicii_set_geometry();

    if (vicii_color_update_palette(raster->canvas) < 0) {
        log_error(vicii.log, "Cannot load palette.");
        return -1;
    }

#if 0
    raster_set_title(raster, machine_name);
#else
    /* hack */
    raster_set_title(raster, "C64 (x64sc)");
#endif

    if (raster_realize(raster) < 0) {
        return -1;
    }

    return 0;
}

static void vicii_new_sprites_init(void)
{
    int i;

    for (i = 0; i < VICII_NUM_SPRITES; i++) {
        vicii.sprite[i].data = 0;
        vicii.sprite[i].mc = 0;
        vicii.sprite[i].mcbase = 0;
        vicii.sprite[i].pointer = 0;
        vicii.sprite[i].exp_flop = 1;
        vicii.sprite[i].x = 0;
    }

    vicii.sprite_display_bits = 0;
    vicii.sprite_dma = 0;
}

/* Initialize the VIC-II emulation.  */
raster_t *vicii_init(unsigned int flag)
{
    if (flag != VICII_STANDARD) {
        return NULL;
    }

    vicii.log = log_open("VIC-II");

    vicii_chip_model_init();

    vicii_irq_init();

    if (init_raster() < 0) {
        return NULL;
    }

    vicii_powerup();

    vicii_update_memory_ptrs();

    vicii_draw_init();
    vicii_draw_cycle_init();
    vicii_new_sprites_init();

    vicii.vmli = 0;

    vicii.initialized = 1;

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

    vicii.raster_line = 0;
    vicii.raster_cycle = 6;
    /* this should probably be updated through some function */
    vicii.cycle_flags = 0;
    vicii.start_of_frame = 0;
    vicii.raster_irq_triggered = 0;

    /* FIXME: I am not sure this is exact emulation.  */
    vicii.raster_irq_line = 0;
    vicii.regs[0x11] = 0;
    vicii.regs[0x12] = 0;

    vicii.light_pen.state = 0;
    vicii.light_pen.triggered = 0;
    vicii.light_pen.x = vicii.light_pen.y = vicii.light_pen.x_extra_bits = 0;
    vicii.light_pen.trigger_cycle = CLOCK_MAX;

    /* Remove all the IRQ sources.  */
    vicii.regs[0x1a] = 0;

    vicii.vborder = 1;
    vicii.set_vborder = 1;
    vicii.main_border = 1;
}

void vicii_reset_registers(void)
{
    WORD i;

    if (!vicii.initialized) {
        return;
    }

    for (i = 0; i <= 0x3f; i++) {
        vicii_store(i, 0);
    }
}

/* This /should/ put the VIC-II in the same state as after a powerup, if
   `reset_vicii()' is called afterwards.  But FIXME, as we are not really
   emulating everything correctly here; just $D011.  */
void vicii_powerup(void)
{
    memset(vicii.regs, 0, sizeof(vicii.regs));

    vicii.irq_status = 0;
    vicii.raster_irq_line = 0;
    vicii.ram_base_phi1 = mem_ram;
    vicii.ram_base_phi2 = mem_ram;

    vicii.vaddr_mask_phi1 = 0xffff;
    vicii.vaddr_mask_phi2 = 0xffff;
    vicii.vaddr_offset_phi1 = 0;
    vicii.vaddr_offset_phi2 = 0;

    vicii.allow_bad_lines = 0;
    vicii.sprite_sprite_collisions = vicii.sprite_background_collisions = 0;
    vicii.clear_collisions = 0x00;
    vicii.idle_state = 0;
    vicii.vcbase = 0;
    vicii.vc = 0;
    vicii.bad_line = 0;
    vicii.light_pen.state = 0;
    vicii.light_pen.x = vicii.light_pen.y = vicii.light_pen.x_extra_bits = vicii.light_pen.triggered = 0;
    vicii.light_pen.trigger_cycle = CLOCK_MAX;
    vicii.vbank_phi1 = 0;
    vicii.vbank_phi2 = 0;

    vicii_reset();

    vicii.ysmooth = 0;
}

/* ---------------------------------------------------------------------*/

/* This hook is called whenever video bank must be changed.  */
static inline void vicii_set_vbanks(int vbank_p1, int vbank_p2)
{
    vicii.vbank_phi1 = vbank_p1;
    vicii.vbank_phi2 = vbank_p2;
    vicii_update_memory_ptrs();
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

/* Set light pen input state. Used by c64cia1.c.  */
void vicii_set_light_pen(CLOCK mclk, int state)
{
    if (state) {
        /* add offset depending on chip model (FIXME use proper variable) */
        vicii.light_pen.x_extra_bits = (vicii.color_latency ? 2 : 1);
        vicii_trigger_light_pen_internal(0);
    }
    vicii.light_pen.state = state;
}

/* Trigger the light pen. Used internally.  */
void vicii_trigger_light_pen_internal(int retrigger)
{
    int x;
    unsigned int y;

    /* Unset the trigger cycle (originating from lightpen.c).
       If this function was call from elsewhere before the cycle,
       then the light pen was triggered by other means than
       an actual light pen and the following "if" would make the
       scheduled "actual" triggering pointless. */
    vicii.light_pen.trigger_cycle = CLOCK_MAX;

    if (vicii.light_pen.triggered) {
        return;
    }

    /* this is rather ugly. the +8 is to compensate for us being
       called one cycle too late + and xpos update order problem.
       This leads to several problems.
       The latency needs to be fixed elsewhere. */

    vicii.light_pen.triggered = 1;

    x = vicii.raster_xpos/2 + 8;
    y = vicii.raster_line;

    /* don't trigger on the last line */
    if (y == (vicii.screen_height - 1)) {
        return;
    }

    /* due to the +8, handle y update on line change */
    if (x == 0xc8) {
        y++;
    }

    /* due to the +8, handle x wrapping */
    if (x > 0xf8) {
        x -= 0xfc;
    }

    /* add offset from chip model or an actual light pen */
    x += vicii.light_pen.x_extra_bits;

    /* signaled retrigger from vicii_cycle */
    if (retrigger) {
        x = 0xd1;

        /* On 6569R1 the interrupt is triggered only when the line is low
           on the first cycle of the frame. */
        if (vicii.lightpen_old_irq_mode) {
            vicii_irq_lightpen_set();
        }
    }

    vicii.light_pen.x = x;
    vicii.light_pen.y = y;

    vicii.light_pen.x_extra_bits = 0;

    if (!vicii.lightpen_old_irq_mode) {
        vicii_irq_lightpen_set();
    }
}

/* Calculate lightpen pulse time based on x/y.  */
CLOCK vicii_lightpen_timing(int x, int y)
{
    CLOCK pulse_time = maincpu_clk;

    x += 0x80 - vicii.screen_leftborderwidth;
    y += vicii.first_displayed_line;

    /* Check if x would wrap to previous line */
    if (x < 104) {
        /* lightpen is off screen */
        pulse_time = 0;
    } else {
        pulse_time += (x / 8) + (y * vicii.cycles_per_line);

        /* Store x extra bits for sub CLK precision */
        vicii.light_pen.x_extra_bits = (x >> 1) & 0x3;
    }

    return pulse_time;
}

/* Trigger the light pen. Used by lightpen.c only.  */
void vicii_trigger_light_pen(CLOCK mclk)
{
    /* Record the real trigger time */
    vicii.light_pen.trigger_cycle = mclk;
}


/* ---------------------------------------------------------------------*/

/* Change the base of RAM seen by the VIC-II.  */
static inline void vicii_set_ram_bases(BYTE *base_p1, BYTE *base_p2)
{
    vicii.ram_base_phi1 = base_p1;
    vicii.ram_base_phi2 = base_p2;
    vicii_update_memory_ptrs();
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
    if (vicii.initialized > 0) {
        vicii_update_memory_ptrs();
    }
}

/* Set the memory pointers according to the values in the registers.  */
void vicii_update_memory_ptrs(void)
{
    WORD screen_addr;             /* Screen start address.  */
    BYTE *char_base;              /* Pointer to character memory.  */
    BYTE *bitmap_low_base;        /* Pointer to bitmap memory (low part).  */
    BYTE *bitmap_high_base;       /* Pointer to bitmap memory (high part).  */
    int tmp, bitmap_bank;

    screen_addr = vicii.vbank_phi2 + ((vicii.regs[0x18] & 0xf0) << 6);

    screen_addr = (screen_addr & vicii.vaddr_mask_phi2)
                  | vicii.vaddr_offset_phi2;

    VICII_DEBUG_REGISTER(("Screen memory at $%04X", screen_addr));

    tmp = (vicii.regs[0x18] & 0xe) << 10;
    tmp = (tmp + vicii.vbank_phi1);
    tmp &= vicii.vaddr_mask_phi1;
    tmp |= vicii.vaddr_offset_phi1;

    bitmap_bank = tmp & 0xe000;
    bitmap_low_base = vicii.ram_base_phi1 + bitmap_bank;

    VICII_DEBUG_REGISTER(("Bitmap memory at $%04X", tmp & 0xe000));

    if (cart_ultimax_phi2 != 0) {
        if ((screen_addr & 0x3fff) >= 0x3000)
            vicii.screen_base_phi2 = romh_banks + (romh_bank << 13)
                                     + (screen_addr & 0xfff) + 0x1000;
        else
            vicii.screen_base_phi2 = vicii.ram_base_phi2 + screen_addr;
    } else {
        if ((screen_addr & vicii.vaddr_chargen_mask_phi2)
            != vicii.vaddr_chargen_value_phi2)
            vicii.screen_base_phi2 = vicii.ram_base_phi2 + screen_addr;
        else
            vicii.screen_base_phi2 = mem_chargen_rom_ptr
                                     + (screen_addr & 0xc00);
    }

    if (cart_ultimax_phi1 != 0) {
        if ((screen_addr & 0x3fff) >= 0x3000)
            vicii.screen_base_phi1 = romh_banks + (romh_bank << 13)
                                     + (screen_addr & 0xfff) + 0x1000;
        else
            vicii.screen_base_phi1 = vicii.ram_base_phi1 + screen_addr;

        if ((tmp & 0x3fff) >= 0x3000)
            char_base = romh_banks + (romh_bank << 13) + (tmp & 0xfff) + 0x1000;
        else
            char_base = vicii.ram_base_phi1 + tmp;

        if (((bitmap_bank + 0x1000) & 0x3fff) >= 0x3000)
            bitmap_high_base = romh_banks + (romh_bank << 13) + 0x1000;
        else
            bitmap_high_base = bitmap_low_base + 0x1000;

    } else {
        if ((screen_addr & vicii.vaddr_chargen_mask_phi1)
            != vicii.vaddr_chargen_value_phi1)
            vicii.screen_base_phi1 = vicii.ram_base_phi1 + screen_addr;
        else
            vicii.screen_base_phi1 = mem_chargen_rom_ptr
                                     + (screen_addr & 0xc00);

        if ((tmp & vicii.vaddr_chargen_mask_phi1)
            != vicii.vaddr_chargen_value_phi1)
            char_base = vicii.ram_base_phi1 + tmp;
        else
            char_base = mem_chargen_rom_ptr + (tmp & 0x0800);

        if (((bitmap_bank + 0x1000) & vicii.vaddr_chargen_mask_phi1)
            != vicii.vaddr_chargen_value_phi1)
            bitmap_high_base = bitmap_low_base + 0x1000;
        else
            bitmap_high_base = mem_chargen_rom_ptr;
    }

    /* update pointers */
    vicii.screen_ptr = vicii.screen_base_phi2;
    vicii.bitmap_low_ptr = bitmap_low_base;
    vicii.bitmap_high_ptr = bitmap_high_base;
    vicii.chargen_ptr = char_base;
}

/* Redraw the current raster line.  This happens after the last cycle
   of each line.  */
void vicii_raster_draw_handler(void)
{
    int in_visible_area;

    in_visible_area = (vicii.raster.current_line
                      >= (unsigned int)vicii.first_displayed_line
                      && vicii.raster.current_line
                      <= (unsigned int)vicii.last_displayed_line);

    /* handle wrap if the first few lines are displayed in the visible lower border */
    if ((unsigned int)vicii.last_displayed_line >= vicii.screen_height) {
        in_visible_area |= vicii.raster.current_line
                          <= ((unsigned int)vicii.last_displayed_line - vicii.screen_height);
    }

    raster_line_emulate(&vicii.raster);

    if (vicii.raster.current_line == 0) {
        /* no vsync here for NTSC  */
        if ((unsigned int)vicii.last_displayed_line < vicii.screen_height) {
            raster_skip_frame(&vicii.raster,
                              vsync_do_vsync(vicii.raster.canvas,
                              vicii.raster.skip_frame));
        }

#ifdef __MSDOS__
        if ((unsigned int)vicii.last_displayed_line < vicii.screen_height) {
            if (vicii.raster.canvas->draw_buffer->canvas_width
                <= VICII_SCREEN_XPIX
                && vicii.raster.canvas->draw_buffer->canvas_height
                <= VICII_SCREEN_YPIX
                && vicii.raster.canvas->viewport->update_canvas)
                canvas_set_border_color(vicii.raster.canvas,
                                        vicii.raster.border_color);
        }
#endif
    }

    /* vsync for NTSC */
    if ((unsigned int)vicii.last_displayed_line >= vicii.screen_height
        && vicii.raster.current_line == vicii.last_displayed_line - vicii.screen_height + 1) {
        raster_skip_frame(&vicii.raster,
                          vsync_do_vsync(vicii.raster.canvas,
                          vicii.raster.skip_frame));
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
}

void vicii_set_canvas_refresh(int enable)
{
    raster_set_canvas_refresh(&vicii.raster, enable);
}

void vicii_shutdown(void)
{
    raster_shutdown(&vicii.raster);
}

void vicii_screenshot(screenshot_t *screenshot)
{
    raster_screenshot(&vicii.raster, screenshot);
    screenshot->chipid = "VICII";
    screenshot->video_regs = vicii.regs;
    screenshot->screen_ptr = vicii.screen_base_phi2;
    screenshot->chargen_ptr = vicii.chargen_ptr;
    screenshot->bitmap_ptr = NULL;
    screenshot->bitmap_low_ptr = vicii.bitmap_low_ptr;
    screenshot->bitmap_high_ptr = vicii.bitmap_high_ptr;
    screenshot->color_ram_ptr = mem_color_ram_vicii;
}

void vicii_async_refresh(struct canvas_refresh_s *refresh)
{
    raster_async_refresh(&vicii.raster, refresh);
}

int vicii_dump(struct vicii_s *vic) {
    int m_muco, m_disp, m_ext, v_bank, v_vram;
    int i, bits;

    m_ext = ((vic->regs[0x11] & 0x40) >> 6); /* 0 standard, 1 extended */
    m_muco = ((vic->regs[0x16] & 0x10) >> 4); /* 0 hires, 1 multi */
    m_disp = ((vic->regs[0x11] & 0x20) >> 5); /* 0 text, 1 bitmap */

    v_bank = vic->vbank_phi2;

    mon_out("Rasterline:   current: %d IRQ: %d\n", vic->raster.current_line, vic->raster_irq_line);
    mon_out("Display Mode:");
    mon_out(m_ext ? " Extended" : " Standard");
    mon_out(m_muco ? " Multi Color" : " Hires");
    mon_out(m_disp ? " Bitmap" : " Text");
    mon_out("\nColors:       Border: %2d Background: %2d\n", vic->regs[0x20], vic->regs[0x21]);
    if (m_ext) {
        mon_out("              BGCol1: %2d BGCol2: %2d BGCol3: %2d\n", vic->regs[0x22], vic->regs[0x23], vic->regs[0x24]);
    } else if (m_muco && !m_disp) {
        mon_out("              MuCol1: %2d MuCol2: %2d\n", vic->regs[0x22], vic->regs[0x23]);
    }
    mon_out("Scroll X/Y:   %d/%d\n", vic->regs[0x16] & 0x07, vic->regs[0x11] & 0x07);
    mon_out("Screen Size:  %d x %d\n", 39 + ((vic->regs[0x16] >> 3) & 1), 24 + ((vic->regs[0x11] >> 3) & 1));

    mon_out("\nVIC Memory Bank:   $%04x - $%04x\n", v_bank, v_bank + 0x3fff);
    v_vram = ((vic->regs[0x18] >> 4) * 0x0400) + v_bank;
    mon_out("\nVideo Memory:      $%04x\n", v_vram);
    if (m_disp) {
        mon_out("Bitmap Memory:     $%04x\n", (((vic->regs[0x18] >> 3) & 1) * 0x2000) + v_bank);
    } else {
        i=(((vic->regs[0x18] >> 1) & 0x7) * 0x800) + v_bank;
        /* FIXME: how does cbm510 work ? */
        if (machine_class == VICE_MACHINE_C64   ||
            machine_class == VICE_MACHINE_C128  ||
            machine_class == VICE_MACHINE_C64DTV||
            machine_class == VICE_MACHINE_C64SC ) {
                /* $1x00 and $9x00 mapped to $dx00 */
                if ( (( i >> 12) == 1 ) || (( i >> 12) == 9 ) ) {
                    i = 0xd000 | (i & 0x0f00);
                }
        }
        mon_out("Character Set:     $%04x\n", i);
    }

    mon_out("\nSprites:");
    mon_out("\n           Spr.0  Spr.1  Spr.2  Spr.3  Spr.4  Spr.5  Spr.6  Spr.7");
    mon_out("\nEnabled: ");
    bits = vic->regs[0x15];
    for (i = 0; i < 8; i++) {
        mon_out("  %5s", (bits & 1) ? "yes" : "no");
        bits >>= 1;
    }
/* FIXME: mmh, how to read from the machine ram here ? */
#if 0
    mon_out("\nPointer: ");
    for (i = 0; i < 8; i++) {
    }
#endif
    mon_out("\nX-Pos:   ");
    bits = vic->regs[0x10]; /* sprite x msb */
    for (i = 0; i < 8; i++) {
        mon_out("  %5d", vic->regs[i << 1] + (256 * (bits & 1)));
        bits >>= 1;
    }
    mon_out("\nY-Pos:   ");
    for (i = 0; i < 8; i++) {
        mon_out("  %5d", vic->regs[1 + (i << 1)]);
    }
    mon_out("\nX-Expand:");
    bits = vic->regs[0x1d];
    for (i = 0; i < 8; i++) {
        mon_out("  %5s", (bits & 1) ? "yes" : "no");
        bits >>= 1;
    }
    mon_out("\nY-Expand:");
    bits = vic->regs[0x17];
    for (i = 0; i < 8; i++) {
        mon_out("  %5s", (bits & 1) ? "yes" : "no");
        bits >>= 1;
    }
    mon_out("\nPriority:");
    bits = vic->regs[0x1b];
    for (i = 0; i < 8; i++) {
        mon_out("  %5s", (bits & 1) ? "bg" : "spr");
        bits >>= 1;
    }
    mon_out("\nMode:    ");
    bits = vic->regs[0x1c];
    for (i = 0; i < 8; i++) {
        mon_out("  %5s", (bits & 1) ? "muco" : "std");
        bits >>= 1;
    }
    mon_out("\nColor:   ");
    for (i = 0; i < 8; i++) {
        mon_out("  %5d", vic->regs[i + 0x27]);
    }
    if (vic->regs[0x1c]) {
        mon_out("\nMulti Color 1: %d  Multi Color 2: %d", vic->regs[0x25], vic->regs[0x26]);
    }
    mon_out("\n");

/*
  TODO:

  Current Scanline: 11
  Raster IRQ Scanline: 311
  Enabled Interrupts:  None
  Pending Interrupts:  Raster
*/
    return 0;
}
