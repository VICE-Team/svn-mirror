/*
 * spritedrv.c - screenshot driver for RISC OS Sprites
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"


static int sprite_save(screenshot_t *screenshot, const char *filename)
{
    unsigned int line, x;
    BYTE *data;
    BYTE *linedata;
    int pitch;
    const palette_t *pal;
    const palette_entry_t *pe;
    int pal_size;
    FILE *fp;
    int sprdesc[4+11];
    int bpp, ldbpp;
    int sprmode;

    if ((fp = fopen(filename, "wb")) == NULL)
      return -1;

    pal = screenshot->palette;
    if (pal->num_entries <= 2)
    {
      /* 1bpp sprite */
      ldbpp = 0;
      sprmode = 25;
    }
    else if (pal->num_entries <= 16)
    {
      /* 4bpp sprite */
      ldbpp = 2;
      sprmode = 27;
    }
    else
    {
      /* 8bpp sprite */
      ldbpp = 3;
      sprmode = 28;
    }

    bpp = (1 << ldbpp); pal_size = (1 << bpp);
    pitch = ((screenshot->width + (1 << (5-ldbpp)) - 1) >> (3 - ldbpp)) & ~3;

    /* sprite area control block */
    sprdesc[0] = (4 + 11 + 2*pal_size) * sizeof(int) + pitch * screenshot->height;
    sprdesc[1] = 1;
    sprdesc[2] = 16;
    sprdesc[3] = sprdesc[0];

    /* sprite header */
    sprdesc[4] = sprdesc[0] - 4*sizeof(int);
    memset(sprdesc + 5, 0, 12);
    strncpy((char*)(sprdesc + 5), "vicescreen", 12);
    sprdesc[8]  = (pitch >> 2) - 1;
    sprdesc[9]  = screenshot->height - 1;
    sprdesc[10] = 0;
    sprdesc[11] = ((screenshot->width << ldbpp) - 1) & 31;
    sprdesc[12] = 44 + 2*pal_size*sizeof(int);
    sprdesc[13] = sprdesc[12];
    sprdesc[14] = sprmode;
    fwrite(sprdesc+1, sizeof(int), 4+11-1, fp);

    /* write palette */
    for (x=0, pe=pal->entries; x<pal->num_entries; x++, pe++)
    {
        sprdesc[0] = 0x10 | (pe->red << 8) | (pe->green << 16) | (pe->blue << 24);
        sprdesc[1] = sprdesc[0];
        fwrite(sprdesc, sizeof(int), 2, fp);
    }
    /* The number of colours might be smaller than the colour depth. Set rest to 0 */
    for (; x<pal_size; x++)
    {
      sprdesc[0] = 0x10; sprdesc[0] = 0x10;
      fwrite(sprdesc, sizeof(int), 2, fp);
    }

    data = (BYTE *)xmalloc(screenshot->width);

    if (ldbpp != 3)
        linedata = (BYTE *)xmalloc(pitch);
    else
        linedata = data;

    memset(linedata, 0, pitch);

    for (line = 0; line < screenshot->height; line++)
    {
        unsigned int pixel, shift;
        unsigned int *dptr = (unsigned int*)linedata;

        screenshot_line_data(screenshot, data, line, SCREENSHOT_MODE_PALETTE);

        switch (ldbpp)
        {
            case 0:
              for (x=0, pixel=0, shift=0; x<screenshot->width; x++)
              {
                pixel |= data[x] << shift;
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
                pixel |= data[x] << shift;
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
        fwrite(linedata, 1, pitch, fp);
    }

    fclose(fp);

    if (ldbpp != 3)
        free(linedata);

    free(data);

    return 0;
}

static screendrv_t sprite_drv =
{
    "Sprite",
    sprite_save
};

void screenshot_init_sprite(void)
{
    screenshot_register(&sprite_drv);
}
