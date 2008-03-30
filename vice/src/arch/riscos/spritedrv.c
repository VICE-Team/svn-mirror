/*
 * spritedrv.c - screenshot driver for RISC OS Sprites
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include "archdep.h"
#include "gfxoutput.h"
#include "lib.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"

#include "wlsprite.h"


typedef struct gfxoutputdrv_data_s {
  FILE *fp;
  BYTE *data;
  BYTE *linedata;
  int ldbpp;
  int pitch;
  unsigned int line;
} gfxoutputdrv_data_t;


static int sprite_open(screenshot_t *screenshot, const char *filename)
{
  gfxoutputdrv_data_t *god;

  god = (gfxoutputdrv_data_t*)lib_malloc(sizeof(gfxoutputdrv_data_t));
  screenshot->gfxoutputdrv_data = god;

  god->data = NULL;
  god->linedata = NULL;
  god->line = 0;

  if ((god->fp = fopen(filename, MODE_WRITE)) != NULL)
  {
    const palette_t *pal = screenshot->palette;
    const palette_entry_t *pe;
    int bpp, pal_size, pal_bytes, header_size;
    unsigned int *sprpalette;
    sprite_area_t *sarea;
    int x;

    if (pal->num_entries <= 2)
    {
      /* 1bpp sprite */
      god->ldbpp = 0;
    }
    else if (pal->num_entries <= 16)
    {
      /* 4bpp sprite */
      god->ldbpp = 2;
    }
    else
    {
      /* 8bpp sprite */
      god->ldbpp = 3;
    }

    bpp = (1 << god->ldbpp); pal_size = (1 << bpp); pal_bytes = 2*pal_size*sizeof(unsigned int);
    if ((sprpalette = (unsigned int*)lib_malloc(pal_bytes)) == NULL)
    {
      fclose(god->fp);
      god->fp = NULL;
      return -1;
    }

    /* create palette */
    for (x=0, pe=pal->entries; x<pal->num_entries; x++, pe++)
    {
      sprpalette[x] = 0x10 | (pe->red << 8) | (pe->green << 16) | (pe->blue << 24);
    }
    /* The number of colours might be smaller than the colour depth. Set rest to 0 */
    for (; x<pal_size; x++)
    {
      sprpalette[x] = 0x10;
    }

    header_size = sizeof(sprite_area_t) + sizeof(sprite_desc_t) + pal_bytes;
    if ((sarea = (sprite_area_t*)lib_malloc(header_size)) != NULL)
    {
      sprite_desc_t *sprite;

      /* sprite area control block */
      sarea->tsize = sizeof(sprite_area_t) + wlsprite_calc_size(screenshot->width, screenshot->height, bpp, sprpalette, 0);
      sarea->numsprites = 1;
      sarea->firstoff = sizeof(sprite_area_t);
      sarea->firstfree = sarea->tsize;

      sprite = wlsprite_area_get_sprite(sarea);
      wlsprite_init_sprite(sprite, screenshot->width, screenshot->height, bpp, "vicescreen", sprpalette, 0);

      god->pitch = (sprite->wwidth + 1) << 2;
      fwrite(&(sarea->numsprites), 1, header_size-4, god->fp);

      god->data = (BYTE *)lib_malloc((screenshot->width + 3) & ~3);

      if (god->ldbpp != 3)
        god->linedata = (BYTE *)lib_malloc(god->pitch);
      else
        god->linedata = god->data;

      memset(god->linedata, 0, god->pitch);

      lib_free(sarea);
      lib_free(sprpalette);

      return 0;
    }
    lib_free(sprpalette);
  }
  return -1;
}


static int sprite_write(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *god = screenshot->gfxoutputdrv_data;

  if (god != NULL)
  {
    unsigned int *dptr = (unsigned int *)(god->linedata);
    unsigned int x, pixel, shift;

    (screenshot->convert_line)(screenshot, god->data, god->line, SCREENSHOT_MODE_PALETTE);

    switch (god->ldbpp)
    {
      case 0:
        for (x=0, pixel=0, shift=0; x<screenshot->width; x++)
        {
          pixel |= (god->data)[x] << shift;
          shift++;
          if (shift >= 32)
          {
            *dptr++ = pixel;
            shift = 0;
            pixel = 0;
          }
        }
        if (shift != 0)
          *dptr = pixel;
        break;
      case 2:
        for (x=0, pixel=0, shift=0; x<screenshot->width; x++)
        {
          pixel |= (god->data)[x] << shift;
          shift += 4;
          if (shift >= 32)
          {
            *dptr++ = pixel;
            shift = 0;
            pixel = 0;
          }
        }
        if (shift != 0)
          *dptr = pixel;
        break;
      default:
        break;
    }

    return (fwrite(god->linedata, 1, god->pitch, god->fp) == god->pitch) ? 0 : -1;
  }

  return -1;
}


static int sprite_close(screenshot_t *screenshot)
{
  gfxoutputdrv_data_t *god = screenshot->gfxoutputdrv_data;

  if (god != NULL)
  {
    if (god->fp != NULL)
      fclose(god->fp);

    if ((god->ldbpp != 3) && (god->linedata != NULL))
      lib_free(god->linedata);

    if (god->data != NULL)
      lib_free(god->data);

    lib_free(god);

    return 0;
  }

  return -1;
}


static int sprite_save(screenshot_t *screenshot, const char *filename)
{
  if (sprite_open(screenshot, filename) == 0)
  {
    gfxoutputdrv_data_t *god = screenshot->gfxoutputdrv_data;

    for (god->line = 0; god->line < screenshot->height; (god->line)++)
    {
      if (sprite_write(screenshot) != 0)
        break;
    }
  }

  return sprite_close(screenshot);
}


static gfxoutputdrv_t sprite_drv =
{
  "Sprite",
  NULL,
  sprite_open,
  sprite_close,
  sprite_write,
  sprite_save
};


void screenshot_init_sprite(void)
{
  gfxoutput_register(&sprite_drv);
}
