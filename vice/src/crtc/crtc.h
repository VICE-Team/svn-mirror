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

#define	CRTC_NUM_COLORS	2



typedef void (*machine_crtc_retrace_signal_t)(unsigned int);

struct _crtc
  {
    /* Flag: Are we initialized?  */
    int initialized;

    /*---------------------------------------------------------------*/

    /* window size computed by crtc_set_screen_options() */
    int screen_width;
    int screen_height;

    /* hardware options as given to crtc_set_hw_options() */
    int hw_cursor;
    int hw_double_cols;
    int vaddr_mask;
    int vaddr_charswitch;
    int vaddr_charoffset;
    int vaddr_revswitch;

    /* screen and charset memory options (almost) as given to 
       crtc_set_chargen_addr() and crtc_set_screen_addr() */
    BYTE *screen_base;
    BYTE *chargen_base;
    int chargen_mask;
    int chargen_offset;

    /* those values are derived */
    int chargen_rel;	/* currently used charset rel. to chargen_base */
    int screen_rel;	/* current screen line rel. to screen_base */

    /* internal CRTC state variables */

    int regno;		/* current register selected with store to addr 0 */

    /* The alarm handler is called in the last cycles of a rasterline.
       Some effects need better timing, though */

    /* rasterline variables */
    CLOCK rl_start;	/* clock when the current rasterline starts */
    int rl_visible;	/* number of visible chars in this line */
    int rl_sync;	/* character in line when the sync starts */
    int rl_len;		/* length of line in cycles */

    /* values of the previous rasterline */
    int prev_rl_visible;
    int prev_rl_sync;
    int prev_rl_len;

    int xoffset;	/* pixel-offset of current rasterline */

    int henable;	/* flagged when horizontal enable flipflop has not
			   been reset in line */

    int venable;	/* flagged when vertical enable flipflop has not
			   been reset in frame */

    int cursor_lines;	/* flagged when rasterline within hw cursor lines */

    int disp_chars;	/* number of displayed chars in current rasterline 
			   (may be disp_cycles or double the value 
			   (hw_double_cols) */

    int current_charline; /* state of the current character line counter */

    int visible_line;	/* flagged when VDispEnable active */

    /*---------------------------------------------------------------*/

    /* this is the function to be called when the retrace signal
       changes. type&1=0: old PET, type&1=1: CRTC-PET. retrace_type 
       Also used by crtc_offscreen() */
    machine_crtc_retrace_signal_t retrace_callback;
    int retrace_type;

    /*---------------------------------------------------------------*/

    /* All the CRTC logging goes here.  */
    log_t log;

    /* CRTC raster.  */
    raster_t raster;

    /* CRTC registers.  */
    int regs[64];

    /* Alarm to update a raster line.  */
    alarm_t raster_draw_alarm;
  };
typedef struct _crtc crtc_t;

extern crtc_t crtc;

#define CRTC_SCREEN_BORDERWIDTH  25
#define CRTC_SCREEN_BORDERHEIGHT 25



canvas_t crtc_init (void);
void crtc_reset (void);

int crtc_init_resources (void);
int crtc_init_cmdline_options (void);

int crtc_write_snapshot_module(snapshot_t *s);
int crtc_read_snapshot_module(snapshot_t *s);

void crtc_set_screen_addr(BYTE *screen);
void crtc_set_chargen_offset(int offset);
void crtc_set_chargen_addr(BYTE *chargen, int cmask);
void crtc_set_screen_options(int num_cols, int rasterlines);
void crtc_set_hw_options(int hwflag, int vmask, int vchar, int vcoffset,
                                                                int vrevmask);
void crtc_set_retrace_callback(machine_crtc_retrace_signal_t callback, 
								int type);
void crtc_screen_enable(int);

int crtc_offscreen(void);

/* FIXME: when interface has settled */
#define	crtc_set_char(a)	crtc_set_chargen_offset((a)?256:0)




/* Private function calls, used by the other VIC-II modules.  FIXME:
   Prepend names with `_'?  */
int crtc_load_palette (const char *name);
void crtc_resize (void);
void crtc_exposure_handler (unsigned int width, unsigned int height);
int crtc_raster_draw_alarm_handler (long offset);

#endif                          /* _CRTC_H */
