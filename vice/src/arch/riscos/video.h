/*
 * video.h - RISC OS graphics routines.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#ifndef _VIDEO_RO_H
#define _VIDEO_RO_H


#include "palette.h"
#include "vice.h"
#include "ui.h"


struct _canvas {
  unsigned int width, height;
  int shiftx, shifty;
  unsigned int emuwindow;
  unsigned int drawable;
  PIXEL *pixel_translation;
};

typedef struct _canvas *canvas_t;


struct _frame_buffer {
  int width, height;
  PIXEL *tmpframebuffer;
  unsigned int tmpframebufferlinesize;
};

typedef struct _frame_buffer frame_buffer_t;

typedef PIXEL *frame_buffer_ptr_t;
typedef ui_exposure_handler_t canvas_redraw_t;

#define FRAME_BUFFER_START(i)		((i).tmpframebuffer)
#define FRAME_BUFFER_LINE_SIZE(i)	((i).tmpframebufferlinesize)
#define FRAME_BUFFER_LINE_START(i, n)	((i).tmpframebuffer \
					+ (n) * (i).tmpframebufferlinesize)

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0


extern int video_init_resources(void);
extern int video_init_cmdline_options(void);
extern int video_init(void);
extern void video_free(void);

extern int frame_buffer_alloc(frame_buffer_t * i, unsigned int width, unsigned int height);
extern void frame_buffer_free(frame_buffer_t * i);
extern void frame_buffer_clear(frame_buffer_t * i, PIXEL value);

extern canvas_t canvas_create(const char *win_name, unsigned int *width,
			      unsigned int *height, int mapped,
			      canvas_redraw_t exposure_handler,
			      const palette_t *palette, PIXEL *pixel_return);
extern int canvas_set_palette(canvas_t c, const palette_t *palette, PIXEL *pixel_return);
extern void canvas_destroy(canvas_t s);
extern void canvas_map(canvas_t s);
extern void canvas_unmap(canvas_t s);
extern void canvas_resize(canvas_t s, unsigned int width, unsigned int height);
extern void canvas_refresh(canvas_t canvas, frame_buffer_t frame_buffer,
				  unsigned int xs, unsigned int ys,
				  unsigned int xi, unsigned int yi,
				  unsigned int w, unsigned int h);
extern void canvas_mode_change(void);

extern void text_enable(void);
extern void text_disable(void);
extern int num_text_lines(void);
extern void enable_text(void);
extern void disable_text(void);

extern int video_full_screen_on(void);
extern int video_full_screen_off(void);


extern unsigned int ColourTable[256];
extern frame_buffer_t *FrameBuffer;
extern canvas_t EmuCanvas;
extern int FullScreenMode;

#endif
