/*
 * videoarch.h - X11 graphics routines.
 *
 * Written by
 *   Ettore Perazzoli <ettore@comm2000.it>
 *   Teemu Rantanen <tvr@cs.hut.fi>
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

#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

#include "vice.h"
#include "palette.h"

#include <X11/Xlib.h>
#ifdef USE_MITSHM
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "log.h"
#include "types.h"
#include "ui.h"
#include "resources.h"		/* app_resources.useXSync */

#ifdef USE_GNOMEUI
#include <gdk/gdk.h>		
#endif

struct _canvas {
    unsigned int width, height;
    ui_window_t emuwindow;
    Window drawable;
};
typedef struct _canvas *canvas_t;

/* Triple buffering is never available on X11.  */
#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

struct _frame_buffer {
    XImage *x_image;
#ifdef USE_GNOMEUI
    GdkImage *gdk_image;	/* FIXME MP #ifdef GNOME! */
#endif

#ifdef USE_MITSHM
    XShmSegmentInfo xshm_info;
    int using_mitshm;     /* True if MITSHM is used for this framebuffer. */
#endif
#if X_DISPLAY_DEPTH == 0
    PIXEL *tmpframebuffer;
    int   tmpframebufferlinesize;
#endif
    GC gc;
};
typedef struct _frame_buffer frame_buffer_t;

typedef PIXEL *frame_buffer_ptr_t;
typedef ui_exposure_handler_t canvas_redraw_t;

extern Display *display;
#if X_DISPLAY_DEPTH == 0
extern void (*_convert_func) (frame_buffer_t *p, int x, int y, int w, int h);
#endif
extern GC _video_gc;
extern int _video_use_xsync;

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

/* ------------------------------------------------------------------------- */

extern void text_enable(void);
extern void text_disable(void);
extern int num_text_lines(void);
extern void enable_text(void);
extern void disable_text(void);

extern log_t video_log;
extern int use_mitshm;

#ifdef USE_MITSHM
extern int shmhandler(Display* display,XErrorEvent* err);
extern int mitshm_failed; /* will be set to true if XShmAttach() failed */
extern int shmmajor;          /* major number of MITSHM error codes */
/* Define this for additional shared memory verbosity. */
/*  #define MITSHM_DEBUG */

#ifdef MITSHM_DEBUG
#define DEBUG_MITSHM(x)		log_debug x
#else
#define DEBUG_MITSHM(x)
#endif
#endif /* USE_MITSHM */

extern void convert_8to8(frame_buffer_t * p, int sx, int sy, int w, int h);
extern void convert_8to1_dither(frame_buffer_t * p, int sx, int sy, int w, int h);
extern void convert_8to16(frame_buffer_t * p, int sx, int sy, int w, int h);
extern void convert_8to32(frame_buffer_t * p, int sx, int sy, int w, int h);
extern void convert_8toall(frame_buffer_t * p, int sx, int sy, int w, int h);

#endif

