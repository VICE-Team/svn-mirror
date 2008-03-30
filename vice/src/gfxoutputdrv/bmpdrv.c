/*
 * bmpdrv.c - Create a BMP file.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include "bmpdrv.h"
#include "lib.h"
#include "log.h"
#include "gfxoutput.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "util.h"


#define BMP_HDR_OFFSET (14 + 40 + 4 * screenshot->palette->num_entries)

typedef struct gfxoutputdrv_data_s {
    FILE *fd;
    char *ext_filename;
    BYTE *data;
    BYTE *bmp_data;
    int line;
    unsigned int bpp;
} gfxoutputdrv_data_t;

#if defined(__BEOS__) && defined(WORDS_BIGENDIAN)
extern gfxoutputdrv_t bmp_drv;
#else
static gfxoutputdrv_t bmp_drv;
#endif

static DWORD bmpdrv_bmp_size(screenshot_t *screenshot)
{
    DWORD size = 0;

    switch (screenshot->gfxoutputdrv_data->bpp) {
      case 1:
        size = (DWORD)(BMP_HDR_OFFSET
               + (screenshot->width / 8 * screenshot->height));
        break;
      case 4:
        size = (DWORD)(BMP_HDR_OFFSET
               + (screenshot->width / 2 * screenshot->height));
        break;
      case 8:
        size = (DWORD)(BMP_HDR_OFFSET
               + (screenshot->width * screenshot->height));
        break;
    }

    return size;
}

static int bmpdrv_write_file_header(screenshot_t *screenshot)
{
    BYTE header[14];

    memset(header, 0, sizeof(header));

    header[0] = 'B';
    header[1] = 'M';

    util_dword_to_le_buf(&header[2], bmpdrv_bmp_size(screenshot));

    util_dword_to_le_buf(&header[10], BMP_HDR_OFFSET);

    if (fwrite(header, sizeof(header), 1, screenshot->gfxoutputdrv_data->fd)
        < 1)
        return -1;

    return 0;
}

static int bmpdrv_write_bitmap_info(screenshot_t *screenshot)
{
    BYTE binfo[40];
    BYTE *bcolor;
    unsigned int i;

    memset(binfo, 0, sizeof(binfo));

    util_dword_to_le_buf(&binfo[0], sizeof(binfo));
    util_dword_to_le_buf(&binfo[4], screenshot->width);
    util_dword_to_le_buf(&binfo[8], screenshot->height);

    binfo[12] = 1;
    binfo[13] = 0;

    binfo[14] = screenshot->gfxoutputdrv_data->bpp;
    binfo[15] = 0;

    util_dword_to_le_buf(&binfo[16], 0); /* BI_RGB */
    util_dword_to_le_buf(&binfo[20], 0);

    /* DPI in Pixels per Meter*/
    util_dword_to_le_buf(&binfo[24], screenshot->dpi_x * 10000 / 254);
    util_dword_to_le_buf(&binfo[28], screenshot->dpi_y * 10000 / 254);

    util_dword_to_le_buf(&binfo[32], screenshot->palette->num_entries);
    util_dword_to_le_buf(&binfo[36], screenshot->palette->num_entries);

    if (fwrite(binfo, sizeof(binfo), 1, screenshot->gfxoutputdrv_data->fd) < 1)
        return -1;

    bcolor = (BYTE *)lib_malloc(screenshot->palette->num_entries * 4);

    for (i = 0; i < screenshot->palette->num_entries; i++) {
        bcolor[i * 4] = screenshot->palette->entries[i].blue;
        bcolor[i * 4 + 1] = screenshot->palette->entries[i].green;
        bcolor[i * 4 + 2] = screenshot->palette->entries[i].red;
        bcolor[i * 4 + 3] = 0;
    }

    if (fwrite(bcolor, screenshot->palette->num_entries * 4, 1,
               screenshot->gfxoutputdrv_data->fd) < 1) {
        lib_free(bcolor);
        return -1;
    }

    lib_free(bcolor);
    return 0;
}

static int bmpdrv_open(screenshot_t *screenshot, const char *filename)
{
    gfxoutputdrv_data_t *sdata;

    if (screenshot->palette->num_entries > 256) {
        log_error(LOG_DEFAULT, "Max 256 colors supported.");
        return -1;
    }

    sdata = (gfxoutputdrv_data_t *)lib_malloc(sizeof(gfxoutputdrv_data_t));

    screenshot->gfxoutputdrv_data = sdata;

    if (screenshot->palette->num_entries <= 2)
        sdata->bpp = 1;
    else if (screenshot->palette->num_entries <= 16)
        sdata->bpp = 4;
    else
        sdata->bpp = 8;

    sdata->line = 0;

    sdata->ext_filename
        = util_add_extension_const(filename, bmp_drv.default_extension);

    sdata->fd = fopen(sdata->ext_filename, MODE_WRITE);

    if (sdata->fd == NULL) {
        lib_free(sdata->ext_filename);
        lib_free(sdata);
        return -1;
    }

    if (bmpdrv_write_file_header(screenshot) < 0) {
        fclose(screenshot->gfxoutputdrv_data->fd);
        lib_free(screenshot->gfxoutputdrv_data->ext_filename);
        lib_free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    if (bmpdrv_write_bitmap_info(screenshot) < 0) {
        fclose(sdata->fd);
        lib_free(sdata->ext_filename);
        lib_free(sdata);
        return -1;
    }

    sdata->data = (BYTE *)lib_malloc(screenshot->width);
    if (sdata->bpp == 1) {
        sdata->bmp_data = (BYTE *)lib_malloc(screenshot->height
                                             * screenshot->width / 8);
    }
    else if (sdata->bpp == 4) {
        sdata->bmp_data = (BYTE *)lib_malloc(screenshot->height
                                             * screenshot->width / 2);
    } else {
        sdata->bmp_data = (BYTE *)lib_malloc(screenshot->height
                                             * screenshot->width);
    }

    return 0;
}

static int bmpdrv_write(screenshot_t *screenshot)
{
    unsigned int row;
    gfxoutputdrv_data_t *sdata;

    sdata = screenshot->gfxoutputdrv_data;

    (screenshot->convert_line)(screenshot, sdata->data, sdata->line,
                               SCREENSHOT_MODE_PALETTE);

    switch (sdata->bpp) {
      case 1:
        {
          int i,j;
          memset(sdata->bmp_data + (screenshot->height - 1 - sdata->line)
                 * screenshot->width/8, 0, screenshot->width/8);

          for (i = 0; i < (int)screenshot->width/8; i++)
            {
              BYTE b=0;
              for (j = 0; j < 8; j++) b |= sdata->data[i*8+j] ? (1<<(7-j)) : 0;
              sdata->bmp_data[((screenshot->height - 1 - sdata->line)
                               * screenshot->width / 8) + i] = b;
            }
        }
        break;
      case 4:
        for (row = 0; row < screenshot->width / 2; row++) {
            sdata->bmp_data[((screenshot->height - 1 - sdata->line)
            * screenshot->width / 2) + row]
            = ((sdata->data[row * 2] & 0xf) << 4)
            | (sdata->data[row * 2 + 1] & 0xf);
        }
        break;
      case 8:
        memcpy(sdata->bmp_data + (screenshot->height - 1 - sdata->line)
               * screenshot->width, sdata->data, screenshot->width);
        break;
    }

    sdata->line++;

    return 0;
}

static int bmpdrv_close(screenshot_t *screenshot)
{
    switch (screenshot->gfxoutputdrv_data->bpp) {
      case 1:
        fwrite(screenshot->gfxoutputdrv_data->bmp_data, screenshot->height
               * screenshot->width / 8, 1,
               screenshot->gfxoutputdrv_data->fd);
        break;
      case 4:
        fwrite(screenshot->gfxoutputdrv_data->bmp_data, screenshot->height
               * screenshot->width / 2, 1,
               screenshot->gfxoutputdrv_data->fd);
        break;
      case 8:
        fwrite(screenshot->gfxoutputdrv_data->bmp_data, screenshot->height
               * screenshot->width, 1, screenshot->gfxoutputdrv_data->fd);
        break;
    }
    lib_free(screenshot->gfxoutputdrv_data->data);
    lib_free(screenshot->gfxoutputdrv_data->bmp_data);
    fclose(screenshot->gfxoutputdrv_data->fd);
    lib_free(screenshot->gfxoutputdrv_data->ext_filename);
    lib_free(screenshot->gfxoutputdrv_data);
    return 0;
}

static int bmpdrv_save(screenshot_t *screenshot, const char *filename)
{
    unsigned int i;

    if (bmpdrv_open(screenshot, filename) < 0)
        return -1;

    for (i = 0; i < screenshot->height; i++) {
        if (bmpdrv_write(screenshot) < 0) {
            fclose(screenshot->gfxoutputdrv_data->fd);
            lib_free(screenshot->gfxoutputdrv_data->ext_filename);
            lib_free(screenshot->gfxoutputdrv_data);
            return -1;
        }
    }

    if (bmpdrv_close(screenshot) < 0)
        return -1;

    return 0;
}

static gfxoutputdrv_t bmp_drv =
{
    "BMP",
    "BMP screenshot",
    "bmp",
    bmpdrv_open,
    bmpdrv_close,
    bmpdrv_write,
    bmpdrv_save,
    NULL
};

void gfxoutput_init_bmp(void)
{
    gfxoutput_register(&bmp_drv);
}

