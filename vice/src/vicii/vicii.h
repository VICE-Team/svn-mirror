/*
 * vicii.h - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
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

#ifndef _VICII_H
#define _VICII_H

#include "vice.h"

#include "c64.h"
#include "maincpu.h"
#include "raster.h"
#include "snapshot.h"
#include "video.h"

#include "vicii-mem.h"

/* Screen constants.  */

#define VIC_II_SCREEN_HEIGHT	        312

#if 0
#define VIC_II_SCREEN_WIDTH		411
#else
/* Not the real size, but more than enough for everything.  Emulating a
   smaller screen makes it faster.  */
#define VIC_II_SCREEN_WIDTH		384
#endif

/* FIXME: Provide NTSC versions.  */

#define VIC_II_SCREEN_XPIX		320
#define VIC_II_SCREEN_YPIX		200
#define VIC_II_SCREEN_TEXTCOLS		40
#define VIC_II_SCREEN_TEXTLINES        	25
#define VIC_II_SCREEN_BORDERWIDTH	32
#define VIC_II_SCREEN_BORDERHEIGHT     	51
#define VIC_II_SCREEN_CHARHEIGHT	8

#define VIC_II_FIRST_DISPLAYED_LINE	0x10
#define VIC_II_LAST_DISPLAYED_LINE	0x11f
#define VIC_II_25ROW_START_LINE		0x33
#define VIC_II_25ROW_STOP_LINE		0xfb
#define VIC_II_24ROW_START_LINE		0x37
#define VIC_II_24ROW_STOP_LINE		0xf7
#define VIC_II_40COL_START_PIXEL	0x20
#define VIC_II_40COL_STOP_PIXEL		0x160
#define VIC_II_38COL_START_PIXEL	0x27
#define VIC_II_38COL_STOP_PIXEL		0x157

#define VIC_II_NUM_SPRITES		8
#define VIC_II_MAX_SPRITE_WIDTH		48
#define VIC_II_SPRITE_WRAP_X		504
#define VIC_II_NUM_COLORS	        16



/* Available video modes.  The number is given by
   ((vic_ii.regs[0x11] & 0x60) | (vic_ii.regs[0x16] & 0x10)) >> 4.  */
enum _vic_ii_video_mode
  {
    VIC_II_NORMAL_TEXT_MODE,
    VIC_II_MULTICOLOR_TEXT_MODE,
    VIC_II_HIRES_BITMAP_MODE,
    VIC_II_MULTICOLOR_BITMAP_MODE,
    VIC_II_EXTENDED_TEXT_MODE,
    VIC_II_ILLEGAL_TEXT_MODE,
    VIC_II_ILLEGAL_BITMAP_MODE_1,
    VIC_II_ILLEGAL_BITMAP_MODE_2,
    VIC_II_IDLE_MODE,		/* Special mode for idle state.  */
    VIC_II_NUM_VMODES
  };
typedef enum vic_ii_video_mode vic_ii_video_mode_t;

#define VIC_II_IS_ILLEGAL_MODE(x)	((x) >= VIC_II_ILLEGAL_TEXT_MODE \
					 && (x) != VIC_II_IDLE_MODE)
#define VIC_II_IS_BITMAP_MODE(x)	((x) & 0x02)



/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#if(!defined(__MSDOS__) && !defined(__riscos) && !defined(OS2))
#define VIC_II_NEED_2X 1
#endif



/* These timings are taken from the ``VIC Article'' by Christian Bauer
   (bauec002@goofy.zdv.uni-mainz.de).  Thanks Christian!
   Note: we measure cycles from 0 to 62, not from 1 to 63 as he does.  */

/* Number of cycles per line.  */
#define VIC_II_CYCLES_PER_LINE      C64_PAL_CYCLES_PER_LINE

/* Cycle # at which the VIC takes the bus in a bad line (BA goes low).  */
#define	VIC_II_FETCH_CYCLE          11

/* Cycle # at which sprite DMA is set.  */
#define VIC_II_SPRITE_FETCH_CYCLE   54

/* Cycle # at which the current raster line is re-drawn.  It is set to
   `VIC_II_CYCLES_PER_LINE', so this actually happens at the very beginning
   (i.e. cycle 0) of the next line.  */
#define VIC_II_DRAW_CYCLE           VIC_II_CYCLES_PER_LINE

/* Delay for the raster line interrupt.  This is not due to the VIC-II, since
   it triggers the IRQ line at the beginning of the line, but to the 6510
   that needs at least 2 cycles to detect it.  */
#define VIC_II_RASTER_IRQ_DELAY     2

/* Current char being drawn by the raster.  < 0 or >= VIC_II_SCREEN_TEXTCOLS
   if outside the visible range.  */
#define VIC_II_RASTER_CHAR(cycle)   ((int)(cycle) - 15)

/* Current horizontal position (in pixels) of the raster.  < 0 or >=
   SCREEN_WIDTH if outside the visible range.  */
#define VIC_II_RASTER_X(cycle)      (((int)(cycle) - 13) * 8)

/* Current vertical position of the raster.  Unlike `rasterline', which is
   only accurate if a pending drawing event has been served, this is
   guarranteed to be always correct.  It is a bit slow, though.  */
#define VIC_II_RASTER_Y(clk)        ((unsigned int)((clk) \
                                                    / VIC_II_CYCLES_PER_LINE) \
                                     % VIC_II_SCREEN_HEIGHT)

/* Cycle # within the current line.  */
#define VIC_II_RASTER_CYCLE(clk)    ((unsigned int)((clk) \
                                                    % VIC_II_CYCLES_PER_LINE))

/* `clk' value for the beginning of the current line.  */
#define VIC_II_LINE_START_CLK(clk)  (((clk) / VIC_II_CYCLES_PER_LINE) \
                                     * VIC_II_CYCLES_PER_LINE)

/* # of the previous and next raster line.  Handles wrap over.  */
#define VIC_II_PREVIOUS_LINE(line)  (((line) > 0) \
                                     ? (line) - 1 : VIC_II_SCREEN_HEIGHT - 1)
#define VIC_II_NEXT_LINE(line)      (((line) + 1) % VIC_II_SCREEN_HEIGHT)

/* Bad line range.  */
#define VIC_II_FIRST_DMA_LINE       0x30
#define VIC_II_LAST_DMA_LINE        0xf7



/* VIC-II structures.  This is meant to be used by VIC-II modules
   *exclusively*!  */

struct _vic_ii_light_pen
  {
    int triggered;
    int x, y;
  };
typedef struct _vic_ii_light_pen vic_ii_light_pen_t;

enum _vic_ii_fetch_idx
  {
    VIC_II_FETCH_MATRIX,
    VIC_II_CHECK_SPRITE_DMA,
    VIC_II_FETCH_SPRITE
  };
typedef enum _vic_ii_fetch_idx vic_ii_fetch_idx_t;

enum _vic_ii_idle_data_location
  {
    IDLE_NONE,
    IDLE_3FFF,
    IDLE_39FF
  };
typedef enum _vic_ii_idle_data_location vic_ii_idle_data_location_t;

struct _vic_ii
  {
    /* Flag: Are we initialized?  */
    int initialized;		/* = 0; */

    /* VIC-II raster.  */
    raster_t raster;

    /* VIC-II registers.  */
    int regs[64];

    /* Interrupt register.  */
    int irq_status;		/* = 0; */

    /* Line for raster compare IRQ.  */
    unsigned int raster_irq_line;

    /* Internal color memory.  */
    BYTE color_ram[0x400];

    /* Pointer to the base of RAM seen by the VIC-II.  */
    BYTE *ram_base;		/* = ram; */

    /* Video memory pointers.  */
    BYTE *screen_ptr;
    BYTE *chargen_ptr;
    BYTE *bitmap_ptr;

    /* Screen memory buffers (chars and color).  */
    BYTE vbuf[VIC_II_SCREEN_TEXTCOLS];
    BYTE cbuf[VIC_II_SCREEN_TEXTCOLS];

    /* If this flag is set, bad lines (DMA's) can happen.  */
    int allow_bad_lines;

    /* Sprite-sprite and sprite-background collision registers.  */
    BYTE sprite_sprite_collisions;
    BYTE sprite_background_collisions;

    /* Extended background colors (1, 2 and 3).  */
    int ext_background_color[3];

    /* Flag: are we in idle state? */
    int idle_state;

    /* Flag: should we force display (i.e. non-idle) state for the following
       line? */
    int force_display_state;

    /* This flag is set if a memory fetch has already happened on the current
       line.  FIXME: Value of 2?...  */
    int memory_fetch_done;

    /* Internal memory pointer (VCBASE).  */
    int memptr;

    /* Internal memory counter (VC).  */
    int mem_counter;

    /* Value to add to `mem_counter' after the graphics has been painted.  */
    int mem_counter_inc;

    /* Flag: is the current line a `bad' line? */
    int bad_line;

    /* Flag: Check for raster.ycounter reset already done on this line?
       (cycle 13) */
    int ycounter_reset_checked;

    /* Flag: Does the currently selected video mode force the overscan
       background color to be black?  (This happens with the hires bitmap and
       illegal modes.)  */
    int force_black_overscan_background_color;

    /* Light pen.  */
    vic_ii_light_pen_t light_pen;

    /* Start of the memory bank seen by the VIC-II.  */
    int vbank;			/* = 0; */

    /* Pointer to the start of the video bank.  */
    BYTE *vbank_ptr;

    /* Data to display in idle state.  */
    int idle_data;

    /* Where do we currently fetch idle stata from?  If `IDLE_NONE', we are
       not in idle state and thus do not need to update `idle_data'.  */
    vic_ii_idle_data_location_t idle_data_location;

    /* Flag: Are the C128 extended keyboard rows enabled?  */
    int extended_keyboard_rows_enabled;

    /* All the VIC-II logging goes here.  */
    log_t log;			/* = LOG_ERR; */

    /* VIC-II alarms.  */
    alarm_t raster_fetch_alarm;
    alarm_t raster_draw_alarm;
    alarm_t raster_irq_alarm;

    /* What do we do when the `A_RASTERFETCH' event happens?  */
    vic_ii_fetch_idx_t fetch_idx;

    /* Number of sprite being DMA fetched.  */
    unsigned int sprite_fetch_idx;

    /* Mask for sprites being fetched at DMA.  */
    unsigned int sprite_fetch_msk;

    /* Clock cycle for the next "raster fetch" alarm.  */
    CLOCK fetch_clk;

    /* Clock cycle for the next "raster draw" alarm.  */
    CLOCK draw_clk;

    /* Clock value for raster compare IRQ.  */
    CLOCK raster_irq_clk;

    /* FIXME: Bad name.  FIXME: Has to be initialized.  */
    CLOCK last_emulate_line_clk;

    /* Clock cycle for the next sprite fetch.  */
    CLOCK sprite_fetch_clk;
  };
typedef struct _vic_ii vic_ii_t;

extern vic_ii_t vic_ii;



int vic_ii_init_resources (void);
int vic_ii_init_cmdline_options (void);
canvas_t vic_ii_init (void);
void vic_ii_reset (void);
void vic_ii_exposure_handler (unsigned int width, unsigned int height);
void vic_ii_trigger_light_pen (CLOCK mclk);
void vic_ii_set_vbank (int new_vbank);
void vic_ii_set_ram_base (BYTE * base);
void vic_ii_enable_extended_keyboard_rows (int flag);
void vic_ii_prepare_for_snapshot (void);
void vic_ii_powerup (void);
void vic_ii_resize (void);

int vic_ii_write_snapshot_module (snapshot_t *s);
int vic_ii_read_snapshot_module (snapshot_t *s);

void video_free (void);
#if 0				/*  def USE_VIDMODE_EXTENSION */
void video_setfullscreen (int v, int width, int height);
#endif



/* Private function calls, used by the other VIC-II modules.  FIXME:
   Prepend names with `_'?  */
int vic_ii_load_palette (const char *name);
void vic_ii_fetch_matrix (int offs, int num);
void vic_ii_set_raster_irq (unsigned int line);
void vic_ii_update_memory_ptrs (unsigned int cycle);
void vic_ii_update_video_mode (unsigned int cycle);
int vic_ii_raster_draw_alarm_handler (long offset);
int vic_ii_raster_fetch_alarm_handler (long offset);
int vic_ii_raster_irq_alarm_handler (long offset);



/* Debugging options.  */

/* #define VIC_II_VMODE_DEBUG */
/* #define VIC_II_RASTER_DEBUG */
/* #define VIC_II_REGISTERS_DEBUG */

#ifdef VIC_II_VMODE_DEBUG
#define VIC_II_DEBUG_VMODE(x) log_debug x
#else
#define VIC_II_DEBUG_VMODE(x)
#endif

#ifdef VIC_II_RASTER_DEBUG
#define VIC_II_DEBUG_RASTER(x) log_debug x
#else
#define VIC_II_DEBUG_RASTER(x)
#endif

#ifdef VIC_II_REGISTERS_DEBUG
#define VIC_II_DEBUG_REGISTER(x) log_debug x
#else
#define VIC_II_DEBUG_REGISTER(x)
#endif



inline static void
vic_ii_handle_pending_alarms (int num_write_cycles)
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
	      vic_ii_raster_draw_alarm_handler (clk - vic_ii.draw_clk);
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

#endif /* _VICII_H */
