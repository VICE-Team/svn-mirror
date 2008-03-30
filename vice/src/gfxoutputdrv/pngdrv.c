/*
 * pngdrv.c - Create a PNG file.
 *
 * Written by
 *  Daniel Sladic <dsladic@cs.cmu.edu>
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
#include <stdlib.h>
#include <png.h>

#include "archdep.h"
#include "gfxoutput.h"
#include "lib.h"
#include "pngdrv.h"
#include "screenshot.h"
#include "types.h"
#include "util.h"


typedef struct gfxoutputdrv_data_s {
    FILE *fd;
    char *ext_filename;
    png_structp png_ptr;
    png_infop info_ptr;
    BYTE *data;
    unsigned int line;
} gfxoutputdrv_data_t;

static gfxoutputdrv_t png_drv;

static int pngdrv_open(screenshot_t *screenshot, const char *filename)
{
    gfxoutputdrv_data_t *sdata;

    sdata = (gfxoutputdrv_data_t *)lib_malloc(sizeof(gfxoutputdrv_data_t));

    screenshot->gfxoutputdrv_data = sdata;

    sdata->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                             (void *)NULL, NULL, NULL);

    if (sdata->png_ptr == NULL) {
        lib_free(sdata);
        return -1;
    }

    sdata->info_ptr = png_create_info_struct(sdata->png_ptr);

    if (sdata->info_ptr == NULL) {
        png_destroy_write_struct(&(sdata->png_ptr), (png_infopp)NULL);
        lib_free(sdata);
        return -1;
    }

    if (setjmp(screenshot->gfxoutputdrv_data->png_ptr->jmpbuf)) {
        png_destroy_write_struct(&(screenshot->gfxoutputdrv_data->png_ptr),
                                 &(screenshot->gfxoutputdrv_data->info_ptr));
        lib_free(sdata);
        return -1;
    }

    sdata->ext_filename
        = util_add_extension_const(filename, png_drv.default_extension);

    sdata->fd = fopen(sdata->ext_filename, MODE_WRITE);

    if (sdata->fd == NULL) {
        lib_free(sdata->ext_filename);
        lib_free(sdata);
        return -1;
    }

    sdata->data = (BYTE *)lib_malloc(screenshot->width * 4);

    png_init_io(sdata->png_ptr, sdata->fd);
    png_set_compression_level(sdata->png_ptr, Z_BEST_COMPRESSION);

    sdata->info_ptr->width = screenshot->width;
    sdata->info_ptr->height= screenshot->height;
    sdata->info_ptr->bit_depth = 8;
    sdata->info_ptr->color_type = PNG_COLOR_TYPE_RGB_ALPHA;

    png_write_info(sdata->png_ptr, sdata->info_ptr);

#ifdef PNG_READ_INVERT_ALPHA_SUPPORTED
    png_set_invert_alpha(sdata->png_ptr);
#endif

    return 0;
}

static int pngdrv_write(screenshot_t *screenshot)
{
    gfxoutputdrv_data_t *sdata;

    sdata = screenshot->gfxoutputdrv_data;

    (screenshot->convert_line)(screenshot, sdata->data, sdata->line,
                               SCREENSHOT_MODE_RGB32);
    png_write_row(sdata->png_ptr, (png_bytep)(sdata->data));

    return 0;
}

static int pngdrv_close(screenshot_t *screenshot)
{
    gfxoutputdrv_data_t *sdata;

    sdata = screenshot->gfxoutputdrv_data;

    png_write_end(sdata->png_ptr, sdata->info_ptr);

    png_destroy_write_struct(&(sdata->png_ptr), &(sdata->info_ptr));

    fclose(sdata->fd);
    lib_free(sdata->data);
    lib_free(sdata->ext_filename);
    lib_free(sdata);

    return 0;
}

static int pngdrv_save(screenshot_t *screenshot, const char *filename)
{
    if (pngdrv_open(screenshot, filename) < 0)
        return -1;

    for (screenshot->gfxoutputdrv_data->line = 0;
        screenshot->gfxoutputdrv_data->line < screenshot->height;
        (screenshot->gfxoutputdrv_data->line)++) {
        pngdrv_write(screenshot);
    }

    if (pngdrv_close(screenshot) < 0)
        return -1;

    return 0;
}

static gfxoutputdrv_t png_drv =
{
    "PNG",
    "PNG screenshot",
    "png",
    pngdrv_open,
    pngdrv_close,
    pngdrv_write,
    pngdrv_save,
    NULL
};

void gfxoutput_init_png(void)
{
    gfxoutput_register(&png_drv);
}

