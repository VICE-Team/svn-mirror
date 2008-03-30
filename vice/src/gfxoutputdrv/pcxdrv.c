/*
 * pcxdrv.c - Create a PCX file.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <string.h>
#include <stdlib.h>

#include "archdep.h"
#include "pcxdrv.h"
#include "lib.h"
#include "log.h"
#include "gfxoutput.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "util.h"


typedef struct gfxoutputdrv_data_s
{
  FILE *fd;
  char *ext_filename;
  BYTE *data;
  BYTE *pcx_data;
  int line;
} gfxoutputdrv_data_t;

static gfxoutputdrv_t pcx_drv;


static int pcxdrv_write_file_header(screenshot_t *screenshot)
{
  BYTE header[128];

  memset(header, 0, sizeof(header));

  header[0]=0xa;
  header[1]=5;
  header[2]=1;
  header[3]=8;

  util_word_to_le_buf(&header[8], screenshot->width-1);
  util_word_to_le_buf(&header[10], screenshot->height-1);

  util_word_to_le_buf(&header[12], screenshot->dpi_x);
  util_word_to_le_buf(&header[14], screenshot->dpi_x);

  header[65]=1;
  util_word_to_le_buf(&header[66], screenshot->width);

  if (fwrite(header, sizeof(header), 1, screenshot->gfxoutputdrv_data->fd)<1)
    return -1;

  return 0;
}

static int pcxdrv_open(screenshot_t *screenshot, const char *filename)
{
  gfxoutputdrv_data_t *sdata;

  if (screenshot->palette->num_entries > 256)
  {
    log_error(LOG_DEFAULT, "Max 256 colors supported.");
    return -1;
  }

  sdata = (gfxoutputdrv_data_t *)lib_malloc(sizeof(gfxoutputdrv_data_t));
  screenshot->gfxoutputdrv_data = sdata;
  sdata->line = 0;
  sdata->ext_filename=util_add_extension_const(filename, pcx_drv.default_extension);
  sdata->fd = fopen(filename, "wb");

  if (sdata->fd==NULL)
  {
    lib_free(sdata->ext_filename);
    lib_free(sdata);
    return -1;
  }

  if (pcxdrv_write_file_header(screenshot)<0)
  {
    fclose(sdata->fd);
    lib_free(sdata->ext_filename);
    lib_free(sdata);
    return -1;
  }

  sdata->data = (BYTE *)lib_malloc(screenshot->width);
  sdata->pcx_data = (BYTE *)lib_malloc(screenshot->width*2);

  return 0;
}

static int pcxdrv_write(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *sdata;
  BYTE color,amount;
  int i,j=0;

  sdata = screenshot->gfxoutputdrv_data;
  (screenshot->convert_line)(screenshot, sdata->data, sdata->line, SCREENSHOT_MODE_PALETTE);

  color=sdata->data[0];
  amount=1;
  for (i = 1; i<screenshot->width; i++)
  {
    if (sdata->data[i]==color)
    {
      amount=amount+1;
      if (amount==63)
      {
        sdata->pcx_data[j]=0xff;
        sdata->pcx_data[j+1]=color;
        j=j+2;
        amount=0;
      }
    }
    else
    {
      if (amount==0)
      {
        color=sdata->data[i];
        amount=1;
      }
      else
      {
        if (amount>1)
        {
          sdata->pcx_data[j]=0xc0 | amount;
          sdata->pcx_data[j+1]=color;
          j=j+2;
          color=sdata->data[i];
          amount=1;
        }
        else
        {
          if (color>0xbf)
          {
            sdata->pcx_data[j]=0xc1;
            sdata->pcx_data[j+1]=color;
            j=j+2;
          }
          else
          {
            sdata->pcx_data[j]=color;
            j++;
          }
          color=sdata->data[i];
          amount=1;
        }
      }
    }
  }
  if (amount==1)
  {
    if (color>0xbf)
    {
      sdata->pcx_data[j]=0xc1;
      sdata->pcx_data[j+1]=color;
      j=j+2;
    }
    else
    {
      sdata->pcx_data[j]=color;
      j++;
    }
  }
  else
  {
    if (amount>1)
    {
      sdata->pcx_data[j]=0xc0 | amount;
      sdata->pcx_data[j+1]=color;
      j=j+2;
    }
  }

  if (fwrite(sdata->pcx_data, j, 1, sdata->fd)<1)
    return -1;

  return 0;
}

static int pcxdrv_close(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *sdata;
  int i;
  BYTE pcx_color_prefix[2]="\x0c";
  BYTE pcx_colors[256*3];

  sdata = screenshot->gfxoutputdrv_data;

  fwrite(pcx_color_prefix, 1, 1, sdata->fd);

  for (i = 0; i < screenshot->palette->num_entries; i++)
  {
    pcx_colors[i*3]=screenshot->palette->entries[i].red;
    pcx_colors[(i*3)+1]=screenshot->palette->entries[i].green;
    pcx_colors[(i*3)+2]=screenshot->palette->entries[i].blue;
  }

  fwrite(pcx_colors, 3*256, 1, sdata->fd);

  fclose(sdata->fd);
  lib_free(sdata->data);
  lib_free(sdata->pcx_data);
  lib_free(sdata->ext_filename);
  lib_free(sdata);

  return 0;
}

static int pcxdrv_save(screenshot_t *screenshot, const char *filename)
{
  if (pcxdrv_open(screenshot, filename) < 0)
    return -1;

  for (screenshot->gfxoutputdrv_data->line = 0; 
       screenshot->gfxoutputdrv_data->line < screenshot->height;
       (screenshot->gfxoutputdrv_data->line)++)
  {
    pcxdrv_write(screenshot);
  }

  if (pcxdrv_close(screenshot) < 0)
    return -1;

  return 0;
}

static gfxoutputdrv_t pcx_drv =
{
    "PCX",
    "PCX screenshot",
    "pcx",
    pcxdrv_open,
    pcxdrv_close,
    pcxdrv_write,
    pcxdrv_save,
    NULL
};

void gfxoutput_init_pcx(void)
{
  gfxoutput_register(&pcx_drv);
}
