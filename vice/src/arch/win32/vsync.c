/*
 * vsync.c - Display synchronization for Win32.
 *
 * Written by
 *  Ettore Perazzoli    (ettore@comm2000.it)
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

#include <stdio.h>
#include <windows.h>

//#define NONAMELESSUNION
//#define HTASK HANDLE
//#include <wtypes.h>

//#include <mmsystem.h>

#include "vsync.h"

#include "cmdline.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "resources.h"
#include "sound.h"
#include "ui.h"

#include <mmsystem.h>

/* #Define to enable `OutputDebugString()' messages.  */
/*#define DEBUG_VSYNC*/

/* ------------------------------------------------------------------------ */

/* Debugging stuff.  */
#ifdef DEBUG_VSYNC
static void vsync_debug(const char *format, ...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        log_debug(tmp);
}
#define DEBUG(x) vsync_debug x
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------- */

/* Relative speed of the emulation (%).  0 means "don't limit speed".  */
static int relative_speed;

/* Refresh rate.  0 means "auto".  */
static int refresh_rate;

/* "Warp mode".  If nonzero, attempt to run as fast as possible.  */
static int warp_mode_enabled;

/* FIXME: This should call `set_timers'.  */
static int set_relative_speed(resource_value_t v)
{
    relative_speed = (int) v;
    return 0;
}

static int set_refresh_rate(resource_value_t v)
{
    if ((int) v < 0)
        return -1;
    refresh_rate = (int) v;
    return 0;
}

static int set_warp_mode(resource_value_t v)
{
    warp_mode_enabled = (int) v;
    return 0;
}

/* Vsync-related resources.  */
static resource_t resources[] = {
    { "Speed", RES_INTEGER, (resource_value_t) 100,
      (resource_value_t *) &relative_speed, set_relative_speed },
    { "RefreshRate", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &refresh_rate, set_refresh_rate },
    { "WarpMode", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &warp_mode_enabled, set_warp_mode },
    { NULL }
};

int vsync_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Vsync-related command-line options.  */
static cmdline_option_t cmdline_options[] = {
    { "-speed", SET_RESOURCE, 1, NULL, NULL, "Speed", NULL,
      "<percent>", "Limit emulation speed to specified value" },
    { "-refresh", SET_RESOURCE, 1, NULL, NULL, "RefreshRate", NULL,
      "<value>", "Update every <value> frames (`0' for automatic)" },
    { "-warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t) 1,
      NULL, "Enable warp mode" },
    { "+warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t) 0,
      NULL, "Disable warp mode" },
    { NULL }
};

int vsync_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Wanted Win32 timer tolerance, in milliseconds.  */
#define TARGET_TIMER_TOLERANCE 0
/* Actual timer tolerance.  */
static UINT timer_tolerance;

/* Maximum number of frames we can skip consecutively when adjusting the
   refresh rate dynamically.  */
#define MAX_SKIPPED_FRAMES      10

/* Number of frames per second on the real machine.  */
static double refresh_frequency;

/* Number of clock cycles per seconds on the real machine.  */
static long cycles_per_sec;

/* Function to call at the end of every screen frame.  */
static void (*vsync_hook)(void);

/* Flag: if nonzero, it means we have stopped evaluating the average speed.
   Evaluation will start again at the next `do_vsync()' call.  */
static int speed_eval_suspended = 1;

/* Count elapsed frames.  */
static int frame_counter;

/* Count skipped frames.  */
static int skip_counter = 0;

/* Count skipped frames in an evaluation interval.  */
static int drawn_frames;

/* Start of speed evaluation period.  */
static DWORD speed_eval_start;

/* Clock value at start of speed evalutation period.  */
static CLOCK speed_eval_clk_start;

/* Speed of the timer callback; 0 = no callback.  */
static int timer_speed = 0;

/* ID of the timer.  */
static UINT timer_id;

/* This struct is used by the timer event.  */
struct {
    /* Used to control concurrency between `do_vsync()' timer callback.  */
    CRITICAL_SECTION critical_section;

    /* Incremented every timer event.  */
    int frame_counter;

    /* Set to 1 by `do_vsync()' when it's waiting for the next timer event
       for slowdown purposes.  */
    int waiting;

    /* Semaphore used for slowdown.  */
    HANDLE semaphore;
} timer_stuff;

/* ------------------------------------------------------------------------- */

void CALLBACK timer_callback(UINT timer_id, UINT msg,
                             DWORD user, DWORD dw1, DWORD dw2)
{
        static int count;
        static DWORD start_time;

        count++;
        if (count == 50) {
                DEBUG(("timer_callback %d msec.", timeGetTime() - start_time));
                start_time = timeGetTime();
                count = 0;
        }
    EnterCriticalSection(&timer_stuff.critical_section);
    DEBUG(("Callback! %d -> %d",
           timer_stuff.frame_counter, timer_stuff.frame_counter + 1));
    timer_stuff.frame_counter++;
    if (timer_stuff.waiting) {
        ReleaseSemaphore(timer_stuff.semaphore, 1, NULL);
        timer_stuff.waiting = 0;
        timer_stuff.frame_counter = 0;
    }
    LeaveCriticalSection(&timer_stuff.critical_section);
}

/* ------------------------------------------------------------------------ */

void vsync_cleanup(void)
{
    timeKillEvent(timer_id);
    timeEndPeriod(timer_tolerance);
    DEBUG(("vsync_cleanup()"));
}

int vsync_init(double hertz, long cycles, void (*hook)(void))
{
    vsync_hook = hook;
    refresh_frequency = hertz;
    cycles_per_sec = cycles;
    suspend_speed_eval();
    vsync_disable_timer();

    /* This makes sure we evaluate speed correctly the first time.  */
    speed_eval_suspended = 1;

    {
        TIMECAPS tc;

        if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
            return -1;

        timer_tolerance = min(max(tc.wPeriodMin, TARGET_TIMER_TOLERANCE),
                              tc.wPeriodMax);
        timeBeginPeriod(timer_tolerance);
        DEBUG(("Timer tolerance set to %d msec.", timer_tolerance));
        /* Darn M$ -- why doesn't `atexit()' work?!  */
        atexit(vsync_cleanup);
    }

    /* Initialize critical section handling.  */
    memset(&timer_stuff, 0, sizeof(timer_stuff));
    InitializeCriticalSection(&timer_stuff.critical_section);

    /* Create semaphore.  */
    timer_stuff.semaphore = CreateSemaphore(NULL, 0, 1,
                                            "VSyncSemaphore");

    if (timer_stuff.semaphore == 0) {
        DEBUG(("Cannot create semaphore!"));
        return -1;
    }

    return 0;
}

int do_vsync(int been_skipped)
{
    int skip_next_frame = 0;    /* 0 means "never skip".  */

    /* Call the hooks that need to be executed at every vertical retrace.  */
    vsync_hook();

    /* Dispatch all the pending UI events.  */
    ui_dispatch_events();

    if (timer_speed != relative_speed) {
        int interval;

        if (timer_id != 0)
            timeKillEvent(timer_id);
        if (relative_speed!=0) {
            interval = (int)(((100.0 / (double) relative_speed)
                                / refresh_frequency) * 1000.0 + .5);
            DEBUG(("Setting up timer -- interval = %d msec.", interval));
            timer_id = timeSetEvent(interval, 0, timer_callback, 0, TIME_PERIODIC);
            if (timer_id == 0) {
                DEBUG(("timeSetEvent failed!"));
                timer_speed = 0;
            } else {
                timer_speed = relative_speed;
            }
        } else {
            timer_speed = relative_speed;
        }
    }

    if (warp_mode_enabled) {
        /* "Warp" mode: run as fast as possible.  */
        if (skip_counter >= MAX_SKIPPED_FRAMES) {
            skip_counter = 0;
            skip_next_frame = 0;
        } else {
            skip_counter++;
            skip_next_frame = 1;
        }
        sound_flush(0);
    } else if (refresh_rate > 0) {
        /* Fixed refresh rate.  */
        if (skip_counter >= refresh_rate - 1) {
            skip_counter = 0;
            skip_next_frame = 0;
        } else {
            skip_counter++;
            skip_next_frame = 1;
        }
        /* Notice that we check `timer_speed' and not `speed' here: this
           allows us to handle the "not working timer" case more nicely.  */
        if (timer_speed != 0) {
            EnterCriticalSection(&timer_stuff.critical_section);
            if (timer_stuff.frame_counter == 0) {
                /* Signal we want to wait for the next timer event...  */
                timer_stuff.waiting = 1;
                LeaveCriticalSection(&timer_stuff.critical_section);
                DEBUG(("Now waiting!"));
                WaitForSingleObject(timer_stuff.semaphore, INFINITE);
                DEBUG(("Now done!"));
#if 0
                EnterCriticalSection(&timer_stuff.critical_section);
                timer_stuff.frame_counter = 0;
                LeaveCriticalSection(&timer_stuff.critical_section);
#endif
            } else {
                DEBUG(("Nothing to wait: timer_stuff.frame_counter = %d",
                       timer_stuff.frame_counter));
                timer_stuff.frame_counter = 0;
                LeaveCriticalSection(&timer_stuff.critical_section);
            }
        }
        sound_flush(relative_speed);
    } else {
        /* Automatic adjustment.  */
        if (timer_speed == 0) {
            skip_next_frame = 0;
            skip_counter = 0;
        } else {
            if (timer_stuff.frame_counter <= skip_counter) {
                /* We are too fast: sleep.  */
                EnterCriticalSection(&timer_stuff.critical_section);
                timer_stuff.waiting = 1;
                DEBUG(("Now waiting! frame_counter %d skip_counter %d",
                       timer_stuff.frame_counter, skip_counter));
                LeaveCriticalSection(&timer_stuff.critical_section);
                WaitForSingleObject(timer_stuff.semaphore, INFINITE);
                DEBUG(("Now done!"));
                skip_counter = 0;
#if 0
                EnterCriticalSection(&timer_stuff.critical_section);
                timer_stuff.frame_counter = 0;
#endif
            } else {
                DEBUG(("Too slow frame_counter %d skip_counter %d",
                       timer_stuff.frame_counter, skip_counter));
                EnterCriticalSection(&timer_stuff.critical_section);
                /* We are too slow: skip frames to catch up.  */
                if (skip_counter < MAX_SKIPPED_FRAMES) {
                    skip_counter++;
                    skip_next_frame = 1;
                } else {
                    /* Too many skipped frames: give up!  */
                    timer_stuff.frame_counter = 0;
                    skip_counter = 0;
                    skip_next_frame = 0;
                }
                LeaveCriticalSection(&timer_stuff.critical_section);
            }
        }
        sound_flush(relative_speed);
    }

    /* Flush keypresses emulated through the keyboard buffer.  */
    kbd_buf_flush();

    if (speed_eval_suspended || clk < speed_eval_clk_start) {
        drawn_frames = 0;
        frame_counter = 0;
        speed_eval_start = timeGetTime();
        speed_eval_clk_start = clk;
    } else {
        frame_counter++;

        /* Evaluate speed every 2 emulated seconds.  */
        if (frame_counter == 2 * (int)(refresh_frequency + .5)) {
            DWORD speed_eval_end;
            double elapsed_secs, speed_index;
            double fps;

            speed_eval_end = timeGetTime();
            elapsed_secs = ((double) (speed_eval_end - speed_eval_start)
                            / 1000.0);
            speed_index = 100.0 * ((double) (clk - speed_eval_clk_start)
                                   / elapsed_secs / cycles_per_sec);
            fps = (double) drawn_frames / elapsed_secs;
            DEBUG(("%ld clocks in %.4f secs",
                   clk - speed_eval_clk_start, elapsed_secs));
            ui_display_speed((float) speed_index, (float) fps,
                             warp_mode_enabled);
            speed_eval_start = speed_eval_end;
            speed_eval_clk_start = clk;
            drawn_frames = frame_counter = 0;
        }
    }

    if (!skip_next_frame)
        drawn_frames++;

    if (speed_eval_suspended)
        speed_eval_suspended = 0;

    return skip_next_frame;
}

void vsync_prevent_clk_overflow(CLOCK sub)
{
}

double vsync_get_avg_frame_rate(void)
{
    return 0.0;
}

double vsync_get_avg_speed_index(void)
{
    return 0.0;
}

void suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

void vsync_disable_timer(void)
{
    if (timer_id != 0) timeKillEvent(timer_id);
    timer_speed=-1;
}