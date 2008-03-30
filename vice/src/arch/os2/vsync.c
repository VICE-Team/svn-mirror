/*
 * vsync.c - End-of-frame handling for Unix.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *  Thomas Bretz (tbretz@gsi.de)
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
#define INCL_DOSPROFILE
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include "vice.h"

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __IBMC__
#include <sys/timeb.h>
#endif

#include "archdep.h"
#include "clkguard.h"
#include "cmdline.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "log.h"
#include "maincpu.h"
#include "resources.h"
#include "sound.h"
#include "ui.h"
#include "ui_status.h"
#include "vsync.h"

#include "usleep.h"

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#ifndef SA_RESTART
#define SA_RESTART 0
#endif


void vlog(char *s, int i)
{
    FILE *fl;
    fl=fopen("output","a");
    fprintf(fl,"%s %i\n",s,i);
    fclose(fl);
}

/* ------------------------------------------------------------------------- */

/* Relative speed of the emulation (%).  0 means "don't limit speed".  */
static int relative_speed;

/* Refresh rate.  0 means "auto".  */
static int refresh_rate;

/* "Warp mode".  If nonzero, attempt to run as fast as possible.  */
int warp_mode_enabled;

/* FIXME: This should call `set_timers'.  */
static int set_relative_speed(resource_value_t v)
  {
    relative_speed = (int) v;
    return 0;
  }

static int set_refresh_rate(resource_value_t v)
  {
    if ((int) v < 0) return -1;
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
#define MAX_SKIPPED_FRAMES	10

/* Number of frames per second on the real machine.  */
static double refresh_frequency;

/* Number of clock cycles per seconds on the real machine.  */
static long cycles_per_sec;

/* Function to call at the end of every screen frame.  */
static void (*vsync_hook)(void);

/* ------------------------------------------------------------------------- */

static volatile int elapsed_frames = 0;
static int timer_disabled = 1;
static int timer_speed    = 0;
static int timer_patch    = 0;

typedef ULONG _time;
static  ULONG ulTmrFreq;  // Hertz (almost 1.2MHz at my PC)

ULONG gettime()
{
    QWORD qwTmrTime; // now
    DosTmrQueryTime(&qwTmrTime);
    return qwTmrTime.ulLo;
}

static _time timer_ticks;
static _time timer_time;

static void update_elapsed_frames(int want)
{
    static int pending;
    _time now;

    if (timer_disabled) return;
    if (!want && timer_patch > 0) {
        timer_patch--;
        elapsed_frames++;
    }
    now=gettime();
    while (now>timer_time) {
        if (pending) pending--;
        else {
            if (timer_patch < 0) timer_patch++;
            else elapsed_frames++;
        }
        timer_time += timer_ticks;
    }
    if (want == 1 && !pending) {
        if (timer_patch < 0) timer_patch++;
        else elapsed_frames++;
        pending++;
    }
}

static int set_timer_speed(int speed)
{
    if (speed > 0) {
        timer_time=gettime();
        timer_ticks = ((100*ulTmrFreq)/(refresh_frequency*speed));
        update_elapsed_frames(-1);
        elapsed_frames = 0;
    }
    else {
        speed       = 0;
        timer_ticks = 0;
    }

    timer_speed    = speed;
    timer_disabled = speed ? 0 : 1;

    return 0;
}

static void timer_sleep(void)
{
    int	old;

    if (timer_disabled) return;
    old = elapsed_frames;
    do {
        update_elapsed_frames(1);
        if (old == elapsed_frames) DosSleep(1);
    } while (old == elapsed_frames);
}

static void patch_timer(int patch)
{
    timer_patch += patch;
}

int vsync_disable_timer(void)
{
    if (!timer_disabled) return set_timer_speed(0);
    else return 0;
}

/* ------------------------------------------------------------------------- */

static int speed_eval_suspended = 1;

/* This should be called whenever something that has nothing to do with the
   emulation happens, so that we don't display bogus speed values. */
void suspend_speed_eval(void)
{
    sound_suspend();
    speed_eval_suspended = 1;
}

static void clk_overflow_callback(CLOCK amount, void *data)
{
    speed_eval_prev_clk -= amount;
}

void vsync_set_machine_parameter(double refresh_rate, long cycles)
{
    refresh_frequency = refresh_rate;
    cycles_per_sec = cycles;
}

void vsync_init(void (*hook)(void))
{
    vsync_hook = hook;

    suspend_speed_eval();
    vsync_disable_timer();
    DosTmrQueryFreq(&ulTmrFreq);

    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);
}

static CLOCK speed_eval_prev_clk;

static void display_speed(int num_frames)
{
    static _time vice_secs=0;
    static _time prev_time;
    if (!speed_eval_suspended) {
        _time curr_time= gettime();
        float diff_clk    = clk - speed_eval_prev_clk;
        float time_diff   = (double)(curr_time - prev_time)/ulTmrFreq;
	float speed_index = diff_clk/(time_diff*cycles_per_sec);
	float frame_rate  = num_frames/time_diff;

        ui_display_speed(speed_index*100, frame_rate, vice_secs++);

        prev_time            = curr_time;
    }
    else prev_time=gettime();
    speed_eval_prev_clk  = clk;
    speed_eval_suspended = 0;
}

/* ------------------------------------------------------------------------- */

/* This is called at the end of each screen frame.  It flushes the audio buffer
   and keeps control of the emulation speed.  */
int do_vsync(int been_skipped)
{
    static unsigned short frame_counter = USHRT_MAX;
    static unsigned short num_skipped_frames;
    static int skip_counter;
    int skip_next_frame = 0;

    vsync_hook();

    if (been_skipped) num_skipped_frames++;

    if (timer_speed != relative_speed) {
	frame_counter = USHRT_MAX;
        if (set_timer_speed(relative_speed) < 0) {
	    log_error(LOG_DEFAULT, "Trouble setting timers... giving up.");
            /* FIXME: Hm, maybe we should be smarter.  But this is should
               never happen.*/
	    exit(-1);
	}
    }

    if (warp_mode_enabled) {
        /* "Warp Mode".  Just skip as many frames as possible and do not
           limit the maximum speed at all.  */
        if (skip_counter < MAX_SKIPPED_FRAMES) {
            skip_next_frame = 1;
            skip_counter++;
        }
        else skip_counter = elapsed_frames = 0;
        sound_flush(0);
    }
    else
        if (refresh_rate != 0) {
            update_elapsed_frames(0); /* Fixed refresh rate.  */
            if (timer_speed && skip_counter >= elapsed_frames) timer_sleep();
            if (skip_counter < refresh_rate - 1) {
                skip_next_frame = 1;
                skip_counter++;
            }
            else skip_counter = elapsed_frames = 0;
            patch_timer(sound_flush(relative_speed));
        }
        else
        {
            /* Dynamically adjusted refresh rate.  */
            update_elapsed_frames(0);
            if (skip_counter >= elapsed_frames) {
                elapsed_frames = -1;
                timer_sleep();
                skip_counter = 0;
            }
            else
                if (skip_counter < MAX_SKIPPED_FRAMES) {
                    skip_next_frame = 1;
                    skip_counter++;
                }
                else skip_counter = elapsed_frames = 0;
            patch_timer(sound_flush(relative_speed));
        }

    if (frame_counter >= refresh_frequency * 2) {
        display_speed(frame_counter + 1 - num_skipped_frames);
	num_skipped_frames = 0;
	frame_counter = 0;
    }
    else frame_counter++;

    kbd_buf_flush();

#ifdef HAS_JOYSTICK
    joystick_update();
#endif

    return skip_next_frame;
}
