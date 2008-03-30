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

/* This does what has to be done at the end of each screen frame (50 times per
   second on PAL machines). */

/* NB! The timing code depends on two's complement arithmetic.
   unsigned long is used for the timer variables, and the difference
   between two time points a and b is calculated with (long)(b - a)
   This allows timer variables to overflow without any explicit
   overflow handling. Time is measured in microseconds.
*/

/* Port me... */
#if !defined(MSDOS) && !defined(RISCOS)

#include "vice.h"

#include "clkguard.h"
#include "cmdline.h"
#include "log.h"
#include "maincpu.h"
#include "sound.h"
#include "vsync.h"
#include "vsyncapi.h"

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
    sound_set_relative_speed(relative_speed);
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
      "<percent>", N_("Limit emulation speed to specified value") },
    { "-refresh", SET_RESOURCE, 1, NULL, NULL, "RefreshRate", NULL,
      "<value>", N_("Update every <value> frames (`0' for automatic)") },
    { "-warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t) 1,
      NULL, N_("Enable warp mode") },
    { "+warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t) 0,
      NULL, N_("Disable warp mode") },
    { NULL }
};

int vsync_init_cmdline_options(void)
{
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
static unsigned long now;
static unsigned long display_start;
static long frame_usec;

static int timer_speed = 0;
static int speed_eval_suspended = 1;
static CLOCK speed_eval_prev_clk;

/* Initialize vsync timers and set relative speed of emulation in percent. */
static int set_timer_speed(int speed)
{
    speed_eval_suspended = 1;

    if (speed > 0 && refresh_frequency > 0) {
        timer_speed = speed;
        frame_usec = 1000000/refresh_frequency*100/speed;
    }
    else {
        timer_speed = 0;
        frame_usec = 0;
    }

    return 0;
}

/* Display speed (percentage) and frame rate (frames per second). */
static void display_speed(int num_frames)
{
    CLOCK diff_clk;
    double diff_sec;
    double speed_index;
    double frame_rate;

    diff_clk = clk - speed_eval_prev_clk;
    diff_sec = (now - display_start)/1000000.0;

    frame_rate = num_frames/diff_sec;
    speed_index = 100.0*diff_clk/(cycles_per_sec*diff_sec);
    vsyncarch_display_speed(speed_index, frame_rate, warp_mode_enabled);

    speed_eval_prev_clk = clk;
}

static void clk_overflow_callback(CLOCK amount, void *data)
{
    speed_eval_prev_clk -= amount;
}

/* ------------------------------------------------------------------------- */

void vsync_set_machine_parameter(double refresh_rate, long cycles)
{
    refresh_frequency = refresh_rate;
    cycles_per_sec = cycles;
    set_timer_speed(relative_speed);
}

void vsync_init(void (*hook)(void))
{
    vsync_hook = hook;
    suspend_speed_eval();
    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);

    vsyncarch_init();
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
void suspend_speed_eval(void)
{
    sound_suspend();
    speed_eval_suspended = 1;
}

/* This is called at the end of each screen frame. It flushes the
   audio buffer and keeps control of the emulation speed. */
int do_vsync(int been_skipped)
{
    static unsigned long next_frame_start = 0;

    /*
     * this are the counters to show how many frames are skipped
     * since the last vsync_display_speed
     */
    static int frame_counter  = 0;
    static int skipped_frames = 0;

    /*
     * This are the frames which are skipped in a row
     */
    static int skipped_redraw = 0;

    int frame_delay;
    int skip_next_frame;

    signed long delay;

    /*
     * process everything wich should be done before the syncronisation
     * e.g. OS/2: exit the programm if trigger_shutdown set
     */
    vsyncarch_presync();

    if (been_skipped) {
        skipped_frames++;
    }

    /*
     * Get current time in units of microseconds [1e-6 s]
     */
    now = vsyncarch_gettime();

    /* Start afresh after pause in frame output. */
    if (speed_eval_suspended) {
        speed_eval_prev_clk = clk;
        display_start = now;
        next_frame_start = now;
        skipped_redraw = 0;
        skipped_frames = 0;
        frame_counter = 0;
        speed_eval_suspended = 0;
    }

    /* This is the start time between the start of the next frame
     * and now.
     */
    delay = (long)(now - next_frame_start);

    /*
     * We sleep until the start of the next frame, if:
     *  - warp_mode is enabled
     *  - a limiting speed is given
     *  - we have not reached next_frame_start yet
     *
     * We have to sleep even if no frame
     * is output because of sound synchronization
     *
     * FIXME: Sound synchronization.
     */
    if (!warp_mode_enabled && timer_speed && delay < 0)
    {
        vsyncarch_sleep(-delay);
    }

    /*
     * Check whether we should skip the next frame or not.
     * Allow delay of up to one frame before skipping frames.
     * Frames are skipped:
     *  - only if maximum skipped frames are not reached
     *  - if warp_mode enabled
     *  - if speed is not limited or we are too slow and
     *    refresh rate is automatic or fixed and needs correction
     *
     * Remark: The time_deviation should be the equivalent of two
     *         frames and must be scaled to make sure, that we
     *         don't start skipping frames before the CPU reaches 100%.
     *         If we are becoming faster a small deviation because of
     *         threading results in a frame rate correction suddenly.
     */
    if (skipped_redraw < MAX_SKIPPED_FRAMES
        && (warp_mode_enabled
            || (skipped_redraw < refresh_rate - 1)
            || ((!timer_speed || delay > 2*frame_usec*timer_speed/100)
                && !refresh_rate
               )
           )
       )
    {
        skip_next_frame = 1;
        skipped_redraw++;
    }
    else {
        skip_next_frame = 0;
        skipped_redraw = 0;
    }

    /*
     * This point in the code is reached at a more or less constant time
     * frequency; this is necessary for the synchronization in sound_flush.
     */
    frame_delay = -sound_flush(warp_mode_enabled ? 0 : relative_speed);

    /*
     * Check whether the hardware can keep up.
     * Allow up to 3 seconds lack.
     * This should (in both cases) set next_frame_start to
     * a value which represents the start the next frame
     */
    if ((signed long)(now - next_frame_start) > 3000000) {
#ifndef __OS2__
        if (!warp_mode_enabled) {
            log_warning(LOG_DEFAULT, _("Your machine is too slow for current settings!"));
        }
#endif
        next_frame_start = now;
    }
    else {
        next_frame_start += frame_usec*frame_delay;
    }
    next_frame_start += frame_usec;

    /*
     * Update display every two second (pc system time)
     * This has some reasons:
     *  - we have a better statistic in case of a fastly running emulator
     *  - we don't slow down fast emulations by updating this value
     *    too often (eg more then 10 times a second)
     *  - I don't want to have a value jumping around for example
     *    between 99% and 101% if the user chooses 100% (s.above)
     *  - We need some statistict to get an avarage number for the
     *    frame-rate without staticstics it would also jump around
     */
    if ((signed long)(now-display_start) > 2000000) {
        display_speed(frame_counter - skipped_frames);
        display_start  = now;
        frame_counter  = 0;
        skipped_frames = 0;
    }
    else
        frame_counter++;

    /* Run vsync jobs. */
    vsync_hook();

    vsyncarch_postsync();

    return skip_next_frame;
}

#endif
