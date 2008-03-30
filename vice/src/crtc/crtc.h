/*
 * crtc.h - A CRTC emulation (under construction)
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _CRTC_H
#define _CRTC_H

#include "vice.h"

#include "alarm.h"
#include "log.h"
#include "raster.h"
#include "snapshot.h"
#include "crtc-mem.h"



/* Border around the crtc screen in the window.  We do not have border
   effects, so keep it small.  */
#define CRTC_SCREEN_BORDER 12

/* FIXME  */
#define CRTC_CYCLES_PER_LINE 63

#define CRTC_NUM_COLORS 2



/* On MS-DOS, we do not need 2x drawing functions.  This is mainly to save
   memory and (little) speed.  */
#if(!defined(__MSDOS__) && !defined(__riscos) && !defined(OS2))
#define CRTC_NEED_2X 1
#endif



enum _crtc_video_mode
  {
    CRTC_STANDARD_MODE,
    CRTC_REVERSE_MODE,
    CRTC_NUM_VMODES
  };
typedef enum _crtc_video_mode crtc_video_mode_t;

#define CRTC_IDLE_MODE CRTC_STANDARD_MODE



struct _crtc
  {
    /* Flag: Are we initialized?  */
    int initialized;

    /* All the CRTC logging goes here.  */
    log_t log;

    /* CRTC raster.  */
    raster_t raster;

    /* CRTC registers.  */
    int regs[64];

    /* Internal memory counter.  */
    int mem_counter;

    /* Alarm to update a raster line.  */
    alarm_t raster_draw_alarm;
  };
typedef struct _crtc crtc_t;

extern crtc_t crtc;



canvas_t crtc_init (void);
void crtc_reset (void);

int crtc_init_resources (void);
int crtc_init_cmdline_options (void);

int crtc_write_snapshot_module(snapshot_t *s);
int crtc_read_snapshot_module(snapshot_t *s);

void crtc_set_char(int crom);
int crtc_offscreen(void);
void crtc_screen_enable(int);




/* Private function calls, used by the other VIC-II modules.  FIXME:
   Prepend names with `_'?  */
int crtc_load_palette (const char *name);
void crtc_resize (void);
void crtc_exposure_handler (unsigned int width, unsigned int height);
void crtc_set_screen_mode (BYTE *base, int vmask, int cols, int hwcrsr);
int crtc_raster_draw_alarm_handler (long offset);

#endif                          /* _CRTC_H */
