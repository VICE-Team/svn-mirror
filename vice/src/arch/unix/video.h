/*
 * video.h - X11 graphics routines.
 *
 * Written by
 *   Ettore Perazzoli (ettore@comm2000.it)
 *   Teemu Rantanen (tvr@cs.hut.fi)
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

#ifndef _VIDEO_X_H
#define _VIDEO_X_H

#include "vice.h"

#include <X11/Xlib.h>
#if defined( MITSHM )
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "types.h"
#include "ui.h"
#include "resources.h"		/* app_resources.useXSync */

struct _canvas {
    unsigned int width, height;
    UiWindow emuwindow;
    Window drawable;
};
typedef struct _canvas *canvas_t;

struct _frame_buffer {
    XImage *x_image;
#if defined (MITSHM)
    XShmSegmentInfo xshm_info;
#endif
#if X_DISPLAY_DEPTH == 0
    PIXEL *tmpframebuffer;
    int   tmpframebufferlinesize;
#endif
    GC gc;
};
typedef struct _frame_buffer frame_buffer_t;

typedef PIXEL *frame_buffer_ptr_t;
typedef UiExposureHandler canvas_redraw_t;
typedef UiColorDef color_def_t;

extern Display *display;
extern void (*_refresh_func) ();
#if X_DISPLAY_DEPTH == 0
extern void (*_convert_func) (frame_buffer_t *p, int x, int y, int w, int h);
#endif
extern GC _video_gc;

#if X_DISPLAY_DEPTH == 0
#define FRAME_BUFFER_START(i)		((i).tmpframebuffer)
#define FRAME_BUFFER_LINE_SIZE(i)	((i).tmpframebufferlinesize)
#define FRAME_BUFFER_LINE_START(i, n)	((i).tmpframebuffer \
					 + (n) * (i).tmpframebufferlinesize)
#else
#define FRAME_BUFFER_START(i)		((PIXEL *)((i).x_image->data))
#define FRAME_BUFFER_LINE_SIZE(i)	((i).x_image->bytes_per_line/sizeof(PIXEL))
#define FRAME_BUFFER_LINE_START(i, n)	(PIXEL *)((i).x_image->data \
					 + (n) * (i).x_image->bytes_per_line)
#endif

inline static void canvas_refresh(canvas_t canvas, frame_buffer_t frame_buffer,
				  unsigned int xs, unsigned int ys,
				  unsigned int xi, unsigned int yi,
				  unsigned int w, unsigned int h)
{
#if X_DISPLAY_DEPTH == 0
    if (_convert_func)
	_convert_func(&frame_buffer, xs, ys, w, h);
#endif
    _refresh_func(display, canvas->drawable, _video_gc,
		   frame_buffer.x_image, xs, ys, xi, yi, w, h, False);
    if (app_resources.useXSync)
	XSync(display, False);
}

/* ------------------------------------------------------------------------- */

extern int video_init(void);
extern int frame_buffer_alloc(frame_buffer_t * i, unsigned int width,
			      unsigned int height);
extern void frame_buffer_free(frame_buffer_t * i);
extern void frame_buffer_clear(frame_buffer_t * i, PIXEL value);

extern canvas_t canvas_create(const char *win_name, unsigned int *width,
			      unsigned int *height, int mapped,
			      canvas_redraw_t exposure_handler,
			      int num_colors, const color_def_t color_defs[],
			      PIXEL *pixel_return);
extern void canvas_destroy(canvas_t s);
extern void canvas_map(canvas_t s);
extern void canvas_unmap(canvas_t s);
extern void canvas_resize(canvas_t s, unsigned int width, unsigned int height);

extern void text_enable(void);
extern void text_disable(void);
extern int num_text_lines(void);
extern void enable_text(void);
extern void disable_text(void);

#endif				/* !defined (_VIDEO_X_H) */
