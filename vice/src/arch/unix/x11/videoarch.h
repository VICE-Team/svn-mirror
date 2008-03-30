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

#include "fullscreen.h"
#include "video.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#ifdef USE_MITSHM
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#ifdef HAVE_XVIDEO
#include "renderxv.h"
#endif

#include "types.h"
#include "ui.h"

#ifdef USE_GNOMEUI
#include "gnome/uiarch.h"
#else
#include "xaw/uiarch.h"
#endif

#ifdef USE_GNOMEUI
#include <gdk/gdk.h>
#endif

struct video_draw_buffer_callback_s;

struct video_canvas_s {
    unsigned int width, height;
    ui_window_t emuwindow;
    struct video_render_config_s *videoconfig;
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
};
typedef struct video_canvas_s video_canvas_t;

/* Double buffering might be available with DGA */
#ifdef USE_XF86_DGA2_EXTENSIONS
#define CANVAS_USES_TRIPLE_BUFFERING(c) fullscreen_is_enabled
#else
#define CANVAS_USES_TRIPLE_BUFFERING(c) 0
#endif

typedef ui_exposure_handler_t canvas_redraw_t;

extern GC _video_gc;
extern int _video_use_xsync;

/* ------------------------------------------------------------------------- */

extern void enable_text(void);
extern void disable_text(void);

extern void video_init_arch(void);

extern int use_mitshm;
extern int use_xvideo;

#ifdef USE_MITSHM
extern int shmhandler(Display* display,XErrorEvent* err);
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

struct palette_s;
struct raster_s;

extern void video_add_handlers(video_canvas_t *canvas);
extern void ui_finish_canvas(video_canvas_t *c);
extern void video_convert_save_pixel(void);
extern void video_convert_restore_pixel(void);
extern void video_refresh_func(void (*rfunc)(void));
extern int video_convert_func(video_canvas_t *canvas, unsigned int width,
                              unsigned int height);
extern void video_register_raster(struct raster_s *raster);

extern void video_convert_color_table(unsigned int i, BYTE *data,
                                      unsigned int dither, long col,
                                      video_canvas_t *c);
extern int video_arch_frame_buffer_alloc(video_canvas_t *canvas,
                                         unsigned int width,
                                         unsigned int height);

#ifdef USE_XF86_DGA2_EXTENSIONS
#define fullscreen_on() fullscreen_mode_on_restore()
#define fullscreen_off() fullscreen_mode_off_restore()
#define fullscreen_update() fullscreen_mode_update()
extern void fullscreen_set_canvas(video_canvas_t *c);
extern void fullscreen_set_palette(video_canvas_t *vc,
                                   const struct palette_s *p);
extern void fullscreen_resize(int w, int h);
extern int fs_draw_buffer_alloc(struct video_canvas_s *c,
				BYTE **db, unsigned int w, unsigned int h,
				unsigned int *pitch);
extern void fs_draw_buffer_free(struct video_canvas_s *c, BYTE *db);
extern void fs_draw_buffer_clear(struct video_canvas_s *, BYTE *db, 
				 BYTE value, unsigned int w, unsigned int h,
				 unsigned int pitch);
extern void fullscreen_create_menus(void);

#else
#define fullscreen_on()
#define fullscreen_off()
#define fullscreen_update()
#endif

#endif /* !_VIDEOARCH_H */

