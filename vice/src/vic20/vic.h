/*
 * vic.h - A VIC-I emulation (under construction)
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * 16/24bpp support added by
 *  Steven Tieu (stieu@physics.ubc.ca)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#ifndef _VIC_H
#define _VIC_H

#include "vice.h"

#include "alarm.h"
#include "log.h"
#include "snapshot.h"
#include "vic20.h"
#include "video.h"

#include "raster.h"

#include "vic-mem.h"



#define VIC_SCREEN_WIDTH                233
#define VIC_SCREEN_HEIGHT               312
    
#define VIC_SCREEN_MAX_TEXT_COLS        31
#define VIC_SCREEN_MAX_TEXT_LINES       30
    
#define VIC_FIRST_DISPLAYED_LINE        28
#define VIC_LAST_DISPLAYED_LINE         293

#define VIC_NUM_SPRITES 0

#define VIC_NUM_COLORS 16



/* On MS-DOS, do not duplicate pixels.  Otherwise, we would always need at
   least 466 horizontal pixels to contain the whole screen.  */
#ifndef __MSDOS__
#define VIC_DUPLICATES_PIXELS
#endif

#ifdef VIC_DUPLICATES_PIXELS
typedef PIXEL2 VIC_PIXEL;
#define VIC_PIXEL(n)    RASTER_PIXEL2 (&vic.raster, n)
typedef PIXEL4 VIC_PIXEL2;
#define VIC_PIXEL2(n)   RASTER_PIXEL4 (&vic.raster, n)
#define VIC_PIXEL_WIDTH 2
#else
typedef PIXEL VIC_PIXEL;
#define VIC_PIXEL(n)    RASTER_PIXEL (&vic.raster, n)
typedef PIXEL2 VIC_PIXEL2;
#define VIC_PIXEL2(n)   RASTER_PIXEL2 (&vic.raster, n)
#define VIC_PIXEL_WIDTH 1
#endif



#define VIC_CYCLES_PER_LINE VIC20_PAL_CYCLES_PER_LINE

/* Cycle # within the current line.  */
#define VIC_RASTER_CYCLE(clk) ((int)((clk) % VIC_CYCLES_PER_LINE))

/* Current vertical position of the raster.  Unlike `rasterline', which is
   only accurate if a pending `A_RASTERDRAW' event has been served, this is
   guarranteed to be always correct.  It is a bit slow, though.  */
#define VIC_RASTER_Y(clk)     ((int)((clk) / VIC_CYCLES_PER_LINE)   \
                               % VIC_SCREEN_HEIGHT)



/* Video mode definitions. */

enum _vic_video_mode
  {
    VIC_STANDARD_MODE,
    VIC_REVERSE_MODE,
    VIC_NUM_VMODES,
  };
typedef enum _vic_video_mode vic_video_mode_t;

#define VIC_IDLE_MODE VIC_STANDARD_MODE



/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#if(!defined(__MSDOS__) && !defined(__riscos) && !defined(OS2))
#define VIC_NEED_2X 1
#endif



struct _vic
  {
    int initialized;

    log_t log;

    raster_t raster;

    palette_t *palette;

    BYTE regs[64];

    alarm_t raster_draw_alarm;

    BYTE auxiliary_color;
    BYTE *color_ptr;
    BYTE *screen_ptr;
    BYTE *chargen_ptr; /* = chargen_rom + 0x400; */

    unsigned int char_height;   /* = 8 */
    unsigned int text_cols;     /* = 22 */
    unsigned int text_lines;    /* = 23 */

    unsigned int memptr;
  };
typedef struct _vic vic_t;

extern vic_t vic;



canvas_t vic_init (void);
void vic_reset (void);

int vic_init_resources (void);
int vic_init_cmdline_options (void);

int vic_write_snapshot_module (snapshot_t *s);
int vic_read_snapshot_module (snapshot_t *s);



/* Private function calls, used by the other VIC modules.  FIXME:
   Prepend names with `_'?  */
void vic_exposure_handler (unsigned int width, unsigned int height);
void vic_update_memory_ptrs (void);
int vic_load_palette (const char *name);
void vic_resize (void);



/* Debugging options.  */

/* #define VIC_RASTER_DEBUG */
/* #define VIC_REGISTERS_DEBUG */

#ifdef VIC_RASTER_DEBUG
#define VIC_DEBUG_RASTER(x) log_debug x
#else
#define VIC_DEBUG_RASTER(x)
#endif

#ifdef VIC_REGISTERS_DEBUG
#define VIC_DEBUG_REGISTER(x) log_debug x
#else
#define VIC_DEBUG_REGISTER(x)
#endif

#endif /* _VIC_H */
