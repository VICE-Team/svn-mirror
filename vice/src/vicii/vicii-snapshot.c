/*
 * vicii-snapshot.c - Snapshot functionality for the MOS 6569 (VIC-II)
 * emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "vicii.h"
#include "vicii-sprites.h"

#include "vicii-snapshot.h"



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
   NewSpriteDmaMask   BYTE   1      value for SpriteDmaMask after drawing sprites
   RamBase            DWORD  1      pointer to the start of RAM seen by the VIC
   RasterCycle        BYTE   1      current vic_ii.raster cycle
   RasterLine         WORD   1      current vic_ii.raster line
   Registers          BYTE   64     VIC-II registers
   SbCollMask         BYTE   1      sprite-background collisions so far
   SpriteDmaMask      BYTE   1      sprites having DMA turned on
   SsCollMask         BYTE   1      sprite-sprite collisions so far
   VBank              BYTE   1      location of memory bank
   Vc                 WORD   1      internal VIC-II counter
   VcAdd              BYTE   1      value to add to Vc at the end of this line (vic_ii.mem_counter_inc)
   VcBase             WORD   1      internal VIC-II memory pointer
   VideoInt           BYTE   1      status of VIC-II IRQ (vic_ii.irq_status)

   [Sprite section: (repeat 8 times)]

   SpriteXMemPtr      BYTE   1      sprite memory pointer
   SpriteXMemPtrInc   BYTE   1      value to add to the MemPtr after fetch
   SpriteXExpFlipFlop BYTE   1      sprite expansion flip-flop

   [Alarm section]
   FetchEventTick     DWORD  1      ticks for the next "fetch" (DMA) event
   FetchEventType     BYTE   1      type of event (0: matrix, 1: sprite check, 2: sprite fetch)

 */



static char snap_module_name[] = "VIC-II";
#define SNAP_MAJOR 1
#define SNAP_MINOR 0



int 
vic_ii_snapshot_write_module (snapshot_t *s)
{
  int i;
  snapshot_module_t *m;

  /* FIXME: Dispatch all events?  */

  m = snapshot_module_create (s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
  if (m == NULL)
    return -1;

  if (0
      || snapshot_module_write_byte (m, (BYTE) vic_ii.allow_bad_lines) < 0 /* AllowBadLines */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.bad_line) < 0 /* BadLine */
   || snapshot_module_write_byte (m, (BYTE) vic_ii.raster.blank_enabled) < 0 /* Blank */
      || snapshot_module_write_byte_array (m, vic_ii.cbuf, 40) < 0 /* ColorBuf */
      || snapshot_module_write_byte_array (m, vic_ii.color_ram, 1024) < 0 /* ColorRam */
      || snapshot_module_write_byte (m, vic_ii.idle_state) < 0 /* IdleState */
    || snapshot_module_write_byte (m, (BYTE) vic_ii.light_pen.triggered) < 0    /* LPTrigger */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.light_pen.x) < 0  /* LPX */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.light_pen.y) < 0  /* LPY */
      || snapshot_module_write_byte_array (m, vic_ii.vbuf, 40) < 0      /* MatrixBuf */
      || snapshot_module_write_byte (m, vic_ii.raster.sprite_status.new_dma_msk) < 0    /* NewSpriteDmaMask */
      || snapshot_module_write_dword (m, (DWORD) (vic_ii.ram_base - ram)) < 0   /* RamBase */
      || snapshot_module_write_byte (m, (BYTE) VIC_II_RASTER_CYCLE (clk)) < 0   /* RasterCycle */
      || snapshot_module_write_word (m, (WORD) VIC_II_RASTER_Y (clk)) < 0       /* RasterLine */
    )
    goto fail;

  for (i = 0; i < 0x40; i++)
    if (snapshot_module_write_byte (m, (BYTE) vic_ii.regs[i]) < 0 /* Registers */ )
      goto fail;

  if (0
      || snapshot_module_write_byte (m, (BYTE) vic_ii.sprite_background_collisions) < 0         /* SbCollMask */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.raster.sprite_status.dma_msk) < 0         /* SpriteDmaMask */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.sprite_sprite_collisions) < 0     /* SsCollMask */
      || snapshot_module_write_word (m, (WORD) vic_ii.vbank) < 0        /* VBank */
      || snapshot_module_write_word (m, (WORD) vic_ii.mem_counter) < 0  /* Vc */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.mem_counter_inc) < 0      /* VcInc */
      || snapshot_module_write_word (m, (WORD) vic_ii.memptr) < 0       /* VcBase */
      || snapshot_module_write_byte (m, (BYTE) vic_ii.irq_status) < 0   /* VideoInt */
    )
    goto fail;

  for (i = 0; i < 8; i++)
    {
      if (0
          || snapshot_module_write_byte (m, (BYTE) vic_ii.raster.sprite_status.sprites[i].memptr) < 0   /* SpriteXMemPtr */
          || snapshot_module_write_byte (m, (BYTE) vic_ii.raster.sprite_status.sprites[i].memptr_inc) < 0       /* SpriteXMemPtrInc */
          || snapshot_module_write_byte (m, (BYTE) vic_ii.raster.sprite_status.sprites[i].exp_flag) < 0         /* SpriteXExpFlipFlop */
        )
        goto fail;
    }

  if (0
      || snapshot_module_write_dword (m, vic_ii.fetch_clk - clk) < 0    /* FetchEventTick */
      || snapshot_module_write_byte (m, vic_ii.fetch_idx) < 0   /* FetchEventType */
    )
    goto fail;

  return snapshot_module_close (m);

fail:
  if (m != NULL)
    snapshot_module_close (m);
  return -1;
}

/* Helper functions.  */

static int 
read_byte_into_int (snapshot_module_t *m, int *value_return)
{
  BYTE b;

  if (snapshot_module_read_byte (m, &b) < 0)
    return -1;
  *value_return = (int) b;
  return 0;
}



static int 
read_word_into_int (snapshot_module_t *m, int *value_return)
{
  WORD b;

  if (snapshot_module_read_word (m, &b) < 0)
    return -1;
  *value_return = (int) b;
  return 0;
}

int 
vic_ii_snapshot_read_module (snapshot_t *s)
{
  BYTE major_version, minor_version;
  int i;
  snapshot_module_t *m;

  m = snapshot_module_open (s, snap_module_name,
                            &major_version, &minor_version);
  if (m == NULL)
    return -1;

  if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR)
    {
      log_error (vic_ii.log,
                 "Snapshot module version (%d.%d) newer than %d.%d.",
                 major_version, minor_version,
                 SNAP_MAJOR, SNAP_MINOR);
      goto fail;
    }

  /* FIXME: initialize changes?  */

  if (0
      || read_byte_into_int (m, &vic_ii.allow_bad_lines) < 0 /* AllowBadLines */
      || read_byte_into_int (m, &vic_ii.bad_line) < 0   /* BadLine */
      || read_byte_into_int (m, &vic_ii.raster.blank_enabled) < 0 /* Blank */
      || snapshot_module_read_byte_array (m, vic_ii.cbuf, 40) < 0 /* ColorBuf */
      || snapshot_module_read_byte_array (m, vic_ii.color_ram, 1024) < 0        /* ColorRam */
      || read_byte_into_int (m, &vic_ii.idle_state) < 0 /* IdleState */
      || read_byte_into_int (m, &vic_ii.light_pen.triggered) < 0 /* LPTrigger */
      || read_byte_into_int (m, &vic_ii.light_pen.x) < 0 /* LPX */
      || read_byte_into_int (m, &vic_ii.light_pen.y) < 0 /* LPY */
      || snapshot_module_read_byte_array (m, vic_ii.vbuf, 40) < 0 /* MatrixBuf */
      || snapshot_module_read_byte (m, &vic_ii.raster.sprite_status.new_dma_msk) < 0    /* NewSpriteDmaMask */
    )
    goto fail;

  {
    DWORD RamBase;

    if (snapshot_module_read_dword (m, &RamBase) < 0)
      goto fail;
    vic_ii.ram_base = ram + RamBase;
  }

  /* Read the current raster line and the current raster cycle.  As they
     are a function of `clk', this is just a sanity check.  */
  {
    WORD RasterLine;
    BYTE RasterCycle;

    if (snapshot_module_read_byte (m, &RasterCycle) < 0
        || snapshot_module_read_word (m, &RasterLine) < 0)
      goto fail;

    if (RasterCycle != (BYTE) VIC_II_RASTER_CYCLE (clk))
      {
        log_error (vic_ii.log,
                "Not matching raster cycle (%d) in snapshot; should be %d.",
                   RasterCycle, VIC_II_RASTER_CYCLE (clk));
        goto fail;
      }

    if (RasterLine != (WORD) VIC_II_RASTER_Y (clk))
      {
        log_error (vic_ii.log, "VIC-II: Not matching raster line (%d) in snapshot; should be %d.",
                   RasterLine, VIC_II_RASTER_Y (clk));
        goto fail;
      }
  }

  for (i = 0; i < 0x40; i++)
    if (read_byte_into_int (m, &vic_ii.regs[i]) < 0 /* Registers */ )
      goto fail;

  if (0
      || snapshot_module_read_byte (m, &vic_ii.sprite_background_collisions) < 0 /* SbCollMask */
      || snapshot_module_read_byte (m, &vic_ii.raster.sprite_status.dma_msk) < 0 /* SpriteDmaMask */
      || snapshot_module_read_byte (m, &vic_ii.sprite_sprite_collisions) < 0 /* SsCollMask */
      || read_word_into_int (m, &vic_ii.vbank) < 0 /* VBank */
      || read_word_into_int (m, &vic_ii.mem_counter) < 0 /* Vc */
      || read_byte_into_int (m, &vic_ii.mem_counter_inc) < 0 /* VcInc */
      || read_word_into_int (m, &vic_ii.memptr) < 0 /* VcBase */
      || read_byte_into_int (m, &vic_ii.irq_status) < 0 /* VideoInt */
    )
    goto fail;

  for (i = 0; i < 8; i++)
    {
      if (0
          || read_byte_into_int (m, &vic_ii.raster.sprite_status.sprites[i].memptr) < 0         /* SpriteXMemPtr */
          || read_byte_into_int (m, &vic_ii.raster.sprite_status.sprites[i].memptr_inc) < 0     /* SpriteXMemPtrInc */
          || read_byte_into_int (m, &vic_ii.raster.sprite_status.sprites[i].exp_flag) < 0       /* SpriteXExpFlipFlop */
        )
        goto fail;
    }

  /* FIXME: Recalculate alarms and derived values.  */

  vic_ii_set_raster_irq (vic_ii.regs[0x12]
                         | ((vic_ii.regs[0x11] & 0x80) << 1));
  vic_ii_update_memory_ptrs (VIC_II_RASTER_CYCLE (clk));

  /* Update sprite parameters.  We had better do this manually, or the
     VIC-II emulation could be quite upset.  */
  {
    BYTE msk;

    for (i = 0, msk = 0x1; i < 8; i++, msk <<= 1)
      {
        raster_sprite_t *sprite;
        int tmp;

        sprite = vic_ii.raster.sprite_status.sprites + i;

        /* X/Y coordinates.  */
        tmp = vic_ii.regs[i * 2] + ((vic_ii.regs[0x10] & msk) ? 0x100 : 0);

        /* (-0xffff makes sure it's updated NOW.) */
        vic_ii_sprites_set_x_position (i, tmp, -0xffff);

        sprite->y = (int) vic_ii.regs[i * 2 + 1];
        sprite->x_expanded = (int) (vic_ii.regs[0x1d] & msk);
        sprite->y_expanded = (int) (vic_ii.regs[0x17] & msk);
        sprite->multicolor = (int) (vic_ii.regs[0x1c] & msk);
        sprite->in_background = (int) (vic_ii.regs[0x1b] & msk);
        sprite->color = (int) vic_ii.regs[0x27 + i] & 0xf;
        sprite->dma_flag = (int) (vic_ii.raster.sprite_status.new_dma_msk
                                  & msk);
      }
  }

  vic_ii.raster.xsmooth = vic_ii.regs[0x16] & 0x7;
  vic_ii.raster.ysmooth = vic_ii.regs[0x11] & 0x7;
  vic_ii.raster.current_line = VIC_II_RASTER_Y (clk);     /* FIXME? */

  vic_ii.raster.sprite_status.visible_msk = vic_ii.regs[0x15];

  /* Update colors.  */
  vic_ii.raster.border_color = vic_ii.regs[0x20] & 0xf;
  vic_ii.raster.background_color = vic_ii.regs[0x21] & 0xf;
  vic_ii.ext_background_color[0] = vic_ii.regs[0x22] & 0xf;
  vic_ii.ext_background_color[1] = vic_ii.regs[0x23] & 0xf;
  vic_ii.ext_background_color[2] = vic_ii.regs[0x24] & 0xf;
  vic_ii.raster.sprite_status.mc_sprite_color_1 = vic_ii.regs[0x25] & 0xf;
  vic_ii.raster.sprite_status.mc_sprite_color_2 = vic_ii.regs[0x26] & 0xf;

  vic_ii.raster.blank = !(vic_ii.regs[0x11] & 0x10);

  if (vic_ii.raster.video_mode == VIC_II_HIRES_BITMAP_MODE
      || VIC_II_IS_ILLEGAL_MODE (vic_ii.raster.video_mode))
    {
      vic_ii.raster.overscan_background_color = 0;
      vic_ii.force_black_overscan_background_color = 1;
    }
  else
    {
      vic_ii.raster.overscan_background_color = vic_ii.raster.background_color;
      vic_ii.force_black_overscan_background_color = 0;
    }

  if (vic_ii.regs[0x11] & 0x8)
    {
      vic_ii.raster.display_ystart = VIC_II_25ROW_START_LINE;
      vic_ii.raster.display_ystop = VIC_II_25ROW_STOP_LINE;
    }
  else
    {
      vic_ii.raster.display_ystart = VIC_II_24ROW_START_LINE;
      vic_ii.raster.display_ystop = VIC_II_24ROW_STOP_LINE;
    }

  if (vic_ii.regs[0x16] & 0x8)
    {
      vic_ii.raster.display_xstart = VIC_II_40COL_START_PIXEL;
      vic_ii.raster.display_xstop = VIC_II_40COL_STOP_PIXEL;
    }
  else
    {
      vic_ii.raster.display_xstart = VIC_II_38COL_START_PIXEL;
      vic_ii.raster.display_xstop = VIC_II_38COL_STOP_PIXEL;
    }

  /* `vic_ii.raster.draw_idle_state', `vic_ii.raster.open_right_border' and
     `vic_ii.raster.open_left_border' should be needed, but they would only
     affect the current vic_ii.raster line, and would not cause any
     difference in timing.  So who cares.  */

  /* FIXME: `vic_ii.ycounter_reset_checked'?  */
  /* FIXME: `vic_ii.force_display_state'?  */

  vic_ii.memory_fetch_done = 0; /* FIXME? */

  vic_ii_update_video_mode (VIC_II_RASTER_CYCLE (clk));

  vic_ii.draw_clk = clk + (VIC_II_DRAW_CYCLE - VIC_II_RASTER_CYCLE (clk));
  vic_ii.last_emulate_line_clk = vic_ii.draw_clk - VIC_II_CYCLES_PER_LINE;
  alarm_set (&vic_ii.raster_draw_alarm, vic_ii.draw_clk);

  {
    DWORD dw;
    BYTE b;

    if (0
        || snapshot_module_read_dword (m, &dw) < 0      /* FetchEventTick */
        || snapshot_module_read_byte (m, &b) < 0        /* FetchEventType */
      )
      goto fail;

    vic_ii.fetch_clk = clk + dw;
    vic_ii.fetch_idx = b;
    alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
  }

  if (vic_ii.irq_status & 0x80)
    set_int_noclk (&maincpu_int_status, I_RASTER, 1);

  raster_force_repaint (&vic_ii.raster);
  return 0;

fail:
  if (m != NULL)
    snapshot_module_close (m);
  return -1;
}
