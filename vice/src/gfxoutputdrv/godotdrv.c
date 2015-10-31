/*
 * godotdrv.c - Create a godot file.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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
#include "godotdrv.h"
#include "lib.h"
#include "log.h"
#include "gfxoutput.h"
#include "palette.h"
#include "screenshot.h"
#include "types.h"
#include "util.h"
#include "nativedrv.h"


typedef struct gfxoutputdrv_data_s {
    FILE *fd;
    char *ext_filename;
    BYTE *data;
    unsigned int line;
} gfxoutputdrv_data_t;

STATIC_PROTOTYPE gfxoutputdrv_t godot_drv;

static int godotdrv_write_file_header(screenshot_t *screenshot)
{
    FILE *fd = screenshot->gfxoutputdrv_data->fd;

    if (fprintf(fd, "GOD\x11%c%c%c%c", 0, 0, screenshot->width / 8, screenshot->height / 8) < 0) {
        return -1;
    }
    return 0;
}

static int godotdrv_open(screenshot_t *screenshot, const char *filename)
{
    gfxoutputdrv_data_t *sdata;

    sdata = lib_malloc(sizeof(gfxoutputdrv_data_t));
    screenshot->gfxoutputdrv_data = sdata;
    sdata->line = 0;
    sdata->ext_filename = util_add_extension_const(filename, godot_drv.default_extension);
    sdata->fd = fopen(sdata->ext_filename, "wb");

    if (sdata->fd == NULL) {
        lib_free(sdata->ext_filename);
        lib_free(sdata);
        return -1;
    }

    if (godotdrv_write_file_header(screenshot) < 0) {
        fclose(sdata->fd);
        lib_free(sdata->ext_filename);
        lib_free(sdata);
        return -1;
    }

    sdata->data = lib_malloc(screenshot->width * 12);

    return 0;
}

const static unsigned char godotpalette[16]={
    0, 15, 4, 12, 5, 10, 1, 13, 6, 2, 9, 3, 7, 14, 8, 11
};

static int godotdrv_write(screenshot_t *screenshot)
{
    gfxoutputdrv_data_t *sdata;

    sdata = screenshot->gfxoutputdrv_data;
    (screenshot->convert_line)(screenshot, sdata->data + ((sdata->line & 7) + 4) * screenshot->width, sdata->line, SCREENSHOT_MODE_PALETTE);

    if ((sdata->line & 7) == 7) {
        native_data_t native;
        int x, y;

        native.xsize = screenshot->width;
        native.ysize = 8;
        native.colormap = &sdata->data[4 * screenshot->width];
        if (!(strcmp(screenshot->chipid, "VICII"))) {
            /* nothing */
        } else if (!(strcmp(screenshot->chipid, "VDC"))) {
            vdc_color_to_vicii_color_colormap(&native);
        } else if (!(strcmp(screenshot->chipid, "VIC"))) {
            vic_color_to_vicii_color_colormap(&native);
        } else  if (!(strcmp(screenshot->chipid, "TED"))) {
            ted_color_to_vicii_color_colormap(&native, NATIVE_SS_TED_LUM_IGNORE);
        } else if (!(strcmp(screenshot->chipid, "CRTC"))) {
            /* nothing */
        }

        for (y = 0; y < 8; y++) {
            unsigned char *p1, *p2;
            p1 = &sdata->data[y * 4];
            p2 = &sdata->data[(y + 4) * screenshot->width];
            for (x = 0; x < (screenshot->width & ~7); x += 2) {
                p1[(x/8)*32 + ((x / 2) & 3)] = godotpalette[p2[x + 1] & 15] | (godotpalette[p2[x] & 15] << 4);
            }
        }

        if (fwrite(sdata->data, 1, screenshot->width * 4, sdata->fd) != screenshot->width * 4) {
            return -1;
        }
    }
    return 0;
}

static int godotdrv_close(screenshot_t *screenshot)
{
    gfxoutputdrv_data_t *sdata;

    sdata = screenshot->gfxoutputdrv_data;

    fclose(sdata->fd);
    lib_free(sdata->data);
    lib_free(sdata->ext_filename);
    lib_free(sdata);

    return 0;
}

static int godotdrv_save(screenshot_t *screenshot, const char *filename)
{
    if (godotdrv_open(screenshot, filename) < 0) {
        return -1;
    }

    for (screenshot->gfxoutputdrv_data->line = 0;
         screenshot->gfxoutputdrv_data->line < screenshot->height;
         (screenshot->gfxoutputdrv_data->line)++) {
        godotdrv_write(screenshot);
    }

    if (godotdrv_close(screenshot) < 0) {
        return -1;
    }

    return 0;
}

static gfxoutputdrv_t godot_drv =
{
    "4BT",
    "Godot screenshot",
    "4bt",
    NULL, /* formatlist */
    godotdrv_open,
    godotdrv_close,
    godotdrv_write,
    godotdrv_save,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
#ifdef FEATURE_CPUMEMHISTORY
    , NULL
#endif
};

void gfxoutput_init_godot(int help)
{
    if (help) {
        return;
    }
    gfxoutput_register(&godot_drv);
}
