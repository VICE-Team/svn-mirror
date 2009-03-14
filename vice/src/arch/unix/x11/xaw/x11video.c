/*
 * x11video.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
 *
 * Support for multiple visuals and depths by
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
#include "translate.h"
#include "types.h"
#include "ui.h"
#include "uicolor.h"
#include "util.h"
#include "video.h"
#include "videoarch.h"
#include "viewport.h"
#include "x11ui.h"
#ifdef HAVE_XVIDEO
#include "renderxv.h"
#endif


static log_t x11video_log = LOG_ERR;

#ifdef HAVE_XVIDEO
#include "video/video-resources.h"
#endif
#ifdef HAVE_OPENGL_SYNC
#include "openGL_sync.h"
#endif

#ifdef USE_MITSHM
extern int shmhandler(Display* display, XErrorEvent* err);
extern int mitshm_failed; /* will be set to true if XShmAttach() failed */
extern int shmmajor;          /* major number of MITSHM error codes */
/* Define this for additional shared memory verbosity. */
/*  #define MITSHM_DEBUG */

#ifdef MITSHM_DEBUG
#define DEBUG_MITSHM(x)         log_debug x
#else
#define DEBUG_MITSHM(x)
#endif
#endif /* USE_MITSHM */

/* Flag: Do we call `XSync()' after blitting?  */
int _video_use_xsync;

/* Flag: Do we try to use the MIT shared memory extensions?  */
static int try_mitshm;

#ifdef HAVE_XVIDEO
static unsigned int fourcc = 0;
static char *fourcc_s = NULL;

static double aspect_ratio;
static char *aspect_ratio_s = NULL;
#endif

static int set_use_xsync(int val, void *param)
{
    _video_use_xsync = val;
    return 0;
}

static int set_try_mitshm(int val, void *param)
{
    try_mitshm = val;
    return 0;
}

#ifdef HAVE_XVIDEO
static int set_fourcc(const char *val, void *param)
{
    if (util_string_set(&fourcc_s, val))
        return 0;

    if (fourcc_s != NULL && strlen(fourcc_s) == 4) {
        memcpy(&fourcc, fourcc_s, 4);
    } else {
        fourcc = 0;
    }
    
    return 0;
}

static int set_aspect_ratio(const char *val, void *param)
{
    if (val) {
        char *endptr;

        util_string_set(&aspect_ratio_s, val);

        aspect_ratio = strtod(val, &endptr);
        if (val == endptr) {
            aspect_ratio = 1.0;
        }
        else if (aspect_ratio < 0.8) {
            aspect_ratio = 0.8;
        }
        else if (aspect_ratio > 1.2) {
            aspect_ratio = 1.2;
        }
    } else {
        util_string_set(&aspect_ratio_s, "1.0");
        aspect_ratio = 1.0;
    }

    return 0;
}
#endif

/* Video-related resources.  */
static const resource_string_t resources_string[] = {
#ifdef HAVE_XVIDEO
    { "FOURCC", "", RES_EVENT_NO, NULL,
      &fourcc_s, set_fourcc, NULL },
    { "AspectRatio", "1.0", RES_EVENT_NO, NULL,
      &aspect_ratio_s, set_aspect_ratio, NULL },
#endif
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "UseXSync", 1, RES_EVENT_NO, NULL,
      &_video_use_xsync, set_use_xsync, NULL },
      /* turn MITSHM on by default */
    { "MITSHM", 1, RES_EVENT_NO, NULL,
      &try_mitshm, set_try_mitshm, NULL },
    { NULL }
};

int video_arch_resources_init(void)
{
#ifdef HAVE_OPENGL_SYNC
    openGL_register_resources();
#endif
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void video_arch_resources_shutdown(void)
{
#ifdef HAVE_XVIDEO
    lib_free(aspect_ratio_s);
    lib_free(fourcc_s);
#endif
}

/* ------------------------------------------------------------------------- */

/* Video-related command-line options.  */
static const cmdline_option_t cmdline_options[] = {
    { "-xsync", SET_RESOURCE, 0,
      NULL, NULL, "UseXSync", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Call `XSync()' after updating the emulation window") },
    { "+xsync", SET_RESOURCE, 0,
      NULL, NULL, "UseXSync", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Do not call `XSync()' after updating the emulation window") },
    { "-mitshm", SET_RESOURCE, 0,
      NULL, NULL, "MITSHM", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Use shared memory") },
    { "+mitshm", SET_RESOURCE, 0,
      NULL, NULL, "MITSHM", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Never use shared memory (slower)") },
#ifdef HAVE_XVIDEO
    { "-fourcc", SET_RESOURCE, 1,
      NULL, NULL, "FOURCC", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      N_("<fourcc>"), N_("Request YUV FOURCC format") },
    { "-aspect", SET_RESOURCE, 1,
      NULL, NULL, "AspectRatio", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      N_("<aspect ratio>"), N_("Set aspect ratio (0.8 - 1.2)") },
#endif
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

#if !defined(__NETBSD__)
static GC _video_gc;
#else
GC _video_gc;
#endif
static void (*_refresh_func)();

/* This is set to 1 if the Shared Memory Extensions can actually be used. */
int use_mitshm = 0;

/* The RootWindow of our screen. */
/* static Window root_window; */


/* ------------------------------------------------------------------------- */

void video_convert_color_table(unsigned int i, BYTE *data, long col,
                               video_canvas_t *canvas)
{
#ifdef HAVE_XVIDEO
    if (canvas->videoconfig->hwscale && canvas->xv_image) {
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
}

static GC video_get_gc(XGCValues *gc_values);

int video_init(void)
{
    XGCValues gc_values;
    Display *display;

    _video_gc = video_get_gc(&gc_values);
    display = x11ui_get_display_ptr();

     x11video_log = log_open("X11Video");

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
            log_warning(x11video_log,
                        "The MITSHM extension is not supported "
                        "on this display.");
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
    if (canvas->xv_image) {
#if defined(__QNX__) || defined(MINIX_SUPPORT)
        XShmSegmentInfo* shminfo = NULL;
#else
        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;
#endif

        display = x11ui_get_display_ptr();
	destroy_yuv_image(display, canvas->xv_image, shminfo);
	return;
    }
#endif

#ifdef HAVE_FULLSCREEN
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
            log_error(x11video_log, "Cannot release shared memory!");
    } 
    else if (canvas->x_image)
        XDestroyImage(canvas->x_image);
#else
    if (canvas->x_image)
        XDestroyImage(canvas->x_image);
#endif
}

/* ------------------------------------------------------------------------- */

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->video_draw_buffer_callback = NULL;

#ifdef HAVE_FULLSCREEN
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

static void init_xv_settings(video_canvas_t *canvas)
{
    /* Find XVideo color setting limits. */
    if (canvas->videoconfig->hwscale && canvas->xv_image) {
        int i, j;
        int numattr = 0;
        Display *dpy = x11ui_get_display_ptr();
        XvAttribute *attr = XvQueryPortAttributes(dpy, canvas->xv_port,
                                                  &numattr);
        for (i = 0; i < (int)(sizeof(xv_settings)/sizeof(xv_settings[0]));
            i++) {
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
        video_canvas_set_palette(canvas, canvas->palette);
    }
}
#endif

static void video_refresh_func(void (*rfunc)(void));


static int video_arch_frame_buffer_alloc(video_canvas_t *canvas, unsigned int width,
                                  unsigned int height)
{
    int sizeofpixel = sizeof(BYTE);
    Display *display;
#ifdef USE_MITSHM
    int (*olderrorhandler)(Display *, XErrorEvent *);
    int dummy;
#endif

#ifdef USE_MITSHM
    canvas->using_mitshm = use_mitshm;
#endif

    display = x11ui_get_display_ptr();

    /* sizeof(PIXEL) is not always what we are using. I guess this should
       be checked from the XImage but I'm lazy... */
    if (canvas->depth > 8)
        sizeofpixel *= 2;
    if (canvas->depth > 16)
        sizeofpixel *= 2;

#ifdef HAVE_XVIDEO
    canvas->xv_image = NULL;

    if (canvas->videoconfig->hwscale
        && (canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_1X1
            || canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_2X2))
    {
#if defined(__QNX__) || defined(MINIX_SUPPORT)
        XShmSegmentInfo* shminfo = NULL;
#else
        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;
#endif

        canvas->xv_image = create_yuv_image(display, canvas->xv_port,
                                            canvas->xv_format, width, height,
                                            shminfo);
        if (!(canvas->xv_image))
            return -1;

        /* Copy data for architecture independent rendering. */
        canvas->yuv_image.width = canvas->xv_image->width;
        canvas->yuv_image.height = canvas->xv_image->height;
        canvas->yuv_image.data_size = canvas->xv_image->data_size;
        canvas->yuv_image.num_planes = canvas->xv_image->num_planes;
        canvas->yuv_image.pitches = canvas->xv_image->pitches;
        canvas->yuv_image.offsets = canvas->xv_image->offsets;
        canvas->yuv_image.data = (unsigned char *)canvas->xv_image->data;

        log_message(x11video_log,
                    "Successfully initialized using XVideo (%dx%d %.4s).",
                    width, height, canvas->xv_format.label);

        return 0;
    }
#endif

    /* Round up to 32-bit boundary (used in XCreateImage). */
    width = (width + 3) & ~0x3;

#ifdef USE_MITSHM
tryagain:
    if (canvas->using_mitshm) {
        DEBUG_MITSHM(("frame_buffer_alloc(): allocating XImage with MITSHM, "
                      "%d x %d pixels...", width, height));
        canvas->x_image = XShmCreateImage(display, visual, canvas->depth,
                                          ZPixmap, NULL, &(canvas->xshm_info),
                                          width, height);
        if (!canvas->x_image) {
            log_warning(x11video_log,
                        "Cannot allocate XImage with XShm; falling back to non MITSHM extension mode.");
            canvas->using_mitshm = 0;
            goto tryagain;
        }
        DEBUG_MITSHM(("Done."));
        DEBUG_MITSHM(("frame_buffer_alloc(): shmgetting %ld bytes...",
                      (long)canvas->x_image->bytes_per_line
                      * canvas->x_image->height));
        canvas->xshm_info.shmid = shmget(IPC_PRIVATE,
                                  canvas->x_image->bytes_per_line
                                  * canvas->x_image->height, IPC_CREAT | 0604);
        if (canvas->xshm_info.shmid == -1) {
            log_warning(x11video_log,
                        "Cannot get shared memory; falling back to non MITSHM extension mode.");
            XDestroyImage(canvas->x_image);
            canvas->using_mitshm = 0;
            goto tryagain;
        }
        DEBUG_MITSHM(("Done, id = 0x%x.", i->xshm_info.shmid));
        DEBUG_MITSHM(("frame_buffer_alloc(): getting address... "));
        canvas->xshm_info.shmaddr = shmat(canvas->xshm_info.shmid, 0, 0);
        canvas->x_image->data = canvas->xshm_info.shmaddr;
        if (canvas->xshm_info.shmaddr == (char *)-1) {
            log_warning(x11video_log,
                        "Cannot get shared memory address; falling back to non MITSHM extension mode.");
            shmctl(canvas->xshm_info.shmid,IPC_RMID,0);
            XDestroyImage(canvas->x_image);
            canvas->using_mitshm = 0;
            goto tryagain;
        }
        DEBUG_MITSHM(("0x%lx OK.", (unsigned long) i->xshm_info.shmaddr));
        canvas->xshm_info.readOnly = True;
        mitshm_failed = 0;

        XQueryExtension(display,"MIT-SHM",&shmmajor,&dummy,&dummy);
        olderrorhandler = XSetErrorHandler(shmhandler);

        if (!XShmAttach(display, &(canvas->xshm_info))) {
            log_warning(x11video_log,
                        "Cannot attach shared memory; falling back to non MITSHM extension mode.");
            shmdt(canvas->xshm_info.shmaddr);
            shmctl(canvas->xshm_info.shmid,IPC_RMID,0);
            XDestroyImage(canvas->x_image);
            canvas->using_mitshm = 0;
            goto tryagain;
        }

        /* Wait for XShmAttach to fail or to succede. */
        XSync(display,False);
        XSetErrorHandler(olderrorhandler);

        /* Mark memory segment for automatic deletion. */
        shmctl(canvas->xshm_info.shmid, IPC_RMID, 0);

        if (mitshm_failed) {
            log_warning(x11video_log,
                        "Cannot attach shared memory; falling back to non MITSHM extension mode.");
            shmdt(canvas->xshm_info.shmaddr);
            XDestroyImage(canvas->x_image);
            canvas->using_mitshm = 0;
            goto tryagain;
        }

        DEBUG_MITSHM((_("MITSHM initialization succeed.\n")));
        video_refresh_func((void (*)(void))XShmPutImage);
    } else
#endif
    {                           /* !i->using_mitshm */
        char *data;

        data = (char *)lib_malloc(width * height * sizeofpixel);

        if (data == NULL)
            return -1;

        canvas->x_image = XCreateImage(display, visual, canvas->depth, ZPixmap,
                                       0, data, width, height, 32, 0);
        if (!canvas->x_image)
            return -1;

        video_refresh_func((void (*)(void))XPutImage);
    }

#ifdef USE_MITSHM
    log_message(x11video_log, "Successfully initialized%s shared memory.",
                (canvas->using_mitshm) ? ", using" : " without");

    if (!(canvas->using_mitshm))
        log_warning(x11video_log, "Performance will be poor.");
#else
    log_message(x11video_log,
                "Successfully initialized without shared memory.");
#endif

    return 0;
}

static GC video_get_gc(XGCValues *gc_values)
{
    Display *display;

    display = x11ui_get_display_ptr();

    return XCreateGC(display, XtWindow(_ui_top_level), 0, gc_values);
}


/* Make the canvas visible. */
void video_canvas_map(video_canvas_t *s)
{
    Display *display;

    display = x11ui_get_display_ptr();

    XMapWindow(display, s->drawable);
    XFlush(display);
}

/* Make the canvas not visible. */
void video_canvas_unmap(video_canvas_t *s)
{
    Display *display;

    display = x11ui_get_display_ptr();

    XUnmapWindow(display, s->drawable);
    XFlush(display);
}

static void ui_finish_canvas(video_canvas_t *c)
{
    c->drawable = XtWindow(c->emuwindow);
}



video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width,
                                    unsigned int *height, int mapped)
{
    int res;
    unsigned int new_width, new_height;
    XGCValues gc_values;

    canvas->depth = x11ui_get_display_depth();

    new_width = *width;
    new_height = *height;

    if (canvas->videoconfig->doublesizex)
        new_width *= 2;

    if (canvas->videoconfig->doublesizey)
        new_height *= 2;

#ifdef HAVE_XVIDEO
    /* Request specified video format. */
    canvas->xv_format.id = fourcc;

    if (!find_yuv_port(x11ui_get_display_ptr(), &canvas->xv_port, &canvas->xv_format))
    {
        if (canvas->videoconfig->hwscale) {
            log_message(x11video_log, "HW scaling not available");
            canvas->videoconfig->hwscale = 0;
        }
        resources_set_int("HwScalePossible", 0);
    }
#else
    resources_set_int("HwScalePossible", 0);
#endif

    if (video_arch_frame_buffer_alloc(canvas, new_width, new_height) < 0) {
        return NULL;
    }

    res = ui_open_canvas_window(canvas, canvas->viewport->title,
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

#ifdef HAVE_XVIDEO
    init_xv_settings(canvas);
#endif
#ifdef HAVE_OPENGL_SYNC
    openGL_sync_init(canvas);
#endif

    return canvas;
}

void video_canvas_destroy(video_canvas_t *canvas)
{
#ifdef HAVE_FULLSCREEN
    if (canvas != NULL) {
        fullscreen_shutdown_alloc_hooks(canvas);
        lib_free(canvas->fullscreenconfig);
    }
#endif

    video_canvas_shutdown(canvas);
}


int video_canvas_set_palette(video_canvas_t *c, struct palette_s *palette)
{
#ifdef HAVE_XVIDEO
    /* Apply color settings to XVideo. */
    if (c->videoconfig->hwscale && c->xv_image) {
        int i;

        Display *dpy = x11ui_get_display_ptr();

        for (i = 0; i < (int)(sizeof(xv_settings) / sizeof(xv_settings[0]));
            i++) {
            /* Map from VICE [0,2000] to XVideo [xv_min, xv_max]. */
            int v_min = 0, v_max = 2000;
            int v_zero = (v_min + v_max) / 2;
            int v_range = v_max - v_min;

            int xv_zero = (xv_settings[i].min + xv_settings[i].max) / 2;
            int xv_range = xv_settings[i].max - xv_settings[i].min;

            int xv_val = (*xv_settings[i].value - v_zero) * xv_range / v_range
                         + xv_zero;

            if (!xv_settings[i].atom) {
                continue;
            }

            XvSetPortAttribute(dpy, c->xv_port, xv_settings[i].atom, xv_val);
        }
    }
#endif

    c->palette = palette;

    return uicolor_set_palette(c, palette);
}

/* Change the size of the canvas. */
void video_canvas_resize(video_canvas_t *canvas, unsigned int width,
                         unsigned int height)
{
    if (console_mode || vsid_mode)
        return;

#ifdef HAVE_XVIDEO
    if (canvas->videoconfig->hwscale) {
	struct geometry_s *geometry = canvas->geometry;
	width = geometry->gfx_size.width + geometry->gfx_position.x * 2;
	height =
	  geometry->last_displayed_line - geometry->first_displayed_line + 1;
    }
#endif
    if (canvas->videoconfig->doublesizex)
        width *= 2;

    if (canvas->videoconfig->doublesizey)
        height *= 2;

    video_arch_frame_buffer_free(canvas);
    video_arch_frame_buffer_alloc(canvas, width, height);

    x11ui_resize_canvas_window(canvas->emuwindow, width, height, canvas->videoconfig->hwscale);
    canvas->width = width;
    canvas->height = height;

    video_canvas_redraw_size(canvas, width, height);

    ui_finish_canvas(canvas);

#ifdef HAVE_XVIDEO
    init_xv_settings(canvas);
#endif
}


/* ------------------------------------------------------------------------- */

static void video_refresh_func(void (*rfunc)(void))
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
#if 0
    log_debug("XS%i YS%i XI%i YI%i W%i H%i PS%i", xs, ys, xi, yi, w, h,
              canvas->draw_buffer->draw_buffer_width);
#endif

    if (console_mode || vsid_mode)
        return;

#ifdef HAVE_XVIDEO
    if (canvas->videoconfig->hwscale && canvas->xv_image) {
        int doublesize = canvas->videoconfig->doublesizex
          && canvas->videoconfig->doublesizey;

#if defined(__QNX__) || defined(MINIX_SUPPORT)
        XShmSegmentInfo* shminfo = NULL;
#else
        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;
#endif
        Window root;
        int x, y;
        unsigned int dest_w, dest_h, border_width, depth;

        display = x11ui_get_display_ptr();

        render_yuv_image(doublesize,
                         canvas->viewport,
                         video_resources.delayloop_emulation,
                         video_resources.pal_blur * 64 / 1000,
                         video_resources.pal_scanlineshade * 1024 / 1000,
                         canvas->xv_format,
                         &canvas->yuv_image,
                         canvas->draw_buffer->draw_buffer,
                         canvas->draw_buffer->draw_buffer_width,
                         canvas->videoconfig,
                         xs, ys, w, h,
                         xi, yi);

        XGetGeometry(display,
                     canvas->drawable,
                     &root, &x, &y,
                     &dest_w, &dest_h, &border_width, &depth);

        /* Xv does subpixel scaling. Since coordinates are in integers we
           refresh the entire image to get it right. */
        display_yuv_image(display, canvas->xv_port,
                          canvas->drawable, _video_gc,
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

#ifdef HAVE_FULLSCREEN
    if (canvas->video_fullscreen_refresh_func) {
        canvas->video_fullscreen_refresh_func(canvas, xs, ys, xi, yi, w, h);
        return;
    }
#endif

    if (xi + w > canvas->width || yi + h > canvas->height) {
        log_debug("Attempt to draw outside canvas!\n"
                  "XI%i YI%i W%i H%i CW%i CH%i\n",
                  xi, yi, w, h, canvas->width, canvas->height);
	return;			/* this makes `-fullscreen -80col' work 
				   XXX fix me some day */
    }

    video_canvas_render(canvas, (BYTE *)canvas->x_image->data,
                        w, h, xs, ys, xi, yi,
                        canvas->x_image->bytes_per_line,
                        canvas->x_image->bits_per_pixel);


    /* This could be optimized away.  */
    display = x11ui_get_display_ptr();

    _refresh_func(display, canvas->drawable, _video_gc, canvas->x_image,
                  xi, yi, xi, yi, w, h, False, NULL, canvas);
    
    if (_video_use_xsync)
        XSync(display, False);
}
