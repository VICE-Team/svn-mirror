/** \file   tick.c
 * \brief   Relating to the management of time.
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
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

#include "mainlock.h"
#include "tick.h"

#ifdef WIN32_COMPILE
#   include <windows.h>
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

/* ------------------------------------------------------------------------- */

#ifdef WIN32_COMPILE
static LARGE_INTEGER timer_frequency;
static HANDLE wait_timer;
#elif defined(MACOSX_SUPPORT)
static mach_timebase_info_data_t timebase_info;
#endif

void tick_init(void)
{
#ifdef WIN32_COMPILE
    QueryPerformanceFrequency(&timer_frequency);

    wait_timer = CreateWaitableTimer(NULL, TRUE, NULL);

#elif defined(MACOSX_SUPPORT)
    mach_timebase_info(&timebase_info);

#endif
}

tick_t tick_per_second(void)
{
    return TICK_PER_SECOND;
}

/* Get time in timer units. */
tick_t tick_now(void)
{
#ifdef WIN32_COMPILE
    LARGE_INTEGER time_now;
    
    QueryPerformanceCounter(&time_now);

    return (tick_t)(time_now.QuadPart / ((double)timer_frequency.QuadPart / TICK_PER_SECOND));

#elif defined(MACOSX_SUPPORT)
    return NANO_TO_TICK(mach_absolute_time() * timebase_info.numer / timebase_info.denom);

#else
    struct timespec now;
#   if defined(__linux__)
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
#   elif defined(__FreeBSD__)
        clock_gettime(CLOCK_MONOTONIC_PRECISE, &now);
#   else
        clock_gettime(CLOCK_MONOTONIC, &now);
#   endif

    return NANO_TO_TICK(((uint64_t)NANO_PER_SECOND * now.tv_sec) + now.tv_nsec);

#endif
}

/* Sleep a number of timer units. */
void tick_sleep(tick_t sleep_ticks)
{
    /* do this asap. */
    tick_t before_yield_tick = tick_now();
    tick_t yield_ticks;

    /* Since we're about to sleep, give another thread a go of the lock */
    mainlock_yield_once();

    yield_ticks = tick_now_delta(before_yield_tick);

    if (yield_ticks >= sleep_ticks) {
        /* The lock yield took as long as we needed to sleep (or longer) */
        return;
    }

    /* Adjust sleep_ticks to account for the yield time */
    sleep_ticks -= yield_ticks;

#ifdef WIN32_COMPILE
    LARGE_INTEGER timeout;

    timeout.QuadPart = 0LL - (timer_frequency.QuadPart / ((double)TICK_PER_SECOND / sleep_ticks));

    SetWaitableTimer(wait_timer, &timeout, 0, NULL, NULL, 0);
    WaitForSingleObject(wait_timer, INFINITE);

#elif defined(HAVE_NANOSLEEP)
    struct timespec ts;
    uint64_t nanos = TICK_TO_NANO(sleep_ticks);

    if (nanos < NANO_PER_SECOND) {
        ts.tv_sec = 0;
        ts.tv_nsec = nanos;
    } else {
        ts.tv_sec = nanos / NANO_PER_SECOND;
        ts.tv_nsec = nanos % NANO_PER_SECOND;
    }

    nanosleep(&ts, NULL);

#else
    if (usleep(TICK_TO_MICRO(sleep_ticks)) == -EINVAL) {
        usleep(MICRO_PER_SECOND - 1);
    }

#endif
}

tick_t tick_now_after(tick_t previous_tick)
{
    /*
     * Fark, high performance counters, called from different threads / cpus, can be off by 1 tick.
     * 
     *    "When you compare performance counter results that are acquired from different
     *     threads, consider values that differ by ± 1 tick to have an ambiguous ordering.
     *     If the time stamps are taken from the same thread, this ± 1 tick uncertainty
     *     doesn't apply. In this context, the term tick refers to a period of time equal
     *     to 1 ÷ (the frequency of the performance counter obtained from
     *     QueryPerformanceFrequency)."
     * 
     * https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps#guidance-for-acquiring-time-stamps
     */

    tick_t after = tick_now();

    if (after == previous_tick - 1) {
        after = previous_tick;
    }
    
    return after;
}

tick_t tick_now_delta(tick_t previous_tick)
{
    return tick_now_after(previous_tick) - previous_tick;
}
