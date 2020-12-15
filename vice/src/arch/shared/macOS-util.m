/**
 * \file macOS-util.m
 * \brief A collection of little macOS helpers.
 *
 * Calling Objective-C code from C is possible, but not very readable.
 * Nicer to just plop a few readable functions in here.
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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

#import "vice.h"

#import "macOS-util.h"

#import <Cocoa/Cocoa.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import <pthread.h>
#import <os/log.h>
#import <os/signpost.h>

#define NANOS_PER_MICRO 1000ULL
#define MICROS_PER_SEC  1000000ULL

void vice_macos_set_main_thread(void)
{
    /*
     * Cocoa doesn't behave if it doesn't know that it's multithreaded.
     * Starting a new thread via the NSThread interface is enough to flip
     * the bit.
     */
    
    [NSThread detachNewThreadSelector: @selector(class)
                             toTarget: [NSObject class]
                           withObject: nil];

    /*
     * Disable app-nap
     */
    [[NSProcessInfo processInfo] beginActivityWithOptions: NSActivityUserInitiated | NSActivityLatencyCritical
                                                   reason: @"Accurate emulation requires low latency access to resources."];

    /* Available from OSX 10.10+ */
    if ([NSThread instancesRespondToSelector:@selector(setQualityOfService:)]) {
        /* The main thread benefits from interactive response levels */
        [[NSThread currentThread] setQualityOfService: NSQualityOfServiceUserInteractive];
    }
}

#if 0 /* Disabled as affects warp speed dramatically */
static void move_pthread_to_realtime_scheduling_class(pthread_t pthread, int period_microseconds, int typical_work_microseconds, int max_work_microseconds)
{
    /*
     * https://developer.apple.com/library/archive/technotes/tn2169/_index.html
     */

    mach_timebase_info_data_t timebase_info;
    mach_timebase_info(&timebase_info);

    double clock2abs = ((double)timebase_info.denom / (double)timebase_info.numer) * NANOS_PER_MICRO;

    thread_time_constraint_policy_data_t policy;
    policy.period      = (uint32_t)(clock2abs * period_microseconds);
    policy.computation = (uint32_t)(clock2abs * typical_work_microseconds);
    policy.constraint  = (uint32_t)(clock2abs * max_work_microseconds);
    policy.preemptible = FALSE;

    int kr =
        thread_policy_set(
            pthread_mach_thread_np(pthread_self()),
                THREAD_TIME_CONSTRAINT_POLICY,
                (thread_policy_t)&policy,
                THREAD_TIME_CONSTRAINT_POLICY_COUNT);

    if (kr != KERN_SUCCESS) {
        mach_error("thread_policy_set:", kr);
        exit(1);
    }
}
#endif

void vice_macos_set_vice_thread_priority(void)
{
    [[NSThread currentThread] setThreadPriority: 1.0];

    /* typically vice thread will run for a couple ms before blocking, but lets not penalise it for running longer */
    /* move_pthread_to_realtime_scheduling_class(pthread_self(), 0, MICROS_PER_SEC / 60, MICROS_PER_SEC / 60); */
}

void vice_macos_set_render_thread_priority(void)
{
    [[NSThread currentThread] setThreadPriority: 1.0];

    /* Likely a 60fps system, passing rendered buffer to OpenGL shouldn't take more than a couple of ms. */
    /* move_pthread_to_realtime_scheduling_class(pthread_self(), MICROS_PER_SEC / 60, MICROS_PER_SEC / 1000 * 2, MICROS_PER_SEC / 1000 * 3); */
}
