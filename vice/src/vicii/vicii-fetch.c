/*
 * vicii-fetch.c - Phi2 data fetch for the MOS 6569 (VIC-II) emulation.
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

#include <string.h>

#include "alarm.h"
#include "c64cart.h"
#include "interrupt.h"
#include "maincpu.h"
#include "mem.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "types.h"
#include "vicii-fetch.h"
#include "vicii-sprites.h"
#include "viciitypes.h"


/* Emulate a matrix line fetch, `num' bytes starting from `offs'.  This takes
   care of the 10-bit counter wraparound.  */
inline void vic_ii_fetch_matrix(int offs, int num)
{
    int start_char;
    int c;

    /* Matrix fetches are done during Phi2, the fabulous "bad lines" */
    start_char = (vic_ii.mem_counter + offs) & 0x3ff;
    c = 0x3ff - start_char + 1;

    if (c >= num) {
        memcpy(vic_ii.vbuf + offs, vic_ii.screen_base + start_char, num);
        memcpy(vic_ii.cbuf + offs, mem_color_ram_ptr + start_char, num);
    } else {
        memcpy(vic_ii.vbuf + offs, vic_ii.screen_base + start_char, c);
        memcpy(vic_ii.vbuf + offs + c, vic_ii.screen_base, num - c);
        memcpy(vic_ii.cbuf + offs, mem_color_ram_ptr + start_char, c);
        memcpy(vic_ii.cbuf + offs + c, mem_color_ram_ptr, num - c);
    }

    vic_ii.background_color_source = vic_ii.vbuf[offs + num - 1];

    /* Set correct background color in in the xsmooth area.
       Because we are at the end of DMA the changes to xsmooth color
       cannot take effect on this line (the border is closed)!  */
    if (offs + num >= VIC_II_SCREEN_TEXTCOLS) {
        switch (vic_ii.get_background_from_vbuf) {
          case VIC_II_HIRES_BITMAP_MODE:
            raster_add_int_change_next_line(
                &vic_ii.raster,
                &vic_ii.raster.xsmooth_color,
                vic_ii.background_color_source & 0x0f);
            break;
          case VIC_II_EXTENDED_TEXT_MODE:
            raster_add_int_change_next_line(
                &vic_ii.raster,
                &vic_ii.raster.xsmooth_color,
                vic_ii.regs[0x21 + (vic_ii.background_color_source >> 6)]);
            break;
        }
    }
}

/* If we are on a bad line, do the DMA.  Return nonzero if cycles have been
   stolen.  */
inline static int do_matrix_fetch(CLOCK sub)
{
    if (!vic_ii.memory_fetch_done) {
        raster_t *raster;

        raster = &vic_ii.raster;

        vic_ii.memory_fetch_done = 1;
        vic_ii.mem_counter = vic_ii.memptr;

        if ((raster->current_line & 7) == (unsigned int)raster->ysmooth
            && vic_ii.allow_bad_lines
            && raster->current_line >= vic_ii.first_dma_line
            && raster->current_line <= vic_ii.last_dma_line) {
            vic_ii_fetch_matrix(0, VIC_II_SCREEN_TEXTCOLS);

            raster->draw_idle_state = 0;
            raster->ycounter = 0;

            vic_ii.idle_state = 0;
            vic_ii.idle_data_location = IDLE_NONE;
            vic_ii.ycounter_reset_checked = 1;
            vic_ii.memory_fetch_done = 2;

            maincpu_steal_cycles(vic_ii.fetch_clk,
                                 VIC_II_SCREEN_TEXTCOLS + 3 - sub, 0);

            vic_ii.bad_line = 1;
            return 1;
        }
    }

    return 0;
}

inline static int handle_fetch_matrix(long offset, CLOCK sub,
                                      CLOCK *write_offset)
{
    raster_t *raster;
    raster_sprite_status_t *sprite_status;

    *write_offset = 0;

    raster = &vic_ii.raster;
    sprite_status = raster->sprite_status;

    if (sprite_status->visible_msk == 0 && sprite_status->dma_msk == 0) {
        do_matrix_fetch(sub);

        /* As sprites are all turned off, there is no need for a sprite DMA
           check; next time we will VIC_II_FETCH_MATRIX again.  This works
           because a VIC_II_CHECK_SPRITE_DMA is forced in `vic_store()'
           whenever the mask becomes nonzero.  */

        /* This makes sure we only create VIC_II_FETCH_MATRIX events in the bad
           line range.  These checks are (a little) redundant for safety.  */
        if (raster->current_line < vic_ii.first_dma_line) {
            vic_ii.fetch_clk += ((vic_ii.first_dma_line
                                - raster->current_line)
                                * vic_ii.cycles_per_line);
        } else {
            if (raster->current_line >= vic_ii.last_dma_line)
                vic_ii.fetch_clk += ((vic_ii.screen_height
                                    - raster->current_line
                                    + vic_ii.first_dma_line)
                                    * vic_ii.cycles_per_line);
            else
                vic_ii.fetch_clk += vic_ii.cycles_per_line;
        }

        alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
        return 1;
    } else {
        int fetch_done;

        fetch_done = do_matrix_fetch(sub);

        /* Sprites might be turned on, check for sprite DMA next
           time.  */
        vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;

        /* Calculate time for next event.  */
        vic_ii.fetch_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                           + vic_ii.sprite_fetch_cycle);

        if (vic_ii.fetch_clk > maincpu_clk || offset == 0) {
            /* Prepare the next fetch event.  */
            alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
            return 1;
        }

        if (fetch_done && sub == 0)
            *write_offset = VIC_II_SCREEN_TEXTCOLS + 3;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/

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

inline static void check_sprite_dma(void)
{
    raster_sprite_status_t *sprite_status;
    int i, b;

    sprite_status = vic_ii.raster.sprite_status;

    if (!sprite_status->visible_msk && !sprite_status->dma_msk)
        return;

    sprite_status->new_dma_msk = sprite_status->dma_msk;

    for (i = 0, b = 1; i < VIC_II_NUM_SPRITES; i++, b <<= 1) {
        raster_sprite_t *sprite;

        sprite = sprite_status->sprites + i;

        if ((sprite_status->visible_msk & b)
            && sprite->y == ((int) vic_ii.raster.current_line & 0xff)
            && !sprite->dma_flag)
            turn_sprite_dma_on(i);
        else if (sprite->dma_flag) {
            sprite->memptr = (sprite->memptr + sprite->memptr_inc) & 0x3f;

            if (sprite->y_expanded)
                sprite->exp_flag = !sprite->exp_flag;

            sprite->memptr_inc = sprite->exp_flag ? 3 : 0;

            if (sprite->memptr == 63) {
                sprite->dma_flag = 0;
                sprite_status->new_dma_msk &= ~b;

                if ((sprite_status->visible_msk & b)
                    && sprite->y == ((int)vic_ii.raster.current_line & 0xff))
                    turn_sprite_dma_on(i);
            }
        }
    }
}

inline static int handle_check_sprite_dma(long offset, CLOCK sub)
{
    swap_sprite_data_buffers();

    check_sprite_dma();

    /* FIXME?  Slow!  */
    vic_ii.sprite_fetch_clk = (VIC_II_LINE_START_CLK(maincpu_clk)
                              + vic_ii.sprite_fetch_cycle);
    vic_ii.sprite_fetch_msk = vic_ii.raster.sprite_status->new_dma_msk;

    if (vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][0].cycle == -1) {
        if (vic_ii.raster.current_line >= vic_ii.first_dma_line - 1
            && vic_ii.raster.current_line <= vic_ii.last_dma_line + 1) {
            vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
            vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                               - vic_ii.sprite_fetch_cycle
                               + VIC_II_FETCH_CYCLE
                               + vic_ii.cycles_per_line);
        } else {
            vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
            vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                               + vic_ii.cycles_per_line);
        }
    } else {
        /* Next time, fetch sprite data.  */
        vic_ii.fetch_idx = VIC_II_FETCH_SPRITE;
        vic_ii.sprite_fetch_idx = 0;
        vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                           + vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][0].cycle);
    }

    /*log_debug("HCSD SCLK %i FCLK %i CLK %i OFFSET %li SUB %i",
                vic_ii.store_clk, vic_ii.fetch_clk, clk, offset, sub);*/

    if (vic_ii.store_clk != CLOCK_MAX) {
        if (vic_ii.store_clk + offset - 3 < vic_ii.fetch_clk) {
            vic_ii.ram_base_phi2[vic_ii.store_addr] = vic_ii.store_value;
            vic_ii.store_clk = CLOCK_MAX;
        }
    }

    if (vic_ii.fetch_clk > maincpu_clk || offset == 0) {
        alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
        return 1;
    }

    return 0;
}

/*-----------------------------------------------------------------------*/

inline static int handle_fetch_sprite(long offset, CLOCK sub,
                                      CLOCK *write_offset)
{
    const vic_ii_sprites_fetch_t *sf;
    unsigned int i;
    int next_cycle;
    raster_sprite_status_t *sprite_status;
    BYTE *bank, *spr_base;

    /* FIXME: optimize.  */

    sf = &vic_ii_sprites_fetch_table[vic_ii.sprite_fetch_msk][vic_ii.sprite_fetch_idx];

    sprite_status = vic_ii.raster.sprite_status;
    /* FIXME: the 3 byte sprite data is instead taken during a Ph1/Ph2/Ph1
       sequence. This is of minor interest, though, only for CBM-II... */
    bank = vic_ii.ram_base_phi1 + vic_ii.vbank_phi1;
    spr_base = (bank + 0x3f8 + ((vic_ii.regs[0x18] & 0xf0) << 6) + sf->first);

    /* Fetch sprite data.  */
    for (i = sf->first; i <= sf->last; i++, spr_base++) {
        if (vic_ii.sprite_fetch_msk & (1 << i)) {
            BYTE *src;
            BYTE *dest;
            int my_memptr;

            /*log_debug("SDMA %i",i);*/

            src = bank + (*spr_base << 6);
            my_memptr = sprite_status->sprites[i].memptr;
            dest = (BYTE *)(sprite_status->new_sprite_data + i);

            if (cart_ultimax_phi1) {
                if (*spr_base >= 0xc0)
                    src = (romh_banks + 0x1000 + (romh_bank << 13)
                          + ((*spr_base - 0xc0) << 6));
            } else {
                if (!(vic_ii.vbank_phi1 & 0x4000) && (*spr_base & 0xc0) == 0x40)
                    src = mem_chargen_rom_ptr + ((*spr_base - 0x40) << 6);
            }

            dest[0] = src[my_memptr];
            dest[1] = src[++my_memptr & 0x3f];
            dest[2] = src[++my_memptr & 0x3f];
        }
    }

    maincpu_steal_cycles(vic_ii.fetch_clk, sf->num - sub, sub);

    *write_offset = sub == 0 ? sf->num : 0;

    next_cycle = (sf + 1)->cycle;
    vic_ii.sprite_fetch_idx++;

    if (next_cycle == -1) {
        /* Next time, handle bad lines.  */
        if (vic_ii.raster.current_line >= vic_ii.first_dma_line - 1
            && vic_ii.raster.current_line <= vic_ii.last_dma_line + 1) {
            vic_ii.fetch_idx = VIC_II_FETCH_MATRIX;
            vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                               - vic_ii.sprite_fetch_cycle
                               + VIC_II_FETCH_CYCLE
                               + vic_ii.cycles_per_line);
        } else {
            vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
            vic_ii.fetch_clk = (vic_ii.sprite_fetch_clk
                               + vic_ii.cycles_per_line);
        }
    } else {
        vic_ii.fetch_clk = vic_ii.sprite_fetch_clk + next_cycle;
    }

    if (maincpu_clk >= vic_ii.draw_clk)
        vic_ii_raster_draw_alarm_handler(maincpu_clk - vic_ii.draw_clk);

    if (vic_ii.fetch_clk > maincpu_clk || offset == 0) {
        alarm_set(vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
        return 1;
    }

    if (maincpu_clk >= vic_ii.raster_irq_clk)
        vic_ii_raster_irq_alarm_handler(maincpu_clk - vic_ii.raster_irq_clk);

    return 0;
}

/*-----------------------------------------------------------------------*/

/* Handle sprite/matrix fetch events.  FIXME: could be made slightly
   faster.  */
void vic_ii_fetch_alarm_handler(CLOCK offset)
{
    CLOCK last_opcode_first_write_clk, last_opcode_last_write_clk;

    /* This kludgy thing is used to emulate the behavior of the 6510 when BA
       goes low.  When BA goes low, every read access stops the processor
       until BA is high again; write accesses happen as usual instead.  */

    if (offset > 0) {
        switch (OPINFO_NUMBER(last_opcode_info)) {
          case 0:
            /* In BRK, IRQ and NMI the 3rd, 4th and 5th cycles are write
               accesses, while the 1st, 2nd, 6th and 7th are read accesses.  */
            last_opcode_first_write_clk = maincpu_clk - 5;
            last_opcode_last_write_clk = maincpu_clk - 3;
            break;

          case 0x20:
            /* In JSR, the 4th and 5th cycles are write accesses, while the
               1st, 2nd, 3rd and 6th are read accesses.  */
            last_opcode_first_write_clk = maincpu_clk - 3;
            last_opcode_last_write_clk = maincpu_clk - 2;
            break;

          default:
            /* In all the other opcodes, all the write accesses are the last
               ones.  */
            if (maincpu_num_write_cycles() != 0) {
                last_opcode_last_write_clk = maincpu_clk - 1;
                last_opcode_first_write_clk = maincpu_clk
                                              - maincpu_num_write_cycles();
            } else {
                last_opcode_first_write_clk = (CLOCK)0;
                last_opcode_last_write_clk = last_opcode_first_write_clk;
            }
            break;
        }
    } else { /* offset <= 0, i.e. offset == 0 */
        /* If we are called with no offset, we don't have to care about write
           accesses.  */
        last_opcode_first_write_clk = last_opcode_last_write_clk = 0;
    }

    while (1) {
        CLOCK sub;
        CLOCK write_offset;
        int leave;

        if (vic_ii.fetch_clk < last_opcode_first_write_clk
            || vic_ii.fetch_clk > last_opcode_last_write_clk)
            sub = 0;
        else
            sub = last_opcode_last_write_clk - vic_ii.fetch_clk + 1;

        switch (vic_ii.fetch_idx) {
          case VIC_II_FETCH_MATRIX:
            leave = handle_fetch_matrix(offset, sub, &write_offset);
            last_opcode_first_write_clk += write_offset;
            last_opcode_last_write_clk += write_offset;
            break;

          case VIC_II_CHECK_SPRITE_DMA:
            leave = handle_check_sprite_dma(offset, sub);
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

