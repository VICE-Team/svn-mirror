/*
 * vsync.c - End-of-frame handling for Unix.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#ifdef __hpux
#define _INCLUDE_HPUX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#define _INCLUDE_XOPEN_SOURCE
#endif

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#ifdef __hpux
#define _INCLUDE_XOPEN_SOURCE
#define _XPG2
#include <limits.h>
#undef  _INCLUDE_XOPEN_SOURCE
#undef  _XPG2
#else
#include <limits.h>
#endif

#include "vsync.h"
#include "vmachine.h"
#include "ui.h"
#include "interrupt.h"
#include "sid.h"
#include "video.h"
#include "joystick.h"
#include "autostart.h"

#ifdef HAVE_TRUE1541
#include "true1541.h"
#endif

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#if defined(VIC20)
#include "vic.h"
#include "via.h"
#elif defined(PET)
#include "crtc.h"
#include "via.h"
#elif defined(CBM64)
#include "vicii.h"
#include "cia.h"
#endif

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

/* Maximum number of frames we can skip consecutively when adjusting the
   refresh rate dynamically. */
#define MAX_SKIPPED_FRAMES	10

/* ------------------------------------------------------------------------- */

static volatile int elapsed_frames = 0;
static int timer_disabled = 1;
static int timer_speed = 0;

static int timer_ticks;
static struct timeval timer_time;

static void update_elapsed_frames(int want)
{
    struct timeval now;
    static int pending;

    if (timer_disabled)
        return;
    gettimeofday(&now, NULL);
    while (now.tv_sec > timer_time.tv_sec ||
	   (now.tv_sec == timer_time.tv_sec &&
	    now.tv_usec > timer_time.tv_usec)) {
	if (pending)
	    pending--;
	else
	    elapsed_frames++;
	timer_time.tv_usec += timer_ticks;
	while (timer_time.tv_usec >= 1000000) {
	    timer_time.tv_usec -= 1000000;
	    timer_time.tv_sec++;
	}
    }
    if (want == 1 && !pending) {
	elapsed_frames++;
	pending++;
    }
}

static int set_timer_speed(int speed)
{
    if (speed > 0) {
	gettimeofday(&timer_time, NULL);
	timer_ticks = ((100.0 / RFSH_PER_SEC)  * 1000000) / speed;
	update_elapsed_frames(-1);
	elapsed_frames = 0;
    } else {
	speed = 0;
	timer_ticks = 0;
    }

    timer_speed = speed;
    timer_disabled = speed ? 0 : 1;

    return 0;
}

static void timer_sleep(void)
{
    int	old;

    if (!timer_disabled) {
        old = elapsed_frames;
	while (old == elapsed_frames) {
	    update_elapsed_frames(1);
	    if (old == elapsed_frames)
		usleep(1);
	}
    }
}

int vsync_disable_timer(void)
{
    if (!timer_disabled)
	return set_timer_speed(0);
    else
	return 0;
}

/* ------------------------------------------------------------------------- */

static int speed_eval_suspended = 1;

/* This should be called whenever something that has nothing to do with the
   emulation happens, so that we don't display bogus speed values. */
void suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

void vsync_init(void)
{
    suspend_speed_eval();
    vsync_disable_timer();
}

static CLOCK speed_eval_prev_clk;

static void display_speed(int num_frames)
{
#ifdef HAVE_GETTIMEOFDAY
    static double prev_time;
    struct timeval tv;
    double curr_time;

    gettimeofday(&tv, NULL);
    curr_time = (double)tv.tv_sec + ((double)tv.tv_usec) / 1000000.0;
    if (!speed_eval_suspended) {
	CLOCK diff_clk;
	double speed_index;
	double frame_rate;

	diff_clk = clk - speed_eval_prev_clk;
	frame_rate = (double)num_frames / (curr_time - prev_time);
	speed_index = ((((double)diff_clk / (curr_time - prev_time))
			/ (double)CYCLES_PER_SEC)) * 100.0;
	UiDisplaySpeed((float)speed_index, (float)frame_rate);
    }
    prev_time = curr_time;
    speed_eval_prev_clk = clk;
    speed_eval_suspended = 0;
#else
    return;
#endif
}

/* This prevents the clock counters from overflowing. */
inline static void vsync_prevent_clk_overflow()
{
    if (maincpu_prevent_clk_overflow()) {
#ifdef CBM64
	vic_ii_prevent_clk_overflow();
	cia1_prevent_clk_overflow();
	cia2_prevent_clk_overflow();
#ifdef SOUND
	sid_prevent_clk_overflow();
#endif
#elif defined(VIC20)
	vic_prevent_clk_overflow();
	via1_prevent_clk_overflow();
	via2_prevent_clk_overflow();
#elif defined(PET)
	crtc_prevent_clk_overflow();
	via_prevent_clk_overflow();
#elif defined(C128)
	vic_ii_prevent_clk_overflow();
	/* vdc_prevent_clk_overflow(); */
	cia1_prevent_clk_overflow();
	cia2_prevent_clk_overflow();
#ifdef SOUND
	sid_prevent_clk_overflow();
#endif
#else  /* !SOUND */
#error
#endif /* !SOUND */

	speed_eval_prev_clk -= PREVENT_CLK_OVERFLOW_SUB;
	/* printf("PREVENT CLK OVERFLOW!\n"); */
    }

#ifdef HAVE_TRUE1541
    true1541_prevent_clk_overflow();
#endif
}

#ifdef HAVE_TRUE1541
/* Actually update the LED status only if the `trap idle' idling method is
   being used, as the LED status could be incorrect otherwise. */
static void update_drive_led(void)
{
    static int old_status = -1;
    int my_status;

    if (app_resources.true1541IdleMethod == TRUE1541_IDLE_TRAP_IDLE)
	my_status = true1541_led_status;
    else
	my_status = 0;

    if (my_status != old_status) {
#if 0
	/* FIXME: this does not seem to work with all X11 servers.
	   It does not work with XFree86 3.3, but some people reported that
	   it worked with older XFree86 versions.  Does anybody know why? */
	static XKeyboardControl kbd_control;
	kbd_control.led_mode = my_status ? LedModeOn : LedModeOff;
	kbd_control.led = 16;
	XChangeKeyboardControl(display, KBLedMode | KBLed, &kbd_control);
#else
	printf("1541: LED %s\n", my_status ? "On" : "Off");
#endif
	old_status = my_status;
    }
}
#endif

/* ------------------------------------------------------------------------- */

/* This is called at the end of each screen frame.  It flushes the audio buffer
   and keeps control of the emulation speed. */
int do_vsync(int been_skipped)
{
    static unsigned short frame_counter = USHRT_MAX;
    static unsigned short num_skipped_frames;
    static int skip_counter;
    static int refresh_rate = 1;
    int skip_next_frame = 0;

#ifdef HAVE_TRUE1541
    if (app_resources.true1541
	&& app_resources.true1541IdleMethod == TRUE1541_IDLE_TRAP_IDLE) {
	true1541_cpu_execute();
    }
#endif

#ifdef AUTOSTART
    autostart_advance();
#endif

    if (been_skipped)
	num_skipped_frames++;

    if (timer_speed != app_resources.speed) {
	frame_counter = USHRT_MAX;
	if (set_timer_speed(app_resources.speed) < 0) {
	    fprintf(stderr, "Trouble setting timers... giving up.\n");
	    exit(-1);
	}
    }

    UiDispatchEvents();

#if defined(HAS_JOYSTICK) /* && !defined(PET) */
    /* Handle `real' joystick. */
    joystick();
#endif

    if (refresh_rate != 0) {
	/* Fixed refresh rate.*/
	update_elapsed_frames(0);
	if (timer_speed && skip_counter >= elapsed_frames)
	    timer_sleep();
	if (skip_counter < refresh_rate - 1) {
	    skip_next_frame = 1;
	    skip_counter++;
	} else {
	    skip_counter = elapsed_frames = 0;
	}
#ifdef SOUND
	if (app_resources.sound)
	    flush_sound();
#endif
    } else {
	/* Dynamically adjusted refresh rate. */
	update_elapsed_frames(0);
	if (skip_counter >= elapsed_frames) {
	    elapsed_frames = -1;
	    timer_sleep();
	    skip_counter = 0;
	} else {
	    if (skip_counter < MAX_SKIPPED_FRAMES) {
		skip_next_frame = 1;
		skip_counter++;
	    } else {
		skip_counter = elapsed_frames = 0;
	    }
	}
#ifdef SOUND
	if (app_resources.sound)
	    flush_sound();
#endif
    }

    if (frame_counter >= RFSH_PER_SEC * 2) {
	display_speed(frame_counter + 1 - num_skipped_frames);
	num_skipped_frames = 0;
	frame_counter = 0;
    } else
	frame_counter++;

    vsync_prevent_clk_overflow();

#ifdef HAVE_TRUE1541
    update_drive_led();
#endif

    refresh_rate = app_resources.refreshRate;

    return skip_next_frame;
}
