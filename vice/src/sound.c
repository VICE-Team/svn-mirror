/*
 * sound.c - General code for the sound interface.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *
 * Resource and cmdline code by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "clkguard.h"
#include "cmdline.h"
#include "fixpoint.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vsync.h"


/* ------------------------------------------------------------------------- */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* ------------------------------------------------------------------------- */

/* Resource handling -- Added by Ettore 98-04-26.  */

/* FIXME: We need sanity checks!  And do we really need all of these
   `sound_close()' calls?  */

static int playback_enabled;          /* app_resources.sound */
static int sample_rate;               /* app_resources.soundSampleRate */
static char *device_name=NULL;        /* app_resources.soundDeviceName */
static char *device_arg=NULL;         /* app_resources.soundDeviceArg */
static int buffer_size;               /* app_resources.soundBufferSize */
static int suspend_time;              /* app_resources.soundSuspendTime */
static int speed_adjustment_setting;  /* app_resources.soundSpeedAdjustment */
static int oversampling_factor;       /* app_resources.soundOversample */

/* I need this to serialize close_sound and enablesound/initsid in
   the OS/2 Multithreaded environment                              */
static int sdev_open           = FALSE;

/* I need this to serialize close_sound and enablesound/initsid in
   the OS/2 Multithreaded environment                              */
int sound_state_changed;

/* Sample based or cycle based sound engine. */
static int cycle_based;
/* Speed in percent, tracks relative_speed from vsync.c */
static int speed_factor;

static int set_playback_enabled(resource_value_t v, void *param)
{
    if ((int)v) vsync_disable_timer();
    playback_enabled = (int)v;
    return 0;
}

static int set_sample_rate(resource_value_t v, void *param)
{
    sample_rate   = (int) v;
    sound_state_changed = TRUE;
    return 0;
}

static int set_device_name(resource_value_t v, void *param)
{
    string_set(&device_name, (char *) v);
    sound_close();
    return 0;
}

static int set_device_arg(resource_value_t v, void *param)
{
    string_set(&device_arg, (char *) v);
    sound_close();
    return 0;
}

static int set_buffer_size(resource_value_t v, void *param)
{
    buffer_size   = (int)v;
    sound_state_changed = TRUE;
    return 0;
}

static int set_suspend_time(resource_value_t v, void *param)
{
    suspend_time = (int)v;
    if (suspend_time < 0)
        suspend_time = 0;
    sound_close();
    return 0;
}

static int set_speed_adjustment_setting(resource_value_t v, void *param)
{
    speed_adjustment_setting = (int)v;
    sound_close();
    return 0;
}

static int set_oversampling_factor(resource_value_t v, void *param)
{
    oversampling_factor = (int)v;
    if (oversampling_factor < 0 || oversampling_factor > 3) {
        log_warning(LOG_DEFAULT,
                    "Invalid oversampling factor %d.  Forcing 3.",
                    oversampling_factor);
        oversampling_factor = 3;
    }
    sound_state_changed = TRUE;
    return 0;
}

static resource_t resources[] = {
    { "Sound", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &playback_enabled,
      set_playback_enabled, NULL },
    { "SoundSampleRate", RES_INTEGER, (resource_value_t) SOUND_SAMPLE_RATE,
      (resource_value_t *) &sample_rate,
      set_sample_rate, NULL },
    { "SoundDeviceName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &device_name,
      set_device_name, NULL },
    { "SoundDeviceArg", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &device_arg,
      set_device_arg, NULL },
    { "SoundBufferSize", RES_INTEGER,
      (resource_value_t) SOUND_SAMPLE_BUFFER_SIZE,
      (resource_value_t *) &buffer_size,
      set_buffer_size, NULL },
    { "SoundSuspendTime", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &suspend_time,
      set_suspend_time, NULL },
    { "SoundSpeedAdjustment", RES_INTEGER,
      (resource_value_t) SOUND_ADJUST_FLEXIBLE,
      (resource_value_t *) &speed_adjustment_setting,
      set_speed_adjustment_setting, NULL },
    { "SoundOversample", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &oversampling_factor,
      set_oversampling_factor, NULL },
    { NULL }
};

int sound_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Command-line options -- Added by Ettore 98-05-09.  */
static cmdline_option_t cmdline_options[] = {
    { "-sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t) 1,
      NULL, "Enable sound playback" },
    { "+sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t) 0,
      NULL, "Disable sound playback" },
    { "-soundrate", SET_RESOURCE, 1, NULL, NULL, "SoundSampleRate", NULL,
      "<value>", "Set sound sample rate to <value> Hz" },
    { "-soundbufsize", SET_RESOURCE, 1, NULL, NULL, "SoundBufferSize", NULL,
      "<value>", "Set sound buffer size to <value> msec" },
    { "-sounddev", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceName", NULL,
      "<name>", "Specify sound driver" },
    { "-soundarg", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceArg", NULL,
      "<args>", "Specify initialization parameters for sound driver" },
    { "-soundsync", SET_RESOURCE, 1, NULL, NULL, "SoundSpeedAdjustment", NULL,
      "<sync>", "Set sound speed adjustment (0: flexible, 1: adjusting, 2: exact)" },
    { NULL }
};

int sound_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* timing constants */
static unsigned int cycles_per_sec;
static unsigned int cycles_per_rfsh;
static double rfsh_per_sec;

/* Flag: Is warp mode enabled?  */
static int warp_mode_enabled;

#define BUFSIZE 32768
typedef struct
{
    /* sid itself */
    sound_t		*psid;
    /* number of clocks between each sample. used value */
    soundclk_t		 clkstep;
    /* number of clocks between each sample. original value */
    soundclk_t		 origclkstep;
    /* factor between those two clksteps */
    soundclk_t		 clkfactor;
    /* time of last sample generated */
    soundclk_t		 fclk;
    /* time of last write to sid. used for pdev->dump() */
    CLOCK		 wclk;
    /* time of last call to sound_run_sound() */
    CLOCK		 lastclk;
    /* sample buffer */
    SWORD		 buffer[BUFSIZE];
    /* sample buffer pointer */
    int			 bufptr;
    /* pointer to device structure in use */
    sound_device_t	*pdev;
    /* number of samples in a fragment */
    int			 fragsize;
    /* number of fragments in kernel buffer */
    int			 fragnr;
    /* number of samples in kernel buffer */
    int			 bufsize;
    /* return value of first pdev->bufferstatus() call to device */
    int			 firststatus;
    /* constants related to adjusting sound */
    int			 prevused;
    int			 prevfill;
    /* is the device suspended? */
    int			 issuspended;
    SWORD		 lastsample;
    /* nr of samples to oversame / real sample */
    DWORD		 oversamplenr;
    /* number of shift needed on oversampling */
    DWORD		 oversampleshift;
} snddata_t;

static snddata_t snddata;

/* device registration code */
static sound_device_t *sound_devices[32];

int sound_register_device(sound_device_t *pdevice)
{
    int					i;
    for (i = 0; sound_devices[i]; i++);
    sound_devices[i] = pdevice;
    return 0;
}


/* close sid device and show error dialog if needed */
static int closesound(const char *msg)
{
    if (snddata.pdev)
    {
	log_message(LOG_DEFAULT, "SOUND: Closing device `%s'", snddata.pdev->name);
	if (snddata.pdev->close)
	    snddata.pdev->close();
        snddata.pdev = NULL;
    }
    if (snddata.psid)
    {
	sound_machine_close(snddata.psid);
	snddata.psid = NULL;
    }
    if (msg)
    {
        suspend_speed_eval();
	if (msg[0])
	{
	    if (console_mode || vsid_mode)
	        log_message(LOG_DEFAULT, "SOUND: %s", msg);
	    else
	        ui_error(msg);
            playback_enabled = 0;
	    if (!console_mode)
	        ui_update_menus();
	}
    }
    snddata.prevused = snddata.prevfill = 0;
    return 1;
}

/* code to disable sid for a given number of seconds if needed */
static int disabletime;

static void suspendsound(const char *reason)
{
    disabletime = time(0);
    log_warning(LOG_DEFAULT, "SUSPEND: disabling sound for %d secs (%s)",
	 suspend_time, reason);
    closesound("");
}

static void enablesound(void)
{
    int	diff;
    if (!disabletime)
        return;
    diff = time(0) - disabletime;
    if (diff < 0 || diff >= suspend_time)
    {
        disabletime = 0;
    }
}

/* open sound device */
static int initsid(void)
{
    int i;
    sound_device_t *pdev;
    char *name;
    char *param;
    int speed;
    int fragsize;
    int fragnr;
    double bufsize;
    char err[1024];

    if (suspend_time > 0 && disabletime)
        return 1;

    /* Special handling for cycle based as opposed to sample based sound
       engines. reSID is cycle based. */
    resources_get_value("SidUseResid", (resource_value_t*)&cycle_based);
    log_message(LOG_DEFAULT, cycle_based ?
		"SOUND: Cycle based engine" : "SOUND: Sample based engine");

    name = device_name;
    if (name && !strcmp(name, ""))
	name = NULL;
    param = device_arg;
    if (param && !strcmp(param, ""))
	param = NULL;

    bufsize = ((buffer_size<100 || buffer_size>1000)?SOUND_SAMPLE_BUFFER_SIZE:buffer_size)/1000.0;
    speed   = (sample_rate<8000 || sample_rate>50000)?SOUND_SAMPLE_RATE:sample_rate;

        /* calculate optimal fragments */
    fragsize = speed / ((int)rfsh_per_sec);
    for (i = 1; 1 << i < fragsize; i++);
    fragsize = 1 << i;
    fragnr = (int)((speed * bufsize + fragsize - 1) / fragsize);
    if (fragnr < 3)
        fragnr = 3;

    for (i = 0; (pdev = sound_devices[i]); i++)
    {
	if ((name && pdev->name && !strcmp(pdev->name, name)) ||
	    (!name && pdev->name))
	{
	    if (pdev->init)
	    {
		sprintf(err, "SOUND(%s)", pdev->name);
		if (pdev->init(param, &speed, &fragsize, &fragnr, bufsize))
		{
		    sprintf(err, "Audio: initialization failed for device `%s'.",
			    pdev->name);
		    return closesound(err);
		}
	    }
	    snddata.issuspended = 0;
	    snddata.lastsample = 0;
	    snddata.pdev = pdev;
	    snddata.fragsize = fragsize;
	    snddata.fragnr = fragnr;
	    snddata.bufsize = fragsize*fragnr;
	    snddata.bufptr = 0;
	    log_message(LOG_DEFAULT,
		 "SOUND: Opened device `%s' speed %dHz fragsize %.3fs bufsize %.3fs",
		 pdev->name, speed, (double)fragsize / speed,
		 (double)snddata.bufsize / speed);
	    sample_rate = speed;
	    /* Cycle based sound engines must do their own filtering,
	       and handle sample rate conversion. */
	    if (cycle_based) {
	        resources_get_value("Speed", (resource_value_t*)&speed_factor);
		/* "No limit" doesn't make sense for cycle based sound engines,
		   which have a fixed sampling rate. */
		if (speed_factor == 0) {
		    speed_factor = 1;
		}
	        snddata.oversampleshift = 0;
		snddata.oversamplenr = 1;
		snddata.psid = sound_machine_open((int)
						  (speed*100/speed_factor),
						  cycles_per_sec);
	    }
	    /* For sample based sound engines, both simple average filtering
	       and sample rate conversion is handled here. */
	    else {
	        snddata.oversampleshift = oversampling_factor;
		snddata.oversamplenr = 1 << snddata.oversampleshift;
		snddata.psid = sound_machine_open((int)
						  (speed * snddata.oversamplenr),
						  cycles_per_sec);
	    }
            if (!snddata.psid)
            {
                return closesound("Audio: Cannot initialize sound module");
            }
	    if (pdev->bufferstatus)
		snddata.firststatus = pdev->bufferstatus(1);
	    snddata.clkstep = SOUNDCLK_CONSTANT(cycles_per_sec) / speed;
	    if (snddata.oversamplenr > 1)
	    {
		snddata.clkstep /= snddata.oversamplenr;
		log_message(LOG_DEFAULT, "SOUND: Using %dx oversampling",
		     snddata.oversamplenr);
	    }
	    snddata.origclkstep = snddata.clkstep;
	    snddata.clkfactor = SOUNDCLK_CONSTANT(1.0);
	    snddata.fclk = SOUNDCLK_CONSTANT(clk);
	    snddata.wclk = clk;
            snddata.lastclk = clk;

	    /* Set warp mode for non-realtime sound devices in vsid mode. */
	    if (vsid_mode && !pdev->bufferstatus)
	        resources_set_value("WarpMode", (resource_value_t)1);

	    return 0;
	}
    }
    sprintf(err, "Audio: device '%s' not found or not supported.", name);
    return closesound(err);
}

/* run sid */
static int sound_run_sound(void)
{
    int	nr, i;
    int delta_t = 0;

    /* XXX: implement the exact ... */
    if (!playback_enabled || (suspend_time > 0 && disabletime))
        return 1;
    if (!snddata.pdev)
    {
        i = initsid();
        if (i)
            return i;
        sdev_open = TRUE;
    }
#ifdef __riscos
    /* RISC OS vidc device uses a different approach... */
    SoundMachineReady = 1;
    if (SoundThreadActive != 0) return 0;
#endif

    /* Handling of cycle based sound engines. */
    if (cycle_based) {
        delta_t = clk - snddata.lastclk;
	nr = sound_machine_calculate_samples(snddata.psid,
					     snddata.buffer + snddata.bufptr,
					     BUFSIZE - snddata.bufptr,
					     &delta_t);
	if (delta_t) {
	    return closesound("Audio: sound buffer overflow.");
	}
    }
    /* Handling of sample based sound engines. */
    else {
        nr = (int)((SOUNDCLK_CONSTANT(clk) - snddata.fclk) / snddata.clkstep);
	if (!nr)
	    return 0;
	if (snddata.bufptr + nr > BUFSIZE) {
	    return closesound("Audio: sound buffer overflow.");
	}
	sound_machine_calculate_samples(snddata.psid,
					snddata.buffer + snddata.bufptr,
					nr, &delta_t);
	snddata.fclk   += nr*snddata.clkstep;
    }

    snddata.bufptr += nr;
    snddata.lastclk = clk;

    return 0;
}

/* reset sid */
void sound_reset(void)
{
    snddata.fclk = SOUNDCLK_CONSTANT(clk);
    snddata.wclk = clk;
    snddata.lastclk = clk;
    snddata.bufptr = 0;		/* ugly hack! */
    if (snddata.psid)
	sound_machine_reset(snddata.psid, clk);
}

static void prevent_clk_overflow_callback(CLOCK sub, void *data)
{
    snddata.lastclk -= sub;
    snddata.fclk -= SOUNDCLK_CONSTANT(sub);
    snddata.wclk -= sub;
    if (snddata.psid)
	sound_machine_prevent_clk_overflow(snddata.psid, sub);
}

#ifdef __riscos
void sound_synthesize(SWORD *buffer, int length)
{
    /* Handling of cycle based sound engines. */
    if (cycle_based) {
        /* FIXME: This is not implemented yet. A possible solution is
	 to make the main thread call sound_run at shorter intervals,
	 and reduce the responsibility of the sound thread to only
	 flush the sample buffer. On the other hand if sound_run were
	 called at shorter intervals the sound thread would probably
	 not be necessary at all. */
	snddata.lastclk = clk;
    }
    /* Handling of sample based sound engines. */
    else {
        int delta_t = 0;
        sound_machine_calculate_samples(snddata.psid, buffer, length,
					&delta_t);
	snddata.fclk += length * snddata.clkstep;
    }
}
#endif

/* flush all generated samples from buffer to sounddevice. adjust sid runspeed
   to match real running speed of program */
int sound_flush(int relative_speed)
{
    int	i, nr, space, used, fill = 0, dir = 0;

    if (!playback_enabled || sound_state_changed ||
	(cycle_based && !warp_mode_enabled && relative_speed != speed_factor))
    {
        if (sdev_open) sound_close();
	speed_factor = relative_speed;
        sound_state_changed = FALSE;
        return 0;
    }

    if (suspend_time > 0)
        enablesound();
    if (sound_run_sound())
        return 0;
    sound_resume();

    if (warp_mode_enabled && snddata.pdev->bufferstatus != NULL) {
      snddata.bufptr = 0;
      return 0;
    }

    if (snddata.pdev->flush)
    {
	char *state = sound_machine_dump_state(snddata.psid);
	i = snddata.pdev->flush(state);
	free(state);
	if (i)
	{
	    closesound("Audio: cannot flush.");
	    return 0;
	}
    }
    nr = snddata.bufptr -
	snddata.bufptr % (snddata.fragsize*snddata.oversamplenr);
    if (!nr)
	return 0;
    /* handle oversampling */
    if (snddata.oversamplenr > 1)
    {
	int j, newnr;
	newnr = nr >> snddata.oversampleshift;
        {
            SDWORD v;
            SWORD *sndbufj = &snddata.buffer[0];
            SWORD *sndbufi = &snddata.buffer[0];
            for (i = 0; i++ < newnr;)
            {
                for (v = j = 0; j++ < (int)(snddata.oversamplenr);)
                    v += *(sndbufj++);
                *(sndbufi++) = v >> snddata.oversampleshift;
            }
        }
        {
            SWORD *sndbufnewnr = &snddata.buffer[newnr];
            SWORD *sndbufnr    = &snddata.buffer[nr];
            for (i = 0; i++ < snddata.bufptr - nr;)
                *(sndbufnewnr++) = *(sndbufnr++);
        }
	snddata.bufptr -= (nr - newnr);
	nr = newnr;
    }
    /* adjust speed */
    if (snddata.pdev->bufferstatus)
    {
	space = snddata.pdev->bufferstatus(0);
	if (!snddata.firststatus)
	    space = snddata.bufsize - space;
	if (space < 0 || space > snddata.bufsize)
	{
	    log_warning(LOG_DEFAULT, "fragment problems %d %d %d",
		 space, snddata.bufsize - space, snddata.firststatus);

            closesound("Audio: fragment problems.");
	    return 0;
	}
	used = snddata.bufsize - space;
	/* buffer empty */
	if (used <= snddata.fragsize)
	{
	    SWORD		*p, v;
	    int			 j;
	    static int		 prev;
	    int			 now;
	    if (suspend_time > 0)
	    {
	        now = time(0);
		if (now == prev)
		{
		    suspendsound("buffer overruns");
		    return 0;
		}
		prev = now;
	    }
	    j = snddata.fragsize*snddata.fragnr - nr;
	    if (j > snddata.bufsize / 2
                && (cycle_based || speed_adjustment_setting != SOUND_ADJUST_ADJUSTING)
                && relative_speed)
	    {
		j = snddata.fragsize*(snddata.fragnr/2);
	    }
	    j *= sizeof(*p);
	    if (j > 0)
	    {
	        p = (short *)xmalloc(j);
		v = snddata.bufptr > 0 ? snddata.buffer[0] : 0;
		for (i = 0; i < j / (int)sizeof(*p); i++)
		    p[i] = (float)v*i/(j / (int)sizeof(*p));
		if (snddata.pdev->write(p, j/sizeof(*p)))
		{
		    closesound("Audio: write to sound device failed.");
                    free(p);
		    return 0;
		}
		snddata.lastsample = v;
                free(p);
	    }
	    fill = j;
	}
	if (cycle_based || speed_adjustment_setting != SOUND_ADJUST_ADJUSTING) {
            if (relative_speed > 0)
	        snddata.clkfactor = SOUNDCLK_CONSTANT(relative_speed) / 100;
	}
	else
	{
	    if (snddata.prevfill)
		snddata.prevused = used;
	    snddata.clkfactor = SOUNDCLK_MULT(snddata.clkfactor, SOUNDCLK_CONSTANT(1.0)
	        + (SOUNDCLK_CONSTANT(0.9)*(used - snddata.prevused))/snddata.bufsize);
	}
	snddata.prevused = used;
	snddata.prevfill = fill;
	if (cycle_based || speed_adjustment_setting == SOUND_ADJUST_EXACT)
	{
	    /* finetune VICE timer */
	    static int lasttime = 0;
	    static int minspace = 0;
	    int t = time(0);
	    if (minspace > space - nr)
		minspace = space - nr;
	    if (t != lasttime)
	    {
		lasttime = t;
		if (minspace <= 0)
		    dir = -1;
		if (minspace > snddata.fragsize)
		    dir = 1;
		minspace = snddata.bufsize;
	    }
	}
	else
	    snddata.clkfactor = SOUNDCLK_MULT(snddata.clkfactor, SOUNDCLK_CONSTANT(0.9)
	                      + ((used+nr)*SOUNDCLK_CONSTANT(0.12))/snddata.bufsize);
	snddata.clkstep = SOUNDCLK_MULT(snddata.origclkstep, snddata.clkfactor);
	if (SOUNDCLK_CONSTANT(cycles_per_rfsh) / snddata.clkstep >= snddata.bufsize)
	{
            if (suspend_time > 0)
	        suspendsound("running too slow");
	    else
            {
                closesound("Audio: running too slow.");
            }
	    return 0;
	}
	if (nr > space && nr < used)
	    nr = space;
    }
    if (snddata.pdev->write(snddata.buffer, nr))
    {
	closesound("Audio: write to sounddevice failed.");
	return 0;
    }
    snddata.lastsample = snddata.buffer[nr-1];
    snddata.bufptr -= nr;
    if (snddata.bufptr > 0)
    {
	for (i = 0; i < snddata.bufptr; i++)
	    snddata.buffer[i] = snddata.buffer[i + nr];
    }

    return dir;
}

/* close sid */
void sound_close(void)
{
    closesound(NULL);
    sdev_open     = FALSE;
    sound_state_changed = FALSE;
}

/* suspend sid (eg. before pause) */
void sound_suspend(void)
{
    int	i;
    SWORD *p, v;
    if (snddata.pdev)
    {
	if (snddata.pdev->write && !snddata.issuspended)
	{
	    p = (short*)xmalloc(snddata.fragsize*sizeof(SWORD));
	    if (!p)
		return;
	    v = snddata.lastsample;
	    for (i = 0; i < snddata.fragsize; i++)
		p[i] = v - (float)v * i / (int)snddata.fragsize;
	    i = snddata.pdev->write(p, snddata.fragsize);
	    free(p);
	    if (i)
		return;
	}
	if (snddata.pdev->suspend && !snddata.issuspended)
	{
	    if (snddata.pdev->suspend())
		return;
	}
	snddata.issuspended = 1;
    }
}

/* resume sid */
void sound_resume(void)
{
    if (snddata.pdev)
    {
        snddata.issuspended = (snddata.pdev->resume && snddata.issuspended == 1)
                              ? (snddata.pdev->resume() ? 1 : 0) : 0;
    }
}

/* set PAL/NTSC clock speed */
void sound_set_machine_parameter(long clock_rate, long ticks_per_frame)
{
    sound_state_changed = TRUE;

    cycles_per_sec  = clock_rate;
    cycles_per_rfsh = ticks_per_frame;
    rfsh_per_sec    = (1.0 / ((double)cycles_per_rfsh / (double)cycles_per_sec));
}

/* initialize sid at program start -time */
void sound_init(unsigned int clock_rate, unsigned int ticks_per_frame)
{
    sound_state_changed = FALSE;

    cycles_per_sec  = clock_rate;
    cycles_per_rfsh = ticks_per_frame;
    rfsh_per_sec    = (1.0 / ((double)cycles_per_rfsh / (double)cycles_per_sec));

    sound_machine_init();

    clk_guard_add_callback(&maincpu_clk_guard, prevent_clk_overflow_callback,
                           NULL);

#if defined(USE_ARTS)
    sound_init_arts_device();
#endif
#if defined(HAVE_LINUX_SOUNDCARD_H) || defined(HAVE_MACHINE_SOUNDCARD_H)
    sound_init_uss_device();
#endif
#if defined(HAVE_ESD_H) && defined(HAVE_LIBESD)
    sound_init_esd_device();
#endif
#if defined(HAVE_DMEDIA_AUDIO_H)
    sound_init_sgi_device();
#endif
#if defined(HAVE_SYS_AUDIOIO_H)
    sound_init_sun_device();
#endif
#if defined(HAVE_SYS_AUDIO_H)
    sound_init_hpux_device();
#endif
#if defined(HAVE_LIBUMSOBJ) && defined(HAVE_UMS_UMSAUDIODEVICE_H) && defined(HAVE_UMS_UMSBAUDDEVICE_H)
    sound_init_aix_device();
#endif
#if defined(HAVE_SDL_SDL_AUDIO_H) && defined(HAVE_LIBSDL)
    sound_init_sdl_device();
#endif

#ifdef __MSDOS__
#ifdef USE_MIDAS_SOUND
    sound_init_midas_device();
#else
    sound_init_sb_device();
#endif
#endif

#ifdef WIN32
    sound_init_dx_device();
    sound_init_wmm_device();
#endif

#ifdef WINCE
    sound_init_ce_device();
#endif

#ifdef OS2
    //    sound_init_mmos2_device();
    sound_init_dart_device();
    sound_init_dart2_device();
#endif

#ifdef __BEOS__
	sound_init_beos_device();
#endif

    sound_init_dummy_device();
    sound_init_fs_device();
    sound_init_speed_device();
    sound_init_dump_device();
    sound_init_wav_device();

#if 0
    sound_init_test_device();	/* XXX: missing */
#endif
}

long sound_sample_position(void)
{
    return (snddata.clkstep==0) ? 0 : (long)(SOUNDCLK_CONSTANT(clk) - snddata.fclk) / snddata.clkstep;
}

int sound_read(ADDRESS addr)
{
    if (sound_run_sound()) return -1;
    return sound_machine_read(snddata.psid, addr);
}

void sound_store(ADDRESS addr, BYTE val)
{
    int	i;
    if (sound_run_sound()) return;
    sound_machine_store(snddata.psid, addr, val);
    if (snddata.pdev->dump)
    {
        i = snddata.pdev->dump(addr, val, clk - snddata.wclk);
        snddata.wclk = clk;
        if (i)
            closesound("Audio: store to sounddevice failed.");
    }
}


void sound_set_warp_mode(int value)
{
    warp_mode_enabled = value;
}
