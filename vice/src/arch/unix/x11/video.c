/*
 * video.c - X11 graphics routines.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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


/*** MITSHM-code rewritten by Dirk Farin <farin@ti.uni-mannheim.de>. **

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

#include "color.h"
#include "cmdline.h"
#include "kbd.h"
#include "log.h"
#include "machine.h"
#include "palette.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uicolor.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif

/* ------------------------------------------------------------------------- */

/* Flag: Do we call `XSync()' after blitting?  */
int _video_use_xsync;

/* Flag: Do we try to use the MIT shared memory extensions?  */
static int try_mitshm;

static int set_use_xsync(resource_value_t v, void *param)
{
    _video_use_xsync = (int)v;
    return 0;
}

static int set_try_mitshm(resource_value_t v, void *param)
{
    try_mitshm = (int)v;
    return 0;
}

/* Video-related resources.  */
static resource_t resources[] = {
    { "UseXSync", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&_video_use_xsync, set_use_xsync, NULL },
      /* turn MITSHM on by default */
    { "MITSHM", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&try_mitshm, set_try_mitshm, NULL },
    { NULL }
};

int video_arch_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Video-related command-line options.  */
static cmdline_option_t cmdline_options[] = {
    { "-xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t)1,
      NULL, N_("Call `XSync()' after updating the emulation window") },
    { "+xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t)0,
      NULL, N_("Do not call `XSync()' after updating the emulation window") },
    { "-mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t) 0,
      NULL, N_("Never use shared memory (slower)") },
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static GC _video_gc;
static void (*_refresh_func)();

/* This is set to 1 if the Shared Memory Extensions can actually be used. */
int use_mitshm = 0;

/* The RootWindow of our screen. */
/* static Window root_window; */

/* Logging goes here.  */
static log_t video_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

#if VIDEO_DISPLAY_DEPTH == 0
static void (*_convert_func) (video_frame_buffer_t *p, int x, int y, int w,
                              int h);
static BYTE shade_table[256];

void video_convert_color_table(unsigned int i, PIXEL *pixel_return, PIXEL *data,
                               unsigned int bits_per_pixel,
                               unsigned int dither, long col,
                               video_canvas_t *c)
{
    *pixel_return = i;

    switch (bits_per_pixel) {
      case 8:
        video_render_setphysicalcolor(&c->videoconfig, i,
                                      (DWORD)(*(PIXEL *)data), 8);
        break;
      case 16:
        video_render_setphysicalcolor(&c->videoconfig, i, (DWORD)(col), 16);
        break;
      case 32:
        video_render_setphysicalcolor(&c->videoconfig, i, (DWORD)(col), 32);
        break;
      default:
        video_render_setphysicalcolor(&c->videoconfig, i, (DWORD)(col),
                                      bits_per_pixel);
    }

    if (bits_per_pixel == 1)
        shade_table[i] = dither;
}

/* This doesn't usually happen, but if it does, this is a great speedup
   comparing the general convert_8toall() -routine. */
static void convert_8to8(video_frame_buffer_t *p, int sx, int sy, int w, int h)
{
    video_render_main(&p->canvas->videoconfig, p->tmpframebuffer,
                      p->x_image->data,
                      w, h, sx, sy, sx, sy, p->tmpframebufferlinesize,
                      p->x_image->bytes_per_line, 8);

}

static void convert_8to16(video_frame_buffer_t *p, int sx, int sy, int w,
                          int h)
{
    video_render_main(&p->canvas->videoconfig, p->tmpframebuffer,
                      p->x_image->data,
                      w, h, sx, sy, sx, sy, p->tmpframebufferlinesize,
                      p->x_image->bytes_per_line, 16);
}

static void convert_8to32(video_frame_buffer_t *p, int sx, int sy, int w,
                          int h)
{
    video_render_main(&p->canvas->videoconfig, p->tmpframebuffer,
                      p->x_image->data,
                      w, h, sx, sy, sx, sy, p->tmpframebufferlinesize,
                      p->x_image->bytes_per_line, 32);
}

#define SRCPTR(i, x, y) \
        ((i)->tmpframebuffer + (y)*(i)->tmpframebufferlinesize + (x))
#define DESTPTR(i, x, y, t) \
        ((t *)((PIXEL *)(i)->x_image->data + \
               (i)->x_image->bytes_per_line*(y)) + (x))

/* Use dither on 1bit display. This is slow but who cares... */
BYTE dither_table[4][4] = {
    { 0, 8, 2, 10 },
    { 12, 4, 14, 6 },
    { 3, 11, 1, 9 },
    { 15, 7, 13, 5 }
};

static void convert_8to1_dither(video_frame_buffer_t *p, int sx, int sy, int w,
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
                      p->canvas->videoconfig.physical_colors[shade_table[src[x]]
                      > dither[(sx + x) % 4]]);
        }
    }
}

/* And this is inefficient... */
static void convert_8toall(video_frame_buffer_t * p, int sx, int sy, int w,
                           int h)
{
    PIXEL *src;
    int x, y;
    for (y = 0; y < h; y++) {
        src = SRCPTR(p, sx, sy + y);
        for (x = 0; x < w; x++)
            XPutPixel(p->x_image, sx + x, sy + y,
                      p->canvas->videoconfig.physical_colors[src[x]]);
    }
}

#endif


int video_convert_func(video_frame_buffer_t *i, int depth, unsigned int width,
                       unsigned int height)
{
#if VIDEO_DISPLAY_DEPTH == 0
    i->tmpframebufferlinesize = width;
    i->tmpframebuffer = (PIXEL *)xmalloc(width * height);
    switch (i->x_image->bits_per_pixel) {
      case 1:
        _convert_func = convert_8to1_dither;
        break;
      case 8:
        _convert_func = convert_8to8;
        break;
      case 16:
        _convert_func = convert_8to16;
        break;
      case 32:
        _convert_func = convert_8to32;
        break;
      default:
        _convert_func = convert_8toall;
    }
#else
/* VIDEO_DISPLAY_DEPTH == 24 should really be 32.  */
#if VIDEO_DISPLAY_DEPTH == 24
    unsigned int sup_depth = 32;
#endif
#if VIDEO_DISPLAY_DEPTH == 16
    unsigned int sup_depth = 16;
#endif
#if VIDEO_DISPLAY_DEPTH == 8
    unsigned int sup_depth = 8;
#endif
    if (sup_depth != i->x_image->bits_per_pixel) {
        log_error(video_log,
                  _("Only %ibpp supported by this emulator."), sup_depth);
        log_error(video_log,
                  _("Current X server depth is %ibpp."),
                  i->x_image->bits_per_pixel);
        log_error(video_log,
                  _("Switch X server depth or recompile with --enable-autobpp."));
        return -1;
    }
#endif
    return 0;
}

extern GC video_get_gc(void *not_used);

int video_init(void)
{
    XGCValues gc_values;
    Display *display;

    _video_gc = video_get_gc(&gc_values);
    display = ui_get_display_ptr();

    if (video_log == LOG_ERR)
        video_log = log_open("Video");

    color_init();

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
                        _("The MITSHM extension is not supported "
                        "on this display."));
            use_mitshm = 0;
        } else {
            DEBUG_MITSHM((_("MITSHM extensions version %d.%d detected."),
                          major_version, minor_version));
            use_mitshm = 1;
        }
    }

#else
    use_mitshm = 0;
#endif

    video_init_arch();

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
void video_frame_buffer_free(video_frame_buffer_t *i)
{
    Display *display;

    if (!i)
        return;

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
	return;
#endif
    display = ui_get_display_ptr();

#ifdef USE_MITSHM
    if (i->using_mitshm) {
        XShmDetach(display, &(i->xshm_info));
        if (i->x_image)
            XDestroyImage(i->x_image);
        if (shmdt(i->xshm_info.shmaddr))
            log_error(video_log, _("Cannot release shared memory!"));
    } else if (i->x_image)
        XDestroyImage(i->x_image);
#else
#ifndef USE_GNOMEUI
    if (i->x_image)
        XDestroyImage(i->x_image);
#endif

#endif

#if VIDEO_DISPLAY_DEPTH == 0
    {
        int depth;

        depth = ui_get_display_depth();

        /* Free temporary 8bit frame buffer.  */
        if (depth != 8 && i->tmpframebuffer)
            free(i->tmpframebuffer);
    }
#endif
    {
        extern video_canvas_t *dangling_canvas;
        dangling_canvas = i->canvas;
    }
    free(i);
}

void video_frame_buffer_clear(video_frame_buffer_t *f, PIXEL value)
{
#if VIDEO_DISPLAY_DEPTH == 0
    memset(f->tmpframebuffer, value,
           f->x_image->height * f->tmpframebufferlinesize);
#else
    int i;

    for (i = 0; i < f->x_image->height * f->x_image->bytes_per_line;
         i += sizeof(PIXEL))
        *((PIXEL *)(f->x_image->data + i)) = value;
#endif
}

void video_register_raster(raster_t *raster)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_set_raster(raster);
#endif
}

/* ------------------------------------------------------------------------- */
/* Create a canvas.  In the X11 implementation, this is just (guess what?) a
   window. */
video_canvas_t *video_canvas_create(const char *win_name, unsigned int *width,
                                    unsigned int *height, int mapped,
                                    void_t exposure_handler,
                                    const palette_t *palette,
                                    PIXEL *pixel_return,
                                    video_frame_buffer_t *fb)
{
    video_canvas_t *c;
    ui_window_t w;
    XGCValues gc_values;

    c = (video_canvas_t *)xmalloc(sizeof(struct video_canvas_s));
    memset(c, 0, sizeof(struct video_canvas_s));

    video_render_initconfig(&c->videoconfig);

    w = ui_open_canvas_window(c, win_name, *width, *height, 1,
                              (canvas_redraw_t)exposure_handler, palette,
                              pixel_return);

    if (!_video_gc)
        _video_gc = video_get_gc(&gc_values);

    if (!w) {
        free(c);
        return (video_canvas_t *)NULL;
    }

    c->emuwindow = w;
    c->width = *width;
    c->height = *height;
    ui_finish_canvas(c);

    video_add_handlers(w);
    if (console_mode || vsid_mode)
        return c;

    fb->canvas = c;

#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_set_palette(palette, pixel_return);
#endif

    return c;
}

void video_canvas_destroy(video_canvas_t *c)
{
        /* FIXME: Just a dummy so far */
}


int video_canvas_set_palette(video_canvas_t *c, const palette_t *palette,
                             PIXEL *pixel_return)
{
    return uicolor_set_palette(c, palette, pixel_return);
}

/* Change the size of the canvas. */
void video_canvas_resize(video_canvas_t *s, unsigned int width,
                         unsigned int height)
{
    if (console_mode || vsid_mode) {
        return;
    }
#ifdef USE_XF86_DGA2_EXTENSIONS
    /* printf("%s: w = %d, h = %d\n", __FUNCTION__, width, height); */
    if (fullscreen_is_enabled)
	return;
    fullscreen_resize(width, height);
#endif

    ui_resize_canvas_window(s->emuwindow, width, height);
    s->width = width;
    s->height = height;
    ui_finish_canvas(s);
}

void enable_text(void)
{
}

void disable_text(void)
{
}

/* ------------------------------------------------------------------------- */

void video_refresh_func(void (*rfunc)(void))
{
    _refresh_func = rfunc;
}

/* ------------------------------------------------------------------------- */

/* Refresh a canvas.  */
void video_canvas_refresh(video_canvas_t *canvas,
                          video_frame_buffer_t *frame_buffer,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    Display *display;

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
    {
	fullscreen_refresh_func(frame_buffer, xs, ys, xi, yi, w, h);
	return;
    }

#endif
#if VIDEO_DISPLAY_DEPTH == 0
    if (_convert_func)
        _convert_func(frame_buffer, xs, ys, w, h);
#endif

    /* This could be optimized away.  */
    display = ui_get_display_ptr();

    _refresh_func(display, canvas->drawable, _video_gc,
                  frame_buffer->x_image, xs, ys, xi, yi, w, h, False,
                  frame_buffer, canvas);
    if (_video_use_xsync)
        XSync(display, False);
}

