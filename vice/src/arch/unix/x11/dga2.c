/*
 * dga2.c
 *
 * Written by
 *  Martin Pottendorfer <pottendo@utanet.at>
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

/*
#define DGA2_DEBUG_BUFFER
#define FS_TRACE_REFRESH
#define DGA2_DEBUG
*/

#include "vice.h"

#ifndef USE_XF86_DGA2_EXTENSIONS
#error "DGA2 extension not available in config.h, check config.log"
#endif


#ifdef DGA2_DEBUG
static void dump_fb(char *wo);
#endif
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <X11/extensions/xf86dga.h>
#define DGA_MINMAJOR 2
#define DGA_MINMINOR 0

#include "kbd.h"
#include "resources.h"
#include "raster/raster.h"
#include "palette.h"
#include "lib.h"
#include "log.h"
#include "ui.h"
#include "uimenu.h"
#include "uisettings.h"
#include "video.h"
#include "videoarch.h"
#include "vsyncapi.h"
#include "x11ui.h"
#include "fullscreenarch.h"
#include "dga2.h"

typedef struct {
  int modeindex;
  char name[30];
} fs_bestvideomode_t;

static fs_bestvideomode_t fs_bestmodes[10];

/* dga v2 */
static XDGADevice* dgadev=NULL;
static XDGAMode *fs_allmodes_dga2;
#ifdef FS_PIXMAP_DGA
static GC gcContext;
#endif

static Colormap cm;
static void_hook_t old_ui_hook;

static int fs_vidmodecount;
static video_canvas_t *dga2_cached_canvas;
static int canvas_width, canvas_height;
static BYTE *fs_fb_data;
static int fs_fb_bpl, fs_cached_db_width, fs_depth;
static int fs_bestmode_counter;
static const palette_t *canvas_palette;
static DWORD *fs_saved_colors;
static int dga2_is_available = 0;
static int dga2_is_enabled = 0;
static int dga2_is_suspended = 0;
static int fs_use_fs_at_start = 0;
static int timeout;
static int fullscreen_is_enabled_restore;
static int EventBase, ErrorBase;

int request_fs_mode = 0;
char *dga2_selected_videomode_at_start;
unsigned int fs_width, fs_height;

static unsigned char *fb_addr, *fb_dump;
static unsigned char *fb_render_target;
static unsigned offs, pitch;

#define BB_DEPTH 2
static unsigned char *fb_page[BB_DEPTH];
static unsigned int fb_offs[BB_DEPTH];
static unsigned int fb_ybegin[BB_DEPTH];
static int fb_current_page = 0;
static int fb_ybegin_static = 0;

static Display *display;
extern int screen;

static int dga2_init_internal(void);
static void dga2_dispatch_events(void);
static void dga2_dispatch_events_2(void);
static int dga2_request_set_mode(int enable, void *p);
static int dga2_draw_buffer_alloc(struct video_canvas_s *c,
                                  BYTE **draw_buffer, unsigned int w,
                                  unsigned int h, unsigned int *pitch);
static void dga2_draw_buffer_free(struct video_canvas_s *c, BYTE *draw_buffer);
static void dga2_draw_buffer_clear(struct video_canvas_s *c,
                                   BYTE *draw_buffer, BYTE value,
                                   unsigned int w, unsigned int h,
                                   unsigned int pitch);
static int dga2_set_mode(resource_value_t v, void *param);
static log_t dga_log = LOG_ERR;

/* public DGA2 interface functions ------------------------------------- */

int dga2_init(void)
{
    if (dga_log == LOG_ERR)
        dga_log = log_open("DGA2");

    dga2_is_available = dga2_init_internal();

#if 0
    fullscreen_set_bestmode(dga2_selected_videomode_at_start, NULL);
    if (dga2_selected_videomode == -1 && fs_bestmode_counter > 0)
        dga2_selected_videomode = fs_bestmodes[0].modeindex;

    if (fs_use_fs_at_start) {
        fullscreen_mode_on();
    }
#endif
    return dga2_is_available;
}

int dga2_init_alloc_hooks(struct video_canvas_s *canvas)
{
    canvas->video_draw_buffer_callback =
        lib_malloc(sizeof(struct video_draw_buffer_callback_s));
    canvas->video_draw_buffer_callback->draw_buffer_alloc =
        dga2_draw_buffer_alloc;
    canvas->video_draw_buffer_callback->draw_buffer_free =
        dga2_draw_buffer_free;
    canvas->video_draw_buffer_callback->draw_buffer_clear =
        dga2_draw_buffer_clear;
    return 0;
}

void dga2_shutdown_alloc_hooks(struct video_canvas_s *canvas)
{
    lib_free(canvas->video_draw_buffer_callback);
}

int dga2_available(void)
{
    return dga2_is_available;
}

int dga2_enable(struct video_canvas_s *canvas, int enable)
{
    dga2_request_set_mode(enable, NULL);
    if (dga2_cached_canvas != canvas)
        dga2_cached_canvas = canvas;

    return 0;
}

int dga2_mode(struct video_canvas_s *canvas, int mode)
{
    if (mode < 0 || dga2_available == 0)
        return 0;
    log_message(dga_log, _("Selected mode: %s"), fs_bestmodes[mode].name);
    canvas->fullscreenconfig->mode = mode;

    return 0;
}

void dga2_shutdown(void)
{
/*     dga2_mode_off(); */
    XFree(fs_allmodes_dga2);
}

void dga2_suspend(int level)
{
   if (!dga2_is_enabled)
       return;

   if (dga2_is_suspended)
       return;

   log_message(dga_log, "suspending dga2 fullscreen level: %d", level);
   dga2_is_suspended = level;
   XDGASync(display, screen);
   x11kbd_focus_change();
   dga2_dispatch_events_2();
   dga2_set_mode(0, NULL);
}

void dga2_resume(void)
{
    if (dga2_is_enabled)
        return;

    if (!dga2_is_suspended)
        return;

    dga2_is_suspended = 0;
    x11kbd_focus_change();
    dga2_set_mode((resource_value_t) 1, NULL);
}

static void *mode_callback;
void dga2_mode_callback(void *callback)
{
    mode_callback = callback;
}

void dga2_menu_create(struct ui_menu_entry_s *menu)
{
    unsigned int i;
    ui_menu_entry_t *resolutions_submenu;

    resolutions_submenu = (ui_menu_entry_t *)lib_calloc(sizeof(ui_menu_entry_t),
                          (size_t)(fs_bestmode_counter + 1));

    for (i = 0; i < fs_bestmode_counter; i++) {
        resolutions_submenu[i].string =
            (ui_callback_data_t)lib_msprintf("*%s", fs_bestmodes[i].name);
        resolutions_submenu[i].callback = (ui_callback_t)mode_callback;
        resolutions_submenu[i].callback_data = (ui_callback_data_t)i;
    }

    for (i = 0; menu[i].string; i++) {
        if (strncmp(menu[i].string, "DGA2", 4) == 0) {
            if (fs_bestmode_counter > 0)
                menu[i].sub_menu = resolutions_submenu;
            break;
        }
    }
}

void dga2_menu_shutdown(struct ui_menu_entry_s *menu)
{
    unsigned int i;
    ui_menu_entry_t *resolutions_submenu = NULL;

    if (fs_bestmode_counter == 0)
        return;

    for (i = 0; menu[i].string; i++) {
        if (strncmp(menu[i].string, "DGA2", 4) == 0) {
            resolutions_submenu = menu[i].sub_menu;
            break;
        }
    }

    menu[i].sub_menu = NULL;

    if (resolutions_submenu != NULL) {
        for (i = 0; i < fs_bestmode_counter; i++)
            lib_free(resolutions_submenu[i].string);
    }

    lib_free(resolutions_submenu);
}

void dga2_set_mouse_timeout(void)
{
    return;
}

/* DGA2 private parts --------------------------------------------------- */
#ifdef FS_TRACE_REFRESH
static void ts_diff(struct timespec *ts1, struct timespec *ts2,
             struct timespec *d)
{
    d->tv_nsec = ts2->tv_nsec - ts1->tv_nsec;
    d->tv_sec = ts2->tv_sec - ts1->tv_sec;
    if (d->tv_nsec < 0) {
        d->tv_nsec += 1000000000L;
        d->tv_sec--;
    }
}
#endif

static void alarm_timeout(int signo) {
    volatile int i = 1;

    XDGASetMode(display, screen, 0);
    XDGACloseFramebuffer(display, screen);
    dga2_is_enabled = 0;
    while (i)
    {
        log_message(dga_log, _("Attach debugger to pid %d..."),
                    getpid());
        sleep(1);               /* Set breakpoint here to debug */
    }
}

static void set_alarm_timeout(void) {
    if (signal(SIGSEGV, alarm_timeout) == SIG_ERR)
        return;
#ifdef DGA2_DEBUG
    if (signal(SIGALRM, alarm_timeout) == SIG_ERR)
        return;
    alarm(20);
#endif
}

static void dga2_refresh_func(video_canvas_t *canvas,
                       int src_x, int src_y,
                       int dest_x, int dest_y,
                       unsigned int width, unsigned int height)
{
    static unsigned int fb_src_x = -1, fb_dest_x = -1, fb_src_y = -1,
    fb_dest_y = -1, fb_w, fb_h;

#ifndef DGA2_DEBUG_BUFFER
#if 0
    printf("curr page = %d, src_x = %d, src_y = %d, dest_x = %d, dest_y = %d, w = %d, h = %d, fb_dest_y = %d\n",
           fb_current_page,
           src_x, src_y, dest_x, dest_y, width, height, fb_dest_y);
#endif
#ifdef FS_TRACE_REFRESH
    struct timespec ts1, ts2, d;
    clock_gettime (CLOCK_REALTIME, &ts1);
#endif
    if (XDGAGetViewportStatus(display, screen))
    {
        log_message(dga_log, _("refresh, not ready, skipping frame"));
        goto end;
    }

    if ((dest_y + height) > fs_height)
        height = fs_height;

    if ((dest_x + width) > fs_width)
        width = fs_width;

    /* convert buffer for PAL emulation */
    video_canvas_render(canvas,
                        fb_render_target,
                        width, height, src_x, src_y, src_x, src_y,
                        fs_fb_bpl, fs_depth);

    fb_current_page = ((fb_current_page + 1) % BB_DEPTH);

    fb_src_x = MIN(fb_src_x, src_x);
    fb_dest_x = MIN(fb_dest_x, dest_x);
    fb_src_y = MIN(fb_src_y, src_y);
    fb_dest_y = MIN(fb_dest_y, dest_y);
    fb_w = MAX(fb_w, width);
    fb_h = MAX(fb_h, height);
    
    XDGACopyArea(display, screen,
                 fb_src_x,  fb_ybegin_static + fb_src_y,
                 fb_w, fb_h,
                 fb_dest_x, fb_ybegin[fb_current_page] + fb_dest_y);
    XDGASetViewport(display, screen,
                    0, fb_ybegin[fb_current_page],
                    XDGAFlipRetrace);
/*
    f->tmpframebuffer = fb_page[fb_current_page];
*/

  end:
#ifdef FS_TRACE_REFRESH
    clock_gettime (CLOCK_REALTIME, &ts2);
    ts_diff(&ts1, &ts2, &d);

    printf("%09ld.%09ld - %09ld.%09ld: %09ld.%09ld\n",
           ts1.tv_sec, ts1.tv_nsec,
           ts2.tv_sec, ts2.tv_nsec,
           d.tv_sec, d.tv_nsec);
#endif /* FS_TRACE_REFRESH */
#else
    dump_fb("refresh");
#endif /* DGA2_DEBUG_BUFFER */
    return;
}

static int dga2_init_internal(void)
{
    int MajorVersion, MinorVersion;
    int i, j;

    fs_bestmode_counter = 0;
    dga2_is_available = 0;

    display = x11ui_get_display_ptr();

#if 0
#ifndef DGA2_DEBUG
    if (XF86DGAForkApp(XDefaultScreen(display)) != 0)
    {
        log_error(dga_log, "Can't fork for DGA; quitting");
        return 0;
    }
    log_message(dga_log, _("Successfully forked DGA"));
#endif
#endif
    if (! XDGAQueryVersion (display, &MajorVersion, &MinorVersion)) {
        log_error(dga_log,
                  _("Unable to query video extension version - disabling fullscreen."));
        dga2_is_available = 0;
        return 0;
    }
    if (! XDGAQueryExtension (display, &EventBase, &ErrorBase)) {
        log_error(dga_log, _("Unable to query video extension information - disabling fullscreen."));
        dga2_is_available = 0;
        return 0;
    }

    if (MajorVersion < DGA_MINMAJOR ||
        (MajorVersion == DGA_MINMAJOR && MinorVersion < DGA_MINMINOR))
    {
        log_error(dga_log,
                  _("Xserver is running an old XFree86-DGA version (%d.%d) "),
                  MajorVersion, MinorVersion);
        log_error(dga_log,
                  _("Minimum required version is %d.%d - disabling fullscreen."),
                  DGA_MINMAJOR, DGA_MINMINOR);
        dga2_is_available = 0;
        return 0;
    }

    fs_allmodes_dga2 = XDGAQueryModes(display,screen, &fs_vidmodecount);
    for (i = 0; i < fs_vidmodecount; i++)
    {
/*
        if (fs_allmodes_dga2[i].viewportWidth <= 800 &&
            fs_allmodes_dga2[i].viewportWidth >= 320 &&
            fs_allmodes_dga2[i].viewportHeight <= 600  &&
            fs_allmodes_dga2[i].viewportHeight >= 200 &&
            fs_allmodes_dga2[i].depth == x11ui_get_display_depth() &&
            fs_allmodes_dga2[i].visualClass != DirectColor &&
            (fs_allmodes_dga2[i].flags & XDGAPixmap))
*/
        if ((fs_allmodes_dga2[i].viewportWidth <= 800) &&
            (fs_allmodes_dga2[i].viewportWidth >= 320) &&
            (fs_allmodes_dga2[i].viewportHeight <= 600)  &&
            (fs_allmodes_dga2[i].viewportHeight >= 200) &&
#if 0 /* works when you define a mode named "vice" in your XF86Config */
            (strcmp(fs_allmodes_dga2[i].name, "vice") == 0) &&
#endif
            (fs_allmodes_dga2[i].depth >= 16) &&
            (fs_allmodes_dga2[i].viewportFlags & XDGAFlipRetrace) &&
            (fs_allmodes_dga2[i].flags & XDGABlitRect)
            )
        {
            for (j = 0; j < fs_bestmode_counter; j++)
            {
                if ((fs_allmodes_dga2[fs_bestmodes[j].modeindex].viewportWidth
                    == fs_allmodes_dga2[i].viewportWidth) &&
                    fs_allmodes_dga2[fs_bestmodes[j].modeindex].viewportHeight
                    == fs_allmodes_dga2[i].viewportHeight)
                    break;
            }
            if ( j == fs_bestmode_counter)
            {
                fs_bestmodes[fs_bestmode_counter].modeindex=i;
                snprintf(fs_bestmodes[fs_bestmode_counter].name,17," %ix%i-%.1fHz",
                         fs_allmodes_dga2[i].viewportWidth ,
                         fs_allmodes_dga2[i].viewportHeight,
                         fs_allmodes_dga2[i].verticalRefresh);
                log_message(dga_log, "Found mode: %s, %d",
                            fs_bestmodes[fs_bestmode_counter].name,
                            fs_bestmodes[fs_bestmode_counter].modeindex);

                fs_bestmode_counter++;
            }
            if (fs_bestmode_counter == 10)
                break;
        }

    }
    if (fs_vidmodecount == 0)
        return 0;
    dga2_is_available = 1;

    return dga2_is_available;
}


static int dga2_request_set_mode(int enable, void *param)
{
    if (!enable)
    {
        request_fs_mode = 2; /* toggle to window mode */
        if (old_ui_hook)
            (void) vsync_set_event_dispatcher(old_ui_hook);
    }
    else
    {
        request_fs_mode = 1; /* toggle to fullscreen mode */
        old_ui_hook = vsync_set_event_dispatcher(dga2_dispatch_events);
    }
    return 0;
}

static DWORD fs_cached_physical_colors[256]; /* from video.h */
static int dga2_draw_buffer_alloc(struct video_canvas_s *c,
                                  BYTE **draw_buffer, unsigned int w,
                                  unsigned int h, unsigned int *pitch)
{
    *draw_buffer = lib_malloc(w * h);
    fs_fb_data = *draw_buffer;
    fs_cached_db_width = w;
    if (*draw_buffer)
        return 0;
    return 1;
}

static void dga2_draw_buffer_free(struct video_canvas_s *c, BYTE *draw_buffer)
{
    lib_free(draw_buffer);
}

static void dga2_draw_buffer_clear(struct video_canvas_s *c,
                                   BYTE *draw_buffer, BYTE value,
                                   unsigned int w, unsigned int h,
                                   unsigned int pitch)
{
    memset(draw_buffer, value, w * h);
}

static int dga2_set_mode(resource_value_t v, void *param)
{
    static int interval,prefer_blanking,allow_exposures;
    XColor color;
    unsigned int i;

    if (!dga2_is_available || !fs_bestmode_counter) {
        fs_use_fs_at_start = (int) v;
        return 0;
    }

    if (v && !dga2_is_enabled)
    {
#ifdef XXX_CHECK_DGA2
        XGrabKeyboard(display,  XRootWindow (display, screen),
                      1, GrabModeAsync,
                      GrabModeAsync,  CurrentTime);
        XGrabPointer(display,  XRootWindow (display, screen), 1,
                     PointerMotionMask | ButtonPressMask |
                     ButtonReleaseMask,
                     GrabModeAsync, GrabModeAsync,
                     None, None, CurrentTime);

        XGetScreenSaver(display,&timeout,&interval,
                        &prefer_blanking,&allow_exposures);
        XSetScreenSaver(display,0,0,DefaultBlanking,DefaultExposures);
#endif
        log_message(dga_log, _("Switch to fullscreen%s"),
                    fs_bestmodes[dga2_cached_canvas->fullscreenconfig->mode].name);

        if (XDGAOpenFramebuffer(display, screen) == False)
        {
            log_message(dga_log,
                        _("Need root privileges for DGA2 fullscreen"));
            dga2_request_set_mode(0, NULL);
            return 0;
        }

        /* add a segfault handler, just in case fullscreen bombs, it
           restores the window mode */
        set_alarm_timeout();

        dgadev = XDGASetMode(display, screen,
                             fs_allmodes_dga2[fs_bestmodes[dga2_cached_canvas->fullscreenconfig->mode].modeindex].num);
        if (!dgadev)
        {
            log_error(dga_log,
                      _("Error switching to fullscreen (SetMode) %ix%i"),
                      fs_allmodes_dga2[fs_bestmodes[dga2_cached_canvas->fullscreenconfig->mode].modeindex].viewportWidth,
                      fs_allmodes_dga2[fs_bestmodes[dga2_cached_canvas->fullscreenconfig->mode].modeindex].viewportHeight);
            return 0;
        }

#if FS_PIXMAP_DGA
        if (!dgadev->pixmap)
        {
            log_error(dga_log,
                      _("Error switching to fullscreen (pixmap) %ix%i"),
                      fs_allmodes_dga2[dga2_selected_videomode].viewportWidth,
                      fs_allmodes_dga2[dga2_selected_videomode].viewportHeight);            XDGASetMode(display,screen,0);
            XFree(dgadev);
            return 0;
        }
#endif

        fb_addr = dgadev->data;
/*
        fb_width = fs_allmodes_dga2[dga2_selected_videomode].bytesPerScanline;
*/
        fs_width = dgadev->mode.viewportWidth;
        fs_height = dgadev->mode.viewportHeight;
        pitch = dgadev->mode.bytesPerScanline /
            (dgadev->mode.bitsPerPixel >> 3);
        offs = dgadev->mode.bytesPerScanline * fs_height;
        for (i = 0; i < BB_DEPTH; i++)
        {
            fb_offs[i] = i * offs;
            fb_ybegin[i] = i * fs_height;
            fb_page[i] = fb_addr + fb_offs[i];

#ifdef DGA2_DEBUG
            log_message(dga_log, "page: %p, offs %d, ybegin: %d",
                        fb_page[i], fb_offs[i], fb_ybegin[i]);
#endif
        }
        fb_ybegin_static = i*fs_height; /* framebuffer is the fixed
                                           last page */
        if (fb_ybegin_static > dgadev->mode.maxViewportY)
        {
            log_message(dga_log,
                        _("Not enough video memory pages in mode %s, disabling fullscreen (%d,%d)."),
                        dgadev->mode.name, fb_ybegin_static, dgadev->mode.maxViewportY);
            dga2_request_set_mode(0, NULL);
            goto nodga;
        }

        fb_current_page = 0;

        dga2_cached_canvas->video_fullscreen_refresh_func = dga2_refresh_func;

#ifdef FS_PIXMAP_DGA
        gcContext =  XCreateGC(display, dgadev->pixmap, 0, 0);
        if (!gcContext) {
            log_error(dga_log,
                      _("Error switching to fullscreen (CreateGC) %ix%i"),
                      fs_allmodes_dga2[dga2_selected_videomode].viewportWidth,
                      fs_allmodes_dga2[dga2_selected_videomode].viewportHeight);            XDGASetMode(display,screen,0);
            XFree(dgadev);
            ui_display_paused(1);
            return 0;
        }
#endif

        XDGASelectInput(display, screen,
                        PointerMotionMask |
                        ButtonPressMask |
                        KeyPressMask |
                        KeyReleaseMask);

        if (canvas_palette != dga2_cached_canvas->palette)
        {
            if (canvas_palette)
            {
                log_message(dga_log, "colormao should be freed, FIXME");
                /* FIXME here */
            }
            canvas_palette = dga2_cached_canvas->palette;
            memcpy(fs_cached_physical_colors,
                   dga2_cached_canvas->videoconfig->physical_colors,
                   sizeof(DWORD) * 256);
            cm = XDGACreateColormap(display, screen, dgadev, AllocNone);
	
            for (i = 0; i < dga2_cached_canvas->palette->num_entries; i++)
            {
                color.blue =
                    (dga2_cached_canvas->palette->entries[i].blue << 8);
                color.red =
                    (dga2_cached_canvas->palette->entries[i].red << 8);
                color.green =
                    (dga2_cached_canvas->palette->entries[i].green << 8);
                color.flags = DoRed | DoGreen | DoBlue;
                XAllocColor(display, cm, &color);
#ifdef DGA2_DEBUG
                log_message (dga_log, "colors[%s]: %lx",
                             dga2_cached_canvas->palette->entries[i].name,
                             color.pixel);
#endif
                video_render_setphysicalcolor(dga2_cached_canvas->videoconfig,
                                              i, color.pixel, fs_depth);
            }

            /* Save pixel values of fullscreen mode for reuse in case no new
               palette is allocated until next fullscreen activation */
            if (fs_saved_colors)
                lib_free(fs_saved_colors);
            fs_saved_colors = (DWORD *)malloc (sizeof(DWORD) * 256);
            if (!fs_saved_colors)
            {
                log_error(dga_log, _("Couldn't allocate color cache"));
                goto nodga;
            }

            memcpy(fs_saved_colors,
                   dga2_cached_canvas->videoconfig->physical_colors,
                   sizeof(DWORD) * 256);

        }
        else
        {
            /* Reuse pixel values from earlier activation */
            if (fs_saved_colors == NULL)
            {
                log_error(dga_log, "inconsistent view for color management, disabling fullscreen.");
                goto nodga;
            }
            memcpy(dga2_cached_canvas->videoconfig->physical_colors,
                   fs_saved_colors, sizeof(DWORD) * 256);
        }

        XDGAInstallColormap(display, screen, cm);
#ifdef FS_PIXMAP_DGA
        XDGAChangePixmapMode(display, screen, &pm_x, &pm_y,
                             XDGAPixmapModeLarge);
#endif

        XDGASetViewport (display, screen, 0, 0, XDGAFlipImmediate);
        while(XDGAGetViewportStatus(display, screen));
#ifdef FS_PIXMAP_DGA
        XSetForeground(display, gcContext, 0);
        XFillRectangle(display, dgadev->pixmap, gcContext, 0, 0,
                       fs_allmodes_dga2[dga2_selected_videomode].maxViewportX,
                       fs_allmodes_dga2[dga2_selected_videomode].maxViewportY);
#endif
        XFlush(display);
        dga2_is_enabled = 1;

#ifdef DGA2_DEBUG
        log_message(dga_log, "membase = %p, pitch = %i, offs = %i",
                    fb_addr, pitch, offs);
#endif

#ifdef DGA2_DEBUG_BUFFER
        fb_addr = malloc(offs * (BB_DEPTH + 1) * sizeof(char));
#endif
        for (i = 0; i < (BB_DEPTH + 1); i++)
            memset (fb_addr + i * offs, 0, offs);

        fs_fb_bpl = dgadev->mode.bytesPerScanline;
        fs_depth = dgadev->mode.depth;
        fb_render_target = fb_dump = fb_addr + ((i - 1)* offs);

        /* remember dimension for restore */
        canvas_width = dga2_cached_canvas->draw_buffer->canvas_width;
        canvas_height = dga2_cached_canvas->draw_buffer->canvas_height;
        dga2_cached_canvas->draw_buffer->canvas_width = fs_width;
        dga2_cached_canvas->draw_buffer->canvas_height = fs_height;

        if (dga2_cached_canvas->videoconfig->doublesizex)
            dga2_cached_canvas->draw_buffer->canvas_width /= 2;
        if (dga2_cached_canvas->videoconfig->doublesizey)
            dga2_cached_canvas->draw_buffer->canvas_height /= 2;

        video_viewport_resize(dga2_cached_canvas);

#ifdef DGA2_DEBUG_BUFFER
        goto nodga;
#endif
    }

    if (!v && dga2_is_enabled)
    {
        log_message(dga_log, _("Switch to windowmode"));

        /* Restore framebuffer details */
#ifndef DGA2_DEBUG_BUFFER
        fb_addr = (unsigned char *)0;
        /* Restore pixel values of window mode */
        memcpy(dga2_cached_canvas->videoconfig->physical_colors,
               fs_cached_physical_colors,
               sizeof(DWORD) * 256);
#endif
        dga2_cached_canvas->draw_buffer->canvas_width = canvas_width;
        dga2_cached_canvas->draw_buffer->canvas_height = canvas_height;
        dga2_cached_canvas->video_fullscreen_refresh_func = NULL;
        video_viewport_resize(dga2_cached_canvas);

      nodga:
        XDGASetMode(display, screen, 0);
        XDGACloseFramebuffer(display, screen);
        XFree(dgadev);
#ifdef FS_PIXMAP_DGA
        XFreeGC(display, gcContext);
#endif
        XSetScreenSaver(display, timeout, interval,
                        prefer_blanking, allow_exposures);

#if XXX_CHECK_DGA2
        XUngrabPointer(display, CurrentTime);
        XUngrabKeyboard(display, CurrentTime);
#endif
        dga2_is_enabled = 0;
#ifdef DGA2_DEBUG
        alarm(0);
#endif
    }
    ui_check_mouse_cursor();
    return 1;
}

/* ------------------------------------------------------------------------- */

static int dga2_mode_on(void)
{
    if (!dga2_is_enabled) {
        x11kbd_focus_change();
        dga2_set_mode((resource_value_t) 1, NULL);
        ui_update_menus();
        return 0;
    }
    return 1;
}

static int dga2_mode_off(void)
{
    fullscreen_is_enabled_restore = 0;
    if (dga2_is_enabled) {
        x11kbd_focus_change();
        XDGASync(display, screen);
        dga2_dispatch_events_2();
        dga2_set_mode(0, NULL);
        ui_update_menus();
        return 1;
    }
    return 0;
}

void dga2_mode_update(void)
{
    if (request_fs_mode == 1)
        dga2_mode_on();
    if (request_fs_mode == 2)
        dga2_mode_off();
    request_fs_mode = 0;
}

static void dga2_dispatch_events_2(void)
{
    XDGAEvent event;
    XKeyEvent xk;
    KeySym key;
    XComposeStatus compose;
    static char buffer[20];

    while (XPending(display))
    {
        XNextEvent(display, (XEvent *) &event);
        switch (event.type - EventBase)
        {
        case KeyPress:
            XDGAKeyEventToXKeyEvent(&(event.xkey), &xk);
            XLookupString(&xk, buffer, 20, &key, &compose);
#ifdef DGA2_DEBUG
            if (key == XK_d)
                dga2_request_set_mode(0, NULL);
            if (key == XK_u)
                dump_fb("on demand");
#endif
            x11kbd_press((signed long)key);
            break;

        case KeyRelease:
            XDGAKeyEventToXKeyEvent(&(event.xkey), &xk);
            XLookupString(&xk, buffer, 20, &key, &compose);
            x11kbd_release((signed long)key);
            break;

        default:
            break;
        }
    }
}

static void dga2_dispatch_events(void)
{
    dga2_dispatch_events_2();
    dga2_mode_update();
}

#ifdef DGA2_DEBUG
int fs_count = 0;
static void dump_fb(char *wo)
{
    int x, y;

    if (!fb_dump)
        return;

    for (y = 0; y < 100; y++)
    {
        printf("%d, %s: %2d: ", fs_count, wo, y);
        for (x = 0 ; x < 60; x++)
        {
            printf("%02x", (int) *(fb_dump + 200+x + (y * pitch)));
        }
        printf("\n");
    }
    fs_count++;
}
#endif /* DGA2_DEBUG */

