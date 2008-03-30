/*
 * gifdrv.c - Create a GIF file.
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
#include <gif_lib.h>

#include "archdep.h"
#include "gifdrv.h"
#include "lib.h"
#include "log.h"
#include "gfxoutput.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "util.h"


typedef struct gfxoutputdrv_data_s
{
  GifFileType *fd;
  char *ext_filename;
  BYTE *data;
  unsigned int line;
} gfxoutputdrv_data_t;

static gfxoutputdrv_t gif_drv;

static ColorMapObject *gif_colors=NULL;

static int gifdrv_open(screenshot_t *screenshot, const char *filename)
{
  unsigned int i;
  gfxoutputdrv_data_t *sdata;
  GifColorType ColorMap256[256];

  if (screenshot->palette->num_entries > 256)
  {
    log_error(LOG_DEFAULT, "Max 256 colors supported.");
    return -1;
  }

  sdata = (gfxoutputdrv_data_t *)lib_malloc(sizeof(gfxoutputdrv_data_t));

  screenshot->gfxoutputdrv_data = sdata;

  sdata->line = 0;

  sdata->ext_filename=util_add_extension_const(filename, gif_drv.default_extension);

  sdata->fd=EGifOpenFileName(sdata->ext_filename, FALSE);

  if (sdata->fd==NULL)
  {
    lib_free(sdata->ext_filename);
    lib_free(sdata);
    return -1;
  }

  sdata->data = (BYTE *)lib_malloc(screenshot->width);

  gif_colors=MakeMapObject(screenshot->palette->num_entries, ColorMap256);

  for (i = 0; i < screenshot->palette->num_entries; i++)
  {
    gif_colors->Colors[i].Blue=screenshot->palette->entries[i].blue;
    gif_colors->Colors[i].Green=screenshot->palette->entries[i].green;
    gif_colors->Colors[i].Red=screenshot->palette->entries[i].red;
  }

  EGifSetGifVersion("87a");

  if (EGifPutScreenDesc(sdata->fd, screenshot->width, screenshot->height, 8, 0, gif_colors) == GIF_ERROR ||
      EGifPutImageDesc(sdata->fd, 0, 0, screenshot->width, screenshot->height, FALSE, NULL) == GIF_ERROR)
  {
    EGifCloseFile(sdata->fd);
    FreeMapObject(gif_colors);
    lib_free(sdata->data);
    lib_free(sdata->ext_filename);
    lib_free(sdata);
    return -1;
  }

  return 0;
}

static int gifdrv_write(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *sdata;

  sdata = screenshot->gfxoutputdrv_data;

  (screenshot->convert_line)(screenshot, sdata->data, sdata->line, SCREENSHOT_MODE_PALETTE);

  if (EGifPutLine(sdata->fd, sdata->data, screenshot->width)==GIF_ERROR)
    return -1;

  return 0;
}

static int gifdrv_close(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *sdata;

  sdata = screenshot->gfxoutputdrv_data;

  EGifCloseFile(sdata->fd);
  FreeMapObject(gif_colors);
  lib_free(sdata->data);
  lib_free(sdata->ext_filename);
  lib_free(sdata);

  return 0;
}

static int gifdrv_save(screenshot_t *screenshot, const char *filename)
{
  if (gifdrv_open(screenshot, filename) < 0)
    return -1;

  for (screenshot->gfxoutputdrv_data->line = 0; 
       screenshot->gfxoutputdrv_data->line < screenshot->height;
       (screenshot->gfxoutputdrv_data->line)++)
  {
    gifdrv_write(screenshot);
  }

  if (gifdrv_close(screenshot) < 0)
    return -1;

  return 0;
}

static gfxoutputdrv_t gif_drv =
{
    "GIF",
    "GIF screenshot",
    "gif",
    gifdrv_open,
    gifdrv_close,
    gifdrv_write,
    gifdrv_save,
    NULL
};

void gfxoutput_init_gif(void)
{
  gfxoutput_register(&gif_drv);
}
