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

#include "config.h"
#include "types.h"
#include "warn.h"
#include "sound.h"

#include "vice.h"

#include "ROlib.h"

int SoundEvery = 20;
int SoundLines = 0;

unsigned char *LinToLog = NULL;
unsigned char *LogScale = NULL;

static int buffersize;
static unsigned char *buffer;

static int init_vidc_device(warn_t *w, char *device, int *speed, int *fragsize, int *fragnr, double bufsize)
{
  if ((DigitalRenderer_ReadState() & DRState_Active) != 0)
    return 1;

  buffersize = (*fragsize + 15) & ~15;
  if (DigitalRenderer_Activate(1, buffersize, 1000000/(*speed)) != NULL)
  {
    return 1;
  }
  DigitalRenderer_GetTables(&LinToLog, &LogScale);

  if ((buffer = (unsigned char*)malloc(buffersize)) == NULL)
  {
    DigitalRenderer_Deactivate();
    return 1;
  }
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

  if ((err = DigitalRenderer_Deactivate()) != NULL)
  {
    fprintf(logfile, "%s\n", err->errmess);
  }
  if (buffer != NULL)
  {
    free(buffer); buffer = NULL;
  }
  /* Use this to mark device inactive */
  LinToLog = NULL; LogScale = NULL;
}


static int vidc_suspend(warn_t *w)
{
  if (DigitalRenderer_Pause() != NULL)
    return 1;
  return 0;
}



static int vidc_resume(warn_t *w)
{
  if (DigitalRenderer_Resume() != NULL)
    return 1;
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
  if (buffer != NULL)
  {
    if ((DigitalRenderer_ReadState() & (DRState_NeedData | DRState_Active)) == (DRState_NeedData | DRState_Active))
    {
      sound_synthesize(buffer, buffersize);
      DigitalRenderer_NewSample(buffer);
    }
  }
}


int sound_init_vidc_device(void)
{
  return sound_register_device(&vidc_device);
}
