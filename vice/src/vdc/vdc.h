/*
 * vdc.h - A first attempt at a MOS8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner   (markus@brenner.de)
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

#ifndef _VDC_H
#define _VDC_H

#include "vice.h"

#include "alarm.h"
#include "log.h"
#include "raster.h"
#include "snapshot.h"

#include "vdc-mem.h"

/* Screen constants.  */
/* Not exact, but for now allow 16 pixels of border each */

#define VDC_SCREEN_WIDTH		672
#define VDC_SCREEN_HEIGHT	        232

#define VDC_SCREEN_XPIX			640
#define VDC_SCREEN_YPIX			200
#define VDC_SCREEN_TEXTCOLS		80
#define VDC_SCREEN_TEXTLINES        	25
#define VDC_SCREEN_BORDERWIDTH		16
#define VDC_SCREEN_BORDERHEIGHT     	16
#define VDC_SCREEN_CHARHEIGHT		8

#define VDC_FIRST_DISPLAYED_LINE	16
#define VDC_LAST_DISPLAYED_LINE		216
#define VDC_25ROW_START_LINE		16
#define VDC_25ROW_STOP_LINE		216
#define VDC_80COL_START_PIXEL		16
#define VDC_80COL_STOP_PIXEL		656

#define VDC_NUM_SPRITES			0
#define VDC_NUM_COLORS		        16


/* Address mask for VDC range (64kB VDC-RAM) */

#define VDC_ADDRESS_MASK		0xffff


/* VDC Attribute masks */

#define VDC_FLASH_ATTR			0x10
#define VDC_UNDERLINE_ATTR		0x20
#define VDC_REVERSE_ATTR		0x40
#define VDC_ALTCHARSET_ATTR		0x80

/* Available video modes. */
enum _vdc_video_mode
  {
    VDC_TEXT_MODE,
    VDC_BITMAP_MODE,
    VDC_IDLE_MODE,
    VDC_NUM_VMODES
  };
typedef enum vdc_video_mode vdc_video_mode_t;

#define VDC_IS_ILLEGAL_MODE(x)	((x) >= VDC_ILLEGAL_TEXT_MODE \
					 && (x) != VDC_IDLE_MODE)
#define VDC_IS_BITMAP_MODE(x)	((x) & 0x02)



/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#if(!defined(__MSDOS__) && !defined(__riscos) && !defined(OS2))
#define VDC_NEED_2X 1
#endif



/* VDC structures.  This is meant to be used by VDC modules
   *exclusively*!  */

struct _vdc_light_pen
  {
    int triggered;
    int x, y;
  };
typedef struct _vdc_light_pen vdc_light_pen_t;


struct _vdc
  {
    /* Flag: Are we initialized?  */
    int initialized;		/* = 0; */

    /* VDC raster.  */
    raster_t raster;

    /* VDC registers.  */
    int regs[38];

    /* Internal VDC video memory */
    BYTE ram[0x10000];

    /* Internal VDC register pointer */
    int update_reg;

    /* Video memory offsets.  */
    unsigned int screen_adr;
    unsigned int cursor_adr;
    unsigned int update_adr;
    unsigned int attribute_adr;
    unsigned int chargen_adr;

    /* Internal memory counter. */
    int mem_counter;
    int bitmap_counter;

    /* Value to add to `mem_counter' after the graphics has been painted.  */
    int mem_counter_inc;

    /* Flag: Does the currently selected video mode force the overscan
       background color to be black?  (This happens with the hires bitmap and
       illegal modes.)  */
    int force_black_overscan_background_color;

    /* All the VDC logging goes here.  */
    log_t log;		/* = LOG_ERR; */

    /* VDC alarms.  */
    /* Alarm to update a raster line. */
    alarm_t raster_draw_alarm;
  };
typedef struct _vdc vdc_t;

extern vdc_t vdc;


int vdc_init_resources(void);
int vdc_init_cmdline_options (void);
canvas_t vdc_init (void);
void vdc_reset (void);
void vdc_exposure_handler (unsigned int width, unsigned int height);
void vdc_prepare_for_snapshot (void);
void vdc_powerup (void);
void vdc_resize (void);

int vdc_write_snapshot_module (snapshot_t *s);
int vdc_read_snapshot_module (snapshot_t *s);

void video_free (void);
#if 0				/*  def USE_VIDMODE_EXTENSION */
void video_setfullscreen (int v, int width, int height);
#endif


/* Private function calls, used by the other VDC modules.  FIXME:
   Prepend names with `_'?  */
int vdc_load_palette (const char *name);
void vdc_fetch_matrix (int offs, int num);
void vdc_update_memory_ptrs (unsigned int cycle);
void vdc_update_video_mode (unsigned int cycle);
int vdc_raster_draw_alarm_handler (long offset);


#endif /* _VDC_H */
