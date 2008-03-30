/*
 * vsync.c - End-of-frame handling on RISC OS.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include "wimp.h"
#include "machine.h"
#include "types.h"
#include "vsync.h"
#include "vsyncarch.h"
#include "resources.h"
#include "videoarch.h"
#include "sound.h"
#include "ui.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "joy.h"
#include "maincpu.h"




/* Exported variables */
int EmuWindowHasInputFocus = 0;


#define MAX_SKIPPED_FRAMES	10

static int NumberOfRefreshes = 0;
static int LastPoll;
static int LastSpeed;
static int RelativeSpeed = 100;
static int CurrentSpeedLimit;
static int PollEvery;
static int SpeedEvery;
static int LastFrame;
static int MaxSkippedFrames;
static int NumberOfFrames = 0;
static int LastSpeedLimit;
static int FramesPerSecond = 50;

static int speed_eval_suspended = 1;

static double refresh_frequency;

static long cycles_per_sec;

/* Function to call at end of every frame */
static void (*vsync_hook)(void);

static int refresh_rate;

static int warp_mode_enabled;



static int set_refresh_rate(resource_value_t v, void *param)
{
  if ((int)v < 0)
    return -1;
  refresh_rate = (int)v;
  return 0;
}


static int set_warp_mode(resource_value_t v, void *param)
{
  warp_mode_enabled = (int)v;
  sound_set_warp_mode(warp_mode_enabled);
  return 0;
}

static int set_speed_limit(resource_value_t v, void *param)
{
  CurrentSpeedLimit = (int)v;
  sound_set_relative_speed(CurrentSpeedLimit);

  return 0;
}

static int set_poll_every(resource_value_t v, void *param)
{
  PollEvery = (int)v;
  return 0;
}

static int set_speed_every(resource_value_t v, void *param)
{
  SpeedEvery = (int)v;
  return 0;
}

static int set_max_skipped_frames(resource_value_t v, void *param)
{
  MaxSkippedFrames = (int)v;
  return 0;
}



void vsync_suspend_speed_eval(void)
{
  sound_suspend();
  speed_eval_suspended = 1;
}

void vsync_sync_reset(void)
{
  speed_eval_suspended = 1;
}


static const resource_t resources[] = {
  {"RefreshRate", RES_INTEGER, (resource_value_t)0,
    (void *)&refresh_rate, set_refresh_rate, NULL },
  {"WarpMode", RES_INTEGER, (resource_value_t)0,
    (void *)&warp_mode_enabled, set_warp_mode, NULL },
  {"PollEvery", RES_INTEGER, (resource_value_t)20,
    (void *)&PollEvery, set_poll_every, NULL },
  {"SpeedEvery", RES_INTEGER, (resource_value_t)100,
    (void *)&SpeedEvery, set_speed_every, NULL },
  {"Speed", RES_INTEGER, (resource_value_t)100,
    (void *)&CurrentSpeedLimit, set_speed_limit, NULL },
  {"MaxSkippedFrames", RES_INTEGER, (resource_value_t)MAX_SKIPPED_FRAMES,
    (void *)&MaxSkippedFrames, set_max_skipped_frames, NULL },
  {NULL}
};


int vsync_resources_init(void)
{
  return resources_register(resources);
}


int vsync_cmdline_options_init(void)
{
  return 0;
}


void vsync_set_machine_parameter(double refresh_rate, long cycles)
{
    refresh_frequency = refresh_rate;
    cycles_per_sec = cycles;
}


int vsync_resync_speed(void)
{
  LastSpeed = OS_ReadMonotonicTime();
  NumberOfFrames = 0; NumberOfRefreshes = 0;
  return LastSpeed;
}


int vsync_resync_poll(void)
{
  LastPoll = OS_ReadMonotonicTime();
  return LastPoll;
}


void vsync_init(void (*hook)(void))
{
  LastPoll = vsync_resync_speed();
  LastFrame = LastPoll;

  LastSpeedLimit = CurrentSpeedLimit;

  vsync_hook = hook;
}


static unsigned short frame_counter = 0xffff;
static unsigned short num_skipped_frames = 0;
static int skip_counter=0;

#define VSYNC_TIME_DELTA(t,n) \
  ((10000*(n)) - FramesPerSecond*CurrentSpeedLimit*((t)-LastSpeed))

int vsync_do_vsync(struct video_canvas_s *canvas, int been_skipped)
{
  int skip_next_frame = 0;
  int frame_delay;
  int now, dopoll;

  /* this can happen in some rare cases; make sure emulation stops then */
  while (EmuPaused != 0) ui_poll(1);

  if (FullScreenMode != 0)
  {
    EmuWindowHasInputFocus = 1;
  }
  else
  {
    RO_Caret caret;

    Wimp_GetCaretPosition(&caret);
    /* This will be used by keyboard and joystick code */
    EmuWindowHasInputFocus = (canvas_for_handle(caret.WHandle) == NULL) ? 0 : 1;
  }

  now = OS_ReadMonotonicTime();

  if (warp_mode_enabled) {
    if (skip_counter < MaxSkippedFrames) {
      skip_next_frame = 1;
      skip_counter++;
    } else {
      skip_counter = 0;
    }
  } else if (refresh_rate != 0) {
    if (skip_counter < refresh_rate - 1) {
      skip_next_frame = 1;
      skip_counter++;
    } else {
      skip_counter = 0;
    }
  } else {
    /* Use NumberOfFrames+1 because this frame isn't counted yet */
    if ((VSYNC_TIME_DELTA(now, NumberOfFrames+1) < 0) && (skip_counter < MaxSkippedFrames)) {
      skip_next_frame = 1;
      skip_counter++;
    } else {
      skip_counter = 0;
    }
  }

  /* always pass the actual speed unless in reSID mode */
  /*frame_delay = sound_flush((CycleBasedSound == 0) ? RelativeSpeed : CurrentSpeedLimit);*/
  frame_delay = sound_flush(RelativeSpeed);

  if (frame_counter >= refresh_frequency * 2) {
    num_skipped_frames = 0;
    frame_counter = 0;
  } else {
    frame_counter++;
  }

  if (skip_next_frame == 0) NumberOfRefreshes++;

  vsync_hook();

  joystick();

  NumberOfFrames += 1 - frame_delay;

  /* Speed limiter? Wait */
  if ((CurrentSpeedLimit != 0) && (warp_mode_enabled == 0))
  {
    while (VSYNC_TIME_DELTA(now, NumberOfFrames) > 0)
      now = OS_ReadMonotonicTime();
  }
  LastFrame = now;

  if (speed_eval_suspended)
  {
    vsync_resync_speed();
    speed_eval_suspended = 0;
  }
  else
  {
    if ((now - LastSpeed) >= SpeedEvery)
    {
      resource_value_t val;

      RelativeSpeed = (10000 * NumberOfFrames) / (FramesPerSecond * (now - LastSpeed));
      ui_display_speed(RelativeSpeed, (100 * NumberOfRefreshes) / (now - LastSpeed), 0);
      LastSpeed = now; NumberOfFrames = 0; NumberOfRefreshes = 0;
      resources_get_value("MachineVideoStandard", (void *)&val);
      FramesPerSecond = ((int)val == MACHINE_SYNC_PAL) ? 50 : 60;
    }
  }

  dopoll = 0;
  if ((now - LastPoll) >= PollEvery)
  {
    dopoll = 1;
    LastPoll = now;
  }

  ui_poll(dopoll);

  video_full_screen_plot_status();

  kbd_buf_flush();

  return skip_next_frame;
}


int vsync_disable_timer(void)
{
  return 0;
}

