/*
 * crtc.c - A line-based CRTC emulation (under construction).
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

#define _CRTC_C

#include "vice.h"

#include "machine.h"
#include "maincpu.h"
#include "utils.h"
#include "vsync.h"

#include "crtc.h"
#include "crtc-cmdline-options.h"
#include "crtc-draw.h"
#include "crtc-resources.h"

/*--------------------------------------------------------------------*/

#define	crtc_min(a,b)	(((a)<(b))?(a):(b))

/*--------------------------------------------------------------------*/
/* CRTC variables */

/* the first variable is the initialized flag. We don´t want that be
   uninitialized... */
crtc_t crtc = { 
	0, 		/* initialized */

	340,		/* screen_width */
	270,		/* screen_heigth */

	0,		/* hw_cursor */
	0,		/* hw_double_cols */
	0x3ff,		/* vaddr_mask */
	0x800,		/* vaddr_charswitch */
	512,		/* vaddr_charoffset */
	0x1000		/* vaddr_revswitch */
};

/* crtc-struct access functions */
#define	CRTC_SCREEN_ADDR() \
		((crtc.regs[11] | (crtc.regs[12] << 8)) & 0x3fff)

#define	CRTC_SCREEN_WIDTH() \
		crtc.screen_width
#define	CRTC_SCREEN_HEIGHT() \
		crtc.screen_height

#define	CRTC_SCREEN_TEXTCOLS() \
		(crtc.regs[1] << (crtc.hw_double_cols ? 1 : 0))
#define	CRTC_SCREEN_TEXTLINES() \
		(crtc.regs[6] & 0x7f)

#define	CRTC_SCREEN_XPIX() \
		(CRTC_SCREEN_TEXTCOLS() << 3)
#define	CRTC_SCREEN_YPIX() \
		(CRTC_SCREEN_TEXTLINES() * (crtc_min(16, crtc.regs[9] + 1)))

#define CRTC_FIRST_DISPLAYED_LINE \
		CRTC_SCREEN_BORDERHEIGHT
#define CRTC_LAST_DISPLAYED_LINE \
		(CRTC_FIRST_DISPLAYED_LINE + CRTC_SCREEN_YPIX())

#define	CRTC_CYCLES_PER_LINE() \
		crtc.regs[0]

/*--------------------------------------------------------------------*/
/* size/mode handling */
/*
 * So far we changed the window size according to the values poked
 * to the CRTC. Now we keep the window size fixed and try to position 
 * the character array in there.
 *
 * The external hardware allows for a number of options.
 * Those are set with 
 *
 *    crtc_set_hw_options(int hwflag, int vmask, int vchar, int vcoffset,
							int vrevmask);
 * 	hwflag & 1 -> hardware cursor available
 * 	hwflag & 2 -> each CRTC character accounts for two chars on the 
 *	              screen (hw_double_cols)
 *	vmask      -> the valid bits for the CRTC screen address (screen
 *	              buffer wraparound)
 *	vmchar     -> bit in CRTC screen address to switch to alternate
 *	              (second) charset
 *	vcoffset   -> how many chars the alternate charset of away
 *	              (CBM default: 512, because the charsets a 256 chars
 *	              for graphics/lowercase are switched elsewhere)
 *	vrevmask   -> bit in CRTC screen address to invert screen 
 *
 * The screen that is attached to the CRTC can have different capabilities.
 * This function sets the expected size of the pixel area that is to 
 * be used. Poking to the CRTC registers might change that, but the 
 * video code positions the area in the window given here.
 *
 *    crtc_set_screen_options(int num_cols, int rasterlines);
 *	num_cols   -> 40 or 80
 *	rasterlines-> number of (text data) rasterlines (25*8, 25*14)
 *
 * 
 * The CRTC memory and charset can be changed by the CPU. Those 
 * functions tell the CRTC about it. The charset is always 
 * organized as 16 raster bytes per char, one char after the other.
 *
 *    crtc_set_chargen_addr(BYTE *chargen, int cmask);
 *	chargen    -> pointer to base of charset
 *	clen       -> length of charset in chars (must be power of 2)
 *    crtc_set_chargen_offset(int offset);
 *	offset     -> offset of current charset in chargen, measured in chars
 *    crtc_set_screen_addr(BYTE *screen);
 *	screen    -> pointer to base of screen character array 
 *
 * The above functions set the appropriate fields in the CRTC struct
 * and then call crtc_update_*().
 * The update functions check whether the CRTC is already initialized
 * and only then perform the appropriate action.
 * The update functions are also called in crtc_init() to 
 * finish any resize/mode settings being made from the resources.
 *
 * Internal CRTC screen pointer handling:
 *
 * We assume that the screen address is increased every rasterline.
 * Only at frame reset (rasterline 0) the value is reloaded and 
 * changes to the screen base register have effect.
 * This effects the selection of the chargen as well as the 
 * screen pointer itself, also the mode selection.
 */
/*--------------------------------------------------------------------*/

/* reset the screen pointer at the beginning of the screen */
static void inline crtc_reset_screen_ptr(void) 
{
    if (!crtc.initialized)
	return;

    crtc.screen_rel = ((CRTC_SCREEN_ADDR() & crtc.vaddr_mask)
			<< (crtc.hw_double_cols ? 1 : 0));

    crtc.chargen_rel = (((CRTC_SCREEN_ADDR() & crtc.vaddr_charswitch)
			    ? crtc.vaddr_charoffset : 0)
			  | crtc.chargen_offset)
			& crtc.chargen_mask;

    if ( (crtc.vaddr_revswitch & crtc.vaddr_mask) 
	|| ((crtc.vaddr_revswitch < 0) 
		&& !(CRTC_SCREEN_ADDR() & (-crtc.vaddr_revswitch)))
	|| ((crtc.vaddr_revswitch > 0) 
		&& (CRTC_SCREEN_ADDR() & crtc.vaddr_revswitch)))
    {
	/* standard mode */
    } else {
	/* reverse mode */
    }

    /* have they changed? */
    crtc.raster.display_ystop = CRTC_SCREEN_YPIX();
    crtc.raster.display_xstop = CRTC_SCREEN_XPIX();

}

/* update the chargen pointer when external switch has changed */
static void inline crtc_update_chargen_rel(void) 
{
    if (!crtc.initialized)
	return;

    crtc.chargen_rel = ((crtc.chargen_rel & crtc.vaddr_charoffset)
			  | crtc.chargen_offset)
			& crtc.chargen_mask;
}

/* update memptr_inc after writing to register 1 */
static void inline crtc_update_memptr_inc(void) 
{
    if (!crtc.initialized)
	return;

    crtc.memptr_inc = (crtc.regs[1] << (crtc.hw_double_cols ? 1 : 0));
}

/* update screen window */
static void inline crtc_update_screen(void)
{
    if (!crtc.initialized)
	return;

    /* FIXME */
}
 
/*--------------------------------------------------------------------*/

void crtc_set_screen_addr(BYTE *screen)
{
    crtc.screen_base = screen;
}

void crtc_set_chargen_offset(int offset)
{
    crtc.chargen_offset = offset << 4; /* times the number of bytes/char */

    crtc_update_chargen_rel();
}

void crtc_set_chargen_addr(BYTE *chargen, int cmask)
{
    crtc.chargen_base = chargen;
    crtc.chargen_mask = cmask;

    crtc_update_chargen_rel();
}

void crtc_set_screen_options(int num_cols, int rasterlines)
{
    crtc.screen_width = num_cols * 8 + 2 * CRTC_SCREEN_BORDERWIDTH;
    crtc.screen_height = rasterlines + 2 * CRTC_SCREEN_BORDERHEIGHT;

    crtc_update_screen();
}

void crtc_set_hw_options(int hwflag, int vmask, int vchar, int vcoffset,
								int vrevmask)
{
    crtc.hw_cursor = hwflag & 1;
    crtc.hw_double_cols = hwflag & 2;
    crtc.vaddr_mask = vmask;
    crtc.vaddr_charswitch = vchar;
    crtc.vaddr_charoffset = vcoffset << 4; /* times the number of bytes/char */
    crtc.vaddr_revswitch = vrevmask;

    crtc_update_chargen_rel();
    crtc_update_memptr_inc();
}

void crtc_set_retrace_callback(machine_crtc_retrace_signal_t callback, 
                                                                int type) 
{
    crtc.retrace_callback = callback;
    crtc.retrace_type = type;
}


/*--------------------------------------------------------------------*/

canvas_t crtc_init (void)
{
  raster_t *raster;
  unsigned int width, height;
  char *title;

  crtc.log = log_open ("CRTC");

  alarm_init (&crtc.raster_draw_alarm, &maincpu_alarm_context,
              "CrtcRasterDraw", crtc_raster_draw_alarm_handler);

  raster = &crtc.raster;

  raster_init (raster, CRTC_NUM_VMODES, 0);
  raster_modes_set_idle_mode (&raster->modes, CRTC_IDLE_MODE);
  raster_set_exposure_handler (raster, crtc_exposure_handler);
  raster_enable_cache (raster, crtc_resources.video_cache_enabled);
  raster_enable_double_scan (raster, crtc_resources.double_scan_enabled);

  /* FIXME */
  width = CRTC_SCREEN_WIDTH();
  height = CRTC_SCREEN_HEIGHT();
#if 0
  if (crtc_resources.double_size_enabled)
    {
      width *= 2;
      height *= 2;
      raster_set_pixel_size (raster, 2, 2);
    }
#endif

  if (! crtc.regs[0]) crtc.regs[0] = 49;
  if (! crtc.regs[1]) crtc.regs[1] = 40;
  if (! crtc.regs[6]) crtc.regs[6] = 25;
  if (! crtc.regs[9]) crtc.regs[9] = 7;

  crtc.retrace_callback = NULL;

  log_debug("scr_width=%d, scr_height=%d, xpix=%d, ypix=%d",
			CRTC_SCREEN_WIDTH(), CRTC_SCREEN_HEIGHT(),
                       CRTC_SCREEN_XPIX(), CRTC_SCREEN_YPIX());
  log_debug("tcols=%d, tlines=%d, bwidth=%d, bheight=%d",
			CRTC_SCREEN_TEXTCOLS(), CRTC_SCREEN_TEXTLINES(),
                       CRTC_SCREEN_BORDERWIDTH, CRTC_SCREEN_BORDERHEIGHT);
  log_debug("displayed lines: first=%d, last=%d",
                       CRTC_FIRST_DISPLAYED_LINE, CRTC_LAST_DISPLAYED_LINE);

  raster_set_geometry (raster,
                       CRTC_SCREEN_WIDTH(), CRTC_SCREEN_HEIGHT(),
                       CRTC_SCREEN_XPIX(), CRTC_SCREEN_YPIX(),
                       CRTC_SCREEN_TEXTCOLS(), CRTC_SCREEN_TEXTLINES(),
                       CRTC_SCREEN_BORDERWIDTH, CRTC_SCREEN_BORDERHEIGHT,
                       FALSE,
                       CRTC_FIRST_DISPLAYED_LINE,
                       CRTC_LAST_DISPLAYED_LINE,
                       0);
  raster_resize_viewport (raster, width, height);

  if (crtc_load_palette (crtc_resources.palette_file_name) < 0)
    log_error (crtc.log, "Cannot load palette.");

  title = concat ("VICE: ", machine_name, " emulator", NULL);
  raster_set_title (raster, title);
  free (title);

  raster_realize (raster);

  crtc.initialized = 1;
  crtc_update_chargen_rel();
  crtc_update_memptr_inc();
  crtc_reset_screen_ptr();

  crtc_draw_init ();
  crtc_draw_set_double_size (crtc_resources.double_size_enabled);
  crtc_reset ();

  raster->display_ystart = 0;
  raster->display_ystop = CRTC_SCREEN_YPIX();
  raster->display_xstart = 0;
  raster->display_xstop = CRTC_SCREEN_XPIX();

  return crtc.raster.viewport.canvas;
}

/* Reset the VIC-II chip.  */
void crtc_reset (void)
{
  raster_reset (&crtc.raster);

  alarm_set (&crtc.raster_draw_alarm, CRTC_CYCLES_PER_LINE());

  crtc_reset_screen_ptr();
}



/* WARNING: This does not change the resource value.  External modules are
   expected to set the resource value to change the VIC-II palette instead of
   calling this function directly.  */
int crtc_load_palette (const char *name)
{
  static const char *color_names[CRTC_NUM_COLORS] =
    {
      "Background", "Foreground"
    };
  palette_t *palette;

  palette = palette_create (CRTC_NUM_COLORS, color_names);
  if (palette == NULL)
    return -1;

  if (palette_load (name, palette) < 0)
    {
      log_message (crtc.log, "Cannot load palette file `%s'.", name);
      return -1;
    }

  raster_set_palette (&crtc.raster, palette);
  return 0;
}



/* Set proper functions and constants for the current video settings. */
void crtc_resize (void)
{
  if (! crtc.initialized)
    return;

  if (crtc_resources.double_size_enabled)
    {
      if (crtc.raster.viewport.pixel_size.width == 1
          && crtc.raster.viewport.canvas != NULL)
        raster_resize_viewport (&crtc.raster,
                                crtc.raster.viewport.width * 2,
                                crtc.raster.viewport.height * 2);

      raster_set_pixel_size (&crtc.raster, 2, 2);

      crtc_draw_set_double_size (1);
    }
  else
    {
      if (crtc.raster.viewport.pixel_size.width == 2
          && crtc.raster.viewport.canvas != NULL)
        raster_resize_viewport (&crtc.raster,
                                crtc.raster.viewport.width / 2,
                                crtc.raster.viewport.height / 2);

      raster_set_pixel_size (&crtc.raster, 1, 1);

      crtc_draw_set_double_size (0);
    }
}



#if 0
void crtc_set_screen_mode (BYTE *screen,
                      int vmask,
                      int num_cols,
                      int hwflags)
{
  /* FIXME: All of this should go into the `crtc' struct.  */
  addr_mask = vmask;

  if (screen)
    screenmem = screen;

  crsr_enable = hwflags & 1;
  hw_double_cols = hwflags & 2;

  if (!num_cols)
    new_memptr_inc = 1;
  else
    new_memptr_inc = num_cols;

  /* no *2 for hw_double_cols, as the caller should have done it.
     This num_cols flag should be gone sometime.... */
  new_memptr_inc = crtc_min (SCREEN_MAX_TEXTCOLS, new_memptr_inc);

#ifdef __MSDOS__
  /* FIXME: This does not have any effect until there is a gfx -> text ->
     gfx mode transition.  Moreover, no resources should be changed behind
     user's back...  So this is definitely a Bad Thing (tm).  For now, it's
     fine with us, though.  */
  resources_set_value ("VGAMode",
                       (resource_value_t)  (num_cols > 40
                                            ? VGA_640x480 : VGA_320x200));
#endif

  /* vmask has changed -> */
  crtc_update_memory_ptrs ();

  /* force window reset with parameter =1 */
  crtc_update_timing (1);
}
#endif /* 0 */



/* Redraw the current raster line.  This happens at the last
   cycle of each line.  */
int crtc_raster_draw_alarm_handler (long offset)
{
    raster_emulate_line (&crtc.raster);

    if (crtc.raster.current_line == 0)
    {
        raster_skip_frame (&crtc.raster, do_vsync (crtc.raster.skip_frame));

        /* Do vsync stuff.  */
        crtc_reset_screen_ptr();

	crtc.raster.ycounter = 0;
    } else {
	if (crtc.raster.current_line > CRTC_SCREEN_BORDERHEIGHT) {
	    /* FIXME: charheight */
	    if (crtc.raster.ycounter < 7) {
	        crtc.raster.ycounter ++;
	    } else {
	        crtc.raster.ycounter = 0;

	        crtc.screen_rel += crtc.memptr_inc;
	    }
	}
    }

    /* FIXME: old PET have full YPIX size signal, new PET only vert. retrace,
       i.e. use retrace_type */
    if (crtc.raster.current_line == (CRTC_SCREEN_BORDERHEIGHT - 1)) {
	if (crtc.retrace_callback) 
	    crtc.retrace_callback(0);
    }
    if (crtc.raster.current_line == (CRTC_SCREEN_BORDERHEIGHT 
		+ CRTC_SCREEN_YPIX() - 1) ) {
	if (crtc.retrace_callback) 
	    crtc.retrace_callback(1);
    }

    alarm_set (&crtc.raster_draw_alarm, clk + CRTC_CYCLES_PER_LINE() - offset);

    return 0;
}



void crtc_exposure_handler (unsigned int width,
                       unsigned int height)
{
  raster_resize_viewport (&crtc.raster, width, height);

  /* FIXME: Needed?  Maybe this should be triggered by
     `raster_resize_viewport()' automatically.  */
  raster_force_repaint (&crtc.raster);
}



/* Free the allocated frame buffer.  FIXME: Not incapsulated.  */
void video_free(void)
{
  frame_buffer_free (&crtc.raster.frame_buffer);
}

void video_setfullscreen (int v, int width, int height)
{
}


/* ------------------------------------------------------------------- */

/* FIXME: use retrace_type */
int crtc_offscreen(void)
{
    if ( (crtc.raster.current_line < CRTC_SCREEN_BORDERHEIGHT)
	|| (crtc.raster.current_line 
		>= CRTC_SCREEN_BORDERHEIGHT + CRTC_SCREEN_YPIX())) {
	return 1;
    } 
    return 0;
}

void crtc_screen_enable(int enable)
{
}


