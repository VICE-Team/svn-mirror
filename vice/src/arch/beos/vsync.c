/*
 * vsync.c - Display synchronization for BeOS
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
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

#include <OS.h>
#include <stdio.h>
#include <sys/time.h>

#include "clkguard.h"
#include "cmdline.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "maincpu.h"
#include "mouse.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "ui.h"
#include "joystick.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */

static int set_timer_speed(int speed);

/* Relative speed of the emulation (%).  0 means "don't limit speed".  */
static int relative_speed;

/* Refresh rate.  0 means "auto".  */
static int refresh_rate;

/* "Warp mode".  If nonzero, attempt to run as fast as possible.  */
static int warp_mode_enabled;

static int set_relative_speed(resource_value_t v)
{
    relative_speed = (int) v;
    set_timer_speed(relative_speed);
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
    set_timer_speed(relative_speed);
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

/* ------------------------------------------------------------------------- */

/* static guarantees zero values. */
static struct timeval now;
static struct timeval frame_start;
static struct timeval display_start;
static long frame_usec;

static int timer_speed = 0;
static int speed_eval_suspended = 1;
static CLOCK speed_eval_prev_clk;

/* Initialize vsync timers and set relative speed of emulation in percent. */
static int set_timer_speed(int speed)
{
  speed_eval_suspended = 1;

  if (speed > 0) {
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
  double long diff_sec;
  double speed_index;
  double frame_rate;

  gettimeofday(&now, NULL);

  diff_clk = clk - speed_eval_prev_clk;
  diff_sec = (now.tv_sec - display_start.tv_sec)
    + (now.tv_usec - display_start.tv_usec)/1000000.0;

  frame_rate = num_frames/diff_sec;
  speed_index = 100.0*diff_clk/(cycles_per_sec*diff_sec);
  ui_display_speed((float)speed_index, (float)frame_rate,
		   warp_mode_enabled);

  display_start = now;
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
    vsync_disable_timer();
    suspend_speed_eval();
    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);
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
   audio buffer and keeps control of the emulation speed.  */
int do_vsync(int been_skipped)
{
  static int frame_counter = 0;
  static int skipped_frames = 0;
  static int skipped_redraw = 0;
  long usec;
  int frame_delay;
  int skip_next_frame;
  struct timeval frame_start_cmp;

  if (been_skipped) {
    skipped_frames++;
  }

  /* Get current time. */
  gettimeofday(&now, NULL);

  /* Start afresh after pause in frame output. */
  if (speed_eval_suspended) {
    speed_eval_prev_clk = clk;
    display_start = now;
    frame_start = now;
    skipped_redraw = 0;
    skipped_frames = 0;
    frame_counter = 0;
    speed_eval_suspended = 0;
  }

  /* Allow delay of up to one frame before skipping frames. */
  usec = frame_start.tv_usec + frame_usec;
  frame_start_cmp.tv_sec = frame_start.tv_sec + usec/1000000;
  frame_start_cmp.tv_usec = usec%1000000;
  
  /* Check whether we're on time. */
  if (warp_mode_enabled || !timer_speed
      || timercmp(&now, &frame_start_cmp, >))
  {
    /* Skip next frame if allowed.
       If warp_mode_enabled is set, as many frames are skipped as allowed.
       If refresh_rate is set, frames are only skipped to output frames
       at the specified interval, not to keep up to speed.
    */
    if (skipped_redraw < MAX_SKIPPED_FRAMES
	&& (warp_mode_enabled
	    || !refresh_rate || skipped_redraw < refresh_rate - 1))
    {
      skip_next_frame = 1;
      skipped_redraw++;
    }
    else {
      skip_next_frame = 0;
      skipped_redraw = 0;
    }
  }
  else {
    /* Sleep until start of frame. We have to sleep even if no frame
       is output because of sound synchronization.
       FIXME: Sound synchronization.
    */
    long delay = (frame_start.tv_sec - now.tv_sec)*1000000
      + frame_start.tv_usec - now.tv_usec;
    if (delay > 0) {
      usleep(delay);
    }

    /* Output frames at specified interval. */
    if (refresh_rate && skipped_redraw < refresh_rate - 1
	&& skipped_redraw < MAX_SKIPPED_FRAMES)
    {
      skip_next_frame = 1;
      skipped_redraw++;
    }
    else {
      skip_next_frame = 0;
      skipped_redraw = 0;
    }
  }

  /* This point in the code is reached at a more or less constant time
     frequency; this is necessary for the synchronization in sound_flush.
  */
  frame_delay = -sound_flush(warp_mode_enabled ? 0 : relative_speed);

  /* Set time for next frame. */
  usec = frame_start.tv_usec + frame_usec*(frame_delay + 1);
  frame_start.tv_sec += usec/1000000;
  frame_start.tv_usec = usec%1000000;

  /* Update display every two seconds. */
  if (++frame_counter > refresh_frequency*2) {
    display_speed(frame_counter - skipped_frames);
    frame_counter = 0;
    skipped_frames = 0;
  }

  /* Run vsync jobs. */
  vsync_hook();
  ui_dispatch_events();
  kbd_buf_flush();
#ifdef HAS_JOYSTICK
  joystick_update();
#endif

  return skip_next_frame;
}

