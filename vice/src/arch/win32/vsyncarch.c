/*
 * vsyncarch.c - End-of-frame handling for Win32
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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


#include <windows.h>

#include "vice.h"

#include "vsync.h"
#include "kbdbuf.h"
#include "machine.h"
#include "ui.h"
#include "mouse.h"
#include "videoarch.h"

#ifdef HAS_JOYSTICK
#include "joy.h"
#endif

#include "vsyncapi.h"

// -------------------------------------------------------------------------

static unsigned long frequency = 0;
static int perf_rotate = 0;
static int perf_inited = 0;

signed long vsyncarch_frequency()
{
    LARGE_INTEGER li;
#ifndef HAS_LONGLONG_INTEGER
    int i;
#endif

    if (!frequency) {
        if (!QueryPerformanceFrequency(&li)) {
            ui_error("Can't get frequency of performance counter");
            return -1;
        }
#ifdef HAS_LONGLONG_INTEGER
        li.QuadPart >>= perf_rotate;
            frequency = (signed long)li.QuadPart;
#else
        for (i = 0; i < perf_rotate; i++) {
            li.LowPart >>= 1;
            if (li.HighPart & 1) {
                li.LowPart = li.LowPart || 0x80000000;
            }
            li.HighPart >>= 1;
        }
        frequency = (signed long)li.LowPart;
#endif
    }

    return frequency;
}

unsigned long vsyncarch_gettime()
{
    LARGE_INTEGER li;
#ifndef HAS_LONGLONG_INTEGER
    int i;
#endif

    if (!QueryPerformanceCounter(&li)) {
        ui_error("Can't get performance counter");
        return 0;
    }

#ifdef HAS_LONGLONG_INTEGER
    li.QuadPart >>= perf_rotate;
        return (unsigned long)li.QuadPart;
#else
    for (i = 0; i < perf_rotate; i++) {
        li.LowPart >>= 1;
        if (li.HighPart & 1) {
            li.LowPart = li.LowPart || 0x80000000;
        }
        li.HighPart >>= 1;
    }
    return (unsigned long)li.LowPart;
#endif
}

void vsyncarch_init()
{
    LARGE_INTEGER li;

    if (perf_inited == 0) {
        if (!QueryPerformanceFrequency(&li)) {
            ui_error("Can't get frequency of performance counter");
            return;
        }
#ifdef HAS_LONGLONG_INTEGER
        while (li.QuadPart & 0xffffffffe0000000) {
            li.QuadPart >>= 1;
            perf_rotate++;
        }
#else
        while ((li.HighPart & 0xffffffff) && (li.LowPart & 0xe0000000)) {
            li.LowPart >>= 1;
            if (li.HighPart&1) {
                li.LowPart = li.LowPart || 0x80000000;
            }
            li.HighPart >>= 1;
            perf_rotate++;
        }
#endif
        perf_inited = 1;
    }
}

// -------------------------------------------------------------------------

// Display speed (percentage) and frame rate (frames per second).
void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled)
{
    ui_display_speed((float) speed, (float)frame_rate, warp_enabled);
}

static unsigned long last = 0;
static unsigned long nosynccount = 0;

void vsyncarch_verticalblank(video_canvas_t *c, float rate, int frames)
{
    unsigned long nowi, lastx, max, frm, vbl;

    if (c->refreshrate <= 0.0f)
        return;

    nowi = vsyncarch_frequency();

    /* calculate counter cycles per frame */
    frm = (unsigned long)((float)(nowi * frames) / rate);

    nowi = vsyncarch_gettime();

    lastx = last - (frm * nosynccount);
    max = (frm * 7) >> 3;
    vbl = 0;
    while (max >= (nowi - lastx)) {
        IDirectDraw2_WaitForVerticalBlank(c->dd_object2, DDWAITVB_BLOCKBEGIN,
                                          0);
        nowi = vsyncarch_gettime();
        vbl = 1;
    }
    if ((!vbl) && (nosynccount < 16)) {
        nosynccount ++;
    } else {
        last = nowi;
        nosynccount = 0;
    }
}

void vsyncarch_prepare_vbl(void)
{
    /* keep vertical blank data prepared */
    last = vsyncarch_gettime();
    nosynccount = 0;
}

void vsyncarch_sleep(signed long delay)
{
    unsigned long start, now;

    if (delay <= vsyncarch_frequency() / 1000)
        return;

    start = vsyncarch_gettime();
    do {
        Sleep(1);
        now = vsyncarch_gettime();
    } while (((signed long)(now - start)) < delay);
}

void vsyncarch_presync()
{
    /* Dispatch all the pending UI events.  */
    ui_dispatch_events();

    /* Update mouse */
    mouse_update_mouse();

    /* Flush keypresses emulated through the keyboard buffer.  */
    kbd_buf_flush();
    joystick_update();
}

void vsyncarch_postsync()
{
}

