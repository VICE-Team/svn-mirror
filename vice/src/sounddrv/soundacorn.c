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

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "types.h"
#include "warn.h"
#include "sound.h"

#include "vice.h"
#include "log.h"

#include "ROlib.h"


extern void ui_set_sound_volume(void);


/* Separate stack for sound thread */
#define STACKSIZE	4096
char SoundCallbackStack[STACKSIZE];

timer_env SoundTimer = {0, NULL, 0, 0, NULL, NULL};


/* Set to 1 as soon as foreground process is allowed to call sample generation */
int SoundMachineReady = 0;
/* Polling frequency in cs. A value of 0 means auto = 0.5 * buffer frequency */
int SoundPollEvery = 0;

log_t vidc_log = LOG_ERR;

unsigned char *LinToLog = NULL;
unsigned char *LogScale = NULL;
SWORD *VIDCSampleBuffer;

static int buffersize;



static int init_vidc_device(warn_t *w, char *device, int *speed, int *fragsize, int *fragnr, double bufsize)
{
  int timerPeriod;

  if ((DigitalRenderer_ReadState() & DRState_Active) != 0)
    return 1;

  if (SoundTimer.stack_lwm == NULL)
  {
    memset(SoundCallbackStack, 0, STACKSIZE);
    if (timer_callback_init_stack(&SoundTimer, SoundCallbackStack, STACKSIZE, NULL) != 0)
    {
      log_error(vidc_log, "Can't set up runtime stack for sound thread!\n");
      return 1;
    }
  }

  *fragsize = (*fragsize + 15) & ~15;
  buffersize = (*fragsize)*(*fragnr);

  if (DigitalRenderer_Activate(1, buffersize, 1000000/(*speed)) != NULL)
  {
    return 1;
  }
  DigitalRenderer_GetTables(&LinToLog, &LogScale);

  if ((VIDCSampleBuffer = (SWORD*)malloc(buffersize*sizeof(SWORD))) == NULL)
  {
    DigitalRenderer_Deactivate();
    return 1;
  }

  SoundMachineReady = 0;
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
  timer_callback_install(&SoundTimer, timerPeriod, sound_poll);

  ui_set_sound_volume();

  /*fprintf(logfile, "vidc OK\n");*/

  return 0;
}


static int vidc_bufferstatus(warn_t *w, int first)
{
  if ((DigitalRenderer_ReadState() & DRState_NeedData) == 0)
    return buffersize;
  else
    return 0;
}


static void vidc_close(warn_t *w)
{
  _kernel_oserror *err;

  /*fprintf(logfile, "vidc_close\n");*/

  timer_callback_remove(&SoundTimer);
  SoundTimer.f = NULL;

  if ((err = DigitalRenderer_Deactivate()) != NULL)
  {
    log_error(vidc_log, "%s\n", err->errmess);
  }
  if (VIDCSampleBuffer != NULL)
  {
    free(VIDCSampleBuffer); VIDCSampleBuffer = NULL;
  }
  /* Use this to mark device inactive */
  LinToLog = NULL; LogScale = NULL;

  /*{
    FILE *fp;
    fp = fopen("soundstack", "wb"); fwrite(SoundCallbackStack, 1, STACKSIZE, fp); fclose(fp);
  }*/
}


static int vidc_suspend(warn_t *w)
{
  timer_callback_remove(&SoundTimer);

  if (DigitalRenderer_Pause() != NULL)
    return 1;

  return 0;
}



static int vidc_resume(warn_t *w)
{
  if (DigitalRenderer_Resume() != NULL)
    return 1;

  if (SoundTimer.f != NULL)
  {
    timer_callback_install(&SoundTimer, SoundTimer.period, sound_poll);
  }

  return 0;
}



/* Dummies */
static int vidc_write(warn_t *w, SWORD *pbuf, int nr)
{
  return 0;
}


static int vidc_dump(warn_t *w, ADDRESS addr, BYTE byte, CLOCK clks)
{
  return 0;
}


static int vidc_flush(warn_t *w, char *state)
{
  return 0;
}


static sound_device_t vidc_device =
{
  "vidc",
  init_vidc_device,
  vidc_write,
  vidc_dump,
  vidc_flush,
  vidc_bufferstatus,
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


int sound_init_vidc_device(void)
{
  return sound_register_device(&vidc_device);
}
