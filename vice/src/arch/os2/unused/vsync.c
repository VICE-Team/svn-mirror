/*
 * vsync.c - End-of-frame handling for Unix.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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
#define INCL_DOSPROFILE
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include "vice.h"

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>

#ifdef __IBMC__
#include <sys/timeb.h>
#endif

#include "vsync.h"
#include "ui.h"
#include "ui_status.h"
#include "interrupt.h"
#include "log.h"
#include "kbdbuf.h"
#include "sound.h"
#include "resources.h"
#include "translate.h"
#include "cmdline.h"
#include "kbd.h"
#include "archdep.h"

#include "usleep.h"

#ifdef HAS_JOYSTICK
#include "joy.h"
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
static int set_relative_speed(int val, void *param)
{
    relative_speed = val;

    return 0;
}

static int set_refresh_rate(int val, void *param)
{
    if (val < 0)
        return -1;

    refresh_rate = val;

    return 0;
}

static int set_warp_mode(resource_value_t v, void *param)
{
    warp_mode_enabled = val;
    sound_set_warp_mode(warp_mode_enabled);

    return 0;
}

/* Vsync-related resources.  */
static const resource_int_t resources_int[] = {
    { "Speed", 100, RES_EVENT_NO, NULL,
      &relative_speed, set_relative_speed, NULL },
    { "RefreshRate", 0, RES_EVENT_STRICT, (resource_value_t)1,
      &refresh_rate, set_refresh_rate, NULL },
    { "WarpMode", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &warp_mode_enabled, set_warp_mode, NULL },
    { NULL }
};

int vsync_init_resources(void)
{
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

/* Vsync-related command-line options.  */
static const cmdline_option_t cmdline_options[] = {
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

#ifdef __IBMC__
  #define SEC    time      // double
  #define subSEC millitm   // ushort
  typedef struct timeb _time;
  const int MICRONS = 10000; //960;
//  #define gettime(now) _ftime(now)  // DosTmrQueryTime
  void gettime(_time *now)
  {
      ULONG ulTmrFreq; // Hertz
      QWORD qwTmrTime; // now
      DosTmrQueryFreq(&ulTmrFreq);
      DosTmrQueryTime(&qwTmrTime);
      now->time    = (int)(qwTmrTime.ulLo/ulTmrFreq);
      now->millitm = (qwTmrTime.ulLo-(ulTmrFreq*now->time))*MICRONS/ulTmrFreq;
  }
#else
  #define SEC    tv_sec
  #define subSEC tv_usec
  #define gettime(now) gettimeofday(now, NULL)
  typedef struct timeval _time;
  const int MICRONS = 1000000;
#endif

static int timer_ticks;
_time      timer_time;

static void update_elapsed_frames(int want)
{
    static int pending;
    _time now;

    if (timer_disabled) return;
    if (!want && timer_patch > 0) {
        timer_patch--;
        elapsed_frames++;
    }
    gettime(&now);
    while (now.SEC > timer_time.SEC ||
           (now.SEC == timer_time.SEC &&
            now.subSEC > timer_time.subSEC)) {
        if (pending) pending--;
        else {
            if (timer_patch < 0) timer_patch++;
            else elapsed_frames++;
        }
        timer_time.subSEC += timer_ticks;
        timer_time.SEC    += (int)(timer_time.subSEC/MICRONS);
        timer_time.subSEC %= MICRONS;
        /*	while (timer_time.tv_usec >= 1000000) {
         timer_time.tv_usec -= 1000000;
         timer_time.tv_sec++;
         }*/
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
        gettime(&timer_time);
        timer_ticks = ((100*MICRONS)/(refresh_frequency*speed))/**1000000*/;///speed;
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
void vsync_suspend_speed_eval(void)
{
    sound_suspend();
    speed_eval_suspended = 1;
}

void vsync_init(double hertz, long cycles, void (*hook)(void))
{
    vsync_hook        = hook;
    refresh_frequency = hertz;
    cycles_per_sec    = cycles;

    vsync_suspend_speed_eval();
    vsync_disable_timer();
}

static CLOCK speed_eval_prev_clk;

static void display_speed(int num_frames)
{
    static long vice_secs=0;
    static double prev_time;
    double curr_time;
    _time tv;
    gettime(&tv);
    curr_time = (double)tv.SEC + (double)tv.subSEC/MICRONS; // casting ???
    if (!speed_eval_suspended) {
        float diff_clk    = clk - speed_eval_prev_clk;
        float time_diff   = curr_time - prev_time;
	float speed_index = diff_clk/(time_diff*cycles_per_sec);
	float frame_rate  = num_frames/time_diff;

        ui_display_speed(speed_index*100, frame_rate, vice_secs++);
    }

    prev_time            = curr_time;
    speed_eval_prev_clk  = clk;
    speed_eval_suspended = 0;
}

void vsync_prevent_clk_overflow(CLOCK sub)
{
    speed_eval_prev_clk -= sub;
}

/* ------------------------------------------------------------------------- */

/* This is called at the end of each screen frame.  It flushes the audio buffer
   and keeps control of the emulation speed.  */
int vsync_do_vsync(int been_skipped)
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
    } else
        frame_counter++;


    kbdbuf_flush();

#ifdef HAS_JOYSTICK
    joystick_update();
#endif

    return skip_next_frame;
}
