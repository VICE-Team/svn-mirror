/*
 * vsync.c - Display synchronization for Win32.
 *
 * Written by
 *  Tibor Biczo         (crown@mail.matav.hu)
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

#include "vsync.h"

#include "cmdline.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "resources.h"
#include "sound.h"
#include "ui.h"
#include "joystick.h"

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
    sound_set_warp_mode(warp_mode_enabled);
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

static int timer_interval;

static DWORD    last_time;

int vsync_init(double hertz, long cycles, void (*hook)(void))
{
    vsync_hook = hook;
    refresh_frequency = hertz;
    cycles_per_sec = cycles;
    suspend_speed_eval();
    vsync_disable_timer();

    /* This makes sure we evaluate speed correctly the first time.  */
    speed_eval_suspended = 1;

    return 0;
}

int do_vsync(int been_skipped)
{
    int skip_next_frame = 0;    /* 0 means "never skip".  */
    DWORD   now_time;
    DWORD   diff_time;

    /* Call the hooks that need to be executed at every vertical retrace.  */
    vsync_hook();

    /* Dispatch all the pending UI events.  */
    ui_dispatch_events();

    now_time=timeGetTime();

    if (warp_mode_enabled) {
        /* "Warp" mode: run as fast as possible.  */
        if (skip_counter >= MAX_SKIPPED_FRAMES) {
            skip_counter = 0;
            skip_next_frame = 0;
        } else {
            skip_counter++;
            skip_next_frame = 1;
        }
    } else if (refresh_rate > 0) {
        /* Fixed refresh rate.  */
        if (timer_speed != 0) {
            now_time=timeGetTime();
            diff_time=now_time-last_time;
            while (diff_time<timer_interval*(skip_counter+1)) {
                now_time=timeGetTime();
                diff_time=now_time-last_time;
            }
        }
        if (skip_counter >= refresh_rate - 1) {
            skip_counter = 0;
            skip_next_frame = 0;
            last_time=now_time;
        } else {
            skip_counter++;
            skip_next_frame = 1;
        }
    } else {
        /* Automatic adjustment.  */
        if (timer_speed == 0) {
            skip_next_frame = 0;
            skip_counter = 0;
        } else {
            now_time=timeGetTime();
            diff_time=now_time-last_time;
            if (diff_time<timer_interval*(skip_counter+1)) {
                /*  We are too fast: wait */
                while (diff_time<timer_interval*(skip_counter+1)) {
                    now_time=timeGetTime();
                    diff_time=now_time-last_time;
                }
                skip_counter=0;
                last_time=now_time;
            } else {
                /*  We are too slow: drop frames */
                if (skip_counter < MAX_SKIPPED_FRAMES) {
                    skip_counter++;
                    skip_next_frame = 1;
                } else {
                    /*  Too many skipped frames: give up!  */
                    skip_counter = 0;
                    skip_next_frame = 0;
                    last_time=now_time;
                }
            }
        }
    }

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

    sound_flush(warp_mode_enabled ? 0 : relative_speed);

    /* Flush keypresses emulated through the keyboard buffer.  */
    kbd_buf_flush();
    joystick_update();



    if (timer_speed!=relative_speed) {
        if (relative_speed!=0) {
            timer_interval=(int)(((100.0/(double)relative_speed)/refresh_frequency)*1000.0+.5);
            DEBUG(("Setting up timer -- interval = %d msec.", interval));
        }
        timer_speed = relative_speed;
        last_time=now_time;
        skip_counter=0;
    }

    return skip_next_frame;
}

void vsync_prevent_clk_overflow(CLOCK sub)
{
    speed_eval_clk_start-=sub;
}

void suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

void vsync_disable_timer(void)
{
    timer_speed=0;
}
