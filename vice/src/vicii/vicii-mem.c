/*
 * vicii-mem.c - Memory interface for the MOS6569 (VIC-II) emulation.
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

#include <stdlib.h>

#include "c64cia.h"

#include "vicii.h"
#include "vicii-resources.h"
#include "vicii-sprites.h"

#include "vicii-mem.h"



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
inline void REGPARM2 
vic_ii_local_store_vbank (ADDRESS addr, BYTE value)
{
  /* This can only cause "aesthetical" errors, so let's save some time if
     the current frame will not be visible.  */
  if (!vic_ii.raster.skip_frame)
    {
      int f;

      /* Argh... this is a dirty kludge!  We should probably find a cleaner
         solution.  */
      do
	{
	  CLOCK mclk;

	  /* WARNING: Assumes `rmw_flag' is 0 or 1.  */
	  mclk = clk - rmw_flag - 1;
	  f = 0;

	  if (mclk >= vic_ii.fetch_clk)
	    {
	      /* If the fetch starts here, the sprite fetch routine should
	         get the new value, not the old one.  */
	      if (mclk == vic_ii.fetch_clk)
		vic_ii.ram_base[addr] = value;
	      vic_ii_raster_fetch_alarm_handler (clk - vic_ii.fetch_clk);
	      f = 1;
	      /* WARNING: Assumes `rmw_flag' is 0 or 1.  */
	      mclk = clk - rmw_flag - 1;
	    }

	  if (mclk >= vic_ii.draw_clk)
	    {
	      vic_ii_raster_draw_alarm_handler (0);
	      f = 1;
	    }
	}
      while (f);
    }

  vic_ii.ram_base[addr] = value;
}

/* Encapsulate inlined function for other modules */
void REGPARM2 
store_vbank (ADDRESS addr, BYTE value)
{
  vic_ii_local_store_vbank(addr, value);
}

/* As `store_vbank()', but for the $3900...$39FF address range.  */
void REGPARM2 
store_vbank_39xx (ADDRESS addr, BYTE value)
{
  vic_ii_local_store_vbank (addr, value);

  if (vic_ii.idle_data_location == IDLE_39FF && (addr & 0x3fff) == 0x39ff)
    raster_add_int_change_foreground
      (&vic_ii.raster,
       VIC_II_RASTER_CHAR (VIC_II_RASTER_CYCLE (clk)),
       &vic_ii.idle_data,
       value);
}

/* As `store_vbank()', but for the $3F00...$3FFF address range.  */
void REGPARM2 
store_vbank_3fxx (ADDRESS addr, BYTE value)
{
  vic_ii_local_store_vbank (addr, value);
  if (vic_ii.idle_data_location == IDLE_3FFF && (addr & 0x3fff) == 0x3fff)
    raster_add_int_change_foreground
      (&vic_ii.raster,
       VIC_II_RASTER_CHAR (VIC_II_RASTER_CYCLE (clk)),
       &vic_ii.idle_data,
       value);
}



inline static void
store_sprite_x_position_lsb (ADDRESS addr, BYTE value)
{
  int n;
  int new_x;

  VIC_II_DEBUG_REGISTER (("\tSprite #%d X position LSB: $%02X\n", n, value));

  if (value == vic_ii.regs[addr])
    return;

  vic_ii.regs[addr] = value;

  n = addr >> 1;		/* Number of changed sprite.  */
  new_x = (value | (vic_ii.regs[0x10] & (1 << n) ? 0x100 : 0));
  vic_ii_sprites_set_x_position (n, new_x,
                                 VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk)));
}

inline static void
store_sprite_y_position (ADDRESS addr, BYTE value)
{
  int cycle;

  VIC_II_DEBUG_REGISTER (("\tSprite #%d Y position: $%02X\n",
			  addr >> 1, value));

  if (vic_ii.regs[addr] == value)
    return;

  cycle = VIC_II_RASTER_CYCLE (clk);

  if (cycle == VIC_II_SPRITE_FETCH_CYCLE + 1
      && value == (vic_ii.raster.current_line & 0xff))
    {
      vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
      vic_ii.fetch_clk = (VIC_II_LINE_START_CLK (clk)
			  + VIC_II_SPRITE_FETCH_CYCLE + 1);
      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
    }

  vic_ii.raster.sprite_status.sprites[addr >> 1].y = value;
  vic_ii.regs[addr] = value;
}

static inline void
store_sprite_x_position_msb (ADDRESS addr, BYTE value)
{
  int i;
  BYTE b;
  int raster_x;

  VIC_II_DEBUG_REGISTER (("\tSprite X position MSBs: $%02X\n", value));

  if (value == vic_ii.regs[addr])
    return;

  raster_x = VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk));

  vic_ii.regs[addr] = value;

  /* Recalculate the sprite X coordinates.  */
  for (i = 0, b = 0x01; i < 8; b <<= 1, i++)
    {
      int new_x;

      new_x = (vic_ii.regs[2 * i] | (value & b ? 0x100 : 0));
      vic_ii_sprites_set_x_position (i, new_x, raster_x);
    }
}

inline static void
check_bad_line_state_change_for_d011 (BYTE value, int cycle, int line)
{
  int was_bad_line, now_bad_line;

  /* Check whether bad line state has changed.  */
  was_bad_line = (vic_ii.allow_bad_lines
		  && (vic_ii.raster.ysmooth == (line & 7)));
  now_bad_line = (vic_ii.allow_bad_lines
		  && ((value & 7) == (line & 7)));

  if (was_bad_line && !now_bad_line)
    {

      /* Bad line becomes good.  */
      vic_ii.bad_line = 0;

      /* By changing the values in the registers, one can make the VIC
         switch from idle to display state, but not from display to
         idle state.  So we are always in display state if this
         happens.  This is only true if the value changes in some
         cycle > 0, though; otherwise, the line never becomes bad.  */
      if (cycle > 0)
	{
	  vic_ii.raster.draw_idle_state = vic_ii.idle_state = 0;
	  vic_ii.idle_data_location = IDLE_NONE;
	  if (cycle > VIC_II_FETCH_CYCLE + 2
	      && !vic_ii.ycounter_reset_checked)
	    {
	      vic_ii.raster.ycounter = 0;
	      vic_ii.ycounter_reset_checked = 1;
	    }
	}

    }
  else if (!was_bad_line && now_bad_line)
    {
      if (cycle >= VIC_II_FETCH_CYCLE
	  && cycle <= VIC_II_FETCH_CYCLE + VIC_II_SCREEN_TEXTCOLS + 3)
	{
	  int pos;		/* Value of line counter when this happens.  */
	  int inc;		/* Total increment for the line counter.  */
	  int num_chars;	/* Total number of characters to fetch.  */
	  int num_0xff_fetches;	/* Number of 0xff fetches to do.  */

	  vic_ii.bad_line = 1;

	  if (cycle <= VIC_II_FETCH_CYCLE + 2)
	    vic_ii.raster.ycounter = 0;

	  vic_ii.ycounter_reset_checked = 1;

	  num_chars = (VIC_II_SCREEN_TEXTCOLS
		       - (cycle - (VIC_II_FETCH_CYCLE + 3)));

	  if (num_chars <= VIC_II_SCREEN_TEXTCOLS)
	    {
	      /* Matrix fetches starts immediately, but the VIC needs
	         at least 3 cycles to become the bus master.  Before
	         this happens, it fetches 0xff.  */
	      num_0xff_fetches = 3;

	      /* If we were in idle state before creating the bad
	         line, the counters have not been incremented.  */
	      if (vic_ii.idle_state)
		{
		  pos = 0;
		  inc = num_chars;
		  if (inc < 0)
		    inc = 0;
		}
	      else
		{
		  pos = cycle - (VIC_II_FETCH_CYCLE + 3);
		  if (pos > VIC_II_SCREEN_TEXTCOLS - 1)
		    pos = VIC_II_SCREEN_TEXTCOLS - 1;
		  inc = VIC_II_SCREEN_TEXTCOLS;
		}
	    }
	  else
	    {
	      pos = 0;
	      num_chars = inc = VIC_II_SCREEN_TEXTCOLS;
	      num_0xff_fetches = cycle - VIC_II_FETCH_CYCLE;
	    }

	  /* This is normally done at cycle `VIC_II_FETCH_CYCLE + 2'.  */
	  vic_ii.mem_counter = vic_ii.memptr;

	  /* Force the DMA.  */
	  /* Note that `vic_ii.cbuf' is loaded from the value of
	     the next opcode as the VIC-II is not the bus master yet.  */
	  if (num_chars <= num_0xff_fetches)
	    {
	      memset (vic_ii.vbuf + pos, 0xff, num_chars);
	      memset (vic_ii.cbuf + pos, vic_ii.ram_base[reg_pc] & 0xf,
		      num_chars);
	    }
	  else
	    {
	      memset (vic_ii.vbuf + pos, 0xff, num_0xff_fetches);
	      memset (vic_ii.cbuf + pos, vic_ii.ram_base[reg_pc] & 0xf,
		      num_0xff_fetches);
	      vic_ii_fetch_matrix (pos + num_0xff_fetches,
				   num_chars - num_0xff_fetches);
	    }

	  /* Set the value by which `vic_ii.mem_counter' is incremented on
	     this line.  */
	  vic_ii.mem_counter_inc = inc;

	  /* Take over the bus until the memory fetch is done.  */
	  clk = (VIC_II_LINE_START_CLK (clk) + VIC_II_FETCH_CYCLE
                 + VIC_II_SCREEN_TEXTCOLS + 3);

	  /* Remember we have done a DMA.  */
	  vic_ii.memory_fetch_done = 2;

	  /* As we are on a bad line, switch to display state.  */
	  vic_ii.idle_state = 0;

	  /* Try to display things correctly.  This is not exact,
	     but should be OK for most cases (FIXME?).  */
	  if (inc == VIC_II_SCREEN_TEXTCOLS)
	    {
	      vic_ii.raster.draw_idle_state = 0;
	      vic_ii.idle_data_location = IDLE_NONE;
	    }
	}
      else if (cycle <= VIC_II_FETCH_CYCLE + VIC_II_SCREEN_TEXTCOLS + 6)
	{
	  /* Bad line has been generated after fetch interval, but
	     before `vic_ii.raster.ycounter' is incremented.  */

	  vic_ii.bad_line = 1;

	  /* If in idle state, counter is not incremented.  */
	  if (vic_ii.idle_state)
	    vic_ii.mem_counter_inc = 0;

	  /* We are not in idle state anymore.  */
	  /* This is not 100% correct, but should be OK for most cases.
	     (FIXME?)  */
	  vic_ii.raster.draw_idle_state = vic_ii.idle_state = 0;
	  vic_ii.idle_data_location = IDLE_NONE;

	}
      else
        {
	  /* Line is now bad, so we must switch to display state.
	     Anyway, we cannot do it here as the `ycounter' handling
	     must happen in as in idle state.  */
	  vic_ii.force_display_state = 1;
	}
    }
}

/* Here we try to emulate $D011...  */
inline static void 
store_d011 (ADDRESS addr, BYTE value)
{
  int new_irq_line;
  int cycle;
  int line;

  cycle = VIC_II_RASTER_CYCLE (clk);
  line = VIC_II_RASTER_Y (clk);

  VIC_II_DEBUG_REGISTER (("\tControl register: $%02X\n", value));
  VIC_II_DEBUG_REGISTER (("$D011 tricks at cycle %d, line $%04X, "
                          "value $%02X\n",
			  cycle, line, value));

  new_irq_line = ((vic_ii.raster_irq_line & 0xff) | ((value & 0x80) << 1));
  vic_ii_set_raster_irq (new_irq_line);

  /* This is the funniest part... handle bad line tricks.  */

  if (line == VIC_II_FIRST_DMA_LINE && (value & 0x10) != 0)
    vic_ii.allow_bad_lines = 1;

  if (vic_ii.raster.ysmooth != (value & 7)
      && line >= VIC_II_FIRST_DMA_LINE
      && line <= VIC_II_LAST_DMA_LINE)
    check_bad_line_state_change_for_d011 (value, cycle, line);

  vic_ii.raster.ysmooth = value & 0x7;

  /* Check for 24 <-> 25 line mode switch.  */
  if ((value ^ vic_ii.regs[addr]) & 8)
    {
      if (value & 0x8)
	{

	  /* 24 -> 25 row mode switch.  */

	  vic_ii.raster.display_ystart = VIC_II_25ROW_START_LINE;
	  vic_ii.raster.display_ystop = VIC_II_25ROW_STOP_LINE;

	  if (line == VIC_II_24ROW_STOP_LINE && cycle > 0)
	    {
	      /* If on the first line of the 24-line border, we
	         still see the 25-line (lowmost) border because the
	         border flip flop has already been turned on.  */
	      vic_ii.raster.blank_enabled = 1;
	    }
	  else if (!vic_ii.raster.blank && line == VIC_II_24ROW_START_LINE
		   && cycle > 0)
	    {
	      /* A 24 -> 25 switch somewhere on the first line of
	         the 24-row mode is enough to disable screen
	         blanking.  */
	      vic_ii.raster.blank_enabled = 0;
	    }

	  VIC_II_DEBUG_REGISTER (("\t25 line mode enabled\n"));

	}
      else
	{

	  /* 25 -> 24 row mode switch.  */

	  vic_ii.raster.display_ystart = VIC_II_24ROW_START_LINE;
	  vic_ii.raster.display_ystop = VIC_II_24ROW_STOP_LINE;

	  /* If on the last line of the 25-line border, we still see the
	     24-line (upmost) border because the border flip flop has
	     already been turned off.  */
	  if (!vic_ii.raster.blank
              && line == VIC_II_25ROW_START_LINE
              && cycle > 0)
            vic_ii.raster.blank_enabled = 0;
	  else if (line == VIC_II_25ROW_STOP_LINE && cycle > 0)
            vic_ii.raster.blank_enabled = 1;

	  VIC_II_DEBUG_REGISTER (("\t24 line mode enabled\n"));

	}
    }

  vic_ii.raster.blank = !(value & 0x10);	/* `DEN' bit.  */

  vic_ii.regs[addr] = value;

  /* FIXME: save time.  */
  vic_ii_update_video_mode (cycle);
}

inline static void
store_d012 (ADDRESS addr, BYTE value)
{
  unsigned int line;
  unsigned int old_raster_irq_line;

  VIC_II_DEBUG_REGISTER (("\tRaster compare register: $%02X\n", value));

  if (value == vic_ii.regs[addr])
    return;

  line = VIC_II_RASTER_Y (clk);
  vic_ii.regs[addr] = value;

  VIC_II_DEBUG_REGISTER (("\tRaster interrupt line set to $%04X\n",
			  vic_ii.raster_irq_line));

  old_raster_irq_line = vic_ii.raster_irq_line;
  vic_ii_set_raster_irq ((vic_ii.raster_irq_line & 0x100) | value);

  /* Check whether we should activate the IRQ line now.  */
  if (vic_ii.regs[0x1a] & 0x1)
    {
      int trigger_irq;

      trigger_irq = 0;

      if (rmw_flag)
	{
	  if (VIC_II_RASTER_CYCLE (clk) == 0)
	    {
	      unsigned int previous_line = VIC_II_PREVIOUS_LINE (line);

	      if (previous_line != old_raster_irq_line
		  && ((old_raster_irq_line & 0x100)
		      == (previous_line & 0x100)))
		trigger_irq = 1;
	    }
	  else if (line != old_raster_irq_line
		   && (old_raster_irq_line & 0x100) == (line & 0x100))
	    trigger_irq = 1;
	}

      if (vic_ii.raster_irq_line == line && line != old_raster_irq_line)
	trigger_irq = 1;

      if (trigger_irq)
	{
	  vic_ii.irq_status |= 0x81;
	  maincpu_set_irq (I_RASTER, 1);
	}
    }
}

inline static void
store_d015 (ADDRESS addr, BYTE value)
{
  int cycle;

  VIC_II_DEBUG_REGISTER (("\tSprite Enable register: $%02X\n", value));

  cycle = VIC_II_RASTER_CYCLE (clk);

  /* On the real C64, sprite DMA is checked two times: first at
     `VIC_II_SPRITE_FETCH_CYCLE', and then at `VIC_II_SPRITE_FETCH_CYCLE +
     1'.  In the average case, one DMA check is OK and there is no need to
     emulate both, but we have to kludge things a bit in case sprites are
     activated at cycle `VIC_II_SPRITE_FETCH_CYCLE + 1'.  */
  if (cycle == VIC_II_SPRITE_FETCH_CYCLE + 1
      && ((value ^ vic_ii.regs[addr]) & value) != 0)
    {
      vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
      vic_ii.fetch_clk = (VIC_II_LINE_START_CLK (clk)
			  + VIC_II_SPRITE_FETCH_CYCLE + 1);
      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
    }

  /* Sprites are turned on: force a DMA check.  */
  if (vic_ii.raster.sprite_status.visible_msk == 0
      && vic_ii.raster.sprite_status.dma_msk == 0
      && value != 0)
    {
      if ((vic_ii.fetch_idx == VIC_II_FETCH_MATRIX
	   && vic_ii.fetch_clk > clk
	   && cycle > VIC_II_FETCH_CYCLE
	   && cycle <= VIC_II_SPRITE_FETCH_CYCLE)
	  || vic_ii.raster.current_line < VIC_II_FIRST_DMA_LINE
	  || vic_ii.raster.current_line > VIC_II_LAST_DMA_LINE)
	{
	  CLOCK new_fetch_clk;

	  new_fetch_clk = (VIC_II_LINE_START_CLK (clk)
			   + VIC_II_SPRITE_FETCH_CYCLE);
	  if (cycle > VIC_II_SPRITE_FETCH_CYCLE)
	    new_fetch_clk += VIC_II_CYCLES_PER_LINE;
	  if (new_fetch_clk < vic_ii.fetch_clk)
	    {
	      vic_ii.fetch_idx = VIC_II_CHECK_SPRITE_DMA;
	      vic_ii.fetch_clk = new_fetch_clk;
	      alarm_set (&vic_ii.raster_fetch_alarm, vic_ii.fetch_clk);
	    }
	}
    }

  vic_ii.regs[addr] = vic_ii.raster.sprite_status.visible_msk = value;
}

inline static void
store_d016 (ADDRESS addr, BYTE value)
{
  raster_t *raster;
  int cycle;

  VIC_II_DEBUG_REGISTER (("\tControl register: $%02X\n", value));

  raster = &vic_ii.raster;
  cycle = VIC_II_RASTER_CYCLE (clk);

  /* FIXME: Line-based emulation!  */
  if ((value & 7) != (vic_ii.regs[addr] & 7))
    {
#if 1
      if (raster->skip_frame || VIC_II_RASTER_CHAR (cycle) <= 1)
	raster->xsmooth = value & 0x7;
      else
	raster_add_int_change_next_line (raster,
					 &raster->xsmooth,
					 value & 0x7);
#else
      raster_add_int_change_foreground (raster,
					VIC_II_RASTER_CHAR (cycle),
					&raster->xsmooth,
					value & 7);
#endif
    }

  /* Bit 4 (CSEL) selects 38/40 column mode.  */
  if ((value & 0x8) != (vic_ii.regs[addr] & 0x8))
    {
      if (value & 0x8)
	{
	  /* 40 column mode.  */
	  if (cycle <= 17)
	    raster->display_xstart = VIC_II_40COL_START_PIXEL;
	  else
	    raster_add_int_change_next_line (raster,
					     &raster->display_xstart,
					     VIC_II_40COL_START_PIXEL);
	  if (cycle <= 56)
	    raster->display_xstop = VIC_II_40COL_STOP_PIXEL;
	  else
	    raster_add_int_change_next_line (raster,
					     &raster->display_xstop,
					     VIC_II_40COL_STOP_PIXEL);
	  VIC_II_DEBUG_REGISTER (("\t40 column mode enabled\n"));

	  /* If CSEL changes from 0 to 1 at cycle 17, the border is
	     not turned off and this line is blank.  */
	  if (cycle == 17 && !(vic_ii.regs[addr] & 0x8))
	    raster->blank_this_line = 1;
	}
      else
	{
	  /* 38 column mode.  */
	  if (cycle <= 17)
	    raster->display_xstart = VIC_II_38COL_START_PIXEL;
	  else
	    raster_add_int_change_next_line (raster,
					     &raster->display_xstart,
					     VIC_II_38COL_START_PIXEL);
	  if (cycle <= 56)
	    raster->display_xstop = VIC_II_38COL_STOP_PIXEL;
	  else
	    raster_add_int_change_next_line (raster,
					     &raster->display_xstop,
					     VIC_II_38COL_STOP_PIXEL);
	  VIC_II_DEBUG_REGISTER (("\t38 column mode enabled\n"));

	  /* If CSEL changes from 1 to 0 at cycle 56, the lateral
	     border is open.  */
	  if (cycle == 56 && (vic_ii.regs[addr] & 0x8)
	      && (!raster->blank_enabled || raster->open_left_border))
	    raster->open_right_border = 1;
	}
    }

  vic_ii.regs[addr] = value;

  vic_ii_update_video_mode (cycle);
}

inline static void
store_d017 (ADDRESS addr, BYTE value)
{
  raster_sprite_status_t *sprite_status;
  int cycle;
  int i;
  BYTE b;

  VIC_II_DEBUG_REGISTER (("\tSprite Y Expand register: $%02X\n", value));

  if (value == vic_ii.regs[0x17])
    return;

  cycle = VIC_II_RASTER_CYCLE (clk);
  sprite_status = &vic_ii.raster.sprite_status;

  for (i = 0, b = 0x01; i < 8; b <<= 1, i++)
    {
      raster_sprite_t *sprite;

      sprite = sprite_status->sprites + i;

      sprite->y_expanded = value & b ? 1 : 0;

      if (!sprite->y_expanded && !sprite->exp_flag)
	{
	  /* Sprite crunch!  */
	  if (cycle == 15)
	    sprite->memptr_inc = vic_ii_sprites_crunch_table[sprite->memptr];
	  else if (cycle < 15)
	    sprite->memptr_inc = 3;
	  sprite->exp_flag = 1;
	}

      /* (Enabling sprite Y-expansion never causes side effects.)  */
    }

  vic_ii.regs[addr] = value;
}

inline static void
store_d018 (ADDRESS addr, BYTE value)
{
  VIC_II_DEBUG_REGISTER (("\tMemory register: $%02X\n", value));

  if (vic_ii.regs[addr] == value)
    return;

  vic_ii.regs[addr] = value;
  vic_ii_update_memory_ptrs (VIC_II_RASTER_CYCLE (clk));
}

inline static void
store_d019 (ADDRESS addr, BYTE value)
{
  if (rmw_flag) /* (emulates the Read-Modify-Write bug) */
    vic_ii.irq_status = 0;
  else
    {
      vic_ii.irq_status &= ~((value & 0xf) | 0x80);
      if (vic_ii.irq_status & vic_ii.regs[0x1a])
	vic_ii.irq_status |= 0x80;
    }

  /* Update the IRQ line accordingly...
     The external VIC IRQ line is an AND of the internal collision and
     vic_ii.raster IRQ lines.  */
  if (vic_ii.irq_status & 0x80)
    maincpu_set_irq (I_RASTER, 1);
  else
    maincpu_set_irq (I_RASTER, 0);

  VIC_II_DEBUG_REGISTER (("\tIRQ flag register: $%02X\n", vic_ii.irq_status));
}

inline static void
store_d01a (ADDRESS addr, BYTE value)
{
  vic_ii.regs[addr] = value & 0xf;

  if (vic_ii.regs[addr] & vic_ii.irq_status)
    {
      vic_ii.irq_status |= 0x80;
      maincpu_set_irq (I_RASTER, 1);
    }
  else
    {
      vic_ii.irq_status &= 0x7f;
      maincpu_set_irq (I_RASTER, 0);
    }

  VIC_II_DEBUG_REGISTER (("\tIRQ mask register: $%02X\n", vic_ii.regs[addr]));
}

inline static void
store_d01b (ADDRESS addr, BYTE value)
{
  int i;
  BYTE b;
  int raster_x;

  VIC_II_DEBUG_REGISTER (("\tSprite priority register: $%02X\n", value));

  if (value == vic_ii.regs[addr])
    return;

  raster_x = VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk));

  for (i = 0, b = 0x01; i < 8; b <<= 1, i++)
    {
      raster_sprite_t *sprite;

      sprite = vic_ii.raster.sprite_status.sprites + i;

      if (sprite->x < raster_x)
	raster_add_int_change_next_line (&vic_ii.raster,
					 &sprite->in_background,
					 value & b ? 1 : 0);
      else
	sprite->in_background = value & b ? 1 : 0;
    }

  vic_ii.regs[addr] = value;
}

inline static void
store_d01c (ADDRESS addr, BYTE value)
{
  int i;
  BYTE b;
  int raster_x;

  VIC_II_DEBUG_REGISTER (("\tSprite Multicolor Enable register: $%02X\n",
			  value));

  if (value == vic_ii.regs[addr])
    return;

  raster_x = VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk));

  vic_ii.regs[addr] = value;

  for (i = 0, b = 0x01; i < 8; b <<= 1, i++)
    {
      raster_sprite_t *sprite;

      sprite = vic_ii.raster.sprite_status.sprites + i;
      if (sprite->x < raster_x)
	raster_add_int_change_next_line (&vic_ii.raster,
					 &sprite->multicolor,
					 value & b ? 1 : 0);
      else
	sprite->multicolor = value & b ? 1 : 0;
    }
}

inline static void
store_d01d (ADDRESS addr, BYTE value)
{
  int raster_x;
  int i;
  BYTE b;

  VIC_II_DEBUG_REGISTER (("\tSprite X Expand register: $%02X\n", value));

  if (value == vic_ii.regs[addr])
    return;

  raster_x = VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk));

  /* FIXME: how is this handled in the middle of one line?  */
  for (i = 0, b = 0x01; i < 8; b <<= 1, i++)
    {
      raster_sprite_t *sprite;

      sprite = vic_ii.raster.sprite_status.sprites + i;

      if (1 || raster_x < sprite->x)
        sprite->x_expanded = value & b ? 1 : 0;
      else
        ;       /* FIXME: We are in trouble! */
    }

  vic_ii.regs[addr] = value;
}

inline static void
store_collision (ADDRESS addr, BYTE value)
{
  VIC_II_DEBUG_REGISTER (("\t(collision register, Read Only)\n"));
}

inline static void
store_d020 (ADDRESS addr, BYTE value)
{
  VIC_II_DEBUG_REGISTER (("\tBorder color register: $%02X\n", value));

  value &= 0xf;

  if (vic_ii.regs[addr] != value)
    {
      vic_ii.regs[addr] = value;
      raster_add_int_change_border (&vic_ii.raster,
				VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk)),
				    &vic_ii.raster.border_color,
				    value);
    }
}

inline static void
store_d021 (ADDRESS addr, BYTE value)
{
  int x_pos;

  value &= 0xf;

  VIC_II_DEBUG_REGISTER (("\tBackground #0 color register: $%02X\n",
			  value));

  if (vic_ii.regs[addr] == value)
    return;

  if (!vic_ii.force_black_overscan_background_color)
    raster_add_int_change_background
      (&vic_ii.raster,
       VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk)),
       &vic_ii.raster.overscan_background_color,
       value);

  x_pos = VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk));
  raster_add_int_change_background (&vic_ii.raster,
                                    x_pos,
                                    &vic_ii.raster.background_color,
                                    value);

  vic_ii.regs[addr] = value;
}

inline static void
store_ext_background (ADDRESS addr, BYTE value)
{
  int char_num;

  value &= 0xf;

  VIC_II_DEBUG_REGISTER (("\tBackground color #%d register: $%02X\n",
			  addr - 0x21, value));

  if (vic_ii.regs[addr] == value)
    return;

  char_num = VIC_II_RASTER_CHAR (VIC_II_RASTER_CYCLE (clk));

  raster_add_int_change_foreground (&vic_ii.raster,
                                    char_num,
                                    &vic_ii.ext_background_color[addr - 0x22],
                                    value);

  vic_ii.regs[addr] = value;
}

inline static void
store_d025 (ADDRESS addr, BYTE value)
{
  raster_sprite_status_t *sprite_status;

  value &= 0xf;

  VIC_II_DEBUG_REGISTER (("\tSprite multicolor register #0: $%02X\n", value));

  if (vic_ii.regs[addr] == value)
    return;

  sprite_status = &vic_ii.raster.sprite_status;

  /* FIXME: this is approximated.  */
  if (VIC_II_RASTER_CYCLE (clk) > VIC_II_CYCLES_PER_LINE / 2)
    raster_add_int_change_next_line (&vic_ii.raster,
				     &sprite_status->mc_sprite_color_1,
				     value);
  else
    sprite_status->mc_sprite_color_1 = value;

  vic_ii.regs[addr] = value;
}

inline static void
store_d026 (ADDRESS addr, BYTE value)
{
  raster_sprite_status_t *sprite_status;

  value &= 0xf;

  VIC_II_DEBUG_REGISTER (("\tSprite multicolor register #1: $%02X\n", value));

  if (vic_ii.regs[addr] == value)
    return;

  sprite_status = &vic_ii.raster.sprite_status;

  /* FIXME: this is approximated.  */
  if (VIC_II_RASTER_CYCLE (clk) > VIC_II_CYCLES_PER_LINE / 2)
    raster_add_int_change_next_line (&vic_ii.raster,
				     &sprite_status->mc_sprite_color_2,
				     value);
  else
    sprite_status->mc_sprite_color_2 = value;

  vic_ii.regs[addr] = value;
}

inline static void
store_sprite_color (ADDRESS addr, BYTE value)
{
  raster_sprite_t *sprite;
  int n;

  value &= 0xf;

  VIC_II_DEBUG_REGISTER (("\tSprite #%d color register: $%02X\n",
			  addr - 0x27, value));

  if (vic_ii.regs[addr] == value)
    return;

  n = addr - 0x27;

  sprite = vic_ii.raster.sprite_status.sprites + n;

  if (sprite->x < VIC_II_RASTER_X (VIC_II_RASTER_CYCLE (clk)))
    raster_add_int_change_next_line (&vic_ii.raster, &sprite->color, value);
  else
    sprite->color = value;

  vic_ii.regs[addr] = value;
}

inline static void
store_d02f (ADDRESS addr, BYTE value)
{
  if (vic_ii.extended_keyboard_rows_enabled)
    {
      VIC_II_DEBUG_REGISTER (("\tExtended keyboard row enable: $%02X\n",
			      value));
      vic_ii.regs[addr] = value | 0xf8;
      cia1_set_extended_keyboard_rows_mask (value);
    }
  else
    VIC_II_DEBUG_REGISTER (("\t(unused)\n"));
}

/* Store a value in a VIC-II register.  */
void REGPARM2 
store_vic (ADDRESS addr, BYTE value)
{
  addr &= 0x3f;

  /* WARNING: assumes `rmw_flag' is 0 or 1.  */
  vic_ii_handle_pending_alarms (rmw_flag + 1);

  /* This is necessary as we must be sure that the previous line has been
     updated and `current_line' is actually set to the current Y position of
     the raster.  Otherwise we might mix the changes for this line with the
     changes for the previous one.  */
  if (clk >= vic_ii.draw_clk)
    vic_ii_raster_draw_alarm_handler (clk - vic_ii.draw_clk);

  VIC_II_DEBUG_REGISTER (("VIC: WRITE $D0%02X at cycle %d of "
                          "current_line $%04X\n",
			  addr,
                          VIC_II_RASTER_CYCLE (clk),
			  VIC_II_RASTER_Y (clk)));

  switch (addr)
    {
    case 0x0:			/* $D000: Sprite #0 X position LSB */
    case 0x2:			/* $D002: Sprite #1 X position LSB */
    case 0x4:			/* $D004: Sprite #2 X position LSB */
    case 0x6:			/* $D006: Sprite #3 X position LSB */
    case 0x8:			/* $D008: Sprite #4 X position LSB */
    case 0xa:			/* $D00a: Sprite #5 X position LSB */
    case 0xc:			/* $D00c: Sprite #6 X position LSB */
    case 0xe:			/* $D00e: Sprite #7 X position LSB */
      store_sprite_x_position_lsb (addr, value);
      break;

    case 0x1:			/* $D001: Sprite #0 Y position */
    case 0x3:			/* $D003: Sprite #1 Y position */
    case 0x5:			/* $D005: Sprite #2 Y position */
    case 0x7:			/* $D007: Sprite #3 Y position */
    case 0x9:			/* $D009: Sprite #4 Y position */
    case 0xb:			/* $D00B: Sprite #5 Y position */
    case 0xd:			/* $D00D: Sprite #6 Y position */
    case 0xf:			/* $D00F: Sprite #7 Y position */
      store_sprite_y_position (addr, value);
      break;

    case 0x10:			/* $D010: Sprite X position MSB */
      store_sprite_x_position_msb (addr, value);
      break;

    case 0x11:			/* $D011: video mode, Y scroll, 24/25 line mode
				   and raster MSB */
      store_d011 (addr, value);
      break;

    case 0x12:			/* $D012: Raster line compare */
      store_d012 (addr, value);
      break;

    case 0x13:			/* $D013: Light Pen X */
    case 0x14:			/* $D014: Light Pen Y */
      break;

    case 0x15:			/* $D015: Sprite Enable */
      store_d015 (addr, value);
      break;

    case 0x16:			/* $D016 */
      store_d016 (addr, value);
      break;

    case 0x17:			/* $D017: Sprite Y-expand */
      store_d017 (addr, value);
      break;

    case 0x18:			/* $D018: Video and char matrix base address */
      store_d018 (addr, value);
      break;

    case 0x19:			/* $D019: IRQ flag register */
      store_d019 (addr, value);
      break;

    case 0x1a:			/* $D01A: IRQ mask register */
      store_d01a (addr, value);
      break;

    case 0x1b:			/* $D01B: Sprite priority */
      store_d01b (addr, value);
      break;

    case 0x1c:			/* $D01C: Sprite Multicolor select */
      store_d01c (addr, value);
      break;

    case 0x1d:			/* $D01D: Sprite X-expand */
      store_d01d (addr, value);
      break;

    case 0x1e:			/* $D01E: Sprite-sprite collision */
    case 0x1f:			/* $D01F: Sprite-background collision */
      store_collision (addr, value);
      break;

    case 0x20:			/* $D020: Border color */
      store_d020 (addr, value);
      break;

    case 0x21:			/* $D021: Background #0 color */
      store_d021 (addr, value);
      break;

    case 0x22:			/* $D022: Background #1 color */
    case 0x23:			/* $D023: Background #2 color */
    case 0x24:			/* $D024: Background #3 color */
      store_ext_background (addr, value);
      break;

    case 0x25:			/* $D025: Sprite multicolor register #0 */
      store_d025 (addr, value);
      break;

    case 0x26:			/* $D026: Sprite multicolor register #1 */
      store_d026 (addr, value);
      break;

    case 0x27:			/* $D027: Sprite #0 color */
    case 0x28:			/* $D028: Sprite #1 color */
    case 0x29:			/* $D029: Sprite #2 color */
    case 0x2a:			/* $D02A: Sprite #3 color */
    case 0x2b:			/* $D02B: Sprite #4 color */
    case 0x2c:			/* $D02C: Sprite #5 color */
    case 0x2d:			/* $D02D: Sprite #6 color */
    case 0x2e:			/* $D02E: Sprite #7 color */
      store_sprite_color (addr, value);
      break;

    case 0x2f:			/* $D02F: Unused (or extended keyboard row
				   select) */
      store_d02f (addr, value);
      break;

    case 0x30:			/* $D030: Unused */
    case 0x31:			/* $D031: Unused */
    case 0x32:			/* $D032: Unused */
    case 0x33:			/* $D033: Unused */
    case 0x34:			/* $D034: Unused */
    case 0x35:			/* $D035: Unused */
    case 0x36:			/* $D036: Unused */
    case 0x37:			/* $D037: Unused */
    case 0x38:			/* $D038: Unused */
    case 0x39:			/* $D039: Unused */
    case 0x3a:			/* $D03A: Unused */
    case 0x3b:			/* $D03B: Unused */
    case 0x3c:			/* $D03C: Unused */
    case 0x3d:			/* $D03D: Unused */
    case 0x3e:			/* $D03E: Unused */
    case 0x3f:			/* $D03F: Unused */
      VIC_II_DEBUG_REGISTER (("\t(unused)\n"));
      break;
    }
}



/* Helper function for reading from $D011/$D012.  */
inline static unsigned int 
read_raster_y (void)
{
  int raster_y;

  raster_y = VIC_II_RASTER_Y (clk);

  /* Line 0 is 62 cycles long, while line (SCREEN_HEIGHT - 1) is 64
     cycles long.  As a result, the counter is incremented one
     cycle later on line 0.  */
  if (raster_y == 0 && VIC_II_RASTER_CYCLE (clk) == 0)
    raster_y = VIC_II_SCREEN_HEIGHT - 1;

  return raster_y;
}

/* Helper function for reading from $D019.  */
inline static BYTE 
read_d019 (void)
{
  if (VIC_II_RASTER_Y (clk) == vic_ii.raster_irq_line
      && (vic_ii.regs[0x1a] & 0x1))
    /* As int_raster() is called 2 cycles later than it should be to
       emulate the 6510 internal IRQ delay, `vic_ii.irq_status' might not
       have bit 0 set as it should.  */
    return vic_ii.irq_status | 0x71;
  else
    return vic_ii.irq_status | 0x70;
}

/* Read a value from a VIC-II register.  */
BYTE REGPARM1 
read_vic (ADDRESS addr)
{
  addr &= 0x3f;

  /* Serve all pending events.  */
  vic_ii_handle_pending_alarms (0);

  VIC_II_DEBUG_REGISTER (("VIC: READ $D0%02X at cycle %d "
                          "of current_line $%04X:\n",
                          addr,
                          VIC_II_RASTER_CYCLE (clk),
                          VIC_II_RASTER_Y (clk)));

  /* Note: we use hardcoded values instead of `unused_bits_in_registers[]'
     here because this is a little bit faster.  */
  switch (addr)
    {
    case 0x0:			/* $D000: Sprite #0 X position LSB */
    case 0x2:			/* $D002: Sprite #1 X position LSB */
    case 0x4:			/* $D004: Sprite #2 X position LSB */
    case 0x6:			/* $D006: Sprite #3 X position LSB */
    case 0x8:			/* $D008: Sprite #4 X position LSB */
    case 0xa:			/* $D00a: Sprite #5 X position LSB */
    case 0xc:			/* $D00c: Sprite #6 X position LSB */
    case 0xe:			/* $D00e: Sprite #7 X position LSB */
      VIC_II_DEBUG_REGISTER (("\tSprite #%d X position LSB: $%02X\n",
			      addr >> 1, vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x1:			/* $D001: Sprite #0 Y position */
    case 0x3:			/* $D003: Sprite #1 Y position */
    case 0x5:			/* $D005: Sprite #2 Y position */
    case 0x7:			/* $D007: Sprite #3 Y position */
    case 0x9:			/* $D009: Sprite #4 Y position */
    case 0xb:			/* $D00B: Sprite #5 Y position */
    case 0xd:			/* $D00D: Sprite #6 Y position */
    case 0xf:			/* $D00F: Sprite #7 Y position */
      VIC_II_DEBUG_REGISTER (("\tSprite #%d Y position: $%02X\n",
                              addr >> 1, vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x10:			/* $D010: Sprite X position MSB */
      VIC_II_DEBUG_REGISTER (("\tSprite X position MSB: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x11:			/* $D011: video mode, Y scroll, 24/25 line mode
				   and raster MSB */
    case 0x12:			/* $D012: Raster line compare */
      {
	unsigned int tmp = read_raster_y ();

	VIC_II_DEBUG_REGISTER (("\tRaster Line register %s value = $%04X\n",
			      (addr == 0x11 ? "(highest bit) " : ""), tmp));

	if (addr == 0x11)
	  return (vic_ii.regs[addr] & 0x7f) | ((tmp & 0x100) >> 1);
	else
	  return tmp & 0xff;
      }

    case 0x13:			/* $D013: Light Pen X */
      VIC_II_DEBUG_REGISTER (("\tLight pen X: %d\n", vic_ii.light_pen.x));
      return vic_ii.light_pen.x;

    case 0x14:			/* $D014: Light Pen Y */
      VIC_II_DEBUG_REGISTER (("\tLight pen Y: %d\n", vic_ii.light_pen.y));
      return vic_ii.light_pen.y;

    case 0x15:			/* $D015: Sprite Enable */
      VIC_II_DEBUG_REGISTER (("\tSprite Enable register: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x16:			/* $D016 */
      VIC_II_DEBUG_REGISTER (("\t$D016 Control register read: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0xc0;

    case 0x17:			/* $D017: Sprite Y-expand */
      VIC_II_DEBUG_REGISTER (("\tSprite Y Expand register: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x18:			/* $D018: Video and char matrix base address */
      VIC_II_DEBUG_REGISTER (("\tVideo memory address register: $%02X\n",
			      vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0x1;

    case 0x19:			/* $D019: IRQ flag register */
      {
	BYTE tmp;

	tmp = read_d019 ();
	VIC_II_DEBUG_REGISTER (("\tInterrupt register: $%02X\n", tmp));

	return tmp;
      }

    case 0x1a:			/* $D01A: IRQ mask register  */
      VIC_II_DEBUG_REGISTER (("\tMask register: $%02X\n",
                              vic_ii.regs[addr] | 0xf0));
      return vic_ii.regs[addr] | 0xf0;

    case 0x1b:			/* $D01B: Sprite priority */
      VIC_II_DEBUG_REGISTER (("\tSprite Priority register: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x1c:			/* $D01C: Sprite Multicolor select */
      VIC_II_DEBUG_REGISTER (("\tSprite Multicolor Enable register: $%02X\n",
			      vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x1d:			/* $D01D: Sprite X-expand */
      VIC_II_DEBUG_REGISTER (("\tSprite X Expand register: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr];

    case 0x1e:			/* $D01E: Sprite-sprite collision */
      /* Remove the pending sprite-sprite interrupt, as the collision
         register is reset upon read accesses.  */
      if (!(vic_ii.irq_status & 0x3))
	{
	  vic_ii.irq_status &= ~0x84;
	  maincpu_set_irq (I_RASTER, 0);
	}
      else
	  vic_ii.irq_status &= ~0x04;

      if (vic_ii_resources.sprite_sprite_collisions_enabled)
	{
	  vic_ii.regs[addr] = vic_ii.sprite_sprite_collisions;
	  vic_ii.sprite_sprite_collisions = 0;
	  VIC_II_DEBUG_REGISTER (("\tSprite-sprite collision mask: $%02X\n",
				  vic_ii.regs[addr]));
	  return vic_ii.regs[addr];
	}
      else
	{
	  VIC_II_DEBUG_REGISTER (("\tSprite-sprite collision mask: $00 "
				  "(emulation disabled)\n"));
	  vic_ii.sprite_sprite_collisions = 0;
	  return 0;
	}

    case 0x1f:			/* $D01F: Sprite-background collision */
      /* Remove the pending sprite-background interrupt, as the collision
         register is reset upon read accesses.  */
      if (!(vic_ii.irq_status & 0x5))
	{
	  vic_ii.irq_status &= ~0x82;
	  maincpu_set_irq (I_RASTER, 0);
	}
      else
        vic_ii.irq_status &= ~0x2;

      if (vic_ii_resources.sprite_background_collisions_enabled)
	{
	  vic_ii.regs[addr] = vic_ii.sprite_background_collisions;
	  vic_ii.sprite_background_collisions = 0;
	  VIC_II_DEBUG_REGISTER (("\tSprite-background collision mask: "
                                  "$%02X\n",
				  vic_ii.regs[addr]));
#if defined (VIC_II_DEBUG_SB_COLLISIONS)
	  log_message (vic_ii.log,
		       "vic_ii.sprite_background_collisions reset by $D01F "
                       "read at line 0x%X.",
		       VIC_II_RASTER_Y (clk));
#endif
	  return vic_ii.regs[addr];
	}
      else
	{
	  VIC_II_DEBUG_REGISTER (("\tSprite-background collision mask: $00 "
				  "(emulation disabled)\n"));
	  vic_ii.sprite_background_collisions = 0;
	  return 0;
	}

    case 0x20:			/* $D020: Border color */
      VIC_II_DEBUG_REGISTER (("\tBorder Color register: $%02X\n",
                              vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0xf0;

    case 0x21:			/* $D021: Background #0 color */
    case 0x22:			/* $D022: Background #1 color */
    case 0x23:			/* $D023: Background #2 color */
    case 0x24:			/* $D024: Background #3 color */
      VIC_II_DEBUG_REGISTER (("\tBackground Color #%d register: $%02X\n",
			      addr - 0x21, vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0xf0;

    case 0x25:			/* $D025: Sprite multicolor register #0 */
    case 0x26:			/* $D026: Sprite multicolor register #1 */
      VIC_II_DEBUG_REGISTER (("\tMulticolor register #%d: $%02X\n",
			      addr - 0x22, vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0xf0;

    case 0x27:			/* $D027: Sprite #0 color */
    case 0x28:			/* $D028: Sprite #1 color */
    case 0x29:			/* $D029: Sprite #2 color */
    case 0x2a:			/* $D02A: Sprite #3 color */
    case 0x2b:			/* $D02B: Sprite #4 color */
    case 0x2c:			/* $D02C: Sprite #5 color */
    case 0x2d:			/* $D02D: Sprite #6 color */
    case 0x2e:			/* $D02E: Sprite #7 color */
      VIC_II_DEBUG_REGISTER (("\tSprite #%d color: $%02X\n",
			      addr - 0x22, vic_ii.regs[addr]));
      return vic_ii.regs[addr] | 0xf0;

    case 0x2f:			/* $D02F: Unused (or extended keyboard row
				   select) */
      if (vic_ii.extended_keyboard_rows_enabled)
	{
	  VIC_II_DEBUG_REGISTER (("\tExtended keyboard row enable: $%02X\n",
				  value));
	  return vic_ii.regs[addr];
	}
      else
	{
	  VIC_II_DEBUG_REGISTER (("\t(unused)\n"));
	  return 0xff;
	}
      break;

    case 0x30:			/* $D030: Unused */
    case 0x31:			/* $D031: Unused */
    case 0x32:			/* $D032: Unused */
    case 0x33:			/* $D033: Unused */
    case 0x34:			/* $D034: Unused */
    case 0x35:			/* $D035: Unused */
    case 0x36:			/* $D036: Unused */
    case 0x37:			/* $D037: Unused */
    case 0x38:			/* $D038: Unused */
    case 0x39:			/* $D039: Unused */
    case 0x3a:			/* $D03A: Unused */
    case 0x3b:			/* $D03B: Unused */
    case 0x3c:			/* $D03C: Unused */
    case 0x3d:			/* $D03D: Unused */
    case 0x3e:			/* $D03E: Unused */
    case 0x3f:			/* $D03F: Unused */
      return 0xff;

    default:
      return 0xff;
    }
}



BYTE REGPARM1 
peek_vic (ADDRESS addr)
{
  addr &= 0x3f;
  switch (addr)
    {
    case 0x11:			/* $D011: video mode, Y scroll, 24/25 line mode
				   and raster MSB */
      return (vic_ii.regs[addr] & 0x7f) | ((read_raster_y () & 0x100) >> 1);
    case 0x12:			/* $D012: Raster line LSB */
      return read_raster_y () & 0xff;
    case 0x13:			/* $D013: Light Pen X */
      return vic_ii.light_pen.x;
    case 0x14:			/* $D014: Light Pen Y */
      return vic_ii.light_pen.y;
    case 0x19:
      return read_d019 ();
    case 0x1e:			/* $D01E: Sprite-sprite collision */
      return vic_ii.sprite_sprite_collisions;
    case 0x1f:			/* $D01F: Sprite-background collision */
      return vic_ii.sprite_background_collisions;
    case 0x2f:			/* Extended keyboard row select */
      if (vic_ii.extended_keyboard_rows_enabled)
	return vic_ii.regs[addr] | 0xf8;
      else
	return vic_ii.regs[addr] | 0xff;
    default:
      return vic_ii.regs[addr] | unused_bits_in_registers[addr];
    }
}



void REGPARM2 
store_colorram (ADDRESS addr, BYTE value)
{
  vic_ii.color_ram[addr & 0x3ff] = value & 0xf;
}

BYTE REGPARM1 
read_colorram (ADDRESS addr)
{
  return vic_ii.color_ram[addr & 0x3ff] | (rand () & 0xf0);
}
