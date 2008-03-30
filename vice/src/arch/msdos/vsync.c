/*
 * vsync.c - End-of-frame handling for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <stdlib.h>
#include <time.h>
#include <pc.h>
#include <dpmi.h>
#include <go32.h>

#include <allegro.h>
#undef EOF			/* Workaround for Allegro bug. */

#include "vsync.h"
#include "resources.h"
#include "vmachine.h"
#include "interrupt.h"
#include "kbd.h"
#include "ui.h"
#include "true1541.h"
#include "sid.h"
#include "vmidas.h"
#include "joystick.h"
#include "autostart.h"
#include "kbdbuf.h"

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

/* If this is #defined, the emulator always runs at full speed. */
#undef NO_SYNC

/* If this is disabled, we reprogram the timer the hard way, thus making
   the internal clock go crazy. (useful under DOSemu, where the MIDAS timer
   might not work) */
#define USE_MIDAS_TIMER

/* Maximum number of frames we can skip consecutively. */
#define MAX_NUM_SKIPPED_FRAMES	10

/* ------------------------------------------------------------------------- */

/* Speed evaluation. */

static CLOCK speed_eval_prev_clk;

static double avg_speed_index;
static double avg_frame_rate;

static int speed_eval_suspended = 1;

void suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

static void calc_avg_performance(int num_frames)
{
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
	avg_speed_index = speed_index;
	avg_frame_rate = frame_rate;
    }
    prev_time = curr_time;
    speed_eval_prev_clk = clk;
    speed_eval_suspended = 0;
}

double vsync_get_avg_frame_rate(void)
{
    if (speed_eval_suspended)
	return -1.0;
    else
	return avg_frame_rate;
}

double vsync_get_avg_speed_index(void)
{
    if (speed_eval_suspended)
	return -1.0;
    else
	return avg_speed_index;
}

/* ------------------------------------------------------------------------- */

/* This prevents the clock counters from overflowing. */
inline static void vsync_prevent_clk_overflow()
{
    if (maincpu_prevent_clk_overflow()) {
#ifdef CBM64
	vic_ii_prevent_clk_overflow();
	cia1_prevent_clk_overflow();
	cia2_prevent_clk_overflow();
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
#else
#error
#endif
	speed_eval_prev_clk -= PREVENT_CLK_OVERFLOW_SUB;
    }

#ifdef HAVE_TRUE1541
    true1541_prevent_clk_overflow();
#endif
}

#ifdef HAVE_TRUE1541
/* This actually updates the LED status only if the `trap idle' idling method
   is being used, as the LED status could be incorrect otherwise. */
static void do_drive(void)
{
    static int old_status = -1;
    int my_status;

    if (app_resources.true1541
	&& app_resources.true1541IdleMethod == TRUE1541_IDLE_TRAP_IDLE) {
	true1541_cpu_execute();
	my_status = true1541_led_status;
    } else
	my_status = 0;

    if (my_status != old_status) {
	kbd_led_set(my_status);
	old_status = my_status;
    }
}
#endif

/* ------------------------------------------------------------------------- */

#if defined(USE_MIDAS_TIMER) && !defined(NO_SYNC)

static volatile int elapsed_frames = 0;
static int timer_speed = -1;

static void MIDAS_CALL my_timer_callback(void)
{
    elapsed_frames++;
}
END_OF_FUNCTION(my_timer_callback)

inline static void set_timer_speed(void)
{
    int new_timer_speed;

    /* Force 100% speed if using automatic refresh rate and there is no speed
       limit, or sound playback is turned on. */
    if (app_resources.sound
	|| (app_resources.speed == 0 && app_resources.refreshRate == 0))
	new_timer_speed = 100;
    else
	new_timer_speed = app_resources.speed;

    if (new_timer_speed == timer_speed)
	return;

    timer_speed = new_timer_speed;
    if (timer_speed == 0) {
	if (!vmidas_remove_timer_callbacks())
	    fprintf(stderr, "%s: Warning: Could not remove timer callbacks.\n",
		    __FUNCTION__);
    } else {
	DWORD rate = (RFSH_PER_SEC * 1000 * timer_speed) / 100;

	/* printf("%s: setting MIDAS timer at %d\n", __FUNCTION__, rate); */
	if (vmidas_set_timer_callbacks(rate, FALSE,
				       my_timer_callback, NULL, NULL) < 0) {
	    fprintf(stderr, "%s: cannot set timer callback at %.2f Hz\n",
		    __FUNCTION__, (double)rate / 1000.0);
	    /* FIXME: is this necessary? */
	    vmidas_remove_timer_callbacks();
	    app_resources.speed = timer_speed = 0;
	}
    }
}

int do_vsync(int been_skipped)
{
    extern int _escape_requested;
    static long skip_counter = 0;
    static int num_skipped_frames = 0;
    static int frame_counter = 0;
    int skip_next_frame = 0;

#ifdef HAVE_TRUE1541
    do_drive();
#endif

    set_timer_speed();

    if (been_skipped)
	num_skipped_frames++;

    if (app_resources.refreshRate != 0) {

	/* Fixed refresh rate. */

	if (timer_speed != 0 && skip_counter >= elapsed_frames)
	    while (skip_counter >= elapsed_frames)
		/* Sleep... */;
	if (skip_counter < app_resources.refreshRate - 1) {
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

	/* Automatic refresh rate adjustment. */

	if (timer_speed && skip_counter >= elapsed_frames) {
	    while (skip_counter >= elapsed_frames)
		/* Sleep... */ ;
	    elapsed_frames = 0;
	    skip_counter = 0;
	} else {
	    if (skip_counter < MAX_NUM_SKIPPED_FRAMES) {
		skip_counter++;
		skip_next_frame = 1;
	    } else {
		/* Give up, we are too slow. */
		skip_next_frame = 0;
		skip_counter = 0;
		elapsed_frames = 0;
	    }
	}

#ifdef SOUND
	if (app_resources.sound)
	    flush_sound();
#endif

    }

    if (frame_counter >= RFSH_PER_SEC * 2) {
	calc_avg_performance(frame_counter + 1 - num_skipped_frames);
	num_skipped_frames = 0;
	frame_counter = 0;
    } else
	frame_counter++;

    kbd_flush_commands();

    if (_escape_requested) {
	_escape_requested = 0;
	maincpu_trigger_trap(UiMain);
    }

    vsync_prevent_clk_overflow();

#if defined(CBM64) || defined(VIC20)
    joystick_update();
#endif

#ifdef AUTOSTART
    autostart_advance();
#endif

    kbd_buf_flush();

    return skip_next_frame;
}

#else  /* !USE_MIDAS_TIMER || NO_VSYNC */

int do_vsync(int been_skipped)
{
    extern int _escape_requested;
    static long skip_counter = 0;
    static long old_rawclock = 0;
    int skip_next_frame = 0;
    int curr_rawclock = rawclock();

#ifdef HAVE_TRUE1541
    do_drive();
#endif

#ifndef NO_SYNC
    /* Here we always do automatic refresh rate adjustment. */
    if (skip_counter >= (curr_rawclock - old_rawclock)) {
    	while (skip_counter >= (rawclock() - old_rawclock))
	    ;
	skip_counter = 0;
	old_rawclock = rawclock();
    } else {
	if (skip_counter < 10) {
	    skip_counter++;
	    skip_next_frame = 1;
	} else {
	    /* Give up, we are too slow. */
	    skip_next_frame = 0;
	    skip_counter = 0;
	    old_rawclock = curr_rawclock;
	}
    }
#else  /* NO_SYNC */
    skip_next_frame = 0;
#endif

    if (_escape_requested) {
	_escape_requested = 0;
	maincpu_trigger_trap(UiMain);
    }

    vsync_prevent_clk_overflow();

#if defined(CBM64) || defined(VIC20)
    joystick_update();
#endif

#ifdef AUTOSTART
    autostart_advance();
#endif

    return skip_next_frame;
}

#endif /* USE_MIDAS_TIMER && !NO_VSYNC */

/* -------------------------------------------------------------------------- */

void vsync_init(void)
{
#ifndef NO_SYNC
#if defined(USE_MIDAS_TIMER)
    LOCK_VARIABLE(elapsed_frames);
    LOCK_FUNCTION(my_timer_callback);
    vmidas_startup();
    vmidas_init();
#else
    outportb(0x40, (BYTE)(0x1234dd / RFSH_PER_SEC));
    outportb(0x40, (BYTE)((0x1234dd / RFSH_PER_SEC) >> 8));
#endif
#endif

    /* FIXME: This should not be here, but in `main()'... */
    joystick_init();
}

