/*
 * ted-snapshot.c - Snapshot functionality for the TED emulation.
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

#include "alarm.h"
#include "interrupt.h"
#include "log.h"
#include "mem.h"
#include "snapshot.h"
#include "raster-sprite-status.h"
#include "raster-sprite.h"
#include "ted-irq.h"
#include "ted-snapshot.h"
#include "ted.h"
#include "tedtypes.h"
#include "types.h"


/* Make sure all the TED alarms are removed.  This just makes it easier to
   write functions for loading snapshot modules in other video chips without
   caring that the TED alarms are dispatched when they really shouldn't
   be.  */

void ted_snapshot_prepare(void)
{
    ted.fetch_clk = CLOCK_MAX;
    alarm_unset(ted.raster_fetch_alarm);
    ted.draw_clk = CLOCK_MAX;
    alarm_unset(ted.raster_draw_alarm);
    ted.raster_irq_clk = CLOCK_MAX;
    alarm_unset(ted.raster_irq_alarm);
}

/*

   This is the format of the VIC-II snapshot module.

   Name               Type   Size   Description

   AllowBadLines      BYTE   1      flag: if true, bad lines can happen
   BadLine            BYTE   1      flag: this is a bad line
   Blank              BYTE   1      flag: draw lines in border color
   ColorBuf           BYTE   40     character memory buffer (loaded at bad line)
   ColorRam           BYTE   1024   contents of color RAM
   IdleState          BYTE   1      flag: idle state enabled
   LPTrigger          BYTE   1      flag: light pen has been triggered
   LPX                BYTE   1      light pen X
   LPY                BYTE   1      light pen Y
   MatrixBuf          BYTE   40     video matrix buffer (loaded at bad line)
   NewSpriteDmaMask   BYTE   1      value for SpriteDmaMask after drawing
                                    sprites
   RamBase            DWORD  1      pointer to the start of RAM seen by the VIC
   RasterCycle        BYTE   1      current ted.raster cycle
   RasterLine         WORD   1      current ted.raster line
   Registers          BYTE   64     VIC-II registers
   SbCollMask         BYTE   1      sprite-background collisions so far
   SpriteDmaMask      BYTE   1      sprites having DMA turned on
   SsCollMask         BYTE   1      sprite-sprite collisions so far
   VBank              BYTE   1      location of memory bank
   Vc                 WORD   1      internal VIC-II counter
   VcAdd              BYTE   1      value to add to Vc at the end of this line
                                    (ted.mem_counter_inc)
   VcBase             WORD   1      internal VIC-II memory pointer
   VideoInt           BYTE   1      status of VIC-II IRQ (ted.irq_status)

   [Sprite section: (repeat 8 times)]

   SpriteXMemPtr      BYTE   1      sprite memory pointer
   SpriteXMemPtrInc   BYTE   1      value to add to the MemPtr after fetch
   SpriteXExpFlipFlop BYTE   1      sprite expansion flip-flop

   [Alarm section]
   FetchEventTick     DWORD  1      ticks for the next "fetch" (DMA) event
   FetchEventType     BYTE   1      type of event (0: matrix, 1: sprite check, 2: sprite fetch)

 */

static char snap_module_name[] = "TED";
#define SNAP_MAJOR 1
#define SNAP_MINOR 1

int ted_snapshot_write_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;

    /* FIXME: Dispatch all events?  */

    m = snapshot_module_create (s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (0
        /* AllowBadLines */
        || SMW_B(m, (BYTE)ted.allow_bad_lines) < 0
        /* BadLine */
        || SMW_B(m, (BYTE)ted.bad_line) < 0
        /* Blank */
        || SMW_B(m, (BYTE)ted.raster.blank_enabled) < 0
        /* ColorBuf */
        || SMW_BA(m, ted.cbuf, 40) < 0
        /* IdleState */
        || SMW_B(m, (BYTE)ted.idle_state) < 0
        /* MatrixBuf */
        || SMW_BA(m, ted.vbuf, 40) < 0
        /* NewSpriteDmaMask */
        || SMW_B(m, ted.raster.sprite_status->new_dma_msk) < 0
        /* RasterCycle */
        || SMW_B(m, (BYTE)TED_RASTER_CYCLE(maincpu_clk)) < 0
        /* RasterLine */
        || SMW_W(m, (WORD)(TED_RASTER_Y(maincpu_clk))) < 0
        )
        goto fail;

    for (i = 0; i < 0x40; i++)
        /* Registers */
        if (SMW_B(m, (BYTE)ted.regs[i]) < 0)
            goto fail;

    if (0
        /* SpriteDmaMask */
        || SMW_B(m, (BYTE)ted.raster.sprite_status->dma_msk) < 0
        /* Vc */
        || SMW_W(m, (WORD)ted.mem_counter) < 0
        /* VcInc */
        || SMW_B(m, (BYTE)ted.mem_counter_inc) < 0
        /* VcBase */
        || SMW_W(m, (WORD)ted.memptr) < 0
        /* VideoInt */
        || SMW_B(m, (BYTE)ted.irq_status) < 0
        )
        goto fail;

    for (i = 0; i < 8; i++) {
        if (0
            /* SpriteXMemPtr */
            || SMW_B(m,
                (BYTE)ted.raster.sprite_status->sprites[i].memptr) < 0
            /* SpriteXMemPtrInc */
            || SMW_B(m,
                (BYTE)ted.raster.sprite_status->sprites[i].memptr_inc) < 0
            /* SpriteXExpFlipFlop */
            || SMW_B(m,
                (BYTE)ted.raster.sprite_status->sprites[i].exp_flag) < 0
            )
            goto fail;
    }

    if (0
        /* FetchEventTick */
        || SMW_DW(m, ted.fetch_clk - maincpu_clk) < 0
        )
        goto fail;

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int ted_snapshot_read_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    int i;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        log_error(ted.log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    /* FIXME: initialize changes?  */

    if (0
        /* AllowBadLines */
        || SMR_B_INT(m, &ted.allow_bad_lines) < 0
        /* BadLine */
        || SMR_B_INT(m, &ted.bad_line) < 0 
        /* Blank */
        || SMR_B_INT(m, &ted.raster.blank_enabled) < 0
        /* ColorBuf */
        || SMR_BA(m, ted.cbuf, 40) < 0
        /* IdleState */
        || SMR_B_INT(m, &ted.idle_state) < 0
        /* MatrixBuf */
        || SMR_BA(m, ted.vbuf, 40) < 0
        /* NewSpriteDmaMask */
        || SMR_B(m, &ted.raster.sprite_status->new_dma_msk) < 0)
        goto fail;

    /* Read the current raster line and the current raster cycle.  As they
       are a function of `clk', this is just a sanity check.  */
    {
        WORD RasterLine;
        BYTE RasterCycle;

        if (SMR_B(m, &RasterCycle) < 0 || SMR_W(m, &RasterLine) < 0)
            goto fail;

        if (RasterCycle != (BYTE)TED_RASTER_CYCLE(maincpu_clk)) {
            log_error(ted.log,
                      "Not matching raster cycle (%d) in snapshot; should be %d.",
                      RasterCycle, TED_RASTER_CYCLE(maincpu_clk));
            goto fail;
        }

        if (RasterLine != (WORD)TED_RASTER_Y(maincpu_clk)) {
            log_error(ted.log,
                      "Not matching raster line (%d) in snapshot; should be %d.",
                      RasterLine, TED_RASTER_Y(maincpu_clk));
            goto fail;
        }
    }

    for (i = 0; i < 0x40; i++)
        if (SMR_B_INT(m, &ted.regs[i]) < 0 /* Registers */ )
            goto fail;


    if (0
        /* SpriteDmaMask */
        || SMR_B(m,
            &ted.raster.sprite_status->dma_msk) < 0
        /* Vc */
        || SMR_W_INT(m, &ted.mem_counter) < 0
        /* VcInc */
        || SMR_B_INT(m, &ted.mem_counter_inc) < 0
        /* VcBase */
        || SMR_W_INT(m, &ted.memptr) < 0
        /* VideoInt */
        || SMR_B_INT(m, &ted.irq_status) < 0)
        goto fail;

    for (i = 0; i < 8; i++) {
        if (0
            /* SpriteXMemPtr */
            || SMR_B_INT(m,
                &ted.raster.sprite_status->sprites[i].memptr) < 0
            /* SpriteXMemPtrInc */
            || SMR_B_INT(m,
                &ted.raster.sprite_status->sprites[i].memptr_inc) < 0
            /* SpriteXExpFlipFlop */
            || SMR_B_INT(m,
                &ted.raster.sprite_status->sprites[i].exp_flag) < 0)
            goto fail;
    }

    /* FIXME: Recalculate alarms and derived values.  */

    ted_irq_set_raster_line(ted.regs[0x12]
                            | ((ted.regs[0x11] & 0x80) << 1));

    ted_update_memory_ptrs(TED_RASTER_CYCLE(maincpu_clk));

    ted.raster.xsmooth = ted.regs[0x16] & 0x7;
    ted.raster.ysmooth = ted.regs[0x11] & 0x7;
    ted.raster.current_line = TED_RASTER_Y(maincpu_clk); /* FIXME? */

    ted.raster.sprite_status->visible_msk = ted.regs[0x15];

    /* Update colors.  */
    ted.raster.border_color = ted.regs[0x20] & 0xf;
    ted.raster.background_color = ted.regs[0x21] & 0xf;
    ted.ext_background_color[0] = ted.regs[0x22] & 0xf;
    ted.ext_background_color[1] = ted.regs[0x23] & 0xf;
    ted.ext_background_color[2] = ted.regs[0x24] & 0xf;
    ted.raster.sprite_status->mc_sprite_color_1 = ted.regs[0x25] & 0xf;
    ted.raster.sprite_status->mc_sprite_color_2 = ted.regs[0x26] & 0xf;

    ted.raster.blank = !(ted.regs[0x11] & 0x10);

    if (TED_IS_ILLEGAL_MODE (ted.raster.video_mode)) {
        ted.raster.idle_background_color = 0;
        ted.force_black_overscan_background_color = 1;
    } else {
        ted.raster.idle_background_color
            = ted.raster.background_color;
        ted.force_black_overscan_background_color = 0;
    }

    if (ted.regs[0x11] & 0x8) {
        ted.raster.display_ystart = ted.row_25_start_line;
        ted.raster.display_ystop = ted.row_25_stop_line;
    } else {
        ted.raster.display_ystart = ted.row_24_start_line;
        ted.raster.display_ystop = ted.row_24_stop_line;
    }

    if (ted.regs[0x16] & 0x8) {
        ted.raster.display_xstart = TED_40COL_START_PIXEL;
        ted.raster.display_xstop = TED_40COL_STOP_PIXEL;
    } else {
        ted.raster.display_xstart = TED_38COL_START_PIXEL;
        ted.raster.display_xstop = TED_38COL_STOP_PIXEL;
    }

    /* `ted.raster.draw_idle_state', `ted.raster.open_right_border' and
       `ted.raster.open_left_border' should be needed, but they would only
       affect the current ted.raster line, and would not cause any
       difference in timing.  So who cares.  */

    /* FIXME: `ted.ycounter_reset_checked'?  */
    /* FIXME: `ted.force_display_state'?  */

    ted.memory_fetch_done = 0; /* FIXME? */

    ted_update_video_mode(TED_RASTER_CYCLE(maincpu_clk));

    ted.draw_clk = maincpu_clk + (ted.draw_cycle
                   - TED_RASTER_CYCLE(maincpu_clk));
    ted.last_emulate_line_clk = ted.draw_clk - ted.cycles_per_line;
    alarm_set(ted.raster_draw_alarm, ted.draw_clk);

    {
        DWORD dw;

        if (SMR_DW(m, &dw) < 0)  /* FetchEventTick */
            goto fail;

        ted.fetch_clk = maincpu_clk + dw;

        alarm_set(ted.raster_fetch_alarm, ted.fetch_clk);
    }

    if (ted.irq_status & 0x80)
        interrupt_restore_irq(maincpu_int_status, ted.int_num, 1);

    raster_force_repaint(&ted.raster);
    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

