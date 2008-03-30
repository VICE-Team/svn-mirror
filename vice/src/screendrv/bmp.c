/*
 * bmp.c - Create a BMP file.
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
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"

#define BMP_HDR_OFFSET (14 + 40 + 4 * screenshot->palette->num_entries)
#define BMP_SIZE       (BMP_HDR_OFFSET + \
                       (screenshot->width / 2 * screenshot->height))

int bmp_write_file_header(screenshot_t *screenshot, FILE *fd)
{
    BYTE header[14];

    memset(header, 0, sizeof(header));

    header[0] = 'B';
    header[1] = 'M';

    util_dword_to_le_buf(&header[2], BMP_SIZE);

    util_dword_to_le_buf(&header[10], BMP_HDR_OFFSET);

    if (fwrite(header, sizeof(header), 1, fd) < 1)
        return -1;

    return 0;
}

int bmp_write_bitmap_info(screenshot_t *screenshot, FILE *fd)
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

    if (fwrite(binfo, sizeof(binfo), 1, fd) < 1)
        return -1;

    bcolor = (BYTE *)xmalloc(screenshot->palette->num_entries * 4);

    for (i = 0; i < screenshot->palette->num_entries; i++) {
        bcolor[i * 4] = screenshot->palette->entries[i].blue;
        bcolor[i * 4 + 1] = screenshot->palette->entries[i].green;
        bcolor[i * 4 + 2] = screenshot->palette->entries[i].red;
        bcolor[i * 4 + 3] = 0;
    }

    if (fwrite(bcolor, screenshot->palette->num_entries * 4, 1, fd) < 1) {
        free(bcolor);
        return -1;
    }

    free(bcolor);
    return 0;
}

int bmp_write_bitmap_data(screenshot_t *screenshot, FILE *fd)
{
    int line, row;
    BYTE *data, *bmp_data;

    data = (BYTE *)xmalloc(screenshot->width);
    bmp_data = (BYTE *)xmalloc(screenshot->width / 2);

    for (line = screenshot->height - 1; line >= 0; line--) {
        screenshot_line_data(screenshot, data, line, SCREENSHOT_MODE_PALETTE);
        for (row = 0; row < screenshot->width / 2; row++) {
            bmp_data[row] = ((data[row * 2] & 0xf) << 4)
                            | (data[row * 2 + 1] & 0xf);
        }

        if (fwrite(bmp_data, screenshot->width / 2, 1, fd) < 1) {
            free(data);
            free(bmp_data);
            return -1;
        }
    }

    free(data);
    free(bmp_data);
    return 0;
}

int bmp_save(screenshot_t *screenshot, const char *filename)
{
    FILE *fd;

    if (screenshot->palette->num_entries > 16) {
        log_error(LOG_DEFAULT, "Max 16 colors supported.");
        return -1;
    }

    fd = fopen(filename, MODE_WRITE);

    if (fd == NULL)
        return -1;

    if (bmp_write_file_header(screenshot, fd) < 0) {
        fclose(fd);
        return -1;
    }

    if (bmp_write_bitmap_info(screenshot, fd) < 0) {
        fclose(fd);
        return -1;
    }

    if (bmp_write_bitmap_data(screenshot, fd) < 0) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}

static screendrv_t bmp_drv =
{
    "BMP",
    "bmp",
    bmp_save
};

void screenshot_init_bmp(void)
{
    screenshot_register(&bmp_drv);
}

