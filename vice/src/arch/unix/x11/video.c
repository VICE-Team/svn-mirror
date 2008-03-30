/*
 * video.c - X11 graphics routines.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "color.h"
#include "cmdline.h"
#include "fullscreenarch.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uicolor.h"
#include "video.h"
#include "videoarch.h"
#include "x11ui.h"
#ifdef USE_GNOMEUI
#include <gdk/gdkx.h>
#endif
#ifdef HAVE_XVIDEO
#include "renderxv.h"
#include "video/video-resources.h"
extern DWORD yuv_table[128];
#endif


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

static int set_use_xvideo(resource_value_t v, void *param)
{
    use_xvideo = (int)v;
    return 0;
}

static unsigned int fourcc;
static int set_fourcc(resource_value_t v, void *param)
{
    if (v && strlen((char *)v) == 4) {
        memcpy(&fourcc, (char *)v, 4);
    }
    else {
        fourcc = 0;
    }
    
    return 0;
}

static double aspect_ratio;
static const char *aspect_ratio_s;
static int set_aspect_ratio(resource_value_t v, void *param)
{
    if (v) {
        char* endptr;
        aspect_ratio_s = (char*)v;
        aspect_ratio = strtod((char*)v, &endptr);
	if ((char*)v == endptr) {
	    aspect_ratio = 1.0;
	}
	else if (aspect_ratio < 0.8) {
	    aspect_ratio = 0.8;
	}
	else if (aspect_ratio > 1.2) {
	    aspect_ratio = 1.2;
	}
    }
    else {
        aspect_ratio = 1.0;
    }
    
    return 0;
}

/* Video-related resources.  */
static const resource_t resources[] = {
    { "UseXSync", RES_INTEGER, (resource_value_t)1,
      (void *)&_video_use_xsync, set_use_xsync, NULL },
      /* turn MITSHM on by default */
    { "MITSHM", RES_INTEGER, (resource_value_t)1,
      (void *)&try_mitshm, set_try_mitshm, NULL },
#ifdef HAVE_XVIDEO
    { "XVIDEO", RES_INTEGER, (resource_value_t)0,
      (void *)&use_xvideo, set_use_xvideo, NULL },
    { "FOURCC", RES_STRING, (resource_value_t)"",
      (void *)&fourcc, set_fourcc, NULL },
    { "AspectRatio", RES_STRING, (resource_value_t)"1.0",
      (void *)&aspect_ratio_s, set_aspect_ratio, NULL },
#endif
    { NULL }
};

int video_arch_init_resources(void)
{

    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Video-related command-line options.  */
static const cmdline_option_t cmdline_options[] = {
    { "-xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t)1,
      NULL, N_("Call `XSync()' after updating the emulation window") },
    { "+xsync", SET_RESOURCE, 0, NULL, NULL,
      "UseXSync", (resource_value_t)0,
      NULL, N_("Do not call `XSync()' after updating the emulation window") },
    { "-mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t)1,
      NULL, N_("Use shared memory") },
    { "+mitshm", SET_RESOURCE, 0, NULL, NULL,
      "MITSHM", (resource_value_t)0,
      NULL, N_("Never use shared memory (slower)") },
#ifdef HAVE_XVIDEO
    { "-xvideo", SET_RESOURCE, 0, NULL, NULL,
      "XVIDEO", (resource_value_t)1,
      NULL, N_("Use XVideo Extension (hardware scaling)") },
    { "+xvideo", SET_RESOURCE, 0, NULL, NULL,
      "XVIDEO", (resource_value_t)0,
      NULL, N_("Use software rendering") },
    { "-fourcc", SET_RESOURCE, 1, NULL, NULL, "FOURCC", NULL,
      "<fourcc>", N_("Request YUV FOURCC format") },
    { "-aspect", SET_RESOURCE, 1, NULL, NULL, "AspectRatio", NULL,
      "<aspect ratio>", N_("Set aspect ratio (0.8 - 1.2)") },
#endif
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

/* This is set to 1 if the XVideo Extension is used. */
int use_xvideo = 0;

/* The RootWindow of our screen. */
/* static Window root_window; */

/* Logging goes here.  */
static log_t video_log = LOG_ERR;

#ifdef USE_XF86_EXTENSIONS
#if defined (USE_XF86_DGA1_EXTENSIONS) || defined(USE_XF86_DGA2_EXTENSIONS)
static video_refresh_func_t video_fullscreen_refresh_func = NULL;
#endif
#endif

/* ------------------------------------------------------------------------- */

static void (*_convert_func) (video_canvas_t *canvas,
			      int xs, int ys, int xt, int yt,
			      int w, int h);
static BYTE shade_table[256];

void video_convert_color_table(unsigned int i, BYTE *data, unsigned int dither,
                               long col, video_canvas_t *canvas)
{
#ifdef HAVE_XVIDEO
    if (use_xvideo && canvas->xv_image) {
        return;
    }
#endif

    switch (canvas->x_image->bits_per_pixel) {
      case 8:
        video_render_setphysicalcolor(canvas->videoconfig, i,
                                      (DWORD)(*data), 8);
        break;
      case 16:
      case 24:
      case 32:
      default:
        video_render_setphysicalcolor(canvas->videoconfig, i, (DWORD)(col),
                                      canvas->x_image->bits_per_pixel);
	break;
    }

    if (canvas->depth == 1)
        shade_table[i] = dither;
}

static void convert_8to8n(video_canvas_t *canvas,
			  int sx, int sy, int tx, int ty,
			  int w, int h)
{
    video_canvas_render(canvas, (BYTE *)canvas->x_image->data,
                        w, h, sx, sy, tx, ty,
                        canvas->x_image->bytes_per_line,
                        canvas->x_image->bits_per_pixel);
}

#define SRCPTR(x, y) \
    (canvas->draw_buffer->draw_buffer \
    + (y) * canvas->draw_buffer->draw_buffer_width + (x))
#define DESTPTR(i, x, y, t) \
    ((t *)((BYTE *)(i)->x_image->data \
    + (i)->x_image->bytes_per_line * (y)) + (x))

/* Use dither on 1bit display. This is slow but who cares... */
BYTE dither_table[4][4] = {
    { 0, 8, 2, 10 },
    { 12, 4, 14, 6 },
    { 3, 11, 1, 9 },
    { 15, 7, 13, 5 }
};

static void convert_8to1_dither(video_canvas_t *canvas,
				int sx, int sy, int tx, int ty,
				int w, int h)
{
    /* FIXME: Double size not handled. */
    BYTE *src, *dither;
    int x, y;
    for (y = 0; y < h; y++) {
        src = SRCPTR(sx, sy + y);
        dither = dither_table[(sy + y) % 4];
        for (x = 0; x < w; x++) {
            /* XXX: trusts that real_pixel[0, 1] == black, white */
            XPutPixel(canvas->x_image, tx + x, ty + y,
                      canvas->videoconfig->physical_colors[shade_table[src[x]]
                      > dither[(sx + x) % 4]]);
        }
    }
}

/* And this is inefficient... */
static void convert_8toall(video_canvas_t *canvas,
			   int sx, int sy, int tx, int ty,
			   int w, int h)
{
    /* FIXME: Double size not handled. */
    BYTE *src;
    int x, y;
    for (y = 0; y < h; y++) {
        src = SRCPTR(sx, sy + y);
        for (x = 0; x < w; x++)
            XPutPixel(canvas->x_image, tx + x, ty + y,
                      canvas->videoconfig->physical_colors[src[x]]);
    }
}


int video_convert_func(video_canvas_t *canvas, unsigned int width,
                       unsigned int height)
{
    switch (canvas->x_image->bits_per_pixel) {
      case 1:
        _convert_func = convert_8to1_dither;
        break;
      case 8:
      case 16:
      case 24:
      case 32:
        _convert_func = convert_8to8n;
        break;
      default:
        _convert_func = convert_8toall;
    }
    return 0;
}

extern GC video_get_gc(void *not_used);

int video_init(void)
{
    XGCValues gc_values;
    Display *display;

    _video_gc = video_get_gc(&gc_values);
    display = x11ui_get_display_ptr();

    video_log = log_open("Video");

    color_init();

#ifdef USE_MITSHM
    if (!try_mitshm) {
        use_mitshm = 0;
    } else {
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

void video_shutdown(void)
{
    color_shutdown();
}

#ifdef USE_MITSHM
int mitshm_failed = 0; /* will be set to true if XShmAttach() failed */
int shmmajor;          /* major number of MITSHM error codes */

/* Catch XShmAttach()-failure. */
int shmhandler(Display *display, XErrorEvent *err)
{
    if (err->request_code == shmmajor &&
        err->minor_code == X_ShmAttach)
      mitshm_failed=1;

    return 0;
}
#endif

/* Free an allocated frame buffer. */
static void video_arch_frame_buffer_free(video_canvas_t *canvas)
{
    Display *display;

    if (canvas == NULL)
        return;

#ifdef HAVE_XVIDEO
    if (use_xvideo && canvas->xv_image) {
        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;

        display = x11ui_get_display_ptr();
	destroy_yuv_image(display, canvas->xv_image, shminfo);
	return;
    }
#endif

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
        return;
#endif

    display = x11ui_get_display_ptr();

#ifdef USE_MITSHM
    if (canvas->using_mitshm) {
        XShmDetach(display, &(canvas->xshm_info));
        if (canvas->x_image)
            XDestroyImage(canvas->x_image);
        if (shmdt(canvas->xshm_info.shmaddr))
            log_error(video_log, _("Cannot release shared memory!"));
    } 
#ifndef USE_GNOMEUI
    else if (canvas->x_image)
        XDestroyImage(canvas->x_image);
#endif
#else
#ifndef USE_GNOMEUI
    if (canvas->x_image)
        XDestroyImage(canvas->x_image);
#endif
#endif
}

/* ------------------------------------------------------------------------- */

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->video_draw_buffer_callback = NULL;

#ifdef USE_XF86_EXTENSIONS
    canvas->fullscreenconfig
        = (fullscreenconfig_t *)lib_calloc(1, sizeof(fullscreenconfig_t));
    fullscreen_init_alloc_hooks(canvas);
#endif
}


#ifdef HAVE_XVIDEO
/* Mapping between VICE and XVideo color settings. */
struct {
  char* name;
  Atom atom;
  int min;
  int max;
  int *value;
}
xv_settings[] = {
  { "XV_SATURATION", 0, 0, 0, &video_resources.color_saturation },
  { "XV_CONTRAST",   0, 0, 0, &video_resources.color_contrast },
  { "XV_BRIGHTNESS", 0, 0, 0, &video_resources.color_brightness },
  { "XV_GAMMA",      0, 0, 0, &video_resources.color_gamma }
};
#endif

video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width,
                                    unsigned int *height, int mapped,
                                    const struct palette_s *palette)
{
    int res;
    unsigned int new_width, new_height;
    XGCValues gc_values;

    canvas->depth = x11ui_get_display_depth();
    canvas->palette = palette;

    new_width = *width;
    new_height = *height;

    if (canvas->videoconfig->doublesizex)
        new_width *= 2;

    if (canvas->videoconfig->doublesizey)
        new_height *= 2;

#ifdef HAVE_XVIDEO
    /* Request specified video format. */
    canvas->xv_format.id = fourcc;
#endif

    if (video_arch_frame_buffer_alloc(canvas, new_width, new_height) < 0) {
        return NULL;
    }

    res = x11ui_open_canvas_window(canvas, canvas->viewport->title,
                                   new_width, new_height, 1);
    if (res < 0) {
        return NULL;
    }

    if (!_video_gc)
        _video_gc = video_get_gc(&gc_values);

    canvas->width = new_width;
    canvas->height = new_height;

    ui_finish_canvas(canvas);

    if (canvas->depth > 8)
	uicolor_init_video_colors();

    video_add_handlers(canvas);

#ifdef HAVE_XVIDEO
    /* Find XVideo color setting limits. */
    if (use_xvideo && canvas->xv_image) {
        int i, j;
        int numattr = 0;
        Display *dpy = x11ui_get_display_ptr();
        XvAttribute *attr = XvQueryPortAttributes(dpy, canvas->xv_port,
                                                  &numattr);
        for (i = 0; i < sizeof(xv_settings)/sizeof(xv_settings[0]); i++) {
            xv_settings[i].atom = 0;

            for (j = 0; j < numattr; j++) {
                if (strcmp(xv_settings[i].name, attr[j].name) == 0) {
                    xv_settings[i].atom = XInternAtom(dpy, xv_settings[i].name,
                                                      False);
                    xv_settings[i].min = attr[j].min_value;
                    xv_settings[i].max = attr[j].max_value;
                    break;
                }
            }
        }

        if (attr) {
            XFree(attr);
        }

        /* Apply color settings to XVideo. */
        video_canvas_set_palette(canvas, palette);
    }
#endif

    return canvas;
}

void video_canvas_destroy(video_canvas_t *canvas)
{
#ifdef USE_XF86_EXTENSIONS
    if (canvas != NULL) {
        fullscreen_shutdown_alloc_hooks(canvas);
        lib_free(canvas->fullscreenconfig);
    }
#endif

    video_canvas_shutdown(canvas);
}


int video_canvas_set_palette(video_canvas_t *c,
                             const struct palette_s *palette)
{
    int res;
    
#ifdef HAVE_XVIDEO
    /* Apply color settings to XVideo. */
    if (use_xvideo && c->xv_image) {
        int i;

        Display *dpy = x11ui_get_display_ptr();

        for (i = 0; i < sizeof(xv_settings)/sizeof(xv_settings[0]); i++) {
            /* Map from VICE [0,2000] to XVideo [xv_min, xv_max]. */
            int v_min = 0, v_max = 2000;
            int v_zero = (v_min + v_max)/2;
            int v_range = v_max - v_min;

            int xv_zero = (xv_settings[i].min + xv_settings[i].max) / 2;
            int xv_range = xv_settings[i].max - xv_settings[i].min;

            int xv_val = (*xv_settings[i].value - v_zero)*xv_range / v_range
                         + xv_zero;

            if (!xv_settings[i].atom) {
                continue;
            }

            XvSetPortAttribute(dpy, c->xv_port, xv_settings[i].atom, xv_val);
        }
    }
#endif

    c->palette = palette;

    res = uicolor_set_palette(c, palette);
    return res;
}

/* Change the size of the canvas. */
void video_canvas_resize(video_canvas_t *canvas, unsigned int width,
                         unsigned int height)
{
    if (console_mode || vsid_mode)
        return;

    if (canvas->videoconfig->doublesizex)
        width *= 2;

    if (canvas->videoconfig->doublesizey)
        height *= 2;

    video_arch_frame_buffer_free(canvas);
    video_arch_frame_buffer_alloc(canvas, width, height);

    x11ui_resize_canvas_window(canvas->emuwindow, width, height);
    canvas->width = width;
    canvas->height = height;
    ui_finish_canvas(canvas);
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
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    Display *display;
    /*log_debug("XS%i YS%i XI%i YI%i W%i H%i PS%i", xs, ys, xi, yi, w, h,
              canvas->draw_buffer->draw_buffer_width);*/

    if (console_mode || vsid_mode)
        return;

#ifdef HAVE_XVIDEO
    if (use_xvideo && canvas->xv_image) {
        int doublesize = canvas->videoconfig->doublesizex
          && canvas->videoconfig->doublesizey;

        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;
        Window root;
        int x, y;
        unsigned int dest_w, dest_h, border_width, depth;

        display = x11ui_get_display_ptr();

        render_yuv_image(doublesize,
                         canvas->videoconfig->doublescan,
                         video_resources.pal_mode,
                         video_resources.pal_scanlineshade * 1024 / 1000,
                         canvas->xv_format,
                         &canvas->yuv_image,
                         canvas->draw_buffer->draw_buffer,
                         canvas->draw_buffer->draw_buffer_width,
                         yuv_table,
                         xs, ys, w, h,
                         xi, yi);

        XGetGeometry(display,
#ifdef USE_GNOMEUI
                     GDK_WINDOW_XWINDOW(canvas->drawable),
#else
                     canvas->drawable,
#endif
                     &root, &x, &y,
                     &dest_w, &dest_h, &border_width, &depth);

        /* Xv does subpixel scaling. Since coordinates are in integers we
           refresh the entire image to get it right. */
        display_yuv_image(display, canvas->xv_port,
#ifdef USE_GNOMEUI
                          GDK_WINDOW_XWINDOW(canvas->drawable),
                          GDK_GC_XGC(_video_gc),
#else
                          canvas->drawable, _video_gc,
#endif
                          canvas->xv_image, shminfo,
                          0, 0,
                          canvas->width, canvas->height,
                          dest_w, dest_h,
                          aspect_ratio);

        if (_video_use_xsync)
            XSync(display, False);

        return;
    }
#endif

    if (canvas->videoconfig->doublesizex) {
        xi *= 2;
        w *= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        yi *= 2;
        h *= 2;
    }

#ifdef USE_XF86_EXTENSIONS
    if (canvas->video_fullscreen_refresh_func) {
        canvas->video_fullscreen_refresh_func(canvas, xs, ys, xi, yi, w, h);
        return;
    }
#endif

    if (xi + w > canvas->width || yi + h > canvas->height) {
        log_debug("Attempt to draw outside canvas!\n"
                  "XI%i YI%i W%i H%i CW%i CH%i\n",
                  xi, yi, w, h, canvas->width, canvas->height);
        exit(-1);
    }

    _convert_func(canvas, xs, ys, xi, yi, w, h);

    /* This could be optimized away.  */
    display = x11ui_get_display_ptr();

    _refresh_func(display, canvas->drawable, _video_gc, canvas->x_image,
                  xi, yi, xi, yi, w, h, False, NULL, canvas);
    if (_video_use_xsync)
        XSync(display, False);
}

