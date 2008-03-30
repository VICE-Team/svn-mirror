/*
 * dga1.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
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

#include "vice.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>

#include "dga1.h"
#include "fullscreenarch.h"
#include "log.h"
#include "resources.h"
#include "types.h"
#include "uimenu.h"
#include "utils.h"
#include "videoarch.h"
#include "vidmode.h"
#include "x11ui.h"

#include "kbd.h"
#include "machine.h"
#include "mouse.h"
#include "ui.h"
#include "videoarch.h"


int dga1_is_enabled = 0;
int dga1_is_suspended = 0;

static log_t dga1_log = LOG_ERR;
static int dga1_is_available = 0;

static XF86VidModeModeLine restoremodeline;

static int timeout;
static int EventBase, ErrorBase;

int fullscreen_width, fullscreen_height;

static int fb_bank, fb_mem;
static char *fb_addr;
static int fb_width;
static int fb_depth;

static Display *display;
extern int screen;

#ifdef FS_DEBUG
void alarm_timeout(int signo) {
    volatile int i = 1;

    dga1_suspend(0);
    while (i) {
        log_message(dga1_log, _("Attach debugger to pid %d...\n"), getpid());
        sleep(1); /* Set breakpoint here to debug */
    }
}

static void set_alarm_timeout(void)
{
    if (signal(SIGSEGV, alarm_timeout) == SIG_ERR)
        return;
    if (signal(SIGALRM, alarm_timeout) == SIG_ERR)
        return;
    alarm(20);
}
#endif

static void dga2_refresh_func(video_canvas_t *canvas,
                              int src_x, int src_y,
                              int dest_x, int dest_y,
                              unsigned int width, unsigned int height)
{
/*
    int y;

    if ((dest_y + height) > fullscreen_height)
        height = fullscreen_height;

    if ((dest_x + width) > fullscreen_width)
        width = fullscreen_width;

    for (y = 0; y < height ; y++) {
        memcpy(fb_addr + (fb_width * (dest_y + y) + dest_x) * fb_depth,
               f->x_image->data
               + (f->x_image->width * (src_y + y) + src_x) * fb_depth,
               width * fb_depth);
    }
*/
}


#if 0
int fullscreen_mode_on(void)
{
    if (!fullscreen_is_enabled) {
        x11kbd_focus_change();
        fullscreen_set_mode((resource_value_t)1, NULL);
        ui_update_menus();
        return 0;
    }
    return 1;
}

int fullscreen_mode_off(void)
{
    fullscreen_is_enabled_restore = 0;
    if (fullscreen_is_enabled) {
        x11kbd_focus_change();
        fullscreen_set_mode(0, NULL);
        ui_update_menus();
        return 1;
    }
    return 0;
}
#endif

int dga1_init(void)
{
    int MajorVersion, MinorVersion;

    dga1_log = log_open("DGA1");

    fb_depth = x11ui_get_display_depth() / 8;

    display = x11ui_get_display_ptr();

    if (XF86DGAForkApp(XDefaultScreen(display)) != 0) {
        log_error(dga1_log, "Can't fork for DGA; quitting");
        return -1;
    }

    log_message(dga1_log, _("Successfully forked DGA."));

    if (!XF86DGAQueryVersion(display, &MajorVersion, &MinorVersion)) {
        log_error(dga1_log, _("Unable to query DGA1 version."));
        return -1;
    }

    if (!XF86DGAQueryExtension(display, &EventBase, &ErrorBase)) {
        log_error(dga1_log, _("Unable to query DGA1 information."));
        return -1;
    }

    if (geteuid() != 0) {
        log_error(dga1_log, _("Root permissions required to use DGA1."));
        return 0;
    }

    if (!XF86DGAGetVideo(display, screen, &fb_addr, &fb_width, &fb_bank,
        &fb_mem) || fb_bank < fb_mem) {
        log_error(dga1_log, _("Problems with DGA - disabling fullscreen."));
        return 0;
    }

    dga1_is_available = 1;
    return 0;
}

extern XF86VidModeModeInfo **vm_modes;

/* FIXME: Limiting vidmode to one active fullscreen window.  */
static video_canvas_t *active_canvas;

int dga1_enable(struct video_canvas_s *canvas, int enable)
{
    static int interval, prefer_blanking, allow_exposures;
    static int dotclock;

    if (enable) {
        XF86VidModeModeInfo *vm;

        XGrabKeyboard(display, XRootWindow(display, screen),
                      1, GrabModeAsync, GrabModeAsync, CurrentTime);
        XGrabPointer(display,  XRootWindow (display, screen), 1,
                     PointerMotionMask | ButtonPressMask |
                     ButtonReleaseMask,
                     GrabModeAsync, GrabModeAsync,
                     None, None, CurrentTime);

        XGetScreenSaver(display, &timeout, &interval,
                        &prefer_blanking, &allow_exposures);
        XSetScreenSaver(display, 0, 0, DefaultBlanking, DefaultExposures);

        log_message(dga1_log, "Enabling Vidmode with%s",
                    vm_bestmodes[canvas->fullscreenconfig->mode].name);
        vm = vm_modes[vm_bestmodes[canvas->fullscreenconfig->mode].modeindex];

        XF86VidModeGetModeLine(display, screen, &dotclock, &restoremodeline);

        XF86VidModeLockModeSwitch(display, screen, 1);
        if (!XF86VidModeSwitchToMode(display, screen, vm)) {
            log_error(dga1_log, _("Error switching to fullscreen%s"),
                      vm_bestmodes[canvas->fullscreenconfig->mode].name);
            return -1;
        } else {
            fullscreen_width = vm->hdisplay;
            fullscreen_height = vm->vdisplay;
        }

        video_set_refresh_func(dga2_refresh_func);

        XF86DGAGetVideo(display, screen, &fb_addr, &fb_width, &fb_bank,
                        &fb_mem);
        log_message(dga1_log,
                    _("DGA extension: addr:%p, width %d, bank size %d mem size %d\n"),
                    fb_addr, fb_width, fb_bank, fb_mem);


        XF86DGADirectVideo(display, screen, XF86DGADirectGraphics
                           /*| XF86DGADirectMouse | XF86DGADirectKeyb*/);
        XF86DGASetViewPort(display, screen, 0, 0);

        active_canvas = canvas;
        dga1_is_enabled = 1;
        dga1_is_suspended = 0;
#ifdef FS_DEBUG
        set_alarm_timeout();
#endif
    } else {
        unsigned int i;

        if (!dga1_is_enabled)
            return 0;

        dga1_is_enabled = 0;

        log_message(dga1_log, _("Switch to windowmode"));

        video_set_refresh_func(NULL);

        XF86DGADirectVideo(display, screen, 0);

        XF86VidModeLockModeSwitch(display, screen, 1);

        for (i = 0; i < vm_mode_count; i++) {
            if (vm_modes[i]->hdisplay == restoremodeline.hdisplay &&
                vm_modes[i]->vdisplay == restoremodeline.vdisplay &&
                vm_modes[i]->dotclock == dotclock ) {
                XF86VidModeSwitchToMode(display, screen, vm_modes[i]);
                break;
            }
        }
        XSetScreenSaver(display, timeout, interval, prefer_blanking,
                        allow_exposures);

        XUngrabPointer(display, CurrentTime);
        XUngrabKeyboard(display, CurrentTime);
    }

    machine_video_refresh();
    ui_check_mouse_cursor();

    return 0;
}

int dga1_mode(struct video_canvas_s *canvas, int mode)
{
    if (mode < 0 || vidmode_available() == 0)
        return 0;

    log_message(dga1_log, "Selected mode: %s", vm_bestmodes[mode].name);
    canvas->fullscreenconfig->mode = mode;

    return 0;
}

void dga1_suspend(int level)
{
    if (dga1_is_enabled == 0)
        return;

    if (dga1_is_suspended > 0)
        return;

    dga1_is_suspended = 1;
    dga1_enable(active_canvas, 0);
}

void dga1_resume(void)
{
   if (dga1_is_enabled == 0)
        return;

   if (dga1_is_suspended == 0)
      return;

   dga1_is_suspended = 0;
   dga1_enable(active_canvas, 1);
}

int dga1_available(void)
{
    return dga1_is_available;
}

static void *mode_callback;

void dga1_mode_callback(void *callback)
{
    mode_callback = callback;
}

void dga1_create_menus(struct ui_menu_entry_s menu[])
{
    unsigned int i, amodes;
    ui_menu_entry_t *resolutions_submenu;

    amodes = vidmode_available_modes();

    resolutions_submenu = (ui_menu_entry_t *)xcalloc((size_t)(amodes + 1),
                          sizeof(ui_menu_entry_t));

    for (i = 0; i < amodes ; i++) {
        resolutions_submenu[i].string =
            (ui_callback_data_t)xmsprintf("*%s", vm_bestmodes[i].name);
        resolutions_submenu[i].callback = (ui_callback_t)mode_callback;
        resolutions_submenu[i].callback_data = (ui_callback_data_t)i;
    }

    for (i = 0; menu[i].string; i++) {
        if (strncmp(menu[i].string, "DGA1", 4) == 0) {
            if (amodes > 0)
                menu[i].sub_menu = resolutions_submenu;
            break;
        }
    }
}

