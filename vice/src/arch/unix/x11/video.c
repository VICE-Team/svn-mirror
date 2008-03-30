/*
 * video.c - X11 graphics routines.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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


/*** MITSHM-code rewritten by Dirk Farin (farin@ti.uni-mannheim.de). **

     This is how the MITSHM initialization now works:

       Three variables are used to enable/disable the usage of MITSHM:
       - 'try_mitshm' is set to true by default to specify that
         MITSHM shall be used if possible. If the user sets this
	 variable to false MITSHM will be disabled.
       - 'use_mitshm' will be set in video_init() after some quick
         tests if the X11 server supports MITSHM.
       - Every framebuffer structure has a new field named 'using_mitshm'
         that is set to true if MITSHM is used for this buffer.
	 Note that it is possible that one buffer is using MITSHM
	 while some other buffer is not.

       Detecting if MITSHM usage is possible is now done using a
       minimum of intelligence (only XShmQueryExtension() is checked
       in video_init() ). Then the allocation process is executed
       and the X11 error in case of failure is catched. If an error
       occured the allocation process is reversed and non-MITSHM
       XImages are used instead.
*/

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>

#include <X11/cursorfont.h>
#ifdef XPM
#include <X11/xpm.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

#include "kbd.h"
#include "log.h"
#include "cmdline.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"


/* ------------------------------------------------------------------------- */

/* Flag: Do we call `XSync()' after blitting?  */
int _video_use_xsync;

/* Flag: Do we try to use the MIT shared memory extensions?  */
static int try_mitshm;

static int set_use_xsync(resource_value_t v)
{
    _video_use_xsync = (int) v;
    return 0;
}

static int set_try_mitshm(resource_value_t v)
{
    try_mitshm = (int) v;
    return 0;
}

/* Video-related resources.  */
static resource_t resources[] = {
    { "UseXSync", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) & _video_use_xsync, set_use_xsync },
    { "MITSHM", RES_INTEGER, (resource_value_t) 1,         /* turn MITSHM on by default */
      (resource_value_t *) & try_mitshm, set_try_mitshm },
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Video-related command-line options.  */
static cmdline_option_t cmdline_options[] = {
    { "-xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t) 1,
      NULL, "Call `XSync()' after updating the emulation window" },
    { "+xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t) 0,
      NULL, "Do not call `XSync()' after updating the emulation window" },
    { "-mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t) 0,
      NULL, "Never use shared memory (slower)" },
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* These are exported by `ui.c'.  FIXME: Ugly!  */
extern Display *display;
extern int screen;
extern Visual *visual;
extern int depth;

/* These are made public so that the other modules can use them via macros. */
GC _video_gc;
void (*_refresh_func) ();
#if X_DISPLAY_DEPTH == 0
void (*_convert_func) (frame_buffer_t * p, int x, int y, int w, int h);
PIXEL real_pixel1[256];
PIXEL2 real_pixel2[256];
PIXEL4 real_pixel4[256];
long real_pixel[256];
BYTE shade_table[256];
#endif

/* This is set to 1 if the Shared Memory Extensions can actually be used. */
int use_mitshm = 0;

/* The RootWindow of our screen. */
/* static Window root_window; */

/* Logging goes here.  */
log_t video_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

#if X_DISPLAY_DEPTH == 0

/* Conversion routines between 8bit and other sizes. */

#define SRCPTR(i, x, y) \
	((i)->tmpframebuffer + (y)*(i)->tmpframebufferlinesize + (x))
#define DESTPTR(i, x, y, t) \
	((t *)((PIXEL *)(i)->x_image->data + \
	       (i)->x_image->bytes_per_line*(y)) + (x))

void convert_8to16(frame_buffer_t * p, int sx, int sy, int w, int h)
{
    PIXEL *src;
    PIXEL2 *dst;
    int x, y;

    for (y = 0; y < h; y++) {
	src = SRCPTR(p, sx, sy + y);
	dst = DESTPTR(p, sx, sy + y, PIXEL2);
	for (x = 0; x < w; x++)
	    dst[x] = real_pixel2[src[x]];
    }
}

void convert_8to32(frame_buffer_t * p, int sx, int sy, int w, int h)
{
    PIXEL *src;
    PIXEL4 *dst;
    int x, y;

    for (y = 0; y < h; y++) {
	src = SRCPTR(p, sx, sy + y);
	dst = DESTPTR(p, sx, sy + y, PIXEL4);
	for (x = 0; x < w; x++)
	    dst[x] = real_pixel4[src[x]];
    }
}

/* This doesn't usually happen, but if it does, this is a great speedup
   comparing the general convert_8toall() -routine. */
void convert_8to8(frame_buffer_t * p, int sx, int sy, int w, int h)
{
    PIXEL *src;
    PIXEL *dst;
    int x, y;

    for (y = 0; y < h; y++) {
	src = SRCPTR(p, sx, sy + y);
	dst = DESTPTR(p, sx, sy + y, PIXEL);
	for (x = 0; x < w; x++)
	    dst[x] = real_pixel1[src[x]];
    }
}

/* Use dither on 1bit display. This is slow but who cares... */
BYTE dither_table[4][4] = {
    { 0, 8, 2, 10 },
    { 12, 4, 14, 6 },
    { 3, 11, 1, 9 },
    { 15, 7, 13, 5 }
};

void convert_8to1_dither(frame_buffer_t * p, int sx, int sy, int w,
			 int h)
{
    PIXEL *src, *dither;
    int x, y;
    for (y = 0; y < h; y++) {
	src = SRCPTR(p, sx, sy + y);
	dither = dither_table[(sy + y) % 4];
	for (x = 0; x < w; x++) {
	    /* XXX: trusts that real_pixel[0, 1] == black, white */
	    XPutPixel(p->x_image, sx + x, sy + y,
                      real_pixel[shade_table[src[x]] > dither[(sx + x) % 4]]);
	}
    }
}

/* And this is inefficient... */
void convert_8toall(frame_buffer_t * p, int sx, int sy, int w, int h)
{
    PIXEL *src;
    int x, y;
    for (y = 0; y < h; y++) {
	src = SRCPTR(p, sx, sy + y);
	for (x = 0; x < w; x++)
	    XPutPixel(p->x_image, sx + x, sy + y, real_pixel[src[x]]);
    }
}

#endif

extern GC video_get_gc(void *not_used);

int video_init(void)
{
    XGCValues gc_values;

    _video_gc = video_get_gc(&gc_values);

    if (video_log == LOG_ERR)
	video_log = log_open("Video");

#ifdef USE_MITSHM
    if (!try_mitshm)
	use_mitshm = 0;
    else {
	/* This checks if the server has MITSHM extensions available
	   If try_mitshm is true and we are on a different machine,
	   frame_buffer_alloc will fall back to non shared memory calls. */
        int major_version, minor_version, pixmap_flag;

	/* Check whether the server supports the Shared Memory Extension. */
	if (!XShmQueryVersion(display, &major_version, &minor_version,
			      &pixmap_flag)) {
	    log_warning(video_log,
                        "The MITSHM extension is not supported "
                        "on this display.");
	    use_mitshm = 0;
	} else {
	    DEBUG_MITSHM(("MITSHM extensions version %d.%d detected.",
			  major_version, minor_version));
	    use_mitshm = 1;
	}
    }

#else
    use_mitshm = 0;
#endif

    return 0;
}

#ifdef USE_MITSHM
int mitshm_failed = 0; /* will be set to true if XShmAttach() failed */
int shmmajor;          /* major number of MITSHM error codes */

/* Catch XShmAttach()-failure. */
int shmhandler(Display* display,XErrorEvent* err)
{
  if (err->request_code == shmmajor &&
      err->minor_code == X_ShmAttach)
    mitshm_failed=1;

  return 0;
}
#endif

/* Free an allocated frame buffer. */
void video_frame_buffer_free(frame_buffer_t *i)
{
    if (!i)
	return;

#ifdef USE_MITSHM
    if (i->using_mitshm) {
	XShmDetach(display, &(i->xshm_info));
	if (i->x_image)
	    XDestroyImage(i->x_image);
	if (shmdt(i->xshm_info.shmaddr))
	    log_error(video_log, "Cannot release shared memory!");
    } else if (i->x_image)
	XDestroyImage(i->x_image);
#else
    if (i->x_image)
	XDestroyImage(i->x_image);
#endif

#if X_DISPLAY_DEPTH == 0
    /* Free temporary 8bit frame buffer.  */
    if (depth != 8 && i->tmpframebuffer)
	free(i->tmpframebuffer);
#endif

    memset(i, 0, sizeof(*i));
}

void video_frame_buffer_clear(frame_buffer_t *f, PIXEL value)
{
#if X_DISPLAY_DEPTH == 0
    memset(f->tmpframebuffer, value,
	   f->x_image->height * f->tmpframebufferlinesize);
    if (_convert_func)
	_convert_func(f, 0, 0, f->x_image->width, f->x_image->height);
#else
    int i;

    for (i = 0; i < f->x_image->height * f->x_image->bytes_per_line;
	 i += sizeof(PIXEL))
	*((PIXEL *) (f->x_image->data + i)) = value;
#endif
}

/* ------------------------------------------------------------------------- */

extern void video_add_handlers(Widget w);

/* Create a canvas.  In the X11 implementation, this is just (guess what?) a
   window. */
canvas_t canvas_create(const char *win_name, unsigned int *width,
		       unsigned int *height, int mapped,
		       canvas_redraw_t exposure_handler,
		       const palette_t * palette, PIXEL * pixel_return)
{
    canvas_t c;
    Widget w;

    w = ui_open_canvas_window(win_name, *width, *height, 1, exposure_handler,
			      palette, pixel_return);
    if (!w)
	return (canvas_t) NULL;

    c = (canvas_t) malloc(sizeof(struct _canvas));
    c->emuwindow = w;
    c->drawable = ui_canvas_get_drawable(w);
    c->width = *width;
    c->height = *height;
    video_add_handlers(w);
    
    return c;
}

int canvas_set_palette(canvas_t c, const palette_t * palette,
		       PIXEL * pixel_return)
{
    return ui_canvas_set_palette(c->emuwindow, palette, pixel_return);
}

/* Change the size of the canvas. */
void canvas_resize(canvas_t s, unsigned int width, unsigned int height)
{
    ui_resize_canvas_window(s->emuwindow, width, height);
}

void enable_text(void)
{
}

void disable_text(void)
{
}

int num_text_lines(void)
{
    char *s;

    s = getenv("LINES");
    if (s != NULL)
	return atoi(s);
    else
	return 24;
}

/* ------------------------------------------------------------------------- */

/* Refresh a canvas.  */
void canvas_refresh(canvas_t canvas, frame_buffer_t frame_buffer,
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
    if (_video_use_xsync)
        XSync(display, False);
}

