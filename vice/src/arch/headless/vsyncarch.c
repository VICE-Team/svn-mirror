/** \file   vsyncarch.c
 * \brief   End-of-frame handling for native GTK3 UI
 *
 * \note    This is altered and trimmed down to fit into the GTK3-native
 *          world, but it's still heavily reliant on UNIX internals.
 *
 * \author  Dag Lem <resid@nimrod.no>
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

#include "kbdbuf.h"
#include "mainlock.h"
#include "ui.h"
#include "vsyncapi.h"
#include "videoarch.h"

#include "joy.h"

#ifdef WIN32_COMPILE
#   include "windows.h"
#elif defined(HAVE_NANOSLEEP)
#   include <time.h>
#else
#   include <unistd.h>
#   include <errno.h>
#   include <sys/time.h>
#endif

#ifdef MACOSX_SUPPORT
#   include <mach/mach.h>
#   include <mach/mach_time.h>
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

static int pause_pending = 0;

/* ------------------------------------------------------------------------- */

#ifndef WIN32_COMPILE
#   ifdef HAVE_NANOSLEEP
#       define TICKSPERSECOND  1000000000L  /* Nanoseconds resolution. */
#       define TICKSPERMSEC    1000000L
#       define TICKSPERUSEC    1000L
#       define TICKSPERNSEC    1L
#   else
#       define TICKSPERSECOND  1000000L     /* Microseconds resolution. */
#       define TICKSPERMSEC    1000L
#       define TICKSPERUSEC    1L
#   endif
#endif

#ifdef WIN32_COMPILE
static LARGE_INTEGER timer_frequency;
static HANDLE wait_timer;
#endif

void vsyncarch_init(void)
{
#ifdef WIN32_COMPILE
    QueryPerformanceFrequency(&timer_frequency);

    wait_timer = CreateWaitableTimer(NULL, TRUE, NULL);
#endif
}

unsigned long vsyncarch_frequency(void)
{
#ifdef WIN32_COMPILE
    return timer_frequency.QuadPart;
#else
    return TICKSPERSECOND;
#endif
}

/* Get time in timer units. */
unsigned long vsyncarch_gettime(void)
{
#ifdef WIN32_COMPILE
    LARGE_INTEGER time_now;
    
    QueryPerformanceCounter(&time_now);

    return time_now.QuadPart;

#elif defined(HAVE_NANOSLEEP)
#   ifdef MACOSX_SUPPORT
        static uint64_t factor = 0;
        uint64_t time = mach_absolute_time();
        if (!factor) {
            mach_timebase_info_data_t info;
            mach_timebase_info(&info);
            factor = info.numer / info.denom;
        }
        return time * factor;
#   else
        struct timespec now;
#       if defined(__linux__)
            clock_gettime(CLOCK_MONOTONIC_RAW, &now);
#       elif defined(__FreeBSD__)
            clock_gettime(CLOCK_MONOTONIC_PRECISE, &now);
#       else
            clock_gettime(CLOCK_MONOTONIC, &now);
#       endif
        return (TICKSPERSECOND * now.tv_sec) + (TICKSPERNSEC * now.tv_nsec);
#   endif
#else
    /* this is really really bad, we should never use the wallclock
       see: https://blog.habets.se/2010/09/gettimeofday-should-never-be-used-to-measure-time.html */
    struct timeval now;
    gettimeofday(&now, NULL);
    return (TICKSPERSECOND * now.tv_sec) + (TICKSPERUSEC * now.tv_usec);
#endif
}

/* Sleep a number of timer units. */
void vsyncarch_sleep(unsigned long delay)
{
    static double smoothed_running_oversleep = 0.0;

    unsigned long before = vsyncarch_gettime();
    unsigned long after;
    unsigned long target = before + delay;

    long oversleep;

#ifdef USE_VICE_THREAD
    /* Don't hold the mainlock while sleeping */
    mainlock_yield_begin();
#endif

    /*
     * Try to avoid oversleeping by compensating for our worse oversleep and
     * busy looping for the rest of the period
     */

    if (delay > smoothed_running_oversleep) {
        delay -= smoothed_running_oversleep;
    } else {
        /* still yield if some other process is waiting */
        delay = 0;
    }

#ifdef WIN32_COMPILE
    LARGE_INTEGER timeout;
    
    timeout.QuadPart = delay;
    timeout.QuadPart *= -10 * 1000 * 1000;
    timeout.QuadPart /= timer_frequency.QuadPart;

    SetWaitableTimer(wait_timer, &timeout, 0, NULL, NULL, 0);
    WaitForSingleObject(wait_timer, INFINITE);

#elif defined(HAVE_NANOSLEEP)
    struct timespec ts;

    if (delay < TICKSPERSECOND) {
        ts.tv_sec = 0;
        ts.tv_nsec = delay;
    } else {
        ts.tv_sec = delay / TICKSPERSECOND;
        ts.tv_nsec = (delay % TICKSPERSECOND);
    }

    nanosleep(&ts, NULL);

#else
    if (usleep(delay) == -EINVAL) usleep(999999);
#endif

    after = vsyncarch_gettime();

    oversleep = after - before - delay;
    
    smoothed_running_oversleep = (0.9 * smoothed_running_oversleep) + (0.1 * oversleep);

#if 0
        printf(
            "overslept %.1f ms, set oversleep compensation to %.1f ms\n",
            (double)oversleep * 1000 / vsyncarch_frequency(),
            /*(double)oversleep_compensate * 1000 / vsyncarch_frequency());*/
            smoothed_running_oversleep * 1000 / vsyncarch_frequency());
        fflush(stdout);
#endif

#ifdef USE_VICE_THREAD
    /* Get the mainlock back before the busy loop */
    mainlock_yield_end();
#endif

    /* busy loop until we've reached the target time */
    while (vsyncarch_gettime() < target)
        ;
}

void vsyncarch_presync(void)
{
    ui_update_lightpen();
    joystick();
}

void vsyncarch_postsync(void)
{
    /* this function is called once a frame, so this
       handles single frame advance */
    if (pause_pending) {
        ui_pause_enable();
        pause_pending = 0;
    }
}

void vsyncarch_advance_frame(void)
{
    ui_pause_disable();
    pause_pending = 1;
}
