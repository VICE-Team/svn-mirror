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

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "log.h"
#include "utils.h"
#include "videoarch.h"
#include "video.h"
#include "x11ui.h"
#ifdef HAVE_XVIDEO
#include "renderxv.h"
#endif

static log_t x11video_log = LOG_ERR;

void video_init_arch(void)
{
    x11video_log = log_open("X11Video");
}

int video_arch_frame_buffer_alloc(video_canvas_t *canvas, unsigned int width,
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

    if (!use_xvideo) {
        /* Round up to 32-bit boundary (used in XCreateImage). */
        width = (width + 3) & ~0x3;
    }

    /* sizeof(PIXEL) is not always what we are using. I guess this should
       be checked from the XImage but I'm lazy... */
    if (canvas->depth > 8)
        sizeofpixel *= 2;
    if (canvas->depth > 16)
        sizeofpixel *= 2;

#ifdef HAVE_XVIDEO
    if (use_xvideo) {
        XShmSegmentInfo* shminfo = use_mitshm ? &canvas->xshm_info : NULL;

        if (!find_yuv_port(display, &canvas->xv_port, &canvas->xv_format))
            return -1;

        canvas->xv_image = create_yuv_image(display, canvas->xv_port,
                                            canvas->xv_format, width, height,
                                            shminfo);
        if (!(canvas->xv_image))
            return -1;

        log_message(x11video_log,
                    _("Successfully initialized using XVideo (%dx%d %.4s)."),
                    width, height, canvas->xv_format.label);

        return 0;
    }
#endif
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
                        _("Cannot allocate XImage with XShm; falling back to non MITSHM extension mode."));
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
                        _("Cannot get shared memory; falling back to non MITSHM extension mode."));
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
                        _("Cannot get shared memory address; falling back to non MITSHM extension mode."));
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
                        _("Cannot attach shared memory; falling back to non MITSHM extension mode."));
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
                        _("Cannot attach shared memory; falling back to non MITSHM extension mode."));
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

        data = (char *)xmalloc(width * height * sizeofpixel);

        if (data == NULL)
            return -1;

        canvas->x_image = XCreateImage(display, visual, canvas->depth, ZPixmap,
                                       0, data, width, height, 32, 0);
        if (!canvas->x_image)
            return -1;

        video_refresh_func((void (*)(void))XPutImage);
    }

#ifdef USE_MITSHM
    log_message(x11video_log, _("Successfully initialized%s shared memory."),
                (canvas->using_mitshm) ? _(", using") : _(" without"));

    if (!(canvas->using_mitshm))
        log_warning(x11video_log, _("Performance will be poor."));
#else
    log_message(x11video_log,
                _("Successfully initialized without shared memory."));
#endif

    if (video_convert_func(canvas, width, height) < 0)
        return -1;

#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_set_canvas(canvas);
#endif

    return 0;
}

GC video_get_gc(XGCValues *gc_values)
{
    Display *display;

    display = x11ui_get_display_ptr();

    return XCreateGC(display, XtWindow(_ui_top_level), 0, gc_values);
}

void video_add_handlers(video_canvas_t *canvas)
{
    XtAddEventHandler(canvas->emuwindow,
                      (EnterWindowMask | LeaveWindowMask | KeyReleaseMask
                      | KeyPressMask), True,
                      (XtEventHandler)kbd_event_handler, NULL);

    /* FIXME: ...REALLY ugly... */
    XtAddEventHandler(XtParent(canvas->emuwindow),
                      (EnterWindowMask | LeaveWindowMask
                      | KeyReleaseMask | KeyPressMask), True,
                      (XtEventHandler)kbd_event_handler, NULL);
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

void ui_finish_canvas(video_canvas_t *c)
{
    c->drawable = XtWindow(c->emuwindow);
}

