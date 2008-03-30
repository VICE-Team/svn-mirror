/*
 * soundacorn.c - Sound driver for Acorn RISC OS.
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

#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "ui.h"

#include "sid/sid.h"

#include "DRender.h"



/* Separate stack for sound thread */
#define STACKSIZE	4096
char SoundCallbackStack[STACKSIZE];

timer_env SoundTimer = {0, NULL, 0, 0, NULL, NULL, 0};


/* Set to 1 as soon as foreground process is allowed to call sample generation */
int SoundMachineReady = 0;
/* Polling frequency in cs. A value of 0 means auto = 0.5 * buffer frequency */
int SoundPollEvery = 0;

log_t vidc_log = LOG_DEFAULT;

int SoundThreadActive = 0;
SWORD *VIDCSampleBuffer;

static int SndTimerActive = 0;
static int buffersize;
static int numbuffers;

/* maximum size of DMA buffer */
static int MaxBufferSize = 0;

static int timerPeriod = 0;



/* convert frequency to exact VIDC frequency */
void sound_get_vidc_frequency(int *speed, int *period)
{
  int per;

  per = (1000000 + (*speed/2)) / (*speed);
  *speed  = (1000000 + (per/2)) / per;
  if (period != NULL)
    *period = per;
}


/* configure sound, taking into account that the desire buffer size may
   not be available (the limit seems to be 512 bytes) */
static int sound_configure_vidc(int *speed, int *fragsize, int *fragnr, int *chanptr, int sync)
{
  _kernel_oserror *err;
  int period;
  int newsound;
  int channelnum;

  if (*fragsize > MaxBufferSize)
  {
    int channels, slength, speriod;
    int oldenable;
    int oldslength;

    oldenable = Sound_Enable(1);
    channels = 0; slength = *fragsize; speriod = 0;
    Sound_Configure(&channels, &slength, &speriod);
    oldslength = slength;
    while (*fragsize > slength)
    {
      *fragsize >>= 1; *fragnr <<= 1;
      channels = 0; slength = *fragsize; speriod = 0;
      Sound_Configure(&channels, &slength, &speriod);
    }
    /* reinstate previous settings */
    channels = 0; speriod = 0;
    Sound_Configure(&channels, &oldslength, &speriod);
    Sound_Enable(oldenable);
    MaxBufferSize = *fragsize;
  }

  *fragsize = (*fragsize + 15) & ~15;

  DigitalRenderer_NumBuffers((sync == 0) ? 0 : *fragnr);

  resources_get_int("Use16BitSound", &newsound);

  channelnum = *chanptr;

  if (newsound != 0)
  {
    /* 16bit linear sound only allows mono or stereo */
    if (channelnum > 2)
      channelnum = 2;

    if (DigitalRenderer_Activate16(channelnum, *fragsize, *speed, 1) != NULL)
      newsound = 0;
    else
      *speed = DigitalRenderer_GetFrequency();
  }

  if (newsound == 0)
  {
    /* 8bit logarithmic allows 1,2,4 and 8 channels */
    channelnum = *chanptr;
    if (channelnum == 3)
      channelnum = 4;
    else if (channelnum > 4)
      channelnum = 8;

    /* adapt sample speed */
    sound_get_vidc_frequency(speed, &period);
    if ((err = DigitalRenderer_Activate(channelnum, *fragsize, period)) != NULL)
    {
      log_error(vidc_log, err->errmess);
      return -1;
    }
  }

  buffersize = *fragsize;
  numbuffers = *fragnr;
  *chanptr = channelnum;

  return 0;
}


/*
 *  Asynchronous sound device code
 */
static int init_vidc_device(const char *device, int *speed, int *fragsize, int *fragnr, int *channels)
{
  int sidEngine;

  if (resources_get_int("SidEngine", &sidEngine) == 0)
  {
    if (sidEngine == SID_ENGINE_RESID)
    {
      log_error(vidc_log, "You must use VIDCS with reSID!");
      return 1;
    }
  }

  if ((DigitalRenderer_ReadState() & DRState_Active) != 0)
  {
    log_error(vidc_log, "DigitalRenderer already active!");
    return 1;
  }

  if (SoundTimer.stack_lwm == NULL)
  {
    memset(SoundCallbackStack, 0, STACKSIZE);
    if (timer_callback_init_stack(&SoundTimer, SoundCallbackStack, STACKSIZE, NULL) != 0)
    {
      log_error(vidc_log, "Can't set up runtime stack for sound thread!");
      return 1;
    }
  }

  if (sound_configure_vidc(speed, fragsize, fragnr, channels, 0) != 0)
    return 1;

  if ((VIDCSampleBuffer = (SWORD*)lib_malloc(buffersize*(*channels)*sizeof(SWORD))) == NULL)
  {
    log_error(vidc_log, "Can't claim memory for sound buffer!");
    DigitalRenderer_Deactivate();
    return 1;
  }

  SoundMachineReady = 0; SndTimerActive = 0;
  /* SoundPollEvery == 0 ==> auto, calculate value from buffersize and frequency */
  if (SoundPollEvery <= 0)
  {
    /* Default poll frequency = 0.5 * buffer frequency in centiseconds */
    timerPeriod = (50 * buffersize) / (*speed);
    if (timerPeriod < 1) timerPeriod = 1;
  }
  else
  {
    timerPeriod = SoundPollEvery;
  }

  SoundThreadActive = 1;
  timer_callback_install(&SoundTimer, timerPeriod, sound_poll, 1);

  ui_set_sound_volume();

  /*log_message(vidc_log, "vidc OK");*/

  return 0;
}


static int vidc_bufferspace(void)
{
  /* if DR needs new samples, we have buffersize stored, otherwise 2*buffersize */
  if ((DigitalRenderer_ReadState() & DRState_NeedData) == 0)
    return (numbuffers - 1)*buffersize;
  else
    return (numbuffers - 2)*buffersize;
}


static void vidc_close(void)
{
  _kernel_oserror *err;

  /*log_message(vidc_log, "vidc_close\n");*/

  SoundMachineReady = 0;
  timer_callback_remove(&SoundTimer);
  SoundTimer.f = NULL;

  if ((err = DigitalRenderer_Deactivate()) != NULL)
  {
    log_error(vidc_log, err->errmess);
  }
  if (VIDCSampleBuffer != NULL)
  {
    lib_free(VIDCSampleBuffer); VIDCSampleBuffer = NULL;
  }
  /* Use this to mark device inactive */
  SoundThreadActive = 0;

  /*{
    FILE *fp;
    fp = fopen("soundstack", "wb"); fwrite(SoundCallbackStack, 1, STACKSIZE, fp); fclose(fp);
  }*/
}


static int vidc_suspend(void)
{
  timer_callback_remove(&SoundTimer);

  if (DigitalRenderer_Pause() != NULL)
    return 1;

  return 0;
}



static int vidc_resume(void)
{
  if (DigitalRenderer_Resume() != NULL)
    return 1;

  if (SoundTimer.f != NULL)
  {
    timer_callback_install(&SoundTimer, timerPeriod, sound_poll, 1);
  }

  return 0;
}



/* Dummies */
static int vidc_write(SWORD *pbuf, size_t nr)
{
  return 0;
}


static sound_device_t vidc_device =
{
  "vidc",
  init_vidc_device,
  vidc_write,
  NULL,
  NULL,
  vidc_bufferspace,
  vidc_close,
  vidc_suspend,
  vidc_resume
};



/* Called every couple of rasterlines */
void sound_poll(void)
{
  if ((VIDCSampleBuffer != NULL) && (SoundMachineReady != 0))
  {
    if ((DigitalRenderer_ReadState() & (DRState_NeedData | DRState_Active)) == (DRState_NeedData | DRState_Active))
    {
      sound_synthesize(VIDCSampleBuffer, buffersize);
      DigitalRenderer_New16BitSample(VIDCSampleBuffer);
    }
  }
}


/* Called before Wimp_Poll */
int sound_wimp_poll_prologue(void)
{
  if ((SoundTimer.f != NULL) && ((SoundTimer.flags & 3) != 0))
  {
    SndTimerActive = 1;
    timer_callback_remove(&SoundTimer);
    return 0;
  }
  return -1;
}

/* Called after Wimp_Poll */
int sound_wimp_poll_epilogue(int install)
{
  if ((SoundTimer.f != NULL) && (SndTimerActive != 0) && (install != 0))
  {
    timer_callback_resume(&SoundTimer);
    SndTimerActive = 0; return 0;
  }
  SndTimerActive = 0; return -1;
}

/* Called on exit (hopefully abnormal exit too) */
void sound_wimp_safe_exit(void)
{
  if ((SoundTimer.f != NULL) && ((SoundTimer.flags & 3) != 0))
  {
    timer_callback_remove(&SoundTimer);
    DigitalRenderer_Deactivate();
  }
}



/*
 *  Synchronous sound device interface
 */
static int init_vidc_sync_device(const char *device, int *speed, int *fragsize, int *fragnr, int *channels)
{
  if ((DigitalRenderer_ReadState() &DRState_Active) != 0)
    return 1;

  if (sound_configure_vidc(speed, fragsize, fragnr, channels, 1) != 0)
    return 1;

  /* unthreaded sound */
  SoundThreadActive = 0;

  log_message(LOG_DEFAULT, "fragsize %d, frags %d", *fragsize, *fragnr);

  ui_set_sound_volume();

  return 0;
}


static int vidc_sync_bufferspace(void)
{
  /* DigitalRenderer_StreamStatistics returns the number of filled buffers. */
  int numbuf = DigitalRenderer_StreamStatistics();

  return (numbuffers - numbuf) * buffersize;
}


static void vidc_sync_close(void)
{
  _kernel_oserror *err;

  if ((err = DigitalRenderer_Deactivate()) != NULL)
     log_error(vidc_log, err->errmess);
}


static int vidc_sync_suspend(void)
{
  if (DigitalRenderer_Pause() != NULL)
    return 1;

  return 0;
}


static int vidc_sync_resume(void)
{
  if (DigitalRenderer_Resume() != NULL)
    return 1;

  return 0;
}


static int vidc_sync_write(SWORD *pbuf, size_t nr)
{
  DigitalRenderer_Stream16BitSamples(pbuf, nr);
  return 0;
}


static sound_device_t vidc_sync_device =
{
  "vidcs",
  init_vidc_sync_device,
  vidc_sync_write,
  NULL,
  NULL,
  vidc_sync_bufferspace,
  vidc_sync_close,
  vidc_sync_suspend,
  vidc_sync_resume,
  1
};





/* Init all RISC OS sound devices */
int sound_init_vidc_device(void)
{
  if (sound_register_device(&vidc_device) != 0)
    return 1;

  return sound_register_device(&vidc_sync_device);
}
