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

#include "vice.h"

#ifndef IDE_COMPILE
#define WINVER 0x0500
#include <windows.h>
#endif

#include "vsync.h"
#include "kbdbuf.h"
#include "lightpendrv.h"
#include "log.h"
#include "machine.h"
#include "ui.h"
#include "uiapi.h"
#include "mousedrv.h"
#include "videoarch.h"

#ifdef HAS_JOYSTICK
#include "joy.h"
#endif

#include "vsyncapi.h"

// -------------------------------------------------------------------------

enum { EXTRA_PRECISION = 10 };

signed long vsyncarch_frequency(void)
{
    return 1000 << EXTRA_PRECISION;
}

unsigned long vsyncarch_gettime(void)
{
    return timeGetTime() << EXTRA_PRECISION;
}

void vsyncarch_init(void)
{
    MMRESULT res = timeBeginPeriod(1);
    if (res != TIMERR_NOERROR)
        log_warning(LOG_DEFAULT, "VSYNC: 1 ms scheduling latency not available.");
}

// -------------------------------------------------------------------------

#if !defined(IDE_COMPILE) && !defined(WATCOM_COMPILE)
static void win32_mouse_jitter(void)
{
    INPUT ip;

    ip.type = INPUT_MOUSE;
    ip.mi.dx = 0;
    ip.mi.dy = 0;
    ip.mi.mouseData = 0;
    ip.mi.dwFlags = MOUSEEVENTF_MOVE;
    ip.mi.time = 0;
    ip.mi.dwExtraInfo = 0;

    SendInput(1, &ip, sizeof(INPUT));
}
#endif

// Display speed (percentage) and frame rate (frames per second).
void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled)
{
    ui_display_speed((float) speed, (float)frame_rate, warp_enabled);
}

void vsyncarch_sleep(signed long delay)
{
    SDWORD current_time = (SDWORD) timeGetTime();
    SDWORD target_time = current_time + (delay >> EXTRA_PRECISION);
    while (current_time < target_time) {
        Sleep(target_time - current_time);
        current_time = timeGetTime();
    }
    //log_debug("Sleep %d ms target reached to %d ms", delay >> EXTRA_PRECISION, current_time - target_time);
}

void vsyncarch_presync(void)
{
    /* Update mouse */
    mouse_update_mouse();

    /* Update lightpen */
    win32_lightpen_update();

    /* Flush keypresses emulated through the keyboard buffer.  */
    kbdbuf_flush();
    joystick_update();
}

void vsyncarch_postsync(void)
{
    /* Dispatch all the pending UI events.  */
    ui_dispatch_events();

#if !defined(IDE_COMPILE) && !defined(WATCOM_COMPILE)
    /* prevent screensaver */
    win32_mouse_jitter();
#endif
}

int vsyncarch_vbl_sync_enabled(void)
{
    return ui_vblank_sync_enabled();
}
