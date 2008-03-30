/*
 * bmpdrv.c - Create a BMP file.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "log.h"
#include "gfxoutput.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"

#define BMP_HDR_OFFSET (14 + 40 + 4 * screenshot->palette->num_entries)
#define BMP_SIZE       (BMP_HDR_OFFSET + \
                       (screenshot->width / 2 * screenshot->height))

typedef struct gfxoutputdrv_data_s {
    FILE *fd;
    char *ext_filename;
    BYTE *data;
    BYTE *bmp_data;
    int line;
} gfxoutputdrv_data_t;

static gfxoutputdrv_t bmp_drv;

static int bmpdrv_write_file_header(screenshot_t *screenshot)
{
    BYTE header[14];

    memset(header, 0, sizeof(header));

    header[0] = 'B';
    header[1] = 'M';

    util_dword_to_le_buf(&header[2], BMP_SIZE);

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

    binfo[14] = 4;
    binfo[15] = 0;

    util_dword_to_le_buf(&binfo[16], 0); /* BI_RGB */
    util_dword_to_le_buf(&binfo[20], 0);

    util_dword_to_le_buf(&binfo[24], 4000);
    util_dword_to_le_buf(&binfo[28], 4000);

    util_dword_to_le_buf(&binfo[32], 0);
    util_dword_to_le_buf(&binfo[36], 0);

    if (fwrite(binfo, sizeof(binfo), 1, screenshot->gfxoutputdrv_data->fd) < 1)
        return -1;

    bcolor = (BYTE *)xmalloc(screenshot->palette->num_entries * 4);

    for (i = 0; i < screenshot->palette->num_entries; i++) {
        bcolor[i * 4] = screenshot->palette->entries[i].blue;
        bcolor[i * 4 + 1] = screenshot->palette->entries[i].green;
        bcolor[i * 4 + 2] = screenshot->palette->entries[i].red;
        bcolor[i * 4 + 3] = 0;
    }

    if (fwrite(bcolor, screenshot->palette->num_entries * 4, 1,
               screenshot->gfxoutputdrv_data->fd) < 1) {
        free(bcolor);
        return -1;
    }

    free(bcolor);
    return 0;
}

int bmpdrv_open(screenshot_t *screenshot, const char *filename)
{
    if (screenshot->palette->num_entries > 16) {
        log_error(LOG_DEFAULT, "Max 16 colors supported.");
        return -1;
    }

    screenshot->gfxoutputdrv_data
        = (gfxoutputdrv_data_t *)xmalloc(sizeof(gfxoutputdrv_data_t));

    screenshot->gfxoutputdrv_data->ext_filename
        = util_add_extension_const(filename, bmp_drv.default_extension);

    screenshot->gfxoutputdrv_data->fd
        = fopen(screenshot->gfxoutputdrv_data->ext_filename, MODE_WRITE);

    if (screenshot->gfxoutputdrv_data->fd == NULL) {
        free(screenshot->gfxoutputdrv_data->ext_filename);
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    if (bmpdrv_write_file_header(screenshot) < 0) {
        fclose(screenshot->gfxoutputdrv_data->fd);
        free(screenshot->gfxoutputdrv_data->ext_filename);
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    if (bmpdrv_write_bitmap_info(screenshot) < 0) {
        fclose(screenshot->gfxoutputdrv_data->fd);
        free(screenshot->gfxoutputdrv_data->ext_filename);
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    screenshot->gfxoutputdrv_data->data = (BYTE *)xmalloc(screenshot->width);
    screenshot->gfxoutputdrv_data->bmp_data
        = (BYTE *)xmalloc(screenshot->width / 2);

    return 0;
}

int bmpdrv_write(screenshot_t *screenshot)
{
    int row;

    (screenshot->convert_line)(screenshot, screenshot->gfxoutputdrv_data->data,
                               screenshot->gfxoutputdrv_data->line,
                               SCREENSHOT_MODE_PALETTE);
    for (row = 0; row < screenshot->width / 2; row++) {
        screenshot->gfxoutputdrv_data->bmp_data[row]
            = ((screenshot->gfxoutputdrv_data->data[row * 2] & 0xf) << 4)
            | (screenshot->gfxoutputdrv_data->data[row * 2 + 1] & 0xf);
    }

    if (fwrite(screenshot->gfxoutputdrv_data->bmp_data,
        screenshot->width / 2, 1,
        screenshot->gfxoutputdrv_data->fd) < 1) {
        free(screenshot->gfxoutputdrv_data->data);
        free(screenshot->gfxoutputdrv_data->bmp_data);
        return -1;
    }

    return 0;
}

int bmpdrv_close(screenshot_t *screenshot)
{
    free(screenshot->gfxoutputdrv_data->data);
    free(screenshot->gfxoutputdrv_data->bmp_data);
    fclose(screenshot->gfxoutputdrv_data->fd);
    free(screenshot->gfxoutputdrv_data->ext_filename);
    free(screenshot->gfxoutputdrv_data);
    return 0;
}

int bmpdrv_save(screenshot_t *screenshot, const char *filename)
{
    if (bmpdrv_open(screenshot, filename) < 0)
        return -1;


    for (screenshot->gfxoutputdrv_data->line = (int) screenshot->height - 1;
        screenshot->gfxoutputdrv_data->line >= 0;
        (screenshot->gfxoutputdrv_data->line)--) {
        if (bmpdrv_write(screenshot) < 0) {
            fclose(screenshot->gfxoutputdrv_data->fd);
            free(screenshot->gfxoutputdrv_data->ext_filename);
            free(screenshot->gfxoutputdrv_data);
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
    "bmp",
    bmpdrv_open,
    bmpdrv_close,
    bmpdrv_write,
    bmpdrv_save
};

void gfxoutput_init_bmp(void)
{
    gfxoutput_register(&bmp_drv);
}

