/*
 * vsync.c - End-of-frame handling
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <viceteam@t-online.de>
 *  Dag Lem <resid@nimrod.no>
 *  Thomas Bretz <tbretz@gsi.de>
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

/* This does what has to be done at the end of each screen frame (50 times per
   second on PAL machines). */

/* NB! The timing code depends on two's complement arithmetic.
   unsigned long is used for the timer variables, and the difference
   between two time points a and b is calculated with (signed long)(b - a)
   This allows timer variables to overflow without any explicit
   overflow handling.
*/

#include "vice.h"

/* Port me... */

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include "clkguard.h"
#include "cmdline.h"
#include "debug.h"
#include "log.h"
#include "maincpu.h"
#include "machine.h"
#ifdef HAVE_NETWORK
#include "monitor_network.h"
#include "monitor_binary.h"
#endif
#include "network.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "vsync.h"
#include "vsyncapi.h"

#include "ui.h"

/* public metrics, updated every vsync */
double vsync_metric_cpu_percent;
double vsync_metric_emulated_fps;
int    vsync_metric_warp_enabled;

/* ------------------------------------------------------------------------- */

static int set_timer_speed(int speed);

/* Relative speed of the emulation (%).  0 means "don't limit speed". */
static int relative_speed;

/* Refresh rate.  0 means "auto". */
static int refresh_rate;

/* "Warp mode".  If nonzero, attempt to run as fast as possible. */
static int warp_mode_enabled;

static int set_relative_speed(int val, void *param)
{
    relative_speed = val;
    sound_set_relative_speed(relative_speed);
    set_timer_speed(relative_speed);

    return 0;
}

static int set_refresh_rate(int val, void *param)
{
    if (val < 0) {
        return -1;
    }

    refresh_rate = val;

    return 0;
}

static int set_warp_mode(int val, void *param)
{
    warp_mode_enabled = val ? 1 : 0;

    sound_set_warp_mode(warp_mode_enabled);
    set_timer_speed(relative_speed);

    /* resid(-dtv).cc use fast resampling for warp mode */
    sid_state_changed = 1;

    return 0;
}


/* Vsync-related resources. */
static const resource_int_t resources_int[] = {
    { "Speed", 100, RES_EVENT_SAME, NULL,
      &relative_speed, set_relative_speed, NULL },
    { "RefreshRate", 0, RES_EVENT_STRICT, (resource_value_t)1,
      &refresh_rate, set_refresh_rate, NULL },
    { "WarpMode", 0, RES_EVENT_STRICT, (resource_value_t)0,
      /* FIXME: maybe RES_EVENT_NO */
      &warp_mode_enabled, set_warp_mode, NULL },
    RESOURCE_INT_LIST_END
};


static const resource_int_t resources_int_vsid[] = {
    { "Speed", 100, RES_EVENT_SAME, NULL,
      &relative_speed, set_relative_speed, NULL },
    { "WarpMode", 0, RES_EVENT_STRICT, (resource_value_t)0,
      /* FIXME: maybe RES_EVENT_NO */
      &warp_mode_enabled, set_warp_mode, NULL },
    RESOURCE_INT_LIST_END
};



int vsync_resources_init(void)
{
    if (machine_class == VICE_MACHINE_VSID) {
        return resources_register_int(resources_int_vsid);
    }
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

/* Vsync-related command-line options. */
static const cmdline_option_t cmdline_options[] =
{
    { "-speed", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Speed", NULL,
      "<percent>", "Limit emulation speed to specified value" },
    { "-refresh", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "RefreshRate", NULL,
      "<value>", "Update every <value> frames (`0' for automatic)" },
    { "-warp", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "WarpMode", (resource_value_t)1,
      NULL, "Enable warp mode" },
    { "+warp", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "WarpMode", (resource_value_t)0,
      NULL, "Disable warp mode" },
    CMDLINE_LIST_END
};


static const cmdline_option_t cmdline_options_vsid[] =
{
    { "-speed", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Speed", NULL,
      "<percent>", "Limit emulation speed to specified value" },
    { "-warp", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "WarpMode", (resource_value_t)1,
      NULL, "Enable warp mode" },
    { "+warp", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "WarpMode", (resource_value_t)0,
      NULL, "Disable warp mode" },
    CMDLINE_LIST_END
};


int vsync_cmdline_options_init(void)
{
    if (machine_class == VICE_MACHINE_VSID) {
        return cmdline_register_options(cmdline_options_vsid);
    }
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Maximum number of frames we can skip consecutively when adjusting the
   refresh rate dynamically.  */
#define MAX_SKIPPED_FRAMES        10

/* Number of frames per second on the real machine. */
static double refresh_frequency;

/* Number of clock cycles per seconds on the real machine. */
static long cycles_per_sec;

/* Function to call at the end of every screen frame. */
static void (*vsync_hook)(void);

/* ------------------------------------------------------------------------- */

/* static guarantees zero values. */
static long vsyncarch_freq = 0;
static unsigned long now;
static double frame_ticks, frame_ticks_orig;

static int timer_speed = 0;
static int speed_eval_suspended = 1;
static int sync_reset = 1;

/* Initialize vsync timers and set relative speed of emulation in percent. */
static int set_timer_speed(int speed)
{
    speed_eval_suspended = 1;
    vsync_sync_reset();

    if (speed && refresh_frequency > 0) {
        timer_speed = speed;
        if (speed < 0) {
            /* negative speeds are fps targets */
            frame_ticks = ((double)vsyncarch_freq / (0.0 - speed));
        } else {
            /* positive speeds are cpu percent targets */
            frame_ticks = (long)(((vsyncarch_freq / refresh_frequency) * 100) / speed);
        }
        frame_ticks_orig = frame_ticks;
        
        printf("new frame ticks: %f)\n", frame_ticks); fflush(stdout);

        return 0;
    }

    timer_speed = 0;
    frame_ticks = 0;

    return 0;
}

/* ------------------------------------------------------------------------- */

void vsync_set_machine_parameter(double refresh, long cycles)
{
    refresh_frequency = refresh;
    cycles_per_sec = cycles;
    set_timer_speed(relative_speed);
}

double vsync_get_refresh_frequency(void)
{
    return refresh_frequency;
}

void vsync_init(void (*hook)(void))
{
    vsync_hook = hook;
    vsync_suspend_speed_eval();

    vsyncarch_freq = vsyncarch_frequency();  /* number of units per second */
}

/* FIXME: This function is not needed here anymore, however it is
   called from sound.c and can only be removed if all other ports are
   changed to use similar vsync code. */
int vsync_disable_timer(void)
{
    return 0;
}

/* This should be called whenever something that has nothing to do with the
   emulation happens, so that we don't display bogus speed values. */
void vsync_suspend_speed_eval(void)
{
    network_suspend();;
    vsync_sync_reset();
    speed_eval_suspended = 1;
}

void vsync_sync_reset(void)
{
    sync_reset = 1;
}

#define MEASUREMENT_SMOOTH_FACTOR 0.97
#define MEASUREMENT_FRAME_WINDOW  250

static void update_performance_metrics(unsigned long frame_time)
{
    static int oldest_measurement_index;
    static int next_measurement_index;
    static int frames_counted;

    /* For knowing the relevant timespan */
    static unsigned long frame_times[MEASUREMENT_FRAME_WINDOW];
    
    /* For measuring emulator cpu cycles per second */
    static CLOCK clocks[MEASUREMENT_FRAME_WINDOW];
    
    /* how many seconds of wallclock time the measurement window covers */
    double frame_timespan_seconds;
    
    /* how many emulated seconds of cpu time have been emulated */
    double clock_delta_seconds;
    
    if (sync_reset) {
        sync_reset = 0;

        /*
         * The emulator is just starting, or resuming from pause, or entering
         * warp, or exiting warp. So we reset the fps calculations. We also
         * throw away this initial measurement, because the emulator is just
         * about to skip the timing sleep and immediately produce the next frame.
         *
         * TODO: Don't reset numbers on unpause, and account for the gap.
         */
        
        frame_times[0] = frame_time;
        clocks[0] = maincpu_clk;
        
        next_measurement_index = 1;
        
        if (warp_mode_enabled) {
            /* Don't bother with seed measurements when entering warp mode, just reset */
            oldest_measurement_index = 0;
            frames_counted = 1;
        } else {
            /*
             * For normal speed changes, exiting warp etc, initialise with a full set
             * of fake perfect measurements
             */
            for (int i = 1; i < MEASUREMENT_FRAME_WINDOW; i++) {
                frame_times[i] = frame_time - ((MEASUREMENT_FRAME_WINDOW - i) * frame_ticks);
                clocks[i] = maincpu_clk - ((MEASUREMENT_FRAME_WINDOW - i) * cycles_per_sec / refresh_frequency);
            }
            
            oldest_measurement_index = 1;
            frames_counted = MEASUREMENT_FRAME_WINDOW;
        }
    
        /* The final smoothing function requires that we initialise the public metrics */
        if (timer_speed > 0) {
            vsync_metric_emulated_fps = (double)timer_speed * refresh_frequency / 100.0;
            vsync_metric_cpu_percent  = timer_speed;
        } else {
            vsync_metric_emulated_fps = (0.0 - timer_speed);
            vsync_metric_cpu_percent  = (0.0 - timer_speed) / refresh_frequency * 100;
        }
        
        //printf("INIT frames_counted %d %.3f seconds - %0.3f%% cpu, %.3f fps\n", frames_counted, frame_timespan_seconds, vsync_metric_cpu_percent, vsync_metric_emulated_fps); fflush(stdout);

        return;
    }
    
    /* Capure this frame's measurements */
    frame_times[next_measurement_index] = frame_time;
    clocks[next_measurement_index] = maincpu_clk;

    if(frames_counted == MEASUREMENT_FRAME_WINDOW) {
        if (++oldest_measurement_index == MEASUREMENT_FRAME_WINDOW) {
            oldest_measurement_index = 0;
        }
    } else {
        frames_counted++;
    }
    
    /* Calculate our final metrics */
    frame_timespan_seconds = (double)(frame_time - frame_times[oldest_measurement_index]) / vsyncarch_freq;
    clock_delta_seconds = (double)(maincpu_clk - clocks[oldest_measurement_index]) / cycles_per_sec;

    /* smooth and make public */
    vsync_metric_cpu_percent  = (MEASUREMENT_SMOOTH_FACTOR * vsync_metric_cpu_percent)  + (1.0 - MEASUREMENT_SMOOTH_FACTOR) * (clock_delta_seconds / frame_timespan_seconds * 100.0);
    vsync_metric_emulated_fps = (MEASUREMENT_SMOOTH_FACTOR * vsync_metric_emulated_fps) + (1.0 - MEASUREMENT_SMOOTH_FACTOR) * ((double)(frames_counted - 1) / frame_timespan_seconds);
    vsync_metric_warp_enabled = warp_mode_enabled;
    
    //printf("frames_counted %d %.3f seconds - %0.3f%% cpu, %.3f fps\n", frames_counted, frame_timespan_seconds, vsync_metric_cpu_percent, vsync_metric_emulated_fps); fflush(stdout);
    
    /* Get ready for next invoke */
    if (++next_measurement_index == MEASUREMENT_FRAME_WINDOW) {
        next_measurement_index = 0;
    }
}

/* This is called at the end of each screen frame. It flushes the audio buffer. */
int vsync_do_vsync(struct video_canvas_s *c, int been_skipped)
{
    static unsigned long next_frame_start = 0;
    unsigned long network_hook_time = 0;
    long delay;

#ifdef HAVE_NETWORK
    /* check if someone wants to connect remotely to the monitor */
    monitor_check_remote();
    monitor_check_binary();
#endif

    /*
     * process everything wich should be done before the synchronisation
     * e.g. OS/2: exit the programm if trigger_shutdown set
     */
    vsyncarch_presync();

    /* Run vsync jobs. */
    if (network_connected()) {
        network_hook_time = vsyncarch_gettime();
    }

    vsync_hook();

    if (network_connected()) {
        network_hook_time = vsyncarch_gettime() - network_hook_time;

        if (network_hook_time > (unsigned long)frame_ticks) {
            next_frame_start += network_hook_time;
            now += network_hook_time;
        }
    }

#ifdef DEBUG
    /* switch between recording and playback in history debug mode */
    debug_check_autoplay_mode();
#endif

    /* Flush sound buffer, get delay in seconds. */
    // sound_delay = sound_flush();
    sound_flush();

    /* Get current time, directly after getting the sound delay. */
    now = vsyncarch_gettime();

    /* Start afresh after pause in frame output. */
    if (speed_eval_suspended) {
        speed_eval_suspended = 0;
        vsync_sync_reset();

        next_frame_start = now;
    } else {
        update_performance_metrics(now);
    }

    /* This is the time between the start of the next frame and now. */
    delay = next_frame_start - now;
    /*
     * We sleep until the start of the next frame, if:
     *  - warp_mode is disabled
     *  - a limiting speed is given
     *  - we have not reached next_frame_start yet
     *
     * We could optimize by sleeping only if a frame is to be output.
     */
    if (!warp_mode_enabled && timer_speed && delay > 0) {
        vsyncarch_sleep(delay);
    }
    
    next_frame_start += frame_ticks;

    vsyncarch_postsync();

#ifdef VSYNC_DEBUG
    log_debug("vsync: start:%lu  delay:%ld  sound-delay:%lf  end:%lu  next-frame:%lu  frame-ticks:%lu", 
                now, delay, sound_delay * 1000000, vsyncarch_gettime(), next_frame_start, frame_ticks);
#endif

    return 0;
}
