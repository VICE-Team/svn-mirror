/*
 * vsync.c - End-of-frame handling
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen   <tvr@cs.hut.fi>
 *  Andreas Boose    <boose@linux.rz.fh-hannover.de>
 *  Dag Lem          <resid@nimrod.no>
 *  Thomas Bretz     <tbretz@gsi.de>
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

/* Port me... */
#if defined OS2 || defined WIN32

#include "log.h"
#include "sound.h"      // sound_flush
#include "maincpu.h"    // maincpu_clk_guard
#include "clkguard.h"   // clk_guard_add_callback
#include "vsyncarch.h"  // archdep part of synchronisation

#include "cmdline.h"
#include "resources.h"

/* ------------------------------------------------------------------------- */

static int set_timer_speed(int speed);

/* Relative speed of the emulation (%).  0 means "don't limit speed". */
static int relative_speed;

/* Refresh rate.  0 means "auto". */
static int refresh_rate;

/* "Warp mode".  If nonzero, attempt to run as fast as possible. */
static int warp_mode_enabled;

static int set_relative_speed(resource_value_t v, void *param)
{
    relative_speed = (int) v;
    set_timer_speed(relative_speed);
    return 0;
}

static int set_refresh_rate(resource_value_t v, void *param)
{
    if ((int) v < 0)
        return -1;
    refresh_rate = (int) v;
    return 0;
}

static int set_warp_mode(resource_value_t v, void *param)
{
    warp_mode_enabled = (int) v;
    sound_set_warp_mode(warp_mode_enabled);
    set_timer_speed(relative_speed);
    return 0;
}

/* Vsync-related resources. */
static resource_t resources[] = {
    { "Speed", RES_INTEGER, (resource_value_t) 100,
      (resource_value_t *) &relative_speed, set_relative_speed, NULL },
    { "RefreshRate", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &refresh_rate, set_refresh_rate, NULL },
    { "WarpMode", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &warp_mode_enabled, set_warp_mode, NULL },
    { NULL }
};

int vsync_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Vsync-related command-line options. */
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
#define MAX_SKIPPED_FRAMES	10

/* Number of frames per second on the real machine. */
static double refresh_frequency;

/* Number of clock cycles per seconds on the real machine. */
static long cycles_per_sec;

/* Function to call at the end of every screen frame. */
static void (*vsync_hook)(void);

/* ------------------------------------------------------------------------- */
static unsigned long display_start;
static unsigned long frame_start;
static signed long frame_ticks;

static int timer_speed = 0;
static int speed_eval_suspended = 1;

/* Initialize vsync timers and set relative speed of emulation in percent. */
static int set_timer_speed(int speed)
{
    speed_eval_suspended = 1;

    if (speed > 0 && refresh_frequency > 0)
    {
        timer_speed = speed;
        frame_ticks = (unsigned long)((double)vsyncarch_timescale()/refresh_frequency*100.0/speed);
    }
    else
    {
        timer_speed = 0;
        frame_ticks = 0;
    }
    return 0;
}

void vsync_disable_timer()
{
}

/* This should be called whenever something that has nothing to do with the
   emulation happens, so that we don't display bogus speed values. */
void suspend_speed_eval(void)
{
    sound_suspend();
    speed_eval_suspended = 1;
}

/* ------------------------------------------------------------------------- */

static CLOCK speed_eval_prev_clk;

static void clk_overflow_callback(CLOCK amount, void *data)
{
    speed_eval_prev_clk -= amount;
}

void vsync_set_machine_parameter(double ref_rate, long cycles)
{
    refresh_frequency = ref_rate;
    cycles_per_sec    = cycles;
    set_timer_speed(relative_speed);
}

void vsync_init(void (*hook)(void))
{
    vsync_hook = hook;
    suspend_speed_eval();
    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);

    vsyncarch_init();
}

void vsync_prevent_clk_overflow(CLOCK sub)
{
    speed_eval_prev_clk -= sub;
}

/* ------------------------------------------------------------------------- */

/* Display speed (percentage) and frame rate (frames per second). */
static void display_speed(int num_frames)
{
    const unsigned long now = vsyncarch_gettime();

    const CLOCK  diff_clk = clk - speed_eval_prev_clk;
    const double diff_tm  = (double)(now - display_start)/vsyncarch_timescale();

    const double speed_index = diff_clk/(cycles_per_sec*diff_tm);
    const double frame_rate  = num_frames/diff_tm;

#ifndef OS2
    if ((now - frame_start) / vsyncarch_timescale() >= 1.0)
    {
        suspend_speed_eval();
    }
#endif

    vsyncarch_display_speed(speed_index*100, frame_rate, warp_mode_enabled);

    display_start       = now;
    speed_eval_prev_clk = clk;
}

/* This is called at the end of each screen frame. It flushes the
   audio buffer and keeps control of the emulation speed. */
int do_vsync(int been_skipped)
{
    static int frame_counter  = 0;
    static int skipped_frames = 0;
    static int skipped_redraw = 0;
    static int display_timer  = 0;

    unsigned long frame_stop, now;
    signed long delay;
    int skip_next_frame;
    int frame_delay;

    /* process everything wich should be done before the syncronisation
     e.g. OS/2: exit the programm if trigger_shutdown set */
    vsyncarch_presync();

    if (been_skipped)
        skipped_frames++;

    /* Get current time. */
    now = vsyncarch_gettime();

    /* Start afresh after pause in frame output. */
    if (speed_eval_suspended)
    {
        speed_eval_prev_clk  = clk;
        display_start        = now;
        frame_start          = now;
        display_timer        = now;
        skipped_redraw       = 0;
        skipped_frames       = 0;
        frame_counter        = 0;
        speed_eval_suspended = 0;
    }

    frame_stop = frame_start + frame_ticks;

    /* Calculate the delay since last time. Because of the base
     arithmetic we don't need an overflow handling here */
    delay = frame_stop - now;

    /* Check whether we're on time. Allow a discrepancy of one frame
      before possible skipping the next frame */
    if (warp_mode_enabled || !timer_speed || (delay+frame_ticks < 0))
    {
        /* Skip next frame if allowed.
        // If warp_mode_enabled is set, as many frames are skipped as allowed.
        // If refresh_rate is set, frames are only skipped to output frames
        // at the specified interval, not to keep up to speed. */
        if (skipped_redraw < MAX_SKIPPED_FRAMES &&
            (warp_mode_enabled || !refresh_rate ||
             skipped_redraw < refresh_rate - 1))
        {
            skip_next_frame = 1;
            skipped_redraw++;
        }
        else
        {
            skip_next_frame = 0;
            skipped_redraw  = 0;
        }
    }
    else
    {
        /* Output frames at specified interval. */
        if (refresh_rate && (skipped_redraw < refresh_rate - 1) &&
            (skipped_redraw < MAX_SKIPPED_FRAMES))
        {
            skip_next_frame = 1;
            skipped_redraw++;
        }
        else
        {
            skip_next_frame = 0;
            skipped_redraw  = 0;
        }

        /* Sleep until start of frame. We have to sleep even if no frame
        // is output because of sound synchronization. */
        vsyncarch_sleep(delay);
    }

    /* This point in the code is reached at a more or less constant time
    // frequency; this is necessary for the synchronization in sound_flush. */
    frame_delay = -sound_flush(warp_mode_enabled ? 0 : relative_speed);

    /* Set time for next frame. */
    frame_start += frame_ticks*(frame_delay + 1);

    /* Update display every two seconds (user-time, not vice-time) */
    /* was: if (++frame_counter > refresh_frequency*2) */
    frame_counter++;

    if (2*vsyncarch_timescale() < (now-display_timer))
    {
        display_speed(frame_counter - skipped_frames);
        display_timer  = now;
        frame_counter  = 0;
        skipped_frames = 0;
    }

    /* Run vsync jobs. */
    vsync_hook();

    vsyncarch_postsync();

    return skip_next_frame;
}
#endif

