/*
 * vicii-mem.c - Memory interface for the MOS6569 (VIC-II) emulation.
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

#include "alarm.h"
#include "c64cia.h"
#include "interrupt.h"
#include "maincpu.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "types.h"
#include "vicii-badline.h"
#include "vicii-fetch.h"
#include "vicii-irq.h"
#include "vicii-resources.h"
#include "vicii-sprites.h"
#include "vicii-mem.h"
#include "vicii.h"
#include "viciitypes.h"


/* Unused bits in VIC-II registers: these are always 1 when read.  */
static int unused_bits_in_registers[64] =
{
    0x00 /* $D000 */ , 0x00 /* $D001 */ , 0x00 /* $D002 */ , 0x00 /* $D003 */ ,
    0x00 /* $D004 */ , 0x00 /* $D005 */ , 0x00 /* $D006 */ , 0x00 /* $D007 */ ,
    0x00 /* $D008 */ , 0x00 /* $D009 */ , 0x00 /* $D00A */ , 0x00 /* $D00B */ ,
    0x00 /* $D00C */ , 0x00 /* $D00D */ , 0x00 /* $D00E */ , 0x00 /* $D00F */ ,
    0x00 /* $D010 */ , 0x00 /* $D011 */ , 0x00 /* $D012 */ , 0x00 /* $D013 */ ,
    0x00 /* $D014 */ , 0x00 /* $D015 */ , 0x00 /* $D016 */ , 0xc0 /* $D017 */ ,
    0x01 /* $D018 */ , 0x70 /* $D019 */ , 0xf0 /* $D01A */ , 0x00 /* $D01B */ ,
    0x00 /* $D01C */ , 0x00 /* $D01D */ , 0x00 /* $D01E */ , 0x00 /* $D01F */ ,
    0xf0 /* $D020 */ , 0xf0 /* $D021 */ , 0xf0 /* $D022 */ , 0xf0 /* $D023 */ ,
    0xf0 /* $D024 */ , 0xf0 /* $D025 */ , 0xf0 /* $D026 */ , 0xf0 /* $D027 */ ,
    0xf0 /* $D028 */ , 0xf0 /* $D029 */ , 0xf0 /* $D02A */ , 0xf0 /* $D02B */ ,
    0xf0 /* $D02C */ , 0xf0 /* $D02D */ , 0xf0 /* $D02E */ , 0xff /* $D02F */ ,
    0xff /* $D030 */ , 0xff /* $D031 */ , 0xff /* $D032 */ , 0xff /* $D033 */ ,
    0xff /* $D034 */ , 0xff /* $D035 */ , 0xff /* $D036 */ , 0xff /* $D037 */ ,
    0xff /* $D038 */ , 0xff /* $D039 */ , 0xff /* $D03A */ , 0xff /* $D03B */ ,
    0xff /* $D03C */ , 0xff /* $D03D */ , 0xff /* $D03E */ , 0xff /* $D03F */
};


/* Store a value in the video bank (it is assumed to be in RAM).  */
inline void REGPARM2 vicii_local_store_vbank(ADDRESS addr, BYTE value)
{
    unsigned int f;

    if (vic_ii.viciie != 0)
        vicii_delay_clk();

    do {
        CLOCK mclk;

        /* WARNING: Assumes `maincpu_rmw_flag' is 0 or 1.  */
        mclk = maincpu_clk - maincpu_rmw_flag - 1;
        f = 0;

        if (mclk >= vic_ii.fetch_clk) {
            /* If the fetch starts here, the sprite fetch routine should
               get the new value, not the old one.  */
            if (mclk == vic_ii.fetch_clk)
                vic_ii.ram_base_phi2[addr] = value;

            /* The sprite DMA check can be followed by a real fetch.
               Save the location to execute the store if a real
               fetch actually happens.  */
            if (vic_ii.fetch_idx == VIC_II_CHECK_SPRITE_DMA) {
                vic_ii.store_clk = mclk;
                vic_ii.store_value = value;
                vic_ii.store_addr = addr;
            }

            vicii_fetch_alarm_handler(maincpu_clk - vic_ii.fetch_clk);
            f = 1;
            /* WARNING: Assumes `maincpu_rmw_flag' is 0 or 1.  */
            mclk = maincpu_clk - maincpu_rmw_flag - 1;
            vic_ii.store_clk = CLOCK_MAX;
        }

        if (mclk >= vic_ii.draw_clk) {
            vicii_raster_draw_alarm_handler(0);
            f = 1;
        }
        if (vic_ii.viciie != 0)
            vicii_delay_clk();
    } while (f);

    vic_ii.ram_base_phi2[addr] = value;
}

/* Encapsulate inlined function for other modules */
void REGPARM2 vicii_mem_vbank_store(ADDRESS addr, BYTE value)
{
    vicii_local_store_vbank(addr, value);
}

/* As `store_vbank()', but for the $3900...$39FF address range.  */
void REGPARM2 vicii_mem_vbank_39xx_store(ADDRESS addr, BYTE value)
{
    vicii_local_store_vbank(addr, value);

    if (vic_ii.idle_data_location == IDLE_39FF && (addr & 0x3fff) == 0x39ff)
        raster_add_int_change_foreground
            (&vic_ii.raster,
            VIC_II_RASTER_CHAR(VIC_II_RASTER_CYCLE(maincpu_clk)),
            &vic_ii.idle_data,
            value);
}

/* As `store_vbank()', but for the $3F00...$3FFF address range.  */
void REGPARM2 vicii_mem_vbank_3fxx_store(ADDRESS addr, BYTE value)
{
    vicii_local_store_vbank(addr, value);

    if ((addr & 0x3fff) == 0x3fff) {
        if (vic_ii.idle_data_location == IDLE_3FFF)
            raster_add_int_change_foreground
                (&vic_ii.raster,
                VIC_II_RASTER_CHAR(VIC_II_RASTER_CYCLE(maincpu_clk)),
                &vic_ii.idle_data,
                value);

        if (vic_ii.raster.sprite_status->visible_msk != 0
            || vic_ii.raster.sprite_status->dma_msk != 0) {
            vic_ii.idle_3fff[vic_ii.num_idle_3fff].cycle = maincpu_clk;
            vic_ii.idle_3fff[vic_ii.num_idle_3fff].value = value;
            vic_ii.num_idle_3fff++;
        }
    }
}


inline static void store_sprite_x_position_lsb(ADDRESS addr, BYTE value)
{
    int n;
    int new_x;

    if (value == vic_ii.regs[addr])
        return;

    vic_ii.regs[addr] = value;

    n = addr >> 1;                /* Number of changed sprite.  */

    VIC_II_DEBUG_REGISTER(("Sprite #%d X position LSB: $%02X", n, value));

    new_x = (value | (vic_ii.regs[0x10] & (1 << n) ? 0x100 : 0));
    vicii_sprites_set_x_position(n, new_x,
        VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)));
}

inline static void store_sprite_y_position(ADDRESS addr, BYTE value)
{
    int cycle;

    VIC_II_DEBUG_REGISTER(("Sprite #%d Y position: $%02X", addr >> 1, value));

    if (vic_ii.regs[addr] == value)
        return;

    cycle = VIC_II_RASTER_CYCLE(maincpu_clk);

    if (cycle == vic_ii.sprite_fetch_cycle + 1
        && value == (vic_ii.raster.current_line & 0xff)) {
        vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
        vic_ii.fetch_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                            + vic_ii.sprite_fetch_cycle + 1);
        alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
    }

    vic_ii.raster.sprite_status->sprites[addr >> 1].y = value;
    vic_ii.regs[addr] = value;
}

static inline void store_sprite_x_position_msb(ADDRESS addr, BYTE value)
{
    int i;
    BYTE b;
    int raster_x;

    VIC_II_DEBUG_REGISTER(("Sprite X position MSBs: $%02X", value));

    if (value == vic_ii.regs[addr])
        return;

    raster_x = VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk));

    vic_ii.regs[addr] = value;

    /* Recalculate the sprite X coordinates.  */
    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
        int new_x;

        new_x = (vic_ii.regs[2 * i] | (value & b ? 0x100 : 0));
        vicii_sprites_set_x_position(i, new_x, raster_x);
    }
}

inline static void check_lower_upper_border(BYTE value, int line, int cycle)
{
    if ((value ^ vic_ii.regs[0x11]) & 8) {
        if (value & 0x8) {
            /* 24 -> 25 row mode switch.  */

            vic_ii.raster.display_ystart = vic_ii.row_25_start_line;
            vic_ii.raster.display_ystop = vic_ii.row_25_stop_line;

            if (line == vic_ii.row_24_stop_line && cycle > 0) {
                /* If on the first line of the 24-line border, we
                   still see the 25-line (lowmost) border because the
                   border flip flop has already been turned on.  */
                vic_ii.raster.blank_enabled = 1;
            } else {
                if (!vic_ii.raster.blank && line == vic_ii.row_24_start_line
                    && cycle > 0) {
                    /* A 24 -> 25 switch somewhere on the first line of
                       the 24-row mode is enough to disable screen
                       blanking.  */
                    vic_ii.raster.blank_enabled = 0;
                }
            }
            VIC_II_DEBUG_REGISTER(("25 line mode enabled"));
        } else {
            /* 25 -> 24 row mode switch.  */

            vic_ii.raster.display_ystart = vic_ii.row_24_start_line;
            vic_ii.raster.display_ystop = vic_ii.row_24_stop_line;

            /* If on the last line of the 25-line border, we still see the
               24-line (upmost) border because the border flip flop has
               already been turned off.  */
            if (!vic_ii.raster.blank && line == vic_ii.row_25_start_line
                && cycle > 0) {
                vic_ii.raster.blank_enabled = 0;
            } else {
                if (line == vic_ii.row_25_stop_line && cycle > 0)
                    vic_ii.raster.blank_enabled = 1;
            }

            VIC_II_DEBUG_REGISTER(("24 line mode enabled"));
        }
    }
}

/* Here we try to emulate $D011...  */
inline static void store_d011(BYTE value)
{
    int new_irq_line, cycle, line, old_allow_bad_lines;

    cycle = VIC_II_RASTER_CYCLE(maincpu_clk);
    line = VIC_II_RASTER_Y(maincpu_clk);

    VIC_II_DEBUG_REGISTER(("Control register: $%02X", value));
    VIC_II_DEBUG_REGISTER(("$D011 tricks at cycle %d, line $%04X, "
                          "value $%02X", cycle, line, value));

    new_irq_line = ((vic_ii.raster_irq_line & 0xff) | ((value & 0x80) << 1));
    vicii_irq_check_state(new_irq_line);

    /* This is the funniest part... handle bad line tricks.  */
    old_allow_bad_lines = vic_ii.allow_bad_lines;

    if (line == vic_ii.first_dma_line && (value & 0x10) != 0)
        vic_ii.allow_bad_lines = 1;

    if (vic_ii.raster.ysmooth != (value & 7)
        && line >= vic_ii.first_dma_line
        && line <= vic_ii.last_dma_line)
        vicii_badline_check_state(value, cycle, line, old_allow_bad_lines);

    vic_ii.raster.ysmooth = value & 0x7;

    /* Check for 24 <-> 25 line mode switch.  */
    check_lower_upper_border(value, line, cycle);

    vic_ii.raster.blank = !(value & 0x10); /* `DEN' bit.  */

    vic_ii.regs[0x11] = value;

    /* FIXME: save time.  */
    vicii_update_video_mode(cycle);
}

inline static void store_d012(BYTE value)
{
    /* FIXME: Not accurate as bit #8 is missing.  */
    value = (value - vic_ii.offset) & 255;

    VIC_II_DEBUG_REGISTER(("Raster compare register: $%02X", value));

    if (value == vic_ii.regs[0x12])
        return;

    vic_ii.regs[0x12] = value;

    VIC_II_DEBUG_REGISTER(("Raster interrupt line set to $%04X",
                          vic_ii.raster_irq_line));

    vicii_irq_check_state((vic_ii.raster_irq_line & 0x100) | value);
}

inline static void store_d015(BYTE value)
{
    int cycle;

    VIC_II_DEBUG_REGISTER(("Sprite Enable register: $%02X", value));

    cycle = VIC_II_RASTER_CYCLE(maincpu_clk);

    /* On the real C64, sprite DMA is checked two times: first at
       `VIC_II_SPRITE_FETCH_CYCLE', and then at `VIC_II_SPRITE_FETCH_CYCLE +
       1'.  In the average case, one DMA check is OK and there is no need to
       emulate both, but we have to kludge things a bit in case sprites are
       activated at cycle `VIC_II_SPRITE_FETCH_CYCLE + 1'.  */
    if (cycle == vic_ii.sprite_fetch_cycle + 1
        && ((value ^ vic_ii.regs[0x15]) & value) != 0) {
        vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
        vic_ii.fetch_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                            + vic_ii.sprite_fetch_cycle + 1);
        alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
    }

    /* Sprites are turned on: force a DMA check.  */
    if (vic_ii.raster.sprite_status->visible_msk == 0
        && vic_ii.raster.sprite_status->dma_msk == 0
        && value != 0) {
        if ((vic_ii.fetch_idx == VIC_II_FETCH_MATRIX
             && vic_ii.fetch_clk > maincpu_clk
             && cycle > VIC_II_FETCH_CYCLE
             && cycle <= vic_ii.sprite_fetch_cycle)
            || vic_ii.raster.current_line < vic_ii.first_dma_line
            || vic_ii.raster.current_line > vic_ii.last_dma_line) {
            CLOCK new_fetch_clk;

            new_fetch_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                             + vic_ii.sprite_fetch_cycle);
            if (cycle > vic_ii.sprite_fetch_cycle)
                new_fetch_clk += vic_ii.cycles_per_line;
            if (new_fetch_clk < vic_ii.fetch_clk) {
                vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
                vic_ii.fetch_clk = new_fetch_clk;
                alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
            }
        }
    }

    vic_ii.regs[0x15] = vic_ii.raster.sprite_status->visible_msk = value;
}

inline static void check_lateral_border(BYTE value, int cycle,
                                        raster_t *raster)
{
    if ((value & 0x8) != (vic_ii.regs[0x16] & 0x8)) {
        if (value & 0x8) {
            /* 40 column mode.  */
            if (cycle <= 17)
                raster->display_xstart = VIC_II_40COL_START_PIXEL;
            else
                raster_add_int_change_next_line(raster,
                                                &raster->display_xstart,
                                                VIC_II_40COL_START_PIXEL);
            if (cycle <= 56)
                raster->display_xstop = VIC_II_40COL_STOP_PIXEL;
            else
                raster_add_int_change_next_line(raster,
                                                &raster->display_xstop,
                                                VIC_II_40COL_STOP_PIXEL);
            VIC_II_DEBUG_REGISTER(("40 column mode enabled"));

            /* If CSEL changes from 0 to 1 at cycle 17, the border is
               not turned off and this line is blank.  */
            if (cycle == 17 && !(vic_ii.regs[0x16] & 0x8))
                raster->blank_this_line = 1;
        } else {
            /* 38 column mode.  */
            if (cycle <= 17)
                raster->display_xstart = VIC_II_38COL_START_PIXEL;
            else
                raster_add_int_change_next_line(raster,
                                                &raster->display_xstart,
                                                VIC_II_38COL_START_PIXEL);
            if (cycle <= 56)
                raster->display_xstop = VIC_II_38COL_STOP_PIXEL;
            else
                raster_add_int_change_next_line(raster,
                                                &raster->display_xstop,
                                                VIC_II_38COL_STOP_PIXEL);
            VIC_II_DEBUG_REGISTER(("38 column mode enabled"));

            /* If CSEL changes from 1 to 0 at cycle 56, the lateral
               border is open.  */
            if (cycle == 56 && (vic_ii.regs[0x16] & 0x8)
                && (!raster->blank_enabled || raster->open_left_border)) {
                raster->open_right_border = 1;
                switch (vic_ii.get_background_from_vbuf) {
                  case VIC_II_HIRES_BITMAP_MODE:
                    raster_add_int_change_background(
                    &vic_ii.raster,
                    VIC_II_RASTER_X(56),
                    &vic_ii.raster.xsmooth_color,
                    vic_ii.background_color_source & 0x0f);
                    break;
                  case VIC_II_EXTENDED_TEXT_MODE:
                    raster_add_int_change_background(
                    &vic_ii.raster,
                    VIC_II_RASTER_X(56),
                    &vic_ii.raster.xsmooth_color,
                    vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
                    break;
                }
            }
        }
    }
}

inline static void store_d016(BYTE value)
{
    raster_t *raster;
    int cycle;
    BYTE xsmooth;

    VIC_II_DEBUG_REGISTER(("Control register: $%02X", value));

    raster = &vic_ii.raster;
    cycle = VIC_II_RASTER_CYCLE(maincpu_clk);
    xsmooth = value & 7;

    if (xsmooth != (vic_ii.regs[0x16] & 7)) {
        if (xsmooth < (vic_ii.regs[0x16] & 7)) {
            if (cycle < 56)
                raster_add_int_change_foreground(raster,
                             VIC_II_RASTER_CHAR(cycle) - 2,
                             &raster->xsmooth_shift_left,
                             (vic_ii.regs[0x16] & 7) - xsmooth);

        } else {
            raster_add_int_change_background(raster,
                                             VIC_II_RASTER_X(cycle),
                                             &raster->xsmooth_shift_right,
                                             xsmooth - (vic_ii.regs[0x16] & 7));
        }
        raster_add_int_change_foreground(raster,
                                         VIC_II_RASTER_CHAR(cycle) - 1,
                                         &raster->xsmooth,
                                         xsmooth);
    }

    /* Bit 4 (CSEL) selects 38/40 column mode.  */
    check_lateral_border(value, cycle, raster);

    vic_ii.regs[0x16] = value;

    vicii_update_video_mode(cycle);
}

inline static void store_d017(BYTE value)
{
    raster_sprite_status_t *sprite_status;
    int cycle;
    int i;
    BYTE b;

    VIC_II_DEBUG_REGISTER(("Sprite Y Expand register: $%02X", value));

    if (value == vic_ii.regs[0x17])
        return;

    cycle = VIC_II_RASTER_CYCLE(maincpu_clk);
    sprite_status = vic_ii.raster.sprite_status;

    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
        raster_sprite_t *sprite;

        sprite = sprite_status->sprites + i;

        sprite->y_expanded = value & b ? 1 : 0;

        if (!sprite->y_expanded && !sprite->exp_flag) {
            /* Sprite crunch!  */
            if (cycle == 15)
                sprite->memptr_inc
                    = vicii_sprites_crunch_table[sprite->memptr];
            else if (cycle < 15)
                sprite->memptr_inc = 3;
            sprite->exp_flag = 1;
        }

        /* (Enabling sprite Y-expansion never causes side effects.)  */
    }

    vic_ii.regs[0x17] = value;
}

inline static void store_d018(BYTE value)
{
    VIC_II_DEBUG_REGISTER(("Memory register: $%02X", value));

    if (vic_ii.regs[0x18] == value)
        return;

    vic_ii.regs[0x18] = value;
    vicii_update_memory_ptrs(VIC_II_RASTER_CYCLE(maincpu_clk));
}

inline static void store_d019(BYTE value)
{
    /* Emulates Read-Modify-Write behaviour. */
    if (maincpu_rmw_flag) {
        vic_ii.irq_status &= ~((vic_ii.last_read_d019 & 0xf) | 0x80);
        if (maincpu_clk - 1 > vic_ii.raster_irq_clk
            && vic_ii.raster_irq_line < vic_ii.screen_height) {
            vic_ii.raster_irq_clk += vic_ii.screen_height
                                     * vic_ii.cycles_per_line;
            alarm_set(vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
        }
    }

    if ((value & 1) && maincpu_clk > vic_ii.raster_irq_clk
        && vic_ii.raster_irq_line < vic_ii.screen_height) {
        vic_ii.raster_irq_clk += vic_ii.screen_height
                                 * vic_ii.cycles_per_line;
        alarm_set(vic_ii.raster_irq_alarm, vic_ii.raster_irq_clk);
    }

    vic_ii.irq_status &= ~((value & 0xf) | 0x80);
    vicii_irq_set_line();

    VIC_II_DEBUG_REGISTER(("IRQ flag register: $%02X", vic_ii.irq_status));
}

inline static void store_d01a(BYTE value)
{
    vic_ii.regs[0x1a] = value & 0xf;

    vicii_irq_set_line();

    VIC_II_DEBUG_REGISTER(("IRQ mask register: $%02X", vic_ii.regs[addr]));
}

inline static void store_d01b(BYTE value)
{
    int i;
    BYTE b;
    int raster_x;

    VIC_II_DEBUG_REGISTER(("Sprite priority register: $%02X", value));

    if (value == vic_ii.regs[0x1b])
        return;

    raster_x = VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk));

    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
        raster_sprite_t *sprite;

        sprite = vic_ii.raster.sprite_status->sprites + i;

        if (sprite->x < raster_x)
            raster_add_int_change_next_line(&vic_ii.raster,
                                            &sprite->in_background,
                                            value & b ? 1 : 0);
        else
            sprite->in_background = value & b ? 1 : 0;
    }

    vic_ii.regs[0x1b] = value;
}

inline static void store_d01c(BYTE value)
{
    int i;
    BYTE b;
    int raster_x;

    VIC_II_DEBUG_REGISTER(("Sprite Multicolor Enable register: $%02X", value));

    if (value == vic_ii.regs[0x1c])
        return;

    raster_x = VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk));

    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
        raster_sprite_t *sprite;

        sprite = vic_ii.raster.sprite_status->sprites + i;
        if (sprite->x < raster_x)
            raster_add_int_change_next_line(&vic_ii.raster,
                                            &sprite->multicolor,
                                            value & b ? 1 : 0);
        else
            sprite->multicolor = value & b ? 1 : 0;
    }

    vic_ii.regs[0x1c] = value;
}

inline static void store_d01d(BYTE value)
{
    int raster_x;
    int i;
    BYTE b;

    VIC_II_DEBUG_REGISTER(("Sprite X Expand register: $%02X", value));

    if (value == vic_ii.regs[0x1d])
        return;

    raster_x = VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk));

    /* FIXME: how is this handled in the middle of one line?  */
    for (i = 0, b = 0x01; i < 8; b <<= 1, i++) {
        raster_sprite_t *sprite;

        sprite = vic_ii.raster.sprite_status->sprites + i;

        if (raster_x < sprite->x)
            sprite->x_expanded = value & b ? 1 : 0;
        else
            raster_add_int_change_next_line(&vic_ii.raster,
                                            &sprite->x_expanded,
                                            value & b ? 1 : 0);
    }

    vic_ii.regs[0x1d] = value;
}

inline static void store_collision(ADDRESS addr, BYTE value)
{
    VIC_II_DEBUG_REGISTER(("(collision register, Read Only)"));
}

inline static void store_d020(BYTE value)
{
    VIC_II_DEBUG_REGISTER(("Border color register: $%02X", value));

    value &= 0xf;

    if (vic_ii.regs[0x20] == value)
        return;

    vic_ii.regs[0x20] = value;
    raster_add_int_change_border(&vic_ii.raster,
        VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
        &vic_ii.raster.border_color,
        value);
}

inline static void store_d021(BYTE value)
{
    int x_pos;

    value &= 0xf;

    VIC_II_DEBUG_REGISTER(("Background #0 color register: $%02X",
                          value));

    if (vic_ii.regs[0x21] == value)
        return;

    x_pos = VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk));

    if (!vic_ii.force_black_overscan_background_color) {
        raster_add_int_change_background
            (&vic_ii.raster, x_pos,
            &vic_ii.raster.overscan_background_color, value);
        raster_add_int_change_background
            (&vic_ii.raster, x_pos,
            &vic_ii.raster.xsmooth_color, value);
    }

    raster_add_int_change_background(&vic_ii.raster, x_pos,
                                     &vic_ii.raster.background_color, value);

    vic_ii.regs[0x21] = value;
}

inline static void store_ext_background(ADDRESS addr, BYTE value)
{
    int char_num;

    value &= 0xf;

    VIC_II_DEBUG_REGISTER(("Background color #%d register: $%02X",
                          addr - 0x21, value));

    if (vic_ii.regs[addr] == value)
        return;

    vic_ii.regs[addr] = value;

    char_num = VIC_II_RASTER_CHAR(VIC_II_RASTER_CYCLE(maincpu_clk));

    if (vic_ii.video_mode == VIC_II_EXTENDED_TEXT_MODE) {
        raster_add_int_change_background
            (&vic_ii.raster,
            VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
            &vic_ii.raster.overscan_background_color,
            vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
        raster_add_int_change_background
            (&vic_ii.raster,
            VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)),
            &vic_ii.raster.xsmooth_color,
            vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
    }

    raster_add_int_change_foreground(&vic_ii.raster,
                                     char_num - 1,
                                     &vic_ii.ext_background_color[addr - 0x22],
                                     value);
}

inline static void store_d025(BYTE value)
{
    raster_sprite_status_t *sprite_status;

    value &= 0xf;

    VIC_II_DEBUG_REGISTER(("Sprite multicolor register #0: $%02X", value));

    if (vic_ii.regs[0x25] == value)
        return;

    sprite_status = vic_ii.raster.sprite_status;

    /* FIXME: this is approximated.  */
    if (VIC_II_RASTER_CYCLE(maincpu_clk) > vic_ii.cycles_per_line / 2)
        raster_add_int_change_next_line(&vic_ii.raster,
            (int *)&sprite_status->mc_sprite_color_1,
            (int)value);
    else
        sprite_status->mc_sprite_color_1 = value;

    vic_ii.regs[0x25] = value;
}

inline static void store_d026(BYTE value)
{
    raster_sprite_status_t *sprite_status;

    value &= 0xf;

    VIC_II_DEBUG_REGISTER(("Sprite multicolor register #1: $%02X", value));

    if (vic_ii.regs[0x26] == value)
        return;

    sprite_status = vic_ii.raster.sprite_status;

    /* FIXME: this is approximated.  */
    if (VIC_II_RASTER_CYCLE(maincpu_clk) > vic_ii.cycles_per_line / 2)
        raster_add_int_change_next_line(&vic_ii.raster,
            (int *)&sprite_status->mc_sprite_color_2,
            (int)value);
    else
        sprite_status->mc_sprite_color_2 = value;

    vic_ii.regs[0x26] = value;
}

inline static void store_sprite_color(ADDRESS addr, BYTE value)
{
    raster_sprite_t *sprite;
    int n;

    value &= 0xf;

    VIC_II_DEBUG_REGISTER(("Sprite #%d color register: $%02X",
                          addr - 0x27, value));

    if (vic_ii.regs[addr] == value)
        return;

    n = addr - 0x27;

    sprite = vic_ii.raster.sprite_status->sprites + n;

    if (sprite->x < VIC_II_RASTER_X(VIC_II_RASTER_CYCLE(maincpu_clk)))
        raster_add_int_change_next_line(&vic_ii.raster,
                                        (int *)&sprite->color,
                                        (int)value);
    else
        sprite->color = value;

    vic_ii.regs[addr] = value;
}

inline static void store_d02f(BYTE value)
{
    if (vic_ii.viciie) {
        VIC_II_DEBUG_REGISTER(("Extended keyboard row enable: $%02X",
                              value));
        vic_ii.regs[0x2f] = value | 0xf8;
        cia1_set_extended_keyboard_rows_mask(value);
    } else {
        VIC_II_DEBUG_REGISTER(("(unused)"));
    }
}

inline static void store_d030(BYTE value)
{
    if (vic_ii.viciie) {
        VIC_II_DEBUG_REGISTER(("Store $D030: $%02X", value));
        vic_ii.regs[0x30] = value | 0xfc;
    } else {
        VIC_II_DEBUG_REGISTER(("(unused)"));
    }
}

/* Store a value in a VIC-II register.  */
void REGPARM2 vicii_store(ADDRESS addr, BYTE value)
{
    addr &= 0x3f;

    /* WARNING: assumes `maincpu_rmw_flag' is 0 or 1.  */
    vicii_handle_pending_alarms(maincpu_rmw_flag + 1);

    /* This is necessary as we must be sure that the previous line has been
       updated and `current_line' is actually set to the current Y position of
       the raster.  Otherwise we might mix the changes for this line with the
       changes for the previous one.  */
    if (maincpu_clk >= vic_ii.draw_clk)
        vicii_raster_draw_alarm_handler(maincpu_clk - vic_ii.draw_clk);

    VIC_II_DEBUG_REGISTER(("WRITE $D0%02X at cycle %d of current_line $%04X",
                          addr, VIC_II_RASTER_CYCLE(maincpu_clk),
                          VIC_II_RASTER_Y(maincpu_clk)));

    switch (addr) {
      case 0x0:                   /* $D000: Sprite #0 X position LSB */
      case 0x2:                   /* $D002: Sprite #1 X position LSB */
      case 0x4:                   /* $D004: Sprite #2 X position LSB */
      case 0x6:                   /* $D006: Sprite #3 X position LSB */
      case 0x8:                   /* $D008: Sprite #4 X position LSB */
      case 0xa:                   /* $D00a: Sprite #5 X position LSB */
      case 0xc:                   /* $D00c: Sprite #6 X position LSB */
      case 0xe:                   /* $D00e: Sprite #7 X position LSB */
        store_sprite_x_position_lsb(addr, value);
        break;

      case 0x1:                   /* $D001: Sprite #0 Y position */
      case 0x3:                   /* $D003: Sprite #1 Y position */
      case 0x5:                   /* $D005: Sprite #2 Y position */
      case 0x7:                   /* $D007: Sprite #3 Y position */
      case 0x9:                   /* $D009: Sprite #4 Y position */
      case 0xb:                   /* $D00B: Sprite #5 Y position */
      case 0xd:                   /* $D00D: Sprite #6 Y position */
      case 0xf:                   /* $D00F: Sprite #7 Y position */
        store_sprite_y_position(addr, (BYTE)((value + vic_ii.offset) & 255));
        break;

      case 0x10:                  /* $D010: Sprite X position MSB */
        store_sprite_x_position_msb(addr, value);
        break;

      case 0x11:                  /* $D011: video mode, Y scroll, 24/25 line
                                     mode and raster MSB */
        store_d011(value);
        break;

      case 0x12:                  /* $D012: Raster line compare */
        store_d012(value);
        break;

      case 0x13:                  /* $D013: Light Pen X */
      case 0x14:                  /* $D014: Light Pen Y */
        break;

      case 0x15:                  /* $D015: Sprite Enable */
        store_d015(value);
        break;

      case 0x16:                  /* $D016 */
        store_d016(value);
        break;

      case 0x17:                  /* $D017: Sprite Y-expand */
        store_d017(value);
        break;

      case 0x18:                  /* $D018: Video and char matrix base
                                     address */
        store_d018(value);
        break;

      case 0x19:                  /* $D019: IRQ flag register */
        store_d019(value);
        break;

      case 0x1a:                  /* $D01A: IRQ mask register */
        store_d01a(value);
        break;

      case 0x1b:                  /* $D01B: Sprite priority */
        store_d01b(value);
        break;

      case 0x1c:                  /* $D01C: Sprite Multicolor select */
        store_d01c(value);
        break;

      case 0x1d:                  /* $D01D: Sprite X-expand */
        store_d01d(value);
        break;

      case 0x1e:                  /* $D01E: Sprite-sprite collision */
      case 0x1f:                  /* $D01F: Sprite-background collision */
        store_collision(addr, value);
        break;

      case 0x20:                  /* $D020: Border color */
        store_d020(value);
        break;

      case 0x21:                  /* $D021: Background #0 color */
        store_d021(value);
        break;

      case 0x22:                  /* $D022: Background #1 color */
      case 0x23:                  /* $D023: Background #2 color */
      case 0x24:                  /* $D024: Background #3 color */
        store_ext_background(addr, value);
        break;

      case 0x25:                  /* $D025: Sprite multicolor register #0 */
        store_d025(value);
        break;

      case 0x26:                  /* $D026: Sprite multicolor register #1 */
        store_d026(value);
        break;

      case 0x27:                  /* $D027: Sprite #0 color */
      case 0x28:                  /* $D028: Sprite #1 color */
      case 0x29:                  /* $D029: Sprite #2 color */
      case 0x2a:                  /* $D02A: Sprite #3 color */
      case 0x2b:                  /* $D02B: Sprite #4 color */
      case 0x2c:                  /* $D02C: Sprite #5 color */
      case 0x2d:                  /* $D02D: Sprite #6 color */
      case 0x2e:                  /* $D02E: Sprite #7 color */
        store_sprite_color(addr, value);
        break;

      case 0x2f:                  /* $D02F: Unused (or extended keyboard row
                                     select) */
        store_d02f(value);
        break;

      case 0x30:                  /* $D030: Unused (or VIC-IIe extension) */
        store_d030(value);
        break;

      case 0x31:                  /* $D031: Unused */
      case 0x32:                  /* $D032: Unused */
      case 0x33:                  /* $D033: Unused */
      case 0x34:                  /* $D034: Unused */
      case 0x35:                  /* $D035: Unused */
      case 0x36:                  /* $D036: Unused */
      case 0x37:                  /* $D037: Unused */
      case 0x38:                  /* $D038: Unused */
      case 0x39:                  /* $D039: Unused */
      case 0x3a:                  /* $D03A: Unused */
      case 0x3b:                  /* $D03B: Unused */
      case 0x3c:                  /* $D03C: Unused */
      case 0x3d:                  /* $D03D: Unused */
      case 0x3e:                  /* $D03E: Unused */
      case 0x3f:                  /* $D03F: Unused */
        VIC_II_DEBUG_REGISTER(("(unused)"));
        break;
    }
}


/* Helper function for reading from $D011/$D012.  */
inline static unsigned int read_raster_y(void)
{
    int raster_y;

    raster_y = VIC_II_RASTER_Y(maincpu_clk);

    /* Line 0 is 62 cycles long, while line (SCREEN_HEIGHT - 1) is 64
       cycles long.  As a result, the counter is incremented one
       cycle later on line 0.  */
    if (raster_y == 0 && VIC_II_RASTER_CYCLE(maincpu_clk) == 0)
        raster_y = vic_ii.screen_height - 1;

    return raster_y;
}

/* Helper function for reading from $D019.  */
inline static BYTE read_d019(void)
{
    if (VIC_II_RASTER_Y(maincpu_clk) == vic_ii.raster_irq_line
        && (vic_ii.regs[0x1a] & 0x1))
        /* As int_raster() is called 2 cycles later than it should be to
           emulate the 6510 internal IRQ delay, `vic_ii.irq_status' might not
           have bit 0 set as it should.  */
        vic_ii.last_read_d019 = vic_ii.irq_status | 0xf1;
    else
        vic_ii.last_read_d019 = vic_ii.irq_status | 0x70;

    return vic_ii.last_read_d019;
}

/* Read a value from a VIC-II register.  */
BYTE REGPARM1 vicii_read(ADDRESS addr)
{
    addr &= 0x3f;

    /* Serve all pending events.  */
    vicii_handle_pending_alarms(0);

    VIC_II_DEBUG_REGISTER(("READ $D0%02X at cycle %d of current_line $%04X:",
                          addr, VIC_II_RASTER_CYCLE(maincpu_clk),
                          VIC_II_RASTER_Y(maincpu_clk)));

    /* Note: we use hardcoded values instead of `unused_bits_in_registers[]'
       here because this is a little bit faster.  */
    switch (addr) {
      case 0x0:                   /* $D000: Sprite #0 X position LSB */
      case 0x2:                   /* $D002: Sprite #1 X position LSB */
      case 0x4:                   /* $D004: Sprite #2 X position LSB */
      case 0x6:                   /* $D006: Sprite #3 X position LSB */
      case 0x8:                   /* $D008: Sprite #4 X position LSB */
      case 0xa:                   /* $D00a: Sprite #5 X position LSB */
      case 0xc:                   /* $D00c: Sprite #6 X position LSB */
      case 0xe:                   /* $D00e: Sprite #7 X position LSB */
          VIC_II_DEBUG_REGISTER(("Sprite #%d X position LSB: $%02X",
                                addr >> 1, vic_ii.regs[addr]));
          return vic_ii.regs[addr];

      case 0x1:                   /* $D001: Sprite #0 Y position */
      case 0x3:                   /* $D003: Sprite #1 Y position */
      case 0x5:                   /* $D005: Sprite #2 Y position */
      case 0x7:                   /* $D007: Sprite #3 Y position */
      case 0x9:                   /* $D009: Sprite #4 Y position */
      case 0xb:                   /* $D00B: Sprite #5 Y position */
      case 0xd:                   /* $D00D: Sprite #6 Y position */
      case 0xf:                   /* $D00F: Sprite #7 Y position */
          VIC_II_DEBUG_REGISTER(("Sprite #%d Y position: $%02X",
                                addr >> 1, vic_ii.regs[addr]));
          return (256 + vic_ii.regs[addr] - vic_ii.offset) % 256;

      case 0x10:                  /* $D010: Sprite X position MSB */
          VIC_II_DEBUG_REGISTER(("Sprite X position MSB: $%02X",
                                vic_ii.regs[addr]));
          return vic_ii.regs[addr];

      case 0x11:                /* $D011: video mode, Y scroll, 24/25 line mode
                                   and raster MSB */
      case 0x12:                /* $D012: Raster line compare */
        {
            unsigned int tmp = (vic_ii.screen_height + read_raster_y()
                               - vic_ii.offset) % vic_ii.screen_height;

            VIC_II_DEBUG_REGISTER(("Raster Line register %svalue = $%04X",
                                  (addr == 0x11 ? "(highest bit) " : ""), tmp));

            if (addr == 0x11)
                return (vic_ii.regs[addr] & 0x7f) | ((tmp & 0x100) >> 1);
            else
                return tmp & 0xff;
        }

      case 0x13:                  /* $D013: Light Pen X */
        VIC_II_DEBUG_REGISTER(("Light pen X: %d", vic_ii.light_pen.x));
        return vic_ii.light_pen.x;

      case 0x14:                  /* $D014: Light Pen Y */
        VIC_II_DEBUG_REGISTER(("Light pen Y: %d", vic_ii.light_pen.y));
        return vic_ii.light_pen.y;

      case 0x15:                  /* $D015: Sprite Enable */
        VIC_II_DEBUG_REGISTER(("Sprite Enable register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr];

      case 0x16:                  /* $D016 */
        VIC_II_DEBUG_REGISTER(("$D016 Control register read: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0xc0;

      case 0x17:                  /* $D017: Sprite Y-expand */
        VIC_II_DEBUG_REGISTER(("Sprite Y Expand register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr];

      case 0x18:                /* $D018: Video and char matrix base address */
        VIC_II_DEBUG_REGISTER(("Video memory address register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0x1;

      case 0x19:                  /* $D019: IRQ flag register */
        {
            BYTE tmp;

            tmp = read_d019();
            VIC_II_DEBUG_REGISTER(("Interrupt register: $%02X", tmp));

            return tmp;
        }

      case 0x1a:                  /* $D01A: IRQ mask register  */
        VIC_II_DEBUG_REGISTER(("Mask register: $%02X",
                              vic_ii.regs[addr] | 0xf0));
        return vic_ii.regs[addr] | 0xf0;

      case 0x1b:                  /* $D01B: Sprite priority */
        VIC_II_DEBUG_REGISTER(("Sprite Priority register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr];

      case 0x1c:                  /* $D01C: Sprite Multicolor select */
        VIC_II_DEBUG_REGISTER(("Sprite Multicolor Enable register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr];

      case 0x1d:                  /* $D01D: Sprite X-expand */
        VIC_II_DEBUG_REGISTER(("Sprite X Expand register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr];

      case 0x1e:                  /* $D01E: Sprite-sprite collision */
        /* Remove the pending sprite-sprite interrupt, as the collision
           register is reset upon read accesses.  */
        vicii_irq_sscoll_clear();

        if (vicii_resources.sprite_sprite_collisions_enabled) {
            vic_ii.regs[addr] = vic_ii.sprite_sprite_collisions;
            vic_ii.sprite_sprite_collisions = 0;
            VIC_II_DEBUG_REGISTER(("Sprite-sprite collision mask: $%02X",
                                  vic_ii.regs[addr]));
            return vic_ii.regs[addr];
        } else {
            VIC_II_DEBUG_REGISTER(("Sprite-sprite collision mask: $00 "
                                  "(emulation disabled)"));
            vic_ii.sprite_sprite_collisions = 0;
            return 0;
        }

      case 0x1f:                  /* $D01F: Sprite-background collision */
        /* Remove the pending sprite-background interrupt, as the collision
           register is reset upon read accesses.  */
        vicii_irq_sbcoll_clear();

        if (vicii_resources.sprite_background_collisions_enabled) {
            vic_ii.regs[addr] = vic_ii.sprite_background_collisions;
            vic_ii.sprite_background_collisions = 0;
            VIC_II_DEBUG_REGISTER(("Sprite-background collision mask: $%02X",
                                  vic_ii.regs[addr]));
#if defined (VIC_II_DEBUG_SB_COLLISIONS)
            log_message(vic_ii.log,
                        "vic_ii.sprite_background_collisions reset by $D01F "
                        "read at line 0x%X.",
                        VIC_II_RASTER_Y(clk));
#endif
            return vic_ii.regs[addr];
        } else {
            VIC_II_DEBUG_REGISTER(("Sprite-background collision mask: $00 "
                                  "(emulation disabled)"));
            vic_ii.sprite_background_collisions = 0;
            return 0;
        }

      case 0x20:                  /* $D020: Border color */
        VIC_II_DEBUG_REGISTER(("Border Color register: $%02X",
                              vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0xf0;

      case 0x21:                  /* $D021: Background #0 color */
      case 0x22:                  /* $D022: Background #1 color */
      case 0x23:                  /* $D023: Background #2 color */
      case 0x24:                  /* $D024: Background #3 color */
        VIC_II_DEBUG_REGISTER(("Background Color #%d register: $%02X",
                              addr - 0x21, vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0xf0;

      case 0x25:                  /* $D025: Sprite multicolor register #0 */
      case 0x26:                  /* $D026: Sprite multicolor register #1 */
        VIC_II_DEBUG_REGISTER(("Multicolor register #%d: $%02X",
                              addr - 0x22, vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0xf0;

      case 0x27:                  /* $D027: Sprite #0 color */
      case 0x28:                  /* $D028: Sprite #1 color */
      case 0x29:                  /* $D029: Sprite #2 color */
      case 0x2a:                  /* $D02A: Sprite #3 color */
      case 0x2b:                  /* $D02B: Sprite #4 color */
      case 0x2c:                  /* $D02C: Sprite #5 color */
      case 0x2d:                  /* $D02D: Sprite #6 color */
      case 0x2e:                  /* $D02E: Sprite #7 color */
        VIC_II_DEBUG_REGISTER(("Sprite #%d color: $%02X",
                              addr - 0x22, vic_ii.regs[addr]));
        return vic_ii.regs[addr] | 0xf0;

      case 0x2f:                  /* $D02F: Unused (or extended keyboard row
                                     select) */
        if (vic_ii.viciie) {
            VIC_II_DEBUG_REGISTER(("Extended keyboard row enable: $%02X",
                                  vic_ii.regs[addr]));
            return vic_ii.regs[addr];
        } else {
            VIC_II_DEBUG_REGISTER(("(unused)"));
            return 0xff;
        }
        break;

      case 0x30:                  /* $D030: Unused (or VIC-IIe extension) */
        if (vic_ii.viciie) {
            VIC_II_DEBUG_REGISTER(("Read $D030: $%02X",
                                  vic_ii.regs[addr]));
            return vic_ii.regs[addr];
        } else {
            VIC_II_DEBUG_REGISTER(("(unused)"));
            return 0xff;
        }
        break;

      case 0x31:                  /* $D031: Unused */
      case 0x32:                  /* $D032: Unused */
      case 0x33:                  /* $D033: Unused */
      case 0x34:                  /* $D034: Unused */
      case 0x35:                  /* $D035: Unused */
      case 0x36:                  /* $D036: Unused */
      case 0x37:                  /* $D037: Unused */
      case 0x38:                  /* $D038: Unused */
      case 0x39:                  /* $D039: Unused */
      case 0x3a:                  /* $D03A: Unused */
      case 0x3b:                  /* $D03B: Unused */
      case 0x3c:                  /* $D03C: Unused */
      case 0x3d:                  /* $D03D: Unused */
      case 0x3e:                  /* $D03E: Unused */
      case 0x3f:                  /* $D03F: Unused */
        return 0xff;

      default:
        return 0xff;
    }
    return 0xff;  /* make compiler happy */
}

BYTE REGPARM1 vicii_peek(ADDRESS addr)
{
    addr &= 0x3f;

    switch (addr) {
      case 0x11:              /* $D011: video mode, Y scroll, 24/25 line mode
                                 and raster MSB */
        return (vic_ii.regs[addr] & 0x7f) | ((read_raster_y () & 0x100) >> 1);
      case 0x12:              /* $D012: Raster line LSB */
        return read_raster_y() & 0xff;
      case 0x13:              /* $D013: Light Pen X */
        return vic_ii.light_pen.x;
      case 0x14:              /* $D014: Light Pen Y */
        return vic_ii.light_pen.y;
      case 0x19:
        return read_d019();
      case 0x1e:              /* $D01E: Sprite-sprite collision */
        return vic_ii.sprite_sprite_collisions;
      case 0x1f:              /* $D01F: Sprite-background collision */
        return vic_ii.sprite_background_collisions;
      case 0x2f:              /* Extended keyboard row select */
        if (vic_ii.viciie)
            return vic_ii.regs[addr] | 0xf8;
        else
            return /* vic_ii.regs[addr] | */ 0xff;
      default:
        return vic_ii.regs[addr] | unused_bits_in_registers[addr];
    }
}

