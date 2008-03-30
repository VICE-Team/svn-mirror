/*
 * videoarch.h - X11 graphics routines.
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

#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#ifdef USE_MITSHM
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "types.h"
#include "video.h"

#ifdef HAVE_XVIDEO
#include "renderxv.h"
#endif

#ifdef USE_GNOMEUI
#include "gnome/uiarch.h"
#else
#include "xaw/uiarch.h"
#endif

#ifdef USE_GNOMEUI
#include <gdk/gdk.h>
#endif

typedef void (*video_refresh_func_t)(struct video_canvas_s *,
              int, int, int, int, unsigned int, unsigned int);

struct video_draw_buffer_callback_s;
struct palette_s;
struct fullscreenconfig_s;

struct video_canvas_s {
    unsigned int initialized;
    unsigned int width, height;
    ui_window_t emuwindow;
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    const struct palette_s *palette;
#ifdef USE_GNOMEUI
    GdkPixmap *drawable;
#else
    Window drawable;
    Colormap colormap;
#endif
    XImage *x_image;
    unsigned int depth;
#ifdef HAVE_XVIDEO
    XvImage *xv_image;
    XvPortID xv_port;
    fourcc_t xv_format;
#endif
#ifdef USE_GNOMEUI
    GdkImage *gdk_image;
#endif

#ifdef USE_MITSHM
    XShmSegmentInfo xshm_info;
    int using_mitshm;     /* True if MITSHM is used for this framebuffer. */
#endif
    GC gc;
    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
#ifdef USE_XF86_EXTENSIONS
    struct fullscreenconfig_s *fullscreenconfig;
    video_refresh_func_t video_fullscreen_refresh_func;
#endif
};
typedef struct video_canvas_s video_canvas_t;

extern GC _video_gc;
extern int _video_use_xsync;

/* ------------------------------------------------------------------------- */

extern void enable_text(void);
extern void disable_text(void);

extern void video_init_arch(void);

extern int use_mitshm;
extern int use_xvideo;

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

extern void video_add_handlers(video_canvas_t *canvas);
extern void ui_finish_canvas(video_canvas_t *c);
extern void video_convert_save_pixel(void);
extern void video_convert_restore_pixel(void);
extern void video_refresh_func(void (*rfunc)(void));
extern int video_convert_func(video_canvas_t *canvas, unsigned int width,
                              unsigned int height);
extern void video_convert_color_table(unsigned int i, BYTE *data,
                                      unsigned int dither, long col,
                                      video_canvas_t *c);
extern int video_arch_frame_buffer_alloc(video_canvas_t *canvas,
                                         unsigned int width,
                                         unsigned int height);
#endif

