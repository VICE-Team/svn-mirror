/*
 * video.c - X11 graphics routines.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "video.h"
#include "kbd.h"
#include "log.h"
#include "resources.h"
#include "cmdline.h"
#include "utils.h"

/* Define this for additional shared memory verbosity. */
#undef MITSHM_DEBUG

#ifdef MITSHM_DEBUG
#define DEBUG_MITSHM(x)		log_debug x
#else
#define DEBUG_MITSHM(x)
#endif

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
    { "MITSHM", RES_INTEGER, (resource_value_t) - 1,
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
      "XSync", (resource_value_t) 1,
      NULL, "Call `XSync()' after updating the emulation window" },
    { "+xsync", SET_RESOURCE, 0, NULL, NULL,
      "XSync", (resource_value_t) 0,
      NULL, "Do not call `XSync()' after updating the emulation window" },
    { "-mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t) 1,
      NULL, "Try to use shared memory if the X server allows it (faster)" },
    { "+mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t) 0,
      NULL, "Never use shared memory (slower)" },
    { "-mitshmauto", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t) - 1,
      NULL, "Autodetect shared memory. If it fails use +/-mitshm." },
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
static int use_mitshm = 0;

/* The RootWindow of our screen. */
static Window root_window;

/* Logging goes here.  */
static log_t video_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

#if X_DISPLAY_DEPTH == 0

/* Conversion routines between 8bit and other sizes. */

#define SRCPTR(i, x, y) \
	((i)->tmpframebuffer + (y)*(i)->tmpframebufferlinesize + (x))
#define DESTPTR(i, x, y, t) \
	((t *)((PIXEL *)(i)->x_image->data + \
	       (i)->x_image->bytes_per_line*(y)) + (x))

static void convert_8to16(frame_buffer_t * p, int sx, int sy, int w, int h)
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

static void convert_8to32(frame_buffer_t * p, int sx, int sy, int w, int h)
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
static void convert_8to8(frame_buffer_t * p, int sx, int sy, int w, int h)
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
static BYTE dither_table[4][4] = {
    { 0, 8, 2, 10 },
    { 12, 4, 14, 6 },
    { 3, 11, 1, 9 },
    { 15, 7, 13, 5 }
};

static void convert_8to1_dither(frame_buffer_t * p, int sx, int sy, int w,
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
static void convert_8toall(frame_buffer_t * p, int sx, int sy, int w, int h)
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


int video_init(void)
{
    int do_try_mitshm = 0;
    XGCValues gc_values;

    _video_gc = XCreateGC(display, XtWindow(_ui_top_level), 0, &gc_values);

    if (video_log == LOG_ERR)
	video_log = log_open("Video");

#ifdef MITSHM

    /* if < 0 then no initialization, neither 0 (don't use) or 1 (use) */
    if (try_mitshm < 0) {
	/* Check wether we are on the same machine or not.  If we are not, we
	   do not even try to use MITSHM. */
	char *p, *dname = stralloc(XDisplayName(NULL));
	struct utsname uts;

	do_try_mitshm = 0;	/* no MIT shm */
	uname(&uts);
	if ((p = strchr(dname, ':')))
	    p[0] = 0;
	if (!strlen(dname)
	    || !strcmp(dname, "localhost")
	    || !strcmp(dname, uts.nodename))
	    do_try_mitshm = 1;	/* use MIT shm */
	free(dname);
    } else {
	do_try_mitshm = try_mitshm;
    }

    if (!do_try_mitshm)
	use_mitshm = 0;
    else {
	/* This checks if the server has MITSHM extensions available
	   If try_mitshm is true and we are on a different machine, this
	   bugs out, though.  Therefore a good "same machine" detection is
	   important.  */
	int major_version, minor_version, pixmap_flag, dummy;

	/* Check whether we can use the Shared Memory Extension. */
	if (!XShmQueryVersion(display, &major_version, &minor_version,
			      &pixmap_flag)
            || !XQueryExtension(display, "MIT-SHM", &dummy, &dummy, &dummy)) {
	    log_warning(video_log,
                        "The MITSHM extension is not supported "
                        "on this display.");
	    use_mitshm = 0;
	} else {
	    DEBUG_MITSHM(("MITSHM extensions version %d.%d detected.\n",
			  major_version, minor_version));
	    use_mitshm = 1;
	}
    }

#else
    use_mitshm = 0;
#endif

    return 0;
}

/* Allocate a frame buffer. */
int frame_buffer_alloc(frame_buffer_t * i, unsigned int width,
		       unsigned int height)
{
    int sizeofpixel = sizeof(PIXEL);

    if (sizeof(PIXEL2) != sizeof(PIXEL) * 2 ||
	sizeof(PIXEL4) != sizeof(PIXEL) * 4) {
	log_error(video_log, "PIXEL2 / PIXEL4 typedefs have wrong size.");
	return -1;
    }
    /* Round up to 32-bit boundary. */
    width = (width + 3) & ~0x3;

#if X_DISPLAY_DEPTH == 0
    /* sizeof(PIXEL) is not always what we are using. I guess this should
       be checked from the XImage but I'm lazy... */
    if (depth > 8)
	sizeofpixel *= 2;
    if (depth > 16)
	sizeofpixel *= 2;
#endif

#ifdef MITSHM
    if (use_mitshm) {
	DEBUG_MITSHM(("frame_buffer_alloc(): allocating XImage with MITSHM, "
		      "%d x %d pixels...", width, height));
	i->x_image = XShmCreateImage(display, visual, depth, ZPixmap,
				   NULL, &(i->xshm_info), width, height);
	if (!i->x_image) {
	    log_error(video_log, "Cannot allocate XImage with XShm.");
	    return -1;
	}
	DEBUG_MITSHM(("Done."));
        DEBUG_MITSHM(("frame_buffer_alloc(): shmgetting %ld bytes...",
                      (long) i->x_image->bytes_per_line * i->x_image->height));
	i->xshm_info.shmid = shmget(IPC_PRIVATE, i->x_image->bytes_per_line *
				    i->x_image->height, IPC_CREAT | 0700);
	if (i->xshm_info.shmid == -1) {
	    log_error(video_log, "Cannot get shared memory.");
	    return -1;
	}
	DEBUG_MITSHM(("Done, id = 0x%lx."));
        DEBUG_MITSHM(("frame_buffer_alloc(): getting address... ",
		      (unsigned long) i->xshm_info.shmid));
	i->xshm_info.shmaddr = i->x_image->data = shmat(i->xshm_info.shmid,
							0, 0);
	if (!i->x_image->data) {
	    log_error(video_log, "Cannot get shared memory address.");
	    return -1;
	}
	DEBUG_MITSHM(("0x%lx OK.\n", (unsigned long) i->xshm_info.shmaddr));
	i->xshm_info.readOnly = 0;
	if (!XShmAttach(display, &(i->xshm_info))) {
	    log_error(video_log, "Cannot attach shared memory.");
	    return -1;
	}
	_refresh_func = (void (*)()) XShmPutImage;
    } else
#endif
    {				/* !use_mitshm */
	PIXEL *data = (PIXEL *) malloc(width * height * sizeofpixel);

	if (!data)
	    return -1;
	i->x_image = XCreateImage(display, visual, depth, ZPixmap, 0,
				  (char *) data, width, height, 32, 0);
	if (!i->x_image)
	    return -1;

	_refresh_func = (void (*)()) XPutImage;
    }

    log_message(video_log, "Successfully initialized%s shared memory.",
                use_mitshm ? ", using" : " without");

    if (!use_mitshm)
	log_warning(video_log, "Performance will be poor.");

#if X_DISPLAY_DEPTH == 0
    /* if display depth != 8 we need a temporary buffer */
    if (depth == 8) {
	i->tmpframebuffer = (PIXEL *) i->x_image->data;
	i->tmpframebufferlinesize = i->x_image->bytes_per_line;
	_convert_func = NULL;
    } else {
	i->tmpframebufferlinesize = width;
	i->tmpframebuffer = (PIXEL *) malloc(width * height);
	if (i->x_image->bits_per_pixel == 8)
	    _convert_func = convert_8to8;
	else if (i->x_image->bits_per_pixel == 1)
	    _convert_func = convert_8to1_dither;
	else if (i->x_image->bits_per_pixel == 16)
	    _convert_func = convert_8to16;
	else if (i->x_image->bits_per_pixel == 32)
	    _convert_func = convert_8to32;
	else
	    _convert_func = convert_8toall;
    }
#endif

    return 0;
}

/* Free an allocated frame buffer. */
void frame_buffer_free(frame_buffer_t * i)
{
    if (!i)
	return;

#ifdef MITSHM
    if (use_mitshm) {
	XShmDetach(display, &(i->xshm_info));
	if (i->x_image)
	    XDestroyImage(i->x_image);
	if (shmdt(i->xshm_info.shmaddr))
	    log_error(video_log, "Cannot release shared memory!");
	shmctl(i->xshm_info.shmid, IPC_RMID, 0);
    } else
#endif

    if (i->x_image)
	XDestroyImage(i->x_image);

#if X_DISPLAY_DEPTH == 0
    /* free temporary 8bit frame buffer */
    if (depth != 8 && i->tmpframebuffer)
	free(i->tmpframebuffer);
#endif
}

void frame_buffer_clear(frame_buffer_t * f, PIXEL value)
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

    c = (canvas_t) XtMalloc(sizeof(struct _canvas));
    c->emuwindow = w;
    c->drawable = ui_canvas_get_drawable(w);
    c->width = *width;
    c->height = *height;
    XtAddEventHandler(w, (EnterWindowMask | LeaveWindowMask | KeyReleaseMask
			  | KeyPressMask), True,
		      (XtEventHandler) kbd_event_handler, NULL);

    /* ...ugly... */
    XtAddEventHandler(XtParent(w), (EnterWindowMask | LeaveWindowMask
				  | KeyReleaseMask | KeyPressMask), True,
		      (XtEventHandler) kbd_event_handler, NULL);
    return c;
}

int canvas_set_palette(canvas_t c, const palette_t * palette,
		       PIXEL * pixel_return)
{
    return ui_canvas_set_palette(c->emuwindow, palette, pixel_return);
}

/* Make the canvas visible. */
void canvas_map(canvas_t s)
{
    XMapWindow(display, s->drawable);
    XFlush(display);
}

/* Make the canvas not visible. */
void canvas_unmap(canvas_t s)
{
    XUnmapWindow(display, s->drawable);
    XFlush(display);
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
