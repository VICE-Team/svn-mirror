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
#include "raster/raster.h"
#include "palette.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#ifdef USE_MITSHM
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "types.h"
#include "ui.h"

#ifdef USE_GNOMEUI
#include <gdk/gdk.h>		
#endif

struct canvas_s {
    unsigned int width, height;
    ui_window_t emuwindow;
#ifdef USE_GNOMEUI
    GdkPixmap *drawable;
#else
    Window drawable;
    Colormap colormap;
#endif

};
typedef struct canvas_s canvas_t;

/* Double buffering might be available with DGA */
#ifdef USE_XF86_DGA2_EXTENSIONS
#define CANVAS_USES_TRIPLE_BUFFERING(c) fullscreen_is_enabled
#else
#define CANVAS_USES_TRIPLE_BUFFERING(c) 0
#endif

struct video_frame_buffer_s {
    XImage *x_image;
#ifdef USE_GNOMEUI
    GdkImage *gdk_image;
    canvas_t *canvas;
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
typedef struct video_frame_buffer_s video_frame_buffer_t;

typedef PIXEL *frame_buffer_ptr_t;
typedef ui_exposure_handler_t canvas_redraw_t;

extern GC _video_gc;
extern int _video_use_xsync;

#if X_DISPLAY_DEPTH == 0
#define VIDEO_FRAME_BUFFER_START(i)         ((i)->tmpframebuffer)
#define VIDEO_FRAME_BUFFER_LINE_SIZE(i)     ((i)->tmpframebufferlinesize)
#define VIDEO_FRAME_BUFFER_LINE_START(i, n) ((i)->tmpframebuffer \
                                            + (n) * (i)->tmpframebufferlinesize)
#else
#define VIDEO_FRAME_BUFFER_START(i)         ((PIXEL *)((i)->x_image->data))
#define VIDEO_FRAME_BUFFER_LINE_SIZE(i)     ((i)->x_image->bytes_per_line \
                                            / sizeof(PIXEL))
#define VIDEO_FRAME_BUFFER_LINE_START(i, n) (PIXEL *)((i)->x_image->data \
					    + (n) * (i)->x_image->bytes_per_line)
#endif

/* ------------------------------------------------------------------------- */

extern void enable_text(void);
extern void disable_text(void);

extern void video_init_arch(void);

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

struct palette_s;

extern void video_add_handlers(ui_window_t w);
extern void ui_finish_canvas(canvas_t *c);
extern void video_convert_save_pixel(void);
extern void video_convert_restore_pixel(void);
extern void video_refresh_func(void (*rfunc)(void));
extern int video_convert_func(video_frame_buffer_t *i, int depth,
                              unsigned int width, unsigned int height);
extern void video_register_raster(raster_t *raster);

#ifdef USE_COLOR_MANAGEMENT
extern void video_convert_color_table(unsigned int i, PIXEL *pixel_return,
                                      PIXEL *data, unsigned int bits_per_pixel,
                                      unsigned int dither, long col);
#else
extern void video_convert_color_table(int i, PIXEL *pixel_return, PIXEL *data,
                                      XImage *im,
                                      const struct palette_s *palette,
                                      long col, int depth);
#endif

#ifdef USE_XF86_DGA2_EXTENSIONS
#define fullscreen_on() fullscreen_mode_on_restore()
#define fullscreen_off() fullscreen_mode_off_restore()
#define fullscreen_update() fullscreen_mode_update()
extern void fullscreen_set_raster(raster_t *raster);
extern void fullscreen_set_framebuffer(video_frame_buffer_t *fb);
extern void fullscreen_set_palette(palette_t *p, PIXEL *pixel_return);
#else
#define fullscreen_on() 
#define fullscreen_off()
#define fullscreen_update() 
#endif

#endif /* !_VIDEOARCH_H */

