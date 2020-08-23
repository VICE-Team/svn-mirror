/** \file   mousedrv.c
 * \brief   Native GTK3 UI mouse driver stuff.
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 */

/*
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

#if defined(MACOSX_SUPPORT)
#import <CoreGraphics/CGRemoteOperation.h>
#import <CoreGraphics/CGEvent.h>
#elif defined(WIN32_COMPILE)
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#endif


#include <stdio.h>
#include <math.h>

#include "debug_gtk3.h"

#include "log.h"
#include "vsyncapi.h"
#include "maincpu.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ui.h"
#include "uimachinewindow.h"


/** \brief The callbacks registered for mouse buttons being pressed or
 *         released. 
 *  \sa mousedrv_resources_init which sets these values properly
 *  \sa mouse_button which uses them
 */
static mouse_func_t mouse_funcs = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/** \brief Current mouse X value.
 *
 *  This is a dead-reckoning sum of left and right motions and does
 *  not necessarily bear any connection to any actual X coordinates.
 *
 *  \sa mousedrv_get_x
 */
static float mouse_x = 0.0;

/** \brief Current mouse Y value.
 *
 *  This is a dead-reckoning sum of left and right motions and does
 *  not necessarily bear any connection to any actual X coordinates.
 *
 *  \sa mousedrv_get_y
 */
static float mouse_y = 0.0;

/** \brief Last time the mouse was moved.
 *
 *  \sa mousedrv_get_timestamp
 */
static unsigned long mouse_timestamp = 0;

/** \brief Whether or not to capture mouse movements and warp back.
 */
static bool enable_capture = false;

/** \brief Host mouse deltas are calculated from this X value.
 *
 * After the delta is calculated, the mouse is warped back to this
 * location.
 */
static int host_delta_origin_x = 0.0;

/** \brief Host mouse deltas are calculated from this Y value.
 *
 * After the delta is calculated, the mouse is warped back to this
 * location.
 */
static int host_delta_origin_y = 0.0;

/** \brief Curent host mouse delta
 *
 * After the delta is calculated, the mouse is warped back to the
 * delta origin.
 */
static float host_delta_x = 0.0;

/** \brief Curent host mouse delta
 *
 * After the delta is calculated, the mouse is warped back to the
 * delta origin.
 */
static float host_delta_y = 0.0;

int mousedrv_cmdline_options_init(void)
{
    return 0;
}

unsigned long mousedrv_get_timestamp(void)
{
    return mouse_timestamp;
}

static void limit_mouse_delta(float *global_delta_x, float *global_delta_y)
{
    /* We can't move more than -32 <> 31 in a single reading.
     * If we have, then reduce the distance travelled without
     * altering the direction. Rather than having the max move
     * distance vary by angle, limit the overall 2d delta length.
     */
    
    float abs_delta;
    
    /* TODO: Why does this value need to be larger than what the emu sees.
     * Any integer value larger than this can cause overflow and sign inversion,
     * somewhere.
     */
    const float max_delta = 63.0f;
    
    /* How far the mouse has travelled */
    abs_delta = sqrt((*global_delta_x * *global_delta_x) + (*global_delta_y * *global_delta_y));
    
    if (abs_delta > max_delta) {
        *global_delta_x = *global_delta_x / abs_delta * max_delta;
        *global_delta_y = *global_delta_y / abs_delta * max_delta;
    }
}

static void poll_host_mouse(void)
{
    static CLOCK last_poll_clock;
    float global_delta_x, global_delta_y;

    /* Don't poll the host computer for mouse position more than once per emu cycle */
    if (maincpu_clk == last_poll_clock) {
        return;
    }
    last_poll_clock = maincpu_clk;
    
    /* Obtain the accumulated mouse delta */
    mouse_host_get_delta(&global_delta_x, &global_delta_y);

    // printf("mouse delta %f, %f\n", global_delta_x, global_delta_y); fflush(stdout);
    
    if (global_delta_x || global_delta_y) {
        /* prevent the mouse delta from overflowing potx/y */
        limit_mouse_delta(&global_delta_x, &global_delta_y);
        mouse_move(global_delta_x, global_delta_y);
    }
}

int mousedrv_get_x(void)
{
    poll_host_mouse();

    return (int)mouse_x;
}

int mousedrv_get_y(void)
{
    poll_host_mouse();

    return (int)mouse_y;
}

void mouse_move(float dx, float dy)
{
#if 0
    mouse_x += dx;
    mouse_y -= dy;  /* why ? */

    /* can't this be done with int modulo ? */
    while (mouse_x < 0.0) {
        mouse_x += 65536.0;
    }
    while (mouse_x >= 65536.0) {
        mouse_x -= 65536.0;
    }
    while (mouse_y < 0.0) {
        mouse_y += 65536.0;
    }
    while (mouse_y >= 65536.0) {
        mouse_y -= 65536.0;
    }
#endif

    mouse_x = (float)((int)(mouse_x + dx) % 0xffff);
    mouse_y = (float)((int)(mouse_y - dy) % 0xffff);

    mouse_timestamp = vsyncarch_gettime();
}

void mouse_button(int bnumber, int state)
{
    switch(bnumber) {
    case 0:
        if (mouse_funcs.mbl) {
            mouse_funcs.mbl(state);
        }
        break;
    case 1:
        if (mouse_funcs.mbm) {
            mouse_funcs.mbm(state);
        }
        break;
    case 2:
        if (mouse_funcs.mbr) {
            mouse_funcs.mbr(state);
        }
        break;
    case 3:
        if (mouse_funcs.mbu) {
            mouse_funcs.mbu(state);
        }
        break;
    case 4:
        if (mouse_funcs.mbd) {
            mouse_funcs.mbd(state);
        }
        break;
    default:
        log_error(LOG_DEFAULT, "GTK3MOUSE: Warning: Strange mouse button %d\n", bnumber);
    }
}

void mousedrv_init(void)
{
    /* This does not require anything special to be done */
}

void mousedrv_mouse_changed(void)
{
    /** \todo Tell UI level to capture mouse cursor if necessary and
     *        permitted */
    log_message(LOG_DEFAULT, "GTK3MOUSE: Status changed: %d (%s)\n", 
            _mouse_enabled, _mouse_enabled ? "enabled" : "disabled");
    if (_mouse_enabled) {
        ui_mouse_grab_pointer();
    } else {
        ui_mouse_ungrab_pointer();
    }
}

int mousedrv_resources_init(mouse_func_t *funcs)
{
    mouse_funcs.mbl = funcs->mbl;
    mouse_funcs.mbr = funcs->mbr;
    mouse_funcs.mbm = funcs->mbm;
    mouse_funcs.mbu = funcs->mbu;
    mouse_funcs.mbd = funcs->mbd;
    return 0;
}

static void warp(int x, int y)
{
#ifdef MACOSX_SUPPORT

    CGWarpMouseCursorPosition(CGPointMake(x, y));
    CGAssociateMouseAndMouseCursorPosition(true);

#elif defined(WIN32_COMPILE)

    SetCursorPos(x, y);

#else /* xlib */

    GtkWidget *gtk_widget = ui_get_window_by_index(PRIMARY_WINDOW);
    GdkWindow *gdk_window = gtk_widget_get_window(gtk_widget);
    Display *display = GDK_WINDOW_XDISPLAY(gdk_window);

    XWarpPointer(display, None, GDK_WINDOW_XID(gdk_window), 0, 0, 0, 0, x, y);

#endif
}

void mouse_host_capture(int warp_x, int warp_y)
{
    enable_capture = true;
    
    warp(warp_x, warp_y);

    /* future mouse moments will be captured relative from here */
    host_delta_origin_x = warp_x;
    host_delta_origin_y = warp_y;

    /* reset the delta accumulation buffer */
    host_delta_x = 0.0f;
    host_delta_y = 0.0f;
}

bool mouse_host_is_captured(void)
{
    return enable_capture;
}

void mouse_host_uncapture(void)
{
    enable_capture = false;
}

void mouse_host_moved(float x, float y)
{
    float delta_x, delta_y;

    if (!enable_capture) {
        return;
    }

    delta_x = x - host_delta_origin_x;
    delta_y = y - host_delta_origin_y;
    
    if (delta_x || delta_y) {
        host_delta_x += delta_x;
        host_delta_y += delta_y;

        warp(host_delta_origin_x, host_delta_origin_y);
    }
}

void mouse_host_get_delta(float *delta_x, float *delta_y)
{
    if (!enable_capture) {
        *delta_x = 0;
        *delta_y = 0;
        return;
    }
    
    *delta_x = host_delta_x;
    *delta_y = host_delta_y;

    /* reset the delta accumulation buffer */
    host_delta_x = 0.0f;
    host_delta_y = 0.0f;
}
